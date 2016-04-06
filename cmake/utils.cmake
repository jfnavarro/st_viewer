macro(INITIALISE_PROJECT)

    set(CMAKE_VERBOSE_MAKEFILE OFF)
    set(CMAKE_INCLUDE_CURRENT_DIR ON)

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
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_DEBUG -DDEBUG")
        add_definitions(-DQT_DEBUG)
    else()
        message(STATUS "Building a release version...")
        add_definitions(-DQT_NO_DEBUG_OUTPUT)
        add_definitions(-DQT_NO_DEBUG)
    endif()

    # Defining compiler specific settings
    if(WIN32)
        if(MSVC)
           string(REPLACE "/W3" "/W3 /WX" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
           set(LINK_FLAGS_PROPERTIES "/STACK:10000000 /MACHINE:X86")
           if(BUILD_TYPE_LOWERCASE STREQUAL "debug")
              set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_DEBUG /MDd /Zi /Ob0 /Od /RTC1")
              set(LINK_FLAGS_PROPERTIES "${LINK_FLAGS_PROPERTIES} /DEBUG")
           else()
              set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DNDEBUG /MD /O2 /Ob2")
           endif()
           # Removes Microsoft Visual Studio's well intentioned warnings about 'unsafe' calls.
           add_definitions(-D_SCL_SECURE_NO_WARNINGS)
        endif()
    else()
        # Adding -std=c++11 flag explicitly
        # It is a temporary fix to get building with CLANG working again.
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
        set(WARNING_ERROR "-Werror")
        # Disabled warnings due to Qt
        set(DISABLED_WARNINGS "-Wno-missing-braces -Wno-float-equal -Wno-shadow -Wno-unreachable-code \
                               -Wno-switch-enum -Wno-type-limits -Wno-deprecated")
        if (APPLE)
            # This is needed for a compatibility issue with XCode 7
            set(DISABLED_WARNINGS "${DISABLED_WARNINGS} -Wno-inconsistent-missing-override")
        endif()

        set(EXTRA_WARNINGS "-Woverloaded-virtual -Wundef -Wall -Wextra -Wformat-nonliteral \
                           -Wformat -Wunused-variable -Wreturn-type -Wempty-body -Wdisabled-optimization \
                           -Wredundant-decls -Wpacked -Wuninitialized -Wcast-align -Wcast-qual -Wswitch \
                           -Wsign-compare -pedantic-errors -fuse-cxa-atexit -ffor-scope")
        if(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            set(EXTRA_WARNINGS "${EXTRA_WARNINGS} -Wold-style-cast -Wpedantic  -Weffc++ -Wnon-virtual-dtor \
                               -Wswitch-default -Wint-to-void-pointer-cast")
            # Needed for a bug in Qt 5.5.0, it will be fixed in 5.5.1
            set(DISABLED_WARNINGS "${DISABLED_WARNINGS} -Wno-unknown-pragmas")
        endif()
    endif()

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_WARNINGS} ${DISABLED_WARNINGS} ${WARNING_ERROR}")

    # Ask for Unicode to be used
    add_definitions(-DUNICODE)
    if(WIN32)
        add_definitions(-D_UNICODE)
    endif()

    # Qt 5.X does not include private headers by default
    add_definitions(-DNO_QT_PRIVATE_HEADERS)

    # Set the RPATH information on Linux
    # Note: this prevent us from having to use the uncool LD_LIBRARY_PATH...
    if(NOT WIN32 AND NOT APPLE)
        set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib:$ORIGIN/../plugins/${PROJECT_NAME}")
    endif()

    if(APPLE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mmacosx-version-min=10.7 -stdlib=libc++")
    endif()
    
endmacro()

macro(use_qt5lib qt5lib)
    find_package(${qt5lib} REQUIRED)
    include_directories(${${qt5lib}_INCLUDE_DIRS})
    add_definitions(${${qt5lib}_DEFINITIONS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${${qt5lib}_EXECUTABLE_COMPILE_FLAGS}")
endmacro()

macro(PROJECT_GROUP TARGET_NAME FOLDER_PATH)
    #Organize projects into folders
    set_property(TARGET ${TARGET_NAME} PROPERTY FOLDER ${FOLDER_PATH})
endmacro()

function(ST_LIBRARY)
    get_filename_component(PARENTDIR ${CMAKE_CURRENT_SOURCE_DIR} NAME)
    set(UI_GENERATED_FILES)
    if(DEFINED LIBRARY_ARG_UI_FILES) 
        qt5_wrap_ui(UI_GENERATED_FILES ${LIBRARY_ARG_UI_FILES})
    endif()
    add_library("${PARENTDIR}" OBJECT ${UI_GENERATED_FILES} ${LIBRARY_ARG_INCLUDES} ${LIBRARY_ARG_SOURCES})
    source_group("${PARENTDIR}" FILES ${LIBRARY_ARG_INCLUDES} ${LIBRARY_ARG_SOURCES})
endfunction()
