### PROJECT AND GLOBAL VARIABLES ##############################################

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

##LIBRARIES#####################################################

# Put everything that is generated into the binary dir
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_BINARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON) # For meta object compiler
set(CMAKE_AUTORCC ON) # Resource files
set(QT_RESOURCES "${PROJECT_BINARY_DIR}/application.qrc")
set(CMAKE_AUTOUIC ON) # UI files

# Qt libraries
set(ST_QT_LIBS Widgets PrintSupport Core Gui OpenGL Concurrent Charts)
if(UNIX AND NOT APPLE)
  list(APPEND ST_QT_LIBS DBus Svg)
endif()
foreach(i ${ST_QT_LIBS})
  USE_QT5LIB("Qt5${i}")
  list(APPEND QT_TARGET_LINK_LIBS "Qt5::${i}")
endforeach()

# External libraries
find_path(QCUSTOMPLOT_PATH NAMES qcustomplot.h)
add_library(qcustomplot SHARED "${QCUSTOMPLOT_PATH}/qcustomplot.cpp")
target_compile_definitions(qcustomplot PUBLIC -DQCUSTOMPLOT_COMPILE_LIBRARY)
target_link_libraries(qcustomplot Qt5::Widgets Qt5::Core Qt5::Gui Qt5::PrintSupport)
include_directories(${QCUSTOMPLOT_PATH})

if(WIN32)
    if(DEFINED ARMADILLO_PATH)
        set(ARMADILLO_INCLUDE_DIRS "${ARMADILLO_PATH}/include")
        set(ARMADILLO_LIBRARIES "${ARMADILLO_PATH}/examples/lib_win64/lapack_win64_MT.lib"
            "${ARMADILLO_PATH}/examples/lib_win64/blas_win64_MT.lib")

    else()
        message(FATAL_ERROR "ARMADILLO_PATH is empty")
    endif()
else()
    find_package(Armadillo REQUIRED)
endif()
include_directories(${ARMADILLO_INCLUDE_DIRS})

set(THREADS_PREFER_PTHREAD_FLAG ON)
if (APPLE)
    find_library(OpenMP_LIBRARY NAMES omp)
    find_path(OpenMP_INCLUDE_DIR omp.h)
    set(OpenMP_LIBRARIES ${OpenMP_LIBRARY})
    set(OpenMP_INCLUDE_DIRS ${OpenMP_INCLUDE_DIR})
    set(OpenMP_COMPILE_OPTIONS -Xpreprocessor -fopenmp)
    add_library(OpenMP::OpenMP SHARED IMPORTED)
    set_target_properties(OpenMP::OpenMP PROPERTIES
            IMPORTED_LOCATION ${OpenMP_LIBRARIES}
            INTERFACE_INCLUDE_DIRECTORIES "${OpenMP_INCLUDE_DIRS}"
            INTERFACE_COMPILE_OPTIONS "${OpenMP_COMPILE_OPTIONS}")
else()
    find_package(OpenMP REQUIRED)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

# Define what sub folders will be compiled
set(subdir_list
                   dialogs
                   color
                   data
                   viewTables
                   viewPages
                   viewRenderer
                   model
                   config
                   math
                   analysis)

# Add the source code as components
foreach(dir ${subdir_list})
    add_subdirectory("${dir}")
    list(APPEND ST_TARGET_OBJECTS "$<TARGET_OBJECTS:${dir}>")
endforeach()

###CREATE TARGET BINARY#################################

# Create main target's sources (resources needs to be part of the target)
set(ST_CLIENT_SOURCES main.cpp mainWindow.cpp SettingsStyle.h ${ST_TARGET_OBJECTS} ${QT_RESOURCES})

# Create executable target
if(WIN32)
    add_executable(${PROJECT_NAME} WIN32 ${ST_CLIENT_SOURCES})
elseif(APPLE)
    add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${ST_CLIENT_SOURCES})
else()
    add_executable(${PROJECT_NAME} ${ST_CLIENT_SOURCES})
endif()

target_compile_definitions(${PROJECT_NAME} PUBLIC -DQCUSTOMPLOT_USE_LIBRARY)

