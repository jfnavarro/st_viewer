macro(INITIALISE_PROJECT)

#    set(CMAKE_VERBOSE_MAKEFILE ON)
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
    
    # Some settings which depend on whether we want a debug or release version
    # of stVi
    if(WIN32)
        string(REPLACE "/W3" "/W3 /WX" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        set(LINK_FLAGS_PROPERTIES "/STACK:10000000 /MACHINE:X86")
    endif()

    if(CMAKE_BUILD_TYPE MATCHES [Dd][Ee][Bb][Uu][Gg])
        message("Building a debug version...")
        # Default compiler settings
        if(WIN32)
            set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /D_DEBUG /MDd /Zi /Ob0 /Od /RTC1")
            set(LINK_FLAGS_PROPERTIES "${LINK_FLAGS_PROPERTIES} /DEBUG")
        else()
            set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
        endif()
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG -DDEBUG")
        # Make sure that debugging is on for Qt
        add_definitions(-DQT_DEBUG)
    else()
        message("Building a release version...")
        # Default compiler and linker settings
        if(WIN32)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DNDEBUG /MD /O2 /Ob2")
        else()
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -ffast-math")
        endif()
        # Make sure that debugging is off for Qt
        add_definitions(-DQT_NO_DEBUG_OUTPUT)
        add_definitions(-DQT_NO_DEBUG)
    endif()

    # Reduce the number of warnings
    # Remove "warning: multi-character character constant"
    option(TREAT_WARNINGS_AS_ERRORS "Treat warnings as errors" ON)

    if(WIN32)
        #TODO
    else()
        if(TREAT_WARNINGS_AS_ERRORS)
            set(WARNING_ERROR "-Werror")
        endif(TREAT_WARNINGS_AS_ERRORS)
        set(DISABLED_WARNINGS "-Wno-float-equal -Wno-shadow -Wno-unreachable-code -Wno-switch-enum -Wno-type-limits")
        set(DISABLED_WARNINGS_DEBUG "-Wno-float-equal -Wno-shadow -Wno-unreachable-code -Wno-switch-enum -Wno-type-limits")
        set(EXTRA_WARNINGS "-Woverloaded-virtual -Wundef -Wall -Wextra -Wformat-nonliteral -Wformat -Wunused-variable -Wreturn-type -Wempty-body -Wdeprecated -Wdisabled-optimization -Winline -Wredundant-decls -Wpacked -Wuninitialized -Wcast-align -Wcast-qual -Wswitch -Wsign-compare -pedantic-errors -fuse-cxa-atexit -ffor-scope")
        if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            set(EXTRA_WARNINGS "${EXTRA_WARNINGS} -Wold-style-cast -Wpedantic  -Weffc++ -Wnon-virtual-dtor -Wswitch-default") # -Wint-to-void-pointer-cast
        endif()
    endif()

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_WARNINGS} ${DISABLED_WARNINGS} ${WARNING_ERROR}")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${EXTRA_WARNINGS} ${DISABLED_WARNINGS_DEBUG} ${WARNING_ERROR}")

    # Ask for Unicode to be used
    add_definitions(-DUNICODE)

    if(WIN32)
        add_definitions(-D_UNICODE)
    endif()

    # Set the RPATH information on Linux
    # Note: this prevent us from having to use the uncool LD_LIBRARY_PATH...
    if(NOT WIN32 AND NOT APPLE)
        set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib:$ORIGIN/../plugins/${PROJECT_NAME}")
    endif()
    
    #enable c++11
    check_for_cxx11_compiler(CXX11_COMPILER)

    # If a C++11 compiler is available, then set the appropriate flags
    if(CXX11_COMPILER)
        enable_cxx11()
    else()
        message(FATAL_ERROR "Your compiler does not support c++11, UPDATE!!")
    endif()
    
    if(APPLE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mmacosx-version-min=10.7 -stdlib=libc++")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -mmacosx-version-min=10.7 -stdlib=libc++")       
    endif()
    
endmacro()

