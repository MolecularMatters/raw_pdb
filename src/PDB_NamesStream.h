// Copyright 2011-2022, Molecular Matters GmbH <office@molecular-matters.com>
// See LICENSE.txt for licensing details (2-clause BSD License: https://opensource.org/licenses/BSD-2-Clause)

#pragma once

#include "Foundation/PDB_Macros.h"
#include "PDB_Types.h"
#include "PDB_CoalescedMSFStream.h"


namespace PDB
{
	class RawFile;

	struct NamesHeader
	{
		uint32_t magic;
		uint32_t hashVersion;
		uint32_t size;
	};

	class PDB_NO_DISCARD NamesStream
	{
	public:
		NamesStream(void) PDB_NO_EXCEPT;
		explicit NamesStream(const RawFile& file, uint32_t streamIndex) PDB_NO_EXCEPT;

		PDB_DEFAULT_MOVE(NamesStream);

		// Returns the header of the stream.
		PDB_NO_DISCARD inline const NamesHeader* GetHeader(void) const PDB_NO_EXCEPT
		{
			return m_header;
		}

		PDB_NO_DISCARD inline const char* GetFilename(uint32_t filenameOffset) const PDB_NO_EXCEPT
		{
			return m_stringTable + filenameOffset;
		}

	private:
		CoalescedMSFStream m_stream;
		const NamesHeader* m_header;
		const char* m_stringTable;

		PDB_DISABLE_COPY(NamesStream);
	};
}
