cmake_minimum_required(VERSION 3.5)

project(GoBang VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        chessboard.h chessboard.cpp chessboard.ui
        chessboard_fupan.h chessboard_fupan.cpp chessboard_fupan.ui
        gameover.h gameover.cpp gameover.ui
        chessboard_local_pvp.h chessboard_local_pvp.cpp chessboard_local_pvp.ui
        chessboard_pve.h chessboard_pve.cpp chessboard_pve.ui
        utils.h
        openpvehouse.h openpvehouse.cpp openpvehouse.ui
        netwindow.h netwindow.cpp netwindow.ui
        openhousedialog.h openhousedialog.cpp openhousedialog.ui
        chessboard_remote_pvp.cpp chessboard_remote_pvp.h chessboard_remote_pvp.ui TcpServer.cpp TcpServer.h TcpClient.cpp TcpClient.h TcpAbstract.h
	res.qrc
        replaywindow.cpp replaywindow.h replaywindow.ui
	open_local_pvp_dialog.h open_local_pvp_dialog.cpp open_local_pvp_dialog.ui
    )

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(GoBang
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET GoBang APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(GoBang SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(GoBang
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(GoBang PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(GoBang PRIVATE Qt${QT_VERSION_MAJOR}::Network)

set_target_properties(GoBang PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS GoBang
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(GoBang)
endif()
