// Copyright 2011-2022, Molecular Matters GmbH <office@molecular-matters.com>
// See LICENSE.txt for licensing details (2-clause BSD License: https://opensource.org/licenses/BSD-2-Clause)

#include "Examples_PCH.h"
#include "ExampleTimedScope.h"
#include "PDB_RawFile.h"
#include "PDB_DBIStream.h"

namespace
{
	// in this example, we are only interested in function symbols: function name, RVA, and size.
	// this is what most profilers need, they aren't interested in any other data.
	struct FunctionSymbol
	{
		std::string name;
		uint32_t rva;
		uint32_t size;
		const PDB::CodeView::DBI::Record* frameProc;
	};
}


void ExampleFunctionSymbols(const PDB::RawFile& rawPdbFile, const PDB::DBIStream& dbiStream);
void ExampleFunctionSymbols(const PDB::RawFile& rawPdbFile, const PDB::DBIStream& dbiStream)
{
	TimedScope total("\nRunning example \"Function symbols\"");

	// in order to keep the example easy to understand, we load the PDB data serially.
	// note that this can be improved a lot by reading streams concurrently.

	// prepare the image section stream first. it is needed for converting section + offset into an RVA
	TimedScope sectionScope("Reading image section stream");
	const PDB::ImageSectionStream imageSectionStream = dbiStream.CreateImageSectionStream(rawPdbFile);
	sectionScope.Done();


	// prepare the module info stream for grabbing function symbols from modules
	TimedScope moduleScope("Reading module info stream");
	const PDB::ModuleInfoStream moduleInfoStream = dbiStream.CreateModuleInfoStream(rawPdbFile);
	moduleScope.Done();


	// prepare symbol record stream needed by the public stream
	TimedScope symbolStreamScope("Reading symbol record stream");
	const PDB::CoalescedMSFStream symbolRecordStream = dbiStream.CreateSymbolRecordStream(rawPdbFile);
	symbolStreamScope.Done();


	// note that we only use unordered_set in order to keep the example code easy to understand.
	// using other hash set implementations like e.g. abseil's Swiss Tables (https://abseil.io/about/design/swisstables) is *much* faster.
	std::vector<FunctionSymbol> functionSymbols;
	std::unordered_set<uint32_t> seenFunctionRVAs;

	// start by reading the module stream, grabbing every function symbol we can find.
	// in most cases, this gives us ~90% of all function symbols already, along with their size.
	{
		TimedScope scope("Storing function symbols from modules");

		const PDB::ArrayView<PDB::ModuleInfoStream::Module> modules = moduleInfoStream.GetModules();

		for (const PDB::ModuleInfoStream::Module& module : modules)
		{
			if (!module.HasSymbolStream())
			{
				continue;
			}

			const PDB::ModuleSymbolStream moduleSymbolStream = module.CreateSymbolStream(rawPdbFile);
			moduleSymbolStream.ForEachSymbol([&functionSymbols, &seenFunctionRVAs, &imageSectionStream](const PDB::CodeView::DBI::Record* record)
			{
				// only grab function symbols from the module streams
				const char* name = nullptr;
				uint32_t rva = 0u;
				uint32_t size = 0u;
				if (record->header.kind == PDB::CodeView::DBI::SymbolRecordKind::S_FRAMEPROC)
				{
					functionSymbols[functionSymbols.size() - 1].frameProc = record;
					return;
				}
				else if (record->header.kind == PDB::CodeView::DBI::SymbolRecordKind::S_THUNK32)
				{
					if (record->data.S_THUNK32.thunk == PDB::CodeView::DBI::ThunkOrdinal::TrampolineIncremental)
					{
						// we have never seen incremental linking thunks stored inside a S_THUNK32 symbol, but better safe than sorry
						name = "ILT";
						rva = imageSectionStream.ConvertSectionOffsetToRVA(record->data.S_THUNK32.section, record->data.S_THUNK32.offset);
						size = 5u;
					}
				}
				else if (record->header.kind == PDB::CodeView::DBI::SymbolRecordKind::S_TRAMPOLINE)
				{
					// incremental linking thunks are stored in the linker module
					name = "ILT";
					rva = imageSectionStream.ConvertSectionOffsetToRVA(record->data.S_TRAMPOLINE.thunkSection, record->data.S_TRAMPOLINE.thunkOffset);
					size = 5u;
				}
				else if (record->header.kind == PDB::CodeView::DBI::SymbolRecordKind::S_LPROC32)
				{
					name = record->data.S_LPROC32.name;
					rva = imageSectionStream.ConvertSectionOffsetToRVA(record->data.S_LPROC32.section, record->data.S_LPROC32.offset);
					size = record->data.S_LPROC32.codeSize;
				}
				else if (record->header.kind == PDB::CodeView::DBI::SymbolRecordKind::S_GPROC32)
				{
					name = record->data.S_GPROC32.name;
					rva = imageSectionStream.ConvertSectionOffsetToRVA(record->data.S_GPROC32.section, record->data.S_GPROC32.offset);
					size = record->data.S_GPROC32.codeSize;
				}
				else if (record->header.kind == PDB::CodeView::DBI::SymbolRecordKind::S_LPROC32_ID)
				{
					name = record->data.S_LPROC32_ID.name;
					rva = imageSectionStream.ConvertSectionOffsetToRVA(record->data.S_LPROC32_ID.section, record->data.S_LPROC32_ID.offset);
					size = record->data.S_LPROC32_ID.codeSize;
				}
				else if (record->header.kind == PDB::CodeView::DBI::SymbolRecordKind::S_GPROC32_ID)
				{
					name = record->data.S_GPROC32_ID.name;
					rva = imageSectionStream.ConvertSectionOffsetToRVA(record->data.S_GPROC32_ID.section, record->data.S_GPROC32_ID.offset);
					size = record->data.S_GPROC32_ID.codeSize;
				}

				if (rva == 0u)
				{
					return;
				}

				functionSymbols.push_back(FunctionSymbol { name, rva, size, nullptr });
				seenFunctionRVAs.emplace(rva);
			});
		}

		scope.Done(modules.GetLength());
	}

	// we don't need to touch global symbols in this case.
	// most of the data we need can be obtained from the module symbol streams, and the global symbol stream only offers data symbols on top of that, which we are not interested in.
	// however, there can still be public function symbols we haven't seen yet in any of the modules, especially for PDBs that don't provide module-specific information.

	// read public symbols
	TimedScope publicScope("Reading public symbol stream");
	const PDB::PublicSymbolStream publicSymbolStream = dbiStream.CreatePublicSymbolStream(rawPdbFile);
	publicScope.Done();
	{
		TimedScope scope("Storing public function symbols");

		const PDB::ArrayView<PDB::HashRecord> hashRecords = publicSymbolStream.GetRecords();
		const size_t count = hashRecords.GetLength();

		for (const PDB::HashRecord& hashRecord : hashRecords)
		{
			const PDB::CodeView::DBI::Record* record = publicSymbolStream.GetRecord(symbolRecordStream, hashRecord);
			if (record->header.kind != PDB::CodeView::DBI::SymbolRecordKind::S_PUB32)
			{
				// normally, a PDB only contains S_PUB32 symbols in the public symbol stream, but we have seen PDBs that also store S_CONSTANT as public symbols.
				// ignore these.
				continue;
			}

			if ((PDB_AS_UNDERLYING(record->data.S_PUB32.flags) & PDB_AS_UNDERLYING(PDB::CodeView::DBI::PublicSymbolFlags::Function)) == 0u)
			{
				// ignore everything that is not a function
				continue;
			}

			const uint32_t rva = imageSectionStream.ConvertSectionOffsetToRVA(record->data.S_PUB32.section, record->data.S_PUB32.offset);
			if (rva == 0u)
			{
				// certain symbols (e.g. control-flow guard symbols) don't have a valid RVA, ignore those
				continue;
			}

			// check whether we already know this symbol from one of the module streams
			const auto it = seenFunctionRVAs.find(rva);
			if (it != seenFunctionRVAs.end())
			{
				// we know this symbol already, ignore it
				continue;
			}

			// this is a new function symbol, so store it.
			// note that we don't know its size yet.
			functionSymbols.push_back(FunctionSymbol { record->data.S_PUB32.name, rva, 0u, nullptr });
		}

		scope.Done(count);
	}


	// we still need to find the size of the public function symbols.
	// this can be deduced by sorting the symbols by their RVA, and then computing the distance between the current and the next symbol.
	// this works since functions are always mapped to executable pages, so they aren't interleaved by any data symbols.
	TimedScope sortScope("std::sort function symbols");
	std::sort(functionSymbols.begin(), functionSymbols.end(), [](const FunctionSymbol& lhs, const FunctionSymbol& rhs)
	{
		return lhs.rva < rhs.rva;
	});
	sortScope.Done();

	const size_t symbolCount = functionSymbols.size();
	if (symbolCount != 0u)
	{
		TimedScope computeScope("Computing function symbol sizes");

		size_t foundCount = 0u;

		// we have at least 1 symbol.
		// compute missing symbol sizes by computing the distance from this symbol to the next.
		// note that this includes "int 3" padding after the end of a function. if you don't want that, but the actual number of bytes of
		// the function's code, your best bet is to use a disassembler instead.
		for (size_t i = 0u; i < symbolCount - 1u; ++i)
		{
			FunctionSymbol& currentSymbol = functionSymbols[i];
			if (currentSymbol.size != 0u)
			{
				// the symbol's size is already known
				continue;
			}

			const FunctionSymbol& nextSymbol = functionSymbols[i + 1u];
			const size_t size = nextSymbol.rva - currentSymbol.rva;
			(void)size; // unused
			++foundCount;
		}

		// we know have the sizes of all symbols, except the last.
		// this can be found by going through the contributions, if needed.
		FunctionSymbol& lastSymbol = functionSymbols[symbolCount - 1u];
		if (lastSymbol.size == 0u)
		{
			// bad luck, we can't deduce the last symbol's size, so have to consult the contributions instead.
			// we do a linear search in this case to keep the code simple.
			const PDB::SectionContributionStream sectionContributionStream = dbiStream.CreateSectionContributionStream(rawPdbFile);
			const PDB::ArrayView<PDB::DBI::SectionContribution> sectionContributions = sectionContributionStream.GetContributions();
			for (const PDB::DBI::SectionContribution& contribution : sectionContributions)
			{
				const uint32_t rva = imageSectionStream.ConvertSectionOffsetToRVA(contribution.section, contribution.offset);
				if (rva == 0u)
				{
					printf("Contribution has invalid RVA\n");
					continue;
				}

				if (rva == lastSymbol.rva)
				{
					lastSymbol.size = contribution.size;
					break;
				}
				
				if (rva > lastSymbol.rva)
				{
					// should have found the contribution by now
					printf("Unknown contribution for symbol %s at RVA 0x%X", lastSymbol.name.c_str(), lastSymbol.rva);
					break;
				}
			}
		}

		computeScope.Done(foundCount);
	}

	total.Done(functionSymbols.size());
}
