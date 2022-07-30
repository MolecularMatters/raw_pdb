// Copyright 2011-2022, Molecular Matters GmbH <office@molecular-matters.com>
// See LICENSE.txt for licensing details (2-clause BSD License: https://opensource.org/licenses/BSD-2-Clause)

#include "PDB_PCH.h"
#include "PDB_NamesStream.h"
#include "PDB_RawFile.h"


// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
PDB::NamesStream::NamesStream(void) PDB_NO_EXCEPT
	: m_stream()
	, m_header(nullptr)
	, m_stringTable(nullptr)
{
}


// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
PDB::NamesStream::NamesStream(const RawFile& file, uint32_t streamIndex) PDB_NO_EXCEPT
	: m_stream(file.CreateMSFStream<CoalescedMSFStream>(streamIndex))
	, m_header(m_stream.GetDataAtOffset<const NamesHeader>(0u))
	, m_stringTable(nullptr)
{
	// grab a pointer into the string table
	m_stringTable = m_stream.GetDataAtOffset<char>(sizeof(NamesHeader));
}
