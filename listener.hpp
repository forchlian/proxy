#ifndef __LISTENER_HPP__
#define __LISTENER_HPP__

#include "log/logger.hpp"

#include <boost/beast/core.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>

namespace   beast           = boost::beast;
namespace   net             = boost::asio;
namespace   websocket       = beast::websocket;
using       tcp             = boost::asio::ip::tcp; 

template<typename T>
class listener : public std::enable_shared_from_this<listener<T>>
{
    net::io_context& ioc_;
    tcp::acceptor acceptor_;

public:
    listener(
        net::io_context& ioc,
        tcp::endpoint endpoint)
        : ioc_(ioc)
        , acceptor_(ioc)
    {
        beast::error_code ec;

        acceptor_.open(endpoint.protocol(), ec);
        if(ec) {
            LOG_ERROR() << "ec = " << ec.message();
            return;
        }

        acceptor_.set_option(net::socket_base::reuse_address(true), ec);
        if(ec) {
            LOG_ERROR() << "ec = " << ec.message();
            return;
        }

        acceptor_.bind(endpoint, ec);
        if(ec) {
            LOG_ERROR() << "ec = " << ec.message();
            return;
        }

        acceptor_.listen(
            net::socket_base::max_listen_connections, ec);
        if(ec) {
            LOG_ERROR() << "ec = " << ec.message();
            return;
        }
    }

    void run()
    {
        do_accept();
    }

private:
    void do_accept()
    {
        acceptor_.async_accept(
            net::make_strand(ioc_),
            beast::bind_front_handler(
                &listener::on_accept,
                std::enable_shared_from_this<listener<T>>::shared_from_this()));
    }

    void on_accept(beast::error_code ec, tcp::socket web_socket)
    {
        if(ec) {
            LOG_ERROR() << "ec = " << ec.message();
        }
        else {
            tcp::socket tcp_socket = tcp::socket(ioc_);
            // Create the session and run it
            std::make_shared<T>(std::move(web_socket), std::move(tcp_socket))->run();
        }

        do_accept();
    }
};

#endif