### PROJECT AND GLOBAL VARIABLES ##############################################

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

### PROJECT AND SOURCE STRUCTURE ##############################################

# NOTE this is very useful for Visual Studio
# Organize projects into folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

##LIBRARIES#####################################################

# Put everything that is generated into the binary dir
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_BINARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the path to Qt binaries and set the Qt libraries (needed here since /ext contains Qt libraries)
find_package(Qt5LinguistTools REQUIRED CONFIG)
set(ST_QT_LIBS Widgets PrintSupport Core Gui OpenGL Concurrent 3DRender)
if(UNIX AND NOT APPLE)
  list(APPEND ST_QT_LIBS DBus Svg)
endif()
foreach(i ${ST_QT_LIBS})
  USE_QT5LIB("Qt5${i}")
  list(APPEND QT_TARGET_LINK_LIBS "Qt5::${i}")
endforeach()

# Load resource and translations files
set(LOCALES "${PROJECT_SOURCE_DIR}/assets/trans/locale_en_us.ts")
set(RESOURCES "${PROJECT_BINARY_DIR}/application.qrc")
qt5_add_resources(QT_RESOURCES ${RESOURCES})
qt5_add_translation(QT_LOCALES ${LOCALES})

###DYNAMIC LIBRARIES#####################################

find_path(QCUSTOMPLOT_PATH NAMES qcustomplot.h)
add_library(qcustomplot SHARED "${QCUSTOMPLOT_PATH}/qcustomplot.cpp")
target_compile_definitions(qcustomplot PUBLIC -DQCUSTOMPLOT_COMPILE_LIBRARY)
target_link_libraries(qcustomplot Qt5::Widgets Qt5::Core Qt5::Gui Qt5::PrintSupport)
include_directories(${QCUSTOMPLOT_PATH})

find_package(Armadillo REQUIRED)
include_directories(${ARMADILLO_INCLUDE_DIRS})

find_package(Rcpp REQUIRED)
include_directories(${RCPP_INCLUDE_DIRS})

# Include third party libraries
add_subdirectory(ext)

# Define what sub folders will be compiled
set(subdir_list
                   dialogs
                   color
                   data
                   customWidgets
                   viewTables
                   viewPages
                   viewRenderer
                   utils
                   model
                   config
                   math
                   analysis)

foreach(dir ${subdir_list})
    add_subdirectory("${dir}")
endforeach()

###STATIC LIBRARIES#####################################

# Link st viewer libraries and third party libraries
foreach(dir ${subdir_list})
    list(APPEND ST_TARGET_OBJECTS "$<TARGET_OBJECTS:${dir}>")
endforeach()
list(APPEND ST_TARGET_OBJECTS "$<TARGET_OBJECTS:qtwaitingspinner>")

###CREATE TARGET BINARY#################################

# Create main target's sources (resources needs to be part of the target)
set(ST_CLIENT_SOURCES main.cpp mainWindow.cpp
    SettingsStyle.h SettingsVisual.h ${ST_TARGET_OBJECTS} ${QT_RESOURCES})

# Create executable target
if(WIN32)
    add_executable(${PROJECT_NAME} WIN32 ${ST_CLIENT_SOURCES})
elseif(APPLE)
    add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${ST_CLIENT_SOURCES})
else()
    add_executable(${PROJECT_NAME} ${ST_CLIENT_SOURCES})
endif()

if(MSVC)
    # A dependency is linking to the non debug version. This causes an annoying 
    # warning in Visual Studio debug builds.    
    string( TOLOWER "${CMAKE_BUILD_TYPE}" BUILD_TYPE_LOWERCASE)
    if(BUILD_TYPE_LOWERCASE STREQUAL "debug")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:libcmt")
    endif()
endif()

target_compile_definitions(${PROJECT_NAME} PUBLIC -DQCUSTOMPLOT_USE_LIBRARY)

# Link libraries for hte ST Viewer target
target_link_libraries(${PROJECT_NAME} ${QT_TARGET_LINK_LIBS} qcustomplot ${ARMADILLO_LIBRARIES} ${RCPP_LIBRARIES})

### UNIT TESTS ################################################################

enable_testing()
add_subdirectory(test)

############################INSTALLATION#########################################

SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)

# PACKAGING OPTIONS: GENERAL
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_VENDOR "${PROJECT_VENDOR}")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")

# Add systems libraries
set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP false)
include(InstallRequiredSystemLibraries)

