#ifndef SQLIB_STATEMENT_HPP
#define SQLIB_STATEMENT_HPP

#include "statement_base.hpp"

namespace sqlib
{
    template<class... Cols>
    class statement : public statement_base
    {
      public:
        statement()
        {
        }

        statement(database& db, const std::string& sql)
          : statement_base(db, sql)
        {
        }

        statement& operator()(const Cols&... args)
        {
            assert(m_prepared);
            reset();
            bind_args(1, args...);
            execute_query();
            return *this;
        }

        int affected_rows()
        {
            return sqlite3_changes(m_db);
        }

        std::int64_t last_insert_id()
        {
            return sqlite3_last_insert_rowid(m_db);
        }
    };
}

#endif
