#
# Program:   MULTIMOD APPLICATION FRAMEWORK (MAF)
# Module:    $RCSfile: CMakeLists.txt,v $
# Language:  CMake 2.0
# Date:      $Date: 2011-09-28 09:28:59 $
# Version:   $Revision: 1.44.2.12 $
#
# Description:
# CMake Project file for the MAF.

CMAKE_MINIMUM_REQUIRED(VERSION 2.4) 

PROJECT (MAF)



IF(CMAKE_VERSION VERSION_LESS 2.8)
MESSAGE (FATAL_ERROR "CMAKE version must be greater than 2.8")
ENDIF (CMAKE_VERSION VERSION_LESS 2.8)

IF(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
ENDIF(COMMAND cmake_policy)

#-----------------------------------------------------------------------------
# This enables support for Visual Studio: Edit & Continue, OpenMP, and parallel compilation
#-----------------------------------------------------------------------------
IF (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
	#Add OpenMP Support
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /openmp")

	#Add support for Edit & Continue
	SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /ZI")
	IF (NOT ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 17.0)
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
		SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /SAFESEH:NO")
	ENDIF (NOT ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 17.0)
ENDIF (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)



IF(MSVC AND NOT (${CMAKE_VERSION} VERSION_LESS 2.8.11))
		# set stack reserved size to ~10MB
		# CMake previously automatically set this value for MSVC builds, but the
		# behavior was changed in CMake 2.8.11 (Issue 12437) to use the MSVC default
		# value (1 MB) which is not enough for us. Test mafOpExporterGRFWSTest fails 
		# due to stackoverflow 
		SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /STACK:10000000")
ENDIF()

#-----------------------------------------------------------------------------
# Macro for DEBUG logging
#-----------------------------------------------------------------------------
INCLUDE (${MAF_SOURCE_DIR}/modules/DebugMacro.cmake)
MARK_AS_ADVANCED (
  DEBUG_MESSAGES
)

OPTION(DEBUG_MESSAGES "Enable debug Messages" OFF)

#-----------------------------------------------------------------------------
# this define the MAF_SUBPROJECT macro
#-----------------------------------------------------------------------------
INCLUDE (${MAF_SOURCE_DIR}/modules/MAFConfigMacro.cmake)

#-----------------------------------------------------------------------------
# where to place output binaries
#-----------------------------------------------------------------------------
SET (LIBRARY_OUTPUT_PATH ${MAF_BINARY_DIR}/bin/ CACHE INTERNAL
     "Single output directory for building all libraries.")
SET (EXECUTABLE_OUTPUT_PATH ${MAF_BINARY_DIR}/bin/ CACHE INTERNAL 
     "Single output directory for building all executables.")

MARK_AS_ADVANCED (
  LIBRARY_OUTPUT_PATH 
  #EXECUTABLE_OUTPUT_PATH
)

#-----------------------------------------------------------------------------
# MAF version number.  An even minor number corresponds to releases.
#-----------------------------------------------------------------------------
SET(MAF_MAJOR_VERSION 2)
SET(MAF_MINOR_VERSION 2)
SET(MAF_BUILD_VERSION 0)
SET(MAF_VERSION
    "${MAF_MAJOR_VERSION}.${MAF_MINOR_VERSION}.${MAF_BUILD_VERSION}")

#-----------------------------------------------------------------------------
# this is to enable precompiled headers under VC6.0
#-----------------------------------------------------------------------------
IF(WIN32)
  IF (NOT UNIX)
      SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /YX" CACHE STRING "Compiler flags")
  ENDIF (NOT UNIX)
ENDIF(WIN32)

#-----------------------------------------------------------------------------
# This enables support for Visual Studio Edit & Continue, OpenMP, and parallel compilation
#-----------------------------------------------------------------------------
IF (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
	#Add OpenMP Support
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /openmp")

	#Add support for Edit & Continue
	SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /ZI")
	IF (NOT ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 17.0)
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
	ENDIF (NOT ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 17.0)
ENDIF (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)


#-----------------------------------------------------------------------------
# Options for MAF building
#-----------------------------------------------------------------------------
#Remove temporarily the BUILD_SHARED_LIBS because it is not possible to switch the compilation to dynamic.
#SET(BUILD_SHARED_LIBS 0)
# OPTION(BUILD_SHARED_LIBS "Build with shared libraries." OFF )
OPTION(BUILD_EXAMPLE_APP "Build Example App." ON )
OPTION(BUILD_TESTING "Build the testing tree." OFF)
OPTION(MAF_BUILD_MAFDLL "Build mafDLL.dll." OFF)
OPTION(MAF_QA "Generate QA" OFF)

FIND_FILE( VISUAL_STUDIO_RUNTIME vcredist_x86.exe )
STRING(REPLACE "/" "\\" VISUAL_STUDIO_RUNTIME ${VISUAL_STUDIO_RUNTIME})

# MARK_AS_ADVANCED (
  # BUILD_SHARED_LIBS
# )

IF(MAF_BUILD_MAFDLL)
  SET(MAF_BUILD_MAFDLL 1)
ENDIF (MAF_BUILD_MAFDLL)

SET (CONFIGURE_ERROR 0)

#-----------------------------------------------------------------------------
# Platform configuration tests.
#-----------------------------------------------------------------------------
INCLUDE (${CMAKE_ROOT}/Modules/FindThreads.cmake)

# Create the header to configure the classes.
CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/mafConfigure.h.in
             ${PROJECT_BINARY_DIR}/mafConfigure.h
             @ONLY)


#-----------------------------------------------------------------------------
# Installer configuration
#-----------------------------------------------------------------------------
if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
  set( INSTALLER_ARCH "x64" )
else( CMAKE_SIZEOF_VOID_P EQUAL 8 )
  set( INSTALLER_ARCH "x86" )
endif( CMAKE_SIZEOF_VOID_P EQUAL 8 )

CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/Installer/EnumUsersReg.nsh ${PROJECT_BINARY_DIR}/Installer/EnumUsersReg.nsh @ONLY)
CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/Installer/InstallerCommon.nsh.in ${PROJECT_BINARY_DIR}/Installer/InstallerCommon.nsh @ONLY)
			 