# Qt plug-ins needed
set(QT_PLUGIN_CATEGORIES imageformats printsupport platforms)
set(PLUGINS_IN_CATEGORY_imageformats qgif qico qjpeg qsvg)
set(PLUGINS_IN_CATEGORY_printsupport)
set(PLUGINS_IN_CATEGORY_platforms)

if(WIN32)
    list(APPEND PLUGINS_IN_CATEGORY_printsupport windowsprintersupport)
	list(APPEND PLUGINS_IN_CATEGORY_platforms qwindows)
endif()

if(UNIX AND NOT APPLE)
    list(APPEND PLUGINS_IN_CATEGORY_printsupport cupsprintersupport)
    list(APPEND PLUGINS_IN_CATEGORY_platforms qlinuxfb qminimal qxcb)
endif()

if(UNIX AND NOT APPLE)

    # To make the binary link to the libraries in the lib folder
    SET_TARGET_PROPERTIES(${PROJECT_NAME}
        PROPERTIES INSTALL_RPATH "$ORIGIN/lib:$ORIGIN/:$ORIGIN/plugins")

    # Select TGZ as the packager on Linux
    set(CPACK_GENERATOR "TGZ")
    set(CPACK_SYSTEM_NAME "Linux")
    set(CPACK_SYSTEM_NAME "${CPACK_SYSTEM_NAME}")

    # Add Qt libraries as shared libraries
    foreach(QT_LIBRARY ${QT_TARGET_LINK_LIBS})
       # note there is also the targets: LOCATION_RELEASE and LOCATION_DEBUG
       get_target_property(ST_QT_LOC "${QT_LIBRARY}" LOCATION)
       INSTALL_LIBRARY_AND_SYMLINKS("${ST_QT_LOC}" lib)
     endforeach()

    # Qt needs ICU 56 libraries in Linux
    set(ICU_VERSION "56.1")
    foreach(ICU_LIB icuuc icui18n icudata)
      find_library(loc${ICU_LIB} "lib${ICU_LIB}.so.${ICU_VERSION}" NO_CMAKE_SYSTEM_PATH)
      if (NOT loc${ICU_LIB})
        message(FATAL_ERROR "Could not find lib${ICU_LIB}.so.${ICU_VERSION}")
      endif()
      message(STATUS "${loc${ICU_LIB}}")
      INSTALL_LIBRARY_AND_SYMLINKS("${loc${ICU_LIB}}" lib)
    endforeach()

    # Deploy the qt plugins in the plugins folder
    deploy_qt_plugins()

    # Install libraries and resources
    install(FILES ${CMAKE_SOURCE_DIR}/LICENSE DESTINATION .)
    install(FILES ${CMAKE_SOURCE_DIR}/REPLACE_QT_LIBRARIES DESTINATION .)
    install(FILES ${CMAKE_SOURCE_DIR}/DEPENDENCIES DESTINATION .)
    install(FILES ${CMAKE_SOURCE_DIR}/cmake/linux/STViewer.sh DESTINATION .)
    install(FILES ${CONFIG_FILE} DESTINATION .)
    install(TARGETS ${PROJECT_NAME} DESTINATION .
            RUNTIME DESTINATION .
            LIBRARY DESTINATION lib
            ARCHIVE DESTINATION .)
    install(TARGETS qcustomplot LIBRARY DESTINATION lib)

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
    add_custom_command(TARGET ${PROJECT_NAME}
        COMMAND mkdir -p ${CMAKE_INSTALL_PREFIX}/Resources
        COMMAND install -m 644 ${CMAKE_SOURCE_DIR}/assets/images/st_icon.icns ${CMAKE_INSTALL_PREFIX}/Resources/
        COMMAND install -m 644 ${CMAKE_SOURCE_DIR}/cmake/mac/qt.conf ${CMAKE_INSTALL_PREFIX}/Resources/
        COMMAND install -m 644 ${CONFIG_FILE} ${CMAKE_INSTALL_PREFIX}/Resources/)
    add_custom_command(TARGET ${PROJECT_NAME} COMMAND ${MACDEPLOYQT} ${OS_X_PROJECT_BINARY_DIR})

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
    install(FILES ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} DESTINATION .)
    install(FILES ${PROJECT_SOURCE_DIR}/assets/images/st_icon.ico DESTINATION .)

    # Add systems libs to build directory
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})
    foreach(LIB ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS})
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                           COMMAND ${CMAKE_COMMAND} -E copy ${LIB}
                           ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/)
    endforeach()

    set(CPACK_GENERATOR "NSIS") # Distribution packaging formats ("make package")
    #set(CPACK_NSIS_MODIFY_PATH "ON")
    set(CPACK_NSIS_CONTACT "Jose Fernandez <jose.fernandez.navarro@scilifelab.se>")
    set(CPACK_NSIS_PACKAGE_NAME ${PROJECT_NAME})
    set(CPACK_NSIS_DISPLAY_NAME "Spatial Transcriptoimcs Viewer")
    # Right now we are invoking Windows CMake from inside Cygwin.
    # Maybe that is why we need to replace the "/" with "\\\\"?
    string(REPLACE "/" "\\\\" source_dir_windows "${CMAKE_SOURCE_DIR}")
    set(CPACK_NSIS_MUI_UNICON "${source_dir_windows}\\\\assets\\\\images\\\\st_icon.ico")
    set(CPACK_NSIS_MUI_ICON "${source_dir_windows}\\\\assets\\\\images\\\\st_icon.ico")
    set(CPACK_PACKAGE_ICON "${source_dir_windows}\\\\assets\\\\images\\\\st_icon.ico")
    set(CPACK_NSIS_INSTALLED_ICON_NAME "\\\\${PROJECT_NAME}.exe")
    set(CPACK_NSIS_EXECUTABLES_DIRECTORY ".")
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
    set(CPACK_NSIS_PAGE_COMPONENTS "
        !define MUI_FINISHPAGE_RUN \\\"$INSTDIR\\\\${PROJECT_NAME}.exe\\\"
        !define MUI_STARTMENUPAGE_NODISABLE")
    set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
        CreateShortCut \\\"\\\$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\${PROJECT_NAME}.lnk\\\" \\\"\\\$INSTDIR\\\\${PROJECT_NAME}.exe\\\" \\\"\\\" \\\"\\\$INSTDIR\\\\st_icon.ico\\\"
        CreateShortCut \\\"\\\$DESKTOP\\\\${PROJECT_NAME}.lnk\\\" \\\"\\\$INSTDIR\\\\${PROJECT_NAME}.exe\\\" \\\"\\\" \\\"\\\$INSTDIR\\\\st_icon.ico\\\"
    ")

    set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "${CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS}
        Delete \\\"$SMPROGRAMS\\\\${PROJECT_NAME}\\\\${PROJECT_NAME}.lnk\\\"
        Delete \\\"$SMPROGRAMS\\\\${PROJECT_NAME}\\\\Uninstall.lnk \\\"
        RmDir \\\"$SMPROGRAMS\\\\${PROJECT_NAME}\\\"
        Delete \\\"$DESKTOP\\\\${PROJECT_NAME}.lnk\\\"
        Delete \\\"\\\$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\${PROJECT_NAME}.lnk\\\"
    ")

    # Location of libraries
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E make_directory
                       ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})
	foreach(PLUGIN_CATEGORY ${QT_PLUGIN_CATEGORIES})
		add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
						   COMMAND ${CMAKE_COMMAND} -E make_directory
						   ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${PLUGIN_CATEGORY})
	endforeach()

    string(TOLOWER "${CMAKE_BUILD_TYPE}" BUILD_TYPE_LOWERCASE)

    # Add QT libraries as shared libraries
    foreach(QT_LIBRARY ${QT_TARGET_LINK_LIBS})
        if(BUILD_TYPE_LOWERCASE STREQUAL "debug")
            get_target_property(ST_QT_LOC "${QT_LIBRARY}" LOCATION_DEBUG)
        else()
            get_target_property(ST_QT_LOC "${QT_LIBRARY}" LOCATION)
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

    # Deploy the qt plugins in the expected location
    deploy_qt_plugins()

    # Qt 5.8 does not include ICU libraries
    # windows_deploy_qt_libraries(icudt56 icuin56 icuuc56)

    # Install resources
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/translations
                       COMMAND ${CMAKE_COMMAND} -E copy ${QT_LOCALES} ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/translations/)
    install(TARGETS ${PROJECT_NAME} DESTINATION .)
    install(FILES ${CONFIG_FILE} DESTINATION .)
    install(FILES ${CMAKE_SOURCE_DIR}/LICENSE DESTINATION .)
    install(FILES ${CMAKE_SOURCE_DIR}/DEPENDENCIES DESTINATION .)
    install(FILES ${CMAKE_SOURCE_DIR}/REPLACE_QT_LIBRARIES DESTINATION .)
    install(FILES ${CMAKE_BINARY_DIR}/qcustomplot.dll DESTINATION .)

endif(WIN32)

include(CPack)

