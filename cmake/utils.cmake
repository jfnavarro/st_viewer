MACRO(INITIALISE_PROJECT)

#    SET(CMAKE_VERBOSE_MAKEFILE ON)
    SET(CMAKE_INCLUDE_CURRENT_DIR ON)

    # Required packages
    find_package(Qt5Widgets REQUIRED)
    # Keep track of some information about Qt
    SET(QT_BINARY_DIR ${_qt5Widgets_install_prefix}/bin)
    SET(QT_LIBRARY_DIR ${_qt5Widgets_install_prefix}/lib)
    SET(QT_PLUGINS_DIR ${_qt5Widgets_install_prefix}/plugins)
    SET(QT_VERSION_MAJOR ${Qt5Widgets_VERSION_MAJOR})
    SET(QT_VERSION_MINOR ${Qt5Widgets_VERSION_MINOR})
    SET(QT_VERSION_PATCH ${Qt5Widgets_VERSION_PATCH})

    # Some settings which depend on whether we want a debug or release version
    # of stVi
    IF(WIN32)
        STRING(REPLACE "/W3" "/W3 /WX" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        SET(LINK_FLAGS_PROPERTIES "/STACK:10000000 /MACHINE:X86")
    ENDIF()

    IF(CMAKE_BUILD_TYPE MATCHES [Dd][Ee][Bb][Uu][Gg])
        MESSAGE("Building a debug version...")
        set(DEBUG_MODE ON)
        # Default compiler settings
        IF(WIN32)
            SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_DEBUG /MDd /Zi /Ob0 /Od /RTC1")
            SET(LINK_FLAGS_PROPERTIES "${LINK_FLAGS_PROPERTIES} /DEBUG")
        ELSE()
            SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
        ENDIF()
        # Make sure that debugging is on for Qt
        ADD_DEFINITIONS(-DQT_DEBUG)
    ELSE()
        MESSAGE("Building a release version...")
        set(DEBUG_MODE OFF)
        # Default compiler and linker settings
        IF(WIN32)
            SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DNDEBUG /MD /O2 /Ob2")
        ELSE()
            SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -ffast-math")
        ENDIF()
#         IF(NOT WIN32 AND NOT APPLE)
#             SET(LINK_FLAGS_PROPERTIES "${LINK_FLAGS_PROPERTIES} -Wl,-s")
#             # Note #1: -Wl,-s strips all the symbols, thus reducing the final
#             #          size of stVi or one its shared libraries...
#             # Note #2: the above linking option has become obsolete on OS X,
#             #          so...
#         ENDIF()
        # Make sure that debugging is off for Qt
        ADD_DEFINITIONS(-DQT_NO_DEBUG_OUTPUT)
        ADD_DEFINITIONS(-DQT_NO_DEBUG)
    ENDIF()

    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG -DDEBUG")
    SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG -DDEBUG")

    # Reduce the number of warnings
    # Remove "warning: multi-character character constant"
    OPTION(TREAT_WARNINGS_AS_ERRORS "Treat warnings as errors" ON)

    IF(WIN32)
        #TODO
    ELSE()
        IF(TREAT_WARNINGS_AS_ERRORS)
            SET(WARNING_ERROR "-Werror")
        ENDIF(TREAT_WARNINGS_AS_ERRORS)
#         SET(POSITION_INDEPENDENT_CODE "-fPIC")
        SET(DISABLED_WARNINGS "-Wno-multichar -Wno-unused-variable -Wno-unused-function -Wno-return-type -Wno-switch")
        SET(DISABLED_WARNINGS_DEBUG "-Wno-unused-variable -Wno-unused-function")
#         SET(PRIVATE_SYMBOLS "-fvisibility=hidden -fvisibility-inlines-hidden")
    ENDIF()

    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${POSITION_INDEPENDENT_CODE} ${DISABLED_WARNINGS} ${WARNING_ERROR}")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${DISABLED_WARNINGS} ${WARNING_ERROR}")
    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${DISABLED_WARNINGS_DEBUG}")
    SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${DISABLED_WARNINGS_DEBUG}")
    SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${PRIVATE_SYMBOLS}")
    SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${PRIVATE_SYMBOLS}")

    # Ask for Unicode to be used
    ADD_DEFINITIONS(-DUNICODE)

    IF(WIN32)
        ADD_DEFINITIONS(-D_UNICODE)
    ENDIF()

    # Set the RPATH information on Linux
    # Note: this prevent us from having to use the uncool LD_LIBRARY_PATH...
    IF(NOT WIN32 AND NOT APPLE)
        SET(CMAKE_INSTALL_RPATH "$ORIGIN/../lib:$ORIGIN/../plugins/${PROJECT_NAME}")
    ENDIF()
    
