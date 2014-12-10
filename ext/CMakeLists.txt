add_subdirectory(libqxt)
add_subdirectory(qcustomplot)
add_subdirectory(qjpeg-turbo)
#ugly trick to pass the scope of the libjpeg library to root
set(LIBJPEG_LIBRARIES ${LIBJPEG_LIBRARIES} PARENT_SCOPE)