# Link libraries for the ST Viewer target
if (APPLE)
    target_link_libraries(${PROJECT_NAME} PUBLIC ${QT_TARGET_LINK_LIBS} qcustomplot
        ${ARMADILLO_LIBRARIES} "-framework Accelerate" OpenMP::OpenMP)
else()
    target_link_libraries(${PROJECT_NAME} PUBLIC ${QT_TARGET_LINK_LIBS} qcustomplot
        ${ARMADILLO_LIBRARIES} ${OpenMP_CXX_LIBRARIES})
endif()

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

if(UNIX AND NOT APPLE)

    # Select TGZ as the packager on Linux
    set(CPACK_GENERATOR "TGZ")
    set(CPACK_SYSTEM_NAME "Linux")
    set(CPACK_SYSTEM_NAME "${CPACK_SYSTEM_NAME}")

    # Install libraries and resources
    install(FILES ${CMAKE_SOURCE_DIR}/LICENSE DESTINATION ${CMAKE_INSTALL_PREFIX}/doc)
    install(FILES ${CMAKE_SOURCE_DIR}/DEPENDENCIES DESTINATION ${CMAKE_INSTALL_PREFIX}/doc)
    install(FILES ${CMAKE_SOURCE_DIR}/REPLACE_QT_LIBRARIES DESTINATION ${CMAKE_INSTALL_PREFIX}/doc)
    install(FILES ${CONFIG_FILE} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
    install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
            RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
            LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
            ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/doc)
    install(TARGETS qcustomplot LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

endif()

if(APPLE)

    # Set project name of binary dir for OSX
    set(OS_X_PROJECT_BINARY_DIR "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.app")
    set(CMAKE_INSTALL_PREFIX ${OS_X_PROJECT_BINARY_DIR}/Contents)
    set(DMG_DIR ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

    set(MACOSX_BUNDLE_ICON_NAME "st_icon.icns")

    # Configure and install Info.plist file
    set(InfoPlistFile ${CMAKE_BINARY_DIR}/Info.plist)
    execute_process(COMMAND date "+%Y"
                    OUTPUT_VARIABLE YEAR
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
    configure_file(${CMAKE_SOURCE_DIR}/cmake/mac/Info.plist.in ${InfoPlistFile})

    set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE TRUE)
    set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${InfoPlistFile})

    # MAC OSX sets
    set(MACOSX_BUNDLE_ICON "${CMAKE_SOURCE_DIR}/assets/images/${MACOSX_BUNDLE_ICON_NAME}")
    set(MACOSX_BUNDLE_ICON_FILE ${MACOSX_BUNDLE_ICON})
    set(MACOSX_BUNDLE_INFO_STRING "${PROJECT_NAME} ${PROJECT_VERSION}")
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "name.elemc.${PROJECT_NAME}")
    set(MACOSX_BUNDLE_VERSION "${PROJECT_VERSION}")
    set(MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}")
    set(MACOSX_BUNDLE_LONG_VERSION_STRING ${PROJECT_VERSION})
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}")
    set(MACOSX_BUNDLE_COPYRIGHT "${CMAKE_SOURCE_DIR}/LICENSE")
    set(CPACK_BUNDLE_PLIST ${InfoPlistFile})
    set(MACOSX_BUNDLE_RESOURCES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.app/Contents/Resources")

    # Architecture OSX
    set(CMAKE_OSX_ARCHITECTURES "ppc;i386")
    set(CMAKE_CXX_FLAGS "--sysroot ${CMAKE_OSX_SYSROOT} ${CMAKE_CXX_FLAGS}")

    # Find macdeoployqt
    find_program(MACDEPLOYQT NAMES macdeployqt macdeployqt-mac PATHS ${QT_BINARY_DIR} NO_DEFAULT_PATH)

    # Resources
    add_custom_command(TARGET ${PROJECT_NAME}
        COMMAND mkdir -p ${CMAKE_INSTALL_PREFIX}/Resources
        COMMAND install -m 644 ${MACOSX_BUNDLE_ICON} ${CMAKE_INSTALL_PREFIX}/Resources/
        COMMAND install -m 644 ${CMAKE_SOURCE_DIR}/cmake/mac/qt.conf ${CMAKE_INSTALL_PREFIX}/Resources/
        COMMAND install -m 644 ${CMAKE_SOURCE_DIR}/LICENSE ${CMAKE_INSTALL_PREFIX}/Resources/
        COMMAND install -m 644 ${CMAKE_SOURCE_DIR}/DEPENDENCIES ${CMAKE_INSTALL_PREFIX}/Resources/
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

    # Add systems libraries
    set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP false)
    include(InstallRequiredSystemLibraries)

    # Qt plug-ins needed
    set(QT_PLUGIN_CATEGORIES imageformats printsupport platforms)
    set(PLUGINS_IN_CATEGORY_imageformats qgif qico qjpeg qsvg)
    set(PLUGINS_IN_CATEGORY_printsupport windowsprintersupport)
    set(PLUGINS_IN_CATEGORY_platforms qwindows)

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

    # Install system libs
    install(FILES ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} DESTINATION .)

	# Install app's icon
    install(FILES ${PROJECT_SOURCE_DIR}/assets/images/st_icon.ico DESTINATION .)

    string(TOLOWER "${CMAKE_BUILD_TYPE}" BUILD_TYPE_LOWERCASE)

    # Add QT libraries as shared libraries
    foreach(QT_LIBRARY ${QT_TARGET_LINK_LIBS})
        if(BUILD_TYPE_LOWERCASE STREQUAL "debug")
            get_target_property(ST_QT_LOC "${QT_LIBRARY}" LOCATION_DEBUG)
        else()
            get_target_property(ST_QT_LOC "${QT_LIBRARY}" LOCATION)
        endif()
        install(FILES ${ST_QT_LOC} DESTINATION .)
    endforeach()

    set(WINDOWS_EXTRA_PLUGIN_NAME_ENDING)
    if(BUILD_TYPE_LOWERCASE STREQUAL "debug")
        set(WINDOWS_EXTRA_PLUGIN_NAME_ENDING "d")
    endif()

    # Deploy the qt plugins in the expected location
    foreach(PLUGIN_CATEGORY ${QT_PLUGIN_CATEGORIES})
        foreach(PLUGIN_NAME ${PLUGINS_IN_CATEGORY_${PLUGIN_CATEGORY}})
			install(FILES "${QT_PLUGINS_DIR}/${PLUGIN_CATEGORY}/${PLUGIN_NAME}${WINDOWS_EXTRA_PLUGIN_NAME_ENDING}${CMAKE_SHARED_LIBRARY_SUFFIX}" DESTINATION "./${PLUGIN_CATEGORY}")
        endforeach()
    endforeach()

    install(TARGETS ${PROJECT_NAME} DESTINATION .)
    install(FILES ${CONFIG_FILE} DESTINATION .)
    install(FILES ${CMAKE_SOURCE_DIR}/LICENSE DESTINATION .)
    install(FILES ${CMAKE_SOURCE_DIR}/DEPENDENCIES DESTINATION .)
    install(FILES ${CMAKE_BINARY_DIR}/libqcustomplot.dll DESTINATION .)
    install(FILES ${ARMADILLO_PATH}/examples/lib_win64/lapack_win64_MT.dll DESTINATION .)
    install(FILES ${ARMADILLO_PATH}/examples/lib_win64/blas_win64_MT.dll DESTINATION .)
	install(FILES ${QT_BINARY_DIR}/libstdc++-6.dll DESTINATION .)
	install(FILES ${QT_BINARY_DIR}/libgcc_s_seh-1.dll DESTINATION .)
	install(FILES ${QT_BINARY_DIR}/libwinpthread-1.dll DESTINATION .)
	install(FILES ${LIBRINSIDE_LIBRARIES} DESTINATION .)
	install(FILES ${LIBR_LIBRARIES} DESTINATION .)
    install(FILES ${CMAKE_SOURCE_DIR}/REPLACE_QT_LIBRARIES DESTINATION .)
	
endif(WIN32)

include(CPack)

