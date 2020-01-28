#include "hkbvariableinfo.h"
#include "highestscore.h"
#include "src/utilities/hkMap.h"

using namespace std;

hkMap<string, hkbvariableinfo::roleattribute::Role> roleMap =
{
	{ "ROLE_DEFAULT", hkbvariableinfo::roleattribute::ROLE_DEFAULT },
	{ "ROLE_FILE_NAME", hkbvariableinfo::roleattribute::ROLE_FILE_NAME },
	{ "ROLE_BONE_INDEX", hkbvariableinfo::roleattribute::ROLE_BONE_INDEX },
	{ "ROLE_BONE_INDEX_MAP", hkbvariableinfo::roleattribute::ROLE_BONE_INDEX_MAP },
	{ "ROLE_EVENT_ID", hkbvariableinfo::roleattribute::ROLE_EVENT_ID },
	{ "ROLE_VARIABLE_INDEX", hkbvariableinfo::roleattribute::ROLE_VARIABLE_INDEX },
	{ "ROLE_ATTRIBUTE_INDEX", hkbvariableinfo::roleattribute::ROLE_ATTRIBUTE_INDEX },
	{ "ROLE_TIME", hkbvariableinfo::roleattribute::ROLE_TIME },
};

hkMap<string, hkbvariableinfo::variabletype> typeMap =
{
	{ "VARIABLE_TYPE_INVALID", hkbvariableinfo::VARIABLE_TYPE_INVALID },
	{ "VARIABLE_TYPE_BOOL", hkbvariableinfo::VARIABLE_TYPE_BOOL },
	{ "VARIABLE_TYPE_INT8", hkbvariableinfo::VARIABLE_TYPE_INT8 },
	{ "VARIABLE_TYPE_INT16", hkbvariableinfo::VARIABLE_TYPE_INT16 },
	{ "VARIABLE_TYPE_INT32", hkbvariableinfo::VARIABLE_TYPE_INT32 },
	{ "VARIABLE_TYPE_REAL", hkbvariableinfo::VARIABLE_TYPE_REAL },
	{ "VARIABLE_TYPE_POINTER", hkbvariableinfo::VARIABLE_TYPE_POINTER },
	{ "VARIABLE_TYPE_VECTOR3", hkbvariableinfo::VARIABLE_TYPE_VECTOR3 },
	{ "VARIABLE_TYPE_VECTOR4", hkbvariableinfo::VARIABLE_TYPE_VECTOR4 },
	{ "VARIABLE_TYPE_QUATERNION", hkbvariableinfo::VARIABLE_TYPE_QUATERNION },
};

hkMap<string, hkbvariableinfo::roleattribute::roleflags::flags> flagMap =
{
	{ "FLAG_NOT_CHARACTER_PROPERTY", hkbvariableinfo::roleattribute::roleflags::FLAG_NOT_CHARACTER_PROPERTY },
	{ "FLAG_OUTPUT", hkbvariableinfo::roleattribute::roleflags::FLAG_OUTPUT },
	{ "FLAG_HIDDEN", hkbvariableinfo::roleattribute::roleflags::FLAG_HIDDEN },
	{ "FLAG_NOT_VARIABLE", hkbvariableinfo::roleattribute::roleflags::FLAG_NOT_VARIABLE },
	{ "FLAG_NORMALIZED", hkbvariableinfo::roleattribute::roleflags::FLAG_NORMALIZED },
	{ "FLAG_RAGDOLL", hkbvariableinfo::roleattribute::roleflags::FLAG_RAGDOLL },
	{ "FLAG_NONE", hkbvariableinfo::roleattribute::roleflags::FLAG_NONE },
};

string hkbvariableinfo::roleattribute::roleflags::getflags()
{
	string flags;

	for (auto& curFlag : flagMap)
	{
		if (data & ~curFlag.second)
		{
			flags.append(curFlag.first + "|");
		}
	}

	if (UNKNOWN_BITS.size() > 0)
	{
		for (auto& bits : UNKNOWN_BITS)
		{
			flags.append(bits + "|");
		}
	}

	if (flags.length() == 0) return "0";

	flags.pop_back();
	return flags;
}

void hkbvariableinfo::roleattribute::roleflags::update(string flag)
{
	usize data2 = static_cast<usize>(data);

	if (flag == "FLAG_NONE") data2 |= FLAG_NONE;
	else if (flag == "FLAG_RAGDOLL") data2 |= FLAG_RAGDOLL;
	else if (flag == "FLAG_NORMALIZED") data2 |= FLAG_NORMALIZED;
	else if (flag == "FLAG_NOT_VARIABLE") data2 |= FLAG_NOT_VARIABLE;
	else if (flag == "FLAG_HIDDEN") data2 |= FLAG_HIDDEN;
	else if (flag == "FLAG_OUTPUT") data2 |= FLAG_OUTPUT;
	else if (flag == "FLAG_NOT_CHARACTER_PROPERTY") data2 |= FLAG_NOT_CHARACTER_PROPERTY;
	else if (flag != "0") UNKNOWN_BITS.push_back(flag);

	data = static_cast<flags>(data2);
}

bool hkbvariableinfo::roleattribute::roleflags::operator==(roleflags& ctrpart)
{
	return data == ctrpart.data;
}

bool hkbvariableinfo::roleattribute::roleflags::operator!=(roleflags& ctrpart)
{
	return data != ctrpart.data;
}

string hkbvariableinfo::roleattribute::getRole()
{
	return roleMap[role];
}

hkbvariableinfo::roleattribute::roleattribute(string n_role)
{
	installRole(n_role);
}

void hkbvariableinfo::roleattribute::installRole(std::string n_role)
{
	role = roleMap[n_role];
}

string hkbvariableinfo::getType()
{
	return typeMap[type];
}

void hkbvariableinfo::typeUpdate(string data)
{
	type = typeMap[data];
}
