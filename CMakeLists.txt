cmake_minimum_required(VERSION 3.16)

project(MinDashboard-Alpha VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Quick QuickWidgets QML SerialPort Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Quick QuickWidgets QML SerialPort Network)


set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
find_package(Qt6 REQUIRED COMPONENTS Core)

    qt_add_executable(MinDashboard-Alpha
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        src/core/log/logger.h src/core/log/logger.cpp
        src/core/log/logworker.h src/core/log/logworker.cpp
        resource.qrc
        src/ui/qml/NavBar.qml
        src/core/dao/dataprovider.h
        src/core/dao/serialprovider.h src/core/dao/serialprovider.cpp
        src/core/dao/networkprovider.h src/core/dao/networkprovider.cpp
        src/core/helper/framparser.h src/core/helper/framparser.cpp
        src/core/middle/model/datamodel.h src/core/middle/model/datamodel.cpp
        src/core/man/datamanager.h src/core/man/datamanager.cpp
        src/core/middle/readproject/readprojectconfig.h src/core/middle/readproject/readprojectconfig.cpp
        src/core/middle/datarecorder/datarecorder.h src/core/middle/datarecorder/datarecorder.cpp
        src/core/middle/StructType.h
        src/core/man/gateway.h src/core/man/gateway.cpp
        src/ui/uicom/gaugewidget.h src/ui/uicom/gaugewidget.cpp
        src/ui/uicom/arcgaugewidget.h src/ui/uicom/arcgaugewidget.cpp
        src/ui/uicom/realtimeareachartwidget.h src/ui/uicom/realtimeareachartwidget.cpp
        src/ui/uicom/statusindicatorwidget.h src/ui/uicom/statusindicatorwidget.cpp
        src/ui/uicom/cardwidget.h src/ui/uicom/cardwidget.cpp
        src/ui/uimain/logpage.h src/ui/uimain/logpage.cpp
        src/ui/uimain/dashboardpage.h src/ui/uimain/dashboardpage.cpp
        src/config/apis.h
        src/ui/qml/LogPage.qml
        src/ui/uimain/settingpage.h src/ui/uimain/settingpage.cpp
        src/ui/qml/AppDialog.qml src/ui/qml/SettingsPage.qml src/ui/qml/ToggleSwitch.qml
        src/core/middle/signals/datasignals.h src/core/middle/signals/datasignals.cpp

    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET MinDashboard-Alpha APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(MinDashboard-Alpha SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(MinDashboard-Alpha
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(MinDashboard-Alpha PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Quick
    Qt${QT_VERSION_MAJOR}::QuickWidgets
    Qt${QT_VERSION_MAJOR}::Qml
    Qt${QT_VERSION_MAJOR}::SerialPort
    Qt${QT_VERSION_MAJOR}::Network
)
target_link_libraries(MinDashboard-Alpha PRIVATE
    Qt6::Core
    Qt6::Quick
    Qt6::QuickWidgets
    Qt6::Qml
)
target_link_libraries(MinDashboard-Alpha PRIVATE Qt6::Core)
target_link_libraries(MinDashboard-Alpha PRIVATE Qt6::Core)
target_link_libraries(MinDashboard-Alpha PRIVATE Qt6::Core)
target_link_libraries(MinDashboard-Alpha PRIVATE Qt6::Core)
target_link_libraries(MinDashboard-Alpha PRIVATE Qt6::Core)
target_link_libraries(MinDashboard-Alpha PRIVATE Qt6::Core)
target_link_libraries(MinDashboard-Alpha PRIVATE Qt6::Core)
target_link_libraries(MinDashboard-Alpha PRIVATE Qt6::Core)
target_link_libraries(MinDashboard-Alpha PRIVATE Qt6::Core)
target_link_libraries(MinDashboard-Alpha PRIVATE Qt6::Core Qt6::Widgets)
target_link_libraries(MinDashboard-Alpha PRIVATE Qt6::Core Qt6::Widgets)
target_link_libraries(MinDashboard-Alpha PRIVATE Qt6::Core Qt6::Widgets)
target_link_libraries(MinDashboard-Alpha PRIVATE Qt6::Core Qt6::Widgets)
target_link_libraries(MinDashboard-Alpha PRIVATE Qt6::Core Qt6::Widgets)
target_link_libraries(MinDashboard-Alpha PRIVATE Qt6::Core Qt6::Widgets)
target_link_libraries(MinDashboard-Alpha PRIVATE Qt6::Core)
target_link_libraries(MinDashboard-Alpha PRIVATE Qt6::Core Qt6::Widgets)
target_link_libraries(MinDashboard-Alpha PRIVATE Qt6::Core Qt6::Widgets)
target_link_libraries(MinDashboard-Alpha PRIVATE Qt6::Core)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.MinDashboard-Alpha)
endif()
set_target_properties(MinDashboard-Alpha PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS MinDashboard-Alpha
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(MinDashboard-Alpha)
endif()
