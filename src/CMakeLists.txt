set(SOURCES
	Foundation/PDB_ArrayView.h
	Foundation/PDB_Assert.h
	Foundation/PDB_BitOperators.h
	Foundation/PDB_BitUtil.h
	Foundation/PDB_CRT.h
	Foundation/PDB_Forward.h
	Foundation/PDB_Log.h
	Foundation/PDB_Macros.h
	Foundation/PDB_Memory.h
	Foundation/PDB_Move.h
	Foundation/PDB_Platform.h
	Foundation/PDB_PointerUtil.h
	Foundation/PDB_TypeTraits.h
	Foundation/PDB_Warnings.h
	
	PDB.cpp
	PDB.h
	PDB_CoalescedMSFStream.cpp
	PDB_CoalescedMSFStream.h
	PDB_DBIStream.cpp
	PDB_DBIStream.h
	PDB_DBITypes.cpp
	PDB_DBITypes.h
	PDB_DirectMSFStream.cpp
	PDB_DirectMSFStream.h
	PDB_ErrorCodes.h
	PDB_GlobalSymbolStream.cpp
	PDB_GlobalSymbolStream.h
	PDB_ImageSectionStream.cpp
	PDB_ImageSectionStream.h
	PDB_InfoStream.cpp
	PDB_InfoStream.h
	PDB_IPIStream.cpp
	PDB_IPIStream.h
	PDB_IPITypes.h
	PDB_ModuleInfoStream.cpp
	PDB_ModuleInfoStream.h
	PDB_ModuleLineStream.cpp
	PDB_ModuleLineStream.h
	PDB_ModuleSymbolStream.cpp
	PDB_ModuleSymbolStream.h
	PDB_NamesStream.cpp
	PDB_NamesStream.h
	PDB_PCH.cpp
	PDB_PCH.h
	PDB_PublicSymbolStream.cpp
	PDB_PublicSymbolStream.h
	PDB_RawFile.cpp
	PDB_RawFile.h
	PDB_SectionContributionStream.cpp
	PDB_SectionContributionStream.h
	PDB_SourceFileStream.cpp
	PDB_SourceFileStream.h
	PDB_TPIStream.cpp
	PDB_TPIStream.h
	PDB_TPITypes.h
	PDB_Types.cpp
	PDB_Types.h
	PDB_Util.h
)

source_group(src FILES
    ${SOURCES}
)

add_library(raw_pdb
    ${SOURCES}
)

target_include_directories(raw_pdb
  PUBLIC
    .
)

target_precompile_headers(raw_pdb
  PRIVATE
    PDB_PCH.h
)

option(RAWPDB_BUILD_EXAMPLES "Build Examples" ON)

if (RAWPDB_BUILD_EXAMPLES)
	add_subdirectory(Examples)
endif()

if (UNIX)
	include(GNUInstallDirs)

	install(
		TARGETS raw_pdb
		LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
	)

	file(GLOB_RECURSE HEADER_FILES
		"${CMAKE_CURRENT_SOURCE_DIR}/*.h"
	)

	file(GLOB_RECURSE HEADER_FILES_FOUNDATION
		"${CMAKE_CURRENT_SOURCE_DIR}/Foundation/*.h"
	)

	install(
		FILES ${HEADER_FILES}
		DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/raw_pdb/"
	)

	install(
		FILES ${HEADER_FILES_FOUNDATION}
		DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/raw_pdb/Foundation"
	)
endif (UNIX)
