enable_testing()

add_executable(basic_test basic_test.cpp)
add_executable(db_test db_test.cpp)

target_link_libraries(basic_test ${SQLITE_LIBRARIES})
target_link_libraries(db_test ${SQLITE_LIBRARIES})

add_test(basic_test basic_test)
add_test(db_test db_test)
