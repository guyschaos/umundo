add_executable(test-core-discovery test-discovery.cpp)
target_link_libraries(test-core-discovery ${UMUNDOCORE_LIBRARIES})
add_test(test-core-discovery test-core-discovery)

add_executable(test-core-domains test-domains.cpp)
target_link_libraries(test-core-domains ${UMUNDOCORE_LIBRARIES})
add_test(test-core-domains test-core-domains)
