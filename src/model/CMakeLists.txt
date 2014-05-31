FIX_QT3D_COMPILATION_BUG()

set(LIBRARY_ARG_INCLUDES
    DatasetItemModel.h
    ExperimentsItemModel.h
    GeneFeatureItemModel.h
    GeneSelectionItemModel.h
    SortGenesProxyModel.h
    CompareGeneNames.h
    GeneNamesModel.h
)

set(LIBRARY_ARG_SOURCES
    DatasetItemModel.cpp
    ExperimentsItemModel.cpp
    GeneFeatureItemModel.cpp
    GeneSelectionItemModel.cpp
    SortGenesProxyModel.cpp
    CompareGeneNames.cpp
    GeneNamesModel.cpp
)

set(LIBRARY_ARG_UI_FILES
)

ST_LIBRARY()
