// Copyright 2011-2022, Molecular Matters GmbH <office@molecular-matters.com>
// See LICENSE.txt for licensing details (2-clause BSD License: https://opensource.org/licenses/BSD-2-Clause)

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
		TPIStream() PDB_NO_EXCEPT;
		TPIStream(TPIStream&& other) PDB_NO_EXCEPT;
		TPIStream& operator=(TPIStream&& other) PDB_NO_EXCEPT;

		explicit TPIStream(const RawFile& file, const TPI::StreamHeader& header) PDB_NO_EXCEPT;
		~TPIStream() PDB_NO_EXCEPT;

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
		PDB_NO_DISCARD inline ArrayView<const CodeView::TPI::Record*> GetTypeRecords() const PDB_NO_EXCEPT
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


	// ------------------------------------------------------------------------------------------------
	// General
	// ------------------------------------------------------------------------------------------------

	PDB_NO_DISCARD ErrorCode HasValidTPIStream(const RawFile& file) PDB_NO_EXCEPT;

	PDB_NO_DISCARD TPIStream CreateTPIStream(const RawFile& file) PDB_NO_EXCEPT;
}
