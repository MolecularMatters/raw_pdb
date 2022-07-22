#include "Examples_PCH.h"
#include "ExampleTimedScope.h"
#include "PDB_RawFile.h"
#include "Foundation/PDB_Assert.h"
#include "PDB_DBIStream.h"
#include "PDB_TPIStream.h"

#include <cstring>
#include <cinttypes>

#pragma warning(push)
#pragma warning(disable : 4061)
#pragma warning(disable : 4774)

uint8_t GetLeafSize(PDB::CodeView::TPI::TypeRecordKind kind)
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
		printf("Error! 0x%04x bogus type encountered, aborting...\n", (unsigned) kind);
	}
	return 0;
}

const char* GetLeafName(const char* data, PDB::CodeView::TPI::TypeRecordKind kind)
{
	return &data[GetLeafSize(kind)];
}

const char* GetTypeName(const PDB::TPIStream& tpiStream, uint32_t typeIndex, uint8_t& pointerLevel, const PDB::CodeView::TPI::Record** referencedType, const PDB::CodeView::TPI::Record** modifierRecord)
{
	const char* typeName = nullptr;
	const PDB::CodeView::TPI::Record* underlyingType = nullptr;

	if (referencedType)
		*referencedType = nullptr;

	if (modifierRecord)
		*modifierRecord = nullptr;

	auto typeIndexBegin = tpiStream.GetFirstTypeIndex();
	if (typeIndex < typeIndexBegin)
	{
		auto type = static_cast<PDB::CodeView::TPI::TypeIndexKind>(typeIndex);
		switch (type)
		{
		case PDB::CodeView::TPI::TypeIndexKind::HRESULT:
			return "HRESULT";
		case PDB::CodeView::TPI::TypeIndexKind::_32PHRESULT:
		case PDB::CodeView::TPI::TypeIndexKind::_64PHRESULT:
			return "PHRESULT";
		case PDB::CodeView::TPI::TypeIndexKind::_VOID:
			return "void";
		case PDB::CodeView::TPI::TypeIndexKind::_32PVOID:
		case PDB::CodeView::TPI::TypeIndexKind::_64PVOID:
		case PDB::CodeView::TPI::TypeIndexKind::PVOID:
			return "PVOID";

		case PDB::CodeView::TPI::TypeIndexKind::BOOL08:
		case PDB::CodeView::TPI::TypeIndexKind::BOOL16:
		case PDB::CodeView::TPI::TypeIndexKind::BOOL32:
			return "BOOL";
		case PDB::CodeView::TPI::TypeIndexKind::RCHAR:
		case PDB::CodeView::TPI::TypeIndexKind::CHAR:
			return "CHAR";
		case PDB::CodeView::TPI::TypeIndexKind::_32PRCHAR:
		case PDB::CodeView::TPI::TypeIndexKind::_32PCHAR:
		case PDB::CodeView::TPI::TypeIndexKind::_64PCHAR:
		case PDB::CodeView::TPI::TypeIndexKind::PRCHAR:
		case PDB::CodeView::TPI::TypeIndexKind::PCHAR:
			return "PCHAR";

		case PDB::CodeView::TPI::TypeIndexKind::UCHAR:
			return "UCHAR";
		case PDB::CodeView::TPI::TypeIndexKind::_32PUCHAR:
		case PDB::CodeView::TPI::TypeIndexKind::_64PUCHAR:
		case PDB::CodeView::TPI::TypeIndexKind::PUCHAR:
			return "PUCHAR";
		case PDB::CodeView::TPI::TypeIndexKind::WCHAR:
			return "WCHAR";
		case PDB::CodeView::TPI::TypeIndexKind::_32PWCHAR:
		case PDB::CodeView::TPI::TypeIndexKind::_64PWCHAR:
		case PDB::CodeView::TPI::TypeIndexKind::PWCHAR:
			return "PWCHAR";
		case PDB::CodeView::TPI::TypeIndexKind::SHORT:
			return "SHORT";
		case PDB::CodeView::TPI::TypeIndexKind::_32PSHORT:
		case PDB::CodeView::TPI::TypeIndexKind::_64PSHORT:
		case PDB::CodeView::TPI::TypeIndexKind::PSHORT:
			return "PSHORT";
		case PDB::CodeView::TPI::TypeIndexKind::USHORT:
			return "USHORT";
		case PDB::CodeView::TPI::TypeIndexKind::_32PUSHORT:
		case PDB::CodeView::TPI::TypeIndexKind::_64PUSHORT:
		case PDB::CodeView::TPI::TypeIndexKind::PUSHORT:
			return "PUSHORT";
		case PDB::CodeView::TPI::TypeIndexKind::LONG:
			return "LONG";
		case PDB::CodeView::TPI::TypeIndexKind::_32PLONG:
		case PDB::CodeView::TPI::TypeIndexKind::_64PLONG:
		case PDB::CodeView::TPI::TypeIndexKind::PLONG:
			return "PLONG";
		case PDB::CodeView::TPI::TypeIndexKind::ULONG:
			return "ULONG";
		case PDB::CodeView::TPI::TypeIndexKind::_32PULONG:
		case PDB::CodeView::TPI::TypeIndexKind::_64PULONG:
		case PDB::CodeView::TPI::TypeIndexKind::PULONG:
			return "PULONG";
		case PDB::CodeView::TPI::TypeIndexKind::REAL32:
			return "FLOAT";
		case PDB::CodeView::TPI::TypeIndexKind::_32PREAL32:
		case PDB::CodeView::TPI::TypeIndexKind::_64PREAL32:
		case PDB::CodeView::TPI::TypeIndexKind::PREAL32:
			return "PFLOAT";
		case PDB::CodeView::TPI::TypeIndexKind::REAL64:
			return "DOUBLE";
		case PDB::CodeView::TPI::TypeIndexKind::_32PREAL64:
		case PDB::CodeView::TPI::TypeIndexKind::_64PREAL64:
		case PDB::CodeView::TPI::TypeIndexKind::PREAL64:
			return "PDOUBLE";
		case PDB::CodeView::TPI::TypeIndexKind::QUAD:
			return "LONGLONG";
		case PDB::CodeView::TPI::TypeIndexKind::_32PQUAD:
		case PDB::CodeView::TPI::TypeIndexKind::_64PQUAD:
		case PDB::CodeView::TPI::TypeIndexKind::PQUAD:
			return "PLONGLONG";
		case PDB::CodeView::TPI::TypeIndexKind::UQUAD:
			return "ULONGLONG";
		case PDB::CodeView::TPI::TypeIndexKind::_32PUQUAD:
		case PDB::CodeView::TPI::TypeIndexKind::_64PUQUAD:
		case PDB::CodeView::TPI::TypeIndexKind::PUQUAD:
			return "PULONGLONG";
		case PDB::CodeView::TPI::TypeIndexKind::INT4:
			return "INT";
		case PDB::CodeView::TPI::TypeIndexKind::_32PINT4:
		case PDB::CodeView::TPI::TypeIndexKind::_64PINT4:
		case PDB::CodeView::TPI::TypeIndexKind::PINT4:
			return "PINT";
		case PDB::CodeView::TPI::TypeIndexKind::UINT4:
			return "UINT";
		case PDB::CodeView::TPI::TypeIndexKind::_32PUINT4:
		case PDB::CodeView::TPI::TypeIndexKind::_64PUINT4:
		case PDB::CodeView::TPI::TypeIndexKind::PUINT4:
			return "PUINT";
		default:
			break;
		}
	}
	else
	{
		auto typeRecord = tpiStream.GetTypeRecord(typeIndex);
		if (!typeRecord)
			return nullptr;

		switch (typeRecord->header.kind)
		{
		case PDB::CodeView::TPI::TypeRecordKind::LF_MODIFIER:
			*modifierRecord = typeRecord;
			return GetTypeName(tpiStream, typeRecord->data.LF_MODIFIER.type, pointerLevel, nullptr, nullptr);
		case PDB::CodeView::TPI::TypeRecordKind::LF_POINTER:
			++pointerLevel;
			*referencedType = typeRecord;
			if (typeRecord->data.LF_POINTER.utype >= typeIndexBegin)
			{
				underlyingType = tpiStream.GetTypeRecord(typeRecord->data.LF_POINTER.utype);
				if (!underlyingType)
					return nullptr;

				if(underlyingType->header.kind == PDB::CodeView::TPI::TypeRecordKind::LF_POINTER)
					return GetTypeName(tpiStream, typeRecord->data.LF_POINTER.utype, pointerLevel, referencedType, modifierRecord);
			}

			return GetTypeName(tpiStream, typeRecord->data.LF_POINTER.utype, pointerLevel, &typeRecord, modifierRecord);
		case PDB::CodeView::TPI::TypeRecordKind::LF_PROCEDURE:
			*referencedType = typeRecord;
			return nullptr;
		case PDB::CodeView::TPI::TypeRecordKind::LF_BITFIELD:
			if (typeRecord->data.LF_BITFIELD.type < typeIndexBegin)
			{
				typeName = GetTypeName(tpiStream, typeRecord->data.LF_BITFIELD.type, pointerLevel, nullptr, modifierRecord);
				*referencedType = typeRecord;
				return typeName;
			}
			else
			{
				*referencedType = typeRecord;
				return nullptr;
			}
		case PDB::CodeView::TPI::TypeRecordKind::LF_ARRAY:
			*referencedType = typeRecord;
			return GetTypeName(tpiStream, typeRecord->data.LF_ARRAY.elemtype, pointerLevel, &typeRecord, modifierRecord);
		case PDB::CodeView::TPI::TypeRecordKind::LF_CLASS:
		case PDB::CodeView::TPI::TypeRecordKind::LF_STRUCTURE:
			return GetLeafName(typeRecord->data.LF_CLASS.data, typeRecord->header.kind);
		case  PDB::CodeView::TPI::TypeRecordKind::LF_UNION:
			return GetLeafName(typeRecord->data.LF_UNION.data, typeRecord->header.kind);
		case PDB::CodeView::TPI::TypeRecordKind::LF_ENUM:
			return &typeRecord->data.LF_ENUM.name[0];
		default:
			break;
		}
	   
	}

	return "unknown_type";
}

