#ifndef __PROXY_CFG_HPP__
#define __PROXY_CFG_HPP__

#include <boost/asio.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/serialization/singleton.hpp>

template<typename T>
using       singleton       = boost::serialization::singleton<T>;

namespace   ip              = boost::asio::ip;
namespace   property_tree   = boost::property_tree;

class ProxyCfg {
public:
    explicit ProxyCfg()
    {
        std::fstream fs;
        fs.open("./config/proxy_cfg.json");
        if (fs.is_open()) {
            property_tree::ptree pt;
            property_tree::read_json(fs, pt);
            for (auto iter = pt.begin(); iter != pt.end(); ++iter) {
                //std::cout << iter->first << std::endl;
                //std::cout << iter->second.get<std::string>("host") << std::endl;
                //std::cout << iter->second.get<std::string>("port") << std::endl; 
                ip::tcp::endpoint endpoint_cfg{ip::make_address(iter->second.get<std::string>("host")),
                        static_cast<unsigned short>(std::stoi(iter->second.get<std::string>("port")))};
                cfg_[iter->first] = endpoint_cfg;
            }
        }
    }

    const ip::tcp::endpoint& get_access_cfg() const
    {
        return cfg_.at("accessServer");
    }

    const ip::tcp::endpoint& get_net_cfg() const
    {
        return cfg_.at("netListen");
    }

    const ip::tcp::endpoint& get_websocket_cfg() const
    {
        return cfg_.at("webListen");
    }

private:
    std::map<std::string, ip::tcp::endpoint> cfg_;
};

#endif
