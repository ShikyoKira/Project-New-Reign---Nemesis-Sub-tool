#include "blendingOldFunction.h"

using namespace std;

bool blendingOldFunction(string id, string address, int functionlayer)
{
	string addline = address;
	addline = addline.substr(0, addline.find_last_of(">"));
	addline = addline.substr(addline.find_last_of(">") + 1) += ">";
	addline = boost::regex_replace(string(addline), boost::regex("([(0-9]*)(j)([0-9]+)>"), string("\\1\\2\\3"));

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
