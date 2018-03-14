#include "Global.h"

using namespace std;

typedef unordered_map<string, string> mapstring;

bool Error = false;
bool Debug = false;
set<int> idcount;
unordered_map<string, mutex> locker;
unordered_map<string, int> regioncount;
unordered_map<string, int> elements;
unordered_map<string, bool> IsForeign;
unordered_map<string, bool> IsExist;
unordered_map<string, bool> IsFileInUse;
unordered_map<string, string> parent;
unordered_map<string, string> region;
unordered_map<string, string> newID;
unordered_map<string, string> eventID;
unordered_map<string, string> variableID;
unordered_map<string, string> attributeID;
unordered_map<string, string> characterID;
unordered_map<string, string> addressID;
unordered_map<string, string> exchangeID;
unordered_map<string, string> addressChange; // temp address change check
unordered_map<string, vector<string>> FunctionLineOriginal; // function original data
unordered_map<string, vector<string>> FunctionLineEdited; // function edited data
unordered_map<string, vector<string>> referencingIDs; // which function use this ID
string modcode = "null";
string targetfilename;
string targetfilenameedited;
string shortFileName;
string shortFileNameEdited;
int functioncount = 0;

bool IsOldRegion(string id, string address, bool special = false);

inline bool IsFileExist(const string& name)
{
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

bool IsOldFunction(string filename, string id, string address)
{
	string tempadd = address.substr(address.find_last_of("="));

	if (((address.find("(e", 0) != string::npos) || (address.find("(m", 0) != string::npos) || (address.find("(y", 0) != string::npos)) && (count(tempadd.begin(), tempadd.end(), '>') == 2) && (filename != "ZeroRegion"))
	{
		return IsOldRegion(id, address);
	}
	else if ((address.find("(i", 0) != string::npos) && (count(tempadd.begin(), tempadd.end(), '>') == 3) && (filename != "ZeroRegion"))
	{
		return IsOldRegion(id, address, true);
	}

	if (addressID[address] != "")
	{
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

			for (int i = 0; i < size; i++)
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
			}
		}
		else
		{
			cout << "ERROR: IsOldFunction Input(File: " << filename << ", ID: " << tempID << ")" << endl;
			Error = true;
			return false;
		}

		// comparing data to determine the function with the old add is new or old (solving same add paradox)
		string tempOldID = addressID[address];
		ifstream input("temp/" + tempOldID + ".txt");
		if (input.is_open())
		{
			bool match = false;
			while (getline(input, line))
			{
				if (IsName)
				{
					if (line.find(searchname, 0) != string::npos)
					{
						ReferenceReplacementExt(id, tempOldID);
						return true;
					}
				}
				else
				{
					if ((line.find(searchname, 0) != string::npos) && (match == false))
					{
						match = true;
					}
					else if ((line.find(searchname2, 0) != string::npos) && (match == true))
					{
						ReferenceReplacementExt(id, tempOldID);
						return true;
					}
				}

			}
			input.close();
		}
		else
		{
			cout << "ERROR: IsOldFunction Input(File: temp/" << tempOldID << ".txt , ID: " << tempOldID << ")" << endl;
			Error = true;
		}
	}
	
	return false;
}

inline string ExePath()
{
	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	string::size_type pos = string(buffer).find_last_of("\\/");
	return string(buffer).substr(0, pos);
}

string CrossReferencing(string id, string address, int functionlayer, bool compare, bool special) // whether it is foreign principle or usual cross referencing
{
	if (compare) // foreign principle 
	{
		string tempadd = address.substr(address.find_last_of("="));

		if ((address.find("(i", 0) == string::npos) || (count(tempadd.begin(), tempadd.end(), '>') != 3))
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
		
		if ((IsOldFunction(("new/" + id + ".txt"), id, address)) || ((special) && (!IsForeign[parent[id]]))) // old function confirmation
		{
			IsForeign[id] = false;
			string oriID = addressID[address];

			if (oriID.length() < 3)
			{
				IsForeign[id] = true;
				return id;
			}

			if (oriID.find(id, 0) == string::npos)
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

int FileLineCount(string filename)
{
	int linecount = 0;
	string line;
	ifstream open(filename);
	if (open.is_open())
	{
		while (getline(open, line))
		{
			linecount++;
		}
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
	parent.erase(parent.find(id));

	if (tempparent == "#90159")
		cout << endl;

	if (!special)
	{
		if (!exchangeID[tempparent].empty())
		{
			tempparent = exchangeID[tempparent];
		}

		int element = elements[tempparent];
		int tempint = (boost::regex_replace(string(address.substr(address.find_last_of("="))), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"))).length();

		for (int i = 0; i < element; i++)
		{
			string tempadd = address;
			tempadd.replace(address.find_last_of("=") + 2, tempint, to_string(i));
			if (IsOldFunction("ZeroRegion", id, tempadd))
			{
				addressChange[address] = tempadd;
				return true;
			}
		}
	}
	else
	{
		string masterparent = parent[tempparent];
		parent.erase(parent.find(tempparent));
		
		if (!exchangeID[masterparent].empty())
		{
			masterparent = exchangeID[masterparent];
		}

		int element = elements[masterparent];

		string tempadd = address;

		int nextposition = 0;
		int position = 0;
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
				IsForeign[tempparent] = false;
				string tempid = addressID[masteradd];
				ReferenceReplacement(tempparent, tempid);
				exchangeID[tempparent] = tempid;
			}
			else
			{
				cout << "ERROR: Missing function address(ID: " << id << ", address: " << masteradd << ")" << endl;
				Error = true;
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

		for (int i = 0; i < element; i++)
		{
			if (!IsExist[addressID[address]])
			{
				tempadd.replace(address.find_last_of("=") + 2, tempint, to_string(i));

				tempint = 0;
				position = 0;
				for (int j = 0; j < 2; j++)
				{
					position = tempadd.find(">", tempint);
					tempint = tempadd.find(">", position + 1);
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
						cout << "ERROR: Missing function address(ID: " << id << ", address: " << masteradd << ")" << endl;
						Error = true;
					}
					addressChange[address] = tempadd;
					return true;
				}
			}
		}
	}
	return false;
}