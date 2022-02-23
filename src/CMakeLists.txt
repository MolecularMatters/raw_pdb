project(raw_pdb)

set(SOURCES
    PDB.cpp
    PDB.h
    PDB_ArrayView.h
    PDB_Assert.h
    PDB_BitUtil.h
    PDB_CoalescedMSFStream.cpp
    PDB_CoalescedMSFStream.h
    PDB_DBIStream.cpp
    PDB_DBIStream.h
    PDB_DBITypes.h
    PDB_DirectMSFStream.cpp
    PDB_DirectMSFStream.h
    PDB_DisableWarningsPop.h
    PDB_DisableWarningsPush.h
    PDB_Forward.h
    PDB_GlobalSymbolStream.cpp
    PDB_GlobalSymbolStream.h
    PDB_ImageSectionStream.cpp
    PDB_ImageSectionStream.h
    PDB_InfoStream.cpp
    PDB_InfoStream.h
    PDB_IPIStream.cpp
    PDB_IPIStream.h
    PDB_IPITypes.h
    PDB_Log.h
    PDB_Memory.h
    PDB_ModuleInfoStream.cpp
    PDB_ModuleInfoStream.h
    PDB_ModuleSymbolStream.cpp
    PDB_ModuleSymbolStream.h
    PDB_Move.h
    PDB_PCH.cpp
    PDB_PCH.h
    PDB_PointerUtil.h
    PDB_PublicSymbolStream.cpp
    PDB_PublicSymbolStream.h
    PDB_RawFile.cpp
    PDB_RawFile.h
    PDB_SectionContributionStream.cpp
    PDB_SectionContributionStream.h
    PDB_SourceFileStream.cpp
    PDB_SourceFileStream.h
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
  PUBLIC
    PDB_PCH.h
)

add_subdirectory(Examples)