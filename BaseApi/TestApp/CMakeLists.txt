set(MODULE_NAME TestApp)

set(${MODULE_NAME}_SRCS
        main.cpp
        )

set(${MODULE_NAME}_HEADERS
        )

add_executable(${MODULE_NAME}
        ${${MODULE_NAME}_SRCS}
        ${${MODULE_NAME}_HEADERS}
        )

target_link_libraries(${MODULE_NAME} PRIVATE
        BaseApi
        )

target_include_directories(${MODULE_NAME} PRIVATE ..)
