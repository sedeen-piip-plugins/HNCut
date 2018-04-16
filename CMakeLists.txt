PROJECT( HNCut )
CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )

set(CMAKE_CXX_STANDARD 17)

## Load the Sedeen dependencies
FIND_PACKAGE( SEDEENSDK REQUIRED 
                HINTS ../../.. 
                "$ENV{PROGRAMFILES}/Sedeen Viewer SDK/v5.3.0.2/msvc2017" )


INCLUDE_DIRECTORIES( "${SEDEENSDK_INCLUDE_DIR}" )

LINK_DIRECTORIES( "${SEDEENSDK_LIBRARY_DIR}" )

##
## Build the code into a module library
ADD_LIBRARY( HNCut MODULE HNCut.cpp HNCut.h)

# Link the library against the Sedeen libraries
TARGET_LINK_LIBRARIES( HNCut ${SEDEENSDK_LIBRARIES} )

##
## Install the plugin in the sedeen plugins directory
IF( ${SEDEEN_FOUND} )
  INSTALL( TARGETS HNCut 
           LIBRARY DESTINATION "${SEDEEN_DIR}/plugins" )
ENDIF()
