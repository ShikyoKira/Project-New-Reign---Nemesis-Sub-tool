#ifndef HKBVARIABLEINFO_H_
#define HKBVARIABLEINFO_H_

#include <string>
#include <vector>
#include "Global-Type.h"
#include "src\hkx\hkbvariablevalue.h"

struct hkbvariableinfo
{
	enum variabletype
	{
		VARIABLE_TYPE_INVALID,
		VARIABLE_TYPE_BOOL,
		VARIABLE_TYPE_INT8,
		VARIABLE_TYPE_INT16,
		VARIABLE_TYPE_INT32,
		VARIABLE_TYPE_REAL,
		VARIABLE_TYPE_POINTER,
		VARIABLE_TYPE_VECTOR3,
		VARIABLE_TYPE_VECTOR4,
		VARIABLE_TYPE_QUATERNION
	};

	struct roleattribute
	{
		enum Role
		{
			ROLE_DEFAULT,
			ROLE_FILE_NAME,
			ROLE_BONE_INDEX,
			ROLE_BONE_INDEX_MAP,
			ROLE_EVENT_ID,
			ROLE_VARIABLE_INDEX,
			ROLE_ATTRIBUTE_INDEX,
			ROLE_TIME
		};

		struct roleflags
		{
			enum flags
			{
				FLAG_NONE = 0,
				FLAG_RAGDOLL = 1,
				FLAG_NORMALIZED = 2,
				FLAG_NOT_VARIABLE = 4,
				FLAG_HIDDEN = 8,
				FLAG_OUTPUT = 16,
				FLAG_NOT_CHARACTER_PROPERTY = 32,
			};

			flags data = FLAG_NONE;
			vecstr UNKNOWN_BITS;
			std::string strdata;

			std::string getflags();

			void update(std::string flag);
			bool operator==(roleflags& ctrpart);
			bool operator!=(roleflags& ctrpart);
		};

		Role role;
		roleflags flags;

		std::string getRole();
		void installRole(std::string n_role);

		roleattribute() {}
		roleattribute(std::string n_role);
	};

	roleattribute role;
	variabletype type;

	std::string getType();
	void typeUpdate(std::string data);

	hkbvariableinfo() {}
	hkbvariableinfo(std::string n_role) : role(n_role) {}
};

#endif
