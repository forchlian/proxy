#ifndef __LOGGER_HPP__
#define __LOGGER_HPP__

#define BOOST_BIND_GLOBAL_PLACEHOLDERS

#include <boost/log/trivial.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/attributes/mutable_constant.hpp>
#include <boost/log/attributes/constant.hpp>

namespace logger_level  = boost::log::trivial;
namespace src           = boost::log::sources;

BOOST_LOG_GLOBAL_LOGGER(logger, src::severity_logger_mt<logger_level::severity_level>)

#define LOG_DATA(LEVEL)   \
    BOOST_LOG_SEV(logger::get(), LEVEL) << "[" << __FILE__ << ":" << __LINE__ << "(" << __FUNCTION__ << ")] "

#define LOG_TRACE()     LOG_DATA(logger_level::trace)
#define LOG_INFO()      LOG_DATA(logger_level::info)
#define LOG_DEBUG()     LOG_DATA(logger_level::debug)
#define LOG_WARNING()   LOG_DATA(logger_level::warning)
#define LOG_ERROR()     LOG_DATA(logger_level::error)
#define LOG_FATAL()     LOG_DATA(logger_level::fatal)

#endif