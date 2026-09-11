cmake_minimum_required(VERSION 3.22)

# enable project folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

set(FRAMEWORK_DIR "${CMAKE_SOURCE_DIR}/framework")
set(APP_DIR "${CMAKE_SOURCE_DIR}/app")


##################################   SFML   ##################################

set(BUILD_SHARED_LIBS       ON CACHE BOOL "")
set(SFML_BUILD_AUDIO        OFF CACHE BOOL "")
set(SFML_BUILD_DOC          OFF CACHE BOOL "")
set(SFML_BUILD_EXAMPLES     OFF CACHE BOOL "")
set(SFML_BUILD_GRAPHICS     ON CACHE BOOL "")
set(SFML_BUILD_NETWORK      OFF CACHE BOOL "")
set(SFML_BUILD_TEST_SUITE   OFF CACHE BOOL "")
set(SFML_BUILD_TEST_WINDOW  OFF CACHE BOOL "")

add_subdirectory(external/SFML)

set(SFML_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/external/SFML/include")

##################################   Core   ##################################
project(ImageWriterCore)

set(FRAMEWORK_INCLUDE_DIR "${FRAMEWORK_DIR}/include")

set(ALL_INCLUDE_DIRS
	"${FRAMEWORK_INCLUDE_DIR}"
    "${SFML_INCLUDE_DIR}"
)

set(FRAMEWORK_SRC_DIR "${FRAMEWORK_DIR}/src")

set(CORE_HEADER_FILES
	"${FRAMEWORK_INCLUDE_DIR}/ImageWriter/CoreEntry.h"
    "${FRAMEWORK_INCLUDE_DIR}/ImageWriter/CoreCommon.h"
    "${FRAMEWORK_INCLUDE_DIR}/ImageWriter/CoreAndAPIShared.h"
)

set(CORE_SRC_FILES
	"${FRAMEWORK_SRC_DIR}/CoreEntry.cpp"
)

add_library(ImageWriterCore SHARED ${CORE_SRC_FILES} ${CORE_HEADER_FILES})

set_target_properties(ImageWriterCore PROPERTIES FOLDER "ImageWriterFramework")
set_target_properties(ImageWriterCore PROPERTIES PREFIX  "")
target_compile_features(ImageWriterCore PRIVATE cxx_std_20)
target_include_directories(ImageWriterCore PRIVATE ${ALL_INCLUDE_DIRS})

target_link_libraries(ImageWriterCore sfml-graphics)

##################################   API   ##################################
project(ImageWriterAPI)

set(API_HEADER_FILES
	"${FRAMEWORK_INCLUDE_DIR}/ImageWriter/API.h"
    "${FRAMEWORK_INCLUDE_DIR}/ImageWriter/CoreAndAPIShared.h"
)

set(API_SRC_FILES
	"${FRAMEWORK_SRC_DIR}/API.cpp"
)

add_library(ImageWriterAPI SHARED ${API_SRC_FILES} ${API_HEADER_FILES})

set_target_properties(ImageWriterAPI PROPERTIES FOLDER "ImageWriterFramework")
set_target_properties(ImageWriterAPI PROPERTIES PREFIX  "")
target_compile_features(ImageWriterAPI PRIVATE cxx_std_20)
target_include_directories(ImageWriterAPI PRIVATE ${ALL_INCLUDE_DIRS})

target_link_libraries(ImageWriterAPI)

##################################   Test App   ##################################
project(TestApp)

set(TEST_APP_SRC_FILES
	"${FRAMEWORK_SRC_DIR}/TestApp.cpp"
)

add_executable(TestApp ${TEST_APP_SRC_FILES})


target_compile_features(TestApp PRIVATE cxx_std_20)
set_target_properties(TestApp PROPERTIES FOLDER "ImageWriterFramework/TestApp")
target_include_directories(TestApp PRIVATE ${ALL_INCLUDE_DIRS})

target_link_libraries(TestApp ImageWriterAPI)


##################################   Python Files ##################################

set(PYTHON_FILES
    "${FRAMEWORK_SRC_DIR}/TestApp.py"
    "${APP_DIR}/ImageWriterProtocol.py"
    "${APP_DIR}/ImageWriterApp.py"
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

if(WIN32) # This command is specific to Windows
    add_custom_command(
        TARGET ImageWriterCore POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_RUNTIME_DLLS:ImageWriterCore> # Generator expression to get all runtime DLLs linked by ImageWriterCore
            $<TARGET_FILE_DIR:ImageWriterCore>     # Generator expression for the output directory of ImageWriterCore
        COMMAND_EXPAND_LISTS # Expands the list of DLLs
        COMMENT "Copying runtime DLLs to app directory"
    )
endif()

# this will name the VS solution
project(TopLevel)
