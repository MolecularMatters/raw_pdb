#include "PDB_PCH.h"
#include "PDB_TPIStream.h"
#include "PDB_RawFile.h"
#include "PDB_Util.h"
#include "PDB_DirectMSFStream.h"
#include "Foundation/PDB_Memory.h"

namespace
{
	// the TPI stream always resides at index 2
	static constexpr const uint32_t TPIStreamIndex = 2u;
}


// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
PDB::TPIStream::TPIStream(void) PDB_NO_EXCEPT
	: m_header()
	, m_stream()
	, m_records(nullptr)
	, m_recordCount(0u)
{
}


// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
PDB::TPIStream::TPIStream(TPIStream&& other) PDB_NO_EXCEPT
	: m_header(PDB_MOVE(other.m_header))
	, m_stream(PDB_MOVE(other.m_stream))
	, m_records(PDB_MOVE(other.m_records))
	, m_recordCount(PDB_MOVE(other.m_recordCount))
{
	other.m_records = nullptr;
	other.m_recordCount = 0u;
}


// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
PDB::TPIStream& PDB::TPIStream::operator=(TPIStream&& other) PDB_NO_EXCEPT
{
	if (this != &other)
	{
		PDB_DELETE_ARRAY(m_records);

		m_header = PDB_MOVE(other.m_header);
		m_stream = PDB_MOVE(other.m_stream);
		m_records = PDB_MOVE(other.m_records);
		m_recordCount = PDB_MOVE(other.m_recordCount);

		other.m_records = nullptr;
		other.m_recordCount = 0u;
	}

	return *this;
}


// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
PDB::TPIStream::TPIStream(const RawFile& file, const TPI::StreamHeader& header) PDB_NO_EXCEPT
	: m_header(header)
	, m_stream(file.CreateMSFStream<CoalescedMSFStream>(TPIStreamIndex))
	, m_records(nullptr)
	, m_recordCount(GetLastTypeIndex() - GetFirstTypeIndex())
{
	// types in the TPI stream are accessed by their index from other streams.
	// however, the index is not stored with types in the TPI stream directly, but has to be built while walking the stream.
	// similarly, because types are variable-length records, there are no direct offsets to access individual types.
	// we therefore walk the TPI stream once, and store pointers to the records for trivial O(N) array lookup by index later.
	m_records = PDB_NEW_ARRAY(const CodeView::TPI::Record*, m_recordCount);

	// ignore the stream's header
	size_t offset = sizeof(TPI::StreamHeader);

	// parse the CodeView records
	uint32_t typeIndex = 0u;
	while (offset < m_stream.GetSize())
	{
		// https://llvm.org/docs/PDB/CodeViewTypes.html
		const CodeView::TPI::Record* record = m_stream.GetDataAtOffset<const CodeView::TPI::Record>(offset);

		const uint32_t recordSize = GetCodeViewRecordSize(record);
		m_records[typeIndex] = record;

		// position the stream offset at the next record
		offset += sizeof(CodeView::TPI::RecordHeader) + recordSize;

		++typeIndex;
	}
}


// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
PDB::TPIStream::~TPIStream(void) PDB_NO_EXCEPT
{
	PDB_DELETE_ARRAY(m_records);
}


// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
PDB_NO_DISCARD PDB::ErrorCode PDB::HasValidTPIStream(const RawFile& file) PDB_NO_EXCEPT
{
	DirectMSFStream stream = file.CreateMSFStream<DirectMSFStream>(TPIStreamIndex);
	if (stream.GetSize() < sizeof(TPI::StreamHeader))
	{
		return ErrorCode::InvalidStream;
	}

	const TPI::StreamHeader header = stream.ReadAtOffset<TPI::StreamHeader>(0u);
	if (header.version != TPI::StreamHeader::Version::V80)
	{
		return ErrorCode::UnknownVersion;
	}

	return ErrorCode::Success;
}


// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
PDB_NO_DISCARD PDB::TPIStream PDB::CreateTPIStream(const RawFile& file) PDB_NO_EXCEPT
{
	DirectMSFStream stream = file.CreateMSFStream<DirectMSFStream>(TPIStreamIndex);

	const TPI::StreamHeader header = stream.ReadAtOffset<TPI::StreamHeader>(0u);
	return TPIStream { file, header };
}
