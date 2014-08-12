set(LIBRARY_ARG_INCLUDES
    util/random.h
    data/sourcegenerator.h
    algorithm/linearizer.h
)

set(LIBRARY_ARG_SOURCES
    algorithm/breadthfirsttopdown.cpp
    data/asciigenerator.cpp
    data/binarygenerator.cpp
    testsuite.cpp
)

set(LIBRARY_ARG_UI_FILES
)

ST_LIBRARY()

