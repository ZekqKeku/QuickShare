cmake_minimum_required(VERSION 3.19)

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

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)

add_executable(QuickShare WIN32 MACOSX_BUNDLE
    src/main.cpp
    src/core/AppConfig.h src/core/AppConfig.cpp
    src/gui/MainWindow.h src/gui/MainWindow.cpp src/gui/MainWindow.ui
    src/network/PixeldrainUploader.h src/network/PixeldrainUploader.cpp
    src/utils/Archiver.h src/utils/Archiver.cpp
    src/utils/NameGenerator.h src/utils/NameGenerator.cpp
    src/utils/SystemUtils.h src/utils/SystemUtils.cpp
)

target_compile_definitions(QuickShare PRIVATE 
    APP_VERSION="${APP_VERSION}"
    APP_STATUS="${APP_STATUS}"
    APP_NAME="${APP_NAME}"
    APP_DESCRIPTION="${APP_DESCRIPTION}"
)

if(APPLE)
    target_sources(QuickShare PRIVATE src/utils/MacUtils.mm)
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

target_link_libraries(QuickShare PRIVATE Qt6::Core Qt6::Widgets Qt6::Network)

install(TARGETS QuickShare
    BUNDLE DESTINATION .
    RUNTIME DESTINATION bin
)

if(UNIX AND NOT APPLE)
    install(FILES resources/com.quickshare.app.desktop DESTINATION share/applications)
endif()
