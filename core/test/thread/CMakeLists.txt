add_executable(test-core-threads test-threads.cpp)
target_link_libraries(test-core-threads ${UMUNDOCORE_LIBRARIES})

add_test(test-core-threads test-core-threads)
