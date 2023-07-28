#pragma once

#include "PDB_TPIStream.h"
#include "PDB_CoalescedMSFStream.h"

class TypeTable
{
public:
	explicit TypeTable(const PDB::TPIStream& tpiStream) PDB_NO_EXCEPT;
	~TypeTable() PDB_NO_EXCEPT;

	// Returns the index of the first type, which is not necessarily zero.
	PDB_NO_DISCARD inline uint32_t GetFirstTypeIndex(void) const PDB_NO_EXCEPT
	{
		return typeIndexBegin;
	}

	// Returns the index of the last type.
	PDB_NO_DISCARD inline uint32_t GetLastTypeIndex(void) const PDB_NO_EXCEPT
	{
		return typeIndexEnd;
	}

	PDB_NO_DISCARD inline const PDB::CodeView::TPI::Record* GetTypeRecord(uint32_t typeIndex) const PDB_NO_EXCEPT
	{
		if (typeIndex < typeIndexBegin || typeIndex > typeIndexEnd)
			return nullptr;

		return m_records[typeIndex - typeIndexBegin];
	}

	// Returns a view of all type records.
	// Records identified by a type index can be accessed via "allRecords[typeIndex - firstTypeIndex]".
	PDB_NO_DISCARD inline PDB::ArrayView<const PDB::CodeView::TPI::Record*> GetTypeRecords(void) const PDB_NO_EXCEPT
	{
		return PDB::ArrayView<const PDB::CodeView::TPI::Record*>(m_records, m_recordCount);
	}

private:
	uint32_t typeIndexBegin;
	uint32_t typeIndexEnd;

	size_t m_recordCount;
	const PDB::CodeView::TPI::Record **m_records;

	PDB::CoalescedMSFStream m_stream;

	PDB_DISABLE_COPY(TypeTable);
};
