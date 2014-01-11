#ifndef SQLIB_TRANSACTION_HPP
#define SQLIB_TRANSACTION_HPP

#include "database.hpp"

#include <cassert>

namespace sqlib
{

class transaction_lock {
 public:
  explicit transaction_lock(database& db)
   : m_db(db)
   , m_active(true)
   , m_commit_fail(false) {
    m_db.execute_sql("BEGIN");
  }

  ~transaction_lock() {
    // TODO: reexamine the m_commit_fail logic
    if(m_active && m_commit_fail == false) {
      try {
        m_db.execute_sql("ROLLBACK");
      } catch(const sql_error&) {
        // Don't let this exception fall through.
      }
    }
  }

  transaction_lock(const transaction_lock&) = delete;
  transaction_lock& operator=(const transaction_lock&) = delete;

  void commit() {
    try {
      assert(m_active);
      m_db.execute_sql("COMMIT");
      m_active = false;
      m_commit_fail = false;
    } catch(const sql_error&) {
      m_commit_fail = true;
      throw;
    }
  }

 private:
  database& m_db;
  bool      m_active;
  bool      m_commit_fail;
};

} // sqlib

#endif
