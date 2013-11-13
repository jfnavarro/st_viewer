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

# Initialise the project
initialise_project()

set(INC_MODEL
    model/Chip.h
    model/Dataset.h
    model/Feature.h
    model/FeatureExtended.h
    model/Gene.h
    model/HitCount.h
    model/ObjectParser.h
    model/User.h
    model/UserExperiment.h
)
set(SRC_MODEL
    model/Chip.cpp
    model/Dataset.cpp
    model/Feature.cpp
    model/FeatureExtended.cpp
    model/Gene.cpp
    model/HitCount.cpp
    model/ObjectParser.cpp
    model/User.cpp
    model/UserExperiment.cpp
)

set(INC_MODEL_CORE
    model/core/ColorPalette.h
    model/core/ColorScheme.h
    model/core/FeatureColor.h
    model/core/HeatMapColor.h
    model/core/DynamicRangeColor.h
)
set(SRC_MODEL_CORE
    model/core/ColorPalette.cpp
    model/core/ColorScheme.cpp
    model/core/FeatureColor.cpp
    model/core/HeatMapColor.cpp
    model/core/DynamicRangeColor.cpp
)

set(INC_MODEL_DTO
    model/dto/ChipDTO.h
    model/dto/DatasetDTO.h
    model/dto/ErrorDTO.h
    model/dto/FeatureDTO.h
    model/dto/GeneDTO.h
    model/dto/HitCountDTO.h
    model/dto/OAuth2TokenDTO.h
    model/dto/UserDTO.h
    model/dto/UserExperimentDTO.h
)
set(SRC_MODEL_DTO
    model/dto/ChipDTO.cpp
    model/dto/DatasetDTO.cpp
    model/dto/ErrorDTO.cpp
    model/dto/FeatureDTO.cpp
    model/dto/GeneDTO.cpp
    model/dto/HitCountDTO.cpp
    model/dto/OAuth2TokenDTO.cpp
    model/dto/UserDTO.cpp
    model/dto/UserExperimentDTO.cpp
)

set(INC_VIEW_COMPONENTS
    view/components/openGL/GenePlotterGL.h
    view/components/openGL/GraphicsSceneGL.h
    view/components/openGL/GraphicsViewGL.h
    view/components/openGL/HeatMapLegendGL.h
    view/components/openGL/ImageItemGL.h
    view/components/openGL/MiniMapGL.h
    view/components/openGL/SelectionEvent.h
    view/components/openGL/WidgetGL.h
    view/components/openGL/ViewItemGL.h
    view/components/AboutDialog.h
    view/components/ExtendedTabWidget.h
    view/components/LoginDialog.h
    view/components/MainMenuBar.h
    view/components/MainStatusBar.h
    view/components/SelectionDialog.h
)
set(SRC_VIEW_COMPONENTS
    view/components/openGL/GenePlotterGL.cpp
    view/components/openGL/GraphicsSceneGL.cpp
    view/components/openGL/GraphicsViewGL.cpp
    view/components/openGL/HeatMapLegendGL.cpp
    view/components/openGL/ImageItemGL.cpp
    view/components/openGL/MiniMapGL.cpp
    view/components/openGL/WidgetGL.cpp
    view/components/openGL/ViewItemGL.cpp
    view/components/AboutDialog.cpp
    view/components/ExtendedTabWidget.cpp
    view/components/LoginDialog.cpp
    view/components/MainMenuBar.cpp
    view/components/MainStatusBar.cpp
    view/components/SelectionDialog.cpp
)

set(INC_VIEW_DELEGATES
    view/delegates/BooleanItemDelegate.h
    view/delegates/GeneViewDelegate.h
    view/delegates/DatasetsViewItemDelegate.h
)
set(SRC_VIEW_DELEGATES
    view/delegates/BooleanItemDelegate.cpp
    view/delegates/GeneViewDelegate.cpp
    view/delegates/DatasetsViewItemDelegate.cpp
)

