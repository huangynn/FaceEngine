# Find Rabbitmq C++  library

# AMQP_FOUND        - system has rabbitmq
# AMQP_INCLUDE_DIRS - rabbitmq include directories
# AMQP_LIBRARIES    - libraries need to use rabbitmq

# Find the include directories
if(AMQP_INCLUDE_DIRS AND AMQP_LIBRARIES)
	set(AMQP_FIND_QUIETLY TRUE)
else()
	find_path(AMQP_INCLUDE_DIR
		NAMES amqpcpp.h
		HINTS ${AMQP_DIR}
		PATH_SUFFIXES include)
	
	if (APPLE)
		set(amqp_lib_name amqp-cpp)
	elseif(UNIX)
		set(amqp_lib_name amqpcpp)
	endif()

	FIND_LIBRARY(AMQP_LIBRARY
		NAMES ${amqp_lib_name} 
		HINTS ${AMQP_DIR}
		PATH_SUFFIXES ${CMAKE_INSTALL_LIBDIR})


	SET(AMQP_INCLUDE_DIRS ${AMQP_INCLUDE_DIR})
	SET(AMQP_LIBRARIES ${AMQP_LIBRARY})

	include(FindPackageHandleStandardArgs)
	FIND_PACKAGE_HANDLE_STANDARD_ARGS(
		AMQP DEFAULT_MSG AMQP_INCLUDE_DIR AMQP_LIBRARY
		)
	mark_as_advanced(AMQP_INCLUDE_DIR AMQP_LIBRARY)
endif()

