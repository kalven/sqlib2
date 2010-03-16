#ifndef SQLIB_TEST_UTIL_HPP
#define SQLIB_TEST_UTIL_HPP

#include <boost/noncopyable.hpp>

#include <algorithm>

#include "sqlib/database.hpp"

template<class Query>
bool has_data(const Query & q)
{
    if(q)
        return true;
    else
        return false;
}

class test_db : public boost::noncopyable
{
  public:
    test_db(const char * db_name) :
      m_db_name(db_name),
      m_remove_on_close(false)
    {
        remove(m_db_name);
        m_db = new sqlib::database(m_db_name);
    }

    ~test_db()
    {
        delete m_db;
        if(m_remove_on_close)
            remove(m_db_name);
    }

    sqlib::database & db()
    {
        return *m_db;
    }

    void remove_on_close()
    {
        m_remove_on_close = true;
    }

  private:
    const char *      m_db_name;
    sqlib::database * m_db;
    bool              m_remove_on_close;
};

bool operator==(const sqlib::blob_type & lhs, const sqlib::blob_type & rhs)
{
    return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

#define CHECK_ROW1(query, col1)                 \
    BOOST_REQUIRE(has_data(query));             \
    BOOST_CHECK(std::get<0>(*query) == col1);   \
    ++query;

#define CHECK_ROW2(query, col1, col2)           \
    BOOST_REQUIRE(has_data(query));             \
    BOOST_CHECK(std::get<0>(*query) == col1);   \
    BOOST_CHECK(std::get<1>(*query) == col2);   \
    ++query;

#define CHECK_DONE(query)                       \
    BOOST_REQUIRE(!has_data(query));

#endif
