// Copyright 2011-2022, Molecular Matters GmbH <office@molecular-matters.com>
// See LICENSE.txt for licensing details (2-clause BSD License: https://opensource.org/licenses/BSD-2-Clause)

#include "Examples_PCH.h"
#include "ExampleTypeTable.h"
#include "Foundation/PDB_Memory.h"

TypeTable::TypeTable(const PDB::TPIStream& tpiStream) PDB_NO_EXCEPT
	: typeIndexBegin(tpiStream.GetFirstTypeIndex()), typeIndexEnd(tpiStream.GetLastTypeIndex()),
	m_recordCount(tpiStream.GetTypeRecordCount())
{
	// Create coalesced stream from TPI stream, so the records can be referenced directly using pointers.
	const PDB::DirectMSFStream& directStream = tpiStream.GetDirectMSFStream();
	m_stream = PDB::CoalescedMSFStream(directStream, directStream.GetSize(), 0);

	// types in the TPI stream are accessed by their index from other streams.
	// however, the index is not stored with types in the TPI stream directly, but has to be built while walking the stream.
	// similarly, because types are variable-length records, there are no direct offsets to access individual types.
	// we therefore walk the TPI stream once, and store pointers to the records for trivial O(1) array lookup by index later.	
	m_records = PDB_NEW_ARRAY(const PDB::CodeView::TPI::Record*, m_recordCount);

	// parse the CodeView records
	uint32_t typeIndex = 0u;

	tpiStream.ForEachTypeRecordOffset([this, &typeIndex](size_t offset)
		{
			const PDB::CodeView::TPI::Record* record = m_stream.GetDataAtOffset<const PDB::CodeView::TPI::Record>(offset);
			m_records[typeIndex] = record;
			++typeIndex;
		});
}

TypeTable::~TypeTable() PDB_NO_EXCEPT
{
	PDB_DELETE_ARRAY(m_records);
}
