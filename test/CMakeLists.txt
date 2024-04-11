add_executable(
  vector_test
  vec-cow-test.cpp
)

target_link_libraries(
  vector_test
  GTest::gtest_main
  vector-cow
)

# target_include_directories(
#   vector_test
#   PUBLIC 
#   ${CMAKE_SOURCE_DIR}/src/
# )

include(GoogleTest)
gtest_discover_tests(vector_test)