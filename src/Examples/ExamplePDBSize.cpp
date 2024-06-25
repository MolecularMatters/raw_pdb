// Copyright 2011-2022, Molecular Matters GmbH <office@molecular-matters.com>
// See LICENSE.txt for licensing details (2-clause BSD License: https://opensource.org/licenses/BSD-2-Clause)

#include "Examples_PCH.h"
#include "ExampleTimedScope.h"
#include "PDB_RawFile.h"
#include "PDB_DBIStream.h"


namespace
{
	struct Stream
	{
		std::string name;
		uint32_t size;
	};
}


void ExamplePDBSize(const PDB::RawFile& rawPdbFile, const PDB::DBIStream& dbiStream);
void ExamplePDBSize(const PDB::RawFile& rawPdbFile, const PDB::DBIStream& dbiStream)
{
	TimedScope total("\nRunning example \"PDBSize\"");

	std::vector<Stream> streams;

	// print show general statistics
	printf("General\n");
	printf("-------\n");
	{
		const PDB::SuperBlock* superBlock = rawPdbFile.GetSuperBlock();
		printf("PDB page size (block size): %u\n", superBlock->blockSize);
		printf("PDB block count: %u\n", superBlock->blockCount);

		const size_t rawSize = static_cast<size_t>(superBlock->blockSize) * static_cast<size_t>(superBlock->blockCount);
		printf("PDB raw size: %zu MiB (%zu GiB)\n", rawSize >> 20u, rawSize >> 30u);
	}

	// print the sizes of all known streams
	printf("\n");
	printf("Sizes of known streams\n");
	printf("----------------------\n");
	{
		const uint32_t streamCount = rawPdbFile.GetStreamCount();
		const uint32_t tpiStreamSize = (streamCount > 2u) ? rawPdbFile.GetStreamSize(2u) : 0u;
		const uint32_t dbiStreamSize = (streamCount > 3u) ? rawPdbFile.GetStreamSize(3u) : 0u;
		const uint32_t ipiStreamSize = (streamCount > 4u) ? rawPdbFile.GetStreamSize(4u) : 0u;

		printf("TPI stream size: %u KiB (%u MiB)\n", tpiStreamSize >> 10u, tpiStreamSize >> 20u);
		printf("DBI stream size: %u KiB (%u MiB)\n", dbiStreamSize >> 10u, dbiStreamSize >> 20u);
		printf("IPI stream size: %u KiB (%u MiB)\n", ipiStreamSize >> 10u, ipiStreamSize >> 20u);

		streams.push_back(Stream { "TPI", tpiStreamSize });
		streams.push_back(Stream { "DBI", dbiStreamSize });
		streams.push_back(Stream { "IPI", ipiStreamSize });

		const uint32_t globalSymbolStreamSize = rawPdbFile.GetStreamSize(dbiStream.GetHeader().globalStreamIndex);
		const uint32_t publicSymbolStreamSize = rawPdbFile.GetStreamSize(dbiStream.GetHeader().publicStreamIndex);
		const uint32_t symbolRecordStreamSize = rawPdbFile.GetStreamSize(dbiStream.GetHeader().symbolRecordStreamIndex);

		printf("Global symbol stream size: %u KiB (%u MiB)\n", globalSymbolStreamSize >> 10u, globalSymbolStreamSize >> 20u);
		printf("Public symbol stream size: %u KiB (%u MiB)\n", publicSymbolStreamSize >> 10u, publicSymbolStreamSize >> 20u);
		printf("Symbol record stream size: %u KiB (%u MiB)\n", symbolRecordStreamSize >> 10u, symbolRecordStreamSize >> 20u);

		streams.emplace_back(Stream { "Global", globalSymbolStreamSize });
		streams.emplace_back(Stream { "Public", publicSymbolStreamSize });
		streams.emplace_back(Stream { "Symbol", symbolRecordStreamSize });
	}

	// print the sizes of all module streams
	printf("\n");
	printf("Sizes of module streams\n");
	printf("-----------------------\n");
	{
		const PDB::ModuleInfoStream moduleInfoStream = dbiStream.CreateModuleInfoStream(rawPdbFile);
		const PDB::ArrayView<PDB::ModuleInfoStream::Module> modules = moduleInfoStream.GetModules();

		for (const PDB::ModuleInfoStream::Module& module : modules)
		{
			const PDB::DBI::ModuleInfo* moduleInfo = module.GetInfo();
			const char* name = module.GetName().Decay();
			const char* objectName = module.GetObjectName().Decay();

			const uint16_t streamIndex = module.HasSymbolStream() ? moduleInfo->moduleSymbolStreamIndex : 0u;
			const uint32_t moduleStreamSize = (streamIndex != 0u) ? rawPdbFile.GetStreamSize(streamIndex) : 0u;

			printf("Module %s (%s) stream size: %u KiB (%u MiB)\n", name, objectName, moduleStreamSize >> 10u, moduleStreamSize >> 20u);

			streams.push_back(Stream { name, moduleStreamSize });
		}
	}

	// sort the streams by their size
	std::sort(streams.begin(), streams.end(), [](const Stream& lhs, const Stream& rhs)
	{
		return lhs.size > rhs.size;
	});

	// log the 20 largest stream
	{
		printf("\n");
		printf("Sizes of 20 largest streams:\n");

		const size_t countToShow = std::min<size_t>(20ul, streams.size());
		for (size_t i = 0u; i < countToShow; ++i)
		{
			const Stream& stream = streams[i];
			printf("%zu: %u KiB (%u MiB) from stream %s\n", i + 1u, stream.size >> 10u, stream.size >> 20u, stream.name.c_str());
		}
	}

	// print the raw stream sizes
	printf("\n");
	printf("Raw sizes of all streams\n");
	printf("------------------------\n");
	{
		const uint32_t streamCount = rawPdbFile.GetStreamCount();
		for (uint32_t i = 0u; i < streamCount; ++i)
		{
			const uint32_t streamSize = rawPdbFile.GetStreamSize(i);
			printf("Stream %u size: %u KiB (%u MiB)\n", i, streamSize >> 10u, streamSize >> 20u);
		}
	}
}
