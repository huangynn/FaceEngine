cmake_minimum_required (VERSION 2.6)
project (Seeta_Test)

#configure_file(
#	"${CMAKE_SOURCE_DIR}/config.h.in"
#	"${CMAKE_SOURCE_DIR}/config.h"
#	)

set(PROJECT_SOURCE_DIR .)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/modules/")

exec_program(
	"git"
	${CMAKE_SOURCE_DIR}
	ARGS "describe"
	OUTPUT_VARIABLE GIT_VERSION
	)

add_definitions(-DGIT_VERSION="${GIT_VERSION}")

find_package(OpenCV COMPONENTS highgui objdetect core bgsegm videoio video tracking xfeatures2d REQUIRED)
find_package(Boost COMPONENTS program_options filesystem thread date_time graph REQUIRED )
find_package(GLOG REQUIRED)

message("${Caffe_LIBRARIES}")
message("${Boost_LIBRARIES}")
#message("${OpenCV_LIBRARIES}")
#message("${Caffe_LIBRARIES}")
if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DDEBUG)
	message("debug mode")
endif()

if(CMAKE_BUILD_TYPE MATCHES Sample)
	add_definitions(-DGENERATE_SAMPLE)
	message("generate training sample")
endif()


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11  -Wall -g -ggdb -msse4.1")

#message("${CMAKE_SOURCE_DIR}")
aux_source_directory(FaceDetection ST_SRC)
aux_source_directory(FaceDetection/classifier ST_SRC)
aux_source_directory(FaceDetection/feat ST_SRC)
aux_source_directory(FaceDetection/io ST_SRC)
aux_source_directory(FaceDetection/util ST_SRC)
aux_source_directory(FaceAlignment ST_SRC)
aux_source_directory(FaceIdentification ST_SRC)
aux_source_directory(. ST_SRC)

add_executable(seeta ${ST_SRC})

include_directories (/usr/local/include
	/usr/include
	${CMAKE_SOURCE_DIR})
link_directories(/usr/local/lib
	/usr/lib)

target_link_libraries(seeta
	${OpenCV_LIBS}
	${Boost_LIBRARIES}
	${GLOG_LIBRARIES}
	)

