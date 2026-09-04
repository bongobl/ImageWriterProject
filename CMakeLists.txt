cmake_minimum_required(VERSION 3.22)

# enable project folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

##################################   Core   ##################################
project(ImageWriterCore)

set(PROJECT_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(STB_IMAGE_DIR "${CMAKE_SOURCE_DIR}/external/stb")

set(ALL_INCLUDE_DIRS
	"${PROJECT_INCLUDE_DIR}"
	"${STB_IMAGE_DIR}"
)

set(PROJECT_SRC_DIR "${CMAKE_SOURCE_DIR}/src")

set(COTE_HEADER_FILES
	"${PROJECT_INCLUDE_DIR}/Image.h"
	"${PROJECT_INCLUDE_DIR}/CoreEntry.h"
)

set(CORE_SRC_FILES
	"${PROJECT_SRC_DIR}/Image.cpp"
	"${PROJECT_SRC_DIR}/CoreEntry.cpp"
)

add_library(ImageWriterCore SHARED ${CORE_SRC_FILES} ${COTE_HEADER_FILES})

set_target_properties(ImageWriterCore PROPERTIES FOLDER "ImageWriter")
set_target_properties(ImageWriterCore PROPERTIES PREFIX  "")
target_compile_features(ImageWriterCore PRIVATE cxx_std_20)
target_include_directories(ImageWriterCore PRIVATE ${ALL_INCLUDE_DIRS})

##################################   API   ##################################
project(ImageWriterAPI)

set(API_HEADER_FILES
	"${PROJECT_INCLUDE_DIR}/API.h"
)

set(API_SRC_FILES
	"${PROJECT_SRC_DIR}/API.cpp"
)

add_library(ImageWriterAPI SHARED ${API_SRC_FILES} ${API_HEADER_FILES})

set_target_properties(ImageWriterAPI PROPERTIES FOLDER "ImageWriter")
set_target_properties(ImageWriterAPI PROPERTIES PREFIX  "")
target_compile_features(ImageWriterAPI PRIVATE cxx_std_20)
target_include_directories(ImageWriterAPI PRIVATE ${ALL_INCLUDE_DIRS})

##################################   Test App   ##################################
project(TestApp)

set(TEST_APP_SRC_FILES
	"${PROJECT_SRC_DIR}/TestApp.cpp"
)

add_executable(TestApp ${TEST_APP_SRC_FILES})


target_compile_features(TestApp PRIVATE cxx_std_20)
set_target_properties(TestApp PROPERTIES FOLDER "TestApp")
target_include_directories(TestApp PRIVATE ${ALL_INCLUDE_DIRS})

target_link_libraries(TestApp ImageWriterAPI)


##################################   Python App ##################################

set(PYTHON_FILES
    "${PROJECT_SRC_DIR}/Writer.py"
    "${PROJECT_SRC_DIR}/ImageWriterProtocol.py"
    "${PROJECT_SRC_DIR}/ImageWriterServer.py"
)

foreach(file ${PYTHON_FILES})

    get_filename_component(filename ${file} NAME)

    add_custom_command(
        TARGET ImageWriterAPI POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${file}"
            "${CMAKE_CURRENT_BINARY_DIR}/${filename}"
            COMMENT "Copying ${filename} to ${CMAKE_CURRENT_BINARY_DIR}"
    )
endforeach()

# this will name the VS solution
project(TopLevel)
