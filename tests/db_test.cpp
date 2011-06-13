#include <boost/test/minimal.hpp>

#include <iostream>

#include "sqlib/all.hpp"
#include "test_util.hpp"

using namespace sqlib;
using std::string;

int test_main(int, char**)
{
    {
        database db1(":memory:");

        db1.execute_sql("create table tags (tag text)");
        db1.execute_sql("create table node_tags (node integer, tag_id integer)");
        db1.execute_sql("create table keys (key text)");
        db1.execute_sql("create table node_meta (node integer, key_id integer, value text)");
        db1.execute_sql("create table links (from_node integer, to_node integer)");

        statement add_tag(db1, "insert into tags values(?1)");
        statement add_key(db1, "insert into keys values(?1)");
        statement add_node_tag(db1, "insert into node_tags values(?1,?2)");
        statement add_node_meta(db1, "insert into node_meta values(?1,?2,?3)");
        statement add_link(db1, "insert into links values(?1,?2)");

        query<string,string> get_node_meta(db1, "select key,value from keys,node_meta where node_meta.node = ?1 and node_meta.key_id = keys.rowid order by key");
        query<string>        get_tags_by_node(db1, "select tag from tags,node_tags where node_tags.node = ?1 and node_tags.tag_id = tags.rowid order by tag");
        query<int>           get_nodes_by_tag(db1, "select node from tags,node_tags where tag = ?1 and node_tags.tag_id = tags.rowid order by node");
        query<int>           get_outgoing_links(db1, "select to_node from links where from_node = ?1 order by to_node");
        query<int>           get_incoming_links(db1, "select from_node from links where to_node = ?1 order by from_node");

        add_key("name")("content-type");

        add_node_meta(1,1,"Bug");
        add_node_meta(1,2,"image/jpeg");
        add_node_meta(2,1,"Midsummer");
        add_node_meta(2,2,"image/jpeg");
        add_node_meta(3,1,"Scratch Pad");
        add_node_meta(3,2,"text/plain");

        get_node_meta(1);
        CHECK_ROW2(get_node_meta, "content-type", "image/jpeg");
        CHECK_ROW2(get_node_meta, "name", "Bug");
        CHECK_DONE(get_node_meta);

        get_node_meta(3);
        CHECK_ROW2(get_node_meta, "content-type", "text/plain");
        CHECK_ROW2(get_node_meta, "name", "Scratch Pad");
        CHECK_DONE(get_node_meta);

        add_tag("nature")("macro")("people")("code");

        add_node_tag(1,1);
        add_node_tag(1,2); // Bug has tags [nature,macro]
        add_node_tag(2,1);
        add_node_tag(2,3); // Midsummer has tags [nature,people]
        add_node_tag(3,4); // Scratch Pad has tags [code]

        get_tags_by_node(1);
        CHECK_ROW1(get_tags_by_node, "macro");
        CHECK_ROW1(get_tags_by_node, "nature");
        CHECK_DONE(get_tags_by_node);

        get_tags_by_node(3);
        CHECK_ROW1(get_tags_by_node, "code");
        CHECK_DONE(get_tags_by_node);

        get_nodes_by_tag("nature");
        CHECK_ROW1(get_nodes_by_tag, 1);
        CHECK_ROW1(get_nodes_by_tag, 2);
        CHECK_DONE(get_nodes_by_tag);

        add_link(3,1);
        add_link(3,2); // Node 3 links to 1 & 2
        add_link(2,3); // Node 2 links to 3

        get_outgoing_links(3);
        CHECK_ROW1(get_outgoing_links, 1);
        CHECK_ROW1(get_outgoing_links, 2);
        CHECK_DONE(get_outgoing_links);

        get_incoming_links(2);
        CHECK_ROW1(get_incoming_links, 3);
        CHECK_DONE(get_incoming_links);
    }

    {
        database db1(":memory:");

        db1.execute_sql("create table nodes (node integer primary key)");
        statement create_node(db1, "insert into nodes values(?1)");

        create_node(1);

        bool caught_exception = false;
        try
        {
            create_node(1);
        }
        catch(const execute_error &)
        {
            caught_exception = true;
        }
        BOOST_CHECK(caught_exception == true);
    }

    {
        database db(":memory:");

        db.execute_sql("create table nodes (node integer primary key)");
        statement create(db, "insert into nodes values(?1)");

        std::int64_t id1 = create(null).last_insert_id();
        std::int64_t id2 = create(null).last_insert_id();

        BOOST_CHECK(id1 != id2);
    }

    return 0;
}
