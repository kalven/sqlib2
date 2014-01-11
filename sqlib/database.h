#ifndef SQLIB_DATABASE_H
#define SQLIB_DATABASE_H

#include <string>
#include <iostream>

#include "error.h"

#include <sqlite3.h>

namespace sqlib {

class database {
 public:
  database(const char* filename) {
    sqlite3_open(filename, &m_sqlite);
  }

  database(database&& rhs)
   : m_sqlite(rhs.m_sqlite) {
    rhs.m_sqlite = 0;
  }

  ~database() {
    if(m_sqlite)
      sqlite3_close(m_sqlite);
  }

  database(const database&) = delete;
  database& operator=(const database&) = delete;

  database& operator=(database&& rhs) {
    std::swap(m_sqlite, rhs.m_sqlite);
    return *this;
  }

  sqlite3* get() {
    return m_sqlite;
  }

  void execute_sql(const std::string& sql) {
    char* errmsg;

    if(sqlite3_exec(m_sqlite, sql.c_str(), 0, 0, &errmsg) != SQLITE_OK) {
      sql_error ex(errmsg);
      sqlite3_free(errmsg);
      throw ex;
    }
  }

  void enable_trace(std::ostream& os) {
    sqlite3_trace(m_sqlite, &database::trace_fn, &os);
  }

  void disable_trace() {
    sqlite3_trace(m_sqlite, 0, 0);
  }

 private:
  static void trace_fn(void* s, const char* msg) {
    (*static_cast<std::ostream*>(s)) << msg << '\n';
  }

  sqlite3* m_sqlite;
};

} // sqlib

#endif