set(INC_VIEW_EDITORS
    view/editors/ColorListEditor.h
)
set(SRC_VIEW_EDITORS
    view/editors/ColorListEditor.cpp
)

set(INC_VIEW_PAGES
    view/pages/CellViewPage.h
    view/pages/DatasetPage.h
    view/pages/ExperimentPage.h
    view/pages/InitPage.h
    view/pages/Page.h
)
set(SRC_VIEW_PAGES
    view/pages/CellViewPage.cpp
    view/pages/DatasetPage.cpp
    view/pages/ExperimentPage.cpp
    view/pages/InitPage.cpp
    view/pages/Page.cpp
)

set(INC_VIEW_MODEL_CONTROLLER
    view/controllers/GeneFeatureItemModel.h
    view/controllers/DatasetItemModel.h
    view/controllers/ExperimentsItemModel.h
    view/controllers/GeneSelectionItemModel.h
)
set(SRC_VIEW_MODEL_CONTROLLER
    view/controllers/GeneFeatureItemModel.cpp
    view/controllers/DatasetItemModel.cpp
    view/controllers/ExperimentsItemModel.cpp
    view/controllers/GeneSelectionItemModel.cpp
)

set(INC_CONTROLLER
    controller/stVi.h
)
set(SRC_CONTROLLER
    controller/stVi.cpp
)

set(INC_CONTROLLER_ASYNC
    controller/async/ImageProcessing.h
)
set(SRC_CONTROLLER_ASYNC
    controller/async/ImageProcessing.cpp
)

set(INC_CONTROLLER_AUTH
    controller/auth/AuthorizationManager.h
    controller/auth/OAuth2.h
    controller/auth/TokenStorage.h
)
set(SRC_CONTROLLER_AUTH
    controller/auth/AuthorizationManager.cpp
    controller/auth/OAuth2.cpp
    controller/auth/TokenStorage.cpp
)

set(INC_CONTROLLER_DATA
    controller/data/DataStore.h
    controller/data/DataProxy.h
    controller/data/ResourceStore.h
)
set(SRC_CONTROLLER_DATA
    controller/data/DataStore.cpp
    controller/data/DataProxy.cpp
)

set(INC_CONTROLLER_ERROR  
    controller/error/SSLNetworkError.h
    controller/error/NetworkError.h
    controller/error/JSONError.h
    controller/error/OAuth2Error.h
    controller/error/ApplicationError.h
    controller/error/Error.h
    controller/error/ErrorManager.h
    controller/error/ServerError.h    
)
set(SRC_CONTROLLER_ERROR  
    controller/error/SSLNetworkError.cpp
    controller/error/NetworkError.cpp
    controller/error/JSONError.cpp
    controller/error/OAuth2Error.cpp
    controller/error/ApplicationError.cpp
    controller/error/Error.cpp
    controller/error/ErrorManager.cpp
    controller/error/ServerError.cpp    
)

set(INC_CONTROLLER_IO
    controller/io/GeneExporter.h
    controller/io/GeneXMLExporter.h
    controller/io/GeneTXTExporter.h
)
set(SRC_CONTROLLER_IO
    controller/io/GeneExporter.cpp
    controller/io/GeneXMLExporter.cpp
    controller/io/GeneTXTExporter.cpp
)

set(INC_CONTROLLER_NETWORK
    controller/network/NetworkCommand.h
    controller/network/NetworkManager.h
    controller/network/NetworkReply.h
    controller/network/RESTCommandFactory.h
    controller/network/DownloadManager.h
)
set(SRC_CONTROLLER_NETWORK
    controller/network/NetworkCommand.cpp
    controller/network/NetworkManager.cpp
    controller/network/NetworkReply.cpp
    controller/network/RESTCommandFactory.cpp
    controller/network/DownloadManager.cpp
)

SET(INC_UTILS
    utils/MathExtended.h
    utils/QGLExtended.h
    utils/Singleton.h
    utils/Utils.h
)
SET(SRC_UTILS
    utils/MathExtended.cpp
    utils/QGLExtended.cpp
    utils/Utils.cpp
)

