
set(MODULE_NAME cppunit)

file(GLOB_RECURSE ${MODULE_NAME}_HEADERS
    "*.h"
)

file(GLOB_RECURSE ${MODULE_NAME}_SRCS
    "*.cpp"
)

add_library(${MODULE_NAME} STATIC
        ${${MODULE_NAME}_HEADERS}
        ${${MODULE_NAME}_SRCS}
        )

target_include_directories(${MODULE_NAME}
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        )
