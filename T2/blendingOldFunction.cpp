#include "blendingOldFunction.h"
#include "src\hkx\generator\statemachine\hkbstatemachinetransitioninfoarray.h"

using namespace std;

bool blendingOldFunction(shared_ptr<hkbobject> hkb_obj, string address, int functionlayer)
{
	string addline = address;
	addline = addline.substr(0, addline.find_last_of(">"));
	addline = addline.substr(addline.find_last_of(">") + 1) + ">";
	addline = boost::regex_replace(string(addline), boost::regex("([(0-9]*j[0-9]+)>"), string("\\1"));

	if (addline.back() != '>')
	{
		auto obj_itr = hkb_parent.find(hkb_obj);

		if (obj_itr != hkb_parent.end() && IsForeign.find(obj_itr->second->ID) == IsForeign.end())
		{
			if (IsOldFunction("ZeroRegion", hkb_obj, address))
			{
				addressChange[address] = address;
				return true;
			}

			string tempparent = obj_itr->second->ID;
			auto trans_itr = hkbstatemachinetransitioninfoarrayList_E.find(tempparent);

			if (trans_itr == hkbstatemachinetransitioninfoarrayList_E.end())
			{
				cout << "ERROR: Invalid behavior format detected (ID 1: " <<  hkb_obj << ", ID 2: " << tempparent << ")" << endl;
				Error = true;
				return false;
			}

			size_t numelement = trans_itr->second->transitions.size();
			string flayer = "j" + to_string(functionlayer);

			for (size_t i = 0; i < numelement; i++)
			{
				string tempadd = address;
				tempadd.replace(address.find_last_of(addline) - addline.length() + 1, addline.length(), to_string(i) + flayer);
				string oriID = addressID[tempadd];

				if (IsExist.find(oriID) == IsExist.end() && IsOldFunction("ZeroRegion", hkb_obj, tempadd))
				{
					addressChange[address] = tempadd;
					return true;
				}
			}
		}
	}

	return false;
}

bool blendingOldFunction(string id, string address, int functionlayer)
{
	string addline = address;
	addline = addline.substr(0, addline.find_last_of(">"));
	addline = addline.substr(addline.find_last_of(">") + 1) + ">";
	addline = boost::regex_replace(string(addline), boost::regex("([(0-9]*j[0-9]+)>"), string("\\1"));

	if (addline.back() != '>')
	{
		string tempparent = parent[id];

		if (!IsForeign[tempparent])
		{
			if (!exchangeID[tempparent].empty())
			{
				tempparent = exchangeID[tempparent];
			}

			if (IsOldFunction("ZeroRegion", id, address))
			{
				addressChange[address] = address;
				return true;
			}

			int element = elements[tempparent + "T"];
			string flayer = "j" + to_string(functionlayer);

			for (int i = 0; i < element; i++)
			{
				string tempadd = address;
				tempadd.replace(address.find_last_of(addline) - addline.length() + 1, addline.length(), to_string(i) + flayer);
				string oriID = addressID[tempadd];

				if (!IsExist[oriID] && IsOldFunction("ZeroRegion", id, tempadd))
				{
					addressChange[address] = tempadd;
					return true;
				}
			}
		}
	}

	return false;
}