SET(INC_UTILS_CONFIG
    utils/config/Configuration.h
    utils/config/SettingsFormatXML.h
)
SET(SRC_UTILS_CONFIG
    utils/config/Configuration.cpp
    utils/config/SettingsFormatXML.cpp
)

set(INCLUDES
    ${INC_MODEL}
    ${INC_MODEL_CORE}
    ${INC_MODEL_DTO}
    ${INC_UTILS}
    ${INC_UTILS_CONFIG}
    ${INC_VIEW_MODEL_CONTROLLER}
    ${INC_VIEW_COMPONENTS}
    ${INC_VIEW_DELEGATES}
    ${INC_VIEW_EDITORS}
    ${INC_VIEW_PAGES}
    ${INC_CONTROLLER}
    ${INC_CONTROLLER_ASYNC}
    ${INC_CONTROLLER_AUTH}
    ${INC_CONTROLLER_DATA}
    ${INC_CONTROLLER_IO}
    ${INC_CONTROLLER_NETWORK}
    ${INC_CONTROLLER_ERROR}
)
set(SOURCES
    ${SRC_MODEL}
    ${SRC_MODEL_CORE}
    ${SRC_MODEL_DTO}
    ${SRC_UTILS}
    ${SRC_UTILS_CONFIG}
    ${SRC_VIEW_MODEL_CONTROLLER}
    ${SRC_VIEW_COMPONENTS}
    ${SRC_VIEW_DELEGATES}
    ${SRC_VIEW_EDITORS}
    ${SRC_VIEW_PAGES}
    ${SRC_CONTROLLER}
    ${SRC_CONTROLLER_ASYNC}
    ${SRC_CONTROLLER_AUTH}
    ${SRC_CONTROLLER_DATA}
    ${SRC_CONTROLLER_IO}
    ${SRC_CONTROLLER_NETWORK}
    ${SRC_CONTROLLER_ERROR}
)

### PROJECT AND SOURCE STRUCTURE ##############################################

# Organize projects into folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Generate folders to group source files into
source_group("Model" FILES ${SRC_MODEL} ${INC_MODEL})
source_group("Model\\Core" FILES ${SRC_MODEL_CORE} ${INC_MODEL_CORE})
source_group("Model\\DTO" FILES ${SRC_MODEL_DTO} ${INC_MODEL_DTO})
source_group("Utils" FILES ${SRC_UTILS} ${INC_UTILS})
source_group("Utils\\Config" FILES ${SRC_UTILS_CONFIG} ${INC_UTILS_CONFIG})
source_group("View\\Components" FILES ${SRC_VIEW_COMPONENTS} ${INC_VIEW_COMPONENTS})
source_group("View\\Delegates" FILES ${SRC_VIEW_DELEGATES} ${INC_VIEW_DELEGATES})
source_group("View\\Editors" FILES ${SRC_VIEW_EDITORS} ${INC_VIEW_EDITORS})
source_group("View\\Pages" FILES ${SRC_VIEW_PAGES} ${INC_VIEW_PAGES})
source_group("View\\Controllers" FILES ${SRC_VIEW_MODEL_CONTROLLER} ${INC_VIEW_MODEL_CONTROLLER})
source_group("Controller" FILES ${SRC_CONTROLLER} ${INC_CONTROLLER})
source_group("Controller\\Async" FILES ${SRC_CONTROLLER_ASYNC} ${INC_CONTROLLER_ASYNC})
source_group("Controller\\Auth" FILES ${SRC_CONTROLLER_AUTH} ${INC_CONTROLLER_AUTH})
source_group("Controller\\Data" FILES ${SRC_CONTROLLER_DATA} ${INC_CONTROLLER_DATA})
source_group("Controller\\Io" FILES ${SRC_CONTROLLER_IO} ${INC_CONTROLLER_IO})
source_group("Controller\\Link" FILES ${SRC_CONTROLLER_LINK} ${INC_CONTROLLER_LINK})
source_group("Controller\\Network" FILES ${SRC_CONTROLLER_NETWORK} ${INC_CONTROLLER_NETWORK})
source_group("Controller\\Error" FILES ${SRC_CONTROLLER_ERROR} ${INC_CONTROLLER_ERROR})

