#
#    Copyright (C) 2012  Spatial Transcriptomics AB,
#    read LICENSE for licensing terms. 
#    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>
#
#

###############################################################################
# PREPARING TO INSTALL
###############################################################################

# PRINT VARIABLES
message( STATUS )
message( STATUS
"-------------------------------------------------------------------------------"
)
message( STATUS "Building ${APPLICATION_NAME} ....." )
message( STATUS "Change a configuration variable with: cmake -D<Variable>=<Value>" )
message( STATUS "CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}" )
message( STATUS "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}" )
message( STATUS "CMAKE_PREFIX_PATH = ${CMAKE_PREFIX_PATH}" )
message( STATUS "TARGET_ARCH = ${TARGET_ARCH}" )
message( STATUS "VERSION = ${PROJECT_VERSION}")
message( STATUS "ENDPOINT = ${ENDPOINT}")
message( STATUS
"-------------------------------------------------------------------------------"
)
message( STATUS )

### PROJECT AND GLOBAL VARIABLES ##############################################

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

set(LOCALES "${PROJECT_SOURCE_DIR}/assets/trans/locale_en_us.ts") 
set(RESOURCES "${PROJECT_BINARY_DIR}/application.qrc")

qt5_add_resources(QT_RESOURCES ${RESOURCES})
qt5_add_translation(QT_LOCALES ${LOCALES})

set(subdir_list
                   dialogs
                   color
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

#NOTE this is very useful for Visual Studio
# Organize projects into folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

###PACKAGES######################################

find_package(OpenGL REQUIRED)
if(OPENGL_FOUND)
    include_directories(${OPENGL_INCLUDE_DIR})
    mark_as_advanced(${OPENGL_LIBRARY})
else(OPENGL_FOUND)
    message(FATAL_ERROR "OpenGL environment missing")
endif(OPENGL_FOUND)

if(APPLE)
    find_library(AppKit_LIBRARY AppKit)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(APP_SERVICES_LIBRARY ApplicationServices)
    mark_as_advanced(APP_SERVICES_LIBRARY COCOA_LIBRARY AppKit_LIBRARY) 
    set(MAC_EXTRA_LIBS ${APP_SERVICES_LIBRARY} ${COCOA_LIBRARY} ${AppKit_LIBRARY}) 
endif()

##FLAGS AND DEFINITIONS###

add_definitions(-DPROJECT_VERSION=\"${PROJECT_VERSION}\")

##BUILDING AND INSTALLING #############################################################

# put everything that is generated into the binary dir
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_BINARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

###STATIC LIBRARIES#####################################

add_subdirectory(testTools)

#link static libraries and third party libraries

set(LIB_COMPONENTS QtSingleApplication)

foreach(dir ${subdir_list})
    list(APPEND ST_TARGET_OBJECTS "$<TARGET_OBJECTS:${dir}>")
endforeach()

list(APPEND ST_TARGET_OBJECTS "$<TARGET_OBJECTS:qxt>")
list(APPEND ST_TARGET_OBJECTS "$<TARGET_OBJECTS:qcustomplot>")

set(QT_TARGET_LINK_LIBS Qt5::Core
                             Qt5::Network
                             Qt5::Gui
                             Qt5::Widgets
                             Qt5::PrintSupport
                             Qt5::OpenGL
                             Qt5::3D)

#create main target (resources needs to be part of the target)

include_directories(${PROJECT_SOURCE_DIR}/ext/QtSingleApplication/src)

set(ST_CLIENT_SOURCES main.cpp stVi.cpp ${ST_TARGET_OBJECTS} ${QT_RESOURCES})

if(WIN32)
    add_executable(${PROJECT_NAME} WIN32 ${ST_CLIENT_SOURCES})
elseif(APPLE)
    add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${ST_CLIENT_SOURCES})
else()
    add_executable(${PROJECT_NAME} ${ST_CLIENT_SOURCES})
endif()

# Platform specific library includes
if(WIN32)
    target_link_libraries(${PROJECT_NAME} Qt5::WinMain)
endif()

if(APPLE)
   target_link_libraries(${PROJECT_NAME} ${MAC_EXTRA_LIBS}) 
endif()

#link libraries to the main target

target_link_libraries(${PROJECT_NAME} ${LIB_COMPONENTS} ${QT_TARGET_LINK_LIBS} ${OPENGL_LIBRARY})

### UNIT TESTS ################################################################

enable_testing()
add_subdirectory(test)

############################INSTALLATION#########################################

# PACKAGING OPTIONS: GENERAL
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_VENDOR "${PROJECT_VENDOR}")
set(CPACK_RESOURCE_FILE_WELCOME "${CMAKE_SOURCE_DIR}/WELCOME")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")

#add systems libraries
set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP true)
include(InstallRequiredSystemLibraries)

