#include "../log/logger.hpp"
#include "websocket_session.hpp"

websocket_session::websocket_session(tcp::socket&& ps_socket, tcp::socket&& tcp_socket)
        : ps_(std::move(ps_socket)), ts_(std::move(tcp_socket))
    {
    }

void websocket_session::run() 
{
    net::dispatch(ps_.get_executor(),
        beast::bind_front_handler(&websocket_session::on_ps_run,shared_from_this())
    );
}

void websocket_session::on_ps_run()
{
        ps_.set_option(
            websocket::stream_base::timeout::suggested(
                beast::role_type::server));

        ps_.set_option(websocket::stream_base::decorator(
            [](websocket::response_type& res)
            {
                res.set(http::field::server,
                    std::string(BOOST_BEAST_VERSION_STRING) +
                        " proxy-server");
            }));

        ps_.async_accept(
            beast::bind_front_handler(
                &websocket_session::on_ps_accept,
                shared_from_this()));
}

void websocket_session::on_ps_accept( const boost::system::error_code& ec)
{
    if(ec) {
        LOG_ERROR() << "ec = " << ec.message();
    }
        
    ts_.async_connect(singleton<ProxyCfg>::get_const_instance().get_access_cfg(),
        beast::bind_front_handler(&websocket_session::on_ts_connect, shared_from_this())
    );
}

void websocket_session::on_ts_connect(const boost::system::error_code& ec)
{
    if (ts_.is_open()) {
        do_ps_read();
        do_ts_read();
    }
}

void websocket_session::do_ps_read()
{
    ps_.async_read(ps_buffer_, beast::bind_front_handler(&websocket_session::on_ps_read, shared_from_this()));
}

void websocket_session::do_ts_read()
{
    ts_.async_read_some(boost::asio::buffer(ts_buffer_, MAX_LENGTH), beast::bind_front_handler(&websocket_session::on_ts_read, shared_from_this()));
}

void websocket_session::on_ps_read( const boost::system::error_code& ec, std::size_t bytes_transferred)
{
    if(ec) {
        LOG_ERROR() << "ec = " << ec.message();
        do_ts_close();
        return;
    }

    net::async_write(ts_, ps_buffer_,
        beast::bind_front_handler(&websocket_session::on_ts_write,shared_from_this())
    );    
}
void websocket_session::on_ts_read( const boost::system::error_code& ec, std::size_t bytes_transferred)
{
    if(ec) {
        LOG_ERROR() << "ec = " << ec.message();
        do_ps_close();
        return;
    }

    ps_.text(ps_.got_text());
    ps_.async_write(boost::asio::buffer(ts_buffer_, bytes_transferred),
        beast::bind_front_handler(&websocket_session::on_ps_write,shared_from_this())
    );

}
void websocket_session::on_ps_write(const boost::system::error_code& ec, std::size_t bytes_transferred)
{
    if(ec) {
        LOG_ERROR() << "ec = " << ec.message();
        return;
    }
        
    do_ts_read();
}

void websocket_session::on_ts_write(const boost::system::error_code& ec, std::size_t bytes_transferred) 
{
    boost::ignore_unused(bytes_transferred);

    if(ec) {
        LOG_ERROR() << "ec = " << ec.message();
        return;
    }
        
    ps_buffer_.consume(ps_buffer_.size());

    do_ps_read();
}

void websocket_session::do_ps_close()
{
    ps_.async_close(websocket::close_reason{websocket::close_code::normal}, [](beast::error_code ec){
        if(ec)
            LOG_ERROR() << "ec = " << ec.message();
    });  
}

void websocket_session::do_ts_close()
{
    boost::system::error_code ec;
    ts_.close(ec);
    if (ec) {
        LOG_ERROR() << "ec = " << ec.message();
        return;
    }
}