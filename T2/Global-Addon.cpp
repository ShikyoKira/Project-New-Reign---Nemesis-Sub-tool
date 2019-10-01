#include <boost\process.hpp>
#include <boost\process\windows.hpp>
#include "Global-Addon.h"
#include "Global.h"
#include "src\atomiclock.h"

using namespace std;

extern safeStringMap<string> newID;
extern vector<usize> datapacktracker;
extern safeStringMap<string> rootNode;
atomic_flag locker = ATOMIC_FLAG_INIT;

void dataIDChange(string& line, usize tracker);

void ReferenceReplacement(string wrongReference, string rightReference, bool reserve) // replacement function for foreign principle
{
	int wrongReferenceInt = stoi(wrongReference.substr(1, wrongReference.length() - 1));
	int rightReferenceInt = stoi(rightReference.substr(1, rightReference.length() - 1));

	// stage 1
	// ref replacement
	if (rightReference.find(wrongReference, 0) == string::npos)
	{
		if (!IsExist[rightReference])
		{
			if (reserve)
			{
				FunctionLineNew[rightReference].push_back(FunctionLineEdited[rightReference][0]);
			}
			else
			{
				FunctionLineNew[rightReference].push_back(FunctionLineOriginal[rightReference][0]);
			}

			if (FunctionLineNew[wrongReference].size() > 0)
			{
				for (unsigned int i = 1; i < FunctionLineNew[wrongReference].size(); i++)
				{
					FunctionLineNew[rightReference].push_back(FunctionLineNew[wrongReference][i]);
				}
			}
			else
			{
				cout << "ERROR: ReferenceReplacement Inputfile (oldReference: " << wrongReference << ", newReference: " << rightReference << ")" << endl;
				Error = true;
				return;
			}

			if (elements[wrongReference] != 0)
			{
				elements[rightReference] = elements[wrongReference];
				elements.erase(elements.find(wrongReference));
			}

			string wrongRef = wrongReference + "T";

			if (elements[wrongRef] != 0)
			{
				elements[rightReference + "T"] = elements[wrongRef];
				elements.erase(elements.find(wrongRef));
			}

			wrongRef = wrongReference + "R";

			if (elements[wrongRef] != 0)
			{
				elements[rightReference + "R"] = elements[wrongRef];
				elements.erase(elements.find(wrongRef));
			}

			IsExist[rightReference] = true;
			idcount.insert(rightReferenceInt); // add rightReference ID
		}
		
		FunctionLineNew.erase(FunctionLineNew.find(wrongReference));
		IsExist.erase(IsExist.find(wrongReference));
		idcount.erase(wrongReferenceInt); // remove wrongReference ID
	}

	// stage 2
	ReferenceReplacementExt(wrongReference, rightReference);
}

void ReferenceReplacementExt(string wrongReference, string rightReference)
{
	while (locker.test_and_set(memory_order_acquire));
	int tempint = referencingIDs[wrongReference].size();
	
	if (tempint > 0)
	{
		for (int i = 0; i < tempint; i++) // referencingIDs update
		{
			string tempID = referencingIDs[wrongReference][i];

			if (!exchangeID[tempID].empty())
			{
				tempID = exchangeID[tempID];
			}

			vecstr storeline;
			storeline.reserve(FunctionLineNew[tempID].size());
			string line;

			if (FunctionLineNew[tempID].size() > 0)
			{
				for (unsigned int k = 0; k < FunctionLineNew[tempID].size(); ++k)
				{
					line = FunctionLineNew[tempID][k];

					if (line[line.size() - 1] == '\n')
					{
						line.pop_back();
					}

					if (line.find(wrongReference, 0) != string::npos)
					{
						usize size = count(line.begin(), line.end(), '#');

						for (unsigned int j = 0; j < size; ++j)
						{
							line.replace(line.find(wrongReference), wrongReference.length(), rightReference);

							if (line.find(wrongReference, 0) == string::npos)
							{
								break;
							}
						}
					}

					storeline.push_back(line);
				}
			}
			else
			{
				cout << "ERROR: ReferenceReplacementExt Inputfile (oldReference: " << wrongReference << ", newReference: " << rightReference << ", ID: " << tempID << ")" << endl;
				Error = true;
				locker.clear(memory_order_release);
				return;
			}

			FunctionLineNew[tempID] = storeline;
			referencingIDs[rightReference].push_back(tempID);
		}

		referencingIDs.erase(referencingIDs.find(wrongReference));
	}

	unordered_map<string, string> tempparents;

	for (auto& each : parent)
	{
		if (each.second == wrongReference)
		{
			each.second = rightReference;
		}
		else if (each.first == wrongReference && each.second.length() > 0)
		{
			tempparents[wrongReference] = each.second;
		}
	}

	for (auto& each : tempparents)
	{
		parent.erase(parent.find(each.first));
		parent[rightReference] = each.second;
	}

	locker.clear(memory_order_release);
}

