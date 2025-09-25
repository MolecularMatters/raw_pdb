// Copyright 2011-2022, Molecular Matters GmbH <office@molecular-matters.com>
// See LICENSE.txt for licensing details (2-clause BSD License: https://opensource.org/licenses/BSD-2-Clause)

#include "Examples_PCH.h"
#include "ExampleTimedScope.h"
#include "ExampleTypeTable.h"
#include "PDB_RawFile.h"
#include "PDB_DBIStream.h"
#include "PDB_TPIStream.h"

using SymbolRecordKind = PDB::CodeView::DBI::SymbolRecordKind;

static std::string GetVariableTypeName(const TypeTable& typeTable, uint32_t typeIndex)
{
	// Defined in ExampleTypes.cpp
	extern std::string GetTypeName(const TypeTable & typeTable, uint32_t typeIndex);

	std::string typeName = GetTypeName(typeTable, typeIndex);

	// Remove any '%s' substring used to insert a variable/field name.
	const uint64_t markerPos = typeName.find("%s");
	if (markerPos != typeName.npos)
	{
		typeName.erase(markerPos, 2);
	}

	return typeName;
}

static void Printf(uint32_t indent, const char* format, ...) 
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

	TimedScope typeTableScope("Create TypeTable");
	TypeTable typeTable(tpiStream);
	typeTableScope.Done();

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

	{
		TimedScope scope("Printing function variable records from modules\n");

		const PDB::ArrayView<PDB::ModuleInfoStream::Module> modules = moduleInfoStream.GetModules();

		uint32_t blockLevel = 0;
		uint32_t recordCount = 0;

		for (const PDB::ModuleInfoStream::Module& module : modules)
		{
			if (!module.HasSymbolStream())
			{
				continue;
			}

			const PDB::ModuleSymbolStream moduleSymbolStream = module.CreateSymbolStream(rawPdbFile);
			moduleSymbolStream.ForEachSymbol([&typeTable, &imageSectionStream, &blockLevel, &recordCount](const PDB::CodeView::DBI::Record* record)
			{
				const SymbolRecordKind kind = record->header.kind;
				const PDB::CodeView::DBI::Record::Data& data = record->data;

				if (kind == SymbolRecordKind::S_END)
				{
					PDB_ASSERT(blockLevel > 0, "Block level for S_END is 0");
					blockLevel--;
					Printf(blockLevel, "S_END\n");

					if (blockLevel == 0)
					{
						Printf(0, "\n");
					}
				}
				else if(kind == SymbolRecordKind::S_SKIP)
				{
					Printf(blockLevel, "S_SKIP\n");
				}
				else if (kind == SymbolRecordKind::S_BLOCK32)
				{
					const uint32_t offset = imageSectionStream.ConvertSectionOffsetToRVA(data.S_BLOCK32.section, data.S_BLOCK32.offset);

					Printf(blockLevel, "S_BLOCK32: '%s' | Code Offset 0x%X\n", data.S_BLOCK32.name, offset);
					blockLevel++;
				}
				else if (kind == SymbolRecordKind::S_LABEL32)
				{
					Printf(blockLevel, "S_LABEL32: '%s' | Offset 0x%X\n", data.S_LABEL32.name, data.S_LABEL32.offset);
				}
				else if(kind == SymbolRecordKind::S_CONSTANT)
				{
					const std::string typeName = GetVariableTypeName(typeTable, data.S_CONSTANT.typeIndex);

					Printf(blockLevel, "S_CONSTANT: '%s' -> '%s' | Value 0x%X\n", typeName.c_str(), data.S_CONSTANT.name, data.S_CONSTANT.value);
				}
				else if(kind == SymbolRecordKind::S_LOCAL)
				{
					const std::string typeName = GetVariableTypeName(typeTable, data.S_LOCAL.typeIndex);
					Printf(blockLevel, "S_LOCAL: '%s' -> '%s' | Param: %s | Optimized Out: %s\n", typeName.c_str(), data.S_LOCAL.name, data.S_LOCAL.flags.fIsParam ? "True" : "False", data.S_LOCAL.flags.fIsOptimizedOut ? "True" : "False");
				}
				else if (kind == SymbolRecordKind::S_DEFRANGE_REGISTER)
				{
					Printf(blockLevel, "S_DEFRANGE_REGISTER: Register 0x%X\n", data.S_DEFRANGE_REGISTER.reg);
				}
				else if(kind == SymbolRecordKind::S_DEFRANGE_FRAMEPOINTER_REL)
				{
					Printf(blockLevel, "S_DEFRANGE_FRAMEPOINTER_REL: Frame Pointer Offset 0x%X | Range Start 0x%X | Range Section Start 0x%X | Range Length %u\n", 
							data.S_DEFRANGE_FRAMEPOINTER_REL.offsetFramePointer, 
							data.S_DEFRANGE_FRAMEPOINTER_REL.range.offsetStart, 
							data.S_DEFRANGE_FRAMEPOINTER_REL.range.isectionStart, 
							data.S_DEFRANGE_FRAMEPOINTER_REL.range.length);
				}
				else if(kind == SymbolRecordKind::S_DEFRANGE_SUBFIELD_REGISTER)
				{
					Printf(blockLevel, "S_DEFRANGE_SUBFIELD_REGISTER: Register %u | Parent offset 0x%X | Range Start 0x%X | Range Section Start 0x%X | Range Length %u\n", 
						data.S_DEFRANGE_SUBFIELD_REGISTER.reg,
						data.S_DEFRANGE_SUBFIELD_REGISTER.offsetParent,
						data.S_DEFRANGE_SUBFIELD_REGISTER.range.offsetStart, 
						data.S_DEFRANGE_SUBFIELD_REGISTER.range.isectionStart, 
						data.S_DEFRANGE_SUBFIELD_REGISTER.range.length);
				}
				else if (kind == SymbolRecordKind::S_DEFRANGE_FRAMEPOINTER_REL_FULL_SCOPE)
				{
					Printf(blockLevel, "S_DEFRANGE_FRAMEPOINTER_REL_FULL_SCOPE: Offset 0x%X\n", data.S_DEFRANGE_FRAMEPOINTER_REL_FULL_SCOPE.offsetFramePointer);
				}
				else if (kind == SymbolRecordKind::S_DEFRANGE_REGISTER_REL)
				{
					Printf(blockLevel, "S_DEFRANGE_REGISTER_REL: Base Register %u | Parent offset 0x%X | Base Register Offset 0x%X | Range Start 0x%X | Range Section Start 0x%X | Range Length %u\n",
						data.S_DEFRANGE_REGISTER_REL.baseRegister,
						data.S_DEFRANGE_REGISTER_REL.offsetParent,
						data.S_DEFRANGE_REGISTER_REL.offsetBasePointer,
						data.S_DEFRANGE_REGISTER_REL.offsetParent,
						data.S_DEFRANGE_REGISTER_REL.range.offsetStart, 
						data.S_DEFRANGE_REGISTER_REL.range.isectionStart, 
						data.S_DEFRANGE_REGISTER_REL.range.length);						
				}
				else if(kind == SymbolRecordKind::S_FILESTATIC)
				{
					Printf(blockLevel, "S_FILESTATIC: '%s'\n", data.S_FILESTATIC.name);
				}
				else if (kind == SymbolRecordKind::S_INLINESITE)
				{
					Printf(blockLevel, "S_INLINESITE: Parent 0x%X\n", data.S_INLINESITE.parent);
					blockLevel++;
				}
				else if (kind == SymbolRecordKind::S_INLINESITE_END)
				{
					PDB_ASSERT(blockLevel > 0, "Block level for S_INLINESITE_END is 0");
					blockLevel--;					
					Printf(blockLevel, "S_INLINESITE_END:\n");
				}
				else if (kind == SymbolRecordKind::S_CALLEES)
				{
					Printf(blockLevel, "S_CALLEES: Count %u\n", data.S_CALLEES.count);
				}
				else if (kind == SymbolRecordKind::S_CALLERS)
				{
					Printf(blockLevel, "S_CALLERS: Count %u\n", data.S_CALLERS.count);
				}
				else if (kind == SymbolRecordKind::S_INLINEES)
				{
					Printf(blockLevel, "S_INLINEES: Count %u\n", data.S_INLINEES.count);
				}
				else if (kind == SymbolRecordKind::S_LDATA32)
				{
					if (blockLevel > 0)
					{
						// Not sure why some type index 0 (T_NO_TYPE) are included in some PDBs.
						if (data.S_LDATA32.typeIndex != 0) // PDB::CodeView::TPI::TypeIndexKind::T_NOTYPE)
						{
							const std::string typeName = GetVariableTypeName(typeTable, data.S_LDATA32.typeIndex);
							Printf(blockLevel, "S_LDATA32: '%s' -> '%s'\n", data.S_LDATA32.name, typeName.c_str());
						}						
					}
				}
				else if (kind == SymbolRecordKind::S_LTHREAD32)
				{
					if (blockLevel > 0)
					{
						const std::string typeName = GetVariableTypeName(typeTable, data.S_LTHREAD32.typeIndex);
						Printf(blockLevel, "S_LTHREAD32: '%s' -> '%s'\n", data.S_LTHREAD32.name, typeName.c_str());
					}
				}
				else if (kind == SymbolRecordKind::S_UDT)
				{
					const std::string typeName = GetVariableTypeName(typeTable, data.S_UDT.typeIndex);

					Printf(blockLevel, "S_UDT: '%s' -> '%s'\n", data.S_UDT.name, typeName.c_str());
				}
				else if (kind == PDB::CodeView::DBI::SymbolRecordKind::S_REGISTER)
				{
					const std::string typeName = GetVariableTypeName(typeTable, data.S_REGSYM.typeIndex);

					Printf(blockLevel, "S_REGSYM: '%s' -> '%s' | Register %i\n",
						data.S_REGSYM.name, typeName.c_str(),
						data.S_REGSYM.reg);
				}
				else if (kind == PDB::CodeView::DBI::SymbolRecordKind::S_BPREL32)
				{
					const std::string typeName = GetVariableTypeName(typeTable, data.S_BPRELSYM32.typeIndex);

					Printf(blockLevel, "S_BPRELSYM32: '%s' -> '%s' | BP register Offset 0x%X\n",
						data.S_BPRELSYM32.name, typeName.c_str(),
						data.S_BPRELSYM32.offset);
				}
				else if (kind == PDB::CodeView::DBI::SymbolRecordKind::S_REGREL32)
				{
					const std::string typeName = GetVariableTypeName(typeTable, data.S_REGREL32.typeIndex);
	
					Printf(blockLevel, "S_REGREL32: '%s' -> '%s' | Register %i | Register Offset 0x%X\n", 
						data.S_REGREL32.name, typeName.c_str(), 
						data.S_REGREL32.reg, 
						data.S_REGREL32.offset);
				}
				else if(kind == SymbolRecordKind::S_FRAMECOOKIE)
				{
					Printf(blockLevel, "S_FRAMECOOKIE: Offset 0x%X | Register %u | Type %u\n", 
						data.S_FRAMECOOKIE.offset, 
						data.S_FRAMECOOKIE.reg, 
						data.S_FRAMECOOKIE.cookietype);
				}
				else if(kind == SymbolRecordKind::S_CALLSITEINFO)
				{
					const std::string typeName = GetVariableTypeName(typeTable, data.S_CALLSITEINFO.typeIndex);
					Printf(blockLevel, "S_CALLSITEINFO: '%s' | Offset 0x%X | Section %u\n", typeName.c_str(), data.S_CALLSITEINFO.offset, data.S_CALLSITEINFO.section);
				}
				else if(kind == SymbolRecordKind::S_HEAPALLOCSITE)
				{
					const std::string typeName = GetVariableTypeName(typeTable, data.S_HEAPALLOCSITE.typeIndex);
					Printf(blockLevel, "S_HEAPALLOCSITE: '%s' | Offset 0x%X | Section %u | Instruction Length %u\n", typeName.c_str(), 
						data.S_HEAPALLOCSITE.offset, 
						data.S_HEAPALLOCSITE.section, 
						data.S_HEAPALLOCSITE.instructionLength);
				}
				else if (kind == SymbolRecordKind::S_FRAMEPROC)
				{
					Printf(blockLevel, "S_FRAMEPROC: Size %u | Padding %u | Padding Offset 0x%X | Callee Registers Size %u\n", 
						data.S_FRAMEPROC.cbFrame, 
						data.S_FRAMEPROC.cbPad, 
						data.S_FRAMEPROC.offPad, 
						data.S_FRAMEPROC.cbSaveRegs);
				}
				else if (kind == SymbolRecordKind::S_ANNOTATION)
				{
					Printf(blockLevel, "S_ANNOTATION: Offset 0x%X | Count %u\n", data.S_ANNOTATIONSYM.offset, data.S_ANNOTATIONSYM.annotationsCount);
					// print N null-terminated annotation strings, skipping their null-terminators to get to the next string
					const char* annotation = data.S_ANNOTATIONSYM.annotations;
					for (int i = 0; i < data.S_ANNOTATIONSYM.annotationsCount; ++i, annotation += strlen(annotation) + 1)
						Printf(blockLevel + 1, "S_ANNOTATION.%u: %s\n", i, annotation);
					PDB_ASSERT(annotation <= (const char*)record + record->header.size + sizeof(record->header.size),
						"Annotation strings end beyond the record size %X; annotaions count: %u", record->header.size, data.S_ANNOTATIONSYM.annotationsCount);
				}
				else if (kind == SymbolRecordKind::S_THUNK32)
				{
					PDB_ASSERT(blockLevel == 0, "BlockLevel %u != 0", blockLevel);

					if (data.S_THUNK32.thunk == PDB::CodeView::DBI::ThunkOrdinal::TrampolineIncremental)
					{
						// we have never seen incremental linking thunks stored inside a S_THUNK32 symbol, but better safe than sorry
						const uint32_t rva = imageSectionStream.ConvertSectionOffsetToRVA(data.S_THUNK32.section, data.S_THUNK32.offset);
						Printf(blockLevel, "Function: 'ILT/Thunk' | RVA 0x%X\n", rva);
					}
					else
					{
						const uint32_t rva = imageSectionStream.ConvertSectionOffsetToRVA(data.S_THUNK32.section, data.S_THUNK32.offset);
						Printf(blockLevel, "S_THUNK32 Function '%s' | RVA 0x%X\n", data.S_THUNK32.name, rva);
						blockLevel++;
					}
				}
				else if (kind == SymbolRecordKind::S_TRAMPOLINE)
				{
					PDB_ASSERT(blockLevel == 0, "BlockLevel %u != 0", blockLevel);
					// incremental linking thunks are stored in the linker module
					const uint32_t rva = imageSectionStream.ConvertSectionOffsetToRVA(data.S_TRAMPOLINE.thunkSection, data.S_TRAMPOLINE.thunkOffset);
					Printf(blockLevel, "Function 'ILT/Trampoline' | RVA 0x%X\n", rva);
				}
				else if (kind == SymbolRecordKind::S_LPROC32)
				{
					PDB_ASSERT(blockLevel == 0, "BlockLevel %u != 0", blockLevel);
					const uint32_t rva = imageSectionStream.ConvertSectionOffsetToRVA(data.S_LPROC32.section, data.S_LPROC32.offset);
					Printf(blockLevel, "S_LPROC32 Function '%s' | RVA 0x%X\n", data.S_LPROC32.name, rva);
					blockLevel++;
				}
				else if (kind == SymbolRecordKind::S_GPROC32)
				{
					PDB_ASSERT(blockLevel == 0, "BlockLevel %u != 0", blockLevel);
					const uint32_t rva = imageSectionStream.ConvertSectionOffsetToRVA(data.S_GPROC32.section, data.S_GPROC32.offset);
					Printf(blockLevel, "S_GPROC32 Function '%s' | RVA 0x%X\n", data.S_GPROC32.name, rva);
					blockLevel++;
				}
				else if (kind == SymbolRecordKind::S_LPROC32_ID)
				{
					PDB_ASSERT(blockLevel == 0, "BlockLevel %u != 0", blockLevel);
					const uint32_t rva = imageSectionStream.ConvertSectionOffsetToRVA(data.S_LPROC32_ID.section, data.S_LPROC32_ID.offset);
					Printf(blockLevel, "S_LPROC32_ID Function '%s' | RVA 0x%X\n", data.S_LPROC32_ID.name, rva);
					blockLevel++;
				}
				else if (kind == SymbolRecordKind::S_GPROC32_ID)
				{
					PDB_ASSERT(blockLevel == 0, "BlockLevel %u != 0", blockLevel);
					const uint32_t rva = imageSectionStream.ConvertSectionOffsetToRVA(data.S_GPROC32_ID.section, data.S_GPROC32_ID.offset);
					Printf(blockLevel, "S_GPROC32_ID Function '%s' | RVA 0x%X\n", data.S_GPROC32_ID.name, rva);
					blockLevel++;
				}
				else if (kind == SymbolRecordKind::S_REGREL32_INDIR)
				{
					const std::string typeName = GetVariableTypeName(typeTable, data.S_REGREL32_INDIR.typeIndex);

					Printf(blockLevel, "S_REGREL32_INDIR: '%s' -> '%s' | Register %i | Unknown1 0x%X | Unknown2 0x%X\n",
						data.S_REGREL32_INDIR.name, typeName.c_str(),
						data.S_REGREL32_INDIR.unknown1,
						data.S_REGREL32_INDIR.unknown1);
				}
				else if (kind == SymbolRecordKind::S_REGREL32_ENCTMP)
				{
					const std::string typeName = GetVariableTypeName(typeTable, data.S_REGREL32.typeIndex);

					Printf(blockLevel, "S_REGREL32_ENCTMP: '%s' -> '%s' | Register %i | Register Offset 0x%X\n",
						data.S_REGREL32.name, typeName.c_str(),
						data.S_REGREL32.reg,
						data.S_REGREL32.offset);
				}
				else if (kind == SymbolRecordKind::S_UNAMESPACE)
				{
					Printf(blockLevel, "S_UNAMESPACE: '%s'\n", data.S_UNAMESPACE.name);
				}
				else if (kind == SymbolRecordKind::S_ARMSWITCHTABLE)
				{
					Printf(blockLevel, "S_ARMSWITCHTABLE: "
						"Switch Type: %u | Num Entries: %u | Base Section: %u | Base Offset: 0x%X | "
						"Branch Section: %u | Branch Offset: 0x%X | Table Section: %u | Table Offset: 0x%X\n",
							data.S_ARMSWITCHTABLE.switchType,
							data.S_ARMSWITCHTABLE.numEntries,
							data.S_ARMSWITCHTABLE.sectionBase,
							data.S_ARMSWITCHTABLE.offsetBase,
							data.S_ARMSWITCHTABLE.sectionBranch,
							data.S_ARMSWITCHTABLE.offsetBranch,
							data.S_ARMSWITCHTABLE.sectionTable,
							data.S_ARMSWITCHTABLE.offsetTable);
				}
				else
				{
					// We only care about records inside functions.
					if (blockLevel > 0)
					{
						PDB_ASSERT(false, "Unhandled record kind 0x%X with block level %u\n", static_cast<uint16_t>(kind), blockLevel);
					}
				}

				recordCount++;

			});
		}

		scope.Done(recordCount);
	}
}
