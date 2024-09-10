// Copyright 2011-2022, Molecular Matters GmbH <office@molecular-matters.com>
// See LICENSE.txt for licensing details (2-clause BSD License: https://opensource.org/licenses/BSD-2-Clause)

#pragma once

#include "Foundation/PDB_Macros.h"
#include "Foundation/PDB_BitOperators.h"
#include "Foundation/PDB_DisableWarningsPush.h"
#include <cstdint>
#include "Foundation/PDB_DisableWarningsPop.h"


namespace PDB
{
	namespace DBI
	{
		// https://llvm.org/docs/PDB/DbiStream.html#stream-header
		// https://github.com/microsoft/microsoft-pdb/blob/master/PDB/dbi/dbi.h#L124
		struct StreamHeader
		{
			static const uint32_t Signature;

			enum class PDB_NO_DISCARD Version : uint32_t
			{
				VC41 = 930803u,
				V50 = 19960307u,
				V60 = 19970606u,
				V70 = 19990903u,
				V110 = 20091201u
			};

			uint32_t signature;
			Version version;
			uint32_t age;
			uint16_t globalStreamIndex;						// index of the global symbol stream
			uint16_t toolchain;
			uint16_t publicStreamIndex;						// index of the public symbol stream
			uint16_t pdbDllVersion;
			uint16_t symbolRecordStreamIndex;				// index of the symbol record stream
			uint16_t pdbDllRbld;
			uint32_t moduleInfoSize;
			uint32_t sectionContributionSize;
			uint32_t sectionMapSize;
			uint32_t sourceInfoSize;
			uint32_t typeServerMapSize;
			uint32_t mfcTypeServerIndex;
			uint32_t optionalDebugHeaderSize;
			uint32_t ecSize;
			uint16_t flags;
			uint16_t machine;
			uint32_t padding;
		};

		// https://llvm.org/docs/PDB/DbiStream.html#optional-debug-header-stream
		struct DebugHeader
		{
			static const uint16_t InvalidStreamIndex;

			uint16_t fpoDataStreamIndex;						// IMAGE_DEBUG_TYPE_FPO
			uint16_t exceptionDataStreamIndex;					// IMAGE_DEBUG_TYPE_EXCEPTION
			uint16_t fixupDataStreamIndex;						// IMAGE_DEBUG_TYPE_FIXUP
			uint16_t omapToSrcDataStreamIndex;					// IMAGE_DEBUG_TYPE_OMAP_TO_SRC
			uint16_t omapFromSrcDataStreamIndex;				// IMAGE_DEBUG_TYPE_OMAP_FROM_SRC
			uint16_t sectionHeaderStreamIndex;					// a dump of all section headers (IMAGE_SECTION_HEADER) from the original executable
			uint16_t tokenDataStreamIndex;
			uint16_t xdataStreamIndex;
			uint16_t pdataStreamIndex;
			uint16_t newFpoDataStreamIndex;
			uint16_t originalSectionHeaderDataStreamIndex;
		};

		// https://llvm.org/docs/PDB/DbiStream.html#section-contribution-substream
		struct SectionContribution
		{
			enum class PDB_NO_DISCARD Version : uint32_t
			{
				Ver60 = 0xeffe0000u + 19970605u,
				V2 = 0xeffe0000u + 20140516u
			};

			uint16_t section;
			uint16_t padding;
			uint32_t offset;
			uint32_t size;
			uint32_t characteristics;
			uint16_t moduleIndex;
			uint16_t padding2;
			uint32_t dataCrc;
			uint32_t relocationCrc;
		};

		// https://llvm.org/docs/PDB/DbiStream.html#module-info-substream
		struct ModuleInfo
		{
			uint32_t unused;
			SectionContribution sectionContribution;
			uint16_t flags;
			uint16_t moduleSymbolStreamIndex;
			uint32_t symbolSize;
			uint32_t c11Size;
			uint32_t c13Size;
			uint16_t sourceFileCount;
			uint16_t padding;
			uint32_t unused2;
			uint32_t sourceFileNameIndex;
			uint32_t pdbFilePathNameIndex;
		};
	}


