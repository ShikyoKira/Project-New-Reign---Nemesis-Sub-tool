#include "animationdata.h"

using namespace std;

AnimDataProject::AnimDataProject(vecstr animdatafile, string project, string filepath, string modcode)
{
	int startline = 0;
	BehaviorListProcess(*this, startline, animdatafile, project, modcode);

	if (startline >= int(animdatafile.size()))
	{
		return;
	}

	AnimDataProcess(animdatalist, startline, animdatafile, project, modcode);

	if (startline >= int(animdatafile.size()))
	{
		return;
	}

	InfoDataProcess(infodatalist, startline, animdatafile, project, modcode);
}

int AnimDataProject::GetAnimTotalLine()
{
	int counter = 3 + int(behaviorlist.size());

	for (auto& it : animdatalist)
	{
		counter += 7 + it.eventname.size();
	}

	return counter;
}

int AnimDataProject::GetInfoTotalLine()
{
	int counter = 0;

	for (auto& it : infodatalist)
	{
		counter += 5 + int(it.motiondata.size()) + int(it.rotationdata.size());
	}

	return counter;
}

void BehaviorListProcess(AnimDataProject& storeline, int& startline, vecstr& animdatafile, string project, string modcode)
{
	if (!isOnlyNumber(animdatafile[startline]))
	{
		cout << "Invalid format for \"animationdatasinglefile.txt\" in AnimData section has been detected. Please contact the template creator" << endl << "Project: " << project << endl << "Header: Header" << endl;
		Error = true;
		throw 1;
	}

	int i = startline + 1;

	if (i + 4 >= int(animdatafile.size()))
	{
		cout << "Missing project information for a project in \"animationdatasinglefile.txt\"" << endl << "Project: " << project << endl;
		Error = true;
		throw 1;
	}

	storeline.unknown1 = animdatafile[i++];
	++i;

	while (!isOnlyNumber(animdatafile[i]))
	{
		if (hasAlpha(animdatafile[i]))
		{
			storeline.behaviorlist.push_back(animdatafile[i++]);
		}
		else
		{
			cout << "Invalid format for \"animationdatasinglefile.txt\" in AnimData section has been detected. Please contact the template creator" << endl << "Project: " << project << endl << "Header: Header" << endl;
			Error = true;
			throw 1;
		}
	}

	storeline.unknown2 = animdatafile[i++];
	startline = i;

	if (i < int(animdatafile.size()) && !hasAlpha(animdatafile[i]))
	{
		cout << "Invalid format for \"animationdatasinglefile.txt\" in AnimData section has been detected. Please contact the template creator" << endl << "Project: " << project << endl << "Header: Header" << endl;
		Error = true;
		throw 1;
	}
}

