cmake_minimum_required(VERSION 3.19)
project(minesweeper LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(minesweeper
    WIN32 MACOSX_BUNDLE
    main.cpp
    view.cpp
    view.h
    view.ui
    controller.h controller.cpp
    model.h model.cpp
    custom_button.h custom_button.cpp
)

target_link_libraries(minesweeper
    PRIVATE
        Qt::Core
        Qt::Widgets
)

include(GNUInstallDirs)

install(TARGETS minesweeper
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET minesweeper
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
