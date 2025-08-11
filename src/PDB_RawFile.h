// Copyright 2011-2022, Molecular Matters GmbH <office@molecular-matters.com>
// See LICENSE.txt for licensing details (2-clause BSD License: https://opensource.org/licenses/BSD-2-Clause)

#pragma once

#include "Foundation/PDB_Macros.h"
#include "PDB_CoalescedMSFStream.h"


// https://llvm.org/docs/PDB/index.html
namespace PDB
{
	struct SuperBlock;


	class PDB_NO_DISCARD RawFile
	{
	public:
		RawFile(RawFile&& other) PDB_NO_EXCEPT;
		RawFile& operator=(RawFile&& other) PDB_NO_EXCEPT;

		explicit RawFile(const void* data) PDB_NO_EXCEPT;
		~RawFile(void) PDB_NO_EXCEPT;

		// Creates any type of MSF stream.
		template <typename T>
		PDB_NO_DISCARD T CreateMSFStream(uint32_t streamIndex) const PDB_NO_EXCEPT;

		// Creates any type of MSF stream with the given size.
		template <typename T>
		PDB_NO_DISCARD T CreateMSFStream(uint32_t streamIndex, uint32_t streamSize) const PDB_NO_EXCEPT;


		// Returns the SuperBlock.
		PDB_NO_DISCARD inline const SuperBlock* GetSuperBlock(void) const PDB_NO_EXCEPT
		{
			return m_superBlock;
		}

		// Returns the number of streams in the PDB file.
		PDB_NO_DISCARD inline uint32_t GetStreamCount(void) const PDB_NO_EXCEPT
		{
			return m_streamCount;
		}

		// Returns the size of the stream with the given index, taking into account nil page sizes.
		PDB_NO_DISCARD inline uint32_t GetStreamSize(uint32_t streamIndex) const PDB_NO_EXCEPT
		{
			const uint32_t streamSize = m_streamSizes[streamIndex];

			return (streamSize == NilPageSize) ? 0u : streamSize;
		}

	private:
		const void* m_data;
		const SuperBlock* m_superBlock;
		CoalescedMSFStream m_directoryStream;

		// stream directory
		uint32_t m_streamCount;
		const uint32_t* m_streamSizes;
		const uint32_t** m_streamBlocks;

		PDB_DISABLE_COPY(RawFile);
	};
}
