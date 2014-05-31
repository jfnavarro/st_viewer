add_library(${lib_name} SHARED qtcolorpicker.cpp qtcolorpicker.h QtColorPicker)
target_link_libraries(${lib_name} ${QT_TARGET_LINK_LIBRARIES})


