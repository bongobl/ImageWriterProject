cmake_minimum_required(VERSION 3.22)

# enable project folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

##################################   Image Writer ##################################
project(ImageWriter)

set(PROJECT_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(STB_IMAGE_DIR "${CMAKE_SOURCE_DIR}/external/stb")

set(ALL_INCLUDE_DIRS
	"${PROJECT_INCLUDE_DIR}"
	"${STB_IMAGE_DIR}"
)

set(PROJECT_SRC_DIR "${CMAKE_SOURCE_DIR}/src")

set(HEADER_FILES
	"${PROJECT_INCLUDE_DIR}/Image.h"
	"${PROJECT_INCLUDE_DIR}/API.h"
)

set(SRC_FILES
	"${PROJECT_SRC_DIR}/Image.cpp"
	"${PROJECT_SRC_DIR}/API.cpp"
)

add_library(ImageWriter SHARED ${SRC_FILES} ${HEADER_FILES})

set_target_properties(ImageWriter PROPERTIES FOLDER "ImageWriter")
target_compile_features(ImageWriter PRIVATE cxx_std_20)
target_include_directories(ImageWriter PRIVATE ${ALL_INCLUDE_DIRS})


##################################   Test App ##################################
project(TestApp)

set(TEST_APP_SRC_FILES
	"${PROJECT_SRC_DIR}/main.cpp"
)

add_executable(TestApp ${TEST_APP_SRC_FILES})


target_compile_features(TestApp PRIVATE cxx_std_20)
set_target_properties(TestApp PROPERTIES FOLDER "TestApp")
target_include_directories(TestApp PRIVATE ${ALL_INCLUDE_DIRS})

target_link_libraries(TestApp ImageWriter)

project(TopLevel)