void AnimDataProcess(vector<AnimDataPack>& storeline, int& startline, vecstr& animdatafile, string project, string modcode)
{
	for (int i = startline; i < int(animdatafile.size()); ++i)
	{
		if (!hasAlpha(animdatafile[i]))
		{
			cout << "Invalid format for \"animationdatasinglefile.txt\" in AnimData section has been detected. Please contact the template creator" << endl << "Project: " << project << endl << "Header: Header" << endl;
			Error = true;
			throw 1;
		}

		string name = animdatafile[i++];

		if (!isOnlyNumber(animdatafile[i]))
		{
			cout << "Invalid format for \"animationdatasinglefile.txt\" in AnimData section has been detected. Please contact the template creator" << endl << "Project: " << project << endl << "Header: Header" << endl;
			Error = true;
			throw 1;
		}

		string uniquecode = animdatafile[i++];
		AnimDataPack curAP;
		curAP.name = name;
		curAP.uniquecode = uniquecode;

		if (!isOnlyNumber(animdatafile[i]))
		{
			cout << "Invalid format for \"animationdatasinglefile.txt\" in AnimData section has been detected. Please contact the template creator" << endl << "Project: " << project << endl << "Header: Header" << endl;
			Error = true;
			throw 1;
		}

		curAP.unknown1 = animdatafile[i++];

		if (!isOnlyNumber(animdatafile[i]))
		{
			cout << "Invalid format for \"animationdatasinglefile.txt\" in AnimData section has been detected. Please contact the template creator" << endl << "Project: " << project << endl << "Header: Header" << endl;
			Error = true;
			throw 1;
		}

		curAP.unknown2 = animdatafile[i++];

		if (!isOnlyNumber(animdatafile[i]))
		{
			cout << "Invalid format for \"animationdatasinglefile.txt\" in AnimData section has been detected. Please contact the template creator" << endl << "Project: " << project << endl << "Header: Header" << endl;
			Error = true;
			throw 1;
		}

		curAP.unknown3 = animdatafile[i++];

		if (!isOnlyNumber(animdatafile[i++]))
		{
			cout << "Invalid format for \"animationdatasinglefile.txt\" in AnimData section has been detected. Please contact the template creator" << endl << "Project: " << project << endl << "Header: Header" << endl;
			Error = true;
			throw 1;
		}

		if (animdatafile[i].length() != 0 && hasAlpha(animdatafile[i]))
		{
			while (animdatafile[i].length() != 0)
			{
				curAP.eventname.push_back(animdatafile[i++]);
			}
		}

		if (animdatafile[i++].length() != 0)
		{
			cout << "Invalid format for \"animationdatasinglefile.txt\" in AnimData section has been detected. Please contact the template creator" << endl << "Project: " << project << endl << "Header: Header" << endl;
			Error = true;
			throw 1;
		}

		storeline.push_back(curAP);

		if (isOnlyNumber(animdatafile[i]))
		{
			startline = i;
			break;
		}
		else
		{
			--i;
		}
	}
}

void InfoDataProcess(vector<InfoDataPack>& storeline, int& startline, vecstr& animdatafile, string project, string modcode)
{
	unordered_map<string, bool> isExist;

	for (int i = startline; i < int(animdatafile.size()); ++i)
	{
		if (!isOnlyNumber(animdatafile[i]))
		{
			cout << "Invalid format for \"animationdatasinglefile.txt\" in InfoData section has been detected. Please contact the template creator" << endl << "Project: " << project << endl << "Unique Code: " << animdatafile[i] << endl;
			Error = true;
			throw 1;
		}

		InfoDataPack curIP;
		string uniquecode = animdatafile[i++];
		curIP.uniquecode = uniquecode;

		if (isExist[uniquecode])
		{
			cout << "Duplicated unique code detected in InfoData section. Please contact the template creator" << endl << "Project: " << project << endl << "Unique Code: " << uniquecode << endl;
			Error = true;
			throw 1;
		}

		isExist[uniquecode] = true;

		if (!isOnlyNumber(animdatafile[i]))
		{
			cout << "Invalid format for \"animationdatasinglefile.txt\" in InfoData section has been detected. Please contact the template creator" << endl << "Project: " << project << endl << "Unique Code: " << uniquecode << endl;
			Error = true;
			throw 1;
		}

		curIP.duration = animdatafile[i];

		if (!isOnlyNumber(animdatafile[++i]))
		{
			cout << "Invalid format for \"animationdatasinglefile.txt\" in InfoData section has been detected. Please contact the template creator" << endl << "Project: " << project << endl << "Unique Code: " << uniquecode << endl;
			Error = true;
			throw 1;
		}

		++i;

		while (count(animdatafile[i].begin(), animdatafile[i].end(), ' ') != 0)
		{
			curIP.motiondata.push_back(animdatafile[i++]);
		}

		if (!isOnlyNumber(animdatafile[i++]))
		{
			cout << "Invalid format for \"animationdatasinglefile.txt\" in InfoData section has been detected. Please contact the template creator" << endl << "Project: " << project << endl << "Unique Code: " << uniquecode << endl;
			Error = true;
			throw 1;
		}

		while (count(animdatafile[i].begin(), animdatafile[i].end(), ' ') != 0)
		{
			curIP.rotationdata.push_back(animdatafile[i++]);
		}

		if (animdatafile[i].length() != 0)
		{
			cout << "Invalid format for \"animationdatasinglefile.txt\" in InfoData section has been detected. Please contact the template creator" << endl << "Project: " << project << endl << "Unique Code: " << uniquecode << endl;
			Error = true;
			throw 1;
		}

		storeline.push_back(curIP);
	}
}