#-----------------------------------------------------------------------------
# Check if MFL is inside the MAF tree.
#-----------------------------------------------------------------------------
MAF_SUBPROJECT(MFL Libraries)

IF (MFL_CONFIG_FOUND)
  FIND_FILE (USE_MFL_FILE UseMFL.cmake "${MAF_BINARY_DIR}/Libraries") 
  IF (USE_MFL_FILE)
    DBG_MESSAGE("MAF: MFL Configured")
    SET (MAF_CAN_BUILD 1)
  ENDIF (USE_MFL_FILE)   
ELSE (MFL_CONFIG_FOUND)
  # Search for MFL outside the MAF tree: MFL must be already built!
  # If found the USE_MFL_FILE variable is set 
  MESSAGE(STATUS "MAF: Searching for MFL libraries")
  INCLUDE (${MAF_SOURCE_DIR}/modules/FindMFL.cmake)
  IF (MFL_FOUND)
    MESSAGE(STATUS "MAF: Found MFL use file at: ${USE_MFL_FILE}")  
    SET (MAF_CAN_BUILD 1)
  ENDIF (MFL_FOUND)
ENDIF (MFL_CONFIG_FOUND)

#-----------------------------------------------------------------------------
# If MFL found or configure continue with MAF Configuration
#-----------------------------------------------------------------------------
IF (MAF_CAN_BUILD)
  # include all necessary for using the MFL
  INCLUDE (${USE_MFL_FILE})

  # Add single dirs to inclusion path
  INCLUDE_DIRECTORIES(BEFORE ".")

  ###############add all subdirs but "modules"
  SET(INCLUDE_DIRS "${MAF_BINARY_DIR}" "${MAF_SOURCE_DIR}/Base" "${MAF_SOURCE_DIR}/Common" "${MAF_BINARY_DIR}/Interaction" "${MAF_SOURCE_DIR}/Interaction" "${MAF_SOURCE_DIR}/Interaction/Drivers" 
              "${MAF_SOURCE_DIR}/Core" "${MAF_SOURCE_DIR}/Service" "${MAF_SOURCE_DIR}/Gui" "${MAF_SOURCE_DIR}/IO" "${MAF_SOURCE_DIR}/Operations" "${MAF_SOURCE_DIR}/VME" "${MAF_SOURCE_DIR}/VisualPipes" "${MAF_SOURCE_DIR}/Views"
              "${MAF_SOURCE_DIR}/VME/DataPipes" "${MAF_SOURCE_DIR}/vtkMAF" "${MAF_BINARY_DIR}/vtkMAF"  "${MAF_SOURCE_DIR}/GPUAPI" 
              "${MAF_SOURCE_DIR}/GPUAPI/GLEW" "${MAF_SOURCE_DIR}/GPUAPI/FBO")
  SET(INCLUDE_DIRS ${INCLUDE_DIRS} "${MAF_SOURCE_DIR}/mafDLL" "${MAF_BINARY_DIR}/Libraries/ITK/Sources/InsightToolkit-3.0.1/Wrapping/WrapITK/ExternalProjects/ItkVtkGlue/src")
	
	if(BUILD_TESTING)
	  SET(INCLUDE_DIRS ${INCLUDE_DIRS} "${MAF_SOURCE_DIR}/Testing" )
	endif(BUILD_TESTING)
  
  INCLUDE_DIRECTORIES(${INCLUDE_DIRS})
  

