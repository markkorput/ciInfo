if( NOT TARGET ciInfo )
	get_filename_component( ciInfo_SOURCE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../src" ABSOLUTE )
	get_filename_component( ciInfo_INCLUDE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../include" ABSOLUTE )
	get_filename_component( CINDER_PATH "${CMAKE_CURRENT_LIST_DIR}/../../../.." ABSOLUTE )

	FILE(GLOB ciInfo_SOURCES ${ciInfo_SOURCE_PATH}/info/*.cpp)

	add_library( ciInfo ${ciInfo_SOURCES} )

	target_include_directories( ciInfo PUBLIC "${ciInfo_INCLUDE_PATH}" )
	target_include_directories( ciInfo SYSTEM BEFORE PUBLIC "${CINDER_PATH}/include" )

	if( NOT TARGET cinder )
		    include( "${CINDER_PATH}/proj/cmake/configure.cmake" )
		    find_package( cinder REQUIRED PATHS
		        "${CINDER_PATH}/${CINDER_LIB_DIRECTORY}"
		        "$ENV{CINDER_PATH}/${CINDER_LIB_DIRECTORY}" )
	endif()
	target_link_libraries( ciInfo PRIVATE cinder )
endif()
