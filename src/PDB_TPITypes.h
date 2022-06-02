#pragma once

#include "Foundation/PDB_Macros.h"
#include "Foundation/PDB_BitOperators.h"
#include "Foundation/PDB_DisableWarningsPush.h"
#include <cstdint>
#include "Foundation/PDB_DisableWarningsPop.h"

namespace PDB
{
	namespace TPI
	{
		// https://llvm.org/docs/PDB/TpiStream.html#stream-header
		struct StreamHeader
		{
			enum class PDB_NO_DISCARD Version : uint32_t
			{
				V40 = 19950410u,
				V41 = 19951122u,
				V50 = 19961031u,
				V70 = 19990903u,
				V80 = 20040203u
			};

			Version version;
			uint32_t headerSize;
			uint32_t typeIndexBegin;
			uint32_t typeIndexEnd;
			uint32_t typeRecordBytes;

			uint16_t hashStreamIndex;
			uint16_t hashAuxStreamIndex;
			uint32_t hashKeySize;
			uint32_t numHashBuckets;

			int32_t hashValueBufferOffset;
			uint32_t hashValueBufferLength;

			int32_t indexOffsetBufferOffset;
			uint32_t indexOffsetBufferLength;

			int32_t hashAdjBufferOffset;
			uint32_t hashAdjBufferLength;
		};
	}


	namespace CodeView
	{
		namespace TPI
		{
			// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L772
			enum class PDB_NO_DISCARD TypeRecordKind : uint16_t
			{
				LF_POINTER = 0x1002u,
				LF_MODIFIER = 0x1001u,
				LF_PROCEDURE = 0x1008u,
				LF_MFUNCTION = 0x1009u,
				LF_LABEL = 0x000eu,
				LF_ARGLIST = 0x1201u,
				LF_FIELDLIST = 0x1203u,
				LF_VTSHAPE = 0x000au,
				LF_BITFIELD = 0x1205u,
				LF_METHODLIST = 0x1206u,
				LF_ENDPRECOMP = 0x0014u,

				LF_BCLASS = 0x001400u,
				LF_VBCLASS = 0x001401u,
				LF_IVBCLASS = 0x001402u,
				LF_FRIENDFCN_ST = 0x001403u,
				LF_INDEX = 0x001404u,
				LF_MEMBER_ST = 0x001405u,
				LF_STMEMBER_ST = 0x001406u,
				LF_METHOD_ST = 0x001407u,
				LF_NESTTYPE_ST = 0x001408u,
				LF_VFUNCTAB = 0x001409u,
				LF_FRIENDCLS = 0x00140Au,
				LF_ONEMETHOD_ST = 0x00140Bu,
				LF_VFUNCOFF = 0x00140Cu,
				LF_NESTTYPEEX_ST = 0x00140Du,
				LF_MEMBERMODIFY_ST = 0x00140Eu,
				LF_MANAGED_ST = 0x00140Fu,

				LF_SMAX = 0x001500u,
				LF_TYPESERVER = 0x001501u,
				LF_ENUMERATE = 0x001502u,
				LF_ARRAY = 0x001503u,
				LF_CLASS = 0x001504u,
				LF_STRUCTURE = 0x001505u,
				LF_UNION = 0x001506u,
				LF_ENUM = 0x001507u,
				LF_DIMARRAY = 0x001508u,
				LF_PRECOMP = 0x001509u,
				LF_ALIAS = 0x00150Au,
				LF_DEFARG = 0x00150Bu,
				LF_FRIENDFCN = 0x00150Cu,
				LF_MEMBER = 0x00150Du,
				LF_STMEMBER = 0x00150Eu,
				LF_METHOD = 0x00150Fu,
				LF_NESTTYPE = 0x001510u,
				LF_ONEMETHOD = 0x001511u,
				LF_NESTTYPEEX = 0x001512u,
				LF_MEMBERMODIFY = 0x001513u,
				LF_MANAGED = 0x001514u,
				LF_TYPESERVER2 = 0x001515u,

