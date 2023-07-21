# Sources and Headers
set(LIBRARY_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/my_lib.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/router_socket.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/router_message.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/client_message.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/stream_socket.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/websocket.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/logger_interface.cpp"
)
set(LIBRARY_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/my_lib.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/router_socket.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/router_message.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/client_message.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/stream_socket.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/websocket.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ring_buffer.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/stream_parser.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/logger_interface.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/bit_converter.hpp"
)

set(LIBRARY_INCLUDES "./" "${CMAKE_BINARY_DIR}/configured_files/include")

# MyLib Library

add_library(${LIBRARY_NAME} STATIC ${LIBRARY_SOURCES} ${LIBRARY_HEADERS})
target_include_directories(${LIBRARY_NAME} PUBLIC ${LIBRARY_INCLUDES})
target_link_libraries(
    ${LIBRARY_NAME}
    PUBLIC nlohmann_json::nlohmann_json
            fmt::fmt
            spdlog::spdlog
            cxxopts::cxxopts
            cppserver
            cppzmq
            tbb
    )

if(${ENABLE_WARNINGS})
    target_set_warnings(
        TARGET
        ${LIBRARY_NAME}
        ENABLE
        ${ENABLE_WARNINGS}
        AS_ERRORS
        ${ENABLE_WARNINGS_AS_ERRORS})
endif()

if(${ENABLE_LTO})
    target_enable_lto(
        TARGET
        ${LIBRARY_NAME}
        ENABLE
        ON)
endif()

if(${ENABLE_CLANG_TIDY})
    add_clang_tidy_to_target(${LIBRARY_NAME})
endif()
