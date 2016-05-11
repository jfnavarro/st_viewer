### PROJECT AND GLOBAL VARIABLES ##############################################

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Load resource and translations files
set(LOCALES "${PROJECT_SOURCE_DIR}/assets/trans/locale_en_us.ts") 
set(RESOURCES "${PROJECT_BINARY_DIR}/application.qrc")
qt5_add_resources(QT_RESOURCES ${RESOURCES})
qt5_add_translation(QT_LOCALES ${LOCALES})

# Define what sub folders will be compiled
set(subdir_list
                   dialogs
                   color
                   concurrent
                   dataModel
                   network
                   auth
                   data
                   customWidgets
                   viewTables
                   viewPages
                   viewOpenGL
                   utils
                   error
                   io
                   model
                   config
                   math
                   analysis)

foreach(dir ${subdir_list})
    add_subdirectory("${dir}")
endforeach()

### PROJECT AND SOURCE STRUCTURE ##############################################

# NOTE this is very useful for Visual Studio
# Organize projects into folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

###PACKAGES######################################

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
mark_as_advanced(${OPENGL_LIBRARY})

# TODO Maybe we should do this for OSX and Linux as well
if (WIN32)
    find_package(OpenSSL REQUIRED)
    include_directories(${OPENSSL_INCLUDE_DIR})
    mark_as_advanced(${OPENSSL_LIBRARIES})
endif(WIN32)

##FLAGS AND DEFINITIONS###

add_definitions(-DPROJECT_VERSION=\"${PROJECT_VERSION}\")

##BUILDING AND INSTALLING #############################################################

# Put everything that is generated into the binary dir
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_BINARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

###STATIC LIBRARIES#####################################

# Link st viewer libraries and third party libraries
foreach(dir ${subdir_list})
    list(APPEND ST_TARGET_OBJECTS "$<TARGET_OBJECTS:${dir}>")
endforeach()
list(APPEND ST_TARGET_OBJECTS "$<TARGET_OBJECTS:qcustomplot>")
list(APPEND ST_TARGET_OBJECTS "$<TARGET_OBJECTS:qtwaitingspinner>")
# 'rapidjson' does not need to be compiled, we just need to include the headers
include_directories(${PROJECT_SOURCE_DIR}/ext/rapidjson)

set(QT_TARGET_LINK_LIBS Qt5::Core
                             Qt5::Network
                             Qt5::Gui
                             Qt5::Widgets
                             Qt5::PrintSupport
                             Qt5::OpenGL
                             Qt5::Concurrent)

# Create main target (resources needs to be part of the target)
set(ST_CLIENT_SOURCES main.cpp mainWindow.cpp
    SettingsNetwork.h SettingsStyle.h SettingsVisual.h ${ST_TARGET_OBJECTS} ${QT_RESOURCES})

if(WIN32)
    add_executable(${PROJECT_NAME} WIN32 ${ST_CLIENT_SOURCES})
elseif(APPLE)
    add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${ST_CLIENT_SOURCES})
else()
    add_executable(${PROJECT_NAME} ${ST_CLIENT_SOURCES})
endif()

# The Qt5 IMPORTED targets for the QtGui libraries specify that the qtmain.lib static
# library shipped with Qt will be linked by all dependent executables which have the WIN32_EXECUTABLE enabled.
#set_property(TARGET ${PROJECT_NAME} PROPERTY Qt5_NO_LINK_QTMAIN ON)

if(MSVC)
    # A dependency is linking to the non debug version. This causes an annoying 
    # warning in Visual Studio debug builds.    
    string( TOLOWER "${CMAKE_BUILD_TYPE}" BUILD_TYPE_LOWERCASE )
    if(BUILD_TYPE_LOWERCASE STREQUAL "debug")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:libcmt")
    endif()
endif()

# Link libraries to the main target
target_link_libraries(${PROJECT_NAME} ${QT_TARGET_LINK_LIBS} ${OPENSSL_LIBRARIES} ${OPENGL_LIBRARY})

### UNIT TESTS ################################################################

enable_testing()
add_subdirectory(test)

############################INSTALLATION#########################################

SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# PACKAGING OPTIONS: GENERAL
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_VENDOR "${PROJECT_VENDOR}")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")

# Add systems libraries
set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP true)
include(InstallRequiredSystemLibraries)

# Qt plug-ins needed
set(QT_PLUGIN_CATEGORIES accessible imageformats printsupport platforms)
set(PLUGINS_IN_CATEGORY_accessible)
set(PLUGINS_IN_CATEGORY_imageformats qgif qico qjpeg qmng qsvg qtga qtiff qwbmp)
set(PLUGINS_IN_CATEGORY_printsupport)
set(PLUGINS_IN_CATEGORY_platforms)

