// Copyright 2011-2022, Molecular Matters GmbH <office@molecular-matters.com>
// See LICENSE.txt for licensing details (2-clause BSD License: https://opensource.org/licenses/BSD-2-Clause)

#include "PDB_PCH.h"
#include "PDB_InfoStream.h"
#include "PDB_RawFile.h"


namespace
{
	// the PDB info stream always resides at index 1
	static constexpr const uint32_t InfoStreamIndex = 1u;
}


// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
PDB::InfoStream::InfoStream(void) PDB_NO_EXCEPT
	: m_stream()
	, m_header(nullptr)
	, m_namesStreamIndex(0)
	, m_usesDebugFastlink(false)
{
}


// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
PDB::InfoStream::InfoStream(const RawFile& file) PDB_NO_EXCEPT
	: m_stream(file.CreateMSFStream<CoalescedMSFStream>(InfoStreamIndex))
	, m_header(m_stream.GetDataAtOffset<const Header>(0u))
	, m_namesStreamIndex(0)
	, m_usesDebugFastlink(false)
{
	// the info stream starts with the header, followed by the named stream map, followed by the feature codes
	// https://llvm.org/docs/PDB/PdbStream.html#named-stream-map
	size_t streamOffset = sizeof(Header);

	const NamedStreamMap* namedStreamMap = m_stream.GetDataAtOffset<const NamedStreamMap>(streamOffset);
	streamOffset += sizeof(NamedStreamMap) + namedStreamMap->length;

	const SerializedHashTable::Header* hashTableHeader = m_stream.GetDataAtOffset<const SerializedHashTable::Header>(streamOffset);
	streamOffset += sizeof(SerializedHashTable::Header);

	const SerializedHashTable::BitVector* presentBitVector = m_stream.GetDataAtOffset<const SerializedHashTable::BitVector>(streamOffset);
	streamOffset += sizeof(SerializedHashTable::BitVector) + sizeof(uint32_t) * presentBitVector->wordCount;

	const SerializedHashTable::BitVector* deletedBitVector = m_stream.GetDataAtOffset<const SerializedHashTable::BitVector>(streamOffset);
	streamOffset += sizeof(SerializedHashTable::BitVector) + sizeof(uint32_t) * deletedBitVector->wordCount;

	// the hash table entries can be used to identify the indices of certain common streams like:
	//	"/UDTSRCLINEUNDONE"
	//	"/src/headerblock"
	//	"/LinkInfo"
	//	"/TMCache"
	//	"/names"

	const NamedStreamMap::HashTableEntry* namedStreamMapHashEntries = m_stream.GetDataAtOffset<const NamedStreamMap::HashTableEntry>(streamOffset);

	// Find "/names" stream, used to look up filenames for lines.
	for (uint32_t i = 0, size = hashTableHeader->size; i < size; ++i)
	{
		const NamedStreamMap::HashTableEntry& entry = namedStreamMapHashEntries[i];
		const char* streamName = &namedStreamMap->stringTable[entry.stringTableOffset];

		if (std::strcmp("/names", streamName) == 0)
		{
			m_namesStreamIndex = entry.streamIndex;
		}
	}

	streamOffset += sizeof(NamedStreamMap::HashTableEntry) * hashTableHeader->size;

	// read feature codes by consuming remaining bytes
	// https://llvm.org/docs/PDB/PdbStream.html#pdb-feature-codes
	const FeatureCode* featureCodes = m_stream.GetDataAtOffset<const FeatureCode>(streamOffset);
	const size_t remainingBytes = m_stream.GetSize() - streamOffset;
	const size_t count = remainingBytes / sizeof(FeatureCode);

	for (size_t i=0u; i < count; ++i)
	{
		FeatureCode code = featureCodes[i];
		if (code == PDB::FeatureCode::MinimalDebugInfo)
		{
			m_usesDebugFastlink = true;
		}
		else if (code == PDB::FeatureCode::VC110 || code == PDB::FeatureCode::VC140)
		{
			m_hasIPIStream = true;
		}
	}
}


// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
PDB::NamesStream PDB::InfoStream::CreateNamesStream(const RawFile& file) const PDB_NO_EXCEPT
{
	return NamesStream(file, m_namesStreamIndex);
}
