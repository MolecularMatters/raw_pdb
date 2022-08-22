// Copyright 2011-2022, Molecular Matters GmbH <office@molecular-matters.com>
// See LICENSE.txt for licensing details (2-clause BSD License: https://opensource.org/licenses/BSD-2-Clause)

#include "PDB_PCH.h"
#include "PDB_ModuleLineStream.h"
#include "PDB_RawFile.h"


// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
PDB::ModuleLineStream::ModuleLineStream(void) PDB_NO_EXCEPT
	: m_stream(), m_c13LineInfoOffset(0)
{
}


// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
PDB::ModuleLineStream::ModuleLineStream(const RawFile& file, uint16_t streamIndex, uint32_t streamSize, size_t c13LineInfoOffset) PDB_NO_EXCEPT
	: m_stream(file.CreateMSFStream<CoalescedMSFStream>(streamIndex, streamSize)), m_c13LineInfoOffset(c13LineInfoOffset)
{
	// https://llvm.org/docs/PDB/ModiStream.html
	// struct ModiStream {
	//	uint32_t Signature;
	//	uint8_t Symbols[SymbolSize - 4];
	//	uint8_t C11LineInfo[C11Size];
	//	uint8_t C13LineInfo[C13Size];
	//	uint32_t GlobalRefsSize;
	//	uint8_t GlobalRefs[GlobalRefsSize];
	// };
}