	namespace CodeView
	{
		namespace DBI
		{
			// code view type records that can appear in a DBI stream.
			// this list is not exhaustive, but only contains what we need so far.
			// https://llvm.org/docs/PDB/CodeViewSymbols.html
			// https://llvm.org/docs/PDB/TpiStream.html#tpi-vs-ipi-stream
			// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L2735
			enum class PDB_NO_DISCARD SymbolRecordKind : uint16_t
			{
				S_END =										0x0006u,		// block, procedure, "with" or thunk end
				S_SKIP =									0x0007u,        // Reserve symbol space in $$Symbols table
				S_FRAMEPROC =								0x1012u,		// extra frame and proc information
				S_ANNOTATION =								0x1019u,		// annotation string literals ("__annotation" intrinsic, e.g. via NT_ASSERT)
				S_OBJNAME =									0x1101u,		// full path to the original compiled .obj. can point to remote locations and temporary files, not necessarily the file that was linked into the executable
				S_THUNK32 =									0x1102u,		// thunk start
				S_BLOCK32 =									0x1103u,		// block start
				S_LABEL32 =									0x1105u,		// code label
				S_REGISTER =								0x1106u,		// register variable
				S_CONSTANT =								0x1107u,		// constant symbol
				S_BPREL32 =									0x110Bu,		// BP-relative address (almost like S_REGREL32)
				S_LDATA32 =									0x110Cu,		// (static) local data
				S_GDATA32 =									0x110Du,		// global data
				S_PUB32 =									0x110Eu,		// public symbol
				S_LPROC32 =									0x110Fu,		// local procedure start
				S_GPROC32 =									0x1110u,		// global procedure start
				S_REGREL32 =								0x1111u,		// register relative address
				S_LTHREAD32 =								0x1112u,		// (static) thread-local data
				S_GTHREAD32 =								0x1113u,		// global thread-local data
				S_UNAMESPACE =								0x1124u,		// using namespace
				S_PROCREF =									0x1125u,		// reference to function in any compiland
				S_LPROCREF =								0x1127u,		// local reference to function in any compiland
				S_TRAMPOLINE =								0x112Cu,		// incremental linking trampoline
				S_SEPCODE =									0x1132u,		// separated code (from the compiler)
				S_SECTION =									0x1136u,		// a COFF section in an executable
				S_COFFGROUP =								0x1137u,		// original COFF group before it was merged into executable sections by the linker, e.g. .CRT$XCU, .rdata, .bss, .lpp_prepatch_hooks
				S_CALLSITEINFO = 							0x1139u,		// Indirect call site information
				S_FRAMECOOKIE = 							0x113Au, 		// Security cookie information
				S_COMPILE3 =								0x113Cu,		// replacement for S_COMPILE2, more info
				S_ENVBLOCK =								0x113Du,		// environment block split off from S_COMPILE2
				S_LOCAL =									0x113Eu,		// defines a local symbol in optimized code 
				S_DEFRANGE_REGISTER =						0x1141u,		// ranges for en-registered symbol
				S_DEFRANGE_FRAMEPOINTER_REL =				0x1142u,		// range for stack symbol.
				S_DEFRANGE_SUBFIELD_REGISTER =				0x1143u,		// ranges for en-registered field of symbol
				S_DEFRANGE_FRAMEPOINTER_REL_FULL_SCOPE =	0x1144u, 		// range for stack symbol span valid full scope of function body, gap might apply.
				S_DEFRANGE_REGISTER_REL = 					0x1145u, 		// range for symbol address as register + offset.
				S_LPROC32_ID =								0x1146u,		// S_PROC symbol that references ID instead of type
				S_GPROC32_ID =								0x1147u,		// S_PROC symbol that references ID instead of type
				S_BUILDINFO =								0x114Cu,		// build info/environment details of a compiland/translation unit
				S_INLINESITE =								0x114Du,		// inlined function callsite
				S_INLINESITE_END =							0x114Eu,
				S_PROC_ID_END =								0x114Fu,
				S_FILESTATIC =								0x1153u,
				S_LPROC32_DPC =								0x1155u,
				S_LPROC32_DPC_ID =							0x1156u,
				S_CALLEES =									0x115Au,
				S_CALLERS =									0x115Bu,
				S_INLINESITE2 =								0x115Du,		// extended inline site information
				S_HEAPALLOCSITE = 							0x115Eu,		// heap allocation site
				S_INLINEES =			 					0x1168u,		// https://llvm.org/docs/PDB/CodeViewSymbols.html#s-inlinees-0x1168
				S_REGREL32_INDIR =							0x1171u,
				S_REGREL32_ENCTMP =							0x1179u,
				S_UDT =										0x1108u,		// user-defined type
				S_UDT_ST =									0x1003u,		// user-defined structured types
			};

