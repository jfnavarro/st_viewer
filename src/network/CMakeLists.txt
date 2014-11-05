set(LIBRARY_ARG_INCLUDES
    NetworkCommand.h
    NetworkManager.h
    NetworkReply.h
    RESTCommandFactory.h
    NetworkDiskCache.h
)

set(LIBRARY_ARG_SOURCES
    NetworkCommand.cpp
    NetworkManager.cpp
    NetworkReply.cpp
    RESTCommandFactory.cpp
    NetworkDiskCache.cpp
)

set(LIBRARY_ARG_UI_FILES)
ST_LIBRARY()