void NemesisReaderFormat(int id, vecstr& output)
{
	usize tracker = 0;
	
	if (id < 90000)
	{
		for (auto& dataid : datapacktracker)
		{
			if (id > int(dataid)) ++tracker;
		}
	}

	for (auto& line : output)
	{
		if (line.find("#", 0) != string::npos)
		{
			usize tempint = 0;
			usize position = 0;
			usize size = count(line.begin(), line.end(), '#');

			for (unsigned int j = 0; j < size; ++j)
			{
				position = line.find("#", tempint);
				tempint = line.find("#", position + 1);
				string tempID;

				tempID = "#";

				for (usize i = position + 1; i < line.length(); ++i)
				{
					if (isdigit(line[i])) tempID.push_back(line[i]);
					else break;
				}

				int intID = stoi(tempID.substr(1));

				if (intID > 89999)
				{
					int position2 = line.find(tempID);
					string modID = NodeIDCheck(tempID);
					line.replace(position2, tempID.length(), modID);
				}
			}
		}

		//dataIDChange(line, tracker);
	}
}

void dataIDChange(string& line, usize tracker)
{
	if (line.find("<hkparam name=\"id\">", 0) != string::npos && line.find("<hkparam name=\"id\">-1</hkparam>", 0) == string::npos)
	{
		usize eventpos = line.find("id\">") + 4;
		string eventid;

		for (usize i = eventpos; i < line.length(); ++i)
		{
			if (isdigit(line[i])) eventid.push_back(line[i]);
			else break;
		}

		if (eventid.length() > 0 && stoi(eventid) >= int(eventCount[tracker]) && eventID[tracker][eventid].length() != 0)
		{
			line.replace(eventpos, eventid.length(), "$eventID[" + eventID[tracker][eventid] + "]$");
		}
	}

	{
		string lowerline = boost::to_lower_copy(line);

		if (lowerline.find("eventid\">", 0) != string::npos && lowerline.find("eventid\">-1</hkparam>", 0) == string::npos)
		{
			usize eventpos = lowerline.find("eventid\">") + 9;
			string eventid;

			for (usize i = eventpos; i < line.length(); ++i)
			{
				if (isdigit(line[i])) eventid.push_back(line[i]);
				else break;
			}

			if (eventid.length() > 0 && stoi(eventid) >= int(eventCount[tracker]) && eventID[tracker][eventid].length() != 0)
			{
				line.replace(eventpos, eventid.length(), "$eventID[" + eventID[tracker][eventid] + "]$");
			}
		}
	}

	if (line.find("<hkparam name=\"variableIndex\">", 0) != string::npos)
	{
		usize varpos = line.find("<hkparam name=\"variableIndex\">") + 30;
		string varid;

		for (usize i = varpos; i < line.length(); ++i)
		{
			if (isdigit(line[i])) varid.push_back(line[i]);
			else break;
		}

		if (varid.length() > 0 && stoi(varid) >= int(varCount[tracker]) && variableID[tracker][varid].length() != 0)
		{
			line.replace(varpos, varid.length(), "$variableID[" + variableID[tracker][varid] + "]$");
		}
	}
}

vecstr GetElements(string number, unordered_map<string, vecstr>& functionlines, bool isTransition, string key)
{
	vecstr elements;
	vecstr storeline = functionlines[number];

	if (isTransition)
	{
		for (auto& line : storeline)
		{
			size_t pos = line.find("name=\"" + key + "\">", 0);

			if (pos != string::npos)
			{
				if (line.find("#", pos) != string::npos)
				{
					size_t tempInt = line.find("#");
					string reference = line.substr(tempInt, line.find("</hkparam>", tempInt) - tempInt);
					elements.push_back(reference);
				}
			}
		}
	}
	else
	{
		bool activate = false;

		for (auto& line : storeline)
		{
			if (!activate)
			{
				if (line.find("numelements=\"") != string::npos)
				{
					activate = true;
				}
			}
			else
			{
				if (line.find("</hkparam>") != string::npos)
				{
					break;
				}

				stringstream sstream(line);
				istream_iterator<string> ssbegin(sstream);
				istream_iterator<string> ssend;
				vecstr curElements(ssbegin, ssend);
				copy(curElements.begin(), curElements.end(), curElements.begin());

				for (auto& element : curElements)
				{
					if (element.length() == 0 || element[0] != '#')
					{
						cout << "ERROR: Invalid element. (Element: " << element << ")" << endl;
						Error = true;
						return elements;
					}
					else
					{
						elements.push_back(element);
					}
				}
			}
		}
	}

	return elements;
}

