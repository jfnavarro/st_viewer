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

find_package(Armadillo REQUIRED)
include_directories(${ARMADILLO_INCLUDE_DIRS})

find_package(R REQUIRED)
include_directories(${LIBR_INCLUDE_DIRS})
include_directories(${LIBRCPP_INCLUDE_DIRS})
include_directories(${LIBRINSIDE_INCLUDE_DIRS})
include_directories(${LIBRCPPARMADILLO_INCLUDE_DIRS})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Add the source code as components

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

foreach(dir ${subdir_list})
    add_subdirectory("${dir}")
endforeach()

foreach(dir ${subdir_list})
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

if(MSVC)
    # A dependency is linking to the non debug version. This causes an annoying 
    # warning in Visual Studio debug builds.    
    string( TOLOWER "${CMAKE_BUILD_TYPE}" BUILD_TYPE_LOWERCASE)
    if(BUILD_TYPE_LOWERCASE STREQUAL "debug")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:libcmt")
    endif()
endif()

target_compile_definitions(${PROJECT_NAME} PUBLIC -DQCUSTOMPLOT_USE_LIBRARY)

# Link libraries for the ST Viewer target
target_link_libraries(${PROJECT_NAME} ${QT_TARGET_LINK_LIBS} qcustomplot
    ${ARMADILLO_LIBRARIES} ${LIBR_LIBRARIES} ${LIBRINSIDE_LIBRARIES} Threads::Threads)

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
    install(FILES ${CONFIG_FILE} DESTINATION ${CMAKE_INSTALL_PREFIX})
    install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}
            RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}
            LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}lib
            ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/doc)
    install(TARGETS qcustomplot LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

endif()

if(APPLE)

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

    set_source_files_properties("${CMAKE_SOURCE_DIR}/assets/images/st_icon.icn"
        PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    set_source_files_properties("${CMAKE_SOURCE_DIR}/cmake/mac/qt.conf"
        PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    set_source_files_properties("${CMAKE_SOURCE_DIR}/LICENSE"
        PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    set_source_files_properties("${CMAKE_SOURCE_DIR}/DEPENDENCIES"
        PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    set_source_files_properties("${CONFIG_FILE}"
        PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

endif(APPLE)

if(WIN32)

##TODO

endif(WIN32)

include(CPack)

