add_executable(test-util-directory-monitoring test-directory-monitoring.cpp)
target_link_libraries(test-util-directory-monitoring ${UMUNDOCORE_LIBRARIES})
target_link_libraries(test-util-directory-monitoring ${UMUNDOUTIL_LIBRARIES})
set_target_properties(test-util-directory-monitoring PROPERTIES FOLDER "Tests")

add_executable(test-util-structured-cache test-structured-cache.cpp)
target_link_libraries(test-util-structured-cache ${UMUNDOCORE_LIBRARIES})
target_link_libraries(test-util-structured-cache ${UMUNDOUTIL_LIBRARIES})
set_target_properties(test-util-structured-cache PROPERTIES FOLDER "Tests")
