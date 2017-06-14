set(LIBRARY_ARG_INCLUDES
    QuadTreeAABB.h
    QuadTree.h
    Common.h
    RInterface.h
    bhtsne/sptree.h
    bhtsne/tsne.h
)

set(LIBRARY_ARG_SOURCES
    QuadTreeAABB.cpp
    bhtsne/sptree.cpp
    bhtsne/tsne.cpp
)

set(LIBRARY_ARG_UI_FILES
)

st_library()



