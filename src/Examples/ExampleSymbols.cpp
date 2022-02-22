// Copyright 2011-2022, Molecular Matters GmbH <office@molecular-matters.com>
// See LICENSE.txt for licensing details (2-clause BSD License: https://opensource.org/licenses/BSD-2-Clause)

#include "Examples_PCH.h"
#include "ExampleMemoryMappedFile.h"
#include "PDB.h"
#include "PDB_RawFile.h"
#include "PDB_DBIStream.h"


namespace
{
	PDB_NO_DISCARD static bool HasValidDBIStreams(const PDB::RawFile& rawPdbFile, const PDB::DBIStream& dbiStream) PDB_NO_EXCEPT
	{
		// check whether the DBI stream offers all sub-streams we need
		if (!dbiStream.HasValidImageSectionStream(rawPdbFile))
		{
			return false;
		}

		if (!dbiStream.HasValidPublicSymbolStream(rawPdbFile))
		{
			return false;
		}

		if (!dbiStream.HasValidGlobalSymbolStream(rawPdbFile))
		{
			return false;
		}

		if (!dbiStream.HasValidSectionContributionStream(rawPdbFile))
		{
			return false;
		}

		return true;
	}

	// we don't have to store std::string in the contributions or symbols, since all the data is memory-mapped anyway.
	// we do it in this example to ensure that we don't "cheat" when reading the PDB file. memory-mapped data will only
	// be faulted into the process once it's touched, so actually copying the string data makes us touch the needed data,
	// giving us a real performance measurement.
	struct Contribution
	{
		std::string objectFile;
		uint32_t rva;
		uint32_t size;
	};

	struct Symbol
	{
		std::string name;
		uint32_t rva;
	};
}


