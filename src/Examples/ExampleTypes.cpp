
#include "Examples_PCH.h"
#include "ExampleTimedScope.h"
#include "ExampleTypeTable.h"
#include "PDB_RawFile.h"
#include "PDB_DBIStream.h"
#include "PDB_TPIStream.h"
#include <cstring>
#include <cinttypes>

// not all enumeration values are handled explicitly by some of the switch statements
PDB_DISABLE_WARNING_MSVC(4061)
PDB_DISABLE_WARNING_CLANG("-Wswitch-enum")

// some format strings are not string literals
PDB_DISABLE_WARNING_MSVC(4774)
PDB_DISABLE_WARNING_CLANG("-Wformat-nonliteral")

std::string GetTypeName(const TypeTable& typeTable, uint32_t typeIndex);

static uint8_t GetLeafSize(PDB::CodeView::TPI::TypeRecordKind kind)
{
	if (kind < PDB::CodeView::TPI::TypeRecordKind::LF_NUMERIC)
	{
		// No leaf can have an index less than LF_NUMERIC (0x8000)
		// so word is the value...
		return sizeof(PDB::CodeView::TPI::TypeRecordKind);
	}

	switch (kind)
	{
	case PDB::CodeView::TPI::TypeRecordKind::LF_CHAR:
		return sizeof(PDB::CodeView::TPI::TypeRecordKind) + sizeof(uint8_t);

	case PDB::CodeView::TPI::TypeRecordKind::LF_USHORT:
	case PDB::CodeView::TPI::TypeRecordKind::LF_SHORT:
		return sizeof(PDB::CodeView::TPI::TypeRecordKind) + sizeof(uint16_t);

	case PDB::CodeView::TPI::TypeRecordKind::LF_LONG:
	case PDB::CodeView::TPI::TypeRecordKind::LF_ULONG:
		return sizeof(PDB::CodeView::TPI::TypeRecordKind) + sizeof(uint32_t);

	case PDB::CodeView::TPI::TypeRecordKind::LF_QUADWORD:
	case PDB::CodeView::TPI::TypeRecordKind::LF_UQUADWORD:
		return sizeof(PDB::CodeView::TPI::TypeRecordKind) + sizeof(uint64_t);

	default:
		printf("Error! 0x%04x bogus type encountered, aborting...\n", PDB_AS_UNDERLYING(kind));
	}
	return 0;
}


static const char* GetLeafName(const char* data, PDB::CodeView::TPI::TypeRecordKind kind)
{
	return &data[GetLeafSize(kind)];
}


