#ifndef SQLIB_STATEMENT_HPP
#define SQLIB_STATEMENT_HPP

#include "statement_base.hpp"

namespace sqlib
{

class statement : public statement_base {
 public:
  statement() = default;

  statement(database& db, const std::string& sql)
   : statement_base(db, sql) {
  }

  template<class... Args>
  statement& operator()(const Args&... args) {
    assert(m_prepared);
    reset();
    bind_args(1, args...);
    step();
    return *this;
  }

  int affected_rows() {
    return sqlite3_changes(m_db);
  }

  std::int64_t last_insert_id() {
    return sqlite3_last_insert_rowid(m_db);
  }
};

} // sqlib

#endif
