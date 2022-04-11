#include "logger.hpp"

#include <fstream>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared_object.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/log/core.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/support/date_time.hpp>

BOOST_LOG_GLOBAL_LOGGER_INIT(logger, src::serverity_logger_mt<>)
{
    boost::shared_ptr<boost::log::sinks::text_file_backend> backend = boost::make_shared<boost::log::sinks::text_file_backend>(
        boost::log::keywords::file_name             = "logger_%Y-%m-%d_%H-%M-%S.%N.log",
        boost::log::keywords::rotation_size         = 52428800, // 50M
        boost::log::keywords::time_based_rotation   = boost::log::sinks::file::rotation_at_time_point(0, 0, 0)
    );
    boost::shared_ptr<boost::log::sinks::synchronous_sink<boost::log::sinks::text_file_backend>> 
        sink(new boost::log::sinks::synchronous_sink<boost::log::sinks::text_file_backend>(backend));
    sink->set_formatter(
        boost::log::expressions::format("[%1% %2%] %3%") 
            % boost::log::expressions::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S.%f")
            % boost::log::trivial::severity
            % boost::log::expressions::message
    );
    //sink->set_filter(boost::log::expressions::attr<logger_level::severity_level>("Sevrity") >= logger_level::trace);
    boost::log::core::get()->add_sink(sink);

    boost::log::add_common_attributes();
    src::severity_logger_mt<logger_level::severity_level> logger;
    return logger;
}