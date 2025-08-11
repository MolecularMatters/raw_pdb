#pragma once

#include "Foundation/PDB_Macros.h"
#include "Foundation/PDB_BitOperators.h"

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
				LF_CLASS2 = 0x001608u,
				LF_STRUCTURE2 = 0x001609u,

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
			// https://github.com/ValveSoftware/wine/blob/cd165953c8b379a78418711f07417022e503c81b/include/wine/mscvpdb.h
			enum class TypeIndexKind : uint16_t
			{
				T_NOTYPE = 0x0000u,				// uncharacterized type (no type)
				T_ABS = 0x0001u,				// absolute symbol
				T_SEGMENT = 0x0002u,			// segment type
				T_VOID = 0x0003u,				// void
				T_HRESULT = 0x0008u,			// OLE/COM HRESULT
				T_32PHRESULT = 0x0408u,			// OLE/COM HRESULT __ptr32 *
				T_64PHRESULT = 0x0608u,			// OLE/COM HRESULT __ptr64 *

				// Emitted due to a compiler bug? 
				// 0x0600 bits appears to indicate a 64-bit pointer, but it has no type?
				// Seen as type index for C11 "_Atomic uint32_t*" variable and constant.
				T_UNKNOWN_0600 = 0x0600u,

				T_PVOID = 0x0103u,				// near pointer to void
				T_PFVOID = 0x0203u,				// far pointer to void
				T_PHVOID = 0x0303u,				// huge pointer to void
				T_32PVOID = 0x0403u,			// 32 bit pointer to void
				T_32PFVOID = 0x0503u,			// 16:32 pointer to void
				T_64PVOID = 0x0603u,			// 64 bit pointer to void
				T_CURRENCY = 0x0004u,			// BASIC 8 byte currency value
				T_NBASICSTR = 0x0005u,			// Near BASIC string
				T_FBASICSTR = 0x0006u,			// Far BASIC string
				T_NOTTRANS = 0x0007u,			// type not translated by cvpack
				T_BIT = 0x0060u,				// bit
				T_PASCHAR = 0x0061u,			// Pascal CHAR
				T_BOOL32FF = 0x0062u,			// 32-bit BOOL where true is 0xffffffff

				T_CHAR = 0x0010u,				// 8 bit signed
				T_PCHAR = 0x0110u,				// 16 bit pointer to 8 bit signed
				T_PFCHAR = 0x0210u,				// 16:16 far pointer to 8 bit signed
				T_PHCHAR = 0x0310u,				// 16:16 huge pointer to 8 bit signed
				T_32PCHAR = 0x0410u,			// 32 bit pointer to 8 bit signed
				T_32PFCHAR = 0x0510u,			// 16:32 pointer to 8 bit signed
				T_64PCHAR = 0x0610u,			// 64 bit pointer to 8 bit signed

				T_UCHAR = 0x0020u,				// 8 bit unsigned
				T_PUCHAR = 0x0120u,				// 16 bit pointer to 8 bit unsigned
				T_PFUCHAR = 0x0220u,			// 16:16 far pointer to 8 bit unsigned
				T_PHUCHAR = 0x0320u,			// 16:16 huge pointer to 8 bit unsigned
				T_32PUCHAR = 0x0420u,			// 32 bit pointer to 8 bit unsigned
				T_32PFUCHAR = 0x0520u,			// 16:32 pointer to 8 bit unsigned
				T_64PUCHAR = 0x0620u,			// 64 bit pointer to 8 bit unsigned

				T_RCHAR = 0x0070u,				// really a char
				T_PRCHAR = 0x0170u,				// 16 bit pointer to a real char
				T_PFRCHAR = 0x0270u,			// 16:16 far pointer to a real char
				T_PHRCHAR = 0x0370u,			// 16:16 huge pointer to a real char
				T_32PRCHAR = 0x0470u,			// 32 bit pointer to a real char
				T_32PFRCHAR = 0x0570u,			// 16:32 pointer to a real char
				T_64PRCHAR = 0x0670u,			// 64 bit pointer to a real char

				// wide character types
				T_WCHAR = 0x0071u,				// wide char
				T_PWCHAR = 0x0171u,				// 16 bit pointer to a wide char
				T_PFWCHAR = 0x0271u,			// 16:16 far pointer to a wide char
				T_PHWCHAR = 0x0371u,			// 16:16 huge pointer to a wide char
				T_32PWCHAR = 0x0471u,			// 32 bit pointer to a wide char
				T_32PFWCHAR = 0x0571u,			// 16:32 pointer to a wide char
				T_64PWCHAR = 0x0671u,			// 64 bit pointer to a wide char

				// 8-bit unicode char
				T_CHAR8 = 0x007c,				// 8-bit unicode char (C++ 20)
				T_PCHAR8 = 0x017c,				// Near pointer to 8-bit unicode char
				T_PFCHAR8 = 0x027c,				// Far pointer to 8-bit unicode char
				T_PHCHAR8 = 0x037c,				// Huge pointer to 8-bit unicode char
				T_32PCHAR8 = 0x047c,			// 16:32 near pointer to 8-bit unicode char
				T_32PFCHAR8 = 0x057c,			// 16:32 far pointer to 8-bit unicode char
				T_64PCHAR8 = 0x067c,			// 64 bit near pointer to 8-bit unicode char

				// 16-bit unicode char
				T_CHAR16 = 0x007au,				// 16-bit unicode char
				T_PCHAR16 = 0x017au,			// 16 bit pointer to a 16-bit unicode char
				T_PFCHAR16 = 0x027au,			// 16:16 far pointer to a 16-bit unicode char
				T_PHCHAR16 = 0x037au,			// 16:16 huge pointer to a 16-bit unicode char
				T_32PCHAR16 = 0x047au,			// 32 bit pointer to a 16-bit unicode char
				T_32PFCHAR16 = 0x057au,			// 16:32 pointer to a 16-bit unicode char
				T_64PCHAR16 = 0x067au,			// 64 bit pointer to a 16-bit unicode char

				// 32-bit unicode char
				T_CHAR32 = 0x007bu,				// 32-bit unicode char
				T_PCHAR32 = 0x017bu,			// 16 bit pointer to a 32-bit unicode char
				T_PFCHAR32 = 0x027bu,			// 16:16 far pointer to a 32-bit unicode char
				T_PHCHAR32 = 0x037bu,			// 16:16 huge pointer to a 32-bit unicode char
				T_32PCHAR32 = 0x047bu,			// 32 bit pointer to a 32-bit unicode char
				T_32PFCHAR32 = 0x057bu,			// 16:32 pointer to a 32-bit unicode char
				T_64PCHAR32 = 0x067bu,			// 64 bit pointer to a 32-bit unicode char

				// 8 bit int types
				T_INT1 = 0x0068u,				// 8 bit signed int
				T_PINT1 = 0x0168u,				// 16 bit pointer to 8 bit signed int
				T_PFINT1 = 0x0268u,				// 16:16 far pointer to 8 bit signed int
				T_PHINT1 = 0x0368u,				// 16:16 huge pointer to 8 bit signed int
				T_32PINT1 = 0x0468u,			// 32 bit pointer to 8 bit signed int
				T_32PFINT1 = 0x0568u,			// 16:32 pointer to 8 bit signed int
				T_64PINT1 = 0x0668u,			// 64 bit pointer to 8 bit signed int

				T_UINT1 = 0x0069u,				// 8 bit unsigned int
				T_PUINT1 = 0x0169u,				// 16 bit pointer to 8 bit unsigned int
				T_PFUINT1 = 0x0269u,			// 16:16 far pointer to 8 bit unsigned int
				T_PHUINT1 = 0x0369u,			// 16:16 huge pointer to 8 bit unsigned int
				T_32PUINT1 = 0x0469u,			// 32 bit pointer to 8 bit unsigned int
				T_32PFUINT1 = 0x0569u,			// 16:32 pointer to 8 bit unsigned int
				T_64PUINT1 = 0x0669u,			// 64 bit pointer to 8 bit unsigned int

				// 16 bit short types
				T_SHORT = 0x0011u,				// 16 bit signed
				T_PSHORT = 0x0111u,				// 16 bit pointer to 16 bit signed
				T_PFSHORT = 0x0211u,			// 16:16 far pointer to 16 bit signed
				T_PHSHORT = 0x0311u,			// 16:16 huge pointer to 16 bit signed
				T_32PSHORT = 0x0411u,			// 32 bit pointer to 16 bit signed
				T_32PFSHORT = 0x0511u,			// 16:32 pointer to 16 bit signed
				T_64PSHORT = 0x0611u,			// 64 bit pointer to 16 bit signed

				T_USHORT = 0x0021u,
				T_PUSHORT = 0x0121u,
				T_PFUSHORT = 0x0221u,
				T_PHUSHORT = 0x0321u,
				T_32PUSHORT = 0x0421u,
				T_32PFUSHORT = 0x0521u,
				T_64PUSHORT = 0x0621u,

				T_INT2 = 0x0072u,
				T_PINT2 = 0x0172u,
				T_PFINT2 = 0x0272u,
				T_PHINT2 = 0x0372u,
				T_32PINT2 = 0x0472u,
				T_32PFINT2 = 0x0572u,
				T_64PINT2 = 0x0672u,

				T_UINT2 = 0x0073u,
				T_PUINT2 = 0x0173u,
				T_PFUINT2 = 0x0273u,
				T_PHUINT2 = 0x0373u,
				T_32PUINT2 = 0x0473u,
				T_32PFUINT2 = 0x0573u,
				T_64PUINT2 = 0x0673u,

				T_LONG = 0x0012u,
				T_PLONG = 0x0112u,
				T_PFLONG = 0x0212u,
				T_PHLONG = 0x0312u,
				T_32PLONG = 0x0412u,
				T_32PFLONG = 0x0512u,
				T_64PLONG = 0x0612u,

				T_ULONG = 0x0022u,
				T_PULONG = 0x0122u,
				T_PFULONG = 0x0222u,
				T_PHULONG = 0x0322u,
				T_32PULONG = 0x0422u,
				T_32PFULONG = 0x0522u,
				T_64PULONG = 0x0622u,

				T_INT4 = 0x0074u,
				T_PINT4 = 0x0174u,
				T_PFINT4 = 0x0274u,
				T_PHINT4 = 0x0374u,
				T_32PINT4 = 0x0474u,
				T_32PFINT4 = 0x0574u,
				T_64PINT4 = 0x0674u,

				T_UINT4 = 0x0075u,
				T_PUINT4 = 0x0175u,
				T_PFUINT4 = 0x0275u,
				T_PHUINT4 = 0x0375u,
				T_32PUINT4 = 0x0475u,
				T_32PFUINT4 = 0x0575u,
				T_64PUINT4 = 0x0675u,

				T_QUAD = 0x0013u,
				T_PQUAD = 0x0113u,
				T_PFQUAD = 0x0213u,
				T_PHQUAD = 0x0313u,
				T_32PQUAD = 0x0413u,
				T_32PFQUAD = 0x0513u,
				T_64PQUAD = 0x0613u,

				T_UQUAD = 0x0023u,
				T_PUQUAD = 0x0123u,
				T_PFUQUAD = 0x0223u,
				T_PHUQUAD = 0x0323u,
				T_32PUQUAD = 0x0423u,
				T_32PFUQUAD = 0x0523u,
				T_64PUQUAD = 0x0623u,

				T_INT8 = 0x0076u,
				T_PINT8 = 0x0176u,
				T_PFINT8 = 0x0276u,
				T_PHINT8 = 0x0376u,
				T_32PINT8 = 0x0476u,
				T_32PFINT8 = 0x0576u,
				T_64PINT8 = 0x0676u,

				T_UINT8 = 0x0077u,
				T_PUINT8 = 0x0177u,
				T_PFUINT8 = 0x0277u,
				T_PHUINT8 = 0x0377u,
				T_32PUINT8 = 0x0477u,
				T_32PFUINT8 = 0x0577u,
				T_64PUINT8 = 0x0677u,

				T_OCT = 0x0014u,
				T_POCT = 0x0114u,
				T_PFOCT = 0x0214u,
				T_PHOCT = 0x0314u,
				T_32POCT = 0x0414u,
				T_32PFOCT = 0x0514u,
				T_64POCT = 0x0614u,

				T_UOCT = 0x0024u,
				T_PUOCT = 0x0124u,
				T_PFUOCT = 0x0224u,
				T_PHUOCT = 0x0324u,
				T_32PUOCT = 0x0424u,
				T_32PFUOCT = 0x0524u,
				T_64PUOCT = 0x0624u,

				T_INT16 = 0x0078u,
				T_PINT16 = 0x0178u,
				T_PFINT16 = 0x0278u,
				T_PHINT16 = 0x0378u,
				T_32PINT16 = 0x0478u,
				T_32PFINT16 = 0x0578u,
				T_64PINT16 = 0x0678u,

				T_UINT16 = 0x0079u,
				T_PUINT16 = 0x0179u,
				T_PFUINT16 = 0x0279u,
				T_PHUINT16 = 0x0379u,
				T_32PUINT16 = 0x0479u,
				T_32PFUINT16 = 0x0579u,
				T_64PUINT16 = 0x0679u,

				T_REAL32 = 0x0040u,
				T_PREAL32 = 0x0140u,
				T_PFREAL32 = 0x0240u,
				T_PHREAL32 = 0x0340u,
				T_32PREAL32 = 0x0440u,
				T_32PFREAL32 = 0x0540u,
				T_64PREAL32 = 0x0640u,

				T_REAL48 = 0x0044u,
				T_PREAL48 = 0x0144u,
				T_PFREAL48 = 0x0244u,
				T_PHREAL48 = 0x0344u,
				T_32PREAL48 = 0x0444u,
				T_32PFREAL48 = 0x0544u,
				T_64PREAL48 = 0x0644u,

				T_REAL64 = 0x0041u,
				T_PREAL64 = 0x0141u,
				T_PFREAL64 = 0x0241u,
				T_PHREAL64 = 0x0341u,
				T_32PREAL64 = 0x0441u,
				T_32PFREAL64 = 0x0541u,
				T_64PREAL64 = 0x0641u,

				T_REAL80 = 0x0042u,
				T_PREAL80 = 0x0142u,
				T_PFREAL80 = 0x0242u,
				T_PHREAL80 = 0x0342u,
				T_32PREAL80 = 0x0442u,
				T_32PFREAL80 = 0x0542u,
				T_64PREAL80 = 0x0642u,

				T_REAL128 = 0x0043u,
				T_PREAL128 = 0x0143u,
				T_PFREAL128 = 0x0243u,
				T_PHREAL128 = 0x0343u,
				T_32PREAL128 = 0x0443u,
				T_32PFREAL128 = 0x0543u,
				T_64PREAL128 = 0x0643u,

				T_CPLX32 = 0x0050u,
				T_PCPLX32 = 0x0150u,
				T_PFCPLX32 = 0x0250u,
				T_PHCPLX32 = 0x0350u,
				T_32PCPLX32 = 0x0450u,
				T_32PFCPLX32 = 0x0550u,
				T_64PCPLX32 = 0x0650u,

				T_CPLX64 = 0x0051u,
				T_PCPLX64 = 0x0151u,
				T_PFCPLX64 = 0x0251u,
				T_PHCPLX64 = 0x0351u,
				T_32PCPLX64 = 0x0451u,
				T_32PFCPLX64 = 0x0551u,
				T_64PCPLX64 = 0x0651u,

				T_CPLX80 = 0x0052u,
				T_PCPLX80 = 0x0152u,
				T_PFCPLX80 = 0x0252u,
				T_PHCPLX80 = 0x0352u,
				T_32PCPLX80 = 0x0452u,
				T_32PFCPLX80 = 0x0552u,
				T_64PCPLX80 = 0x0652u,

				T_CPLX128 = 0x0053u,
				T_PCPLX128 = 0x0153u,
				T_PFCPLX128 = 0x0253u,
				T_PHCPLX128 = 0x0353u,
				T_32PCPLX128 = 0x0453u,
				T_32PFCPLX128 = 0x0553u,
				T_64PCPLX128 = 0x0653u,

				T_BOOL08 = 0x0030u,
				T_PBOOL08 = 0x0130u,
				T_PFBOOL08 = 0x0230u,
				T_PHBOOL08 = 0x0330u,
				T_32PBOOL08 = 0x0430u,
				T_32PFBOOL08 = 0x0530u,
				T_64PBOOL08 = 0x0630u,

				T_BOOL16 = 0x0031u,
				T_PBOOL16 = 0x0131u,
				T_PFBOOL16 = 0x0231u,
				T_PHBOOL16 = 0x0331u,
				T_32PBOOL16 = 0x0431u,
				T_32PFBOOL16 = 0x0531u,
				T_64PBOOL16 = 0x0631u,

				T_BOOL32 = 0x0032u,
				T_PBOOL32 = 0x0132u,
				T_PFBOOL32 = 0x0232u,
				T_PHBOOL32 = 0x0332u,
				T_32PBOOL32 = 0x0432u,
				T_32PFBOOL32 = 0x0532u,
				T_64PBOOL32 = 0x0632u,

				T_BOOL64 = 0x0033u,
				T_PBOOL64 = 0x0133u,
				T_PFBOOL64 = 0x0233u,
				T_PHBOOL64 = 0x0333u,
				T_32PBOOL64 = 0x0433u,
				T_32PFBOOL64 = 0x0533u,
				T_64PBOOL64 = 0x0633u,

				T_NCVPTR = 0x01F0u,
				T_FCVPTR = 0x02F0u,
				T_HCVPTR = 0x03F0u,
				T_32NCVPTR = 0x04F0u,
				T_32FCVPTR = 0x05F0u,
				T_64NCVPTR = 0x06F0u
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
			struct TypeProperty
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

					// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L2521
					struct
					{
						MemberAttributes	attributes;	// attribute
						uint32_t			index;		// type index of direct virtual base class
						uint32_t			vbpIndex;   // type index of virtual base pointer
						PDB_FLEXIBLE_ARRAY_MEMBER(char, vbpOffset); // virtual base pointer offset from address point
					} LF_VBCLASS, LF_IVBCLASS;

					// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L2483
					// index leaf - contains type index of another leaf
					// a major use of this leaf is to allow the compilers to emit a
					// long complex list (LF_FIELD) in smaller pieces.
					struct
					{
						uint16_t pad0; // internal padding, must be 0
						uint32_t type; // type index of referenced leaf
					} LF_INDEX;

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

			// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L2131
			struct MethodListEntry
			{
				MemberAttributes attributes;					// method attribute
				uint16_t		pad0;							// internal padding, must be 0
				uint32_t		index;							// index to type record for procedure
				PDB_FLEXIBLE_ARRAY_MEMBER(uint32_t, vbaseoff);	// offset in vfunctable if virtual, empty otherwise.
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
						// This is actually a list of the MethodListEntry type above, but it has flexible
						// size, so you need to manually iterate.
						PDB_FLEXIBLE_ARRAY_MEMBER(char, mList);
					} LF_METHODLIST;

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
					} LF_MFUNCTION;

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
						TypeProperty property;	// property attribute field
						uint32_t field;			// type index of LF_FIELD descriptor list
						uint32_t derived;		// type index of derived from list if not zero
						uint32_t vshape;		// type index of vshape table for this class
						union
						{
							PDB_FLEXIBLE_ARRAY_MEMBER(char, data);
							LeafEasy lfEasy;
						};
					} LF_CLASS;

					struct
					{
						uint16_t count;			// count of number of elements in class
						uint32_t property;		// property attribute field
						uint32_t field;			// type index of LF_FIELD descriptor list
						uint32_t derived;		// type index of derived from list if not zero
						uint32_t vshape;		// type index of vshape table for this class
						union
						{
							PDB_FLEXIBLE_ARRAY_MEMBER(char, data);
							LeafEasy lfEasy;
						};
					} LF_CLASS2;

					// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L1647
					struct
					{
						uint16_t count;			// count of number of elements in class
						TypeProperty property;	// property attribute field
						uint32_t field;			// type index of LF_FIELD descriptor list
						PDB_FLEXIBLE_ARRAY_MEMBER(char, data);
					} LF_UNION;

					// https://github.com/microsoft/microsoft-pdb/blob/master/include/cvinfo.h#L1752
					struct
					{
						uint16_t count;			// count of number of elements in class
						TypeProperty property;	// property attribute field
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
