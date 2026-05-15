cmake_minimum_required(VERSION 3.19)
cmake_policy(SET CMP0135 NEW)

file(READ "manifest.json" MANIFEST_JSON)
string(JSON APP_NAME GET "${MANIFEST_JSON}" "name")
string(JSON APP_VERSION GET "${MANIFEST_JSON}" "version")
string(JSON APP_STATUS GET "${MANIFEST_JSON}" "status")
string(JSON APP_DESCRIPTION GET "${MANIFEST_JSON}" "description")

project(${APP_NAME} VERSION ${APP_VERSION} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(MSVC)
    add_compile_options(/Zc:__cplusplus)
endif()

if(APPLE AND EXISTS "/opt/homebrew/opt/qt")
    list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew/opt/qt")
elseif(APPLE AND EXISTS "/usr/local/opt/qt")
    list(APPEND CMAKE_PREFIX_PATH "/usr/local/opt/qt")
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick QuickControls2 Widgets Network Test)

set(QUICKSHARE_SOURCES
    src/core/AppConfig.h src/core/AppConfig.cpp
    src/core/LanguageManager.h src/core/LanguageManager.cpp
    src/core/SettingsManager.h src/core/SettingsManager.cpp
    src/network/PixeldrainUploader.h src/network/PixeldrainUploader.cpp
    src/gui/UploadController.h src/gui/UploadController.cpp
    src/cli/CliHandler.h src/cli/CliHandler.cpp
    src/utils/Archiver.h src/utils/Archiver.cpp
    src/utils/NameGenerator.h src/utils/NameGenerator.cpp
    src/utils/SystemUtils.h src/utils/SystemUtils.cpp
    src/utils/Clipboard.h
)

if(APPLE)
    list(APPEND QUICKSHARE_SOURCES src/utils/MacUtils.mm)
endif()

qt_add_resources(RESOURCES resources/qml.qrc)

add_executable(QuickShare WIN32 MACOSX_BUNDLE
    src/main.cpp
    ${QUICKSHARE_SOURCES}
    ${RESOURCES}
)

add_custom_command(TARGET QuickShare POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/src/lang"
    "$<TARGET_FILE_DIR:QuickShare>/lang"
)

if(APPLE)
    add_custom_command(TARGET QuickShare POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/src/lang"
        "$<TARGET_FILE_DIR:QuickShare>/../Resources/lang"
    )
endif()

target_compile_definitions(QuickShare PRIVATE 
    APP_VERSION="${APP_VERSION}"
    APP_STATUS="${APP_STATUS}"
    APP_NAME="${APP_NAME}"
    APP_DESCRIPTION="${APP_DESCRIPTION}"
)

if(APPLE)
    set_target_properties(QuickShare PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.quickshare.app"
        MACOSX_BUNDLE_BUNDLE_NAME "${APP_NAME}"
        MACOSX_BUNDLE_BUNDLE_VERSION "${APP_VERSION}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${APP_VERSION}"
    )
    
    add_custom_command(TARGET QuickShare POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/resources/mac/Share via QuickShare.workflow"
        "$<TARGET_FILE_DIR:QuickShare>/../Library/Services/Share via QuickShare.workflow"
    )
elseif(WIN32)
endif()

target_link_libraries(QuickShare PRIVATE 
    Qt6::Core 
    Qt6::Gui 
    Qt6::Qml 
    Qt6::Quick 
    Qt6::QuickControls2 
    Qt6::Widgets
    Qt6::Network
)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.tar.gz
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(QuickShareTests
    tests/test_utils.cpp
    tests/test_network.cpp
    ${QUICKSHARE_SOURCES}
)

target_compile_definitions(QuickShareTests PRIVATE 
    APP_VERSION="${APP_VERSION}"
    APP_STATUS="${APP_STATUS}"
    APP_NAME="${APP_NAME}"
    APP_DESCRIPTION="${APP_DESCRIPTION}"
)

target_link_libraries(QuickShareTests 
    PRIVATE 
        Qt6::Core 
        Qt6::Gui
        Qt6::Widgets
        Qt6::Network 
        Qt6::Test
        GTest::gtest_main
)

add_test(NAME QuickShareTests COMMAND QuickShareTests)


install(TARGETS QuickShare
    BUNDLE DESTINATION .
    RUNTIME DESTINATION bin
)

if(UNIX AND NOT APPLE)
    install(FILES resources/com.quickshare.app.desktop DESTINATION share/applications)
    install(FILES resources/com.quickshare.app.png DESTINATION share/icons/hicolor/512x512/apps)
endif()