IF(WIN32)
	LINK_DIRECTORIES(${MAF_SOURCE_DIR}/GPUAPI/GLEW)
ENDIF(WIN32)

# IF(MAF_BUILD_MAFDLL)
  ################################
  #It's necessary to delete the previous files from the mafDLL project
  # FILE(GLOB h_to_delete "${MAF_SOURCE_DIR}/mafDLL/*.h")
  # FILE(GLOB cpp_to_delete "${MAF_SOURCE_DIR}/mafDLL/*.cpp")
  # FILE(GLOB cxx_to_delete "${MAF_SOURCE_DIR}/mafDLL/*.cxx")
  # FILE(GLOB txx_to_delete "${MAF_SOURCE_DIR}/mafDLL/*.txx")

  # FOREACH(f ${h_to_delete})
    # FILE(REMOVE "${f}")
  # ENDFOREACH(f ${h_to_delete})

  # FOREACH(f ${cpp_to_delete})
    # FILE(REMOVE "${f}")
  # ENDFOREACH(f ${cpp_to_delete})
  
  # FOREACH(f ${cxx_to_delete})
    # FILE(REMOVE "${f}")
  # ENDFOREACH(f ${cxx_to_delete})

  # FOREACH(f ${txx_to_delete})
    # FILE(REMOVE "${f}")
  # ENDFOREACH(f ${txx_to_delete})
  ################################
# ENDIF(MAF_BUILD_MAFDLL)

  # Configuring MAF kits
  DBG_MESSAGE("MAF: Configuring MAF Kits")
  MESSAGE(STATUS "MAF: Configuring MAF Kits")
  
  IF(MAF_BUILD_MAFDLL)
  
    IF(WIN32)
      SUBDIRS (GPUAPI)
    ENDIF(WIN32)
    IF (MAF_USE_VTK)
      SUBDIRS (vtkMAF)
    ENDIF (MAF_USE_VTK)
    SUBDIRS (mafDLL)
    
  ELSE (MAF_BUILD_MAFDLL)
  
    IF(WIN32)
      SUBDIRS (GPUAPI)
    ENDIF(WIN32)
      SUBDIRS (Base)
    IF (MAF_USE_VTK)
      SUBDIRS (Interaction)
    ENDIF (MAF_USE_VTK)	
    SUBDIRS (Gui)
    SUBDIRS (IO)
    SUBDIRS (Core)
	SUBDIRS (Service)
    IF (MAF_USE_VTK)
	  SUBDIRS (Common)
      SUBDIRS (vtkMAF)
      SUBDIRS (VME)
			SUBDIRS (VisualPipes)
	  SUBDIRS (Views)
      SUBDIRS (Operations)
    ENDIF (MAF_USE_VTK)
    
  ENDIF(MAF_BUILD_MAFDLL)

  SET(BUILD_SHARED_LIBS 0)
  # IF(BUILD_SHARED_LIBS)
    # SET(BUILD_SHARED_LIBS 1)
  # ENDIF (BUILD_SHARED_LIBS)
  # Configuring Doxygen documentation  
  SUBDIRS (Docs)
  
  IF(WIN32)
		# Disable automatic manifest generation.
		STRING(REPLACE "/MANIFEST" "/MANIFEST:NO" CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS})
		# Explicitly disable it since it is the default for newer versions of VS
    STRING(REGEX MATCH "MANIFEST:NO" tmp_manifest ${CMAKE_EXE_LINKER_FLAGS})
    IF(NOT tmp_manifest)
        SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MANIFEST:NO")
    ENDIF(NOT tmp_manifest)
	ENDIF(WIN32)

  # Export Build settings and library dependences
  INCLUDE(CMakeExportBuildSettings)
  
  CMAKE_EXPORT_BUILD_SETTINGS(
  ${MAF_BINARY_DIR}/MAFBuildSettings.cmake
  )
  
  IF(NOT (${CMAKE_VERSION} VERSION_LESS 3.0))
    cmake_policy(SET CMP0033 OLD)
  ENDIF(NOT (${CMAKE_VERSION} VERSION_LESS 3.0))
  
  EXPORT_LIBRARY_DEPENDENCIES(
  ${MAF_BINARY_DIR}/MAFLibraryDepends.cmake
  )

  CONFIGURE_FILE(
  ${MAF_SOURCE_DIR}/MAFConfig.cmake.in
  ${MAF_BINARY_DIR}/MAFConfig.cmake
  @ONLY IMMEDIATE
  )
  
  #### Include directory for binary distribution of MAF
  SET(MAF_INSTALL_INCLUDE_DIRS ${INCLUDE_DIRS})
  
  
  CONFIGURE_FILE(
  ${MAF_SOURCE_DIR}/MAFUse.cmake.in
  ${MAF_BINARY_DIR}/MAFUse.cmake
  @ONLY IMMEDIATE
  )

  #-----------------------------------------------------------------------------
  # Build examples too ?
  #-----------------------------------------------------------------------------
  IF (BUILD_EXAMPLE_APP AND MAF_CAN_BUILD)
    SUBDIRS(Examples)
  ENDIF (BUILD_EXAMPLE_APP AND MAF_CAN_BUILD)
  
  #-----------------------------------------------------------------------------
  # QA stuff
  #-----------------------------------------------------------------------------
  IF(MAF_QA)
    SUBDIRS(qa)
  ENDIF(MAF_QA)
  
  #-----------------------------------------------------------------------------
  # Testing stuff
  #-----------------------------------------------------------------------------
  
  # TestData MAF_DATA_ROOT should be filled even if BUILD_TESTING is set to off 
  FIND_PATH(MAF_DATA_ROOT "TestingDataRoot.txt" "${MAF_SOURCE_DIR}/Testing/unittestData" "${MAF_SOURCE_DIR}/../mmData/TestingData" "${MAF_SOURCE_DIR}/../unittestData" DOC "Test data repository")
    
  IF(BUILD_TESTING)
    INCLUDE (${CMAKE_ROOT}/Modules/Dart.cmake)
    ENABLE_TESTING()
  
    INCLUDE (${CMAKE_ROOT}/Modules/Dart.cmake)
    
    # where will executable tests be written ?
    IF (EXECUTABLE_OUTPUT_PATH)
        SET (CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})
    ELSE (EXECUTABLE_OUTPUT_PATH)
        SET (CXX_TEST_PATH .)
    ENDIF (EXECUTABLE_OUTPUT_PATH)
    
    SUBDIRS (Testing)
    
  ENDIF(BUILD_TESTING)