void animDataInitialize(string filepath, vecstr& projectList, vector<AnimDataProject>& ADProject)
{
#define NOT_FOUND string::npos

	unordered_map<string, int> projectNameCount;	// count the occurance of the project name
	unordered_map<string, int> nextProject;

	string project;
	string header;

	unordered_map<string, unordered_map<string, vecstr>> catalystMap;	// project, header, list of lines
	unordered_map<string, vecstr> animDataHeader;						// project, list of headers
	unordered_map<string, vecstr> animDataInfo;							// project, list of info headers

	{
		// read behavior file
		vecstr catalyst;
		GetFunctionLines(filepath, catalyst);
		vecstr newline;

		// check for error
		if (Error)
		{
			return;
		}

		int projectcounter = 0;
		bool isInfo = false;
		int num = 0;
		projectList.reserve(500);

		for (unsigned int i = 1; i < catalyst.size(); ++i)
		{
			if (catalyst[i].find(".txt") == NOT_FOUND)
			{
				num = i;
				break;
			}

			projectList.push_back(catalyst[i]);
		}

		projectList.shrink_to_fit();
		project = projectList[0] + " " + to_string(++projectNameCount[projectList[0]]);
		header = "$header$";
		animDataHeader[project].push_back(header);
		newline.reserve(20);
		newline.clear();

		// add picked behavior and remove not picked behavior 
		// separation of all items for easier access and better compatibility
		for (unsigned int l = num; l < catalyst.size(); ++l)
		{
			string line = catalyst[l];

			if (l + 3 < catalyst.size() && l > 2)
			{
				if (catalyst[l - 1] == "")
				{
					bool out = false;

					if (isOnlyNumber(line))
					{
						int next = 1;

						if (l + next < catalyst.size() && catalyst[l + next].find("<!--") != NOT_FOUND)
						{
							++next;
						}

						if (l + next < catalyst.size() && isOnlyNumber(catalyst[l + next]))
						{
							++next;

							if (l + next < catalyst.size() && catalyst[l + next].find("<!--") != NOT_FOUND)
							{
								++next;
							}

							if (l + next < catalyst.size() && isOnlyNumber(catalyst[l + next]))
							{
								int nextnext = next + 1;

								if (l + next < catalyst.size() && catalyst[l + next].find("<!--") != NOT_FOUND)
								{
									++nextnext;
								}

								if (catalyst[l + next] == "0" || (l + nextnext < catalyst.size() && catalyst[l + nextnext].find("\\") != NOT_FOUND))
								{
									newline.shrink_to_fit();
									catalystMap[project][header] = newline;
									string newproject = projectList[++projectcounter];
									header = "$header$";
									project = newproject + " " + to_string(++projectNameCount[newproject]);
									animDataHeader[project].push_back(header);
									newline.reserve(20);
									newline.clear();
									isInfo = false;
									out = true;
								}
							}
						}
					}

					if (!out)
					{
						if (!isInfo)
						{
							if (hasAlpha(line))
							{
								if (isOnlyNumber(catalyst[l + 1]))
								{
									newline.shrink_to_fit();
									catalystMap[project][header] = newline;
									newline.reserve(20);
									newline.clear();
									header = line + " " + catalyst[l + 1];
									animDataHeader[project].push_back(header);
								}
							}
							else if (isOnlyNumber(line))
							{
								isInfo = true;
								newline.shrink_to_fit();
								catalystMap[project][header] = newline;;
								newline.reserve(20);
								newline.clear();
								header = catalyst[++l];
								line = catalyst[l];
								animDataInfo[project].push_back(header);
							}
						}
						else if (isOnlyNumber(line))
						{
							newline.shrink_to_fit();
							catalystMap[project][header] = newline;
							newline.reserve(20);
							newline.clear();
							header = line;
							animDataInfo[project].push_back(header);
						}
					}
				}
				else if (header == "$header$")
				{
					if (hasAlpha(line) && line.find("\\") == NOT_FOUND && l + 1 < catalyst.size())
					{
						if (isOnlyNumber(catalyst[l + 1]))	// if it is unique code
						{
							newline.shrink_to_fit();
							catalystMap[project][header] = newline;
							newline.reserve(20);
							newline.clear();
							header = line + " " + catalyst[l + 1];
							animDataHeader[project].push_back(header);
						}
					}
					else if (isOnlyNumber(catalyst[l - 1]) && catalyst[l - 1] == "0" && isOnlyNumber(line))
					{
						int next = 1;

						if (l + next < catalyst.size() && isOnlyNumber(catalyst[l + next]))
						{
							++next;

							if (l + next < catalyst.size() && catalyst[l + next].find("<!--") != NOT_FOUND)
							{
								++next;
							}

							if (l + next < catalyst.size() && isOnlyNumber(catalyst[l + next]))
							{
								int nextnext = next + 1;

								if (l + next < catalyst.size() && catalyst[l + next].find("<!--") != NOT_FOUND)
								{
									++nextnext;
								}

								if (catalyst[l + next] == "0" || (l + nextnext < catalyst.size() && catalyst[l + nextnext].find("\\") != NOT_FOUND))
								{
									newline.shrink_to_fit();
									catalystMap[project][header] = newline;
									string newproject = projectList[++projectcounter];
									project = newproject + " " + to_string(++projectNameCount[newproject]);
									animDataHeader[project].push_back(header);
									newline.reserve(20);
									newline.clear();
									isInfo = false;
								}
							}
						}
					}
				}
			}

			newline.push_back(line);
		}

		if (newline.size() != 0)
		{
			if (newline.back().length() == 0)
			{
				newline.pop_back();
			}

			newline.shrink_to_fit();
			catalystMap[project][header] = newline;
			newline.clear();
		}

		try
		{
			for (unsigned int i = 0; i < projectList.size(); ++i)
			{
				if (projectNameCount[project] > 1)
				{
					project = projectList[i] + to_string(++nextProject[project]);
				}
				else
				{
					project = projectList[i] + " 1";
				}

				vecstr combined;

				for (unsigned int j = 0; j < animDataHeader[project].size(); ++j)
				{
					header = animDataHeader[project][j];
					combined.insert(combined.end(), catalystMap[project][header].begin(), catalystMap[project][header].end());
				}

				for (unsigned int j = 0; j < animDataInfo[project].size(); ++j)
				{
					header = animDataInfo[project][j];
					combined.insert(combined.end(), catalystMap[project][header].begin(), catalystMap[project][header].end());
				}

				if (combined.size() == 0)
				{
					cout << "Empty project detected. Could be a bug. Report to Nemesis' author if you are sure" << endl << "Project: " << project << endl;
					Error = true;
					return;
				}

				AnimDataProject newProject(combined, projectList[i], filepath);
				ADProject.push_back(newProject);
			}
		}
		catch (int)
		{
			return;
		}
	}
}