			// https://docs.microsoft.com/en-us/visualstudio/debugger/debug-interface-access/thunk-ordinal
			enum class PDB_NO_DISCARD ThunkOrdinal : uint8_t
			{
				NoType,
				ThisAdjustor,
				VirtualCall,
				PCode,
				DelayLoad,
				TrampolineIncremental,
				TrampolineBranchIsland
			};

			enum class PDB_NO_DISCARD TrampolineType : uint16_t
			{
				Incremental,
				BranchIsland
			};

			enum class PDB_NO_DISCARD CookieType : uint8_t
			{
			   COPY = 0, 
			   XOR_SP, 
			   XOR_BP,
			   XOR_R13,
			};

			// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvconst.h#L392
			enum class PDB_NO_DISCARD Register : uint16_t
			{
				EAX = 17,
				ECX = 18,
				EDX = 19,
				EBX = 20,
				ESP = 21,
				EBP = 22,
				ESI = 23,
				EDI = 24,

				RAX = 328,
				RBX = 329,
				RCX = 330,
				RDX = 331,
				RSI = 332,
				RDI = 333,
				RBP = 334,
				RSP = 335,
				R8 = 336,
				R9 = 337,
				R10 = 338,
				R11 = 339,
				R12 = 340,
				R13 = 341,
				R14 = 342,
				R15 = 343,

				RIP = 33,		// also EIP for x32
				EFLAGS = 34,	// same for x64 and x32
			};


			// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L3038
			enum class PDB_NO_DISCARD ProcedureFlags : uint8_t
			{
				None = 0u,
				NoFPO = 1u << 0u,
				InterruptReturn = 1u << 1u,
				FarReturn = 1u << 2u,
				NoReturn = 1u << 3u,
				Unreachable = 1u << 4u,
				CustomCallingConvention = 1u << 5u,
				NoInline = 1u << 6u,
				OptimizedDebugInformation = 1u << 7u
			};
			PDB_DEFINE_BIT_OPERATORS(ProcedureFlags);


			// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L3676
			enum class PDB_NO_DISCARD PublicSymbolFlags : uint32_t
			{
				None = 0u,
				Code = 1u << 0u,					// set if public symbol refers to a code address
				Function = 1u << 1u,				// set if public symbol is a function
				ManagedCode = 1u << 2u,				// set if managed code (native or IL)
				ManagedILCode = 1u << 3u			// set if managed IL code
			};
			PDB_DEFINE_BIT_OPERATORS(PublicSymbolFlags);


			// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L3341
			enum class PDB_NO_DISCARD CompileSymbolFlags : uint32_t
			{
				None = 0u,
				SourceLanguageMask = 0xFFu,
				EC = 1u << 8u,
				NoDebugInfo = 1u << 9u,
				LTCG = 1u << 10u,
				NoDataAlign = 1u << 11u,
				ManagedCodeOrDataPresent = 1u << 12u,
				SecurityChecks = 1u << 13u,
				HotPatch = 1u << 14u,
				CVTCIL = 1u << 15u,
				MSILModule = 1u << 16u,
				SDL = 1u << 17u,
				PGO = 1u << 18u,
				Exp = 1u << 19u
			};
			PDB_DEFINE_BIT_OPERATORS(CompileSymbolFlags);


			// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvconst.h#L324
			enum class PDB_NO_DISCARD CPUType : uint16_t
			{
				Intel8080 = 0x0,
				Intel8086 = 0x1,
				Intel80286 = 0x2,
				Intel80386 = 0x3,
				Intel80486 = 0x4,
				Pentium = 0x5,
				PentiumII = 0x6,
				PentiumPro = PentiumII,
				PentiumIII = 0x7,
				MIPS = 0x10,
				MIPSR4000 = MIPS,
				MIPS16 = 0x11,
				MIPS32 = 0x12,
				MIPS64 = 0x13,
				MIPSI = 0x14,
				MIPSII = 0x15,
				MIPSIII = 0x16,
				MIPSIV = 0x17,
				MIPSV = 0x18,
				M68000 = 0x20,
				M68010 = 0x21,
				M68020 = 0x22,
				M68030 = 0x23,
				M68040 = 0x24,
				Alpha = 0x30,
				Alpha21164 = 0x31,
				Alpha21164A = 0x32,
				Alpha21264 = 0x33,
				Alpha21364 = 0x34,
				PPC601 = 0x40,
				PPC603 = 0x41,
				PPC604 = 0x42,
				PPC620 = 0x43,
				PPCFP = 0x44,
				PPCBE = 0x45,
				SH3 = 0x50,
				SH3E = 0x51,
				SH3DSP = 0x52,
				SH4 = 0x53,
				SHMedia = 0x54,
				ARM3 = 0x60,
				ARM4 = 0x61,
				ARM4T = 0x62,
				ARM5 = 0x63,
				ARM5T = 0x64,
				ARM6 = 0x65,
				ARM_XMAC = 0x66,
				ARM_WMMX = 0x67,
				ARM7 = 0x68,
				Omni = 0x70,
				IA64 = 0x80,
				IA64_1 = 0x80,
				IA64_2 = 0x81,
				CEE = 0x90,
				AM33 = 0xA0,
				M32R = 0xB0,
				TriCore = 0xC0,
				X64 = 0xD0,
				AMD64 = X64,
				EBC = 0xE0,
				Thumb = 0xF0,
				ARMNT = 0xF4,
				ARM64 = 0xF6,
				HybridX86ARM64 = 0xF7,
				ARM64EC = 0xF8,
				ARM64X = 0xF9,
				D3D11_Shader = 0x100
			};

			// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L3100
			// represents an address range, used for optimized code debug info
			struct LocalVariableAddressRange // defines a range of addresses
			{
				uint32_t offsetStart;
				uint16_t isectionStart;
				uint16_t length;
			};

			// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L3108
			// Represents the holes in overall address range, all address is pre-bbt. 
			// it is for compress and reduce the amount of relocations need.
			struct LocalVariableAddressGap
			{
				uint16_t offset; // relative offset from the beginning of the live range.
				uint16_t length; // length of this gap.
			};

			// https://llvm.org/docs/PDB/CodeViewTypes.html#leaf-types
			struct RecordHeader
			{
				uint16_t size;					// record length, not including this 2-byte field
				SymbolRecordKind kind;			// record kind
			};

			// all CodeView records are stored as a header, followed by variable-length data.
			// internal Record structs such as S_PUB32, S_GDATA32, etc. correspond to the data layout of a CodeView record of that kind.
			struct Record
			{
				RecordHeader header;
				union Data
				{
#pragma pack(push, 1)
					// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L4069
					struct
					{
						uint32_t cbFrame;		// count of bytes of total frame of procedure
						uint32_t cbPad;			// count of bytes of padding in the frame
						uint32_t offPad;		// offset (relative to frame poniter) to where
												//  padding starts
						uint32_t cbSaveRegs;	// count of bytes of callee save registers
						uint32_t offExHdlr;		// offset of exception handler
						uint16_t sectExHdlr;	// section id of exception handler