ELSE (MAF_CAN_BUILD)
  MESSAGE (FATAL_ERROR "MFL library not found or configuration error, cannot build MAF")
ENDIF (MAF_CAN_BUILD)

MARK_AS_ADVANCED(
   USE_MFL_FILE
)


#-----------------------------------------------------------------------------
# MAF_PARABUILD deploy stuff.  
#-----------------------------------------------------------------------------

# This is specific for automatic deploy of maf through Parabuild.
# A MAF_PARABUILD_VERSION variable is created to gather versioning information
      
# this values are set by hand when major or minor is released
SET(MAF_PARABUILD_MAJOR_VERSION 2 CACHE INTERNAL "major version")
SET(MAF_PARABUILD_MINOR_VERSION 2 CACHE INTERNAL "minor version")

# automatic values from parabuild created temporary environment variable
SET(MAF_PARABUILD_BUILD_TIMESTAMP $ENV{PARABUILD_BUILD_TIMESTAMP} CACHE INTERNAL "build timestamp")
SET(MAF_PARABUILD_BUILD_NUMBER $ENV{PARABUILD_BUILD_NUMBER} CACHE INTERNAL "build number")

# create the main version variable:
SET(MAF_PARABUILD_VERSION "${MAF_PARABUILD_MAJOR_VERSION}_${MAF_PARABUILD_MINOR_VERSION}_TS_${MAF_PARABUILD_BUILD_TIMESTAMP}_BUILD_${MAF_PARABUILD_BUILD_NUMBER}" CACHE INTERNAL "big version identifier concatenating other identifier")
 
#-----------------------------------------------------------------------------
# Write the list of variables and their values that are defined by CMake 
# when running this CMakeList file
#-----------------------------------------------------------------------------
IF (DEBUG_MESSAGES)
  GET_CMAKE_PROPERTY(VARS VARIABLES)
  FOREACH(var ${VARS})
  	FILE(APPEND ${CMAKE_CURRENT_BINARY_DIR}/AllVariables.txt 
  							"${var} \"${${var}}\"\n")
  ENDFOREACH(var ${VARS})
ENDIF (DEBUG_MESSAGES)
