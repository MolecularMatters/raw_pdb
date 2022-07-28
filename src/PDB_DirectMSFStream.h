// Copyright 2011-2022, Molecular Matters GmbH <office@molecular-matters.com>
// See LICENSE.txt for licensing details (2-clause BSD License: https://opensource.org/licenses/BSD-2-Clause)

#pragma once

#include "Foundation/PDB_Macros.h"
#include "Foundation/PDB_DisableWarningsPush.h"
#include <cstdint>
#include "Foundation/PDB_DisableWarningsPop.h"


// https://llvm.org/docs/PDB/index.html#the-msf-container
// https://llvm.org/docs/PDB/MsfFile.html
namespace PDB
{
	// provides direct access to the data of an MSF stream.
	// inherently thread-safe, the stream doesn't carry any internal offset or similar.
	// trivial to construct.
	// slower individual reads, but pays off when not all data of a stream is needed.
	class PDB_NO_DISCARD DirectMSFStream
	{
	public:
		DirectMSFStream(void) PDB_NO_EXCEPT;
		explicit DirectMSFStream(const void* data, uint32_t blockSize, const uint32_t* blockIndices, uint32_t streamSize) PDB_NO_EXCEPT;

		PDB_DEFAULT_MOVE(DirectMSFStream);

		// Reads a number of bytes from the stream.
		void ReadAtOffset(void* destination, size_t size, size_t offset) const PDB_NO_EXCEPT;

		// Reads from the stream.
		template <typename T>
		PDB_NO_DISCARD inline T ReadAtOffset(size_t offset) const PDB_NO_EXCEPT
		{
			T data;
			ReadAtOffset(&data, sizeof(T), offset);
			return data;
		}

		// Returns the block size of the stream.
		PDB_NO_DISCARD inline uint32_t GetBlockSize(void) const PDB_NO_EXCEPT
		{
			return m_blockSize;
		}

		// Returns the size of the stream.
		PDB_NO_DISCARD inline uint32_t GetSize(void) const PDB_NO_EXCEPT
		{
			return m_size;
		}

	private:
		friend class CoalescedMSFStream;

		struct IndexAndOffset
		{
			uint32_t index;
			uint32_t offsetWithinBlock;
		};

		// Returns the block index and offset within the block that correspond to the given offset.
		PDB_NO_DISCARD IndexAndOffset GetBlockIndexForOffset(uint32_t offset) const PDB_NO_EXCEPT;

		// Returns the offset into the data that corresponds to the given indices and offset within a block.
		PDB_NO_DISCARD size_t GetDataOffsetForIndexAndOffset(const IndexAndOffset& indexAndOffset) const PDB_NO_EXCEPT;

		// Provides read-only access to the memory-mapped data.
		PDB_NO_DISCARD inline const void* GetData(void) const PDB_NO_EXCEPT
		{
			return m_data;
		}

		// Provides read-only access to the block indices.
		PDB_NO_DISCARD inline const uint32_t* GetBlockIndices(void) const PDB_NO_EXCEPT
		{
			return m_blockIndices;
		}

		const void* m_data;
		const uint32_t* m_blockIndices;
		uint32_t m_blockSize;
		uint32_t m_size;
		uint32_t m_blockSizeLog2;

		PDB_DISABLE_COPY(DirectMSFStream);
	};
}
