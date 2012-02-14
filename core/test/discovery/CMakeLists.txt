add_executable(test-core-api test-api.cpp)
target_link_libraries(test-core-api ${UMUNDOCORE_LIBRARIES})

add_executable(test-core-domains test-domains.cpp)
target_link_libraries(test-core-domains ${UMUNDOCORE_LIBRARIES})

#target_link_libraries(test-api debug ${UMUNDO_LIBRARIES_DEBUG})

# add_executable(test-publisher-churn test-publisher-churn.cpp)
# target_link_libraries(test-publisher-churn ${UMUNDO_LIBRARIES})
# target_link_libraries(test-publisher-churn debug ${UMUNDO_LIBRARIES_DEBUG})
