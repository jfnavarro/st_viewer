include_directories(${PROJECT_SOURCE_DIR}/ext;${PROJECT_SOURCE_DIR}/ext/japy;${PROJECT_SOURCE_DIR}/ext/picojson)

set(LIBRARY_ARG_INCLUDES
    NetworkCommand.h
    NetworkManager.h
    NetworkReply.h
    RESTCommandFactory.h
    DownloadManager.h
)

set(LIBRARY_ARG_SOURCES
    NetworkCommand.cpp
    NetworkManager.cpp
    NetworkReply.cpp
    RESTCommandFactory.cpp
    DownloadManager.cpp
)

set(LIBRARY_ARG_UI_FILES)

ST_LIBRARY()