				LF_NUMERIC = 0x8000u,
				LF_CHAR = 0x8000u,
				LF_SHORT = 0x8001u,
				LF_USHORT = 0x8002u,
				LF_LONG = 0x8003u,
				LF_ULONG = 0x8004u,
				LF_REAL32 = 0x8005u,
				LF_REAL64 = 0x8006u,
				LF_REAL80 = 0x8007u,
				LF_REAL128 = 0x8008u,
				LF_QUADWORD = 0x8009u,
				LF_UQUADWORD = 0x800au,
				LF_REAL48 = 0x800bu,
				LF_COMPLEX32 = 0x800cu,
				LF_COMPLEX64 = 0x800du,
				LF_COMPLEX80 = 0x800eu,
				LF_COMPLEX128 = 0x800fu,
				LF_VARSTRING = 0x8010u,

				LF_OCTWORD = 0x8017u,
				LF_UOCTWORD = 0x8018u,

				LF_DECIMAL = 0x8019u,
				LF_DATE = 0x801au,
				LF_UTF8STRING = 0x801bu,

				LF_REAL16 = 0x801cu
			};

			// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L328
			enum class TypeIndexKind : uint16_t
			{
				NOTYPE = 0x0000u,			// uncharacterized type (no type)
				ABS = 0x0001u,				// absolute symbol
				SEGMENT = 0x0002u,			// segment type
				_VOID = 0x0003u,			// void
				HRESULT = 0x0008u,			// OLE/COM HRESULT
				_32PHRESULT = 0x0408u,		// OLE/COM HRESULT __ptr32 *
				_64PHRESULT = 0x0608u,		// OLE/COM HRESULT __ptr64 *

				PVOID = 0x0103u,			// near pointer to void
				PFVOID = 0x0203u,			// far pointer to void
				PHVOID = 0x0303u,			// huge pointer to void
				_32PVOID = 0x0403u,			// 32 bit pointer to void
				_32PFVOID = 0x0503u,		// 16:32 pointer to void
				_64PVOID = 0x0603u,			// 64 bit pointer to void
				CURRENCY = 0x0004u,			// BASIC 8 byte currency value
				NBASICSTR = 0x0005u,		// Near BASIC string
				FBASICSTR = 0x0006u,		// Far BASIC string
				NOTTRANS = 0x0007u,			// type not translated by cvpack
				BIT = 0x0060u,				// bit
				PASCHAR = 0x0061u,			// Pascal CHAR
				BOOL32FF = 0x0062u,			// 32-bit BOOL where true is 0xffffffff

				CHAR = 0x0010u,				// 8 bit signed
				PCHAR = 0x0110u,			// 16 bit pointer to 8 bit signed
				PFCHAR = 0x0210u,			// 16:16 far pointer to 8 bit signed
				PHCHAR = 0x0310u,			// 16:16 huge pointer to 8 bit signed
				_32PCHAR = 0x0410u,			// 32 bit pointer to 8 bit signed
				_32PFCHAR = 0x0510u,		// 16:32 pointer to 8 bit signed
				_64PCHAR = 0x0610u,			// 64 bit pointer to 8 bit signed

				UCHAR = 0x0020u,			// 8 bit unsigned
				PUCHAR = 0x0120u,			// 16 bit pointer to 8 bit unsigned
				PFUCHAR = 0x0220u,			// 16:16 far pointer to 8 bit unsigned
				PHUCHAR = 0x0320u,			// 16:16 huge pointer to 8 bit unsigned
				_32PUCHAR = 0x0420u,		// 32 bit pointer to 8 bit unsigned
				_32PFUCHAR = 0x0520u,		// 16:32 pointer to 8 bit unsigned
				_64PUCHAR = 0x0620u,		// 64 bit pointer to 8 bit unsigned

				RCHAR = 0x0070u,			// really a char
				PRCHAR = 0x0170u,			// 16 bit pointer to a real char
				PFRCHAR = 0x0270u,			// 16:16 far pointer to a real char
				PHRCHAR = 0x0370u,			// 16:16 huge pointer to a real char
				_32PRCHAR = 0x0470u,		// 32 bit pointer to a real char
				_32PFRCHAR = 0x0570u,		// 16:32 pointer to a real char
				_64PRCHAR = 0x0670u,		// 64 bit pointer to a real char

