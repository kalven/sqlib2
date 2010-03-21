#include <boost/test/minimal.hpp>

#include "sqlib/database.hpp"
#include "sqlib/statement.hpp"
#include "sqlib/query.hpp"
#include "sqlib/transaction.hpp"

#include "test_util.hpp"

using namespace sqlib;
using std::string;

int test_main(int, char **)
{
    const char * const table_def = "create table table1 (col1 integer, col2 text)";
    const char * const table_def2 = "create table table1 (col1 integer, col2 blob)";

    {
        test_db db1("simple.db");

        query<int> add1(db1.db(), "select ?1+?2");
        add1(3,4);

        BOOST_CHECK(has_data(add1) == true);
        CHECK_ROW1(add1, 7);

        query<string> add2(db1.db(), "select ?1+?2");
        add2(3,4);
        BOOST_CHECK(has_data(add2) == true);
        CHECK_ROW1(add2, "7");

        db1.remove_on_close();
    }

    {
        test_db db1("select_test.db");
        db1.db().execute_sql(table_def);

        query<int,string> query1(db1.db(), "select col1,col2 from table1");
        query1();

        BOOST_CHECK(has_data(query1) == false);

        db1.remove_on_close();
    }

    {
        test_db db1("insert_select_test.db");
        db1.db().execute_sql(table_def);

        statement insert1(db1.db(), "insert into table1 (col1,col2) values(?1,?2)");

        insert1(1,"first")(2,"second")(3,"third");

        query<int,string> query1(db1.db(), "select col1,col2 from table1 order by col1 asc");
        query1();
        CHECK_ROW2(query1, 1, "first");
        CHECK_ROW2(query1, 2, "second");
        CHECK_ROW2(query1, 3, "third");
        CHECK_DONE(query1);

        db1.remove_on_close();
    }

    {
        test_db db1("insert_update_test.db");

        db1.db().execute_sql(table_def);

        statement insert(db1.db(), "insert into table1 (col1,col2) values(?1,?2)");
        insert(1,"first")(2,"second")(3,"third");

        statement update1(db1.db(), "update table1 set col2=?1 where col1=?2");
        BOOST_CHECK(update1("uno",1).affected_rows() == 1);
        BOOST_CHECK(update1("dos",2).affected_rows() == 1);
        BOOST_CHECK(update1("tres",3).affected_rows() == 1);

        query<int,string> query1(db1.db(), "select col1, col2 from table1 order by col1 asc");
        query1();
        CHECK_ROW2(query1, 1, "uno");
        CHECK_ROW2(query1, 2, "dos");
        CHECK_ROW2(query1, 3, "tres");
        CHECK_DONE(query1);

        db1.remove_on_close();
    }

    {
        test_db db1("insert_remove_test.db");

        db1.db().execute_sql(table_def);

        statement insert(db1.db(), "insert into table1 (col1,col2) values(?1,?2)");
        insert(1,"first")(2,"second")(3,"third");

        statement delete1(db1.db(), "delete from table1 where col2=?1");
        BOOST_CHECK(delete1("third").affected_rows() == 1);

        query<string> query1(db1.db(), "select col2 from table1 order by col1 asc");
        query1();

        BOOST_REQUIRE(has_data(query1));
        CHECK_ROW1(query1, "first");
        CHECK_ROW1(query1, "second");
        CHECK_DONE(query1);

        BOOST_CHECK(delete1("first").affected_rows() == 1);
        BOOST_CHECK(delete1("second").affected_rows() == 1);

        query1();
        CHECK_DONE(query1);

        db1.remove_on_close();
    }

    {
        test_db db1("transaction_test.db");
        db1.db().execute_sql(table_def);

        statement insert(db1.db(), "insert into table1 (col1,col2) values(?1,?2)");
        query<int> query1(db1.db(), "select col1 from table1 order by col1 asc");

        {
            // Start a transaction
            transaction_lock lock(db1.db());
            BOOST_CHECK(insert(1,"first").affected_rows() == 1);
            BOOST_CHECK(insert(2,"second").affected_rows() == 1);

            query1();
            CHECK_ROW1(query1, 1);
            CHECK_ROW1(query1, 2);
            CHECK_DONE(query1);

            // Leave scope without commit
        }

        query1();
        CHECK_DONE(query1);

        {
            // Start another transaction
            transaction_lock lock(db1.db());
            insert(1,"first");

            query1();
            CHECK_ROW1(query1, 1);
            CHECK_DONE(query1);

            query1();
            lock.commit();

            CHECK_ROW1(query1, 1);
            CHECK_DONE(query1);
        }

        query1();
        CHECK_ROW1(query1, 1);
        CHECK_DONE(query1);

        db1.remove_on_close();
    }

    {
        test_db db1("statement_copy_test.db");
        db1.db().execute_sql(table_def);

        typedef query<int,string> query_t;

        statement inserter(db1.db(), "insert into table1 (col1,col2) values(?1,?2)");
        query_t query1(db1.db(), "select col1,col2 from table1 order by col1 asc");

        inserter(1,"first");

        statement inserter_copy = inserter;
        inserter_copy(2,"second");

        statement inserter_assign;
        inserter_assign = inserter;
        inserter_assign(3,"third");

        query1();
        CHECK_ROW2(query1, 1, "first");
        CHECK_ROW2(query1, 2, "second");
        CHECK_ROW2(query1, 3, "third");
        CHECK_DONE(query1);

        query1();
        CHECK_ROW2(query1, 1, "first");

        query_t query_copy = query1;
        CHECK_DONE(query_copy);

        CHECK_ROW2(query1, 2, "second");

        query_copy();
        CHECK_ROW2(query_copy, 1, "first");
        CHECK_ROW2(query_copy, 2, "second");
        CHECK_ROW2(query_copy, 3, "third");
        CHECK_DONE(query_copy);

        query_t query_assignment;
        query_assignment = query1;
        CHECK_DONE(query_assignment);

        query_assignment();
        CHECK_ROW2(query_assignment, 1, "first");
        CHECK_ROW2(query_assignment, 2, "second");
        CHECK_ROW2(query_assignment, 3, "third");
        CHECK_DONE(query_assignment);

        CHECK_ROW2(query1, 3, "third");
        CHECK_DONE(query1);

        db1.remove_on_close();
    }

    {
        test_db db1("blob_test.db");
        db1.db().execute_sql(table_def2);

        typedef query<int,blob_type> query_t;

        statement inserter(db1.db(), "insert into table1 (col1,col2) values(?1,?2)");
        query_t query1(db1.db(), "select col1,col2 from table1 order by col1 asc");

        // Prepare the blob with some data
        blob_type blob, empty_blob;
        for(int i = 0; i != 256; ++i)
            blob.push_back(static_cast<char>(i));

        inserter(1, blob);
        inserter(2, empty_blob);

        query1();
        CHECK_ROW2(query1, 1, blob);
        CHECK_ROW2(query1, 2, empty_blob);
        CHECK_DONE(query1);

        db1.remove_on_close();
    }

    return 0;
}
