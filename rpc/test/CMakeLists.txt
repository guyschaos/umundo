add_executable(rpc-test-api services/test-api.cpp)
target_link_libraries(rpc-test-api ${UMUNDOCORE_LIBRARIES})
target_link_libraries(rpc-test-api ${UMUNDORPC_LIBRARIES})
target_link_libraries(rpc-test-api ${UMUNDOS11N_LIBRARIES})