						struct {
							uint32_t fHasAlloca : 1;				// function uses _alloca()
							uint32_t fHasSetJmp : 1;				// function uses setjmp()
							uint32_t fHasLongJmp : 1;				// function uses longjmp()
							uint32_t fHasInlAsm : 1;				// function uses inline asm
							uint32_t fHasEH : 1;					// function has EH states
							uint32_t fInlSpec : 1;					// function was speced as inline
							uint32_t fHasSEH : 1;					// function has SEH
							uint32_t fNaked : 1;					// function is __declspec(naked)
							uint32_t fSecurityChecks : 1;			// function has buffer security check introduced by /GS.
							uint32_t fAsyncEH : 1;					// function compiled with /EHa
							uint32_t fGSNoStackOrdering : 1;		// function has /GS buffer checks, but stack ordering couldn't be done
							uint32_t fWasInlined : 1;				// function was inlined within another function
							uint32_t fGSCheck : 1;					// function is __declspec(strict_gs_check)
							uint32_t fSafeBuffers : 1;				// function is __declspec(safebuffers)
							uint32_t encodedLocalBasePointer : 2;	// record function's local pointer explicitly.
							uint32_t encodedParamBasePointer : 2;	// record function's parameter pointer explicitly.
							uint32_t fPogoOn : 1;					// function was compiled with PGO/PGU
							uint32_t fValidCounts : 1;				// Do we have valid Pogo counts?
							uint32_t fOptSpeed : 1;					// Did we optimize for speed?
							uint32_t fGuardCF : 1;					// function contains CFG checks (and no write checks)
							uint32_t fGuardCFW : 1;					// function contains CFW checks and/or instrumentation
							uint32_t pad : 9;						// must be zero
						} flags;
					} S_FRAMEPROC;

					struct
					{
						uint32_t offset;
						uint16_t section;
						uint16_t annotationsCount;						// number of zero-terminated annotation strings
						PDB_FLEXIBLE_ARRAY_MEMBER(char, annotations);	// sequence of zero-terminated annotation strings
					} S_ANNOTATIONSYM;

					// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L3696
					struct
					{
						PublicSymbolFlags flags;
						uint32_t offset;
						uint16_t section;
						PDB_FLEXIBLE_ARRAY_MEMBER(char, name);
					} S_PUB32;

					struct
					{
						uint32_t typeIndex;
						uint32_t offset;
						uint16_t section;
						PDB_FLEXIBLE_ARRAY_MEMBER(char, name);
					} S_GDATA32, S_GTHREAD32, S_LDATA32, S_LTHREAD32;

					struct
					{
						PDB_FLEXIBLE_ARRAY_MEMBER(char, name);
					} S_UNAMESPACE;

					struct
					{
						uint32_t signature;
						PDB_FLEXIBLE_ARRAY_MEMBER(char, name);
					} S_OBJNAME;

					struct
					{
						TrampolineType type;
						uint16_t size;
						uint32_t thunkOffset;
						uint32_t targetOffset;
						uint16_t thunkSection;
						uint16_t targetSection;
					} S_TRAMPOLINE;

					struct
					{
						uint16_t sectionNumber;
						uint8_t alignment;
						uint32_t rva;
						uint32_t length;
						uint32_t characteristics;
						PDB_FLEXIBLE_ARRAY_MEMBER(char, name);
					} S_SECTION;

					struct
					{
						uint32_t size;
						uint32_t characteristics;
						uint32_t offset;
						uint16_t section;
						PDB_FLEXIBLE_ARRAY_MEMBER(char, name);
					} S_COFFGROUP;

					struct
					{
						uint32_t offset ;	// offset of call site
						uint16_t section; 	// section index of call site
						uint16_t padding; 	// alignment padding field, must be zero
						uint32_t typeIndex;	// type index describing function signature
					} S_CALLSITEINFO;

					struct
					{
						uint32_t offset; 		// Frame relative offset
						uint16_t reg;			// Register index
						CookieType cookietype;	// Type of the cookie
						uint8_t flags;			// Flags describing this cookie
					} S_FRAMECOOKIE;

					struct
					{
						uint32_t parent;
						uint32_t end;
						uint32_t next;
						uint32_t offset;
						uint16_t section;
						uint16_t length;
						ThunkOrdinal thunk;
						PDB_FLEXIBLE_ARRAY_MEMBER(char, name);
					} S_THUNK32;

