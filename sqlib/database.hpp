#ifndef SQLIB_DATABASE_HPP
#define SQLIB_DATABASE_HPP

#include <string>

#include "sqlib.hpp"
#include "error.hpp"
#include "tracing.hpp"

#include <sqlite3.h>

namespace sqlib
{
    class database
    {
        SQLIB_NOCOPY(database);

      public:
        database(const char* filename)
        {
            sqlite3_open(filename, &m_sqlite);
        }

        ~database()
        {
            sqlite3_close(m_sqlite);
        }

        sqlite3* get()
        {
            return m_sqlite;
        }

        void execute_sql(const std::string& sql)
        {
            SQLIB_TRACE("execute_sql(" << sql << ")");
            char* errmsg;

            if(sqlite3_exec(m_sqlite, sql.c_str(), 0, 0, &errmsg) != SQLITE_OK)
            {
                sql_error ex(errmsg);
                sqlite3_free(errmsg);
                throw ex;
            }
        }

      private:

        sqlite3 * m_sqlite;
    };
}

#endif