static const char* GetTypeName(const TypeTable& typeTable, uint32_t typeIndex, uint8_t& pointerLevel, const PDB::CodeView::TPI::Record** referencedType, const PDB::CodeView::TPI::Record** modifierRecord)
{
	const char* typeName = nullptr;
	const PDB::CodeView::TPI::Record* underlyingType = nullptr;

	if (referencedType)
		*referencedType = nullptr;

	if (modifierRecord)
		*modifierRecord = nullptr;

	auto typeIndexBegin = typeTable.GetFirstTypeIndex();
	if (typeIndex < typeIndexBegin)
	{
		auto type = static_cast<PDB::CodeView::TPI::TypeIndexKind>(typeIndex);
		switch (type)
		{
		case PDB::CodeView::TPI::TypeIndexKind::T_NOTYPE:
			return "<NO TYPE>";
		case PDB::CodeView::TPI::TypeIndexKind::T_HRESULT:
			return "HRESULT";
		case PDB::CodeView::TPI::TypeIndexKind::T_32PHRESULT:
		case PDB::CodeView::TPI::TypeIndexKind::T_64PHRESULT:
			return "PHRESULT";

		case PDB::CodeView::TPI::TypeIndexKind::T_UNKNOWN_0600:
			return "UNKNOWN_0x0600";

		case PDB::CodeView::TPI::TypeIndexKind::T_VOID:
			return "void";
		case PDB::CodeView::TPI::TypeIndexKind::T_32PVOID:
		case PDB::CodeView::TPI::TypeIndexKind::T_64PVOID:
		case PDB::CodeView::TPI::TypeIndexKind::T_PVOID:
			return "PVOID";

		case PDB::CodeView::TPI::TypeIndexKind::T_32PBOOL08:
		case PDB::CodeView::TPI::TypeIndexKind::T_32PBOOL16:
		case PDB::CodeView::TPI::TypeIndexKind::T_32PBOOL32:
		case PDB::CodeView::TPI::TypeIndexKind::T_32PBOOL64:
		case PDB::CodeView::TPI::TypeIndexKind::T_64PBOOL08:
		case PDB::CodeView::TPI::TypeIndexKind::T_64PBOOL16:
		case PDB::CodeView::TPI::TypeIndexKind::T_64PBOOL32:
		case PDB::CodeView::TPI::TypeIndexKind::T_64PBOOL64:
			return "PBOOL";

		case PDB::CodeView::TPI::TypeIndexKind::T_BOOL08:
		case PDB::CodeView::TPI::TypeIndexKind::T_BOOL16:
		case PDB::CodeView::TPI::TypeIndexKind::T_BOOL32:
			return "BOOL";

		case PDB::CodeView::TPI::TypeIndexKind::T_RCHAR:
		case PDB::CodeView::TPI::TypeIndexKind::T_CHAR:
			return "CHAR";
		case PDB::CodeView::TPI::TypeIndexKind::T_32PRCHAR:
		case PDB::CodeView::TPI::TypeIndexKind::T_32PCHAR:
		case PDB::CodeView::TPI::TypeIndexKind::T_64PRCHAR:
		case PDB::CodeView::TPI::TypeIndexKind::T_64PCHAR:
		case PDB::CodeView::TPI::TypeIndexKind::T_PRCHAR:
		case PDB::CodeView::TPI::TypeIndexKind::T_PCHAR:
			return "PCHAR";

		case PDB::CodeView::TPI::TypeIndexKind::T_UCHAR:
			return "UCHAR";
		case PDB::CodeView::TPI::TypeIndexKind::T_32PUCHAR:
		case PDB::CodeView::TPI::TypeIndexKind::T_64PUCHAR:
		case PDB::CodeView::TPI::TypeIndexKind::T_PUCHAR:
			return "PUCHAR";

		case PDB::CodeView::TPI::TypeIndexKind::T_WCHAR:
			return "WCHAR";
		case PDB::CodeView::TPI::TypeIndexKind::T_32PWCHAR:
		case PDB::CodeView::TPI::TypeIndexKind::T_64PWCHAR:
		case PDB::CodeView::TPI::TypeIndexKind::T_PWCHAR:
			return "PWCHAR";

		case PDB::CodeView::TPI::TypeIndexKind::T_CHAR8:
			return "CHAR8";
		case PDB::CodeView::TPI::TypeIndexKind::T_PCHAR8:
		case PDB::CodeView::TPI::TypeIndexKind::T_PFCHAR8:
		case PDB::CodeView::TPI::TypeIndexKind::T_PHCHAR8:
		case PDB::CodeView::TPI::TypeIndexKind::T_32PCHAR8:
		case PDB::CodeView::TPI::TypeIndexKind::T_32PFCHAR8:
		case PDB::CodeView::TPI::TypeIndexKind::T_64PCHAR8:
			return "PCHAR8";

		case PDB::CodeView::TPI::TypeIndexKind::T_CHAR16:
			return "CHAR16";
		case PDB::CodeView::TPI::TypeIndexKind::T_PCHAR16:
		case PDB::CodeView::TPI::TypeIndexKind::T_32PCHAR16:
		case PDB::CodeView::TPI::TypeIndexKind::T_64PCHAR16:
			return "PCHAR16";

		case PDB::CodeView::TPI::TypeIndexKind::T_CHAR32:
			return "CHAR32";
		case PDB::CodeView::TPI::TypeIndexKind::T_PCHAR32:
		case PDB::CodeView::TPI::TypeIndexKind::T_32PCHAR32:
		case PDB::CodeView::TPI::TypeIndexKind::T_64PCHAR32:
			return "PCHAR32";

		case PDB::CodeView::TPI::TypeIndexKind::T_SHORT:
			return "SHORT";
		case PDB::CodeView::TPI::TypeIndexKind::T_32PSHORT:
		case PDB::CodeView::TPI::TypeIndexKind::T_64PSHORT:
		case PDB::CodeView::TPI::TypeIndexKind::T_PSHORT:
			return "PSHORT";
		case PDB::CodeView::TPI::TypeIndexKind::T_USHORT:
			return "USHORT";
		case PDB::CodeView::TPI::TypeIndexKind::T_32PUSHORT:
		case PDB::CodeView::TPI::TypeIndexKind::T_64PUSHORT:
		case PDB::CodeView::TPI::TypeIndexKind::T_PUSHORT:
			return "PUSHORT";
		case PDB::CodeView::TPI::TypeIndexKind::T_LONG:
			return "LONG";
		case PDB::CodeView::TPI::TypeIndexKind::T_32PLONG:
		case PDB::CodeView::TPI::TypeIndexKind::T_64PLONG:
		case PDB::CodeView::TPI::TypeIndexKind::T_PLONG:
			return "PLONG";
		case PDB::CodeView::TPI::TypeIndexKind::T_ULONG:
			return "ULONG";
		case PDB::CodeView::TPI::TypeIndexKind::T_32PULONG:
		case PDB::CodeView::TPI::TypeIndexKind::T_64PULONG:
		case PDB::CodeView::TPI::TypeIndexKind::T_PULONG:
			return "PULONG";
		case PDB::CodeView::TPI::TypeIndexKind::T_REAL32:
			return "FLOAT";
		case PDB::CodeView::TPI::TypeIndexKind::T_32PREAL32:
		case PDB::CodeView::TPI::TypeIndexKind::T_64PREAL32:
		case PDB::CodeView::TPI::TypeIndexKind::T_PREAL32:
			return "PFLOAT";
		case PDB::CodeView::TPI::TypeIndexKind::T_REAL64:
			return "DOUBLE";
		case PDB::CodeView::TPI::TypeIndexKind::T_32PREAL64:
		case PDB::CodeView::TPI::TypeIndexKind::T_64PREAL64:
		case PDB::CodeView::TPI::TypeIndexKind::T_PREAL64:
			return "PDOUBLE";
		case PDB::CodeView::TPI::TypeIndexKind::T_REAL80:
			return "REAL80";
		case PDB::CodeView::TPI::TypeIndexKind::T_32PREAL80:
		case PDB::CodeView::TPI::TypeIndexKind::T_64PREAL80:
		case PDB::CodeView::TPI::TypeIndexKind::T_PREAL80:
			return "PREAL80";
		case PDB::CodeView::TPI::TypeIndexKind::T_QUAD:
			return "LONGLONG";
		case PDB::CodeView::TPI::TypeIndexKind::T_32PQUAD:
		case PDB::CodeView::TPI::TypeIndexKind::T_64PQUAD:
		case PDB::CodeView::TPI::TypeIndexKind::T_PQUAD:
			return "PLONGLONG";
		case PDB::CodeView::TPI::TypeIndexKind::T_UQUAD:
			return "ULONGLONG";
		case PDB::CodeView::TPI::TypeIndexKind::T_32PUQUAD:
		case PDB::CodeView::TPI::TypeIndexKind::T_64PUQUAD:
		case PDB::CodeView::TPI::TypeIndexKind::T_PUQUAD:
			return "PULONGLONG";
		case PDB::CodeView::TPI::TypeIndexKind::T_INT4:
			return "INT";
		case PDB::CodeView::TPI::TypeIndexKind::T_32PINT4:
		case PDB::CodeView::TPI::TypeIndexKind::T_64PINT4:
		case PDB::CodeView::TPI::TypeIndexKind::T_PINT4:
			return "PINT";
		case PDB::CodeView::TPI::TypeIndexKind::T_UINT4:
			return "UINT";
		case PDB::CodeView::TPI::TypeIndexKind::T_32PUINT4:
		case PDB::CodeView::TPI::TypeIndexKind::T_64PUINT4:
		case PDB::CodeView::TPI::TypeIndexKind::T_PUINT4:
			return "PUINT";

		case PDB::CodeView::TPI::TypeIndexKind::T_UINT8:
			return "UINT8";
		case PDB::CodeView::TPI::TypeIndexKind::T_PUINT8:
		case PDB::CodeView::TPI::TypeIndexKind::T_PFUINT8:
		case PDB::CodeView::TPI::TypeIndexKind::T_PHUINT8:
		case PDB::CodeView::TPI::TypeIndexKind::T_32PUINT8:
		case PDB::CodeView::TPI::TypeIndexKind::T_32PFUINT8:
		case PDB::CodeView::TPI::TypeIndexKind::T_64PUINT8:
			return "PUINT8";

		case PDB::CodeView::TPI::TypeIndexKind::T_INT8:
			return "INT8";
		case PDB::CodeView::TPI::TypeIndexKind::T_PINT8:
		case PDB::CodeView::TPI::TypeIndexKind::T_PFINT8:
		case PDB::CodeView::TPI::TypeIndexKind::T_PHINT8:
		case PDB::CodeView::TPI::TypeIndexKind::T_32PINT8:
		case PDB::CodeView::TPI::TypeIndexKind::T_32PFINT8:
		case PDB::CodeView::TPI::TypeIndexKind::T_64PINT8:
			return "PINT8";

		case PDB::CodeView::TPI::TypeIndexKind::T_OCT:
			return "OCTAL";

		case PDB::CodeView::TPI::TypeIndexKind::T_POCT:
		case PDB::CodeView::TPI::TypeIndexKind::T_PFOCT:
		case PDB::CodeView::TPI::TypeIndexKind::T_PHOCT:
		case PDB::CodeView::TPI::TypeIndexKind::T_32POCT:
		case PDB::CodeView::TPI::TypeIndexKind::T_32PFOCT:
		case PDB::CodeView::TPI::TypeIndexKind::T_64POCT:
			return "POCTAL";

		case PDB::CodeView::TPI::TypeIndexKind::T_UOCT:
			return "UOCTAL";

		case PDB::CodeView::TPI::TypeIndexKind::T_PUOCT:
		case PDB::CodeView::TPI::TypeIndexKind::T_PFUOCT:
		case PDB::CodeView::TPI::TypeIndexKind::T_PHUOCT:
		case PDB::CodeView::TPI::TypeIndexKind::T_32PUOCT:
		case PDB::CodeView::TPI::TypeIndexKind::T_32PFUOCT:
		case PDB::CodeView::TPI::TypeIndexKind::T_64PUOCT:
			return "PUOCTAL";

		default:
			PDB_ASSERT(false, "Unhandled special type 0x%X", typeIndex);
			return "unhandled_special_type";
		}
	}
	else
	{
		auto typeRecord = typeTable.GetTypeRecord(typeIndex);
		if (!typeRecord)
			return nullptr;

		switch (typeRecord->header.kind)
		{
		case PDB::CodeView::TPI::TypeRecordKind::LF_MODIFIER:
			if(modifierRecord)
				*modifierRecord = typeRecord;
			return GetTypeName(typeTable, typeRecord->data.LF_MODIFIER.type, pointerLevel, referencedType, nullptr);
		case PDB::CodeView::TPI::TypeRecordKind::LF_POINTER:
			++pointerLevel;
			if(referencedType)
				*referencedType = typeRecord;
			if (typeRecord->data.LF_POINTER.utype >= typeIndexBegin)
			{
				underlyingType = typeTable.GetTypeRecord(typeRecord->data.LF_POINTER.utype);
				if (!underlyingType)
					return nullptr;

				if(underlyingType->header.kind == PDB::CodeView::TPI::TypeRecordKind::LF_POINTER)
					return GetTypeName(typeTable, typeRecord->data.LF_POINTER.utype, pointerLevel, referencedType, modifierRecord);

				// Type record order can be LF_POINTER -> LF_MODIFIER -> LF_POINTER -> ...
				if (underlyingType->header.kind == PDB::CodeView::TPI::TypeRecordKind::LF_MODIFIER)
				{
					if (modifierRecord)
						*modifierRecord = underlyingType;

					return GetTypeName(typeTable, underlyingType->data.LF_MODIFIER.type, pointerLevel, referencedType, nullptr);
				}
			}

			return GetTypeName(typeTable, typeRecord->data.LF_POINTER.utype, pointerLevel, &typeRecord, modifierRecord);
		case PDB::CodeView::TPI::TypeRecordKind::LF_PROCEDURE:
			if (referencedType)
				*referencedType = typeRecord;
			return nullptr;
		case PDB::CodeView::TPI::TypeRecordKind::LF_BITFIELD:
			if (typeRecord->data.LF_BITFIELD.type < typeIndexBegin)
			{
				typeName = GetTypeName(typeTable, typeRecord->data.LF_BITFIELD.type, pointerLevel, nullptr, modifierRecord);
				if (referencedType)
					*referencedType = typeRecord;
				return typeName;
			}
			else
			{
				if (referencedType)
					*referencedType = typeRecord;
				return nullptr;
			}
		case PDB::CodeView::TPI::TypeRecordKind::LF_ARRAY:
			if (referencedType)
				*referencedType = typeRecord;
			return GetTypeName(typeTable, typeRecord->data.LF_ARRAY.elemtype, pointerLevel, &typeRecord, modifierRecord);
		case PDB::CodeView::TPI::TypeRecordKind::LF_CLASS:
		case PDB::CodeView::TPI::TypeRecordKind::LF_STRUCTURE:
			return GetLeafName(typeRecord->data.LF_CLASS.data, typeRecord->header.kind);

		case PDB::CodeView::TPI::TypeRecordKind::LF_CLASS2:
		case PDB::CodeView::TPI::TypeRecordKind::LF_STRUCTURE2:
			return GetLeafName(typeRecord->data.LF_CLASS2.data, typeRecord->header.kind);

		case  PDB::CodeView::TPI::TypeRecordKind::LF_UNION:
			return GetLeafName(typeRecord->data.LF_UNION.data, typeRecord->header.kind);
		case PDB::CodeView::TPI::TypeRecordKind::LF_ENUM:
			return &typeRecord->data.LF_ENUM.name[0];
		case PDB::CodeView::TPI::TypeRecordKind::LF_MFUNCTION:
			if (referencedType)
				*referencedType = typeRecord;
			return nullptr;
	
		default:
			PDB_ASSERT(false, "Unhandled TypeRecordKind 0x%X", static_cast<uint16_t>(typeRecord->header.kind));
			break;
		}
	   
	}

	return "unknown_type";
}

