# The macro FIX_QT3D_COMPILATION_BUG, is a hack fix to get rid of compile errors in Cygwin 
# when including some of the Qt3d headers.
# The compile error is probably caused by the confusion of having
# the interchangable representations of the same directory /cygdrive/c/tmp and C:\tmp
# Some how a ".." to much is specified in the include statements in the QMake generated files.
# When Qt3d gets merged into the standard Qt library we could remove this hack.
macro(FIX_QT3D_COMPILATION_BUG)
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dummy)
    include_directories(${CMAKE_CURRENT_BINARY_DIR}/dummy)
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dummy/dummy)
    include_directories(${CMAKE_CURRENT_BINARY_DIR}/dummy/dummy)
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dummy/dummy/dummy)
    include_directories(${CMAKE_CURRENT_BINARY_DIR}/dummy/dummy/dummy)
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dummy/dummy/dummy/dummy)
    include_directories(${CMAKE_CURRENT_BINARY_DIR}/dummy/dummy/dummy/dummy)
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

    if(CMAKE_BUILD_TYPE MATCHES [Dd][Ee][Bb][Uu][Gg])
        message(STATUS "Building a debug version...")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_DEBUG -DDEBUG")
        add_definitions(-DQT_DEBUG)
    else()
        message(STATUS "Building a release version...")
        add_definitions(-DQT_NO_DEBUG_OUTPUT)
        add_definitions(-DQT_NO_DEBUG)
    endif()

    if(WIN32)
        string(REPLACE "/W3" "/W3 /WX" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        set(LINK_FLAGS_PROPERTIES "/STACK:10000000 /MACHINE:X86")
        if(CMAKE_BUILD_TYPE MATCHES [Dd][Ee][Bb][Uu][Gg])
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_DEBUG /MDd /Zi /Ob0 /Od /RTC1")
            set(LINK_FLAGS_PROPERTIES "${LINK_FLAGS_PROPERTIES} /DEBUG")
        else()
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DNDEBUG /MD /O2 /Ob2")
        endif()
    else()
        # Adding -std=c++11 flag explicitly
        # It is a temporary fix to get building with CLANG working again.
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
        set(WARNING_ERROR "-Werror")
        set(DISABLED_WARNINGS "-Wno-missing-braces -Wno-float-equal -Wno-shadow -Wno-unreachable-code -Wno-switch-enum -Wno-type-limits -Wno-deprecated")
        set(EXTRA_WARNINGS "-Woverloaded-virtual -Wundef -Wall -Wextra -Wformat-nonliteral -Wformat -Wunused-variable -Wreturn-type -Wempty-body -Wdisabled-optimization -Wredundant-decls -Wpacked -Wuninitialized -Wcast-align -Wcast-qual -Wswitch -Wsign-compare -pedantic-errors -fuse-cxa-atexit -ffor-scope")
        if(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
            set(EXTRA_WARNINGS "${EXTRA_WARNINGS} -Wold-style-cast -Wpedantic  -Weffc++ -Wnon-virtual-dtor -Wswitch-default -Wint-to-void-pointer-cast")
        endif()
    endif()

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_WARNINGS} ${DISABLED_WARNINGS} ${WARNING_ERROR}")

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

macro(DEPLOY_QT_PLUGINS)
    foreach(PLUGIN_CATEGORY ${QT_PLUGIN_CATEGORIES})
        foreach(PLUGIN_NAME ${PLUGINS_IN_CATEGORY_${PLUGIN_CATEGORY}})
            if(WIN32) 
                windows_deploy_library(${QT_PLUGINS_DIR}/${PLUGIN_CATEGORY}
                ${CMAKE_SHARED_LIBRARY_PREFIX}${PLUGIN_NAME}${WINDOWS_EXTRA_PLUGIN_NAME_ENDING}${CMAKE_SHARED_LIBRARY_SUFFIX}
                plugins/${PLUGIN_CATEGORY})
            endif()
            if(NOT WIN32 AND NOT APPLE)
                install(FILES ${QT_PLUGINS_DIR}/${PLUGIN_CATEGORY}/${CMAKE_SHARED_LIBRARY_PREFIX}${PLUGIN_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}
                        DESTINATION plugins/${PLUGIN_CATEGORY})
            endif()
        endforeach()
    endforeach()
endmacro()

macro(WINDOWS_DEPLOY_QT_LIBRARIES)
    foreach(LIBRARY ${ARGN})
        windows_deploy_library(${QT_BINARY_DIR}
        ${CMAKE_SHARED_LIBRARY_PREFIX}${LIBRARY}${CMAKE_SHARED_LIBRARY_SUFFIX} .)
    endforeach()
endmacro()

macro(WINDOWS_DEPLOY_LIBRARY DIRNAME FILENAME DESTINATION)
    # Copy the library file to both the build and build/bin folders, so we can
    # test things without first having to deploy stVi
    copy_file_to_build_dir(${DIRNAME} ${DESTINATION} ${FILENAME})
    # Install the library file
    install(FILES ${DIRNAME}/${FILENAME} DESTINATION ${DESTINATION})
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

function(INSTALL_LIBRARY_AND_SYMLINKS SRCPATH DEST)
  # SRCPATH is the full path to the library
  # DEST is the destination (for instance "lib")
  # 
  # This function will help us to install a shared library together with the symlinks pointing to it.
  # For instance, to install the following library and its symbolic links into "lib"
  #
  # esjolund@zebra:~/code/st_client$ ls -l  ~/Qt5.2.1/5.2.1/gcc_64/lib/libQt5Core.so*
  # lrwxrwxrwx 1 esjolund esjolund      19 feb  6 13:45 /home/esjolund/Qt5.2.1/5.2.1/gcc_64/lib/libQt5Core.so -> libQt5Core.so.5.2.1
  # lrwxrwxrwx 1 esjolund esjolund      19 feb  6 13:45 /home/esjolund/Qt5.2.1/5.2.1/gcc_64/lib/libQt5Core.so.5 -> libQt5Core.so.5.2.1
  # lrwxrwxrwx 1 esjolund esjolund      19 feb  6 13:45 /home/esjolund/Qt5.2.1/5.2.1/gcc_64/lib/libQt5Core.so.5.2 -> libQt5Core.so.5.2.1
  # -rwxr-xr-x 1 esjolund esjolund 5030256 feb  6 13:45 /home/esjolund/Qt5.2.1/5.2.1/gcc_64/lib/libQt5Core.so.5.2.1
  #
  # we would call the function like this:
  # INSTALL_LIBRARY_AND_SYMLINKS(/home/esjolund/Qt5.2.1/5.2.1/gcc_64/lib/libQt5Core.so.5.2.1 lib)
  #
  set(CURRENTSTRING "dummyvalue")
  set(NEXTSTRING "${SRCPATH}")
  while(NOT ${NEXTSTRING} STREQUAL ${CURRENTSTRING})
    set(CURRENTSTRING "${NEXTSTRING}")
    if(EXISTS "${CURRENTSTRING}")
      install(FILES ${CURRENTSTRING}
              DESTINATION ${DEST})
    endif()
    # The REGEX tries to remove a version number from the end of the string
    string(REGEX REPLACE "\\.[0-9]+$" "" NEXTSTRING "${CURRENTSTRING}")
  endwhile()
endfunction()
