#ifndef SQLIB_RANGE_HPP
#define SQLIB_RANGE_HPP

#include "query.hpp"

#include <iterator>
#include <cassert>

namespace sqlib
{
    template<class... Cols>
    class row_iterator : public std::iterator<std::input_iterator_tag,
                                              typename query<Cols...>::row_type>
    {
      public:
        typename query<Cols...>::row_type& operator*()
        {
            assert(m_query);
            return **m_query;
        }

        typename query<Cols...>::row_type* operator->()
        {
            assert(m_query);
            return &**m_query;
        }

        row_iterator& operator++()
        {
            ++(*m_query);

            if(!*m_query)
                m_query = 0;

            return *this;
        }

        row_iterator operator++(int)
        {
            ++(*this);
            return *this;
        }

        bool operator!=(const row_iterator& rhs)
        {
            return !(*this == rhs);
        }

        bool operator==(const row_iterator& rhs)
        {
            return m_query == rhs.m_query;
        }
        
      private:

        row_iterator(query<Cols...>* qry)
          : m_query(qry)
        {
            if(m_query && !*m_query)
                m_query = 0;
        }

        template<class... Cols2>
        friend row_iterator<Cols2...> begin(query<Cols2...>& qry);

        template<class... Cols2>
        friend row_iterator<Cols2...> end(query<Cols2...>& qry);

        query<Cols...>* m_query;
    };

    template<class... Cols>
    row_iterator<Cols...> begin(query<Cols...>& qry)
    {
        return row_iterator<Cols...>(&qry);
    }

    template<class... Cols>
    row_iterator<Cols...> end(query<Cols...>& qry)
    {
        return row_iterator<Cols...>(0);
    }
}

#endif
