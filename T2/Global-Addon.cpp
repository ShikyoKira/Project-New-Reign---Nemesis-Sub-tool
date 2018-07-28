#include "Global-Addon.h"
#include "Global.h"

using namespace std;

atomic_flag locker = ATOMIC_FLAG_INIT;

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

			vector<string> storeline;
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
		else if (each.first == wrongReference)
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

void NemesisReaderFormat(vector<string>& output, bool hasID)
{
	if (hasID)
	{
		for (unsigned int i = 0; i < output.size(); ++i) // changing newID to modCode ID
		{
			if (output[i].find("#", 0) != string::npos)
			{
				usize tempint = 0;
				usize position = 0;
				usize size = count(output[i].begin(), output[i].end(), '#');

				for (unsigned int j = 0; j < size; ++j)
				{
					position = output[i].find("#", tempint);
					tempint = output[i].find("#", position + 1);
					string tempID;

					if (tempint == -1)
					{
						string templine;

						if (output[i].find("signature", 0) != string::npos)
						{
							templine = output[i].substr(0, output[i].find("class"));
						}
						else
						{
							templine = output[i].substr(position);
						}

						tempID = "#" + boost::regex_replace(string(templine), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
					}
					else
					{
						tempID = output[i].substr(position, tempint - position - 1);
					}

					int tempLength = tempID.length();
					string strID = tempID.substr(1, tempLength - 1);
					int intID = stoi(strID);

					if (intID > 10000)
					{
						int position2 = output[i].find(tempID);
						string modID;

						if (!newID[tempID].empty())
						{
							modID = newID[tempID];
						}
						else
						{
							modID = "#" + modcode + "$" + to_string(functioncount++);
							newID[tempID] = modID;
						}

						output[i].replace(position2, tempLength, modID);
					}
				}
			}

			if ((output[i].find("<hkparam name=\"id\">", 0) != string::npos) && (output[i].find("<hkparam name=\"id\">-1</hkparam>", 0) == string::npos))
			{
				usize eventpos = output[i].find("id\">") + 4;
				string eventid = output[i].substr(eventpos, output[i].find("</hkparam>"));

				if (eventID[eventid].length() != 0)
				{
					output[i].replace(eventpos, output[i].find("</hkparam>") - eventpos, "$eventID[" + eventID[eventid] + "]$");
				}
			}
		}
	}
	else
	{
		for (unsigned int i = 0; i < output.size(); ++i) // changing newID to modCode ID
		{
			if (output[i].find("#", 0) != string::npos)
			{
				usize tempint = 0;
				usize position = 0;
				usize size = count(output[i].begin(), output[i].end(), '#');

				for (unsigned int j = 0; j < size; ++j)
				{
					position = output[i].find("#", tempint);
					tempint = output[i].find("#", position + 1);
					string tempID;

					if (tempint == -1)
					{
						string templine;

						if (output[i].find("signature", 0) != string::npos)
						{
							templine = output[i].substr(0, output[i].find("class"));
						}
						else
						{
							templine = output[i].substr(position);
						}

						tempID = "#" + boost::regex_replace(string(templine), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
					}
					else
					{
						tempID = output[i].substr(position, tempint - position - 1);
					}

					int tempLength = tempID.length();
					string strID = tempID.substr(1, tempLength - 1);
					int intID = stoi(strID);

					if (intID > 10000)
					{
						int position2 = output[i].find(tempID);
						string modID;

						if (!newID[tempID].empty())
						{
							modID = newID[tempID];
						}
						else
						{
							modID = "#" + modcode + "$" + to_string(functioncount++);
							newID[tempID] = modID;
						}

						output[i].replace(position2, tempLength, modID);
					}
				}
			}
		}
	}
}

vector<string> GetElements(string number, unordered_map<string, vector<string>>& functionlines, bool isTransition, string key)
{
	vector<string> elements;
	vector<string> storeline = functionlines[number];

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
				vector<string> curElements(ssbegin, ssend);
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

void GetFunctionLines(string filename, vector<string>& storeline)
{
	{
		vector<string> emptyVS;
		storeline = emptyVS;
	}

	string line;
	storeline.reserve(FileLineCount(filename));
	char charline[2000];
	FILE* file;
	fopen_s(&file, filename.c_str(), "r");

	if (file)
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

		fclose(file);
	}
	else
	{
		cout << "ERROR: Unable to open file (File: " << filename << ")" << endl;
		Error = true;
	}
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

