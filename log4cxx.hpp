/*
    log4cxx is C++ Log manager like log4js
    (C) 2016 SiLeader.
*/

// Version 1.0.0-release
#pragma once

#include <string>
#include <fstream>
#include <cctype>
#include <chrono>
#include <unordered_map>

#include "picojson.hpp"

namespace log4cxx {
    namespace detail {
        enum class Level {
            Fatal, Error, Warn, Info, Debug, Trace, Off,
        };
        inline Level convert_string_to_level(const std::string& ls) {
            std::string lev;
            std::transform(ls.begin(), ls.end(), lev.begin(), ::tolower);
            if(lev=="off") {
                return Level::Off;
            }else if(lev=="trace") {
                return Level::Trace;
            }else if(lev=="debug") {
                return Level::Debug;
            }else if(lev=="info") {
                return Level::Info;
            }else if(lev=="warn") {
                return Level::Warn;
            }else if(lev=="error") {
                return Level::Error;
            }else if(lev=="fatal") {
                return Level::Fatal;
            }else{
                return Level::Trace;
            }
        }
        namespace time {
            struct Time {
                unsigned int year, month, day, hour, minute, second, nanosec;
            };
            inline std::string nanoduration_to_time(/*std::chrono::nanoseconds nsec*/std::chrono::high_resolution_clock::time_point& tp) {
                //Time tt;
                auto nsec=std::chrono::duration_cast<std::chrono::nanoseconds>(tp.time_since_epoch());

                auto sec=std::chrono::duration_cast<std::chrono::seconds>(nsec);
                auto ns=std::chrono::duration_cast<std::chrono::nanoseconds>(nsec-sec).count();

                auto tim=std::chrono::system_clock::to_time_t(tp);
                struct tm ttm, *ptm;
                ptm=gmtime_r(&tim, &ttm);

                char buf[256]={0};
                strftime(buf, 256, "%Y:%m:%d %H:%M:%S.", ptm);
                std::string str=buf;
                str+=std::to_string(ns);
                //return tt;
                return str;
            }
        } /* time */
    } /* detail */

    class logger {
    private:
        std::string m_path, m_category;
        bool m_syslog, m_console;
        detail::Level m_level;

        std::ofstream m_fout;

        void m_write(const std::string& msg) {
            auto now=std::chrono::high_resolution_clock::now();
            auto ttstr=detail::time::nanoduration_to_time(now);
            m_fout<<"["<<ttstr<<"] "<<msg<<std::endl;
        }
    public:
        explicit logger()=default;
        explicit logger(const logger&)=default;
        explicit logger(logger&)=default;
        logger(const std::string& path, const std::string& category, bool syslog, bool console, detail::Level level) noexcept
            : m_path(path), m_category(category), m_syslog(syslog), m_console(console), m_level(level), m_fout(m_path, std::ios::app) {}

        logger& operator=(const logger&)=default;
        logger& operator=(logger&&)=default;

        void trace(const std::string& msg)noexcept {
            m_write(std::string("[trace] ")+m_category+" - "+msg);
        }

        void debug(const std::string& msg)noexcept {
            m_write(std::string("[debug] ")+m_category+" - "+msg);
        }

        void info(const std::string& msg)noexcept {
            m_write(std::string("[info] ")+m_category+" - "+msg);
        }

        void warn(const std::string& msg)noexcept {
            m_write(std::string("[warn] ")+m_category+" - "+msg);
        }

        void error(const std::string& msg)noexcept {
            m_write(std::string("[error] ")+m_category+" - "+msg);
        }

        void fatal(const std::string& msg)noexcept {
            m_write(std::string("[fatal] ")+m_category+" - "+msg);
        }
    };

    class log4cxx {
    private:
        struct Setting {
            std::string path, category;
            bool write_to_syslog, write_to_console;
            detail::Level level;
            logger log;
        };
        std::string m_setting_file;
        std::unordered_map<std::string, Setting> m_setting;

        void m_load()noexcept {
            std::ifstream fin(m_setting_file);
            picojson::value val;
            fin>>val;
            fin.close();
            {
                auto&& appenders=val.get<picojson::object>()["appenders"].get<picojson::array>();
                Setting set;
                for(auto&& a : appenders) {
                    set.category=a.get<picojson::object>()["category"].get<std::string>();
                    set.path=a.get<picojson::object>()["filename"].get<std::string>();

                    if(a.contains("syslog")) {
                        set.write_to_syslog=a.get<picojson::object>()["syslog"].get<bool>();
                    }else{
                        set.write_to_syslog=false;
                    }

                    if(a.contains("console")) {
                        set.write_to_console=a.get<picojson::object>()["console"].get<bool>();
                    }else{
                        set.write_to_console=true;
                    }

                    if(a.contains("level")) {
                        set.level=detail::convert_string_to_level(a.get<picojson::object>()["level"].get<std::string>());
                    }else{
                        set.level=detail::Level::Trace;
                    }
                    set.log=logger(set.path, set.category, set.write_to_syslog, set.write_to_console, set.level);
                    m_setting[set.category]=std::move(set);
                }
            }
        }
    public:
        log4cxx(const std::string& setting_file="log_config.json")noexcept
            : m_setting_file(setting_file)
        {
            m_load();
        }

        void reload()noexcept {
            m_load();
        }

        logger& get_logger(const std::string& category)noexcept {
            return m_setting[category].log;
        }
    };
} /* log4cxx */