static const char* GetModifierName(const PDB::CodeView::TPI::Record* modifierRecord)
{
	if (modifierRecord->data.LF_MODIFIER.attr.MOD_const)
		return "const";
	else if (modifierRecord->data.LF_MODIFIER.attr.MOD_volatile)
		return "volatile";
	else if (modifierRecord->data.LF_MODIFIER.attr.MOD_unaligned)
		return "unaligned";
	
	return "";
}

static bool GetMethodPrototype(const TypeTable& typeTable, const PDB::CodeView::TPI::Record* methodRecord, std::string& methodPrototype);

static bool GetFunctionPrototype(const TypeTable& typeTable, const PDB::CodeView::TPI::Record* functionRecord, std::string& functionPrototype)
{
	PDB_ASSERT(functionRecord->header.kind == PDB::CodeView::TPI::TypeRecordKind::LF_PROCEDURE, "TPI Record kind is 0x%X, expected 0x%X (LF_PROCEDURE)",
		(uint32_t)functionRecord->header.kind, (uint32_t)PDB::CodeView::TPI::TypeRecordKind::LF_PROCEDURE);

	std::string underlyingTypePrototype;

	size_t markerPos = 0;
	uint8_t pointerLevel = 0;
	const PDB::CodeView::TPI::Record* referencedType = nullptr;
	const PDB::CodeView::TPI::Record* underlyingType = nullptr;
	const PDB::CodeView::TPI::Record* modifierRecord = nullptr;

	functionPrototype.clear();

	auto typeName = GetTypeName(typeTable, functionRecord->data.LF_PROCEDURE.rvtype, pointerLevel, &referencedType, &modifierRecord);
	if (typeName)
	{
		if (modifierRecord)
		{
			functionPrototype += GetModifierName(modifierRecord);
			functionPrototype += ' ';
		}

		functionPrototype += typeName;

		for (size_t i = 0; i < pointerLevel; i++)
			functionPrototype += '*';
	}
	else
	{
		PDB_ASSERT(referencedType->header.kind == PDB::CodeView::TPI::TypeRecordKind::LF_POINTER, "Referenced type kind 0x%X != LF_POINTER (0x%X)", (uint32_t)referencedType->header.kind, (uint32_t)PDB::CodeView::TPI::TypeRecordKind::LF_POINTER);

		underlyingType = typeTable.GetTypeRecord(referencedType->data.LF_POINTER.utype);
		if (!underlyingType)
			return false;

		if (underlyingType->header.kind == PDB::CodeView::TPI::TypeRecordKind::LF_PROCEDURE)
		{
			if (!GetFunctionPrototype(typeTable, underlyingType, underlyingTypePrototype))
				return false;
		}
		else if (underlyingType->header.kind == PDB::CodeView::TPI::TypeRecordKind::LF_MFUNCTION)
		{
			if (!GetMethodPrototype(typeTable, underlyingType, underlyingTypePrototype))
				return false;
		}
		else
		{
			PDB_ASSERT(false, "Unhandled underlyingType kind 0x%X", (uint32_t)underlyingType->header.kind);
		}

		markerPos = underlyingTypePrototype.find("%s");
		underlyingTypePrototype.erase(markerPos, 2);
		functionPrototype = underlyingTypePrototype;
	}

	functionPrototype += " (*%s)(";

	if (functionRecord->data.LF_PROCEDURE.parmcount)
	{
		auto argList = typeTable.GetTypeRecord(functionRecord->data.LF_PROCEDURE.arglist);
		if (!argList)
			return false;

		for (size_t i = 0; i < argList->data.LF_ARGLIST.count; i++)
		{
			pointerLevel = 0;
			typeName = GetTypeName(typeTable, argList->data.LF_ARGLIST.arg[i], pointerLevel, &referencedType, &modifierRecord);
			if (referencedType)
			{
				if (referencedType->data.LF_POINTER.utype >= typeTable.GetFirstTypeIndex())
				{
					underlyingType = typeTable.GetTypeRecord(referencedType->data.LF_POINTER.utype);
					if (!underlyingType)
						return false;
				}

				if (!underlyingType || (underlyingType->header.kind != PDB::CodeView::TPI::TypeRecordKind::LF_PROCEDURE && underlyingType->header.kind != PDB::CodeView::TPI::TypeRecordKind::LF_MFUNCTION))
				{
					if (modifierRecord)
					{
						functionPrototype += GetModifierName(modifierRecord);
						functionPrototype += ' ';
					}

					functionPrototype += typeName;
					functionPrototype += '*';

					if (referencedType->data.LF_POINTER.attr.isvolatile)
						functionPrototype += "volatile";
					else if (referencedType->data.LF_POINTER.attr.isconst)
						functionPrototype += "const";
				}
				else if(underlyingType->header.kind == PDB::CodeView::TPI::TypeRecordKind::LF_PROCEDURE)
				{
					if (!GetFunctionPrototype(typeTable, underlyingType, underlyingTypePrototype))
						return false;

					markerPos = underlyingTypePrototype.find("%s");
					underlyingTypePrototype.erase(markerPos, 2);

					for (size_t j = 1; j < pointerLevel; j++)
						underlyingTypePrototype.insert(markerPos, 1, '*');

					functionPrototype += underlyingTypePrototype;
				}
				else if(underlyingType->header.kind == PDB::CodeView::TPI::TypeRecordKind::LF_MFUNCTION)
				{
					functionPrototype += GetTypeName(typeTable, argList->data.LF_ARGLIST.arg[i]);
				}
			}
			else
			{
				functionPrototype += typeName;
			}

			if (i < (argList->data.LF_ARGLIST.count - 1))
				functionPrototype += ", ";
		}
	}

	functionPrototype += ')';

	return true;
}


