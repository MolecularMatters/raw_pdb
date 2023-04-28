// Copyright 2011-2022, Molecular Matters GmbH <office@molecular-matters.com>
// See LICENSE.txt for licensing details (2-clause BSD License: https://opensource.org/licenses/BSD-2-Clause)

#include "Examples_PCH.h"
#include "ExampleTimedScope.h"
#include "PDB_RawFile.h"
#include "PDB_DBIStream.h"
#include "PDB_TPIStream.h"

namespace
{
	struct FunctionRecords
	{
		size_t index;
		size_t size;
	};

	struct FunctionRecord
	{
		enum class Kind : uint8_t
		{
			None = 0,
			Block = 1,
			Variable = 2
		};

		Kind kind;

		union
		{
			struct
			{
				size_t nameIndex;
				size_t typeIndex;
			} variable;

			struct
			{
				FunctionRecords records;
			} block;
		};
	};

	struct FunctionSymbol
	{
		std::string name;
		uint32_t rva;
		uint32_t size;
		const PDB::CodeView::DBI::Record* frameProc;

		FunctionRecords records;
	};
}

using SymbolRecordKind = PDB::CodeView::DBI::SymbolRecordKind;

static std::string GetVariableTypeName(const PDB::TPIStream& tpiStream, uint32_t typeIndex)
{
	// Defined in ExampleTypes.cpp
	extern std::string GetTypeName(const PDB::TPIStream & tpiStream, uint32_t typeIndex);

	std::string typeName = GetTypeName(tpiStream, typeIndex);

	// Remove any '%s' substring used to insert a variable/field name.
	const uint64_t markerPos = typeName.find("%s");
	if (markerPos != typeName.npos)
	{
		typeName.erase(markerPos, 2);
	}

	return typeName;
}

void Printf(uint32_t indent, const char* format, ...) 
{
	va_list args;
	va_start(args, format);

	printf("%*s", indent * 4, "");
	vprintf(format, args);

	va_end(args);
}