				// wide character types
				WCHAR = 0x0071u,			// wide char
				PWCHAR = 0x0171u,			// 16 bit pointer to a wide char
				PFWCHAR = 0x0271u,			// 16:16 far pointer to a wide char
				PHWCHAR = 0x0371u,			// 16:16 huge pointer to a wide char
				_32PWCHAR = 0x0471u,		// 32 bit pointer to a wide char
				_32PFWCHAR = 0x0571u,		// 16:32 pointer to a wide char
				_64PWCHAR = 0x0671u,		// 64 bit pointer to a wide char

				// 16-bit unicode char
				CHAR16 = 0x007au,			// 16-bit unicode char
				PCHAR16 = 0x017au,			// 16 bit pointer to a 16-bit unicode char
				PFCHAR16 = 0x027au,			// 16:16 far pointer to a 16-bit unicode char
				PHCHAR16 = 0x037au,			// 16:16 huge pointer to a 16-bit unicode char
				_32PCHAR16 = 0x047au,		// 32 bit pointer to a 16-bit unicode char
				_32PFCHAR16 = 0x057au,		// 16:32 pointer to a 16-bit unicode char
				_64PCHAR16 = 0x067au,		// 64 bit pointer to a 16-bit unicode char

				// 32-bit unicode char
				CHAR32 = 0x007bu,			// 32-bit unicode char
				PCHAR32 = 0x017bu,			// 16 bit pointer to a 32-bit unicode char
				PFCHAR32 = 0x027bu,			// 16:16 far pointer to a 32-bit unicode char
				PHCHAR32 = 0x037bu,			// 16:16 huge pointer to a 32-bit unicode char
				_32PCHAR32 = 0x047bu,		// 32 bit pointer to a 32-bit unicode char
				_32PFCHAR32 = 0x057bu,		// 16:32 pointer to a 32-bit unicode char
				_64PCHAR32 = 0x067bu,		// 64 bit pointer to a 32-bit unicode char

				// 8 bit int types
				INT1 = 0x0068u,				// 8 bit signed int
				PINT1 = 0x0168u,			// 16 bit pointer to 8 bit signed int
				PFINT1 = 0x0268u,			// 16:16 far pointer to 8 bit signed int
				PHINT1 = 0x0368u,			// 16:16 huge pointer to 8 bit signed int
				_32PINT1 = 0x0468u,			// 32 bit pointer to 8 bit signed int
				_32PFINT1 = 0x0568u,		// 16:32 pointer to 8 bit signed int
				_64PINT1 = 0x0668u,			// 64 bit pointer to 8 bit signed int

				_UINT1 = 0x0069u,			// 8 bit unsigned int
				_PUINT1 = 0x0169u,			// 16 bit pointer to 8 bit unsigned int
				_PFUINT1 = 0x0269u,			// 16:16 far pointer to 8 bit unsigned int
				_PHUINT1 = 0x0369u,			// 16:16 huge pointer to 8 bit unsigned int
				_32PUINT1 = 0x0469u,		// 32 bit pointer to 8 bit unsigned int
				_32PFUINT1 = 0x0569u,		// 16:32 pointer to 8 bit unsigned int
				_64PUINT1 = 0x0669u,		// 64 bit pointer to 8 bit unsigned int

				// 16 bit short types
				SHORT = 0x0011u,			// 16 bit signed
				PSHORT = 0x0111u,			// 16 bit pointer to 16 bit signed
				PFSHORT = 0x0211u,			// 16:16 far pointer to 16 bit signed
				PHSHORT = 0x0311u,			// 16:16 huge pointer to 16 bit signed
				_32PSHORT = 0x0411u,		// 32 bit pointer to 16 bit signed
				_32PFSHORT = 0x0511u,		// 16:32 pointer to 16 bit signed
				_64PSHORT = 0x0611u,		// 64 bit pointer to 16 bit signed

				USHORT = 0x0021u,
				PUSHORT = 0x0121u,
				PFUSHORT = 0x0221u,
				PHUSHORT = 0x0321u,
				_32PUSHORT = 0x0421u,
				_32PFUSHORT = 0x0521u,
				_64PUSHORT = 0x0621u,

