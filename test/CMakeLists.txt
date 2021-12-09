project(test)

file(GLOB TEST_FILE_LIST test_*.c)
foreach (TEST_FILE ${TEST_FILE_LIST})
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_FILE})
    target_link_libraries(${TEST_NAME} mulprec util)
endforeach (TEST_FILE ${TEST_FILE_LIST})