const char* GetModifierName(const PDB::CodeView::TPI::Record* modifierRecord)
{
	if (modifierRecord->data.LF_MODIFIER.attr.MOD_const)
		return "const";
	else if (modifierRecord->data.LF_MODIFIER.attr.MOD_volatile)
		return "volatile";
	else if (modifierRecord->data.LF_MODIFIER.attr.MOD_volatile)
		return "unaligned";
	
	return "";
}

bool GetFunctionPrototype(const PDB::TPIStream& tpiStream, const PDB::CodeView::TPI::Record* functionRecord, std::string& functionPrototype)
{
	std::string underlyingTypePrototype;

	size_t markerPos = 0;
	uint8_t pointerLevel = 0;
	const PDB::CodeView::TPI::Record* referencedType = nullptr;
	const PDB::CodeView::TPI::Record* underlyingType = nullptr;
	const PDB::CodeView::TPI::Record* modifierRecord = nullptr;

	functionPrototype.clear();

	auto typeName = GetTypeName(tpiStream, functionRecord->data.LF_PROCEDURE.rvtype, pointerLevel, &referencedType, &modifierRecord);
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
		underlyingType = tpiStream.GetTypeRecord(referencedType->data.LF_POINTER.utype);
		if (!underlyingType)
			return false;

		if (!GetFunctionPrototype(tpiStream, underlyingType, underlyingTypePrototype))
			return false;

		markerPos = underlyingTypePrototype.find("%s");
		underlyingTypePrototype.erase(markerPos, 2);
		functionPrototype = underlyingTypePrototype;
	}

	functionPrototype += " (*%s)(";

	if (functionRecord->data.LF_PROCEDURE.parmcount)
	{
		auto argList = tpiStream.GetTypeRecord(functionRecord->data.LF_PROCEDURE.arglist);
		if (!argList)
			return false;

		for (size_t i = 0; i < argList->data.LF_ARGLIST.count; i++)
		{
			pointerLevel = 0;
			typeName = GetTypeName(tpiStream, argList->data.LF_ARGLIST.arg[i], pointerLevel, &referencedType, &modifierRecord);
			if (referencedType)
			{
				if (referencedType->data.LF_POINTER.utype >= tpiStream.GetFirstTypeIndex())
				{
					underlyingType = tpiStream.GetTypeRecord(referencedType->data.LF_POINTER.utype);
					if (!underlyingType)
						return false;
				}

				if (!underlyingType || underlyingType->header.kind != PDB::CodeView::TPI::TypeRecordKind::LF_PROCEDURE)
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
				else
				{
					if (!GetFunctionPrototype(tpiStream, underlyingType, underlyingTypePrototype))
						return false;

					markerPos = underlyingTypePrototype.find("%s");
					underlyingTypePrototype.erase(markerPos, 2);

					for (size_t j = 1; j < pointerLevel; j++)
						underlyingTypePrototype.insert(markerPos, 1, '*');

					functionPrototype += underlyingTypePrototype;
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

bool GetMethodPrototype(const PDB::TPIStream& tpiStream, const PDB::CodeView::TPI::Record* methodRecord, std::string& methodPrototype)
{
	std::string underlyingTypePrototype;

	size_t markerPos = 0;
	uint8_t pointerLevel = 0;
	const PDB::CodeView::TPI::Record* referencedType = nullptr;
	const PDB::CodeView::TPI::Record* underlyingType = nullptr;
	const PDB::CodeView::TPI::Record* modifierRecord = nullptr;

	methodPrototype.clear();

	auto typeName = GetTypeName(tpiStream, methodRecord->data.LF_MFUNCTION.rvtype, pointerLevel, &referencedType, &modifierRecord);
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
		underlyingType = tpiStream.GetTypeRecord(referencedType->data.LF_POINTER.utype);
		if (!underlyingType)
			return false;

		if (!GetFunctionPrototype(tpiStream, underlyingType, underlyingTypePrototype))
			return false;

		markerPos = underlyingTypePrototype.find("%s");
		underlyingTypePrototype.erase(markerPos, 2);
		methodPrototype = underlyingTypePrototype;
	}

	methodPrototype += " %s(";

	if (methodRecord->data.LF_MFUNCTION.parmcount)
	{
		auto argList = tpiStream.GetTypeRecord(methodRecord->data.LF_MFUNCTION.arglist);
		if (!argList)
			return false;

		for (size_t i = 0; i < argList->data.LF_ARGLIST.count; i++)
		{
			pointerLevel = 0;
			typeName = GetTypeName(tpiStream, argList->data.LF_ARGLIST.arg[i], pointerLevel, &referencedType, &modifierRecord);
			if (referencedType)
			{
				if (referencedType->data.LF_POINTER.utype >= tpiStream.GetFirstTypeIndex())
				{
					underlyingType = tpiStream.GetTypeRecord(referencedType->data.LF_POINTER.utype);
					if (!underlyingType)
						return false;
				}

				if (!underlyingType || underlyingType->header.kind != PDB::CodeView::TPI::TypeRecordKind::LF_PROCEDURE)
				{
					if (modifierRecord)
					{
						methodPrototype += GetModifierName(modifierRecord);
						methodPrototype += ' ';
					}

					methodPrototype += typeName;
					methodPrototype += '*';

					if (referencedType->data.LF_POINTER.attr.isvolatile)
						methodPrototype += "volatile";
					else if (referencedType->data.LF_POINTER.attr.isconst)
						methodPrototype += "const";
				}
				else
				{
					if (!GetFunctionPrototype(tpiStream, underlyingType, underlyingTypePrototype))
						return false;

					markerPos = underlyingTypePrototype.find("%s");
					underlyingTypePrototype.erase(markerPos, 2);

					for (size_t j = 1; j < pointerLevel; j++)
						underlyingTypePrototype.insert(markerPos, 1, '*');

					methodPrototype += underlyingTypePrototype;
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

const char* GetMethodName(const PDB::CodeView::TPI::FieldList* fieldRecord)
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

void DisplayFields(const PDB::TPIStream& tpiStream, const PDB::CodeView::TPI::Record* record)
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
			fieldRecord->kind == PDB::CodeView::TPI::TypeRecordKind::LF_VFUNCTAB ||
			fieldRecord->kind == PDB::CodeView::TPI::TypeRecordKind::LF_NESTTYPE ||
			fieldRecord->kind == PDB::CodeView::TPI::TypeRecordKind::LF_ENUM ||
			fieldRecord->kind == PDB::CodeView::TPI::TypeRecordKind::LF_MEMBER ||
			fieldRecord->kind == PDB::CodeView::TPI::TypeRecordKind::LF_STMEMBER ||
			fieldRecord->kind == PDB::CodeView::TPI::TypeRecordKind::LF_METHOD ||
			fieldRecord->kind == PDB::CodeView::TPI::TypeRecordKind::LF_ONEMETHOD,
			"Unknown record kind %X",
			fieldRecord->kind);

		if (fieldRecord->kind == PDB::CodeView::TPI::TypeRecordKind::LF_MEMBER)
		{
			if (fieldRecord->data.LF_MEMBER.lfEasy.kind < PDB::CodeView::TPI::TypeRecordKind::LF_NUMERIC)
				offset = *reinterpret_cast<const uint16_t*>(&fieldRecord->data.LF_MEMBER.offset[0]);
			else
				offset = *reinterpret_cast<const uint16_t*>(&fieldRecord->data.LF_MEMBER.offset[sizeof(PDB::CodeView::TPI::TypeRecordKind)]);

			leafName = GetLeafName(fieldRecord->data.LF_MEMBER.offset, fieldRecord->data.LF_MEMBER.lfEasy.kind);

			typeName = GetTypeName(tpiStream, fieldRecord->data.LF_MEMBER.index, pointerLevel, &referencedType, &modifierRecord);
			if (referencedType)
			{
				switch (referencedType->header.kind)
				{
				case PDB::CodeView::TPI::TypeRecordKind::LF_POINTER:
					if (referencedType->data.LF_POINTER.utype >= tpiStream.GetFirstTypeIndex())
					{
						underlyingType = tpiStream.GetTypeRecord(referencedType->data.LF_POINTER.utype);
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
							if (!GetFunctionPrototype(tpiStream, underlyingType, functionPrototype))
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
						modifierRecord = tpiStream.GetTypeRecord(referencedType->data.LF_BITFIELD.type);
						if (!modifierRecord)
							break;

						printf("[0x%X]%s %s %s : %d\n",
							offset,
							GetModifierName(modifierRecord),
							GetTypeName(tpiStream, modifierRecord->data.LF_MODIFIER.type, pointerLevel, nullptr, nullptr),
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
							GetTypeName(tpiStream, modifierRecord->data.LF_MODIFIER.type, pointerLevel, nullptr, nullptr),
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
			typeName = GetTypeName(tpiStream, fieldRecord->data.LF_NESTTYPE.index, pointerLevel, &referencedType, &modifierRecord);

			printf("%s %s\n", typeName, leafName);
		}
		else if (fieldRecord->kind == PDB::CodeView::TPI::TypeRecordKind::LF_STMEMBER)
		{
			leafName = &fieldRecord->data.LF_STMEMBER.name[0];
			typeName = GetTypeName(tpiStream, fieldRecord->data.LF_STMEMBER.index, pointerLevel, &referencedType, &modifierRecord);

			if (!modifierRecord)
				printf("%s %s\n", typeName, leafName);
			else
				printf("%s %s %s\n", GetModifierName(modifierRecord), typeName, leafName);
		}
		else if (fieldRecord->kind == PDB::CodeView::TPI::TypeRecordKind::LF_METHOD)
		{
			leafName = GetMethodName(fieldRecord);

			auto methodList = tpiStream.GetTypeRecord(fieldRecord->data.LF_METHOD.mList);
			if (!methodList)
				break;

			for (size_t j = 0; j < fieldRecord->data.LF_METHOD.count; j++)
			{
				if (methodList->data.LF_METHODLIST.mList[j] < tpiStream.GetFirstTypeIndex())
					continue;

				if (!GetMethodPrototype(tpiStream, tpiStream.GetTypeRecord(methodList->data.LF_METHODLIST.mList[j]), functionPrototype))
					break;

				printf(functionPrototype.c_str(), leafName);
				printf("\n");
			}
		}
		else if (fieldRecord->kind == PDB::CodeView::TPI::TypeRecordKind::LF_ONEMETHOD)
		{
			leafName = GetMethodName(fieldRecord);

			referencedType = tpiStream.GetTypeRecord(fieldRecord->data.LF_ONEMETHOD.index);
			if (!referencedType)
				break;

			if (!GetMethodPrototype(tpiStream, referencedType, functionPrototype))
				break;

			printf(functionPrototype.c_str(), leafName);
			printf("\n");
		}
		else if (fieldRecord->kind == PDB::CodeView::TPI::TypeRecordKind::LF_BCLASS)
		{
			leafName = GetLeafName(fieldRecord->data.LF_BCLASS.offset, fieldRecord->data.LF_BCLASS.lfEasy.kind);

			i += (leafName - reinterpret_cast<const char*>(fieldRecord));
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

		i += (leafName - reinterpret_cast<const char*>(fieldRecord));
		i += strnlen(leafName, maximumSize - i - 1) + 1;
		i = (i + (sizeof(uint32_t) - 1)) & (0 - sizeof(uint32_t));
	}
}

void DisplayEnumerates(const PDB::CodeView::TPI::Record* record, uint8_t underlyingTypeSize)
{
	const char* leafName = nullptr;
	uint64_t value = 0;
	const char* valuePtr = nullptr;

	auto maximumSize = record->header.size - sizeof(uint16_t);

	for (size_t i = 0; i < maximumSize;)
	{
		auto fieldRecord = reinterpret_cast<const PDB::CodeView::TPI::FieldList*>(reinterpret_cast<const uint8_t*>(&record->data.LF_FIELD.list) + i);

		leafName = GetLeafName(fieldRecord->data.LF_ENUMERATE.value, static_cast<PDB::CodeView::TPI::TypeRecordKind>(0u));
		
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
		default:
			break;
		}

		printf("%s = %" PRIu64 "\n", leafName, value);

		i += (leafName - reinterpret_cast<const char*>(fieldRecord));
		i += strnlen(leafName, maximumSize - i - 1) + 1;
		i = (i + (sizeof(uint32_t) - 1)) & (0 - sizeof(uint32_t));
	}
}

void ExampleTypes(const PDB::TPIStream& tpiStream)
{
	for (const auto& record : tpiStream.GetTypeRecords())
	{
		if (record->header.kind == PDB::CodeView::TPI::TypeRecordKind::LF_STRUCTURE)
		{
			if (record->data.LF_CLASS.property.fwdref)
				continue;

			auto typeRecord = tpiStream.GetTypeRecord(record->data.LF_CLASS.field);
			if (!typeRecord)
				continue;

			auto leafName = GetLeafName(record->data.LF_CLASS.data, record->data.LF_CLASS.lfEasy.kind);

			printf("struct %s\n{\n", leafName);
			
			DisplayFields(tpiStream, typeRecord);

			printf("}\n");
		}
		else if (record->header.kind == PDB::CodeView::TPI::TypeRecordKind::LF_UNION)
		{
			if (record->data.LF_UNION.property.fwdref)
				continue;

			auto typeRecord = tpiStream.GetTypeRecord(record->data.LF_UNION.field);
			if (!typeRecord)
				continue;

			auto leafName = GetLeafName(record->data.LF_UNION.data, static_cast<PDB::CodeView::TPI::TypeRecordKind>(0));

			printf("union %s\n{\n", leafName);

			DisplayFields(tpiStream, typeRecord);

			printf("}\n");
		}
		else if (record->header.kind == PDB::CodeView::TPI::TypeRecordKind::LF_ENUM)
		{
			if (record->data.LF_ENUM.property.fwdref)
				continue;

			auto typeRecord = tpiStream.GetTypeRecord(record->data.LF_ENUM.field);
			if (!typeRecord)
				continue;

			printf("enum %s\n{\n", record->data.LF_ENUM.name);

			DisplayEnumerates(typeRecord, GetLeafSize(static_cast<PDB::CodeView::TPI::TypeRecordKind>(record->data.LF_ENUM.utype)));

			printf("}\n");
		}
	}
}
#pragma warning(pop)