static bool GetMethodPrototype(const TypeTable& typeTable, const PDB::CodeView::TPI::Record* methodRecord, std::string& methodPrototype)
{
	PDB_ASSERT(methodRecord->header.kind == PDB::CodeView::TPI::TypeRecordKind::LF_MFUNCTION, "TPI Record kind is 0x%X, expected 0x%X (LF_MFUNCTION)",
		(uint32_t)methodRecord->header.kind, (uint32_t)PDB::CodeView::TPI::TypeRecordKind::LF_MFUNCTION);

	std::string underlyingTypePrototype;

	size_t markerPos = 0;
	uint8_t pointerLevel = 0;
	const PDB::CodeView::TPI::Record* referencedType = nullptr;
	const PDB::CodeView::TPI::Record* underlyingType = nullptr;
	const PDB::CodeView::TPI::Record* modifierRecord = nullptr;

	methodPrototype.clear();

	auto typeName = GetTypeName(typeTable, methodRecord->data.LF_MFUNCTION.rvtype, pointerLevel, &referencedType, &modifierRecord);
	if (typeName)
	{
		if (modifierRecord)
		{
			methodPrototype += GetModifierName(modifierRecord);
			methodPrototype += ' ';
		}

		methodPrototype += typeName;

		for (size_t i = 0; i < pointerLevel; i++)
			methodPrototype += '*';
	}
	else
	{
		underlyingType = typeTable.GetTypeRecord(referencedType->data.LF_POINTER.utype);
		if (!underlyingType)
			return false;

		if (underlyingType->header.kind == PDB::CodeView::TPI::TypeRecordKind::LF_PROCEDURE)
		{
			if (!GetFunctionPrototype(typeTable, underlyingType, underlyingTypePrototype))
				return false;
		}
		else if(underlyingType->header.kind == PDB::CodeView::TPI::TypeRecordKind::LF_MFUNCTION)
		{
			if (!GetMethodPrototype(typeTable, underlyingType, underlyingTypePrototype))
				return false;
		}
		else
		{
			PDB_ASSERT(false, "Unhandled underlyingType kind 0x%X", (uint32_t)underlyingType->header.kind);
		}

		markerPos = underlyingTypePrototype.find("%s");
		underlyingTypePrototype.erase(markerPos, 2);
		methodPrototype = underlyingTypePrototype;
	}

	methodPrototype += " %s(";

	if (methodRecord->data.LF_MFUNCTION.parmcount)
	{
		auto argList = typeTable.GetTypeRecord(methodRecord->data.LF_MFUNCTION.arglist);
		if (!argList)
			return false;

		for (size_t i = 0; i < argList->data.LF_ARGLIST.count; i++)
		{
			pointerLevel = 0;
			typeName = GetTypeName(typeTable, argList->data.LF_ARGLIST.arg[i], pointerLevel, &referencedType, &modifierRecord);
			if (referencedType)
			{
				if (referencedType->data.LF_POINTER.utype >= typeTable.GetFirstTypeIndex())
				{
					underlyingType = typeTable.GetTypeRecord(referencedType->data.LF_POINTER.utype);
					if (!underlyingType)
						return false;
				}

				if (!underlyingType || (underlyingType->header.kind != PDB::CodeView::TPI::TypeRecordKind::LF_PROCEDURE && underlyingType->header.kind != PDB::CodeView::TPI::TypeRecordKind::LF_MFUNCTION))
				{
					if (modifierRecord)
					{
						methodPrototype += GetModifierName(modifierRecord);
						methodPrototype += ' ';
					}

					if(typeName)
						methodPrototype += typeName;
	
					methodPrototype += '*';

					if (referencedType->data.LF_POINTER.attr.isvolatile)
						methodPrototype += "volatile";
					else if (referencedType->data.LF_POINTER.attr.isconst)
						methodPrototype += "const";
				}
				else if (underlyingType->header.kind == PDB::CodeView::TPI::TypeRecordKind::LF_PROCEDURE)
				{
					if (!GetFunctionPrototype(typeTable, underlyingType, underlyingTypePrototype))
						return false;

					markerPos = underlyingTypePrototype.find("%s");
					underlyingTypePrototype.erase(markerPos, 2);

					for (size_t j = 1; j < pointerLevel; j++)
						underlyingTypePrototype.insert(markerPos, 1, '*');

					methodPrototype += underlyingTypePrototype;
				}
				else if (underlyingType->header.kind == PDB::CodeView::TPI::TypeRecordKind::LF_MFUNCTION)
				{
					methodPrototype += GetTypeName(typeTable, argList->data.LF_ARGLIST.arg[i]);
				}
			}
			else
			{
				methodPrototype += typeName;
			}

			if (i < (argList->data.LF_ARGLIST.count - 1))
				methodPrototype += ", ";
		}
	}

	methodPrototype += ')';

	return true;
}


static const char* GetMethodName(const PDB::CodeView::TPI::FieldList* fieldRecord)
{
	auto methodAttributes = static_cast<PDB::CodeView::TPI::MethodProperty>(fieldRecord->data.LF_ONEMETHOD.attributes.mprop);
	switch (methodAttributes)
	{
	case PDB::CodeView::TPI::MethodProperty::Intro:
	case PDB::CodeView::TPI::MethodProperty::PureIntro:
		return &reinterpret_cast<const char*>(fieldRecord->data.LF_ONEMETHOD.vbaseoff)[sizeof(uint32_t)];
	default:
		break;
	}

	return  &reinterpret_cast<const char*>(fieldRecord->data.LF_ONEMETHOD.vbaseoff)[0];
}


