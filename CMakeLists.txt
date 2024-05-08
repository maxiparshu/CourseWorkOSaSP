cmake_minimum_required(VERSION 3.5)

project(HtmlBrowser VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Core Gui WebEngineWidgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Core Gui WebEngineWidgets)

set(PROJECT_SOURCES
        main.cpp



)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(HtmlBrowser
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        webpage.h webpage.cpp
        tabwidget.h tabwidget.cpp
        webview.h webview.cpp
        ../../../../Qt/Examples/Qt-6.7.0/webenginewidgets/simplebrowser/data/simplebrowser.qrc
        webpopupwindow.h webpopupwindow.cpp
        browserwindow.h browserwindow.cpp
        browser.h browser.cpp
        downloadwidget.ui
        downloadmanager.ui
        downloadmanager.h downloadmanager.cpp
        downloadwidget.h downloadwidget.cpp


    )

set(browser_resource_files
    "assets/3rdparty/dialog-error.png"
    "assets/3rdparty/edit-clear.png"
    "assets/3rdparty/go-bottom.png"
    "assets/3rdparty/go-next.png"
    "assets/3rdparty/go-previous.png"
    "assets/3rdparty/process-stop.png"
    "assets/3rdparty/text-html.png"
    "assets/3rdparty/view-refresh.png"
)
qt_add_resources(HtmlBrowser  "HtmlBrowser"
    PREFIX
        "/"
    BASE
        "assets/"
    FILES
        ${browser_resource_files}
)
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET HtmlBrowser APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(HtmlBrowser SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(HtmlBrowser
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(HtmlBrowser PUBLIC Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::WebEngineWidgets
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.HtmlBrowser)
endif()
set_target_properties(HtmlBrowser PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS HtmlBrowser
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(HtmlBrowser)
endif()