set(QT_PLUGIN_CATEGORIES accessible imageformats printsupport platforms)
set(PLUGINS_IN_CATEGORY_accessible qtaccessiblequick qtaccessiblewidgets)
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

    # use, i.e. don't skip the full RPATH for the build tree
    set(CMAKE_SKIP_BUILD_RPATH FALSE)

    # when building, don't use the install RPATH already
    # (but later on when installing)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib;${CMAKE_INSTALL_PREFIX}/plugins")

    # add the automatically determined parts of the RPATH
    # which point to directories outside the build tree to the install RPATH
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

    # the RPATH to be used when installing, but only if it's not a system directory
    list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
    if("${isSystemDir}" STREQUAL "-1")
        set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
    endif("${isSystemDir}" STREQUAL "-1")

    # Select TGZ as the packager on Linux
    set(CPACK_GENERATOR "TGZ")
    set(CPACK_SYSTEM_NAME "Linux")
    set(CPACK_SYSTEM_NAME "${CMAKE_BUILD_TYPE}-${CPACK_SYSTEM_NAME}-32")

    foreach(QT_LIBRARY ${ST_QT_LIBS})
      # note there is also the targets: LOCATION_RELEASE and LOCATION_DEBUG 
      # but I think we don't need them /Erik Sjolund
      get_target_property(ST_QT_LOC "Qt5::${QT_LIBRARY}" LOCATION)
      INSTALL_LIBRARY_AND_SYMLINKS("${ST_QT_LOC}" lib)
    endforeach()

    #qt needs ICU libaries in Linux
    set(ICU_VERSION "52.1")
    foreach(ICU_LIB icuuc icui18n icudata)
      find_library(loc${ICU_LIB} lib${ICU_LIB}.so.${ICU_VERSION} NO_CMAKE_SYSTEM_PATH)
      if (NOT loc${ICU_LIB})
        message(FATAL_ERROR "Could not find lib${ICU_LIB}.so.${ICU_VERSION}")
      endif()
      INSTALL_LIBRARY_AND_SYMLINKS("${loc${ICU_LIB}}" lib)
    endforeach()

    deploy_qt_plugins()

    #install libraries and resources
    install(FILES ${CMAKE_SOURCE_DIR}/cmake/linux/qt.conf DESTINATION bin)
    install(TARGETS ${PROJECT_NAME} DESTINATION bin)
    install(FILES ${CMAKE_SOURCE_DIR}/LICENSE DESTINATION .)
endif()

