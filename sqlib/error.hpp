#ifndef SQLIB_ERROR_HPP
#define SQLIB_ERROR_HPP

#include <stdexcept>

namespace sqlib
{
    class sql_error : public std::logic_error
    {
      public:
        sql_error()
          : std::logic_error("sql_error")
        {
        }

        sql_error(const std::string & msg)
          : std::logic_error("sql_error: " + msg)
        {
        }
    };

    class prepare_error : public sql_error
    {
      public:
        prepare_error(const std::string & sql)
          : sql_error("prepare failed with \"" + sql + "\"")
        {
        }
    };

    class execute_error : public sql_error
    {
      public:
        execute_error(const std::string & msg)
          : sql_error("execute_query failed: \"" + msg + "\"")
        {
        }
    };

    class busy_error : public sql_error
    {
      public:
        busy_error()
          : sql_error("busy")
        {
        }
    };

    class misuse_error : public sql_error
    {
      public:
        misuse_error()
          : sql_error("misuse")
        {
        }
    };
}

#endif
