cmake_minimum_required(VERSION 3.19)
project(Numerical_Analysis LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find Qt6
find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

# Find GiNaC manually using pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(GiNaC REQUIRED ginac)

# Fallback (in case pkg-config is missing or paths are not set)
if(NOT GiNaC_FOUND)
    set(GiNaC_INCLUDE_DIRS "/usr/include")
    set(GiNaC_LIBRARIES "/usr/lib/x86_64-linux-gnu/libginac.so")
endif()

# Include directories
include_directories(${GiNaC_INCLUDE_DIRS})

qt_standard_project_setup()
qt_add_resources(resources.qrc)
qt_add_executable(Numerical_Analysis
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui

    rootmethods.cpp
    rootmethods.h

    interpolationmethods.h
    interpolationmethods.cpp

    integrationmethods.h
    integrationmethods.cpp
    resources.qrc

)

# Link Qt and GiNaC
target_link_libraries(Numerical_Analysis
    PRIVATE
        Qt::Core
        Qt::Widgets
        ${GiNaC_LIBRARIES}
)

include(GNUInstallDirs)

install(TARGETS Numerical_Analysis
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET Numerical_Analysis
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})

# Debug info (optional)
message(STATUS "GiNaC include dirs: ${GiNaC_INCLUDE_DIRS}")
message(STATUS "GiNaC libraries: ${GiNaC_LIBRARIES}")