static void DisplayFields(const TypeTable& typeTable, const PDB::CodeView::TPI::Record* record)
{
	const PDB::CodeView::TPI::Record* referencedType = nullptr;
	const PDB::CodeView::TPI::Record* underlyingType = nullptr;
	const PDB::CodeView::TPI::Record* modifierRecord = nullptr;
	const char* leafName = nullptr;
	const char* typeName = nullptr;
	std::string functionPrototype;
	uint16_t offset = 0;

	auto maximumSize = record->header.size - sizeof(uint16_t);

	for (size_t i = 0; i < maximumSize;)
	{
		uint8_t pointerLevel = 0;
		auto fieldRecord = reinterpret_cast<const PDB::CodeView::TPI::FieldList*>(reinterpret_cast<const uint8_t*>(&record->data.LF_FIELD.list) + i);

		// Other kinds of records are not implemented
		PDB_ASSERT(
			fieldRecord->kind == PDB::CodeView::TPI::TypeRecordKind::LF_BCLASS ||
			fieldRecord->kind == PDB::CodeView::TPI::TypeRecordKind::LF_VBCLASS ||
			fieldRecord->kind == PDB::CodeView::TPI::TypeRecordKind::LF_IVBCLASS ||
			fieldRecord->kind == PDB::CodeView::TPI::TypeRecordKind::LF_INDEX ||
			fieldRecord->kind == PDB::CodeView::TPI::TypeRecordKind::LF_VFUNCTAB ||
			fieldRecord->kind == PDB::CodeView::TPI::TypeRecordKind::LF_NESTTYPE ||
			fieldRecord->kind == PDB::CodeView::TPI::TypeRecordKind::LF_ENUM ||
			fieldRecord->kind == PDB::CodeView::TPI::TypeRecordKind::LF_MEMBER ||
			fieldRecord->kind == PDB::CodeView::TPI::TypeRecordKind::LF_STMEMBER ||
			fieldRecord->kind == PDB::CodeView::TPI::TypeRecordKind::LF_METHOD ||
			fieldRecord->kind == PDB::CodeView::TPI::TypeRecordKind::LF_ONEMETHOD,
			"Unknown record kind %X",
			static_cast<unsigned int>(fieldRecord->kind));

		if (fieldRecord->kind == PDB::CodeView::TPI::TypeRecordKind::LF_MEMBER)
		{
			if (fieldRecord->data.LF_MEMBER.lfEasy.kind < PDB::CodeView::TPI::TypeRecordKind::LF_NUMERIC)
				offset = *reinterpret_cast<const uint16_t*>(&fieldRecord->data.LF_MEMBER.offset[0]);
			else
				offset = *reinterpret_cast<const uint16_t*>(&fieldRecord->data.LF_MEMBER.offset[sizeof(PDB::CodeView::TPI::TypeRecordKind)]);

			leafName = GetLeafName(fieldRecord->data.LF_MEMBER.offset, fieldRecord->data.LF_MEMBER.lfEasy.kind);

			typeName = GetTypeName(typeTable, fieldRecord->data.LF_MEMBER.index, pointerLevel, &referencedType, &modifierRecord);
			if (referencedType)
			{
				switch (referencedType->header.kind)
				{
				case PDB::CodeView::TPI::TypeRecordKind::LF_POINTER:
					if (referencedType->data.LF_POINTER.utype >= typeTable.GetFirstTypeIndex())
					{
						underlyingType = typeTable.GetTypeRecord(referencedType->data.LF_POINTER.utype);
						if (!underlyingType)
							break;

						if (underlyingType->header.kind != PDB::CodeView::TPI::TypeRecordKind::LF_PROCEDURE)
						{
							if (modifierRecord)
								printf("[0x%X]%s %s", offset, GetModifierName(modifierRecord), typeName);
							else
								printf("[0x%X]%s", offset, typeName);

							for (size_t j = 0; j < pointerLevel; j++)
								printf("*");

							printf(" %s\n", leafName);
						}
						else
						{
							if (!GetFunctionPrototype(typeTable, underlyingType, functionPrototype))
								break;

							printf("[0x%X]", offset);
							printf(functionPrototype.c_str(), leafName);
							printf("\n");
						}
					}
					else
					{
						printf("[0x%X]%s", offset, typeName);

						for (size_t j = 0; j < pointerLevel; j++)
							printf("*");

						if (referencedType->data.LF_POINTER.attr.isvolatile)
							printf(" volatile");
						else if (referencedType->data.LF_POINTER.attr.isconst)
							printf(" const");

						printf(" %s\n", leafName);
					}
					break;
				case PDB::CodeView::TPI::TypeRecordKind::LF_BITFIELD:
					if (typeName)
					{
						printf("[0x%X]%s %s : %d\n",
							offset,
							typeName,
							leafName,
							referencedType->data.LF_BITFIELD.length);
					}
					else
					{
						modifierRecord = typeTable.GetTypeRecord(referencedType->data.LF_BITFIELD.type);
						if (!modifierRecord)
							break;

						printf("[0x%X]%s %s %s : %d\n",
							offset,
							GetModifierName(modifierRecord),
							GetTypeName(typeTable, modifierRecord->data.LF_MODIFIER.type, pointerLevel, nullptr, nullptr),
							leafName,
							referencedType->data.LF_BITFIELD.length);
					}
					break;
				case PDB::CodeView::TPI::TypeRecordKind::LF_ARRAY:
					if (!modifierRecord)
					{
						printf("[0x%X]%s %s[] /*0x%X*/\n",
							offset,
							typeName,
							leafName,
							*reinterpret_cast<const uint16_t*>(referencedType->data.LF_ARRAY.data));
					}
					else
					{
						printf("[0x%X]%s %s %s[] /*0x%X*/\n",
							offset,
							GetModifierName(modifierRecord),
							GetTypeName(typeTable, modifierRecord->data.LF_MODIFIER.type, pointerLevel, nullptr, nullptr),
							leafName,
							*reinterpret_cast<const uint16_t*>(referencedType->data.LF_ARRAY.data));
					}
					break;
				default:
					break;
				}
			}
			else
			{
				if (modifierRecord)
					printf("[0x%X]%s %s %s\n", offset, GetModifierName(modifierRecord), typeName, leafName);
				else
					printf("[0x%X]%s %s\n", offset, typeName, leafName);
			}
		}
		else if (fieldRecord->kind == PDB::CodeView::TPI::TypeRecordKind::LF_NESTTYPE)
		{
			leafName = &fieldRecord->data.LF_NESTTYPE.name[0];
			typeName = GetTypeName(typeTable, fieldRecord->data.LF_NESTTYPE.index, pointerLevel, &referencedType, &modifierRecord);

			printf("%s %s\n", typeName, leafName);
		}
		else if (fieldRecord->kind == PDB::CodeView::TPI::TypeRecordKind::LF_STMEMBER)
		{
			leafName = &fieldRecord->data.LF_STMEMBER.name[0];
			typeName = GetTypeName(typeTable, fieldRecord->data.LF_STMEMBER.index, pointerLevel, &referencedType, &modifierRecord);

			if (!modifierRecord)
				printf("%s %s\n", typeName, leafName);
			else
				printf("%s %s %s\n", GetModifierName(modifierRecord), typeName, leafName);
		}
		else if (fieldRecord->kind == PDB::CodeView::TPI::TypeRecordKind::LF_METHOD)
		{
			leafName = fieldRecord->data.LF_METHOD.name;

			auto methodList = typeTable.GetTypeRecord(fieldRecord->data.LF_METHOD.mList);
			if (!methodList)
				break;

			// https://github.com/microsoft/microsoft-pdb/blob/master/PDB/include/symtypeutils.h#L220
			size_t offsetInMethodList = 0;
			for (size_t j = 0; j < fieldRecord->data.LF_METHOD.count; j++)
			{
				size_t entrySize = 2 * sizeof(uint32_t);
				const PDB::CodeView::TPI::MethodListEntry* entry = (const PDB::CodeView::TPI::MethodListEntry*)(methodList->data.LF_METHODLIST.mList + offsetInMethodList);
				if (!GetMethodPrototype(typeTable, typeTable.GetTypeRecord(entry->index), functionPrototype))
					break;
				printf(functionPrototype.c_str(), leafName);
				printf("\n");
				PDB::CodeView::TPI::MethodProperty methodProp = (PDB::CodeView::TPI::MethodProperty)entry->attributes.mprop;
				if (methodProp == PDB::CodeView::TPI::MethodProperty::Intro || methodProp == PDB::CodeView::TPI::MethodProperty::PureIntro)
					entrySize += sizeof(uint32_t);
				offsetInMethodList += entrySize;
			}
		}
		else if (fieldRecord->kind == PDB::CodeView::TPI::TypeRecordKind::LF_ONEMETHOD)
		{
			leafName = GetMethodName(fieldRecord);

			referencedType = typeTable.GetTypeRecord(fieldRecord->data.LF_ONEMETHOD.index);
			if (!referencedType)
				break;

			if (!GetMethodPrototype(typeTable, referencedType, functionPrototype))
				break;

			printf(functionPrototype.c_str(), leafName);
			printf("\n");
		}
		else if (fieldRecord->kind == PDB::CodeView::TPI::TypeRecordKind::LF_BCLASS)
		{
			leafName = GetLeafName(fieldRecord->data.LF_BCLASS.offset, fieldRecord->data.LF_BCLASS.lfEasy.kind);

			i += static_cast<size_t>(leafName - reinterpret_cast<const char*>(fieldRecord));
			i = (i + (sizeof(uint32_t) - 1)) & (0 - sizeof(uint32_t));
			continue;
		}
		else if (fieldRecord->kind == PDB::CodeView::TPI::TypeRecordKind::LF_VBCLASS || fieldRecord->kind == PDB::CodeView::TPI::TypeRecordKind::LF_IVBCLASS)
		{
			// virtual base pointer offset from address point
			// followed by virtual base offset from vbtable

			const PDB::CodeView::TPI::TypeRecordKind vbpOffsetAddressPointKind = *(const PDB::CodeView::TPI::TypeRecordKind*)(fieldRecord->data.LF_IVBCLASS.vbpOffset);
			const uint8_t vbpOffsetAddressPointSize = GetLeafSize(vbpOffsetAddressPointKind);

			const  PDB::CodeView::TPI::TypeRecordKind vbpOffsetVBTableKind = *(const PDB::CodeView::TPI::TypeRecordKind*)(fieldRecord->data.LF_IVBCLASS.vbpOffset + vbpOffsetAddressPointSize);
			const uint8_t vbpOffsetVBTableSize = GetLeafSize(vbpOffsetVBTableKind);

			i += sizeof(PDB::CodeView::TPI::FieldList::Data::LF_VBCLASS);
			i += vbpOffsetAddressPointSize + vbpOffsetVBTableSize;
			i = (i + (sizeof(uint32_t) - 1)) & (0 - sizeof(uint32_t));
			continue;
		}
		else if (fieldRecord->kind == PDB::CodeView::TPI::TypeRecordKind::LF_INDEX)
		{
			i += sizeof(PDB::CodeView::TPI::FieldList::Data::LF_INDEX);
			i = (i + (sizeof(uint32_t) - 1)) & (0 - sizeof(uint32_t));
			continue;
		}
		else if (fieldRecord->kind == PDB::CodeView::TPI::TypeRecordKind::LF_VFUNCTAB)
		{
			i += sizeof(PDB::CodeView::TPI::FieldList::Data::LF_VFUNCTAB);
			i = (i + (sizeof(uint32_t) - 1)) & (0 - sizeof(uint32_t));
			continue;
		}
		else
		{
			break;
		}

		i += static_cast<size_t>(leafName - reinterpret_cast<const char*>(fieldRecord));
		i += strnlen(leafName, maximumSize - i - 1) + 1;
		i = (i + (sizeof(uint32_t) - 1)) & (0 - sizeof(uint32_t));
	}
}

