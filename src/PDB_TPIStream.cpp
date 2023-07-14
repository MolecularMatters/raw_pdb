#include "PDB_PCH.h"
#include "PDB_TPIStream.h"
#include "PDB_RawFile.h"
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
	: m_stream()
	, m_header()
	, m_recordCount(0u)
{
}


// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
PDB::TPIStream::TPIStream(TPIStream&& other) PDB_NO_EXCEPT
	: m_stream(PDB_MOVE(other.m_stream))
	, m_header(PDB_MOVE(other.m_header))
	, m_recordCount(PDB_MOVE(other.m_recordCount))
{
	other.m_recordCount = 0u;
}


// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
PDB::TPIStream& PDB::TPIStream::operator=(TPIStream&& other) PDB_NO_EXCEPT
{
	if (this != &other)
	{
		m_stream = PDB_MOVE(other.m_stream);
		m_header = PDB_MOVE(other.m_header);
		m_recordCount = PDB_MOVE(other.m_recordCount);

		other.m_recordCount = 0u;
	}

	return *this;
}


// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
PDB::TPIStream::TPIStream(const RawFile& file) PDB_NO_EXCEPT
	: m_stream(file.CreateMSFStream<DirectMSFStream>(TPIStreamIndex)),
	  m_header(m_stream.ReadAtOffset<TPI::StreamHeader>(0u)),
	  m_recordCount(GetLastTypeIndex() - GetFirstTypeIndex())
{
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
	return TPIStream { file };
}