ENDMACRO()

MACRO(use_qt5lib qt5lib)
    find_package(${qt5lib} REQUIRED)
    include_directories(${${qt5lib}_INCLUDE_DIRS})
    add_definitions(${${qt5lib}_DEFINITIONS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${${qt5lib}_EXECUTABLE_COMPILE_FLAGS}")
ENDMACRO()

MACRO(COPY_FILE_TO_BUILD_DIR ORIG_DIRNAME DEST_DIRNAME FILENAME)

    IF(EXISTS ${CMAKE_BINARY_DIR}/../cmake)
        # A CMake directory exists at the same level as the binary directory,
        # so we are dealing with the main project

        SET(REAL_DEST_DIRNAME ${CMAKE_BINARY_DIR}/${DEST_DIRNAME})
    ELSE()
        # No CMake directory exists at the same level as the binary directory,
        # so we are dealing with a non-main project
        if(WIN32)
            SET(REAL_DEST_DIRNAME ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${DEST_DIRNAME})
        else()
            SET(REAL_DEST_DIRNAME ${CMAKE_BINARY_DIR}/${DEST_DIRNAME})
        endif()
    ENDIF()

    IF("${ARGN}" STREQUAL "")
        ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} POST_BUILD
                           COMMAND ${CMAKE_COMMAND} -E copy ${ORIG_DIRNAME}/${FILENAME}
                                                            ${REAL_DEST_DIRNAME}/${FILENAME})
    ELSE()
        # An argument was passed so use it to rename the file which is to be
        # copied
        ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} POST_BUILD
                           COMMAND ${CMAKE_COMMAND} -E copy ${ORIG_DIRNAME}/${FILENAME}
                                                            ${REAL_DEST_DIRNAME}/${ARGN})
    ENDIF()
ENDMACRO()

MACRO(WINDOWS_DEPLOY_QT_LIBRARIES)
    FOREACH(LIBRARY ${ARGN})
        # Deploy the Qt library itself
        #INSTALL(FILES ${QT_BINARY_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}${LIBRARY}${CMAKE_SHARED_LIBRARY_SUFFIX} DESTINATION .)
        WINDOWS_DEPLOY_LIBRARY(${QT_BINARY_DIR} ${CMAKE_SHARED_LIBRARY_PREFIX}${LIBRARY}${CMAKE_SHARED_LIBRARY_SUFFIX} .)
    ENDFOREACH()
ENDMACRO()

MACRO(WINDOWS_DEPLOY_QT_PLUGIN PLUGIN_CATEGORY)
    FOREACH(PLUGIN_NAME ${ARGN})
        # Deploy the Qt plugin itself
        WINDOWS_DEPLOY_LIBRARY(${QT_PLUGINS_DIR}/${PLUGIN_CATEGORY} ${CMAKE_SHARED_LIBRARY_PREFIX}${PLUGIN_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX} plugins/${PLUGIN_CATEGORY})
        #INSTALL(FILES ${QT_PLUGINS_DIR}/${PLUGIN_CATEGORY}/${CMAKE_SHARED_LIBRARY_PREFIX}${PLUGIN_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}
        #        DESTINATION plugins/${PLUGIN_CATEGORY})
    ENDFOREACH()
ENDMACRO()

