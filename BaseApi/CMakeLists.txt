
set(MODULE_NAME BaseApi)

set(${MODULE_NAME}_SRCS
        src/BaCore.cpp
        src/BaIniParse.cpp
        src/BaLog.cpp
        src/BaMsg.cpp
        src/BaseApi.cpp
        src/BaSwOsci.cpp
        src/CtrlPT1.cpp
        src/HwCom.cpp
        src/HwGpio.cpp
        src/HwPi.cpp
        src/impl/CBaLog.cpp
        src/impl/CBaMsg.cpp
        src/impl/CBaSwOsci.cpp
        src/impl/CCtrlPT1.cpp
        src/impl/COsIpcRegistry.cpp
        src/impl/COsIpcSvr.cpp
        src/OsIpc.cpp
        src/OsIpcRegistry.cpp
        src/OsProc.cpp
        src/SensAds1115.cpp
        )

set(${MODULE_NAME}_HEADERS
        src/BaBool.h
        src/BaCore.h
        src/BaGenMacros.h
        src/BaIniParse.h
        src/BaLog.h
        src/BaLogMacros.h
        src/BaMsg.h
        src/BaseApi.h
        src/BaSwOsci.h
        src/BaUtils.hpp
        src/CtrlPT1.h
        src/dbg/BaDbgMacros.h
        src/HwCom.h
        src/HwGpio.h
        src/HwPi.h
        src/impl/BaTmpTime.hpp
        src/impl/CBaLog.h
        src/impl/CBaMsg.h
        src/impl/CBaSwOsci.h
        src/impl/CCtrlPT1.h
        src/impl/COsIpcRegistry.h
        src/impl/COsIpcSvr.h
        src/impl/HwGpioPinout.h
        src/OsDef.h
        src/OsIpc.h
        src/OsIpcRegistry.h
        src/OsProc.h
        src/SensAds1115.h
        )

add_library(${MODULE_NAME} STATIC
        ${${MODULE_NAME}_SRCS}
        ${${MODULE_NAME}_HEADERS}
        )

target_include_directories(${MODULE_NAME}
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/impl
        ${CMAKE_CURRENT_SOURCE_DIR}/src/dbg
        )
