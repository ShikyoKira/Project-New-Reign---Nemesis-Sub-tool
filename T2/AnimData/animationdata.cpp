#include "animationdata.h"

using namespace std;

void BehaviorListProcess(AnimDataProject& storeline, int& startline, vecstr& animdatafile, std::string project);
void AnimDataProcess(std::vector<AnimDataPack>& storeline, int& startline, vecstr& animdatafile, std::string project);
void InfoDataProcess(std::vector<InfoDataPack>& storeline, int& startline, vecstr& animdatafile, std::string project);

AnimDataProject::AnimDataProject(vecstr animdatafile, string project, string projectfile)
{
	name = project;
	filename = projectfile.replace(projectfile.find_last_of(".txt") - 3, 4, "");
	proxy = false;
	int startline = 0;
	BehaviorListProcess(*this, startline, animdatafile, project);

	if (startline >= int(animdatafile.size()))
	{
		return;
	}

	AnimDataProcess(animdatalist, startline, animdatafile, project);

	if (startline >= int(animdatafile.size()))
	{
		return;
	}

	InfoDataProcess(infodatalist, startline, animdatafile, project);
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

void BehaviorListProcess(AnimDataProject& storeline, int& startline, vecstr& animdatafile, string project)
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

void AnimDataProcess(vector<AnimDataPack>& storeline, int& startline, vecstr& animdatafile, string project)
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
		curAP.proxy = false;
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

void InfoDataProcess(vector<InfoDataPack>& storeline, int& startline, vecstr& animdatafile, string project)
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
		curIP.proxy = false;

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

void animDataInitialize(string filepath, vecstr& catalyst, vector<shared_ptr<AnimDataProject>>& ADProject)
{
#define NOT_FOUND string::npos

	unordered_map<string, int> projectNameCount;	// count the occurance of the project name
	unordered_map<string, int> nextProject;

	string project;
	string header;

	unordered_map<string, unordered_map<string, vecstr>> catalystMap;	// project, header, list of lines
	unordered_map<string, vecstr> animDataHeader;						// project, list of headers
	unordered_map<string, vecstr> animDataInfo;							// project, list of info headers
	
	vecstr projectList;
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
	project = projectList[0] + "~" + to_string(++projectNameCount[projectList[0]]);
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
					if (isOnlyNumber(catalyst[l + 1]))
					{
						if (isOnlyNumber(catalyst[l + 2]))
						{
							if (catalyst[l + 2] == "0" || (l + 3 < catalyst.size() && catalyst[l + 3].find("\\") != NOT_FOUND))
							{
								newline.shrink_to_fit();
								catalystMap[project][header] = newline;
								string newproject = projectList[++projectcounter];
								header = "$header$";
								project = newproject + "~" + to_string(++projectNameCount[newproject]);
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
								header = line + "~" + catalyst[l + 1];
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
						header = line + "~" + catalyst[l + 1];
						animDataHeader[project].push_back(header);
					}
				}
				else if (isOnlyNumber(catalyst[l - 1]) && catalyst[l - 1] == "0" && isOnlyNumber(line) && l + 3 < catalyst.size())
				{
					if (isOnlyNumber(catalyst[l + 1]))
					{
						if (isOnlyNumber(catalyst[l + 2]))
						{
							if (catalyst[l + 2] == "0" || catalyst[l + 3].find("\\") != NOT_FOUND)
							{
								newline.shrink_to_fit();
								catalystMap[project][header] = newline;
								string newproject = projectList[++projectcounter];
								project = newproject + "~" + to_string(++projectNameCount[newproject]);
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

		if (Error)
		{
			return;
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
			if (projectNameCount[projectList[i]] > 1)
			{
				project = projectList[i] + "~" + to_string(++nextProject[projectList[i]]);
			}
			else
			{
				project = projectList[i] + "~1";
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

			AnimDataProject newProject(combined, projectList[i], project);
			ADProject.push_back(make_shared<AnimDataProject>(newProject));

			if (Error)
			{
				return;
			}
		}
	}
	catch (int)
	{
		return;
	}
}