void ExampleFunctionVariables(const PDB::RawFile& rawPdbFile, const PDB::DBIStream& dbiStream, const PDB::TPIStream& tpiStream);
void ExampleFunctionVariables(const PDB::RawFile& rawPdbFile, const PDB::DBIStream& dbiStream, const PDB::TPIStream& tpiStream)
{
	TimedScope total("\nRunning example \"Function variables\"");

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

		uint32_t blockLevel = 0;

		for (const PDB::ModuleInfoStream::Module& module : modules)
		{
			if (!module.HasSymbolStream())
			{
				continue;
			}

			const PDB::ModuleSymbolStream moduleSymbolStream = module.CreateSymbolStream(rawPdbFile);
			moduleSymbolStream.ForEachSymbol([&tpiStream, &functionSymbols, &seenFunctionRVAs, &imageSectionStream, &blockLevel](const PDB::CodeView::DBI::Record* record)
			{
				const SymbolRecordKind kind = record->header.kind;
				const PDB::CodeView::DBI::Record::Data& data = record->data;

				// only grab function symbols from the module streams
				const char* name = nullptr;
				uint32_t rva = 0u;
				uint32_t size = 0u;

				if (record->header.kind == PDB::CodeView::DBI::SymbolRecordKind::S_END)
				{
					PDB_ASSERT(blockLevel > 0, "BlockIndent for S_END is 0");
					blockLevel--;
					printf("%*sS_END\n", blockLevel * 4, "");
				}
				else if (record->header.kind == PDB::CodeView::DBI::SymbolRecordKind::S_BLOCK32)
				{
					const uint32_t offset = imageSectionStream.ConvertSectionOffsetToRVA(data.S_BLOCK32.section, data.S_BLOCK32.offset);

					printf("%*sS_BLOCK32: '%s' | Code Offset 0x%X\n", blockLevel*4, "", data.S_BLOCK32.name, offset);
					blockLevel++;
				}
				else if (kind == SymbolRecordKind::S_LABEL32)
				{
					printf("%*sS_LABEL32: '%s' | Offset 0x%X\n", blockLevel * 4, "", data.S_LABEL32.name, data.S_LABEL32.offset);
				}
				else if(kind == SymbolRecordKind::S_CONSTANT)
				{
					const std::string typeName = GetVariableTypeName(tpiStream, data.S_CONSTANT.typeIndex);

					printf("%*sS_CONSTANT: '%s' -> '%s' | Value 0x%X\n", blockLevel*4, "", typeName.c_str(), data.S_CONSTANT.name, data.S_CONSTANT.value);
				}
				else if(kind == SymbolRecordKind::S_LOCAL)
				{
					const std::string typeName = GetVariableTypeName(tpiStream, data.S_LOCAL.typeIndex);
					printf("%*sS_LOCAL: '%s' -> '%s'\n", blockLevel*4, "", typeName.c_str(), data.S_LOCAL.name);
				}
				else if (kind == SymbolRecordKind::S_DEFRANGE_REGISTER)
				{
					printf("%*sS_DEFRANGE_REGISTER: Register 0x%X\n", blockLevel * 4, "", data.S_DEFRANGE_REGISTER.reg);
				}
				else if(kind == SymbolRecordKind::S_DEFRANGE_FRAMEPOINTER_REL)
				{
					printf("%*sS_DEFRANGE_FRAMEPOINTER_REL: <TODO>\n", blockLevel * 4, "");
				}
				else if(kind == SymbolRecordKind::S_DEFRANGE_SUBFIELD_REGISTER)
				{
					printf("%*sS_DEFRANGE_SUBFIELD_REGISTER: <TODO>\n", blockLevel * 4, "");
				}
				else if (kind == SymbolRecordKind::S_DEFRANGE_FRAMEPOINTER_REL_FULL_SCOPE)
				{
					printf("%*sS_DEFRANGE_FRAMEPOINTER_REL_FULL_SCOPE: Offset 0x%X\n", blockLevel * 4, "", data.S_DEFRANGE_FRAMEPOINTER_REL_FULL_SCOPE.offsetFramePointer);
				}
				else if (kind == SymbolRecordKind::S_DEFRANGE_REGISTER_REL)
				{
					printf("%*sS_DEFRANGE_REGISTER_REL: <TODO>\n", blockLevel * 4, "");
				}
				else if(kind == SymbolRecordKind::S_FILESTATIC)
				{
					printf("%*sS_FILESTATIC: '%s'\n", blockLevel * 4, "", data.S_FILESTATIC.name);
				}
				else if (kind == SymbolRecordKind::S_INLINESITE)
				{
					printf("%*sS_INLINESITE: Parent 0x%X\n", blockLevel * 4, "", data.S_INLINESITE.parent);
					blockLevel++;
				}
				else if (kind == SymbolRecordKind::S_INLINESITE_END)
				{
					PDB_ASSERT(blockLevel > 0, "BlockIndent for S_INLINESITE_END is 0");
					blockLevel--;					
					printf("%*sS_INLINESITE_END:\n", blockLevel * 4, "");
				}
				else if (kind == SymbolRecordKind::S_CALLEES)
				{
					printf("%*sS_CALLEES: Count %u\n", blockLevel * 4, "", data.S_CALLEES.count);
				}
				else if (kind == SymbolRecordKind::S_CALLERS)
				{
					printf("%*sS_CALLERS: Count %u\n", blockLevel * 4, "", data.S_CALLERS.count);
				}
				else if (kind == SymbolRecordKind::S_INLINEES)
				{
					printf("%*sS_INLINEES: Count %u\n", blockLevel * 4, "", data.S_INLINEES.count);
				}
				else if (kind == SymbolRecordKind::S_LDATA32)
				{
					if (blockLevel > 0)
					{
						// Not sure why some type index 0 (T_NO_TYPE) are included in some PDBs.
						if (data.S_LDATA32.typeIndex != 0) // PDB::CodeView::TPI::TypeIndexKind::T_NOTYPE)
						{
							const std::string typeName = GetVariableTypeName(tpiStream, data.S_LDATA32.typeIndex);
							printf("%*sS_LDATA32: '%s' -> '%s'\n", blockLevel * 4, "", data.S_LDATA32.name, typeName.c_str());
						}						
					}
				}
				else if (kind == SymbolRecordKind::S_LTHREAD32)
				{
					if (blockLevel > 0)
					{
						const std::string typeName = GetVariableTypeName(tpiStream, data.S_LTHREAD32.typeIndex);
						printf("%*sS_LTHREAD32: '%s' -> '%s'\n", blockLevel * 4, "", data.S_LTHREAD32.name, typeName.c_str());
					}
				}

				else if (record->header.kind == PDB::CodeView::DBI::SymbolRecordKind::S_UDT)
				{
					const std::string typeName = GetVariableTypeName(tpiStream, data.S_UDT.typeIndex);

					printf("%*sS_UDT: '%s' -> '%s'\n", blockLevel * 4, "", data.S_UDT.name, typeName.c_str());
				}
				else if (record->header.kind == PDB::CodeView::DBI::SymbolRecordKind::S_REGREL32)
				{
					std::string typeName = GetVariableTypeName(tpiStream, data.S_REGREL32.typeIndex);
	
					printf("%*sS_REGREL32: '%s' -> '%s' | Register %i | Register Offset 0x%X\n", blockLevel * 4, "", data.S_REGREL32.name, typeName.c_str(), data.S_REGREL32.reg, data.S_REGREL32.offset);
				}
				else if(kind == SymbolRecordKind::S_FRAMECOOKIE)
				{
					printf("%*sS_FRAMECOOKIE Offset 0x%X | Register %u | Type %u\n", blockLevel * 4, "", data.S_FRAMECOOKIE.offset, data.S_FRAMECOOKIE.reg, data.S_FRAMECOOKIE.cookietype);
				}
				else if(kind == SymbolRecordKind::S_CALLSITEINFO)
				{
					const std::string typeName = GetVariableTypeName(tpiStream, data.S_CALLSITEINFO.typeIndex);
					printf("%*sS_CALLSITEINFO '%s' | Offset 0x%X | Section %u\n", blockLevel * 4, "" , typeName.c_str(), data.S_CALLSITEINFO.offset, data.S_CALLSITEINFO.section);
				}
				else if(kind == SymbolRecordKind::S_HEAPALLOCSITE)
				{
					const std::string typeName = GetVariableTypeName(tpiStream, data.S_HEAPALLOCSITE.typeIndex);
					Printf(blockLevel, "S_HEAPALLOCSITE '%s' | Offset 0x%X | Section %u | Instruction Length %u\n", typeName.c_str(), data.S_HEAPALLOCSITE.offset, data.S_HEAPALLOCSITE.section, data.S_HEAPALLOCSITE.instructionLength);
				}
				else if (record->header.kind == PDB::CodeView::DBI::SymbolRecordKind::S_FRAMEPROC)
				{
					functionSymbols[functionSymbols.size() - 1].frameProc = record;
					return;
				}
				else if (record->header.kind == PDB::CodeView::DBI::SymbolRecordKind::S_THUNK32)
				{
					if (data.S_THUNK32.thunk == PDB::CodeView::DBI::ThunkOrdinal::TrampolineIncremental)
					{
						// we have never seen incremental linking thunks stored inside a S_THUNK32 symbol, but better safe than sorry
						name = "ILT - Thunk";
						rva = imageSectionStream.ConvertSectionOffsetToRVA(data.S_THUNK32.section, data.S_THUNK32.offset);
						size = 5u;
					}
					else
					{
						name = data.S_THUNK32.name;
						rva = imageSectionStream.ConvertSectionOffsetToRVA(data.S_THUNK32.section, data.S_THUNK32.offset);
						size = data.S_THUNK32.length; // Correct ?
					}
				}
				else if (record->header.kind == PDB::CodeView::DBI::SymbolRecordKind::S_TRAMPOLINE)
				{
					// incremental linking thunks are stored in the linker module
					name = "ILT - Trampoline";
					rva = imageSectionStream.ConvertSectionOffsetToRVA(data.S_TRAMPOLINE.thunkSection, data.S_TRAMPOLINE.thunkOffset);
					size = 5u;
				}
				else if (record->header.kind == PDB::CodeView::DBI::SymbolRecordKind::S_LPROC32)
				{
					name = data.S_LPROC32.name;
					rva = imageSectionStream.ConvertSectionOffsetToRVA(data.S_LPROC32.section, data.S_LPROC32.offset);
					size = data.S_LPROC32.codeSize;
				}
				else if (record->header.kind == PDB::CodeView::DBI::SymbolRecordKind::S_GPROC32)
				{
					name = data.S_GPROC32.name;
					rva = imageSectionStream.ConvertSectionOffsetToRVA(data.S_GPROC32.section, data.S_GPROC32.offset);
					size = data.S_GPROC32.codeSize;
				}
				else if (record->header.kind == PDB::CodeView::DBI::SymbolRecordKind::S_LPROC32_ID)
				{
					name = data.S_LPROC32_ID.name;
					rva = imageSectionStream.ConvertSectionOffsetToRVA(data.S_LPROC32_ID.section, data.S_LPROC32_ID.offset);
					size = data.S_LPROC32_ID.codeSize;
				}
				else if (record->header.kind == PDB::CodeView::DBI::SymbolRecordKind::S_GPROC32_ID)
				{
					name = data.S_GPROC32_ID.name;
					rva = imageSectionStream.ConvertSectionOffsetToRVA(data.S_GPROC32_ID.section, data.S_GPROC32_ID.offset);
					size = data.S_GPROC32_ID.codeSize;
				}
				else
				{
					if (blockLevel != 0)
					{
						PDB_ASSERT(false, "Unhandled record kind 0x%X with block identation %u\n", record->header.kind, blockLevel);
					}
				}

				if (rva == 0u)
				{
					return;
				}

				PDB_ASSERT(blockLevel == 0, "BlockLevel %u != 0", blockLevel);

				printf("%*sFunction: '%s' | RVA 0x%X\n", blockLevel*4, "", name, rva);

				if (kind != SymbolRecordKind::S_TRAMPOLINE)
				{
					blockLevel++;
				}

				functionSymbols.push_back(FunctionSymbol{ name, rva, size, nullptr, {0, 0} });
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
			functionSymbols.push_back(FunctionSymbol{ record->data.S_PUB32.name, rva, 0u, nullptr, {0, 0} });
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
		if (lastSymbol.size != 0u)
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
