#ifndef __WEBSOCKET_SESSION_HPP__
#define __WEBSOCKET_SESSION_HPP__

#include "../proxy_cfg.hpp"

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>

namespace   beast           = boost::beast;
namespace   websocket       = beast::websocket;
namespace   http            = beast::http;
namespace   net             = boost::asio; 

using       tcp             = boost::asio::ip::tcp;

class websocket_session: public std::enable_shared_from_this<websocket_session> {
public:
    explicit websocket_session(tcp::socket&& ps_socket, tcp::socket&& tcp_socket);
    void run();
    void on_ps_run();
    void on_ps_accept( const boost::system::error_code& ec);
    void on_ts_connect(const boost::system::error_code& ec);
    void do_ps_read();
    void do_ts_read();
    void on_ps_read( const boost::system::error_code& ec, std::size_t bytes_transferred);
    void on_ts_read( const boost::system::error_code& ec, std::size_t bytes_transferred);
    void on_ps_write(const boost::system::error_code& ec, std::size_t bytes_transferred);
    void on_ts_write(const boost::system::error_code& ec, std::size_t bytes_transferred);
    void do_ps_close();
    void do_ts_close();
private:
    websocket::stream<beast::tcp_stream>    ps_;
    tcp::socket                             ts_;
    tcp::resolver::results_type             endpoints_;
    enum {MAX_LENGTH = 1024};
    beast::flat_buffer                      ps_buffer_;
    char                                    ts_buffer_[MAX_LENGTH];
};

#endif