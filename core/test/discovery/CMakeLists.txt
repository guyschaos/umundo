# add_executable(test-discovery test-discovery.cpp)
# target_link_libraries(test-discovery ${UMUNDO_LIBRARIES})
# target_link_libraries(test-discovery debug ${UMUNDO_LIBRARIES_DEBUG})

add_executable(test-api test-api.cpp)
target_link_libraries(test-api 
	${UMUNDO_LIBRARIES}
)


#target_link_libraries(test-api debug ${UMUNDO_LIBRARIES_DEBUG})

# add_executable(test-publisher-churn test-publisher-churn.cpp)
# target_link_libraries(test-publisher-churn ${UMUNDO_LIBRARIES})
# target_link_libraries(test-publisher-churn debug ${UMUNDO_LIBRARIES_DEBUG})
