macro(INITIALISE_PROJECT)
    set(CMAKE_VERBOSE_MAKEFILE ON)

    # Required packages
    find_package(Qt5Widgets REQUIRED)
    # Keep track of some information about Qt
    set(QT_BINARY_DIR ${_qt5Widgets_install_prefix}/bin)
    set(QT_LIBRARY_DIR ${_qt5Widgets_install_prefix}/lib)
    set(QT_PLUGINS_DIR ${_qt5Widgets_install_prefix}/plugins)
    set(QT_VERSION_MAJOR ${Qt5Widgets_VERSION_MAJOR})
    set(QT_VERSION_MINOR ${Qt5Widgets_VERSION_MINOR})
    set(QT_VERSION_PATCH ${Qt5Widgets_VERSION_PATCH})

    string(TOLOWER "${CMAKE_BUILD_TYPE}" BUILD_TYPE_LOWERCASE)
    if(BUILD_TYPE_LOWERCASE STREQUAL "debug")
        message(STATUS "Building a debug version...")
        #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_DEBUG -DDEBUG")
        add_definitions(-DQT_DEBUG)
    else()
        message(STATUS "Building a release version...")
        add_definitions(-DQT_NO_DEBUG_OUTPUT)
        add_definitions(-DQT_NO_DEBUG)
    endif()

    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)

    # Defining compiler specific settings
    if(WIN32)

    else()

        # Adding -std=c++17 flag explicitly
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
        set(CMAKE_CXX_REQUIRED_FLAGS -std=c++17)

        # Enable warning errors
        set(WARNING_ERROR "-Werror")

        set(DISABLED_WARNINGS "-Wno-unused-parameter -Wno-unused-function -Wno-undef -Wno-missing-declarations -Wno-unknown-pragmas \
            -Wno-sign-compare -Wno-cast-qual -Wno-cast-align -Wno-redundant-decls -Wno-implicit-fallthrough \
            -Wno-unused-local-typedef -Wno-unused-lambda-capture -Wno-pedantic -Wno-deprecated -Wno-parentheses")

        if (APPLE)
            set(DISABLED_WARNINGS "${DISABLED_WARNINGS} -Wno-pessimizing-move \
                -Wno-inconsistent-missing-override -Wno-infinite-recursion \
                -Wno-c++1z-extensions -Wno-macro-redefined -Wno-#pragma-messages")
        endif()

        set(EXTRA_WARNINGS "-Woverloaded-virtual -Wundef -Wall -Wextra \
                           -Wformat -Wunused-variable -Wreturn-type -Wempty-body -Wdisabled-optimization \
                           -Wredundant-decls -Wpacked -Wuninitialized -Wswitch \
                           -pedantic-errors -fuse-cxa-atexit -ffast-math -funroll-loops")
        if(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            set(EXTRA_WARNINGS "${EXTRA_WARNINGS} -Wpedantic -Weffc++ -Wnon-virtual-dtor \
                               -Wswitch-default -Wint-to-void-pointer-cast")
        endif()
    endif()

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_WARNINGS} ${DISABLED_WARNINGS} ${WARNING_ERROR}")

    # Check whether the compiler supports position-independent code
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("-fPIC" COMPILER_SUPPORTS_PIC)
    if(COMPILER_SUPPORTS_PIC)
      add_definitions("-fPIC")
    endif()

    # Qt 5.X does not include private headers by default
    add_definitions(-DNO_QT_PRIVATE_HEADERS)
    
endmacro()

macro(USE_QT5LIB qt5lib)
    find_package(${qt5lib} REQUIRED)
    include_directories(${${qt5lib}_INCLUDE_DIRS})
    add_definitions(${${qt5lib}_DEFINITIONS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${${qt5lib}_EXECUTABLE_COMPILE_FLAGS}")
endmacro()

function(ST_LIBRARY)
    get_filename_component(PARENTDIR ${CMAKE_CURRENT_SOURCE_DIR} NAME)
    add_library("${PARENTDIR}" OBJECT ${LIBRARY_ARG_INCLUDES} ${LIBRARY_ARG_SOURCES})
    source_group("${PARENTDIR}" FILES ${LIBRARY_ARG_INCLUDES} ${LIBRARY_ARG_SOURCES})
endfunction()