				INT2 = 0x0072u,
				PINT2 = 0x0172u,
				PFINT2 = 0x0272u,
				PHINT2 = 0x0372u,
				_32PINT2 = 0x0472u,
				_32PFINT2 = 0x0572u,
				_64PINT2 = 0x0672u,

				UINT2 = 0x0073u,
				PUINT2 = 0x0173u,
				PFUINT2 = 0x0273u,
				PHUINT2 = 0x0373u,
				_32PUINT2 = 0x0473u,
				_32PFUINT2 = 0x0573u,
				_64PUINT2 = 0x0673u,

				LONG = 0x0012u,
				PLONG = 0x0112u,
				PFLONG = 0x0212u,
				PHLONG = 0x0312u,
				_32PLONG = 0x0412u,
				_32PFLONG = 0x0512u,
				_64PLONG = 0x0612u,

				ULONG = 0x0022u,
				PULONG = 0x0122u,
				PFULONG = 0x0222u,
				PHULONG = 0x0322u,
				_32PULONG = 0x0422u,
				_32PFULONG = 0x0522u,
				_64PULONG = 0x0622u,

				INT4 = 0x0074u,
				PINT4 = 0x0174u,
				PFINT4 = 0x0274u,
				PHINT4 = 0x0374u,
				_32PINT4 = 0x0474u,
				_32PFINT4 = 0x0574u,
				_64PINT4 = 0x0674u,

				UINT4 = 0x0075u,
				PUINT4 = 0x0175u,
				PFUINT4 = 0x0275u,
				PHUINT4 = 0x0375u,
				_32PUINT4 = 0x0475u,
				_32PFUINT4 = 0x0575u,
				_64PUINT4 = 0x0675u,

				QUAD = 0x0013u,
				PQUAD = 0x0113u,
				PFQUAD = 0x0213u,
				PHQUAD = 0x0313u,
				_32PQUAD = 0x0413u,
				_32PFQUAD = 0x0513u,
				_64PQUAD = 0x0613u,

				UQUAD = 0x0023u,
				PUQUAD = 0x0123u,
				PFUQUAD = 0x0223u,
				PHUQUAD = 0x0323u,
				_32PUQUAD = 0x0423u,
				_32PFUQUAD = 0x0523u,
				_64PUQUAD = 0x0623u,

				INT8 = 0x0076u,
				PINT8 = 0x0176u,
				PFINT8 = 0x0276u,
				PHINT8 = 0x0376u,
				_32PINT8 = 0x0476u,
				_32PFINT8 = 0x0576u,
				_64PINT8 = 0x0676u,

				UINT8 = 0x0077u,
				PUINT8 = 0x0177u,
				PFUINT8 = 0x0277u,
				PHUINT8 = 0x0377u,
				_32PUINT8 = 0x0477u,
				_32PFUINT8 = 0x0577u,
				_64PUINT8 = 0x0677u,

				OCT = 0x0014u,
				POCT = 0x0114u,
				PFOCT = 0x0214u,
				PHOCT = 0x0314u,
				_32POCT = 0x0414u,
				_32PFOCT = 0x0514u,
				_64POCT = 0x0614u,

				UOCT = 0x0024u,
				PUOCT = 0x0124u,
				PFUOCT = 0x0224u,
				PHUOCT = 0x0324u,
				_32PUOCT = 0x0424u,
				_32PFUOCT = 0x0524u,
				_64PUOCT = 0x0624u,

				INT16 = 0x0078u,
				PINT16 = 0x0178u,
				PFINT16 = 0x0278u,
				PHINT16 = 0x0378u,
				_32PINT16 = 0x0478u,
				_32PFINT16 = 0x0578u,
				_64PINT16 = 0x0678u,

				UINT16 = 0x0079u,
				PUINT16 = 0x0179u,
				PFUINT16 = 0x0279u,
				PHUINT16 = 0x0379u,
				_32PUINT16 = 0x0479u,
				_32PFUINT16 = 0x0579u,
				_64PUINT16 = 0x0679u,

