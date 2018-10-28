#include "FillFunction.h"
#include "AnimData\animationdata.h"
#include "AnimSetData\animationsetdata.h"

using namespace std;

void AddBehavior(string filename, vecstr& storeline, bool edited)
{
	string namesearch = "SERIALIZE_IGNORED";
	string tempID;
	bool record = false;
	bool eventOpen = false;
	bool attriOption = false;
	bool varOpen = false;
	bool charOpen = false;
	int counter = 0;

	for (auto& line: storeline)
	{
		if (Error)
		{
			return;
		}
		
		if (line.find(namesearch, 0) == string::npos)
		{
			if (edited)
			{
				if (line.find("#", 0) != string::npos)
				{
					if (line.find("&#", 0) == string::npos && line.find(";", line.find("&#")) == string::npos)
					{
						usize tempint = 0;
						usize size = count(line.begin(), line.end(), '#');

						for (unsigned int i = 0; i < size; i++)
						{
							usize position = line.find("#", tempint);
							tempint = line.find("#", position + 1);
							string tempID;

							if (tempint == -1)
							{
								string templine;

								if (line.find("signature", 0) != string::npos)
								{
									templine = line.substr(0, line.find("class"));
								}
								else if (line.find("toplevelobject", 0) != string::npos)
								{
									templine = line.substr(line.find("toplevelobject"), line.find(">"));
								}
								else
								{
									templine = line.substr(position, tempint - position - 1);
								}

								tempID = "#" + boost::regex_replace(string(templine), boost::regex("[^0-9]*([0-9]+).*"), string("\\1"));
							}
							else
							{
								tempID = line.substr(position, tempint - position - 1);
							}

							int tempLength = tempID.length();

							if (tempLength > 6)
							{
								cout << "ERROR: File contains invalid data(File: " << filename << ")" << endl;
								Error = true;
								return;
							}

							string strID = tempID.substr(1, tempLength - 1);
							int intID = stoi(strID);

							if (intID < 10000)
							{
								usize position2 = line.find(tempID);
								line.replace(position2, tempLength, "#9" + strID);
							}
						}
					}
				}

				if (line.find("<hkparam name=\"eventNames\" numelements=\"") != string::npos && line.find("<hkparam name=\"eventNames\" numelements=\"0\">") == string::npos)
				{
					eventOpen = true;
				}
				else if (line.find("<hkparam name=\"attributeNames\" numelements=\"") != string::npos && line.find("<hkparam name=\"attributeNames\" numelements=\"0\">") == string::npos)
				{
					attriOption = true;
				}
				else if (line.find("<hkparam name=\"variableNames\" numelements=\"") != string::npos && line.find("<hkparam name=\"variableNames\" numelements=\"0\">") == string::npos)
				{
					varOpen = true;
				}
				else if (line.find("<hkparam name=\"characterPropertyNames\" numelements=\"") != string::npos && line.find("<hkparam name=\"characterPropertyNames\" numelements=\"0\">") == string::npos)
				{
					charOpen = true;
				}
				else if (line.find("</hkparam>") != string::npos)
				{
					if (eventOpen)
					{
						eventOpen = false;
					}
					else if (attriOption)
					{
						attriOption = false;
					}
					else if (varOpen)
					{
						varOpen = false;
					}
					else if (charOpen)
					{
						charOpen = false;
					}

					counter = 0;
				}

				if (line.find("<hkcstring>") != string::npos && line.find("</hkcstring>") != string::npos)
				{
					if (eventOpen)
					{
						usize pos = line.find("<hkcstring>") + 11;
						string templine = line.substr(pos, line.find("</hkcstring>") - pos);
						eventID[to_string(counter++)] = templine;
					}
					else if (attriOption)
					{
						usize pos = line.find("<hkcstring>") + 11;
						string templine = line.substr(pos, line.find("</hkcstring>") - pos);
						attributeID[to_string(counter++)] = templine;
					}
					else if (varOpen)
					{
						usize pos = line.find("<hkcstring>") + 11;
						string templine = line.substr(pos, line.find("</hkcstring>") - pos);
						variableID[to_string(counter++)] = templine;
					}
					else if (charOpen)
					{
						usize pos = line.find("<hkcstring>") + 11;
						string templine = line.substr(pos, line.find("</hkcstring>") - pos);
						characterID[to_string(counter++)] = templine;
					}
				}
			}
			else
			{
				if (line.find("<hkparam name=\"eventNames\" numelements=\"") != string::npos && line.find("<hkparam name=\"eventNames\" numelements=\"0\">") == string::npos)
				{
					usize pos = line.find("numelements=\"") + 13;
					string number = boost::regex_replace(string(line), boost::regex("[\t]*<hkparam name=\"eventNames\" numelements=\"([0-9]+)\">"), string("\\1"));
					eventCount = static_cast<unsigned int>(stoi(number) - 1);
				}
				else if (line.find("<hkparam name=\"variableNames\" numelements=\"") != string::npos && line.find("<hkparam name=\"variableNames\" numelements=\"0\">") == string::npos)
				{
					usize pos = line.find("numelements=\"") + 13;
					string number = boost::regex_replace(string(line), boost::regex("[\t]*<hkparam name=\"variableNames\" numelements=\"([0-9]+)\">"), string("\\1"));
					varCount = static_cast<unsigned int>(stoi(number) - 1);
				}
			}

			if (line.find("<hkobject name=\"", 0) != string::npos && record == false)
			{
				tempID = line.substr(line.find("name") + 6, line.find("class") - line.find("name") - 8);
				record = true;
			}

			if (line.length() == 0)
			{
				record = false;
			}

			if (record == true)
			{
				if (!edited)
				{
					FunctionLineOriginal[tempID].push_back(line);
				}
				else
				{
					FunctionLineEdited[tempID].push_back(line);
				}
			}
		}
	}
}

void AddAnimData(string filename, vecstr& storeline, bool edited)
{
	if (edited)
	{
		animDataInitialize(filename, storeline, AnimDataEdited);
	}
	else
	{
		animDataInitialize(filename, storeline, AnimDataOriginal);
	}
}

void AddAnimSetData(string filename, vecstr& storeline, bool edited)
{
	if (edited)
	{
		animSetDataInitialize(filename, storeline, AnimSetDataEdited);
	}
	else
	{
		animSetDataInitialize(filename, storeline, AnimSetDataOriginal);
	}
}