###PACKAGES######################################

use_qt5lib(Qt5LinguistTools)
use_qt5lib(Qt5Widgets)
use_qt5lib(Qt5WebKit)
use_qt5lib(Qt5WebKitWidgets)
use_qt5lib(Qt5Network)
use_qt5lib(Qt5Core)
use_qt5lib(Qt5Gui)
use_qt5lib(Qt5OpenGL)

find_package(OpenGL REQUIRED)
if(OPENGL_FOUND)
    message(STATUS "OpenGL Correctly Found")
    include_directories(${OPENGL_INCLUDE_DIR})
    mark_as_advanced(${OPENGL_LIBRARY})
else(OPENGL_FOUND)
    message(FATAL_ERROR "OpenGL environment missing")
endif(OPENGL_FOUND)

# if(APPLE)
#     find_library(AppKit_LIBRARY AppKit)
#     find_library(COCOA_LIBRARY Cocoa)
#     find_library(APP_SERVICES_LIBRARY ApplicationServices)
#     mark_as_advanced(APP_SERVICES_LIBRARY COCOA_LIBRARY AppKit_LIBRARY) 
#     set(MAC_EXTRA_LIBS ${APP_SERVICES_LIBRARY} ${COCOA_LIBRARY} ${AppKit_LIBRARY}) 
# endif()

##FLAGS AND DEFINITIONS###

