// Copyright 2011-2022, Molecular Matters GmbH <office@molecular-matters.com>
// See LICENSE.txt for licensing details (2-clause BSD License: https://opensource.org/licenses/BSD-2-Clause)

#include "PDB_PCH.h"
#include "PDB.h"
#include "PDB_Types.h"
#include "PDB_Util.h"
#include "PDB_RawFile.h"


// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
PDB_NO_DISCARD bool PDB::ValidateFile(const wchar_t* const path, const void* data) PDB_NO_EXCEPT
{
	// validate the super block
	const SuperBlock* superBlock = Pointer::Offset<const SuperBlock*>(data, 0u);
	const uint32_t directoryBlockCount = ConvertSizeToBlockCount(superBlock->directorySize, superBlock->blockSize);
	{
		// validate header magic		
		if (std::memcmp(superBlock->fileMagic, SuperBlock::MAGIC, sizeof(SuperBlock::MAGIC) != 0))
		{
			PDB_LOG_ERROR("Invalid Superblock in PDB file %ls", path);
			return false;
		}

		// validate directory
		{
			// the directory is a block which consists of a list of block indices (uint32_t).
			// we cannot deal with directories being larger than a single block.
			const uint32_t blockIndicesPerBlock = superBlock->blockSize / sizeof(uint32_t);
			if (directoryBlockCount > blockIndicesPerBlock)
			{
				PDB_LOG_ERROR("Directory is too large in PDB file %ls", path);
				return false;
			}
		}

		// validate free block map.
		// the free block map should always reside at either index 1 or 2.
		if (superBlock->freeBlockMapIndex != 1u && superBlock->freeBlockMapIndex != 2u)
		{
			PDB_LOG_ERROR("Invalid free block map in PDB file %ls", path);
			return false;
		}
	}

	return true;
}


// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
PDB_NO_DISCARD PDB::RawFile PDB::CreateRawFile(const void* data) PDB_NO_EXCEPT
{
	return RawFile(data);
}
