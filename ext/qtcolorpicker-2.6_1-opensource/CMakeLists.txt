cmake_minimum_required(VERSION 2.8.12)
project(qtcolorpicker CXX)
include(CPack)

set(QT_LIBS 
    Core
    Gui  
    Widgets)

foreach(i ${QT_LIBS})
   find_package(Qt5${i} 5.3.0 REQUIRED)
   list(APPEND QT_TARGET_LINK_LIBRARIES Qt5::${i})
endforeach()

set(CMAKE_AUTOMOC ON)
set(lib_name ${PROJECT_NAME})

add_subdirectory(src)
add_subdirectory(examples)




