#ifndef SQLIB_QUERY_H
#define SQLIB_QUERY_H

#include "statement_base.h"

#include <tuple>

namespace sqlib {

template<int Low, int High>
struct for_iter {
  static_assert(Low < High, "Low < High");

  template<int C>
  struct int_c {
    enum { value = C };
  };

  template<class T>
  static void fun(T& op) {
    op(int_c<Low>());
    for_iter<Low+1, High>::fun(op);
  }
};

template<int Same>
struct for_iter<Same, Same> {
  template<class T>
  static void fun(T&) {}
};

template<int Count, class T>
void ct_for(T& op) {
  for_iter<0, Count>::fun(op);
}

template<class... Cols>
class query : public statement_base {
 public:
  typedef std::tuple<Cols...> row_type;

  query() = default;

  query(database& db, const std::string& sql)
   : statement_base(db, sql)
   , m_has_data(false) {
  }

  query(query&& rhs)
   : statement_base(std::move(rhs))
   , m_has_data(false) {
  }

  query(const query& rhs)
   : statement_base(rhs)
   , m_has_data(false) {
  }

  query& operator=(query&& rhs) {
    statement_base::operator=(std::move(rhs));
    m_has_data = false;
    return *this;
  }

  query& operator=(const query& rhs) {
    if(this != &rhs) {
      statement_base::operator=(rhs);
      m_has_data = false;
    }
    return *this;
  }

  template<class... Args>
  query& operator()(const Args&... args) {
    reset();
    bind_args(1, args...);
    this->operator++();
    return *this;
  }

  query& operator++() {
    step();
    if(m_has_data) {
      column_extractor ex(m_prepared, m_row);
      ct_for<sizeof...(Cols)>(ex);
    }
    return *this;
  }

  explicit operator bool() const {
    return m_has_data;
  }

  row_type& operator*() {
    assert(m_has_data);
    return m_row;
  }

 private:
  void step() {
    m_has_data = statement_base::step() == SQLITE_ROW;
  }

  struct column_extractor {
    column_extractor(sqlite3_stmt* prepared, row_type& row)
     : m_prepared(prepared)
     , m_row(row) {
    }

    template<class T>
    void operator()(const T&) {
      detail::extract_sql_column(m_prepared, T::value, std::get<T::value>(m_row));
    }

    sqlite3_stmt* m_prepared;
    row_type&     m_row;
  };

  row_type m_row;
  bool     m_has_data;
};

} // sqlib

#endif