MACRO(WINDOWS_DEPLOY_LIBRARY DIRNAME FILENAME DESTINATION)
    # Copy the library file to both the build and build/bin folders, so we can
    # test things without first having to deploy stVi
    COPY_FILE_TO_BUILD_DIR(${DIRNAME} ${DESTINATION} ${FILENAME})
    # Install the library file
    INSTALL(FILES ${DIRNAME}/${FILENAME}
            DESTINATION ${DESTINATION})
ENDMACRO()

MACRO(LINUX_DEPLOY_QT_PLUGIN PLUGIN_CATEGORY)
    FOREACH(PLUGIN_NAME ${ARGN})
        # Deploy the Qt plugin itself
        INSTALL(FILES ${QT_PLUGINS_DIR}/${PLUGIN_CATEGORY}/${CMAKE_SHARED_LIBRARY_PREFIX}${PLUGIN_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}
                DESTINATION plugins/${PLUGIN_CATEGORY})
    ENDFOREACH()
ENDMACRO()

# MACRO(LINUX_DEPLOY_LIBRARY DIRNAME FILENAME)
#     # Copy the library file to the build folder, so we can test things without
#     # first having to deploy stVi
#     COPY_FILE_TO_BUILD_DIR(${DIRNAME} . ${FILENAME})
#     # Install the library file
#     INSTALL(FILES ${DIRNAME}/${FILENAME} DESTINATION lib)
# ENDMACRO()

# MACRO(OS_X_QT_DEPENDENCIES FILENAME QT_DEPENDENCIES)
#     # Retrieve the file's full-path Qt dependencies as a list
# 
#     SET(QT_LIBRARY_DIR_FOR_GREP "\t${QT_LIBRARY_DIR}/")
# 
#     EXECUTE_PROCESS(COMMAND otool -L ${FILENAME}
#                     COMMAND grep ${QT_LIBRARY_DIR_FOR_GREP}
#                     OUTPUT_VARIABLE RAW_QT_DEPENDENCIES)
# 
#     STRING(REPLACE "\n" ";" RAW_QT_DEPENDENCIES "${RAW_QT_DEPENDENCIES}")
# 
#     # Extract and return the Qt depencies as a list
#     SET(${QT_DEPENDENCIES})
#     FOREACH(RAW_QT_DEPENDENCY ${RAW_QT_DEPENDENCIES})
#         STRING(REPLACE ${QT_LIBRARY_DIR_FOR_GREP} "" RAW_QT_DEPENDENCY "${RAW_QT_DEPENDENCY}")
#         STRING(REGEX REPLACE "\\.framework.*$" "" QT_DEPENDENCY "${RAW_QT_DEPENDENCY}")
#         LIST(APPEND ${QT_DEPENDENCIES} ${QT_DEPENDENCY})
#     ENDFOREACH()
# ENDMACRO()

# MACRO(OS_X_CLEAN_UP_FILE_WITH_QT_DEPENDENCIES DIRNAME FILENAME)
#     # Strip the Qt file of all local symbols
#     SET(FULL_FILENAME ${DIRNAME}/${FILENAME})
#     IF(NOT DEBUG_MODE)
#         ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} POST_BUILD
#                            COMMAND strip -x ${FULL_FILENAME})
#     ENDIF()
# 
#     # Clean up the Qt file's id
#     ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} POST_BUILD
#                        COMMAND install_name_tool -id ${FILENAME}
#                                                      ${FULL_FILENAME})
#     # Make sure that the Qt file refers to our embedded version of its Qt
#     # dependencies
#     FOREACH(DEPENDENCY ${ARGN})
#         SET(DEPENDENCY_FILENAME ${DEPENDENCY}.framework/Versions/${QT_VERSION_MAJOR}/${DEPENDENCY})
#         ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} POST_BUILD
#                            COMMAND install_name_tool -change ${QT_LIBRARY_DIR}/${DEPENDENCY_FILENAME}
#                                                              @executable_path/../Frameworks/${DEPENDENCY_FILENAME}
#                                                              ${FULL_FILENAME})
#     ENDFOREACH()
# ENDMACRO()

