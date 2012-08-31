#ifndef SQLIB_TEST_UTIL_HPP
#define SQLIB_TEST_UTIL_HPP

#include <algorithm>
#include <cstdlib>

#include "sqlib/database.hpp"

template<class Query>
bool has_data(const Query & q)
{
    if(q)
        return true;
    else
        return false;
}

bool operator==(const sqlib::blob_type & lhs, const sqlib::blob_type & rhs)
{
    return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

#define SQLIB_CHECK(x)                                                  \
    { if(!(x)) { std::cerr << "Check failed on "                        \
                           << __FILE__ << ':' << __LINE__ << "\n   "    \
                           << #x << "\n"; } }

#define SQLIB_REQUIRE(x)                                                \
    { if(!(x)) { std::cerr << "Check failed on "                        \
                           << __FILE__ << ':' << __LINE__ << " (fatal)\n   " \
                           << #x << "\n"; std::exit(1); } }

#define CHECK_ROW1(query, col1)                 \
    SQLIB_REQUIRE(has_data(query));             \
    SQLIB_CHECK(std::get<0>(*query) == col1);   \
    ++query;

#define CHECK_ROW2(query, col1, col2)           \
    SQLIB_REQUIRE(has_data(query));             \
    SQLIB_CHECK(std::get<0>(*query) == col1);   \
    SQLIB_CHECK(std::get<1>(*query) == col2);   \
    ++query;

#define CHECK_DONE(query)                       \
    SQLIB_REQUIRE(!has_data(query));

#endif
