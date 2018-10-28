#include "Global.h"

using namespace std;

typedef unordered_map<string, string> mapstring;

bool Error = false;
bool Debug = false;
set<int> idcount;
nodelist FunctionLineNew;		// function new data
nodelist FunctionLineTemp;		// function temp data
nodelist FunctionLineOriginal;	// function original data
nodelist FunctionLineEdited;	// function edited data
unordered_map<string, int> regioncount;
unordered_map<string, int> elements;
unordered_map<string, bool> IsForeign;
unordered_map<string, bool> IsExist;
unordered_map<string, bool> IsFileInUse;
unordered_map<string, bool> IsBranchOrigin;
unordered_map<string, bool> PerfectMatchCondition;
unordered_map<string, string> parent;
unordered_map<string, string> region;
unordered_map<string, string> newID;
unordered_map<string, string> eventID;
unordered_map<string, string> variableID;
unordered_map<string, string> attributeID;
unordered_map<string, string> characterID;
unordered_map<string, string> addressID;
unordered_map<string, string> exchangeID;
unordered_map<string, string> addressChange;				// temp address change check
unordered_map<string, vector<string>> referencingIDs;		// which function use this ID
vector<shared_ptr<AnimDataProject>> AnimDataOriginal;
vector<shared_ptr<AnimDataProject>> AnimDataEdited;
vector<shared_ptr<AnimSetDataProject>> AnimSetDataOriginal;
vector<shared_ptr<AnimSetDataProject>> AnimSetDataEdited;
string modcode = "null";
string targetfilename;
string targetfilenameedited;
string shortFileName;
string shortFileNameEdited;
int functioncount = 0;
unsigned int eventCount;
unsigned int varCount;

bool IsOldRegion(string id, string address, bool special = false);
bool IsOldFunctionExt(string filename, string id, string address, bool condition = false);
bool blendingOldFunction(string id, string address, int functionlayer);

inline int sameWordCount(string line, string word)
{
	size_t nextWord = 0;
	int wordCount = 0;

	while (true)
	{
		nextWord = line.find(word, nextWord + 1);

		if (nextWord != -1)
		{
			wordCount++;
		}
		else
		{
			break;
		}
	}

	return wordCount;
}