# MACRO(OS_X_DEPLOY_QT_FILE ORIG_DIRNAME DEST_DIRNAME FILENAME)
#     # Copy the Qt file itself
#     SET(ORIG_FILENAME ${ORIG_DIRNAME}/${FILENAME})
#     ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} POST_BUILD
#                        COMMAND ${CMAKE_COMMAND} -E copy ${ORIG_FILENAME}
#                                                         ${DEST_DIRNAME}/${FILENAME})
#     # Retrieve the Qt file's Qt dependencies
#     OS_X_QT_DEPENDENCIES(${ORIG_FILENAME} DEPENDENCIES)
#     # Clean up the Qt file
#     OS_X_CLEAN_UP_FILE_WITH_QT_DEPENDENCIES(${DEST_DIRNAME} ${FILENAME} ${DEPENDENCIES})
# ENDMACRO()

# MACRO(OS_X_DEPLOY_QT_LIBRARIES)
#     FOREACH(LIBRARY_NAME ${ARGN})
#         # Deploy the Qt library itself
#         SET(QT_FRAMEWORK_DIR ${LIBRARY_NAME}.framework/Versions/${QT_VERSION_MAJOR})
# 
#         OS_X_DEPLOY_QT_FILE(${QT_LIBRARY_DIR}/${QT_FRAMEWORK_DIR}
#                             ${OS_X_PROJECT_BINARY_DIR}/Contents/Frameworks/${QT_FRAMEWORK_DIR}
#                             ${LIBRARY_NAME})
#     ENDFOREACH()
# ENDMACRO()

# MACRO(OS_X_DEPLOY_QT_PLUGIN PLUGIN_CATEGORY)
#     FOREACH(PLUGIN_NAME ${ARGN})
#         # Deploy the Qt plugin itself
#         OS_X_DEPLOY_QT_FILE(${QT_PLUGINS_DIR}/${PLUGIN_CATEGORY}
#                             ${OS_X_PROJECT_BINARY_DIR}/Contents/PlugIns/${PLUGIN_CATEGORY}
#                             ${CMAKE_SHARED_LIBRARY_PREFIX}${PLUGIN_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX})
#     ENDFOREACH()
# ENDMACRO()

# MACRO(OS_X_DEPLOY_LIBRARY DIRNAME LIBRARY_NAME)
#     # Copy the library itself
#     SET(LIBRARY_FILENAME ${CMAKE_SHARED_LIBRARY_PREFIX}${LIBRARY_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX})
#     SET(LIBRARY_FILEPATH ${OS_X_PROJECT_BINARY_DIR}/Contents/Frameworks/${LIBRARY_FILENAME})
#     ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} POST_BUILD
#                        COMMAND ${CMAKE_COMMAND} -E copy ${DIRNAME}/${LIBRARY_FILENAME}
#                                                         ${LIBRARY_FILEPATH})
#     # Copy the library to the build directory, so that we can test any plugin
#     # that has a dependency on it
#     ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} POST_BUILD
#                        COMMAND ${CMAKE_COMMAND} -E copy ${DIRNAME}/${LIBRARY_FILENAME}
#                                                         ${CMAKE_BINARY_DIR}/${LIBRARY_FILENAME})
#     # Strip the library of all local symbols
#     IF(NOT DEBUG_MODE)
#         ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} POST_BUILD
#                            COMMAND strip -x ${LIBRARY_FILEPATH})
#     ENDIF()
#     # Make sure that the library refers to our embedded version of the libraries
#     # on which it depends
#     FOREACH(DEPENDENCY_NAME ${ARGN})
#         SET(DEPENDENCY_FILENAME ${CMAKE_SHARED_LIBRARY_PREFIX}${DEPENDENCY_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX})
# 
#         ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} POST_BUILD
#                            COMMAND install_name_tool -change ${DEPENDENCY_FILENAME}
#                                                              @executable_path/../Frameworks/${DEPENDENCY_FILENAME}
#                                                              ${LIBRARY_FILEPATH})
#     ENDFOREACH()
# ENDMACRO()

MACRO(PROJECT_GROUP TARGET_NAME FOLDER_PATH)
    #Organize projects into folders
    SET_PROPERTY(TARGET ${TARGET_NAME} PROPERTY FOLDER ${FOLDER_PATH})
ENDMACRO()
