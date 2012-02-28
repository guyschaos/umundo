if (WIN32)
	include_directories(${PROJECT_SOURCE_DIR}/contrib/snippets)
	add_executable(umundo-monitor umundo-monitor.cpp ${PROJECT_SOURCE_DIR}/contrib/snippets/XGetopt.cpp)
else()
	add_executable(umundo-monitor umundo-monitor.cpp)
endif()
target_link_libraries(umundo-monitor ${UMUNDOCORE_LIBRARIES})

install(
	TARGETS umundo-monitor
	DESTINATION bin
	PERMISSIONS WORLD_EXECUTE OWNER_EXECUTE GROUP_EXECUTE OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ
	COMPONENT tools 
)

add_executable(umundo-pingpong umundo-pingpong.cpp)
target_link_libraries(umundo-pingpong ${UMUNDOCORE_LIBRARIES})

install(
	TARGETS umundo-pingpong
	DESTINATION bin
	PERMISSIONS WORLD_EXECUTE OWNER_EXECUTE GROUP_EXECUTE OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ
	COMPONENT tools 
)