void animDataProcess(string directory, string originalfile, string editedfile)
{
	vector<string> originalproject;
	vector<string> editedproject;
	vector<AnimDataProject> originaldata;
	vector<AnimDataProject> editeddata;

	animDataInitialize(originalfile, originalproject, originaldata);
	animDataInitialize(editedfile, editedproject, editeddata);

	int projectcount = 0;

	for (unsigned int i = 0; i < editedproject.size(); ++i)
	{
		if (projectcount < int(originalproject.size()) && originalproject[projectcount] == editedproject[i])
		{
			vector<int> changes;

			if (originaldata[projectcount].unknown1 != editeddata[i].unknown1)
			{
				changes.push_back(0);
			}

			int behaviorcounter = 0;

			for (unsigned int j = 0; j < editeddata[i].behaviorlist.size(); ++j)
			{
				if (originaldata[projectcount].behaviorlist[behaviorcounter] != editeddata[i].behaviorlist[j])
				{
					changes.push_back(1);
					break;
				}

				++behaviorcounter;
			}

			if (originaldata[projectcount].unknown2 != editeddata[i].unknown2)
			{
				changes.push_back(2);
			}

			++projectcount;
		}
		else
		{
			if ((CreateDirectory((directory + editedproject[i]).c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError()))
			{
				{
					ofstream output(directory + editedproject[i] + "/" + "$header$.txt");

					if (output.is_open())
					{
						FunctionWriter fwriter(&output);

						fwriter << to_string(editeddata[i].GetAnimTotalLine()) << "\n";
						fwriter << editeddata[i].unknown1 << "\n";
						fwriter << to_string(editeddata[i].behaviorlist.size()) << "\n";

						for (unsigned int j = 0; j < editeddata[i].behaviorlist.size(); ++j)
						{
							fwriter << editeddata[i].behaviorlist[j] << "\n";
						}

						fwriter << editeddata[i].unknown2 << "\n";
						output.close();
					}
					else
					{
						cout << "ERROR: Failed to output animationdata header file(Project: " << editedproject[i] << ", Header: $header$)" << endl;
						Error = true;
						return;
					}
				}

				if (editeddata[i].unknown2 != "0")
				{
					unordered_map<string, string> namecode;

					for (unsigned int j = 0; j < editeddata[i].animdatalist.size(); ++j)
					{
						string name = editeddata[i].animdatalist[j].name;
						namecode[editeddata[i].animdatalist[j].uniquecode] = name;
						ofstream output(directory + editedproject[i] + "/$" + name + "$.txt");

						if (output.is_open())
						{
							FunctionWriter fwriter(&output);

							fwriter << name << "\n";
							fwriter << editeddata[i].animdatalist[j].uniquecode << "\n";
							fwriter << editeddata[i].animdatalist[j].unknown1 << "\n";
							fwriter << editeddata[i].animdatalist[j].unknown2 << "\n";
							fwriter << editeddata[i].animdatalist[j].unknown3 << "\n";
							fwriter << to_string(editeddata[i].animdatalist[j].eventname.size()) << "\n";

							for (unsigned int k = 0; k < editeddata[i].animdatalist[j].eventname.size(); ++k)
							{
								fwriter << editeddata[i].animdatalist[j].eventname[k] << "\n";
							}

							fwriter << "\n";
							output.close();
						}
						else
						{
							cout << "ERROR: Failed to output animationdata header file(Project: " << editedproject[i] << ", Header: " << name << ")" << endl;
							Error = true;
							return;
						}
					}

					for (unsigned int j = 0; j < editeddata[i].infodatalist.size(); ++j)
					{
						string name = namecode[editeddata[i].infodatalist[j].uniquecode];

						if (name.length() == 0)
						{
							cout << "ERROR: Non-registered uniquecode detected. It will ";
						}

						ofstream output(directory + editedproject[i] + "/$" + name + "$UC.txt");

						if (output.is_open())
						{
							FunctionWriter fwriter(&output);

							fwriter << editeddata[i].infodatalist[j].uniquecode << "\n";
							fwriter << editeddata[i].infodatalist[j].duration << "\n";
							fwriter << to_string(editeddata[i].infodatalist[j].motiondata.size()) << "\n";

							for (unsigned int k = 0; k < editeddata[i].infodatalist[j].motiondata.size(); ++k)
							{
								fwriter << editeddata[i].infodatalist[j].motiondata[k] << "\n";
							}

							fwriter << to_string(editeddata[i].infodatalist[j].rotationdata.size()) << "\n";

							for (unsigned int k = 0; k < editeddata[i].infodatalist[j].rotationdata.size(); ++k)
							{
								fwriter << editeddata[i].infodatalist[j].rotationdata[k] << "\n";
							}

							fwriter << "\n";
							output.close();
						}
						else
						{
							cout << "ERROR: Failed to output animationdata header file(Project: " << editedproject[i] << ", Unique Code: " << editeddata[i].infodatalist[j].uniquecode << ")" << endl;
							Error = true;
							return;
						}
					}
				}
			}
		}
	}
}
