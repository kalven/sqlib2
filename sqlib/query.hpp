#ifndef SQLIB_QUERY_HPP
#define SQLIB_QUERY_HPP

#include "statement_base.hpp"

#include <boost/mpl/for_each.hpp>
#include <boost/mpl/range_c.hpp>

#include <tuple>

namespace sqlib
{
    template<class... Cols>
    class query : public statement_base
    {
      public:
        typedef std::tuple<Cols...> row_type;

        query()
        {
        }

        query(database& db, const std::string& sql)
          : statement_base(db, sql)
          , m_has_data(false)
        {
        }

        query(query&& rhs)
          : statement_base(std::move(rhs))
          , m_has_data(false)
        {
        }

        query(const query& rhs)
          : statement_base(rhs)
          , m_has_data(false)
        {
        }

        query& operator=(query&& rhs)
        {
            statement_base::operator=(std::move(rhs));
            m_has_data = false;

            return *this;
        }

        query& operator=(const query& rhs)
        {
            if(this != &rhs)
            {
                statement_base::operator=(rhs);
                m_has_data = false;
            }

            return *this;
        }

        template<class... Args>
        query& operator()(const Args&... args)
        {
            reset();
            bind_args(1, args...);
            execute_query();
            return *this;
        }

        query& operator++()
        {
            if(sqlite3_step(m_prepared) == SQLITE_ROW)
            {
                m_has_data = true;
                typedef boost::mpl::range_c<int, 0, sizeof...(Cols)> range_type;
                boost::mpl::for_each<range_type>(column_extractor(m_prepared,m_row));
            }
            else
            {
                m_has_data = false;
            }

            return *this;
        }

        operator void*() const
        {
            return reinterpret_cast<void*>(m_has_data);
        }

        row_type& operator*()
        {
            assert(m_has_data);
            return m_row;
        }

      private:

        void execute_query()
        {
            this->operator++();
        }

        struct column_extractor
        {
            column_extractor(sqlite3_stmt* prepared, row_type& row)
              : m_prepared(prepared)
              , m_row(row)
            {
            }

            template<class T>
            void operator()(const T&)
            {
                detail::extract_sql_column(m_prepared, T::value, std::get<T::value>(m_row));
            }

            sqlite3_stmt* m_prepared;
            row_type&     m_row;
        };

        row_type m_row;
        bool     m_has_data;
    };
}

#endif
