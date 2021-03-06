########################################################################
# Project setup
########################################################################
cmake_minimum_required(VERSION 3.1.0)
project(PothosPlotters CXX)

if(${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME})
    find_package(Pothos "0.6.0" CONFIG REQUIRED)
else()
    find_package(Pothos CONFIG REQUIRED) #in-tree build
endif()

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

#args to set the CMAKE_PREFIX_PATH environment variable
if (CMAKE_PREFIX_PATH)
    set(ENV{CMAKE_PREFIX_PATH} ${CMAKE_PREFIX_PATH})
endif (CMAKE_PREFIX_PATH)

include(GNUInstallDirs)

########################################################################
# Qt devel setup
########################################################################
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Concurrent)
message(STATUS "QT_VERSION_MAJOR=${QT_VERSION_MAJOR}")
set(CMAKE_AUTOMOC ON)

########################################################################
# Spuce library
########################################################################
if(NOT SPUCE_IN_TREE)
    find_package(Spuce CONFIG)
endif(NOT SPUCE_IN_TREE)

if (Spuce_FOUND)
    message(STATUS "Spuce_VERSION: ${Spuce_VERSION}")
    message(STATUS "Spuce_INCLUDE_DIRS: ${Spuce_INCLUDE_DIRS}")
    message(STATUS "Spuce_LIBRARIES: ${Spuce_LIBRARIES}")
    if (NOT TARGET spuce)
    add_library(spuce INTERFACE)
    target_link_libraries(spuce INTERFACE "${Spuce_LIBRARIES}")
    target_include_directories(spuce INTERFACE "${Spuce_INCLUDE_DIRS}")
    endif()
else (Spuce_FOUND)
    message(WARNING "Spuce filter designer library not found...")
endif (Spuce_FOUND)

########################################################################
## Feature registration
########################################################################
include(FeatureSummary)
include(CMakeDependentOption)
cmake_dependent_option(ENABLE_PLOTTERS "Enable Pothos Plotters component" ON "Pothos_FOUND;Qt${QT_VERSION_MAJOR}_FOUND" OFF)
add_feature_info(Plotters ENABLE_PLOTTERS "Graphical plotter widgets for the GUI")

########################################################################
# Qwt setup
########################################################################
find_package(Qwt)

if (QWT_FOUND AND NOT QWT_VERSION_STRING)
    message(WARNING "Failed to extract QWT_VERSION_STRING from ${QWT_INCLUDE_DIRS} - using built-in Qwt")
    set(QWT_FOUND FALSE)
endif()

if (QWT_FOUND)
    message(STATUS "QWT_VERSION_STRING: ${QWT_VERSION_STRING}")
    if (QWT_VERSION_STRING LESS "6.1")
        set(QWT_FOUND FALSE)
        message(STATUS "Qwt older than v6.1 - using built-in Qwt")
    endif()
else (QWT_FOUND)
    message(STATUS "Qwt not found - using built-in Qwt")
endif (QWT_FOUND)

if (QWT_FOUND)

    add_library(PothosQwt INTERFACE)
    target_link_libraries(PothosQwt INTERFACE "${QWT_LIBRARIES}")
    target_include_directories(PothosQwt INTERFACE "${QWT_INCLUDE_DIRS}")
    target_compile_definitions(PothosQwt INTERFACE -DQWT_DLL)
    message(STATUS "QWT_INCLUDE_DIRS: ${QWT_INCLUDE_DIRS}")
    message(STATUS "QWT_LIBRARIES: ${QWT_LIBRARIES}")

elseif (ENABLE_PLOTTERS)

    #compiling qwt from source requires many more Qt libraries
    find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Concurrent OpenGL Svg PrintSupport)

    set(QWT_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/qwt6/src)

    if (NOT EXISTS ${QWT_SRC_DIR})
        message(FATAL_ERROR "Qwt submodule not configured (git submodule update --init --recursive)")
    endif ()

    set (QWT_VERSION_STRING 0)
    set ( _VERSION_FILE ${QWT_SRC_DIR}/qwt_global.h )
    if ( EXISTS ${_VERSION_FILE} )
      file ( STRINGS ${_VERSION_FILE} _VERSION_LINE REGEX "define[ ]+QWT_VERSION_STR" )
      if ( _VERSION_LINE )
        string ( REGEX REPLACE ".*define[ ]+QWT_VERSION_STR[ ]+\"(.*)\".*" "\\1" QWT_VERSION_STRING "${_VERSION_LINE}" )
      endif ()
    endif ()
    message(STATUS "QWT_VERSION_STRING: ${QWT_VERSION_STRING}")

    file(GLOB QWT_SOURCES "${QWT_SRC_DIR}/*.cpp")
    add_library(PothosQwt SHARED ${QWT_SOURCES})
    target_include_directories(PothosQwt PUBLIC ${QWT_SRC_DIR})
    set_target_properties(PothosQwt PROPERTIES DEFINE_SYMBOL "QWT_MAKEDLL")
    set_target_properties(PothosQwt PROPERTIES VERSION ${QWT_VERSION_STRING})
    target_link_libraries(PothosQwt PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
    target_link_libraries(PothosQwt PRIVATE Qt${QT_VERSION_MAJOR}::OpenGL)
    target_link_libraries(PothosQwt PRIVATE Qt${QT_VERSION_MAJOR}::Concurrent)
    target_link_libraries(PothosQwt PRIVATE Qt${QT_VERSION_MAJOR}::PrintSupport)
    target_link_libraries(PothosQwt PRIVATE Qt${QT_VERSION_MAJOR}::Svg)
    target_compile_definitions(PothosQwt PUBLIC -DQWT_DLL)
    install(TARGETS PothosQwt
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} # .so file
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} # .lib file
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} # .dll file
    )

endif (QWT_FOUND)

########################################################################
# plotter utils library
########################################################################
if (ENABLE_PLOTTERS)

file(GLOB PLOTTER_UTILS_SOURCES "PlotterUtils/*.cpp")
add_library(PothosPlotterUtils SHARED ${PLOTTER_UTILS_SOURCES})
target_include_directories(PothosPlotterUtils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/PlotterUtils)
set_target_properties(PothosPlotterUtils PROPERTIES DEFINE_SYMBOL "POTHOS_PLOTTER_UTILS_MAKEDLL")
set_target_properties(PothosPlotterUtils PROPERTIES VERSION 0.2.0)
target_link_libraries(PothosPlotterUtils PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(PothosPlotterUtils PRIVATE PothosQwt)
target_link_libraries(PothosPlotterUtils PRIVATE Pothos)
install(TARGETS PothosPlotterUtils
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} # .so file
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} # .lib file
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} # .dll file
)

endif (ENABLE_PLOTTERS)

########################################################################
# Build plotter widgets
########################################################################
add_subdirectory(Constellation)
add_subdirectory(LogicAnalyzer)
add_subdirectory(Periodogram)
add_subdirectory(QwtWidgets)
add_subdirectory(Spectrogram)
add_subdirectory(WaveMonitor)