macro(use_qt5lib qt5lib)
    find_package(${qt5lib} REQUIRED)
    include_directories(${${qt5lib}_INCLUDE_DIRS})
    add_definitions(${${qt5lib}_DEFINITIONS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${${qt5lib}_EXECUTABLE_COMPILE_FLAGS}")
endmacro()

macro(COPY_FILE_TO_BUILD_DIR ORIG_DIRNAME DEST_DIRNAME FILENAME)
    if(EXISTS ${CMAKE_BINARY_DIR}/../cmake)
        set(REAL_DEST_DIRNAME ${CMAKE_BINARY_DIR}/${DEST_DIRNAME})
    else()
        if(WIN32)
            set(REAL_DEST_DIRNAME ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${DEST_DIRNAME})
        else()
            set(REAL_DEST_DIRNAME ${CMAKE_BINARY_DIR}/${DEST_DIRNAME})
        endif()
    endif()
    if("${ARGN}" STREQUAL "")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                           COMMAND ${CMAKE_COMMAND} -E copy ${ORIG_DIRNAME}/${FILENAME}
                                                            ${REAL_DEST_DIRNAME}/${FILENAME})
    else()
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                           COMMAND ${CMAKE_COMMAND} -E copy ${ORIG_DIRNAME}/${FILENAME}
                                                            ${REAL_DEST_DIRNAME}/${ARGN})
    endif()
endmacro()

macro(WINDOWS_DEPLOY_QT_LIBRARIES)
    foreach(LIBRARY ${ARGN})
        windows_deploy_library(${QT_BINARY_DIR} ${CMAKE_SHARED_LIBRARY_PREFIX}${LIBRARY}${CMAKE_SHARED_LIBRARY_SUFFIX} .)
    endforeach()
endmacro()

macro(WINDOWS_DEPLOY_QT_PLUGIN PLUGIN_CATEGORY)
    foreach(PLUGIN_NAME ${ARGN})
        windows_deploy_library(${QT_PLUGINS_DIR}/${PLUGIN_CATEGORY} ${CMAKE_SHARED_LIBRARY_PREFIX}${PLUGIN_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX} plugins/${PLUGIN_CATEGORY})
    endforeach()
endmacro()

macro(WINDOWS_DEPLOY_LIBRARY DIRNAME FILENAME DESTINATION)
    # Copy the library file to both the build and build/bin folders, so we can
    # test things without first having to deploy stVi
    copy_file_to_build_dir(${DIRNAME} ${DESTINATION} ${FILENAME})
    # Install the library file
    install(FILES ${DIRNAME}/${FILENAME} DESTINATION ${DESTINATION})
endmacro()

macro(LINUX_DEPLOY_QT_PLUGIN PLUGIN_CATEGORY)
    foreach(PLUGIN_NAME ${ARGN})
        install(FILES ${QT_PLUGINS_DIR}/${PLUGIN_CATEGORY}/${CMAKE_SHARED_LIBRARY_PREFIX}${PLUGIN_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}
                DESTINATION plugins/${PLUGIN_CATEGORY})
    endforeach()
endmacro()

macro(PROJECT_GROUP TARGET_NAME FOLDER_PATH)
    #Organize projects into folders
    set_property(TARGET ${TARGET_NAME} PROPERTY FOLDER ${FOLDER_PATH})
endmacro()

# Determines whether or not the compiler supports C++11
macro(check_for_cxx11_compiler _VAR)
    message(STATUS "Checking for C++11 compiler")
    set(${_VAR})
    if((MSVC AND MSVC10) OR
       (CMAKE_COMPILER_IS_GNUCXX AND NOT ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 4.6) OR
       (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND NOT ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 3.1))
        set(${_VAR} 1)
        message(STATUS "Checking for C++11 compiler - available")
    else()
        message(STATUS "Checking for C++11 compiler - unavailable")
    endif()
endmacro()

# Sets the appropriate flag to enable C++11 support
macro(enable_cxx11)
    if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++0x")
    endif()
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
