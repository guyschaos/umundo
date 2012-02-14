#add_subdirectory(discovery)

add_executable(umundo-pingpong test-pingpong.cpp)
target_link_libraries(umundo-pingpong ${UMUNDOCORE_LIBRARIES})

install(
	TARGETS umundo-pingpong
	DESTINATION bin
	PERMISSIONS WORLD_EXECUTE OWNER_EXECUTE GROUP_EXECUTE OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ
	COMPONENT tools 
)