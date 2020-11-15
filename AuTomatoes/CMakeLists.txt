
set(MODULE_NAME AuTomatoes)

set(${MODULE_NAME}_SRCS
        src/Appl.cpp
        src/AuTomatoesMain.cpp
        )

set(${MODULE_NAME}_HEADERS
        src/Appl.h
        )

add_executable(${MODULE_NAME}
        ${${MODULE_NAME}_SRCS}
        ${${MODULE_NAME}_HEADERS}
        )

target_link_libraries(${MODULE_NAME} PRIVATE
        Threads::Threads
        BaseApi
        )

target_include_directories(${MODULE_NAME}
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        )
