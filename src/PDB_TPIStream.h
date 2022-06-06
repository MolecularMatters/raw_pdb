
#pragma once

#include "Foundation/PDB_Macros.h"
#include "Foundation/PDB_ArrayView.h"
#include "PDB_ErrorCodes.h"
#include "PDB_TPITypes.h"
#include "PDB_CoalescedMSFStream.h"

// PDB TPI stream
// https://llvm.org/docs/PDB/TpiStream.html
namespace PDB
{
	class RawFile;


	class PDB_NO_DISCARD TPIStream
	{
	public:
		TPIStream(void) PDB_NO_EXCEPT;
		TPIStream(TPIStream&& other) PDB_NO_EXCEPT;
		TPIStream& operator=(TPIStream&& other) PDB_NO_EXCEPT;

		explicit TPIStream(const RawFile& file, const TPI::StreamHeader& header) PDB_NO_EXCEPT;
		~TPIStream(void) PDB_NO_EXCEPT;

		// Returns the index of the first type, which is not necessarily zero.
		PDB_NO_DISCARD inline uint32_t GetFirstTypeIndex(void) const PDB_NO_EXCEPT
		{
			return m_header.typeIndexBegin;
		}

		// Returns the index of the last type.
		PDB_NO_DISCARD inline uint32_t GetLastTypeIndex(void) const PDB_NO_EXCEPT
		{
			return m_header.typeIndexEnd;
		}

		PDB_NO_DISCARD inline const CodeView::TPI::Record* GetTypeRecord(uint32_t typeIndex) const PDB_NO_EXCEPT
		{
			if (typeIndex < m_header.typeIndexBegin || typeIndex > m_header.typeIndexEnd)
				return nullptr;

			return m_records[typeIndex - m_header.typeIndexBegin];
		}

		// Returns a view of all type records.
		// Records identified by a type index can be accessed via "allRecords[typeIndex - firstTypeIndex]".
		PDB_NO_DISCARD inline ArrayView<const CodeView::TPI::Record*> GetTypeRecords(void) const PDB_NO_EXCEPT
		{
			return ArrayView<const CodeView::TPI::Record*>(m_records, m_recordCount);
		}

	private:
		TPI::StreamHeader m_header;
		CoalescedMSFStream m_stream;
		const CodeView::TPI::Record** m_records;
		size_t m_recordCount;

		PDB_DISABLE_COPY(TPIStream);
	};

	// Returns whether the given raw file provides a valid TPI stream.
	PDB_NO_DISCARD ErrorCode HasValidTPIStream(const RawFile& file) PDB_NO_EXCEPT;

	// Creates the TPI stream from a raw file.
	PDB_NO_DISCARD TPIStream CreateTPIStream(const RawFile& file) PDB_NO_EXCEPT;
}
