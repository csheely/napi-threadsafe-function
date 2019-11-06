cmake_minimum_required(VERSION 3.5.1)
project (tsf_test)

add_library(${PROJECT_NAME} SHARED ${CMAKE_JS_SRC})

add_definitions(-DNAPI_VERSION=4)

target_sources(${PROJECT_NAME}
    PRIVATE
        src/foo.cpp
        src/foo.h
        src/main.cpp
        src/bar_activity_receiver.cpp
        src/bar_activity_receiver.h
        src/bar_interface.cpp
        src/bar_interface.h
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_JS_INC}
        "${CMAKE_SOURCE_DIR}/node_modules/node-addon-api"
)

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")

target_link_libraries(${PROJECT_NAME} ${CMAKE_JS_LIB})
