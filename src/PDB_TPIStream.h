#pragma once

#include "Foundation/PDB_Macros.h"
#include "PDB_ErrorCodes.h"
#include "PDB_TPITypes.h"
#include "PDB_CoalescedMSFStream.h"
#include "PDB_DirectMSFStream.h"
#include "PDB_ImageSectionStream.h"
#include "PDB_PublicSymbolStream.h"
#include "PDB_GlobalSymbolStream.h"
#include "PDB_SourceFileStream.h"
#include "PDB_SectionContributionStream.h"
#include "PDB_ModuleInfoStream.h"

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
		size_t m_hr_recordCount;

		PDB_DISABLE_COPY(TPIStream);
	};

	// Returns whether the given raw file provides a valid DBI stream.
	PDB_NO_DISCARD ErrorCode HasValidTPIStream(const RawFile& file) PDB_NO_EXCEPT;

	// Creates the DBI stream from a raw file.
	PDB_NO_DISCARD TPIStream CreateTPIStream(const RawFile& file) PDB_NO_EXCEPT;
}
