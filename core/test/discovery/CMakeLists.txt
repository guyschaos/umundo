add_executable(test-core-api test-api.cpp)

foreach(LIB ${UMUNDOCORE_LIBRARIES})
	target_link_libraries(test-core-api optimized ${LIB})
endforeach()

foreach(LIB ${UMUNDOCORE_LIBRARIES_DEBUG})
	target_link_libraries(test-core-api debug ${LIB})
endforeach()


add_executable(test-core-domains test-domains.cpp)
foreach(LIB ${UMUNDOCORE_LIBRARIES})
	target_link_libraries(test-core-domains optimized ${LIB})
endforeach()

foreach(LIB ${UMUNDOCORE_LIBRARIES_DEBUG})
	target_link_libraries(test-core-domains debug ${LIB})
endforeach()


#target_link_libraries(test-api debug ${UMUNDO_LIBRARIES_DEBUG})

# add_executable(test-publisher-churn test-publisher-churn.cpp)
# target_link_libraries(test-publisher-churn ${UMUNDO_LIBRARIES})
# target_link_libraries(test-publisher-churn debug ${UMUNDO_LIBRARIES_DEBUG})
