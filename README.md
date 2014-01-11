# sqlib2

sqlib2 is a thin C++ wrapper for the SQLite database library.

## Compatibility

sqlib2 requires:

* A somewhat recent version of SQLite 3 (tested with 3.6.16, 3.7.6.3, 3.7.13).
* A C++ compiler with reasonable C++11 support, variadic templates in particular (tested with GCC 4.4.1, 4.6).

## Usage

sqlib2 provides three basic classes for working with an sqlite3 database:

 * database - holds the sqlite3 C object and provides functionality for tracing and executing one-shot
   pieces of SQL.
 * statement - represents a prepared statement which doesn't return data (e.g. INSERT, UPDATE).
 * query - a prepared statement which returns data (e.g. SELECT).

Here is a complete example which creates a table, inserts a few rows and then reads them back:

```cpp
#include <string>
#include <iostream>

#include "sqlib/all.h"

int main()
{
    sqlib::database db(":memory:");
    db.execute_sql("CREATE TABLE tab (col1 INTEGER, col2 TEXT)");

    sqlib::statement insert(db, "INSERT INTO tab (col1, col2) VALUES(?1, ?2)");

    insert(1, "first");
    insert(2, "second");

    sqlib::query<int, std::string> qry(db, "SELECT col1,col2 FROM tab");

    for(const auto& row : qry())
    {
        std::cout << std::get<0>(row) << ", "
                  << std::get<1>(row) << std::endl;
    }
}
```

see tests/basic_test.cpp for more examples.

## License

sqlib2 is public domain.
