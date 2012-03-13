add_executable(test-rpc-api services/test-api.cpp)
target_link_libraries(test-rpc-api ${UMUNDOCORE_LIBRARIES})
target_link_libraries(test-rpc-api ${UMUNDORPC_LIBRARIES})
target_link_libraries(test-rpc-api ${UMUNDOS11N_LIBRARIES})