void GetFunctionLines(string filename, vecstr& storeline)
{
	{
		vecstr emptyVS;
		storeline = emptyVS; 
	}

	string curfilename;
	bool hkx = false;

	if (boost::filesystem::exists("hkxcmd.exe"))
	{
		usize counter = 0;
		char charline[2000];
		bool good = false;
		FILE* file;
		fopen_s(&file, filename.c_str(), "r");

		if (file)
		{
			while (fgets(charline, 2000, file))
			{
				if (string(charline).find("<?xml version=\"1.0\" encoding=\"ascii\"?>") != string::npos)
				{
					good = true;
					break;
				}

				if (counter++ > 20)
				{
					break;
				}
			}

			fclose(file);
		}
		else
		{
			cout << "ERROR: Unable to open file (File: " << filename << ")" << endl;
			Error = true;
			return;
		}

		boost::filesystem::path curfile(filename);

		if (good || curfile.extension().string() == ".hkx")
		{
			if (curfile.extension().string() != ".hkx")
			{
				curfilename = curfile.stem().string() + ".hkx";
				boost::filesystem::copy_file(curfile, curfilename, boost::filesystem::copy_option::overwrite_if_exists);
			}
			else
			{
				curfilename = filename;
			}

			string cmd = "hkxcmd convert -v:XML \"" + curfilename + "\" \"" + curfilename + ".hkx\"";

			try
			{
				boost::process::pipe p;
				boost::process::ipstream is;

				if (boost::process::system(cmd, boost::process::windows::hide, boost::process::std_in < p, boost::process::std_out > is) != 0)
				{
					cout << "ERROR: Unable to convert file. (Command: " << cmd << ")" << endl;
					Error = true;
					return;
				}
				else
				{
					if (!boost::filesystem::exists(curfilename + ".hkx"))
					{
						cout << "ERROR: Unable to convert file. (Command: " << cmd << ")" << endl;
						Error = true;
						return;
					}
				}
			}
			catch (...)
			{
				cout << "ERROR: Unable to convert file. (Command: " << cmd << ")" << endl;
				Error = true;
				return;
			}

			if (curfilename != filename)
			{
				while (boost::filesystem::exists(curfilename))
				{
					try
					{
						boost::filesystem::remove(curfilename);
					}
					catch (...) {}
				}
			}

			curfilename.append(".hkx");
			hkx = true;
		}
		else
		{
			curfilename = filename;
		}
	}
	else
	{
		curfilename = filename;
	}

	string line;
	storeline.reserve(FileLineCount(curfilename));
	char charline[2000];
	FILE* file;
	fopen_s(&file, curfilename.c_str(), "r");

	if (file)
	{
		if (hkx)
		{
			while (fgets(charline, 2000, file))
			{
				line = charline;

				if (line.length() > 0 && line.back() == '\n')
				{
					line.pop_back();
				}

				storeline.push_back(line);
			}

			for (auto& curline : storeline)
			{
				if (curline.find("toplevelobject", 0) != string::npos)
				{
					rootNode[filename] = boost::regex_replace(curline, boost::regex(".*toplevelobject=\"#([0-9]+)\">.*"), "\\1");
					break;
				}
			}
		}
		else
		{
			while (fgets(charline, 2000, file))
			{
				line = charline;

				if (line.length() > 0 && line.back() == '\n')
				{
					line.pop_back();
				}

				storeline.push_back(line);
			}
		}

		fclose(file);
	}
	else
	{
		cout << "ERROR: Unable to open file (File: " << filename << ")" << endl;
		Error = true;
	}

	if (curfilename != filename)
	{
		try
		{
			boost::filesystem::remove(curfilename);
		}
		catch (...) {}
	}
}

string NodeIDCheck(string ID)
{
	while (atomLock.test_and_set(std::memory_order_acquire));
	string modID;

	if (!newID[ID].empty())
	{
		modID = newID[ID];
	}
	else
	{
		modID = "#" + modcode + "$" + to_string(functioncount++);
		newID[ID] = modID;
	}

	atomLock.clear(std::memory_order_release);
	return modID;
}

void FolderCreate(string curBehaviorPath)
{
	size_t pos = curBehaviorPath.find("/") + 1;
	string curFolder = curBehaviorPath.substr(0, pos);
	__int64 counter = sameWordCount(curBehaviorPath, "/");

	for (int i = 0; i < counter; ++i)
	{
		if (CreateDirectory((curFolder).c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError())
		{
			pos = curBehaviorPath.find("/", pos) + 1;

			if (pos != 0)
			{
				curFolder = curBehaviorPath.substr(0, pos);
			}
		}
	}
}

bool isOnlyNumber(string line)
{
	try
	{
		boost::lexical_cast<double>(line);
	}
	catch (boost::bad_lexical_cast &)
	{
		return false;
	}

	return true;
}

bool hasAlpha(string line)
{
	string lower = boost::to_lower_copy(line);
	string upper = boost::to_upper_copy(line);

	if (lower != upper)
	{
		return true;
	}

	return false;
}

