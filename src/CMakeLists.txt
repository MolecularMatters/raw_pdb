set(SOURCES
	Foundation/PDB_ArrayView.h
	Foundation/PDB_Assert.h
	Foundation/PDB_BitOperators.h
	Foundation/PDB_BitUtil.h
	Foundation/PDB_DisableWarningsPop.h
	Foundation/PDB_DisableWarningsPush.h
	Foundation/PDB_Forward.h
	Foundation/PDB_Log.h
	Foundation/PDB_Macros.h
	Foundation/PDB_Memory.h
	Foundation/PDB_Move.h
	Foundation/PDB_Platform.h
	Foundation/PDB_PointerUtil.h
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
	PDB_ModuleSymbolStream.cpp
	PDB_ModuleSymbolStream.h
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

add_subdirectory(Examples)