					struct
					{
						uint32_t parent;
						uint32_t end;
						uint32_t next;
						uint32_t codeSize;
						uint32_t debugStart;
						uint32_t debugEnd;
						uint32_t typeIndex;
						uint32_t offset;
						uint16_t section;
						ProcedureFlags flags;
						PDB_FLEXIBLE_ARRAY_MEMBER(char, name);
					} S_LPROC32, S_GPROC32, S_LPROC32_ID, S_GPROC32_ID, S_LPROC32_DPC, S_LPROC32_DPC_ID;

					struct
					{
						uint32_t offset;
						uint32_t typeIndex;
						PDB_FLEXIBLE_ARRAY_MEMBER(char, name);
					} S_BPRELSYM32;

					struct
					{
						uint32_t offset;
						uint32_t typeIndex;
						Register reg;
						PDB_FLEXIBLE_ARRAY_MEMBER(char, name);
					} S_REGREL32, S_REGREL32_ENCTMP;

					struct
					{
						uint32_t typeIndex;
						Register reg;
						PDB_FLEXIBLE_ARRAY_MEMBER(char, name);
					} S_REGSYM;

					struct
					{
						uint32_t parent;
						uint32_t end;
						uint32_t codeSize;
						uint32_t offset;
						uint16_t section;
						PDB_FLEXIBLE_ARRAY_MEMBER(char, name);
					} S_BLOCK32;

					struct
					{
						uint32_t offset;
						uint16_t section;
						ProcedureFlags flags;
						PDB_FLEXIBLE_ARRAY_MEMBER(char, name);
					} S_LABEL32;

					struct
					{
						uint32_t typeIndex;
						uint16_t value;
						PDB_FLEXIBLE_ARRAY_MEMBER(char, name);
					} S_CONSTANT;

					struct
					{
						uint32_t typeIndex;	// refers to a type index in the IPI stream
					} S_BUILDINFO;

					struct
					{
						uint32_t parent; // pointer to the inliner
						uint32_t end; // pointer to this block's end
						uint32_t inlinee; // CV_ItemId of inlinee
						PDB_FLEXIBLE_ARRAY_MEMBER(uint8_t, binaryAnnotations);
					} S_INLINESITE;

					// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L4199
					struct
					{
						uint32_t typeIndex; // type index
						uint32_t moduleFilenameOffset; // index of mod filename in stringtable

						struct 
						{
							uint16_t fIsParam : 1; 			// variable is a parameter
							uint16_t fAddrTaken : 1; 		// address is taken
							uint16_t fCompGenx : 1; 		// variable is compiler generated
							uint16_t fIsAggregate : 1;		// the symbol is splitted in temporaries,
															// which are treated by compiler as 
															// independent entities
							uint16_t fIsAggregated : 1;		// Counterpart of fIsAggregate - tells
															// that it is a part of a fIsAggregate symbol
							uint16_t fIsAliased : 1;		// variable has multiple simultaneous lifetimes
							uint16_t fIsAlias : 1; 			// represents one of the multiple simultaneous lifetimes
							uint16_t fIsRetValue : 1;		// represents a function return value
							uint16_t fIsOptimizedOut : 1;	// variable has no lifetimes
							uint16_t fIsEnregGlob : 1; 		// variable is an enregistered global
							uint16_t fIsEnregStat : 1; 		// variable is an enregistered static
							uint16_t unused : 5; 			// must be zero
						} flags;

						PDB_FLEXIBLE_ARRAY_MEMBER(char, name);
					} S_FILESTATIC;

					struct
					{
						CompileSymbolFlags flags;
						CPUType machine;
						uint16_t versionFrontendMajor;
						uint16_t versionFrontendMinor;
						uint16_t versionFrontendBuild;
						uint16_t versionFrontendQFE;
						uint16_t versionBackendMajor;
						uint16_t versionBackendMinor;
						uint16_t versionBackendBuild;
						uint16_t versionBackendQFE;
						PDB_FLEXIBLE_ARRAY_MEMBER(char, version);
					} S_COMPILE3;

					// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L3372
					struct
					{
						uint8_t flags;
						PDB_FLEXIBLE_ARRAY_MEMBER(char, strings);
					} S_ENVBLOCK;

					// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L4190
					struct
					{
						uint32_t typeIndex;