				REAL32 = 0x0040u,
				PREAL32 = 0x0140u,
				PFREAL32 = 0x0240u,
				PHREAL32 = 0x0340u,
				_32PREAL32 = 0x0440u,
				_32PFREAL32 = 0x0540u,
				_64PREAL32 = 0x0640u,

				REAL48 = 0x0044u,
				PREAL48 = 0x0144u,
				PFREAL48 = 0x0244u,
				PHREAL48 = 0x0344u,
				_32PREAL48 = 0x0444u,
				_32PFREAL48 = 0x0544u,
				_64PREAL48 = 0x0644u,

				REAL64 = 0x0041u,
				PREAL64 = 0x0141u,
				PFREAL64 = 0x0241u,
				PHREAL64 = 0x0341u,
				_32PREAL64 = 0x0441u,
				_32PFREAL64 = 0x0541u,
				_64PREAL64 = 0x0641u,

				REAL80 = 0x0042u,
				PREAL80 = 0x0142u,
				PFREAL80 = 0x0242u,
				PHREAL80 = 0x0342u,
				_32PREAL80 = 0x0442u,
				_32PFREAL80 = 0x0542u,
				_64PREAL80 = 0x0642u,

				REAL128 = 0x0043u,
				PREAL128 = 0x0143u,
				PFREAL128 = 0x0243u,
				PHREAL128 = 0x0343u,
				_32PREAL128 = 0x0443u,
				_32PFREAL128 = 0x0543u,
				_64PREAL128 = 0x0643u,

				CPLX32 = 0x0050u,
				PCPLX32 = 0x0150u,
				PFCPLX32 = 0x0250u,
				PHCPLX32 = 0x0350u,
				_32PCPLX32 = 0x0450u,
				_32PFCPLX32 = 0x0550u,
				_64PCPLX32 = 0x0650u,

				CPLX64 = 0x0051u,
				PCPLX64 = 0x0151u,
				PFCPLX64 = 0x0251u,
				PHCPLX64 = 0x0351u,
				_32PCPLX64 = 0x0451u,
				_32PFCPLX64 = 0x0551u,
				_64PCPLX64 = 0x0651u,

				CPLX80 = 0x0052u,
				PCPLX80 = 0x0152u,
				PFCPLX80 = 0x0252u,
				PHCPLX80 = 0x0352u,
				_32PCPLX80 = 0x0452u,
				_32PFCPLX80 = 0x0552u,
				_64PCPLX80 = 0x0652u,

				CPLX128 = 0x0053u,
				PCPLX128 = 0x0153u,
				PFCPLX128 = 0x0253u,
				PHCPLX128 = 0x0353u,
				_32PCPLX128 = 0x0453u,
				_32PFCPLX128 = 0x0553u,
				_64PCPLX128 = 0x0653u,

				BOOL08 = 0x0030u,
				PBOOL08 = 0x0130u,
				PFBOOL08 = 0x0230u,
				PHBOOL08 = 0x0330u,
				_32PBOOL08 = 0x0430u,
				_32PFBOOL08 = 0x0530u,
				_64PBOOL08 = 0x0630u,

				BOOL16 = 0x0031u,
				PBOOL16 = 0x0131u,
				PFBOOL16 = 0x0231u,
				PHBOOL16 = 0x0331u,
				_32PBOOL16 = 0x0431u,
				_32PFBOOL16 = 0x0531u,
				_64PBOOL16 = 0x0631u,

				BOOL32 = 0x0032u,
				PBOOL32 = 0x0132u,
				PFBOOL32 = 0x0232u,
				PHBOOL32 = 0x0332u,
				_32PBOOL32 = 0x0432u,
				_32PFBOOL32 = 0x0532u,
				_64PBOOL32 = 0x0632u,

				BOOL64 = 0x0033u,
				PBOOL64 = 0x0133u,
				PFBOOL64 = 0x0233u,
				PHBOOL64 = 0x0333u,
				_32PBOOL64 = 0x0433u,
				_32PFBOOL64 = 0x0533u,
				_64PBOOL64 = 0x0633u,

				NCVPTR = 0x01F0u,
				FCVPTR = 0x02F0u,
				HCVPTR = 0x03F0u,
				_32NCVPTR = 0x04F0u,
				_32FCVPTR = 0x05F0u,
				_64NCVPTR = 0x06F0u,
			};

			// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvconst.h#L31
			enum class CallingConvention : uint8_t
			{
				NEAR_C = 0x00u,		// near right to left pushu, caller pops stack
				FAR_C = 0x01u,		// far right to left pushu, caller pops stack
				NEAR_PASCAL = 0x02u,// near left to right pushu, callee pops stack
				FAR_PASCAL = 0x03u, // far left to right pushu, callee pops stack
				NEAR_FAST = 0x04u,	// near left to right push with regsu, callee pops stack
				FAR_FAST = 0x05u,	// far left to right push with regsu, callee pops stack
				SKIPPED = 0x06u,	// skipped (unused) call index
				NEAR_STD = 0x07u,	// near standard call
				FAR_STD = 0x08u,	// far standard call
				NEAR_SYS = 0x09u,	// near sys call
				FAR_SYS = 0x0au,	// far sys call
				THISCALL = 0x0bu,	// this call (this passed in register)
				MIPSCALL = 0x0cu,	// Mips call
				GENERIC = 0x0du,	// Generic call sequence
				ALPHACALL = 0x0eu,	// Alpha call
				PPCCALL = 0x0fu,	// PPC call
				SHCALL = 0x10u,		// Hitachi SuperH call
				ARMCALL = 0x11u,	// ARM call
				AM33CALL = 0x12u,	// AM33 call
				TRICALL = 0x13u,	// TriCore Call
				SH5CALL = 0x14u,	// Hitachi SuperH-5 call
				M32RCALL = 0x15u,	// M32R Call
				CLRCALL = 0x16u,	// clr call
				INLINE = 0x17u,		// Marker for routines always inlined and thus lacking a convention
				NEAR_VECTOR = 0x18u,// near left to right push with regsu, callee pops stack
				RESERVED = 0x19u	// first unused call enumeration

				// Do NOT add any more machine specific conventions.  This is to be used for
				// calling conventions in the source only (e.g. __cdeclu, __stdcall).
			};

			// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L1049
			enum class MethodProperty : uint8_t
			{
				Vanilla = 0x00u,
				Virtual = 0x01u,
				Static = 0x02u,
				Friend = 0x03u,
				Intro = 0x04u,
				PureVirt = 0x05u,
				PureIntro = 0x06u
			};

			// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L1120
			struct TypePropery
			{
				uint16_t packed : 1;			// true if structure is packed
				uint16_t ctor : 1;				// true if constructors or destructors present
				uint16_t ovlops : 1;			// true if overloaded operators present
				uint16_t isnested : 1;			// true if this is a nested class
				uint16_t cnested : 1;			// true if this class contains nested types
				uint16_t opassign : 1;			// true if overloaded assignment (=)
				uint16_t opcast : 1;			// true if casting methods
				uint16_t fwdref : 1;			// true if forward reference (incomplete defn)
				uint16_t scoped : 1;			// scoped definition
				uint16_t hasuniquename : 1;		// true if there is a decorated name following the regular name
				uint16_t sealed : 1;			// true if class cannot be used as a base class
				uint16_t hfa : 2;				// CV_HFA_e
				uint16_t intrinsic : 1;			// true if class is an intrinsic type (e.g. __m128d)
				uint16_t mocom : 2;				// CV_MOCOM_UDe
			};

			// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L1142
			struct MemberAttributes
			{
				uint16_t access : 2;			// access protection CV_access_t
				uint16_t mprop : 3;				// method properties CV_methodprop_t
				uint16_t pseudo : 1;			// compiler generated fcn and does not exist
				uint16_t noinherit : 1;			// true if class cannot be inherited
				uint16_t noconstruct : 1;		// true if class cannot be constructed
				uint16_t compgenx : 1;			// compiler generated fcn and does exist
				uint16_t sealed : 1;			// true if method cannot be overridden
				uint16_t unused : 6;			// unused
			};

			// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L1156
			struct FunctionAttributes
			{
				uint8_t cxxreturnudt : 1;		// true if C++ style ReturnUDT
				uint8_t ctor : 1;				// true if func is an instance constructor
				uint8_t ctorvbase : 1;			// true if func is an instance constructor of a class with virtual bases
				uint8_t unused : 5;				// unused
			};

