#ifndef SQLIB_STATEMENT_BASE_HPP
#define SQLIB_STATEMENT_BASE_HPP

#include "database.hpp"
#include "error.hpp"
#include "null.hpp"

#include <string>
#include <vector>
#include <cstdint>
#include <cassert>
#include <algorithm>

#include <sqlite3.h>

namespace sqlib
{
    typedef std::vector<char> blob_type;

    namespace detail
    {
        inline void extract_sql_column(sqlite3_stmt* stmt, int col, std::string& dest)
        {
            dest = reinterpret_cast<const char*>(sqlite3_column_text(stmt, col));
        }

        inline void extract_sql_column(sqlite3_stmt* stmt, int col, int& dest)
        {
            dest = sqlite3_column_int(stmt, col);
        }

        inline void extract_sql_column(sqlite3_stmt* stmt, int col, std::int64_t& dest)
        {
            dest = sqlite3_column_int64(stmt, col);
        }

        inline void extract_sql_column(sqlite3_stmt* stmt, int col, double& dest)
        {
            dest = sqlite3_column_double(stmt, col);
        }

        inline void extract_sql_column(sqlite3_stmt* stmt, int col, blob_type& dest)
        {
            int bytes = sqlite3_column_bytes(stmt, col);
            const char* data = reinterpret_cast<const char*>(sqlite3_column_blob(stmt, col));

            dest.resize(bytes);
            if(bytes > 0)
                std::copy(data, data+bytes, dest.begin());
        }

        inline void bind_arg(sqlite3_stmt* stmt, int slot, const std::string& arg)
        {
            sqlite3_bind_text(stmt, slot, arg.c_str(), arg.size(), SQLITE_TRANSIENT);
        }

        inline void bind_arg(sqlite3_stmt* stmt, int slot, const int& arg)
        {
            sqlite3_bind_int(stmt, slot, arg);
        }

        inline void bind_arg(sqlite3_stmt* stmt, int slot, const std::int64_t& arg)
        {
            sqlite3_bind_int64(stmt, slot, arg);
        }

        inline void bind_arg(sqlite3_stmt* stmt, int slot, const double& arg)
        {
            sqlite3_bind_double(stmt, slot, arg);
        }

        inline void bind_arg(sqlite3_stmt* stmt, int slot, const blob_type& arg)
        {
            sqlite3_bind_blob(stmt, slot, arg.size() == 0 ? "" : &arg[0], arg.size(), SQLITE_STATIC);
        }

        inline void bind_arg(sqlite3_stmt* stmt, int slot, const null_type& arg)
        {
            sqlite3_bind_null(stmt, slot);
        }
    }

    class statement_base
    {
      public:
        statement_base()
          : m_prepared(0)
          , m_db(0)
        {
        }

        statement_base(statement_base&& rhs)
          : m_prepared(rhs.m_prepared)
          , m_db(rhs.m_db)
          , m_sql(std::move(rhs.m_sql))
        {
            rhs.m_prepared = 0;
            rhs.m_db = 0;
        }

        statement_base(const statement_base& rhs)
          : m_prepared(0)
          , m_db(rhs.m_db)
          , m_sql(rhs.m_sql)
        {
            if(rhs.m_prepared) // The rhs statement had a valid prepared statement, let's duplicate it.
                prepare();
        }

        statement_base(database& db, const std::string& sql)
          : m_prepared(0)
          , m_db(db.get())
          , m_sql(sql)
        {
            prepare();
        }

        void prepare(database& db, const std::string& sql)
        {
            statement_base tmp(db, sql);
            swap(tmp);
        }

        ~statement_base()
        {
            if(m_prepared)
                sqlite3_finalize(m_prepared);
        }

        statement_base& operator=(statement_base&& rhs)
        {
            swap(rhs);

            return *this;
        }

        statement_base& operator=(const statement_base& rhs)
        {
            if(this != &rhs)
            {
                statement_base tmp(rhs);
                swap(tmp);
            }

            return *this;
        }

        void swap(statement_base& rhs)
        {
            std::swap(m_sql, rhs.m_sql);
            std::swap(m_db, rhs.m_db);
            std::swap(m_prepared, rhs.m_prepared);
        }

        void reset()
        {
            if(m_prepared)
                sqlite3_reset(m_prepared);
        }

      protected:

        void bind_args(int)
        {
        }

        template<class A1, class... Rest>
        void bind_args(int n, const A1& arg, const Rest&... rest)
        {
            detail::bind_arg(m_prepared, n, arg);
            bind_args(n+1, rest...);
        }

        void prepare()
        {
            assert(m_prepared == 0);
            int res = sqlite3_prepare(m_db, m_sql.c_str(), -1, &m_prepared, 0);
            if(res != SQLITE_OK)
                throw prepare_error(m_sql);
        }

        void execute_query()
        {
            int res = sqlite3_step(m_prepared);
            switch(res)
            {
                case SQLITE_OK:
                case SQLITE_ROW:
                case SQLITE_DONE:
                    break;
                case SQLITE_ERROR:
                    throw execute_error(sqlite3_errmsg(m_db));
                case SQLITE_BUSY:
                    throw busy_error();
                case SQLITE_MISUSE:
                    throw misuse_error();
                default:
                    throw sql_error(sqlite3_errmsg(m_db));
            }
        }

        sqlite3_stmt* m_prepared;
        sqlite3*      m_db;
        std::string   m_sql;
    };
}

#endif
