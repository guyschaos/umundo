add_executable(test-core-domains test-domains.cpp)
target_link_libraries(test-core-domains ${UMUNDOCORE_LIBRARIES})

add_test(test-core-domains test-core-domains)

add_executable(test-publisher-churn test-publisher-churn.cpp)
target_link_libraries(test-publisher-churn ${UMUNDOCORE_LIBRARIES})