			struct RecordHeader
			{
				uint16_t size;					// record length, not including this 2-byte field
				TypeRecordKind kind;			// record kind
			};

			struct LeafEasy
			{
				TypeRecordKind kind;			// record kind
			};

			struct FieldList
			{
				TypeRecordKind kind;			// record kind
				union Data
				{
#pragma pack(push, 1)
					// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L2499
					struct
					{
						MemberAttributes attributes;					// method attribute
						uint32_t		index;							// type index of base class
						union
						{
							PDB_FLEXIBLE_ARRAY_MEMBER(char, offset);	// variable length offset of base within class
							LeafEasy lfEasy;
						};
					}LF_BCLASS;

					// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L2615
					struct
					{
						uint16_t		pad0;   // internal padding, must be 0.
						uint32_t        type;   // type index of pointer
					}LF_VFUNCTAB;

					// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L2683
					struct
					{
						MemberAttributes attributes;
						union
						{
							PDB_FLEXIBLE_ARRAY_MEMBER(char, value);
							LeafEasy lfEasy;
						};
					} LF_ENUMERATE;

					// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L2693
					struct
					{
						uint16_t		pad0;	// internal padding, must be 0
						uint32_t		index;	// index of nested type definition
						PDB_FLEXIBLE_ARRAY_MEMBER(char, name);
					}LF_NESTTYPE;

					// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L2650
					struct
					{
						uint16_t		count;	// number of occurrences of function
						uint32_t        mList;  // index to LF_METHODLIST record
						PDB_FLEXIBLE_ARRAY_MEMBER(char, name);
					}LF_METHOD;

					// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L2671
					struct
					{
						MemberAttributes attributes;					// method attribute
						uint32_t index;									// index to type record for procedure
						PDB_FLEXIBLE_ARRAY_MEMBER(uint32_t, vbaseoff);	// offset in vfunctable if
					}LF_ONEMETHOD;

					// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L2580
					struct
					{
						MemberAttributes attributes;
						uint32_t index;			// type index of referenced leaf
						union
						{
							PDB_FLEXIBLE_ARRAY_MEMBER(char, offset);
							LeafEasy lfEasy;
						};
					} LF_MEMBER;

					// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L2592
					struct
					{
						MemberAttributes attributes;
						uint32_t index;			// index of type record for field
						PDB_FLEXIBLE_ARRAY_MEMBER(char, name);
					}LF_STMEMBER;
#pragma pack(pop)
				} data;
			};

			// all CodeView records are stored as a header, followed by variable-length data.
			// internal Record structs such as S_PUB32, S_GDATA32, etc. correspond to the data layout of a CodeView record of that kind.
			struct Record
			{
				RecordHeader header;
				union Data
				{
#pragma pack(push, 1)
					// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L2144
					struct
					{
						PDB_FLEXIBLE_ARRAY_MEMBER(uint32_t, mList);
					}LF_METHODLIST;

					// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L2131
					struct
					{
						MemberAttributes attributes;					// method attribute
						uint16_t		pad0;							// internal padding, must be 0
						uint32_t		index;							// index to type record for procedure
						PDB_FLEXIBLE_ARRAY_MEMBER(uint32_t, vbaseoff);	// offset in vfunctable if
					}METHOD;

					// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L1801
					struct
					{
						uint32_t        rvtype;         // type index of return value
						uint32_t        classtype;      // type index of containing class
						uint32_t        thistype;       // type index of this pointer (model specific)
						uint8_t			calltype;       // calling convention (call_t)
						FunctionAttributes funcattr;	// attributes
						uint16_t		parmcount;      // number of parameters
						uint32_t        arglist;        // type index of argument list
						int32_t         thisadjust;     // this adjuster (long because pad required anyway)
					}LF_MFUNCTION;

					// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L1460
					struct
					{
						uint32_t type;					// modified type

						// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L1090
						struct
						{
							uint16_t MOD_const : 1;
							uint16_t MOD_volatile : 1;
							uint16_t MOD_unaligned : 1;
							uint16_t MOD_unused : 13;
						} attr;							// modifier attribute modifier_t
					} LF_MODIFIER;

					// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L1508
					struct
					{
						uint32_t utype;					// type index of the underlying type
						struct PointerAttributes
						{
							uint32_t ptrtype : 5;		// ordinal specifying pointer type (CV_ptrtype_e)
							uint32_t ptrmode : 3;		// ordinal specifying pointer mode (CV_ptrmode_e)
							uint32_t isflat32 : 1;		// TRUE if 0:32 pointer
							uint32_t isvolatile : 1;	// TRUE if volatile pointer
							uint32_t isconst : 1;		// TRUE if const pointer
							uint32_t isunaligned : 1;	// TRUE if unaligned pointer
							uint32_t isrestrict : 1;	// TRUE if restricted pointer (allow agressive opts)
							uint32_t size : 6;			// size of pointer (in bytes)
							uint32_t ismocom : 1;		// TRUE if it is a MoCOM pointer (^ or %)
							uint32_t islref : 1;		// TRUE if it is this pointer of member function with & ref-qualifier
							uint32_t isrref : 1;		// TRUE if it is this pointer of member function with && ref-qualifier
							uint32_t unused : 10;		// pad out to 32-bits for following cv_typ_t's
						} attr;

						union
						{
							struct
							{
								uint32_t pmclass;						// index of containing class for pointer to member
								uint16_t pmenum;						// enumeration specifying pm format (CV_pmtype_e)
							} pm;

							uint16_t bseg;								// base segment if PTR_BASE_SEG
							PDB_FLEXIBLE_ARRAY_MEMBER(uint8_t, Sym);	// copy of base symbol record (including length)

							struct
							{
								uint32_t index;							// type index if CV_PTR_BASE_TYPE
								PDB_FLEXIBLE_ARRAY_MEMBER(char, name);	// name of base type
							} btype;
						} pbase;
					} LF_POINTER;

					// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L1775
					struct
					{
						uint32_t rvtype;				// type index of return value
						CallingConvention calltype;		// calling convention (CV_call_t)
						FunctionAttributes funcattr;	// attributes
						uint16_t parmcount;				// number of parameters
						uint32_t arglist;				// type index of argument list
					} LF_PROCEDURE;

					// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L2043
					struct
					{
						uint32_t count;					// number of arguments
						PDB_FLEXIBLE_ARRAY_MEMBER(uint32_t, arg);
					} LF_ARGLIST;

					// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L2164
					struct
					{
						uint32_t type;
						uint8_t length;
						uint8_t position;
						PDB_FLEXIBLE_ARRAY_MEMBER(char, data);
					} LF_BITFIELD;

					struct
					{
						uint32_t elemtype;							// type index of element type
						uint32_t idxtype;							// type index of indexing type
						PDB_FLEXIBLE_ARRAY_MEMBER(char, data);		// variable length data specifying size in bytes and name
					} LF_ARRAY;

					// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L1631
					struct
					{
						uint16_t count;			// count of number of elements in class
						TypePropery property;	// property attribute field
						uint32_t field;			// type index of LF_FIELD descriptor list
						uint32_t derived;		// type index of derived from list if not zero
						uint32_t vshape;		// type index of vshape table for this class
						union
						{
							PDB_FLEXIBLE_ARRAY_MEMBER(char, data);
							LeafEasy lfEasy;
						};
					} LF_CLASS;

					// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L1647
					struct
					{
						uint16_t count;			// count of number of elements in class
						TypePropery property;	// property attribute field
						uint32_t field;			// type index of LF_FIELD descriptor list
						PDB_FLEXIBLE_ARRAY_MEMBER(char, data);
					} LF_UNION;

					// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L1752
					struct
					{
						uint16_t count;			// count of number of elements in class
						TypePropery property;	// property attribute field
						uint32_t utype;			// underlying type of the enum
						uint32_t field;			// type index of LF_FIELD descriptor list
						PDB_FLEXIBLE_ARRAY_MEMBER(char, name);
					} LF_ENUM;

					// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L2112
					struct
					{
						FieldList list;
					} LF_FIELD;
#pragma pack(pop)
				} data;
			};
		}
	}
}
