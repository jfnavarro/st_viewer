macro(INITIALISE_PROJECT)
    set(CMAKE_VERBOSE_MAKEFILE OFF)

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

    set(CMAKE_CXX_STANDARD 14)

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
        # Adding -std=c++14 flag explicitly
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

        # Enable warning errors
        set(WARNING_ERROR "-Werror")

        set(DISABLED_WARNINGS "-Wno-unused-parameter -Wno-unused-function \
            -Wno-sign-compare -Wno-cast-qual -Wno-cast-align")

        if (APPLE)
            set(DISABLED_WARNINGS "${DISABLED_WARNINGS} -Wno-pessimizing-move \
                -Wno-inconsistent-missing-override -Wno-infinite-recursion \
                -Wno-c++1z-extensions -Wno-macro-redefined")
        endif()

        set(EXTRA_WARNINGS "-Woverloaded-virtual -Wundef -Wall -Wextra \
                           -Wformat -Wunused-variable -Wreturn-type -Wempty-body -Wdisabled-optimization \
                           -Wredundant-decls -Wpacked -Wuninitialized -Wswitch \
                           -pedantic-errors -fuse-cxa-atexit -ffor-scope")
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

macro(PROJECT_GROUP TARGET_NAME FOLDER_PATH)
    # Organize projects into folders
    set_property(TARGET ${TARGET_NAME} PROPERTY FOLDER ${FOLDER_PATH})
endmacro()

function(ST_LIBRARY)
    get_filename_component(PARENTDIR ${CMAKE_CURRENT_SOURCE_DIR} NAME)
    add_library("${PARENTDIR}" OBJECT ${UI_GENERATED_FILES} ${LIBRARY_ARG_INCLUDES} ${LIBRARY_ARG_SOURCES})
    source_group("${PARENTDIR}" FILES ${LIBRARY_ARG_INCLUDES} ${LIBRARY_ARG_SOURCES})
endfunction()