						struct 
						{
							uint16_t fIsParam : 1; 			// variable is a parameter
							uint16_t fAddrTaken : 1; 		// address is taken
							uint16_t fCompGenx : 1; 		// variable is compiler generated
							uint16_t fIsAggregate : 1;		// the symbol is splitted in temporaries,
															// which are treated by compiler as 
															// independent entities
							uint16_t fIsAggregated : 1;		// Counterpart of fIsAggregate - tells
															// that it is a part of a fIsAggregate symbol
							uint16_t fIsAliased : 1;		// variable has multiple simultaneous lifetimes
							uint16_t fIsAlias : 1; 			// represents one of the multiple simultaneous lifetimes
							uint16_t fIsRetValue : 1;		// represents a function return value
							uint16_t fIsOptimizedOut : 1;	// variable has no lifetimes
							uint16_t fIsEnregGlob : 1; 		// variable is an enregistered global
							uint16_t fIsEnregStat : 1; 		// variable is an enregistered static
							uint16_t unused : 5; 			// must be zero
						} flags;

						PDB_FLEXIBLE_ARRAY_MEMBER(char, name);
					} S_LOCAL;

					// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L4236
					struct
					{
						uint16_t reg; // Register to hold the value of the symbol

						struct
						{
							uint16_t maybe : 1;		// May have no user name on one of control flow path.
							uint16_t padding : 15;	// Padding for future use.
						} attribute; // Attribute of the register range.

						LocalVariableAddressRange range; // Range of addresses where this program is valid
						PDB_FLEXIBLE_ARRAY_MEMBER(LocalVariableAddressGap, gaps); // The value is not available in following gaps.
					} S_DEFRANGE_REGISTER;

					// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L4245
					struct
					{
						uint32_t offsetFramePointer;
						LocalVariableAddressRange range; // Range of addresses where this program is valid
						PDB_FLEXIBLE_ARRAY_MEMBER(LocalVariableAddressGap, gaps); // The value is not available in following gaps.
					} S_DEFRANGE_FRAMEPOINTER_REL;

					// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L4265
					struct
					{
						uint16_t reg; // Register to hold the value of the symbol

						struct
						{
							uint16_t maybe : 1;		// May have no user name on one of control flow path.
							uint16_t padding : 15;	// Padding for future use.
						} attribute; // Attribute of the register range.

						uint32_t offsetParent : 12; // Offset in parent variable.
						uint32_t padding : 20; // Padding for future use.
						LocalVariableAddressRange range; // Range of addresses where this program is valid
						PDB_FLEXIBLE_ARRAY_MEMBER(LocalVariableAddressGap, gaps); // The value is not available in following gaps.
					} S_DEFRANGE_SUBFIELD_REGISTER;

					// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L4255
					struct
					{
						uint32_t offsetFramePointer;  // offset to frame pointer
					} S_DEFRANGE_FRAMEPOINTER_REL_FULL_SCOPE;

					// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L4279
					struct
					{
						uint16_t  baseRegister; // Register to hold the base pointer of the symbol
						uint16_t  spilledUDTMember : 1; // Spilled member for s.i.
						uint16_t  padding : 3; // Padding for future use.
						uint16_t  offsetParent : 12; // Offset in parent variable.
						uint32_t  offsetBasePointer; // offset to register
						LocalVariableAddressRange range;   // Range of addresses where this program is valid
						PDB_FLEXIBLE_ARRAY_MEMBER(LocalVariableAddressGap, gaps); // The value is not available in following gaps.
					} S_DEFRANGE_REGISTER_REL;

					// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L4500
					struct 
					{
						uint32_t offset;			// offset of call site
						uint16_t section;			// section index of call site
						uint16_t instructionLength; // length of heap allocation call instruction
						uint32_t typeIndex;			// type index describing function signature
					} S_HEAPALLOCSITE;

					// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L4382
					struct
					{
						uint32_t count; // Number of functions
						PDB_FLEXIBLE_ARRAY_MEMBER(uint32_t, funcs); // List of functions, dim == count
						// uint32_t   invocations[CV_ZEROLEN]; Followed by a parallel array of
						// invocation counts. Counts > reclen are assumed to be zero
					} S_CALLERS, S_CALLEES, S_INLINEES;

