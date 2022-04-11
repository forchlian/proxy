#include "../log/logger.hpp"
#include "tcp_session.hpp"

tcp_session::tcp_session(tcp::socket&& ps_socket, tcp::socket&& tcp_socket)
        : ps_(std::move(ps_socket)), ts_(std::move(tcp_socket))
    {
    }

void tcp_session::run() 
{
    on_ps_run();
}

void tcp_session::on_ps_run()
{
    on_ps_accept();
}

void tcp_session::on_ps_accept()
{
    ts_.async_connect(singleton<ProxyCfg>::get_const_instance().get_access_cfg(),
        beast::bind_front_handler(&tcp_session::on_ts_connect, shared_from_this())
    );
}

void tcp_session::on_ts_connect(const boost::system::error_code& ec)
{
    if (ts_.is_open()) {
        do_ps_read();
        do_ts_read();
    }
}

void tcp_session::do_ps_read()
{
    ps_.async_read_some(boost::asio::buffer(ps_buffer_, MAX_LENGTH), beast::bind_front_handler(&tcp_session::on_ts_read, shared_from_this()));
}

void tcp_session::do_ts_read()
{
    ps_.async_read_some(boost::asio::buffer(ts_buffer_, MAX_LENGTH), beast::bind_front_handler(&tcp_session::on_ps_read, shared_from_this()));    
}

void tcp_session::on_ps_read( const boost::system::error_code& ec, std::size_t bytes_transferred)
{
    if(ec) {
        LOG_ERROR() << "ec = " << ec.message();
        do_ts_close();
        return;
    }

    net::async_write(ts_, boost::asio::buffer(ps_buffer_, bytes_transferred),
        beast::bind_front_handler(&tcp_session::on_ts_write,shared_from_this())
    );
}

void tcp_session::on_ts_read( const boost::system::error_code& ec, std::size_t bytes_transferred)
{
    if(ec) {
        LOG_ERROR() << "ec = " << ec.message();
        do_ps_close();
        return;
    }

    net::async_write(ps_, boost::asio::buffer(ps_buffer_, bytes_transferred),
        beast::bind_front_handler(&tcp_session::on_ps_write,shared_from_this())
    );
}

void tcp_session::on_ps_write(const boost::system::error_code& ec, std::size_t bytes_transferred)
{
    if(ec) {
        LOG_ERROR() << "ec = " << ec.message();
        return;
    }
        
    do_ts_read();
}

void tcp_session::on_ts_write(const boost::system::error_code& ec, std::size_t bytes_transferred)
{
    if(ec) {
        LOG_ERROR() << "ec = " << ec.message();
        return;
    }
        
    do_ps_read();
}

void tcp_session::do_ps_close()
{
    boost::system::error_code ec;
    ts_.close(ec);
    if (ec) {
        LOG_ERROR() << "ec = " << ec.message();
        return;
    }
}

void tcp_session::do_ts_close()
{
    boost::system::error_code ec;
    ts_.close(ec);
    if (ec) {
        LOG_ERROR() << "ec = " << ec.message();
        return;
    }
}