if(WIN32)
    list(APPEND PLUGINS_IN_CATEGORY_printsupport windowsprintersupport)
endif()

if(UNIX AND NOT APPLE)
    list(APPEND PLUGINS_IN_CATEGORY_printsupport cupsprintersupport)
    list(APPEND PLUGINS_IN_CATEGORY_platforms qlinuxfb qminimal qxcb)
endif()

if(UNIX AND NOT APPLE)

    # Use, i.e. don't skip the full RPATH for the build tree
    set(CMAKE_SKIP_BUILD_RPATH FALSE)

    # When building, don't use the install RPATH already
    # (but later on when installing)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib;${CMAKE_INSTALL_PREFIX}/plugins")

    # Add the automatically determined parts of the RPATH
    # which point to directories outside the build tree to the install RPATH
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

    # The RPATH to be used when installing, but only if it's not a system directory
    list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
    if("${isSystemDir}" STREQUAL "-1")
        set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
    endif("${isSystemDir}" STREQUAL "-1")

    # Select TGZ as the packager on Linux
    set(CPACK_GENERATOR "TGZ")
    set(CPACK_SYSTEM_NAME "Linux")
    set(CPACK_SYSTEM_NAME "${CPACK_SYSTEM_NAME}")

    # Add Qt libraries as shared libraries
    foreach(QT_LIBRARY ${ST_QT_LIBS})
       # note there is also the targets: LOCATION_RELEASE and LOCATION_DEBUG
       # but I think we don't need them /Erik Sjolund
       get_target_property(ST_QT_LOC "Qt5::${QT_LIBRARY}" LOCATION)
       INSTALL_LIBRARY_AND_SYMLINKS("${ST_QT_LOC}" lib)
     endforeach()

    # Qt 5.5 needs ICU libaries in Linux 54
    set(ICU_VERSION "54.1")
    foreach(ICU_LIB icuuc icui18n icudata)
      find_library(loc${ICU_LIB} lib${ICU_LIB}.so.${ICU_VERSION} NO_CMAKE_SYSTEM_PATH)
      if (NOT loc${ICU_LIB})
        message(FATAL_ERROR "Could not find lib${ICU_LIB}.so.${ICU_VERSION}")
      endif()
      INSTALL_LIBRARY_AND_SYMLINKS("${loc${ICU_LIB}}" lib)
    endforeach()

    deploy_qt_plugins()

    # Install libraries and resources
    install(FILES ${CMAKE_SOURCE_DIR}/cmake/linux/qt.conf DESTINATION ${CMAKE_INSTALL_PREFIX})
    install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX})
    install(FILES ${CMAKE_SOURCE_DIR}/LICENSE DESTINATION ${CMAKE_INSTALL_PREFIX})
    install(FILES ${CMAKE_SOURCE_DIR}/DEPENDENCIES DESTINATION ${CMAKE_INSTALL_PREFIX})
    install(FILES ${CMAKE_SOURCE_DIR}/REPLACE_QT_LIBRARIES DESTINATION ${CMAKE_INSTALL_PREFIX})

endif()