add_definitions(-DPROJECT_VERSION=\"${PROJECT_VERSION}\")

##BUILDING AND INSTALLING #############################################################

# put everything that is generated into the binary dir
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_BINARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

#QT5 DIRECTIVES
set(FORMS
    "${PROJECT_SOURCE_DIR}/assets/ui/aboutdialog.ui"
    "${PROJECT_SOURCE_DIR}/assets/ui/cellview.ui"
    "${PROJECT_SOURCE_DIR}/assets/ui/datasets.ui"
    "${PROJECT_SOURCE_DIR}/assets/ui/experiments.ui"
    "${PROJECT_SOURCE_DIR}/assets/ui/initpage.ui"
    "${PROJECT_SOURCE_DIR}/assets/ui/selectiondialog.ui"
    "${PROJECT_SOURCE_DIR}/assets/ui/login.ui"
)

set(LOCALES "${PROJECT_SOURCE_DIR}/assets/trans/locale_en_us.ts")
              
set(RESOURCES "${PROJECT_BINARY_DIR}/application.qrc")

qt5_wrap_ui(QT_FORMS ${FORMS})
qt5_add_resources(QT_RESOURCES ${RESOURCES})
qt5_add_translation(QT_LOCALES ${LOCALES})

###STATIC LIBRARIES#####################################

include("libs/QtDebugHelper/CMakeLists.cmake")
include("libs/QtSingleApplication/CMakeLists.cmake")
include("libs/QtUnitTest/CMakeLists.cmake")
include("libs/STCore/CMakeLists.cmake")
#include("libs/STCoreWidgets/CMakeLists.cmake")
include("libs/STOpenGL/CMakeLists.cmake")
include("libs/libqxt/CMakeLists.cmake")
set(LIB_COMPONENTS QtDebugHelper QtSingleApplication STOpenGL STCore libqxt) #STCoreWidgets #

## create main library
add_library(STMain STATIC ${SOURCES} ${QT_FORMS} ${QT_LOCALES}) #${INCLUDES}

target_link_libraries(STMain ${LIB_COMPONENTS})

## link qt libraries to main library
qt5_use_modules(STMain Core Gui Widgets WebKit WebKitWidgets OpenGL Network LinguistTools)

#create main target (resources needs to be part of the target)
if(WIN32)
  add_executable(${PROJECT_NAME} WIN32 main.cpp ${QT_RESOURCES} )
elseif(APPLE)
  add_executable(${PROJECT_NAME} MACOSX_BUNDLE main.cpp ${QT_RESOURCES} )
else()
  add_executable(${PROJECT_NAME} main.cpp ${QT_RESOURCES} )
endif()

# Platform specific library includes
if(WIN32)
    target_link_libraries(${PROJECT_NAME} ${Qt5Core_QTMAIN_LIBRARIES})
endif()

# if(APPLE)
#    target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARY} ${MAC_EXTRA_LIBS}) 
# endif()

#link libraries to the main target
target_link_libraries(${PROJECT_NAME} STMain )

### UNIT TESTS ################################################################

enable_testing()
add_subdirectory(unit)

##################################################################INSTALLATION###########################################################################

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

    #TODO replace TGZ for DEB package

    # Select TGZ as the packager on Linux
    set(CPACK_GENERATOR "TGZ")
    set(CPACK_SYSTEM_NAME "Linux")
    set(CPACK_SYSTEM_NAME "${CMAKE_BUILD_TYPE}-${CPACK_SYSTEM_NAME}-32")

    #qt libraries
    #qt 5.1.1 needs qtsensors
    foreach(QT_LIBRARY Concurrent Core DBus Gui Help Network OpenGL PrintSupport Svg WebKit WebKitWidgets Widgets CLucene Sql Xml Qml V8 Quick Sensors)
        install(FILES ${QT_LIBRARY_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}Qt${QT_VERSION_MAJOR}${QT_LIBRARY}${CMAKE_SHARED_LIBRARY_SUFFIX}.${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}
                DESTINATION lib
                RENAME ${CMAKE_SHARED_LIBRARY_PREFIX}Qt${QT_VERSION_MAJOR}${QT_LIBRARY}${CMAKE_SHARED_LIBRARY_SUFFIX}.${QT_VERSION_MAJOR})
    endforeach()

    set(ICU_VERSION "51.1") #qt5.1.1 icu is 51 for lower versions of qt5 icu is 49

    #qt needs ICU libaries in Linux
    foreach(QT_FILE icudata icui18n icuuc)
        install(FILES ${QT_LIBRARY_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}${QT_FILE}${CMAKE_SHARED_LIBRARY_SUFFIX}.${ICU_VERSION}
                DESTINATION lib
                RENAME ${CMAKE_SHARED_LIBRARY_PREFIX}${QT_FILE}${CMAKE_SHARED_LIBRARY_SUFFIX}.5)
    endforeach()

    # Qt plugins required by stVi
    LINUX_DEPLOY_QT_PLUGIN(accessible qtaccessiblequick qtaccessiblewidgets)
    LINUX_DEPLOY_QT_PLUGIN(imageformats qgif qico qjpeg qmng qsvg qtga qtiff qwbmp)
    LINUX_DEPLOY_QT_PLUGIN(platforms qlinuxfb qminimal qxcb)
    LINUX_DEPLOY_QT_PLUGIN(printsupport cupsprintersupport)

    #install libraries and resources
    install(FILES ${CMAKE_SOURCE_DIR}/cmake/linux/qt.conf DESTINATION bin)
    install(TARGETS ${PROJECT_NAME} DESTINATION bin)
    install(FILES ${QT_LOCALES} DESTINATION share/${PROJECT_NAME}/translations)
    install(FILES ${CMAKE_SOURCE_DIR}/LICENSE DESTINATION .)
#     install(FILES ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}} DESTINATION lib )

    # Shell script to run stVi
    set(ShellFile ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.sh)
    configure_file(${CMAKE_SOURCE_DIR}/cmake/linux/application.sh.in ${ShellFile})

    install(FILES ${ShellFile} RENAME ${PROJECT_NAME}
            DESTINATION .
            PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

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
        COMMAND mkdir -p ${CMAKE_INSTALL_PREFIX}/Resources/translations
        COMMAND install -m 644 ${CMAKE_SOURCE_DIR}/assets/images/stVi.icns ${CMAKE_INSTALL_PREFIX}/Resources/
        COMMAND install -m 644 ${QT_LOCALES} ${CMAKE_INSTALL_PREFIX}/Resources/translations/
#         COMMAND install -m 644 ${CMAKE_SOURCE_DIR}/cmake/mac/qt.conf ${CMAKE_INSTALL_PREFIX}/Resources/
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
    
    
    #TODO  Refactor and optmize this!!
    
    # Qt libraries required by stVi
    if(CMAKE_BUILD_TYPE MATCHES [Dd][Ee][Bb][Uu][Gg])
    
        WINDOWS_DEPLOY_QT_LIBRARIES(Qt5CLucened Qt5Cored Qt5Guid Qt5Helpd Qt5Multimediad
                                    Qt5MultimediaWidgetsd Qt5Networkd Qt5OpenGLd
                                    Qt5PrintSupportd Qt5Qmld Qt5Quickd Qt5Sqld Qt5Svgd Qt5V8d
                                    Qt5WebKitd Qt5WebKitWidgetsd Qt5Widgetsd Qt5Xmld Qt5Sensorsd)

        # Qt plugins required by stVi
        WINDOWS_DEPLOY_QT_PLUGIN(accessible qtaccessiblequickd qtaccessiblewidgetsd)
        WINDOWS_DEPLOY_QT_PLUGIN(imageformats qgifd qicod qjpegd qmngd qsvgd qtgad qtiffd qwbmpd)
        WINDOWS_DEPLOY_QT_PLUGIN(printsupport windowsprintersupportd)
        set(PLUGINS qminimald qwindowsd)
        
    else()
    
        WINDOWS_DEPLOY_QT_LIBRARIES(Qt5CLucene Qt5Core Qt5Gui Qt5Help Qt5Multimedia
                                    Qt5MultimediaWidgets Qt5Network Qt5OpenGL
                                    Qt5PrintSupport Qt5Qml Qt5Quick Qt5Sql Qt5Svg
                                    Qt5V8 Qt5WebKit Qt5WebKitWidgets Qt5Widgets Qt5Xml Qt5Sensors)

        # Qt plugins required by stVi
        WINDOWS_DEPLOY_QT_PLUGIN(accessible qtaccessiblequick qtaccessiblewidgets)
        WINDOWS_DEPLOY_QT_PLUGIN(imageformats qgif qico qjpeg qmng qsvg qtga qtiff qwbmp)
        WINDOWS_DEPLOY_QT_PLUGIN(printsupport windowsprintersupport)
        set(PLUGINS qminimal qwindows)
        
    endif()
    
    #qt5.1.1 icu is 51 for lower versions is 49
    WINDOWS_DEPLOY_QT_LIBRARIES(icudt51 icuin51 icuuc51)
    
    #install platforms plugins
    foreach(QT_PLUGIN_FILE ${PLUGINS})

        install(FILES ${QT_PLUGINS_DIR}/platforms/${CMAKE_SHARED_LIBRARY_PREFIX}${QT_PLUGIN_FILE}${CMAKE_SHARED_LIBRARY_SUFFIX}
                DESTINATION platforms)
		add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                                   COMMAND ${CMAKE_COMMAND} -E copy ${QT_PLUGINS_DIR}/platforms/${CMAKE_SHARED_LIBRARY_PREFIX}${QT_PLUGIN_FILE}${CMAKE_SHARED_LIBRARY_SUFFIX}
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
    install(FILES ${QT_LOCALES} DESTINATION translations)
    install(FILES ${CMAKE_SOURCE_DIR}/cmake/win/qt.conf DESTINATION . )   
    install(FILES ${CMAKE_SOURCE_DIR}/cmake/win/libeay32.dll DESTINATION . )
    install(FILES ${CMAKE_SOURCE_DIR}/cmake/win/ssleay32.dll DESTINATION . )

endif(WIN32)

include(CPack)