inline bool IsFileExist(const string& name)
{
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

bool IsOldFunction(string filename, string id, string address)
{
	if (IsOldFunctionExt(filename, id, address))
	{
		if (IsForeign[parent[id]])
		{
			string tempparent = parent[id];

			while (IsForeign[parent[tempparent]])
			{
				tempparent = parent[tempparent];
			}

			IsBranchOrigin[tempparent] = true;
		}

		return true;
	}

	return false;
}

bool IsOldFunctionExt(string filename, string id, string address, bool condition)
{
	string tempadd = address.substr(address.find_last_of("="));

	if (((address.find("(e", 0) != string::npos) || (address.find("(m", 0) != string::npos) || (address.find("(cj", 0) != string::npos) || (address.find("(r", 0) != string::npos) || (address.find("(y", 0) != string::npos)) && (count(tempadd.begin(), tempadd.end(), '>') == 2) && (filename != "ZeroRegion"))
	{
		return IsOldRegion(id, address);
	}
	else if ((address.find("(i", 0) != string::npos) && (count(tempadd.begin(), tempadd.end(), '>') == 3) && (filename != "ZeroRegion"))
	{
		return IsOldRegion(id, address, true);
	}

	if (addressID[address] != "")
	{
		string tempOldID = addressID[address];

		if (!condition && IsExist[tempOldID])
		{
			if (exchangeID[id] == tempOldID || (tempOldID == id && !IsForeign[id]))
			{
				return true;
			}

			for (auto& ID : exchangeID)
			{
				if (tempOldID == ID.second)
				{
					return false;
				}
			}
		}

		if (tempOldID == id)
		{
			return true;
		}

		// stage 1
		// read function data for comparison
		bool IsName = false;
		bool check = false;
		string line;
		string searchname;
		string searchname2;
		string tempID = id;

		if ((address.find("(i", 0) != string::npos) || (count(tempadd.begin(), tempadd.end(), '>') == 3))
		{
			for (unordered_map<string, string>::iterator it = exchangeID.begin(); it != exchangeID.end(); it++)
			{
				if (it->second == id)
				{
					tempID = it->first;
					break;
				}
			}
		}

		if (!FunctionLineEdited[tempID].empty())
		{
			usize size = FunctionLineEdited[tempID].size();

			for (usize i = 0; i < size; ++i)
			{
				line = FunctionLineEdited[tempID][i];

				if (line.find("name=\"name\">", 0) != string::npos)
				{
					searchname = line;
					IsName = true;
					break;
				}
				else if (line.find("name=\"weight\">", 0) != string::npos)
				{
					searchname = line;
				}
				else if (line.find("name=\"worldFromModelWeight\">", 0) != string::npos)
				{
					searchname2 = line;
					break;
				}
				else if (line.find("name=\"memberPath\">", 0) != string::npos)
				{
					searchname = line;
				}
				else if (line.find("name=\"variableIndex\">", 0) != string::npos)
				{
					searchname2 = line;
					break;
				}
				else if (condition && line.find("name=\"expression\">", 0) != string::npos)
				{
					searchname = line;
					break;
				}
			}
		}
		else
		{
			cout << "ERROR: IsOldFunction Input (File: " << filename << ", ID: " << tempID << ")" << endl;
			Error = true;
			return false;
		}

		// comparing data to determine the function with the old add is new or old (solving same add paradox)
		if (FunctionLineTemp[tempOldID].size() > 0)
		{
			for (unsigned int i = 0; i < FunctionLineTemp[tempOldID].size(); ++i)
			{
				line = FunctionLineTemp[tempOldID][i];

				if (IsName)
				{
					if (line.find(searchname, 0) != string::npos)
					{
						ReferenceReplacementExt(id, tempOldID);
						return true;
					}
				}
				else if (condition)
				{
					if (line.find(searchname, 0) != string::npos)
					{
						return true;
					}
				}
				else
				{
					if (line.find(searchname, 0) != string::npos)
					{
						ReferenceReplacementExt(id, tempOldID);
						return true;
					}
				}

			}
		}
		else
		{
			cout << "ERROR: IsOldFunction Input (Old ID: " << tempOldID << ")" << endl;
			Error = true;
		}
	}

	return false;
}

inline string ExePath()
{
	string path = boost::filesystem::current_path().string();
	string::size_type pos = path.find_last_of("\\/");
	return path.substr(0, pos);
}

string CrossReferencing(string id, string address, int functionlayer, bool compare, bool special) // whether it is foreign principle or usual cross referencing
{
	if (compare) // foreign principle 
	{
		string tempadd = address.substr(address.find_last_of("="));
		bool pass = false;
		string newID;

		if ((address.find("(i", 0) == string::npos) || (count(tempadd.begin(), tempadd.end(), '>') != 3) || (IsForeign[parent[id]]))
		{
			for (auto it = exchangeID.begin(); it != exchangeID.end(); it++) // is ID already old
			{
				if (it->second == id)
				{
					IsForeign[id] = false;
					return id;
				}
			}
		}

		if (tempadd.find("l") != string::npos || tempadd.find("ck") != string::npos)
		{
			string addline = address;
			addline.pop_back();
			addline = addline.substr(addline.find_last_of(">") + 1) += ">";
			bool ck = false;

			if (tempadd.find("l") != string::npos)
			{
				addline = boost::regex_replace(string(addline), boost::regex("([(0-9]+)(l)([0-9]+)>"), string("\\1\\2\\3"));
			}
			else
			{
				addline = boost::regex_replace(string(addline), boost::regex("([(0-9]+)(ck)([0-9]+)>"), string("\\1\\2\\3"));
				ck = true;
			}

			if (addline.back() == '>')
			{
				if (special && !IsForeign[parent[id]])
				{
					pass = true;
				}
			}
			else
			{
				newID = conditionOldFunction(id, address, functionlayer, addline, ck);

				if (PerfectMatchCondition[id])
				{
					return id;
				}

				if (newID.length() > 0)
				{
					if (special && !IsForeign[parent[id]])
					{
						PerfectMatchCondition[newID] = true;
						pass = true;
					}
					else if (newID != id)
					{
						if (addressChange.find(address) == addressChange.end())
						{
							addressChange[address] = address;
						}

						return newID;
					}
				}
			}
		}
		else
		{
			if (special && !IsForeign[parent[id]])
			{
				pass = true;
			}
		}

		if (!pass && address.find("j") != string::npos)
		{
			if (blendingOldFunction(id, address, functionlayer))
			{
				if (addressChange.find(address) != addressChange.end())
				{
					auto& iter = addressChange.find(address);
					string oriID = addressID[addressChange[address]];
					addressChange.erase(iter);
					IsForeign[id] = false;

					if (oriID != id)
					{
						exchangeID[id] = oriID;
						ReferenceReplacement(id, oriID);
					}

					return oriID;
				}
			}
		}

		if (pass || IsOldFunction(("new/" + id + ".txt"), id, address)) // old function confirmation
		{
			IsForeign[id] = false;
			string oriID = addressID[address];

			if (oriID.length() < 3)
			{
				IsForeign[id] = true;
				return id;
			}

			if (id == "#90184")
			{
				string parentID = parent[id];
				id = id;
			}

			if (oriID != id)
			{
				exchangeID[id] = oriID;
				ReferenceReplacement(id, oriID);
			}

			return oriID;
		}
		else
		{
			IsForeign[id] = true;
		}
	}
	else
	{
		if (address.find("region", 0) == string::npos)
		{
			vector<string> oldAdd;
			string tempid;
			string tempadd;

			for (unordered_map<string, string>::iterator it = addressID.begin(); it != addressID.end(); ++it) // update new address to existing function
			{
				tempid = it->second;

				if (tempid == id)
				{
					tempadd = it->first;
					oldAdd.push_back(tempadd);
				}
			}

			unordered_map<string, string> tempmap;

			for (unsigned int i = 0; i < oldAdd.size(); i++)
			{
				for (unordered_map<string, string>::iterator it = addressID.begin(); it != addressID.end(); ++it) // update new address to children of the existing function
				{
					tempadd = it->first;

					if (tempadd.find(oldAdd[i], 0) != string::npos)
					{
						tempadd.replace(0, oldAdd[i].length(), address);

						if (tempmap[tempadd].empty())
						{
							tempmap[tempadd] = it->second;
						}
					}
				}
			}

			for (auto it = tempmap.begin(); it != tempmap.end(); it++)
			{
				addressID[it->first] = it->second;
			}
		}

		if (addressID[address].empty())
		{
			addressID[address] = id;
		}
	}

	return id;
}

string conditionOldFunction(string id, string address, int functionlayer, string addline, bool isString)
{
	if (PerfectMatchCondition[id])
	{
		return id;
	}

	string tempparent = parent[id];
	string oldID;

	if (!exchangeID[tempparent].empty())
	{
		tempparent = exchangeID[tempparent];
	}

	if (IsOldFunctionExt("ZeroRegion", id, address, true))
	{
		string oriID = addressID[address];
		PerfectMatchCondition[oriID] = true;

		if (IsExist[oriID])
		{
			string changeID;

			for (auto& iterID : exchangeID)
			{
				if (iterID.second == oriID)
				{
					changeID = iterID.first;
				}
			}

			if (changeID.length() == 0)
			{
				cout << "BUG DETECTED in CrossReference function (ID: " << id << ", supposed ID: " << oriID << ")" << endl;
				Error = true;
				return id;
			}

			ReferenceReplacement(oriID, changeID, true);
		}

		return oriID;
	}

	int element = elements[tempparent];
	string flayer;

	if (isString)
	{
		flayer = "ck" + to_string(functionlayer);
	}
	else
	{
		flayer = "l" + to_string(functionlayer);
	}

	// run through all possible elements
	// as there might be new elements added changing the address
	for (int i = 0; i < element; i++)
	{
		string tempadd = address;
		tempadd.replace(address.find_last_of(addline) - addline.length() + 1, addline.length(), to_string(i) + flayer);

		if (IsOldFunctionExt("ZeroRegion", id, tempadd, true))
		{
			string oriID = addressID[tempadd];
			PerfectMatchCondition[oriID] = true;

			if (IsExist[oriID])
			{
				string changeID;

				for (auto& iterID : exchangeID)
				{
					if (iterID.second == oriID)
					{
						changeID = iterID.first;
					}
				}

				if (changeID.length() == 0)
				{
					cout << "BUG DETECTED in CrossReference function (ID: " << id << ", supposed ID: " << oriID << ")" << endl;
					Error = true;
					return id;
				}

				ReferenceReplacement(oriID, changeID, true);
			}

			addressChange[address] = tempadd;
			return oriID;
		}
	}

	if (!IsForeign[parent[id]])
	{
		string oriID = addressID[address];

		if (!PerfectMatchCondition[oriID] && !IsExist[oriID])
		{
			return addressID[address];
		}
	}

	return "";
}

int FileLineCount(string filename)
{
	int linecount = 0;
	char line[1000];
	FILE* file;
	fopen_s(&file, filename.c_str(), "r");

	if (file)
	{
		while (fgets(line, 1000, file))
		{
			linecount++;
		}

		fclose(file);
	}
	else
	{
		cout << "ERROR: Unable to open file (File: " << filename << ")" << endl;
		Error = true;
	}

	return linecount;
}

void RecordID(string id, string address, bool compare)
{
	idcount.insert(stoi(id.substr(1, id.length() - 1)));

	if (!exchangeID[id].empty())
	{
		IsExist[exchangeID[id]] = true;
	}
	else
	{
		IsExist[id] = true;
	}

	if (!compare)
	{
		addressID.emplace(address, id);
	}
}

bool IsOldRegion(string id, string address, bool special)
{
	string tempparent = parent[id];

	if (tempparent == "")
	{
		string curID;

		for (auto& ID : exchangeID)
		{
			if (ID.second == id)
			{
				curID = ID.first;
				break;
			}
		}

		tempparent = parent[curID];

		if (tempparent == "")
		{
			cout << "ERROR: Old Region missing parent (ID: " << id << ", address: " << address << ")" << endl;
			Error = true;
			return false;
		}
	}

	if (!special)
	{
		while (IsForeign[tempparent])
		{
			tempparent = parent[tempparent];
		}

		if (!exchangeID[tempparent].empty())
		{
			tempparent = exchangeID[tempparent];
		}

		if (IsOldFunction("ZeroRegion", id, address))
		{
			return true;
		}

		int element = elements[tempparent + "T"];
		int tempint = (boost::regex_replace(string(address.substr(address.find_last_of("="))), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"))).length();
		
		for (int i = 0; i < element; i++)
		{
			string tempadd = address;
			tempadd.replace(address.find_last_of("=") + 2, tempint, to_string(i));

			if (!IsExist[addressID[tempadd]] && IsOldFunction("ZeroRegion", id, tempadd))
			{
				addressChange[address] = tempadd;
				return true;
			}
		}
	}
	else
	{
		string masterparent = parent[tempparent];
		
		if (IsForeign[masterparent])
		{
			return false;
		}

		int element = elements[masterparent + "R"];
		string tempadd = address;

		int nextposition = 0;
		usize position = 0;

		for (int j = 0; j < 2; j++)
		{
			position = tempadd.find(">", nextposition);
			nextposition = tempadd.find(">", position + 1);
		}

		string masteradd = tempadd.substr(0, position + 1);

		if (IsOldFunction("ZeroRegion", tempparent, masteradd))
		{
			if (!addressID[masteradd].empty())
			{
				if (IsForeign[tempparent] && tempparent != addressID[masteradd])
				{
					IsForeign[tempparent] = false;
					string tempid = addressID[masteradd];
					ReferenceReplacement(tempparent, tempid);
					exchangeID[tempparent] = tempid;
				}
			}
			else
			{
				cout << "ERROR: Missing function address (ID: " << id << ", address: " << masteradd << ")" << endl;
				Error = true;
				return false;
			}

			if ((IsOldFunction("ZeroRegion", id, tempadd)))
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		int tempint = (boost::regex_replace(string(address.substr(address.find_last_of("="))), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"))).length();

		if (!IsExist[addressID[address]])
		{
			for (int i = 0; i < element; i++)
			{
				tempadd.replace(address.find_last_of("=") + 2, tempint, to_string(i));

				if (!IsExist[addressID[tempadd]])
				{
					size_t size = 0;
					position = 0;

					for (int j = 0; j < 2; j++)
					{
						position = tempadd.find(">", size);
						size = tempadd.find(">", position + 1);
					}

					masteradd = tempadd.substr(0, position + 1);

					if ((IsOldFunction("ZeroRegion", id, tempadd)) && (IsOldFunction("ZeroRegion", tempparent, masteradd)))
					{
						if (!addressID[masteradd].empty())
						{
							IsForeign[tempparent] = false;
							string tempid = addressID[masteradd];
							ReferenceReplacement(tempparent, tempid);
							exchangeID[tempparent] = tempid;
						}
						else
						{
							cout << "ERROR: Missing function address (ID: " << id << ", address: " << masteradd << ")" << endl;
							Error = true;
						}

						addressChange[address] = tempadd;
						return true;
					}
				}
			}
		}
	}

	return false;
}