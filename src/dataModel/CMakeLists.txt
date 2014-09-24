set(LIBRARY_ARG_INCLUDES
    Chip.h
    Dataset.h
    Feature.h
    Gene.h
    User.h
    GeneSelection.h
    ImageAlignment.h
    ChipDTO.h
    DatasetDTO.h
    ErrorDTO.h
    FeatureDTO.h
    GeneSelectionDTO.h
    ImageAlignmentDTO.h
    OAuth2TokenDTO.h
    UserDTO.h
    MinVersionDTO.h
    LastModifiedDTO.h
    WrappedFileDTO.h
)

set(LIBRARY_ARG_SOURCES
    Chip.cpp
    Dataset.cpp
    Feature.cpp
    Gene.cpp
    User.cpp
    GeneSelection.cpp
    ImageAlignment.cpp
)

set(LIBRARY_ARG_UI_FILES
)

ST_LIBRARY()
