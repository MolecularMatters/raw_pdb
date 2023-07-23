#pragma once

#include "Foundation/PDB_Macros.h"
#include "Foundation/PDB_ArrayView.h"
#include "PDB_ErrorCodes.h"
#include "PDB_TPITypes.h"
#include "PDB_DirectMSFStream.h"
#include "PDB_Util.h"

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

		explicit TPIStream(const RawFile& file) PDB_NO_EXCEPT;

		PDB_NO_DISCARD inline const DirectMSFStream& GetDirectMSFStream(void) const PDB_NO_EXCEPT
		{
			return m_stream;
		}

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

		// Returns the number of type records.
		PDB_NO_DISCARD inline size_t GetTypeRecordCount(void) const PDB_NO_EXCEPT
		{
			return m_recordCount;
		}

		CodeView::TPI::RecordHeader ReadTypeRecordHeader(size_t offset) const PDB_NO_EXCEPT
		{
			const CodeView::TPI::RecordHeader header = m_stream.ReadAtOffset<CodeView::TPI::RecordHeader>(offset);
			return header;
		}

		template <typename F>
		void ForEachTypeRecordHeaderAndOffset(F&& functor) const PDB_NO_EXCEPT
		{
			// ignore the stream's header
			size_t offset = sizeof(TPI::StreamHeader);

			while (offset < m_stream.GetSize())
			{
				const CodeView::TPI::RecordHeader header = ReadTypeRecordHeader(offset);

				functor(header, offset);

				// position the stream offset at the next record
				offset += sizeof(CodeView::TPI::RecordHeader) + header.size - sizeof(uint16_t);
			}
		}

	private:
		DirectMSFStream m_stream;
		TPI::StreamHeader m_header;
		size_t m_recordCount;

		PDB_DISABLE_COPY(TPIStream);
	};

	// Returns whether the given raw file provides a valid TPI stream.
	PDB_NO_DISCARD ErrorCode HasValidTPIStream(const RawFile& file) PDB_NO_EXCEPT;

	// Creates the TPI stream from a raw file.
	PDB_NO_DISCARD TPIStream CreateTPIStream(const RawFile& file) PDB_NO_EXCEPT;
}
