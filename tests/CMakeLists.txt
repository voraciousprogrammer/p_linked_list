INCLUDE_DIRECTORIES(${gtest_SOURCE_DIR}
                    ${gtest_SOURCE_DIR}/include
                    ${PROJECT_SOURCE_DIR}/src)

ADD_EXECUTABLE(test_p_linked_list
               p_linked_list_test_main.cc
               p_linked_list_unittest.cc)

TARGET_LINK_LIBRARIES(test_p_linked_list gtest p_linked_list)

ADD_TEST(NAME unittest_p_linked_list
         COMMAND ${PROJECT_BINARY_DIR}/bin/test_p_linked_list)
