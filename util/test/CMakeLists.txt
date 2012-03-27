add_executable(test-util-directory-monitoring test-directory-monitoring.cpp)
target_link_libraries(test-util-directory-monitoring ${UMUNDOCORE_LIBRARIES})
target_link_libraries(test-util-directory-monitoring ${UMUNDOUTIL_LIBRARIES})
set_target_properties(test-util-directory-monitoring PROPERTIES FOLDER "Tests")
