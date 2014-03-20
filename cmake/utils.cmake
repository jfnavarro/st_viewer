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

    if(CMAKE_BUILD_TYPE MATCHES [Dd][Ee][Bb][Uu][Gg])
        message(STATUS "Building a debug version...")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG -DDEBUG")
        add_definitions(-DQT_DEBUG)
    else()
        message(STATUS "Building a release version...")
        add_definitions(-DQT_NO_DEBUG_OUTPUT)
        add_definitions(-DQT_NO_DEBUG)
    endif()

    if(WIN32)
        #TODO
    else()
        set(WARNING_ERROR "-Werror")
        set(DISABLED_WARNINGS "-Wno-float-equal -Wno-shadow -Wno-unreachable-code -Wno-switch-enum -Wno-type-limits")
        set(DISABLED_WARNINGS_DEBUG "-Wno-float-equal -Wno-shadow -Wno-unreachable-code -Wno-switch-enum -Wno-type-limits")
        #-Wdeprecated #this causes compilations error on MAC
        set(EXTRA_WARNINGS "-Woverloaded-virtual -Wundef -Wall -Wextra -Wformat-nonliteral -Wformat -Wunused-variable -Wreturn-type -Wempty-body -Wdisabled-optimization -W -Wredundant-decls -Wpacked -Wuninitialized -Wcast-align -Wcast-qual -Wswitch -Wsign-compare -pedantic-errors -fuse-cxa-atexit -ffor-scope")
        if(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
            set(EXTRA_WARNINGS "${EXTRA_WARNINGS} -Wold-style-cast -Wpedantic  -Weffc++ -Wnon-virtual-dtor -Wswitch-default -Wint-to-void-pointer-cast")
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

    find_package(CXXFeatures REQUIRED)
    # http://stackoverflow.com/questions/10984442/how-to-detect-c11-support-of-a-compiler-with-cmake/20165220#20165220
    set(needed_features
        #CXXFeatures_class_override_final ##not supported by Clang in OSX
        CXXFeatures_constexpr
        CXXFeatures_auto
        CXXFeatures_nullptr
        CXXFeatures_static_assert
        #CXXFeatures_initializer_list ##not supported by Clang in OSX
    )
    foreach(i ${needed_features})
      if(NOT ${i}_FOUND)
        message(FATAL_ERROR "CXX feature \"${i} is not supported by the compiler")
     endif()
   endforeach()

   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX11_COMPILER_FLAGS}")
   set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${CXX11_COMPILER_FLAGS}")
   set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${CXX11_COMPILER_FLAGS}")
  
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
        windows_deploy_library(${QT_BINARY_DIR}
        ${CMAKE_SHARED_LIBRARY_PREFIX}${LIBRARY}${CMAKE_SHARED_LIBRARY_SUFFIX} .)
    endforeach()
endmacro()

macro(WINDOWS_DEPLOY_QT_PLUGIN PLUGIN_CATEGORY)
    foreach(PLUGIN_NAME ${ARGN})
        windows_deploy_library(${QT_PLUGINS_DIR}/${PLUGIN_CATEGORY}
        ${CMAKE_SHARED_LIBRARY_PREFIX}${PLUGIN_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}
        plugins/${PLUGIN_CATEGORY})
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
    string(REGEX REPLACE "\.[0-9]+$" "" NEXTSTRING ${CURRENTSTRING})
  endwhile()
endfunction()
