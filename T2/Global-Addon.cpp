#include "Global-Addon.h"
#include "Global.h"

using namespace std;

void ReferenceReplacement(string wrongReference, string rightReference) // replacement function for foreign principle
{
	string ErrorFile = "new/" + wrongReference + ".txt";
	string CorrectFile = "temp/" + rightReference + ".txt";
	int wrongReferenceInt = stoi(wrongReference.substr(1, wrongReference.length() - 1));
	int rightReferenceInt = stoi(rightReference.substr(1, rightReference.length() - 1));

	// stage 1
	// ref replacement
	if (rightReference.find(wrongReference, 0) == string::npos)
	{
		if (!IsExist[rightReference])
		{
			string line;
			string templine;
			vector<string> newline;
			newline.reserve(FileLineCount(ErrorFile));
			ifstream ErrorInput(ErrorFile);
			if (ErrorInput.is_open())
			{
				while (getline(ErrorInput, line))
				{
					newline.push_back(line);
				}
				ErrorInput.close();
			}
			else
			{
				cout << "ERROR: ReferenceReplacement Inputfile(oldReference: " << wrongReference << ", newReference: " << rightReference << ")" << endl;
				Error = true;
			}

			ifstream CorrectionInput(CorrectFile); // get the header line
			if (CorrectionInput.is_open())
			{
				while (getline(CorrectionInput, line))
				{
					templine = line;
					break;
				}
			}

			ofstream CorrectOutput("new/" + rightReference + ".txt"); // output the new function with old header
			if (CorrectOutput.is_open())
			{
				CorrectOutput << templine << "\n";
				for (unsigned int i = 1; i < newline.size(); i++)
				{
					CorrectOutput << newline[i] << "\n";
				}
				CorrectOutput.close();
			}
			else
			{
				cout << "ERROR: ReferenceReplacement Outputfile(oldReference: " << wrongReference << ", newReference: " << rightReference << ")" << endl;
				Error = true;
			}

			if (elements[wrongReference] != 0)
			{
				elements[rightReference] = elements[wrongReference];
				elements.erase(elements.find(wrongReference));
			}

			IsExist[rightReference] = true;
			idcount.insert(rightReferenceInt); // add rightReference ID
		}
		
		if (remove(ErrorFile.c_str()) != 0) // delete wrongReference file
		{
			perror("Error deleting file");
		}

		IsExist[wrongReference] = false;
		idcount.erase(wrongReferenceInt); // remove wrongReference ID
	}

	// stage 2
	ReferenceReplacementExt(wrongReference, rightReference);
}

void ReferenceReplacementExt(string wrongReference, string rightReference)
{
	lock_guard<mutex> filelock(locker[wrongReference]);

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

			string line;
			vector<string> storeline;
			string filename = "new/" + tempID + ".txt";

			IsFileInUse[filename] = true;
			storeline.reserve(FileLineCount(filename));
			ifstream input(filename);
			if (input.is_open())
			{
				while (getline(input, line))
				{
					if (line.find(wrongReference, 0) != string::npos)
					{
						usize size = count(line.begin(), line.end(), '#');
						for (unsigned int i = 0; i < size; i++)
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
				input.close();
			}
			else
			{
				cout << "ERROR: ReferenceReplacementExt Inputfile(oldReference: " << wrongReference << ", newReference: " << rightReference << ", ID: " << tempID << ")" << endl;
				Error = true;
			}

			ofstream output(filename);
			if (output.is_open())
			{
				for (unsigned int j = 0; j < storeline.size(); j++)
				{
					output << storeline[j] << "\n";
				}
				output.close();
			}
			else
			{
				cout << "ERROR: ReferenceReplacementExt Outputfile(oldReference: " << wrongReference << ", newReference: " << rightReference << ", ID: " << tempID << ")" << endl;
				Error = true;
			}
			IsFileInUse[filename] = false;

			referencingIDs[rightReference].push_back(tempID);
		}
		referencingIDs.erase(referencingIDs.find(wrongReference));
	}
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
				int position = 0;
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
				int position = 0;
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