// Used in ExamplesFunctionVariables
std::string GetTypeName(const TypeTable& typeTable, uint32_t typeIndex)
{
	uint8_t pointerLevel = 0;
	const PDB::CodeView::TPI::Record* referencedType = nullptr;
	const PDB::CodeView::TPI::Record* modifierRecord = nullptr;

	const char* typeName = GetTypeName(typeTable, typeIndex, pointerLevel, &referencedType, &modifierRecord);

	if (typeName == nullptr)
	{
		if (referencedType == nullptr && (typeIndex & 0x80000000) != 0)
		{
			// d3d12.pdb\1DEAE23C86E6462A86018FB180EB8E4A1, S_CALLSITE for `dynamic initializer for 'g_Telemetry'': typeIndex == 0x80900001
			char typeIndexBuf[0x0C];
			sprintf_s(typeIndexBuf, sizeof(typeIndexBuf), "%08X", typeIndex);
			return std::string("<BAD_TYPE_INDEX:0x") + typeIndexBuf + ">";
		}
		PDB_ASSERT(referencedType != nullptr, "Neither typeName nor referencedType are set.");

		if (referencedType->header.kind == PDB::CodeView::TPI::TypeRecordKind::LF_POINTER)
		{
			std::string pointerType = GetTypeName(typeTable, referencedType->data.LF_POINTER.utype);

			for (size_t i = 0; i < pointerLevel; i++)
				pointerType += '*';

			return pointerType;
		}
		else if (referencedType->header.kind == PDB::CodeView::TPI::TypeRecordKind::LF_ARRAY)
		{
			const std::string elementType = GetTypeName(typeTable, referencedType->data.LF_ARRAY.elemtype);
			const std::string indexType = GetTypeName(typeTable, referencedType->data.LF_ARRAY.idxtype);

			return elementType + "[" + indexType + "]";
		}
		else if (referencedType->header.kind == PDB::CodeView::TPI::TypeRecordKind::LF_PROCEDURE)
		{
			std::string functionPrototype;

			if (!GetFunctionPrototype(typeTable, referencedType, functionPrototype))
			{
				PDB_ASSERT(false, "Resolving function prototype failed");
				return "resolving function type failed";
			}

			return functionPrototype;
		}
		else if (referencedType->header.kind == PDB::CodeView::TPI::TypeRecordKind::LF_MFUNCTION)
		{
			std::string methodPrototype;

			if (!GetMethodPrototype(typeTable, referencedType, methodPrototype))
			{
				PDB_ASSERT(false, "Resolving method prototype failed");
				return "resolving method type failed";
			}

			std::string classTypeName = GetTypeName(typeTable, referencedType->data.LF_MFUNCTION.classtype);
			classTypeName += "::*";

			const int stringLength = std::snprintf(nullptr, 0, methodPrototype.c_str(), classTypeName.c_str());
			PDB_ASSERT(stringLength > 0, "String length %i <= 0", stringLength);

			std::vector<char> resultString(static_cast<size_t>(stringLength) + 1u);

			std::snprintf(&resultString[0], resultString.size(), methodPrototype.c_str(), classTypeName.c_str());

			return std::string(resultString.data());
		}
		else
		{
			PDB_ASSERT(false, "Unhandled referencedType kind 0x%X", static_cast<uint16_t>(referencedType->header.kind));
			return "not found";
		}
	}

	return typeName;
}

static void DisplayEnumerates(const PDB::CodeView::TPI::Record* record, uint8_t underlyingTypeSize)
{
	const char* leafName = nullptr;
	uint64_t value = 0;
	const char* valuePtr = nullptr;

	auto maximumSize = record->header.size - sizeof(uint16_t);

	for (size_t i = 0; i < maximumSize;)
	{
		auto fieldRecord = reinterpret_cast<const PDB::CodeView::TPI::FieldList*>(reinterpret_cast<const uint8_t*>(&record->data.LF_FIELD.list) + i);

		leafName = GetLeafName(fieldRecord->data.LF_ENUMERATE.value, fieldRecord->data.LF_ENUMERATE.lfEasy.kind);
		
		if (fieldRecord->data.LF_ENUMERATE.lfEasy.kind < PDB::CodeView::TPI::TypeRecordKind::LF_NUMERIC)
			valuePtr = &fieldRecord->data.LF_ENUMERATE.value[0];
		else
			valuePtr = &fieldRecord->data.LF_ENUMERATE.value[sizeof(PDB::CodeView::TPI::TypeRecordKind)];

		switch (underlyingTypeSize)
		{
		case 1:
			value = *reinterpret_cast<const uint8_t*>(&fieldRecord->data.LF_ENUMERATE.value[0]);
			break;
		case 2:
			value = *reinterpret_cast<const uint16_t*>(&fieldRecord->data.LF_ENUMERATE.value[0]);
			break;
		case 4:
			value = *reinterpret_cast<const uint32_t*>(&fieldRecord->data.LF_ENUMERATE.value[0]);
			break;
		case 8:
			value = *reinterpret_cast<const uint64_t*>(&fieldRecord->data.LF_ENUMERATE.value[0]);
			break;
		default:
			break;
		}

		printf("%s = %" PRIu64 "\n", leafName, value);

		i += static_cast<size_t>(leafName - reinterpret_cast<const char*>(fieldRecord));
		i += strnlen(leafName, maximumSize - i - 1) + 1;
		i = (i + (sizeof(uint32_t) - 1)) & (0 - sizeof(uint32_t));

		(void)valuePtr;
	}
}


void ExampleTypes(const PDB::TPIStream& tpiStream);
void ExampleTypes(const PDB::TPIStream& tpiStream)
{
	TimedScope total("\nRunning example \"Function types\"");

	TimedScope typeTableScope("Create TypeTable");
	TypeTable typeTable(tpiStream);
	typeTableScope.Done();

	for (const auto& record : typeTable.GetTypeRecords())
	{
		if ((record->header.kind == PDB::CodeView::TPI::TypeRecordKind::LF_CLASS) || (record->header.kind == PDB::CodeView::TPI::TypeRecordKind::LF_STRUCTURE))
		{
			if (record->data.LF_CLASS.property.fwdref)
				continue;

			auto typeRecord = typeTable.GetTypeRecord(record->data.LF_CLASS.field);
			if (!typeRecord)
				continue;

			auto leafName = GetLeafName(record->data.LF_CLASS.data, record->data.LF_CLASS.lfEasy.kind);

			printf("struct %s\n{\n", leafName);
			
			DisplayFields(typeTable, typeRecord);

			printf("}\n");
		}
		else if (record->header.kind == PDB::CodeView::TPI::TypeRecordKind::LF_UNION)
		{
			if (record->data.LF_UNION.property.fwdref)
				continue;

			auto typeRecord = typeTable.GetTypeRecord(record->data.LF_UNION.field);
			if (!typeRecord)
				continue;

			auto leafName = GetLeafName(record->data.LF_UNION.data, static_cast<PDB::CodeView::TPI::TypeRecordKind>(0));

			printf("union %s\n{\n", leafName);

			DisplayFields(typeTable, typeRecord);

			printf("}\n");
		}
		else if (record->header.kind == PDB::CodeView::TPI::TypeRecordKind::LF_ENUM)
		{
			if (record->data.LF_ENUM.property.fwdref)
				continue;

			auto typeRecord = typeTable.GetTypeRecord(record->data.LF_ENUM.field);
			if (!typeRecord)
				continue;

			printf("enum %s\n{\n", record->data.LF_ENUM.name);

			DisplayEnumerates(typeRecord, GetLeafSize(static_cast<PDB::CodeView::TPI::TypeRecordKind>(record->data.LF_ENUM.utype)));

			printf("}\n");
		}
	}

	total.Done(tpiStream.GetTypeRecordCount());
}

template<typename T>
static void TagRecursively(const TypeTable& typeTable, uint32_t typeIndex, T setName);

#define TAG_AND_CHECK(typeIndex) if (setName(typeIndex)) TagRecursively(typeTable, typeIndex, setName)

