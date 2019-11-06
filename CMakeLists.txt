cmake_minimum_required(VERSION 3.5.1)
project (tsf_test)

add_library(${PROJECT_NAME} SHARED ${CMAKE_JS_SRC})

target_sources(${PROJECT_NAME}
    PRIVATE
        src/foo_interface.cpp
        src/foo_interface.h
        src/main.cpp
        src/bar_callback_data.h
        src/bar_data.cpp
        src/bar_data.h
        src/bar_handler.cpp
        src/bar_handler.h
        src/bar_interface.cpp
        src/bar_interface.h
	src/scoped_lock_mutex.h
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_JS_INC}
        "${CMAKE_SOURCE_DIR}/node_modules/node-addon-api"
)

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")

target_link_libraries(${PROJECT_NAME} ${CMAKE_JS_LIB})