int main(void)
{
	const auto timestampBegin = std::chrono::high_resolution_clock::now();

#ifdef _DEBUG
	const wchar_t* const pdbPath = LR"(..\bin\x64\Debug\ExampleSymbols.pdb)";
#else
	const wchar_t* const pdbPath = LR"(..\bin\x64\Release\ExampleSymbols.pdb)";
#endif
	
	printf("Opening PDB file %ls\n", pdbPath);

	// try to open the PDB file and check whether all the data we need is available
	MemoryMappedFile::Handle pdbFile = MemoryMappedFile::Open(pdbPath);
	if (!pdbFile.baseAddress)
	{
		return 1;
	}

	const bool isValid = PDB::ValidateFile(pdbPath, pdbFile.baseAddress);
	if (!isValid)
	{
		MemoryMappedFile::Close(pdbFile);

		return 2;
	}

	const PDB::RawFile rawPdbFile = PDB::CreateRawFile(pdbFile.baseAddress);
	if (!PDB::HasValidDBIStream(rawPdbFile))
	{
		MemoryMappedFile::Close(pdbFile);

		return 3;
	}

	const PDB::DBIStream dbiStream = PDB::CreateDBIStream(rawPdbFile);
	if (!HasValidDBIStreams(rawPdbFile, dbiStream))
	{
		MemoryMappedFile::Close(pdbFile);

		return 4;
	}


	// prepare the image section stream first. it is needed for converting section + offset into an RVA
	printf("Reading image section stream...");
	const PDB::ImageSectionStream imageSectionStream = dbiStream.CreateImageSectionStream(rawPdbFile);
	printf("done\n");


	// prepare the module info stream for matching contributions against files
	printf("Reading module info stream...");
	const PDB::ModuleInfoStream moduleInfoStream = dbiStream.CreateModuleInfoStream(rawPdbFile);
	printf("done\n");


	// read contribution stream
	printf("Reading section contribution stream...");
	const PDB::SectionContributionStream sectionContributionStream = dbiStream.CreateSectionContributionStream(rawPdbFile);
	printf("done\n");

	std::vector<Contribution> contributions;
	{
		const ArrayView<PDB::DBI::SectionContribution> sectionContributions = sectionContributionStream.GetContributions();
		const size_t count = sectionContributions.GetLength();

		printf("Storing %zu section contributions...", count);

		contributions.reserve(count);

		for (const PDB::DBI::SectionContribution& contribution : sectionContributions)
		{
			const uint32_t rva = imageSectionStream.ConvertSectionOffsetToRVA(contribution.section, contribution.offset);
			if (rva == 0u)
			{
				printf("Contribution has invalid RVA\n");
				continue;
			}

			const PDB::ModuleInfoStream::Module& module = moduleInfoStream.GetModule(contribution.moduleIndex);

			contributions.push_back(Contribution { module.GetName().Decay(), rva, contribution.size });
		}
		printf("done\n");
	}


	// prepare symbol record stream needed by both public and global streams
	printf("Reading symbol record stream...");
	const PDB::CoalescedMSFStream symbolRecordStream = dbiStream.CreateSymbolRecordStream(rawPdbFile);
	printf("done\n");

	std::vector<Symbol> symbols;

	// read public symbols
	printf("Reading public symbol stream...");
	const PDB::PublicSymbolStream publicSymbolStream = dbiStream.CreatePublicSymbolStream(rawPdbFile);
	printf("done\n");
	{
		const ArrayView<PDB::HashRecord> hashRecords = publicSymbolStream.GetRecords();
		const size_t count = hashRecords.GetLength();

		printf("Parsing %zu public symbols...", count);

		symbols.reserve(count);

		for (const PDB::HashRecord& hashRecord : hashRecords)
		{
			const PDB::CodeView::DBI::Record* record = publicSymbolStream.GetRecord(symbolRecordStream, hashRecord);
			const uint32_t rva = imageSectionStream.ConvertSectionOffsetToRVA(record->data.S_PUB32.section, record->data.S_PUB32.offset);
			if (rva == 0u)
			{
				// certain symbols (e.g. control-flow guard symbols) don't have a valid RVA, ignore those
				continue;
			}

			symbols.push_back(Symbol { record->data.S_PUB32.name, rva });
		}
		printf("done\n");
	}


	// read global symbols
	printf("Reading global symbol stream...");
	const PDB::GlobalSymbolStream globalSymbolStream = dbiStream.CreateGlobalSymbolStream(rawPdbFile);
	printf("done\n");
	{
		const ArrayView<PDB::HashRecord> hashRecords = globalSymbolStream.GetRecords();
		const size_t count = hashRecords.GetLength();

		printf("Parsing %zu global symbols...", count);

		symbols.reserve(symbols.size() + count);

		for (const PDB::HashRecord& hashRecord : hashRecords)
		{
			const PDB::CodeView::DBI::Record* record = globalSymbolStream.GetRecord(symbolRecordStream, hashRecord);

			const char* name = nullptr;
			uint32_t rva = 0u;
			if (record->header.kind == PDB::CodeView::DBI::SymbolRecordKind::S_GDATA32)
			{
				name = record->data.S_GDATA32.name;
				rva = imageSectionStream.ConvertSectionOffsetToRVA(record->data.S_GDATA32.section, record->data.S_GDATA32.offset);
			}
			else if (record->header.kind == PDB::CodeView::DBI::SymbolRecordKind::S_GTHREAD32)
			{
				name = record->data.S_GTHREAD32.name;
				rva = imageSectionStream.ConvertSectionOffsetToRVA(record->data.S_GTHREAD32.section, record->data.S_GTHREAD32.offset);
			}
			else if (record->header.kind == PDB::CodeView::DBI::SymbolRecordKind::S_LDATA32)
			{
				name = record->data.S_LDATA32.name;
				rva = imageSectionStream.ConvertSectionOffsetToRVA(record->data.S_LDATA32.section, record->data.S_LDATA32.offset);
			}
			else if (record->header.kind == PDB::CodeView::DBI::SymbolRecordKind::S_LTHREAD32)
			{
				name = record->data.S_LTHREAD32.name;
				rva = imageSectionStream.ConvertSectionOffsetToRVA(record->data.S_LTHREAD32.section, record->data.S_LTHREAD32.offset);
			}

			if (rva == 0u)
			{
				// certain symbols (e.g. control-flow guard symbols) don't have a valid RVA, ignore those
				continue;
			}

			symbols.push_back(Symbol { name, rva });
		}
		printf("done\n");
	}


	// read module symbols
	{
		const ArrayView<PDB::ModuleInfoStream::Module> modules = moduleInfoStream.GetModules();

		printf("Reading and parsing %zu module streams...", modules.GetLength());
		for (const PDB::ModuleInfoStream::Module& module : modules)
		{
			if (!module.HasSymbolStream())
			{
				continue;
			}

			const PDB::ModuleSymbolStream moduleSymbolStream = module.CreateSymbolStream(rawPdbFile);
			moduleSymbolStream.ForEachSymbol([&symbols, &imageSectionStream](const PDB::CodeView::DBI::Record* record)
			{
				const char* name = nullptr;
				uint32_t rva = 0u;
				if (record->header.kind == PDB::CodeView::DBI::SymbolRecordKind::S_THUNK32)
				{
					// incremental linking thunks are stored in the linker module
					name = "ILT";
					rva = imageSectionStream.ConvertSectionOffsetToRVA(record->data.S_THUNK32.section, record->data.S_THUNK32.offset);
				}
				else if (record->header.kind == PDB::CodeView::DBI::SymbolRecordKind::S_BLOCK32)
				{
					// blocks never store a name and are only stored for indicating whether other symbols are children of this block
				}
				else if (record->header.kind == PDB::CodeView::DBI::SymbolRecordKind::S_LABEL32)
				{
					// labels don't have a name
				}
				else if (record->header.kind == PDB::CodeView::DBI::SymbolRecordKind::S_LPROC32)
				{
					name = record->data.S_LPROC32.name;
					rva = imageSectionStream.ConvertSectionOffsetToRVA(record->data.S_LPROC32.section, record->data.S_LPROC32.offset);
				}
				else if (record->header.kind == PDB::CodeView::DBI::SymbolRecordKind::S_GPROC32)
				{
					name = record->data.S_GPROC32.name;
					rva = imageSectionStream.ConvertSectionOffsetToRVA(record->data.S_GPROC32.section, record->data.S_GPROC32.offset);
				}
				else if (record->header.kind == PDB::CodeView::DBI::SymbolRecordKind::S_LPROC32_ID)
				{
					name = record->data.S_LPROC32_ID.name;
					rva = imageSectionStream.ConvertSectionOffsetToRVA(record->data.S_LPROC32_ID.section, record->data.S_LPROC32_ID.offset);
				}
				else if (record->header.kind == PDB::CodeView::DBI::SymbolRecordKind::S_GPROC32_ID)
				{
					name = record->data.S_GPROC32_ID.name;
					rva = imageSectionStream.ConvertSectionOffsetToRVA(record->data.S_GPROC32_ID.section, record->data.S_GPROC32_ID.offset);
				}
				else if (record->header.kind == PDB::CodeView::DBI::SymbolRecordKind::S_LDATA32)
				{
					name = record->data.S_LDATA32.name;
					rva = imageSectionStream.ConvertSectionOffsetToRVA(record->data.S_LDATA32.section, record->data.S_LDATA32.offset);
				}
				else if (record->header.kind == PDB::CodeView::DBI::SymbolRecordKind::S_LTHREAD32)
				{
					name = record->data.S_LTHREAD32.name;
					rva = imageSectionStream.ConvertSectionOffsetToRVA(record->data.S_LTHREAD32.section, record->data.S_LTHREAD32.offset);
				}

				if (rva == 0u)
				{
					// certain symbols (e.g. control-flow guard symbols) don't have a valid RVA, ignore those
					return;
				}

				symbols.push_back(Symbol { name, rva });
			});
		}
		printf("done\n");
	}

	MemoryMappedFile::Close(pdbFile);

	const auto timestampNow = std::chrono::high_resolution_clock::now();
	const std::chrono::duration<float> seconds = timestampNow - timestampBegin;

	printf("Stored %zu symbols in std::vector using std::string\n", symbols.size());
	printf("Running time: %.3fms\n", seconds.count()*1000.0);

	return 0;
}