template<typename T>
static void TagChildren(const TypeTable& typeTable, const PDB::CodeView::TPI::Record* record, T setName)
{
	const char* leafName = nullptr;

	auto maximumSize = record->header.size - sizeof(uint16_t);

	for (size_t i = 0; i < maximumSize;)
	{
		auto fieldRecord = reinterpret_cast<const PDB::CodeView::TPI::FieldList*>(reinterpret_cast<const uint8_t*>(&record->data.LF_FIELD.list) + i);

		// these are all the record kinds I have observed
		PDB_ASSERT(
			fieldRecord->kind == PDB::CodeView::TPI::TypeRecordKind::LF_BCLASS ||
			fieldRecord->kind == PDB::CodeView::TPI::TypeRecordKind::LF_VBCLASS ||
			fieldRecord->kind == PDB::CodeView::TPI::TypeRecordKind::LF_IVBCLASS ||
			fieldRecord->kind == PDB::CodeView::TPI::TypeRecordKind::LF_INDEX ||
			fieldRecord->kind == PDB::CodeView::TPI::TypeRecordKind::LF_VFUNCTAB ||
			fieldRecord->kind == PDB::CodeView::TPI::TypeRecordKind::LF_NESTTYPE ||
			fieldRecord->kind == PDB::CodeView::TPI::TypeRecordKind::LF_ENUM ||
			fieldRecord->kind == PDB::CodeView::TPI::TypeRecordKind::LF_MEMBER ||
			fieldRecord->kind == PDB::CodeView::TPI::TypeRecordKind::LF_STMEMBER ||
			fieldRecord->kind == PDB::CodeView::TPI::TypeRecordKind::LF_METHOD ||
			fieldRecord->kind == PDB::CodeView::TPI::TypeRecordKind::LF_ONEMETHOD ||
			fieldRecord->kind == PDB::CodeView::TPI::TypeRecordKind::LF_ENUMERATE,
			"Unknown record kind %X",
			static_cast<unsigned int>(fieldRecord->kind));

		if (fieldRecord->kind == PDB::CodeView::TPI::TypeRecordKind::LF_MEMBER)
		{
			leafName = GetLeafName(fieldRecord->data.LF_MEMBER.offset, fieldRecord->data.LF_MEMBER.lfEasy.kind);
			TAG_AND_CHECK(fieldRecord->data.LF_MEMBER.index);
		}
		else if (fieldRecord->kind == PDB::CodeView::TPI::TypeRecordKind::LF_NESTTYPE)
		{
			leafName = &fieldRecord->data.LF_NESTTYPE.name[0];
			TAG_AND_CHECK(fieldRecord->data.LF_NESTTYPE.index);
		}
		else if (fieldRecord->kind == PDB::CodeView::TPI::TypeRecordKind::LF_STMEMBER)
		{
			leafName = &fieldRecord->data.LF_STMEMBER.name[0];
			TAG_AND_CHECK(fieldRecord->data.LF_STMEMBER.index);
		}
		else if (fieldRecord->kind == PDB::CodeView::TPI::TypeRecordKind::LF_METHOD)
		{
			leafName = fieldRecord->data.LF_METHOD.name;
			setName(fieldRecord->data.LF_METHOD.mList);

			auto methodList = typeTable.GetTypeRecord(fieldRecord->data.LF_METHOD.mList);
			if (!methodList)
				break;

			// https://github.com/microsoft/microsoft-pdb/blob/master/PDB/include/symtypeutils.h#L220
			size_t offsetInMethodList = 0;
			for (size_t j = 0; j < fieldRecord->data.LF_METHOD.count; j++)
			{
				size_t entrySize = sizeof(PDB::CodeView::TPI::MethodListEntry);
				const PDB::CodeView::TPI::MethodListEntry* entry = (const PDB::CodeView::TPI::MethodListEntry*)(methodList->data.LF_METHODLIST.mList + offsetInMethodList);
				TAG_AND_CHECK(entry->index);
				PDB::CodeView::TPI::MethodProperty methodProp = (PDB::CodeView::TPI::MethodProperty)entry->attributes.mprop;
				if (methodProp == PDB::CodeView::TPI::MethodProperty::Intro || methodProp == PDB::CodeView::TPI::MethodProperty::PureIntro)
					entrySize += sizeof(uint32_t);
				offsetInMethodList += entrySize;
			}
		}
		else if (fieldRecord->kind == PDB::CodeView::TPI::TypeRecordKind::LF_ONEMETHOD)
		{
			leafName = GetMethodName(fieldRecord);
			TAG_AND_CHECK(fieldRecord->data.LF_ONEMETHOD.index);
		}
		else if (fieldRecord->kind == PDB::CodeView::TPI::TypeRecordKind::LF_BCLASS)
		{
			leafName = GetLeafName(fieldRecord->data.LF_BCLASS.offset, fieldRecord->data.LF_BCLASS.lfEasy.kind);

			i += static_cast<size_t>(leafName - reinterpret_cast<const char*>(fieldRecord));
			i = (i + (sizeof(uint32_t) - 1)) & (0 - sizeof(uint32_t));
			continue;
		}
		else if (fieldRecord->kind == PDB::CodeView::TPI::TypeRecordKind::LF_VBCLASS || fieldRecord->kind == PDB::CodeView::TPI::TypeRecordKind::LF_IVBCLASS)
		{
			// virtual base pointer offset from address point
			// followed by virtual base offset from vbtable

			const PDB::CodeView::TPI::TypeRecordKind vbpOffsetAddressPointKind = *(const PDB::CodeView::TPI::TypeRecordKind*)(fieldRecord->data.LF_IVBCLASS.vbpOffset);
			const uint8_t vbpOffsetAddressPointSize = GetLeafSize(vbpOffsetAddressPointKind);

			const  PDB::CodeView::TPI::TypeRecordKind vbpOffsetVBTableKind = *(const PDB::CodeView::TPI::TypeRecordKind*)(fieldRecord->data.LF_IVBCLASS.vbpOffset + vbpOffsetAddressPointSize);
			const uint8_t vbpOffsetVBTableSize = GetLeafSize(vbpOffsetVBTableKind);

			TAG_AND_CHECK(fieldRecord->data.LF_VBCLASS.vbpIndex);

			i += sizeof(PDB::CodeView::TPI::FieldList::Data::LF_VBCLASS);
			i += vbpOffsetAddressPointSize + vbpOffsetVBTableSize;
			i = (i + (sizeof(uint32_t) - 1)) & (0 - sizeof(uint32_t));
			continue;
		}
		else if (fieldRecord->kind == PDB::CodeView::TPI::TypeRecordKind::LF_INDEX)
		{
			// this is continued elsewhere
			setName(fieldRecord->data.LF_INDEX.type);
			auto continued = typeTable.GetTypeRecord(fieldRecord->data.LF_INDEX.type);
			if (continued)
				TagChildren(typeTable, continued, setName);

			i += sizeof(PDB::CodeView::TPI::FieldList::Data::LF_INDEX);
			i = (i + (sizeof(uint32_t) - 1)) & (0 - sizeof(uint32_t));
			continue;
		}
		else if (fieldRecord->kind == PDB::CodeView::TPI::TypeRecordKind::LF_VFUNCTAB)
		{
			TAG_AND_CHECK(fieldRecord->data.LF_VFUNCTAB.type);
			i += sizeof(PDB::CodeView::TPI::FieldList::Data::LF_VFUNCTAB);
			i = (i + (sizeof(uint32_t) - 1)) & (0 - sizeof(uint32_t));
			continue;
		}
		else if (fieldRecord->kind == PDB::CodeView::TPI::TypeRecordKind::LF_ENUMERATE)
		{
			leafName = GetLeafName(fieldRecord->data.LF_ENUMERATE.value, fieldRecord->data.LF_ENUMERATE.lfEasy.kind);
		}
		else
		{
			break;
		}

		i += static_cast<size_t>(leafName - reinterpret_cast<const char*>(fieldRecord));
		i += strnlen(leafName, maximumSize - i - 1) + 1;
		i = (i + (sizeof(uint32_t) - 1)) & (0 - sizeof(uint32_t));
	}
}

template<typename T>
static void TagRecursively(const TypeTable& typeTable, uint32_t typeIndex, T setName)
{
	const PDB::CodeView::TPI::Record* record = typeTable.GetTypeRecord(typeIndex);
	if (!record)
		return;
	switch (record->header.kind)
	{
		case PDB::CodeView::TPI::TypeRecordKind::LF_ARRAY:
			TAG_AND_CHECK(record->data.LF_ARRAY.elemtype);
			TAG_AND_CHECK(record->data.LF_ARRAY.idxtype);
			break;
		case PDB::CodeView::TPI::TypeRecordKind::LF_POINTER:
			TAG_AND_CHECK(record->data.LF_POINTER.utype);
			break;
		case PDB::CodeView::TPI::TypeRecordKind::LF_MODIFIER:
			TAG_AND_CHECK(record->data.LF_MODIFIER.type);
			break;
		case PDB::CodeView::TPI::TypeRecordKind::LF_PROCEDURE:
			TAG_AND_CHECK(record->data.LF_PROCEDURE.rvtype);
			TAG_AND_CHECK(record->data.LF_PROCEDURE.arglist);
			break;
		case PDB::CodeView::TPI::TypeRecordKind::LF_ARGLIST:
		{
			size_t count = record->data.LF_ARGLIST.count;
			for (size_t i = 0; i < count; i++)
			{
				uint32_t type = record->data.LF_ARGLIST.arg[i];
				TAG_AND_CHECK(type);
			}
			break;
		}
		case PDB::CodeView::TPI::TypeRecordKind::LF_MFUNCTION:
			TAG_AND_CHECK(record->data.LF_MFUNCTION.rvtype);
			TAG_AND_CHECK(record->data.LF_MFUNCTION.arglist);
			TAG_AND_CHECK(record->data.LF_MFUNCTION.thistype);
			break;
		case PDB::CodeView::TPI::TypeRecordKind::LF_FIELDLIST:
			TagChildren(typeTable, record, setName);
			break;
		default:
			break;
	}
}

