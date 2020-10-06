set(MODULE_NAME BaseApi)

set(${MODULE_NAME}_SRCS
        BaCore.cpp
        BaIniParse.cpp
        BaLog.cpp
        BaMsg.cpp
        BaseApi.cpp
        BaSwOsci.cpp
        CtrlPT1.cpp
        HwCom.cpp
        HwGpio.cpp
        HwPi.cpp
        impl/CBaLog.cpp
        impl/CBaMsg.cpp
        impl/CBaSwOsci.cpp
        impl/CCtrlPT1.cpp
        impl/COsIpcRegistry.cpp
        impl/COsIpcSvr.cpp
        OsIpc.cpp
        OsIpcRegistry.cpp
        OsProc.cpp
        SensAds1115.cpp
        )

set(${MODULE_NAME}_HEADERS
        BaBool.h
        BaCore.h
        BaGenMacros.h
        BaIniParse.h
        BaLog.h
        BaLogMacros.h
        BaMsg.h
        BaseApi.h
        BaSwOsci.h
        BaUtils.hpp
        CtrlPT1.h
        dbg/BaDbgMacros.h
        HwCom.h
        HwGpio.h
        HwPi.h
        impl/BaTmpTime.hpp
        impl/CBaLog.h
        impl/CBaMsg.h
        impl/CBaSwOsci.h
        impl/CCtrlPT1.h
        impl/COsIpcRegistry.h
        impl/COsIpcSvr.h
        impl/HwGpioPinout.h
        OsDef.h
        OsIpc.h
        OsIpcRegistry.h
        OsProc.h
        SensAds1115.h
        )

add_library(${MODULE_NAME} STATIC
        ${${MODULE_NAME}_SRCS}
        ${${MODULE_NAME}_HEADERS}
        ../TestApp/main.cpp)

target_include_directories(${MODULE_NAME}
        PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/impl
        )

