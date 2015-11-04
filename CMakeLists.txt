project(SociEntityFramework)
cmake_minimum_required(VERSION 2.8)
find_package(Boost REQUIRED COMPONENTS filesystem system program_options )
aux_source_directory(src SRC)
list(APPEND FULL_SRC
	${SRC} )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")
set(SOCI "${CMAKE_CURRENT_BINARY_DIR}/soci")

include_directories(${Boost_INCLUDE_DIRS} ${SOCI}/include)
link_directories(${LINK_DIRECTORIES} ${SOCI}/lib)
add_executable(${PROJECT_NAME} ${FULL_SRC})
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} soci_core)

include(ExternalProject)
ExternalProject_Add (
    soci
    URL  "https://github.com/SOCI/soci/archive/master.zip"
    UPDATE_COMMAND ""
    CMAKE_ARGS
        "-G${CMAKE_GENERATOR}"
        "-DSOCI_CORE_TARGET_OUTPUT_NAME=soci_core"
        "-DSOCI_TESTS=OFF"
        "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
        "-DCMAKE_INSTALL_PREFIX=${SOCI}" )
add_dependencies(${PROJECT_NAME} soci)
