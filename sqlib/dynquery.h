#ifndef SQLIB_DYNQUERY_H
#define SQLIB_DYNQUERY_H

#include "statement_base.h"

namespace sqlib {

class dynrow {
 public:
  explicit dynrow(sqlite3_stmt* prepared)
   : m_prepared(prepared)
   , m_next_column(0) {
  }

  template<class H, class... T>
  void get(H& head, T&... tail) {
    detail::extract_sql_column(m_prepared, m_next_column++, head);
    get(tail...);
  }

  void get() {}

 private:
  sqlite3_stmt* m_prepared;
  int m_next_column;
};

class dynquery : public statement_base {
 public:
  dynquery() = default;

  dynquery(database& db, const std::string& sql)
   : statement_base(db, sql)
   , m_has_data(false) {
  }

  dynquery(dynquery&& rhs)
   : statement_base(std::move(rhs))
   , m_has_data(false) {
  }

  dynquery& operator=(const dynquery& rhs) {
    if(this != &rhs) {
      statement_base::operator=(rhs);
      m_has_data = false;
    }
    return *this;
  }

  dynquery& operator=(const dynquery&& rhs) {
    statement_base::operator=(std::move(rhs));
    m_has_data = false;
    return *this;
  }

  template<class... Args>
  dynquery& operator()(const Args&... args) {
    reset();
    bind_args(1, args...);
    this->operator++();
    return *this;
  }

  dynquery& operator++() {
    step();
    return *this;
  }

  explicit operator bool() const {
    return m_has_data;
  }

  dynrow operator*() {
    assert(m_has_data);
    return dynrow(m_prepared);
  }

 private:
  void step() {
    m_has_data = statement_base::step() == SQLITE_ROW;
  }

  bool m_has_data;
};

} // sqlib

#endif
