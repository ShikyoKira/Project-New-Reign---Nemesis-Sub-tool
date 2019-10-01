#include "hkbvariableinfo.h"
#include "highestscore.h"

using namespace std;


string hkbvariableinfo::roleattribute::roleflags::getflags()
{
	string flags;

	if (FLAG_NOT_CHARACTER_PROPERTY) flags.append("FLAG_NOT_CHARACTER_PROPERTY|");
	if (FLAG_OUTPUT) flags.append("FLAG_OUTPUT|");
	if (FLAG_HIDDEN) flags.append("FLAG_HIDDEN|");
	if (FLAG_NOT_VARIABLE) flags.append("FLAG_NOT_VARIABLE|");
	if (FLAG_NORMALIZED) flags.append("FLAG_NORMALIZED|");
	if (FLAG_RAGDOLL) flags.append("FLAG_RAGDOLL|");
	if (FLAG_NONE) flags.append("FLAG_NONE|");

	if (UNKNOWN_BITS.size())
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
	if (flag == "FLAG_NONE") FLAG_NONE = true;
	else if (flag == "FLAG_RAGDOLL") FLAG_RAGDOLL = true;
	else if (flag == "FLAG_NORMALIZED") FLAG_NORMALIZED = true;
	else if (flag == "FLAG_NOT_VARIABLE") FLAG_NOT_VARIABLE = true;
	else if (flag == "FLAG_HIDDEN") FLAG_HIDDEN = true;
	else if (flag == "FLAG_OUTPUT") FLAG_OUTPUT = true;
	else if (flag == "FLAG_NOT_CHARACTER_PROPERTY") FLAG_NOT_CHARACTER_PROPERTY = true;
	else if (flag != "0") UNKNOWN_BITS.push_back(flag);
}

bool hkbvariableinfo::roleattribute::roleflags::operator==(roleflags& ctrpart)
{
	if (FLAG_NONE != ctrpart.FLAG_NONE) return false;
	else if (FLAG_RAGDOLL != ctrpart.FLAG_RAGDOLL) return false;
	else if (FLAG_NORMALIZED != ctrpart.FLAG_NORMALIZED) return false;
	else if (FLAG_NOT_VARIABLE != ctrpart.FLAG_NOT_VARIABLE) return false;
	else if (FLAG_HIDDEN != ctrpart.FLAG_HIDDEN) return false;
	else if (FLAG_OUTPUT != ctrpart.FLAG_OUTPUT) return false;
	else if (FLAG_NOT_CHARACTER_PROPERTY != ctrpart.FLAG_NOT_CHARACTER_PROPERTY) return false;
	else if (UNKNOWN_BITS != ctrpart.UNKNOWN_BITS) return false;

	return true;
}

bool hkbvariableinfo::roleattribute::roleflags::operator!=(roleflags& ctrpart)
{
	if (FLAG_NONE != ctrpart.FLAG_NONE) return true;
	else if (FLAG_RAGDOLL != ctrpart.FLAG_RAGDOLL) return true;
	else if (FLAG_NORMALIZED != ctrpart.FLAG_NORMALIZED) return true;
	else if (FLAG_NOT_VARIABLE != ctrpart.FLAG_NOT_VARIABLE) return true;
	else if (FLAG_HIDDEN != ctrpart.FLAG_HIDDEN) return true;
	else if (FLAG_OUTPUT != ctrpart.FLAG_OUTPUT) return true;
	else if (FLAG_NOT_CHARACTER_PROPERTY != ctrpart.FLAG_NOT_CHARACTER_PROPERTY) return true;
	else if (UNKNOWN_BITS != ctrpart.UNKNOWN_BITS) return true;

	return false;
}

string hkbvariableinfo::roleattribute::getRole()
{
	switch (role)
	{
		case ROLE_DEFAULT: return "ROLE_DEFAULT";
		case ROLE_FILE_NAME: return "ROLE_FILE_NAME";
		case ROLE_BONE_INDEX: return "ROLE_BONE_INDEX";
		case ROLE_BONE_INDEX_MAP: return "ROLE_BONE_INDEX_MAP";
		case ROLE_EVENT_ID: return "ROLE_EVENT_ID";
		case ROLE_VARIABLE_INDEX: return "ROLE_VARIABLE_INDEX";
		case ROLE_ATTRIBUTE_INDEX: return "ROLE_ATTRIBUTE_INDEX";
		case ROLE_TIME: return "ROLE_TIME";
		default: return "ROLE_DEFAULT";
	}
}

hkbvariableinfo::roleattribute::roleattribute(string n_role)
{
	installRole(n_role);
}

void hkbvariableinfo::roleattribute::installRole(std::string n_role)
{
	if (n_role == "ROLE_DEFAULT") role = ROLE_DEFAULT;
	else if (n_role == "ROLE_FILE_NAME") role = ROLE_FILE_NAME;
	else if (n_role == "ROLE_BONE_INDEX") role = ROLE_BONE_INDEX;
	else if (n_role == "ROLE_BONE_INDEX_MAP") role = ROLE_BONE_INDEX_MAP;
	else if (n_role == "ROLE_EVENT_ID") role = ROLE_EVENT_ID;
	else if (n_role == "ROLE_VARIABLE_INDEX") role = ROLE_VARIABLE_INDEX;
	else if (n_role == "ROLE_ATTRIBUTE_INDEX") role = ROLE_ATTRIBUTE_INDEX;
	else role = ROLE_TIME;
}

string hkbvariableinfo::getType()
{
	switch (type)
	{
		case VARIABLE_TYPE_INVALID: return "VARIABLE_TYPE_INVALID";
		case VARIABLE_TYPE_BOOL: return "VARIABLE_TYPE_BOOL";
		case VARIABLE_TYPE_INT8: return "VARIABLE_TYPE_INT8";
		case VARIABLE_TYPE_INT16: return "VARIABLE_TYPE_INT16";
		case VARIABLE_TYPE_INT32: return "VARIABLE_TYPE_INT32";
		case VARIABLE_TYPE_REAL: return "VARIABLE_TYPE_REAL";
		case VARIABLE_TYPE_POINTER: return "VARIABLE_TYPE_POINTER";
		case VARIABLE_TYPE_VECTOR3: return "VARIABLE_TYPE_VECTOR3";
		case VARIABLE_TYPE_VECTOR4: return "VARIABLE_TYPE_VECTOR4";
		case VARIABLE_TYPE_QUATERNION: return "VARIABLE_TYPE_QUATERNION";
		default: return "VARIABLE_TYPE_INVALID";
	}
}

void hkbvariableinfo::typeUpdate(string data)
{
	if (data == "VARIABLE_TYPE_INVALID") type = VARIABLE_TYPE_INVALID;
	else if (data == "VARIABLE_TYPE_BOOL") type = VARIABLE_TYPE_BOOL;
	else if (data == "VARIABLE_TYPE_INT8") type = VARIABLE_TYPE_INT8;
	else if (data == "VARIABLE_TYPE_INT16") type = VARIABLE_TYPE_INT16;
	else if (data == "VARIABLE_TYPE_INT32") type = VARIABLE_TYPE_INT32;
	else if (data == "VARIABLE_TYPE_REAL") type = VARIABLE_TYPE_REAL;
	else if (data == "VARIABLE_TYPE_POINTER") type = VARIABLE_TYPE_POINTER;
	else if (data == "VARIABLE_TYPE_VECTOR3") type = VARIABLE_TYPE_VECTOR3;
	else if (data == "VARIABLE_TYPE_VECTOR4") type = VARIABLE_TYPE_VECTOR4;
	else type = VARIABLE_TYPE_QUATERNION;
}