if(APPLE)

    # Set project name of binary dir for OSX
    set(OS_X_PROJECT_BINARY_DIR "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.app")
    set(CMAKE_INSTALL_PREFIX ${OS_X_PROJECT_BINARY_DIR}/Contents)
    set(DMG_DIR ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

    # Configure and install Info.plist file
    set(InfoPlistFile ${CMAKE_BINARY_DIR}/Info.plist)
    execute_process(COMMAND date "+%Y"
                    OUTPUT_VARIABLE YEAR
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
    configure_file(${CMAKE_SOURCE_DIR}/cmake/mac/Info.plist.in ${InfoPlistFile})
    set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${InfoPlistFile})

    # MAC OSX sets
    set(MACOSX_BUNDLE_ICON_FILE "${CMAKE_SOURCE_DIR}/assets/images/STViewer.icns")
    set(MACOSX_BUNDLE_INFO_STRING "Spatial Transcriptomics Viewer")
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "name.elemc.${PROJECT_NAME}")
    set(MACOSX_BUNDLE_VERSION "${PROJECT_VERSION}")
    set(MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}")
    set(CPACK_BUNDLE_PLIST ${InfoPlistFile})

    # Architecture OSX
    set(CMAKE_OSX_ARCHITECTURES "ppc;i386")
    set(CMAKE_CXX_FLAGS "--sysroot ${CMAKE_OSX_SYSROOT} ${CMAKE_CXX_FLAGS}")

    # Find macdeoployqt
    find_program(MACDEPLOYQT NAMES macdeployqt macdeployqt-mac PATHS ${QT_BINARY_DIR} NO_DEFAULT_PATH)

    # Resources
    add_custom_command (TARGET ${PROJECT_NAME}
        COMMAND mkdir -p ${CMAKE_INSTALL_PREFIX}/Resources
        COMMAND install -m 644 ${CMAKE_SOURCE_DIR}/assets/images/st_icon.icns ${CMAKE_INSTALL_PREFIX}/Resources/
        COMMAND install -m 644 ${CMAKE_SOURCE_DIR}/cmake/mac/qt.conf ${CMAKE_INSTALL_PREFIX}/Resources/
        COMMAND ${MACDEPLOYQT} ${OS_X_PROJECT_BINARY_DIR}
        )

    # Build dmg
    add_custom_target(dmg
        COMMAND mkdir -p ${DMG_DIR}
        COMMAND cp -R ${OS_X_PROJECT_BINARY_DIR} ${DMG_DIR}/
        COMMAND ln -sf /Applications ${DMG_DIR}
        COMMAND cp ${CMAKE_SOURCE_DIR}/assets/images/st_icon.icns .VolumeIcon.icns
        COMMAND cp ${CMAKE_SOURCE_DIR}/LICENSE ${DMG_DIR}
        COMMAND cp ${CMAKE_SOURCE_DIR}/DEPENDENCIES ${DMG_DIR}
        COMMAND cp ${CMAKE_SOURCE_DIR}/REPLACE_QT_LIBRARIES ${DMG_DIR}
        COMMAND hdiutil create ${CMAKE_BINARY_DIR}/${PROJECT_NAME}-${CMAKE_BUILD_TYPE}-${PROJECT_VERSION}.dmg -srcfolder ${DMG_DIR}
        DEPENDS ${PROJECT_NAME}
        COMMENT "Create DMG image"
    )

endif(APPLE)

if(WIN32)

    # By default, do not warn when built on machines using only VS Express:
    if(NOT DEFINED CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_NO_WARNINGS)
      set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_NO_WARNINGS ON)
    endif()

    # Install system libs
    install(FILES ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} DESTINATION . )
    install(FILES ${PROJECT_SOURCE_DIR}/assets/images/st_icon.ico DESTINATION . )

    # Add systems libs to build directory
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})
    foreach(LIB ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS})
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                           COMMAND ${CMAKE_COMMAND} -E copy ${LIB}
                           ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/)
    endforeach()

    # Create folder for the deployment
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/platforms)

    # Location of libraries
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E make_directory
                       ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E make_directory
                       ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/plugins)

    string(TOLOWER "${CMAKE_BUILD_TYPE}" BUILD_TYPE_LOWERCASE)

    # Add QT libraries as shared libraries
    foreach(QT_LIBRARY ${ST_QT_LIBS})
        if(BUILD_TYPE_LOWERCASE STREQUAL "debug")
            get_target_property(ST_QT_LOC "Qt5::${QT_LIBRARY}" LOCATION_DEBUG)
        else()
            get_target_property(ST_QT_LOC "Qt5::${QT_LIBRARY}" LOCATION)
        endif()
        install(FILES ${ST_QT_LOC} DESTINATION .)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                           COMMAND ${CMAKE_COMMAND} -E copy ${ST_QT_LOC}
                           ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/)
    endforeach()

    set(WINDOWS_EXTRA_PLUGIN_NAME_ENDING)
    if(BUILD_TYPE_LOWERCASE STREQUAL "debug")
        set(WINDOWS_EXTRA_PLUGIN_NAME_ENDING "d")
    endif()

    deploy_qt_plugins()

    # Qt 5.5 ICU is 54
    windows_deploy_qt_libraries(icudt54 icuin54 icuuc54)

    # Install resources
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/translations
                       COMMAND ${CMAKE_COMMAND} -E copy ${QT_LOCALES} ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/translations/ )
    install(TARGETS ${PROJECT_NAME} DESTINATION . )
    install(FILES ${CMAKE_SOURCE_DIR}/cmake/win/qt.conf DESTINATION . )
    install(FILES ${CMAKE_SOURCE_DIR}/LICENSE DESTINATION . )
    install(FILES ${CMAKE_SOURCE_DIR}/DEPENDENCIES DESTINATION . )
    install(FILES ${CMAKE_SOURCE_DIR}/REPLACE_QT_LIBRARIES DESTINATION . )

endif(WIN32)

include(CPack)