// This example takes a PDB's TPI stream and prints out a CSV file that contains all records in the TPI stream.
// You can use it to figure out what's taking up space in the stream.
//
// The format of the CSV is Size; Kind; Name. "Size" is the size of the record in bytes, "Kind" is the kind of
// the entry, and "Name" is a name associated with this entry.Type - definitions, member functions, and member
// lists use their type as the name. The idea is that you can bucket by "Name" to get actionable information
//and insight.
//
// The Name is set to "???" if no name was found, and it is set to "!!!" if multiple names reference the entry.
void ExampleTPISize(const PDB::TPIStream& tpiStream, const char* outPath);
void ExampleTPISize(const PDB::TPIStream& tpiStream, const char* outPath)
{
	TimedScope total("\nRunning example \"TPI Size\"");

	FILE* f;
#ifndef __unix
	fopen_s(&f, outPath, "w");
#else
	f = fopen(outPath, "w");
#endif
	PDB_ASSERT(f, "Failed to open %s for writing", outPath);

	fprintf(f, "Size;Kind;Name\n");

	TimedScope typeTableScope("Create TypeTable");
	TypeTable typeTable(tpiStream);
	typeTableScope.Done();

	std::vector<const char*> names;
	names.resize(typeTable.GetTypeRecords().GetLength());

	const size_t minIndex = typeTable.GetFirstTypeIndex();
	// sets the name of an entry and returns whether the name changed (because it wasn't set, or because we've found
	// conflicting information).
	auto setNameGlobal = [&names, minIndex](uint32_t typeIndex, const char* name) -> bool {
		if (!name || typeIndex < minIndex)
			return false;
		size_t idx = typeIndex - minIndex;
		const char* prev = names[idx];
		if (names[idx] == nullptr)
		{
			names[idx] = name;
			return true;
		}
		else
		{
			names[idx] = "!!!"; // multiple references
			return names[idx] != prev;
		}
	};

	// collect base types and propagate their name
	auto typeRecords = typeTable.GetTypeRecords();
	for (size_t i = 0, n = typeRecords.GetLength(); i < n; i++)
	{
		const PDB::CodeView::TPI::Record* record = typeRecords[i];
		PDB::CodeView::TPI::TypeRecordKind kind = record->header.kind;
		if (kind == PDB::CodeView::TPI::TypeRecordKind::LF_STRUCTURE)
		{
			names[i] = GetLeafName(record->data.LF_CLASS.data, record->data.LF_CLASS.lfEasy.kind);
			auto setName = [&setNameGlobal, names, i](uint32_t typeIndex) -> bool {
				return setNameGlobal(typeIndex, names[i]);
			};
			TAG_AND_CHECK(record->data.LF_CLASS.field);
		}
		else if (kind == PDB::CodeView::TPI::TypeRecordKind::LF_CLASS)
		{
			names[i] = GetLeafName(record->data.LF_CLASS.data, record->data.LF_CLASS.lfEasy.kind);
			auto setName = [&setNameGlobal, names, i](uint32_t typeIndex) -> bool {
				return setNameGlobal(typeIndex, names[i]);
			};
			TAG_AND_CHECK(record->data.LF_CLASS.field);
		}
		else if (kind == PDB::CodeView::TPI::TypeRecordKind::LF_UNION)
		{
			names[i] = GetLeafName(record->data.LF_UNION.data, static_cast<PDB::CodeView::TPI::TypeRecordKind>(0));
			auto setName = [&setNameGlobal, names, i](uint32_t typeIndex) -> bool {
				return setNameGlobal(typeIndex, names[i]);
			};
			TAG_AND_CHECK(record->data.LF_UNION.field);
		}
		else if (kind == PDB::CodeView::TPI::TypeRecordKind::LF_ENUM)
		{
			names[i] = record->data.LF_ENUM.name;
			auto setName = [&setNameGlobal, names, i](uint32_t typeIndex) -> bool {
				return setNameGlobal(typeIndex, names[i]);
			};
			TAG_AND_CHECK(record->data.LF_ENUM.field);
		}
		else if (kind == PDB::CodeView::TPI::TypeRecordKind::LF_MFUNCTION)
		{
			const char* name = names[i];
			if (!name)
			{
				const PDB::CodeView::TPI::Record* containingRecord = typeTable.GetTypeRecord((record->data.LF_MFUNCTION.classtype));
				if (containingRecord) {
					if (containingRecord->header.kind == PDB::CodeView::TPI::TypeRecordKind::LF_CLASS ||
						containingRecord->header.kind == PDB::CodeView::TPI::TypeRecordKind::LF_STRUCTURE)
						name = GetLeafName(containingRecord->data.LF_CLASS.data, containingRecord->data.LF_CLASS.lfEasy.kind);
					else if (containingRecord->header.kind == PDB::CodeView::TPI::TypeRecordKind::LF_UNION)
						name = GetLeafName(record->data.LF_UNION.data, static_cast<PDB::CodeView::TPI::TypeRecordKind>(0));
					else
						PDB_ASSERT(false, "unsupported");
				}
			}
			auto setName = [&setNameGlobal, name](uint32_t typeIndex) -> bool {
				return setNameGlobal(typeIndex, name);
			};
			uint32_t typeIndex = (uint32_t)(minIndex + i);
			TAG_AND_CHECK(typeIndex);
		}
	}

	for (size_t i = 0, n = typeRecords.GetLength(); i < n; i++)
	{
		const PDB::CodeView::TPI::Record* record = typeRecords[i];
		const char* kindName = nullptr;
		const char* typeName = i < names.size() ? names[i] : nullptr;
		switch (record->header.kind)
		{
			case PDB::CodeView::TPI::TypeRecordKind::LF_VTSHAPE: kindName = "LF_VTSHAPE;"; break;
			case PDB::CodeView::TPI::TypeRecordKind::LF_POINTER: kindName = "LF_POINTER;"; break;
			case PDB::CodeView::TPI::TypeRecordKind::LF_MODIFIER: kindName = "LF_MODIFIER;"; break;
			case PDB::CodeView::TPI::TypeRecordKind::LF_PROCEDURE: kindName = "LF_PROCEDURE;"; break;
			case PDB::CodeView::TPI::TypeRecordKind::LF_FIELDLIST: kindName = "LF_FIELDLIST;"; break;
			case PDB::CodeView::TPI::TypeRecordKind::LF_LABEL: kindName = "LF_LABEL;"; break;
			case PDB::CodeView::TPI::TypeRecordKind::LF_ARGLIST: kindName = "LF_ARGLIST;"; break;
			case PDB::CodeView::TPI::TypeRecordKind::LF_BITFIELD: kindName = "LF_BITFIELD;"; break;
			case PDB::CodeView::TPI::TypeRecordKind::LF_METHODLIST: kindName = "LF_METHODLIST;"; break;
			case PDB::CodeView::TPI::TypeRecordKind::LF_ARRAY: kindName = "LF_ARRAY;"; break;
			case PDB::CodeView::TPI::TypeRecordKind::LF_PRECOMP: kindName = "LF_PRECOMP;"; break;
			case PDB::CodeView::TPI::TypeRecordKind::LF_MFUNCTION: kindName = "LF_MFUNCTION;"; break;
			case PDB::CodeView::TPI::TypeRecordKind::LF_STRUCTURE: kindName = "LF_STRUCTURE;"; break;
			case PDB::CodeView::TPI::TypeRecordKind::LF_CLASS: kindName = "LF_CLASS;"; break;
			case PDB::CodeView::TPI::TypeRecordKind::LF_UNION: kindName = "LF_UNION;"; break;
			case PDB::CodeView::TPI::TypeRecordKind::LF_ENUM: kindName = "LF_ENUM;"; break;
			default: break;
		}

		fprintf(f, "%hu;", 2 + record->header.size);
		if (kindName)
			fprintf(f, "%s;", kindName);
		else
			fprintf(f, "0x%04X;", static_cast<uint16_t>(record->header.kind));

		if (typeName)
			fprintf(f, "%s\n", typeName);
		else
			fprintf(f, "???\n");
	}

	fclose(f);
	total.Done(tpiStream.GetTypeRecordCount());
}
#undef TAG_AND_CHECK