					struct
					{
						uint32_t typeIndex;
						PDB_FLEXIBLE_ARRAY_MEMBER(char, name);
					} S_UDT, S_UDT_ST;

					struct
					{
						uint32_t unknown1;
						uint32_t typeIndex;
						uint32_t unknown2;
						Register reg;
						PDB_FLEXIBLE_ARRAY_MEMBER(char, name);

					} S_REGREL32_INDIR;
#pragma pack(pop)
				} data;
			};

			// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L4576
			enum class PDB_NO_DISCARD DebugSubsectionKind : uint32_t 
			{
				S_IGNORE = 0x80000000,    // if this bit is set in a subsection type then ignore the subsection contents

				S_SYMBOLS = 0xF1,
				S_LINES = 0xF2,
				S_STRINGTABLE = 0xF3,
				S_FILECHECKSUMS = 0xF4,
				S_FRAMEDATA = 0xF5,
				S_INLINEELINES = 0xF6,
				S_CROSSSCOPEIMPORTS = 0xF7,
				S_CROSSSCOPEEXPORTS = 0xF8,

				S_IL_LINES = 0xF9,
				S_FUNC_MDTOKEN_MAP = 0xFA,
				S_TYPE_MDTOKEN_MAP = 0xFB,
				S_MERGED_ASSEMBLYINPUT = 0xFC,

				S_COFF_SYMBOL_RVA = 0xFD,
			};

			// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L4596
			struct DebugSubsectionHeader
			{
				DebugSubsectionKind kind;
				uint32_t size;
			};

			// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L4617
			struct Line
			{
				uint32_t offset;             // Offset to start of code bytes for line number
				uint32_t linenumStart : 24;  // line where statement/expression starts
				uint32_t deltaLineEnd : 7;   // delta to line where statement ends (optional)
				uint32_t fStatement : 1;     // true if a statement linenumber, else an expression line num
			};

			// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L4630
			struct Column
			{
				uint16_t start;
				uint16_t end;
			};

			// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L4601
			struct LinesHeader 
			{
				uint32_t sectionOffset;
				uint16_t sectionIndex;
				struct
				{
					uint16_t fHasColumns : 1;
					uint16_t pad : 15;
				} flags;

				uint32_t  codeSize;
			};

			// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L4608
			struct LinesFileBlockHeader
			{
				uint32_t fileChecksumOffset;
				uint32_t numLines;
				uint32_t size;
				// Line lines[numLines];
				// Column columns[numLines]; Might not be present
			};

			// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvconst.h#L88
			enum class PDB_NO_DISCARD ChecksumKind : uint8_t
			{
				None = 0,
				MD5 = 1,
				SHA1 = 2,
				SHA256 = 3,
			};

			// https://github.com/microsoft/microsoft-pdb/blob/master/cvdump/dumpsym7.cpp#L1097
			struct FileChecksumHeader
			{
				uint32_t filenameOffset;
				uint8_t  checksumSize;
				ChecksumKind checksumKind;
				PDB_FLEXIBLE_ARRAY_MEMBER(uint8_t, checksum);
			};

			// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L4822
			enum class InlineeSourceLineKind : uint32_t
			{
				Signature = 0,
				SignatureEx = 1,
			};

			struct InlineeSourceLineHeader
			{
				InlineeSourceLineKind kind;
			};

			// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L4825
			struct InlineeSourceLine
			{
				uint32_t inlinee;
				uint32_t fileChecksumOffset;
				uint32_t lineNumber;
			};

			struct InlineeSourceLineEx
			{
				uint32_t inlinee;
				uint32_t fileChecksumOffset;
				uint32_t lineNumber;
				uint32_t extraLines;
				PDB_FLEXIBLE_ARRAY_MEMBER(uint32_t, extrafileChecksumOffsets);
			};

			// Combine DebugSubsectionHeader and first subsection header into one struct.
			struct LineSection
			{
				DebugSubsectionHeader header;
				union
				{
					LinesHeader linesHeader;
					FileChecksumHeader checksumHeader;
					InlineeSourceLineHeader inlineeHeader;
				};
			};
		}
	}
}
