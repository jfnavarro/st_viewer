# Files that make up QtMacExtrass

message(STATUS "Building QtMacExtras")

include_directories(${Qt5Gui_PRIVATE_INCLUDE_DIRS})

list(APPEND SOURCES
    ${CMAKE_SOURCE_DIR}/src/libs/QtMacExtrass/src/qmacfunctions.mm
    ${CMAKE_SOURCE_DIR}/src/libs/QtMacExtrass/src/qmaccocoaviewcontainer.mm
    ${CMAKE_SOURCE_DIR}/src/libs/QtMacExtrass/src/qmacfunctions_mac.mm
    ${CMAKE_SOURCE_DIR}/src/libs/QtMacExtrass/src/qmacnativetoolbar.mm
    ${CMAKE_SOURCE_DIR}/src/libs/QtMacExtrass/src/qmacnativewidget.mm
    ${CMAKE_SOURCE_DIR}/src/libs/QtMacExtrass/src/qmactoolbardelegate.mm
    ${CMAKE_SOURCE_DIR}/src/libs/QtMacExtrass/src/qmactoolbutton.mm
    ${CMAKE_SOURCE_DIR}/src/libs/QtMacExtrass/src/qnstoolbar.mm
    ${CMAKE_SOURCE_DIR}/src/libs/QtMacExtrass/src/qmacpasteboardmime.mm
)

# Include directory

include_directories(${CMAKE_SOURCE_DIR}/src/libs/QtMacExtrass/src)

##TODO make it a static library