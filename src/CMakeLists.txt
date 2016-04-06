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

# Put everything that is generated into the binary dir
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_BINARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

###STATIC LIBRARIES#####################################

# Link static libraries and third party libraries
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
set(ST_CLIENT_SOURCES main.cpp stVi.cpp ${ST_TARGET_OBJECTS} ${QT_RESOURCES})

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
    string( TOLOWER "${CMAKE_BUILD_TYPE}" BUILD_TYPE_LOWERCASE )
    if(BUILD_TYPE_LOWERCASE STREQUAL "debug")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:libcmt")
    endif()
endif()

# Platform specific library includes
if(WIN32)
    target_link_libraries(${PROJECT_NAME} Qt5::WinMain)
endif()

if(APPLE)
   target_link_libraries(${PROJECT_NAME} ${MAC_EXTRA_LIBS})
endif()

# Link libraries to the main target
target_link_libraries(${PROJECT_NAME} ${QT_TARGET_LINK_LIBS} ${OPENGL_LIBRARY})

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

# Add systems libraries
set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP true)
include(InstallRequiredSystemLibraries)

set(QT_PLUGIN_CATEGORIES accessible imageformats printsupport platforms)
set(PLUGINS_IN_CATEGORY_accessible)
set(PLUGINS_IN_CATEGORY_imageformats qgif qico qjpeg)
set(PLUGINS_IN_CATEGORY_printsupport)
set(PLUGINS_IN_CATEGORY_platforms)

if(WIN32) 
    list(APPEND PLUGINS_IN_CATEGORY_printsupport windowsprintersupport)
endif()

if(UNIX AND NOT APPLE)
    list(APPEND PLUGINS_IN_CATEGORY_printsupport cupsprintersupport)
    list(APPEND PLUGINS_IN_CATEGORY_platforms qlinuxfb qminimal qxcb)
endif()

# TODO add installing code that uses dynamic linking
# TODO add code to make installer that creates binaries that used dynamic linking

