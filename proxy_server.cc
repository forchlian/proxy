#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include "log/logger.hpp"
#include "proxy_cfg.hpp"
#include "listener.hpp"
#include "session/tcp_session.hpp"
#include "session/websocket_session.hpp"

int main(int argc, char* argv[])
{
    auto const threads = std::max<int>(2, std::atoi(argv[1]));
    net::io_context ioc{threads};

    std::make_shared<listener<tcp_session>>(ioc, singleton<ProxyCfg>::get_const_instance().get_net_cfg())->run();
    std::make_shared<listener<websocket_session>>(ioc, singleton<ProxyCfg>::get_const_instance().get_websocket_cfg())->run();
    std::vector<std::thread> v;
    v.reserve(threads - 1);
    for(auto i = threads - 1; i > 0; --i)
        v.emplace_back(
        [&ioc]
        {
            ioc.run();
        });
    ioc.run();

    return EXIT_SUCCESS;
} 