if(APPLE)

    #set project name of binary dir for OSX
    set(OS_X_PROJECT_BINARY_DIR "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.app")
    set(CMAKE_INSTALL_PREFIX ${OS_X_PROJECT_BINARY_DIR}/Contents)
    set(DMG_DIR ${CMAKE_BINARY_DIR}/${PROJECT_NAME})
    
    #configure and install Info.plist file
    set(InfoPlistFile ${CMAKE_BINARY_DIR}/Info.plist)
    execute_process(COMMAND date "+%Y"
                    OUTPUT_VARIABLE YEAR
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
    configure_file(${CMAKE_SOURCE_DIR}/cmake/mac/Info.plist.in ${InfoPlistFile})
    set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${InfoPlistFile})

    # MACOSX sets
    set(MACOSX_BUNDLE_ICON_FILE "${CMAKE_SOURCE_DIR}/assets/images/stVi.icns")
    set(MACOSX_BUNDLE_INFO_STRING "The Spatial Transcriptomics Viewer")
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "name.elemc.${PROJECT_NAME}")
    set(MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJECT_VERSION}")
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}")
    set(MACOSX_BUNDLE_VERSION "${PROJECT_VERSION}")
    set(MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}")
    set(MACOSX_BUNDLE_COPYRIGHT "Copyright (C) 2012  Spatial Transcriptomics AB,
                                 read LICENSE for licensing terms. 
                                 Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>")
    set(CPACK_BUNDLE_PLIST ${InfoPlistFile})
    
    #architecture OSX
    set(CMAKE_OSX_ARCHITECTURES "ppc;i386")
    set(CMAKE_CXX_FLAGS "--sysroot ${CMAKE_OSX_SYSROOT} ${CMAKE_CXX_FLAGS}")

    #find macdeoployqt
    find_program(MACDEPLOYQT NAMES macdeployqt macdeployqt-mac PATHS ${QT_BINARY_DIR} NO_DEFAULT_PATH)
    
    #resources
    add_custom_command (TARGET ${PROJECT_NAME}
        COMMAND mkdir -p ${CMAKE_INSTALL_PREFIX}/Resources
        COMMAND install -m 644 ${CMAKE_SOURCE_DIR}/assets/images/stVi.icns ${CMAKE_INSTALL_PREFIX}/Resources/
        COMMAND install -m 644 ${CMAKE_SOURCE_DIR}/cmake/mac/qt.conf ${CMAKE_INSTALL_PREFIX}/Resources/
        COMMAND ${MACDEPLOYQT} ${OS_X_PROJECT_BINARY_DIR}
        )
  
    # build dmg
    add_custom_target(dmg
        COMMAND mkdir -p ${DMG_DIR}
        COMMAND cp -R ${OS_X_PROJECT_BINARY_DIR} ${DMG_DIR}/
        COMMAND ln -sf /Applications ${DMG_DIR}
        COMMAND cp ${CMAKE_SOURCE_DIR}/assets/images/stVi.icns .VolumeIcon.icns
        COMMAND cp ${CMAKE_SOURCE_DIR}/LICENSE ${DMG_DIR}
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
    
    #install system libs
    install(FILES ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} DESTINATION . )

    #add systems libs to build directory
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})
    foreach(LIB ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS})
	    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E copy ${LIB} 
				${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/)
    endforeach()

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/platforms)
                       
    set(CPACK_GENERATOR "NSIS") # Distribution packaging formats ("make package")
    #set(CPACK_NSIS_MODIFY_PATH "ON")
    set(CPACK_NSIS_CONTACT "Jose Fernandez <jose.fernandez.navarro@scilifelab.se>")
    set(CPACK_NSIS_PACKAGE_NAME ${PROJECT_NAME}-${CMAKE_BUILD_TYPE})
    set(CPACK_NSIS_DISPLAY_NAME "Spatial Transcriptoimcs Viewer")
    set(CPACK_NSIS_URL_INFO_ABOUT "http://www.spatialtranscriptomics.com")
    #set(CPACK_NSIS_MUI_ICON ${CMAKE_SOURCE_DIR}/assets/images\\\\st.png)
    #set(CPACK_PACKAGE_ICON ${CMAKE_SOURCE_DIR}/assets/images\\\\st_icon.png)
    #set(CPACK_NSIS_INSTALLED_ICON_NAME ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.exe)
    set(CPACK_NSIS_EXECUTABLES_DIRECTORY ".")
    #set(CPACK_NSIS_MENU_LINKS "http://www.spatialtranscriptomics.com")
    set(CPACK_NSIS_URL_INFO_ABOUT "http://www.spatialtranscriptomics.com")
    set(CPACK_NSIS_HELP_LINK "http://www.spatialtranscriptomics.com")
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
    set(CPACK_NSIS_PAGE_COMPONENTS "
        !define MUI_FINISHPAGE_RUN \\\"$INSTDIR\\\\${PROJECT_NAME}.exe\\\"
        !define MUI_STARTMENUPAGE_NODISABLE
    ")

    set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
        CreateShortCut \\\"$DESKTOP\\\\${PROJECT_NAME}.lnk\\\" \\\"$INSTDIR\\\\${PROJECT_NAME}.exe\\\"
    ")

    set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
        Delete \\\"$DESKTOP\\\\${PROJECT_NAME}.lnk\\\"
    ")

    # Qt libraries required by stVi
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E make_directory 
                       ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E make_directory 
                       ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/plugins)

    foreach(QT_LIBRARY ${ST_QT_LIBS})
        get_target_property(ST_QT_LOC "Qt5::${QT_LIBRARY}" LOCATION)
        install(FILES ${ST_QT_LOC} DESTINATION .)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD   
                           COMMAND ${CMAKE_COMMAND} -E copy ${ST_QT_LOC} 
                           ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/)
    endforeach()
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:stVi>
                       ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/ )

    set(WINDOWS_EXTRA_PLUGIN_NAME_ENDING)
    if(CMAKE_BUILD_TYPE MATCHES [Dd][Ee][Bb][Uu][Gg])
        set(WINDOWS_EXTRA_PLUGIN_NAME_ENDING "d")
    endif()

    deploy_qt_plugins()
    
    #qt5.1.1 icu is 51 for lower versions is 49
    windows_deploy_qt_libraries(icudt52 icuin52 icuuc52)
    
    #install platforms plugins
    foreach(QT_PLUGIN_FILE qminimal qwindows)
        install(FILES ${QT_PLUGINS_DIR}/platforms/${CMAKE_SHARED_LIBRARY_PREFIX}${QT_PLUGIN_FILE}${CMAKE_SHARED_LIBRARY_SUFFIX}
                DESTINATION platforms)
		add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                                   COMMAND ${CMAKE_COMMAND} -E copy 
                                   ${QT_PLUGINS_DIR}/platforms/${CMAKE_SHARED_LIBRARY_PREFIX}${QT_PLUGIN_FILE}${WINDOWS_EXTRA_PLUGIN_NAME_ENDING}${CMAKE_SHARED_LIBRARY_SUFFIX}
                                   ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/platforms/)
    endforeach()

    #install resources
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/translations
                       COMMAND ${CMAKE_COMMAND} -E copy ${QT_LOCALES} ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/translations/
                       COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/cmake/win/qt.conf ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/
                       COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/cmake/win/libeay32.dll ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/
                       COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/cmake/win/ssleay32.dll ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/ )

    install(TARGETS ${PROJECT_NAME} DESTINATION . )
    install(FILES ${CMAKE_SOURCE_DIR}/cmake/win/qt.conf DESTINATION . )   
    install(FILES ${CMAKE_SOURCE_DIR}/cmake/win/libeay32.dll DESTINATION . )
    install(FILES ${CMAKE_SOURCE_DIR}/cmake/win/ssleay32.dll DESTINATION . )

endif(WIN32)

include(CPack)
