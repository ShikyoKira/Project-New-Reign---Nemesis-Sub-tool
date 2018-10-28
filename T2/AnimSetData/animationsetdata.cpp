#include "animationsetdata.h"

using namespace std;

void DataPackProcess(map<string, datapack, alphanum_less>& storeline, int& startline, vecstr& animdatafile, string filename);
void EquipPackProcess(vector<equip>& storeline, int& startline, vecstr& animdatafile, string filename, string header);
void TypePackProcess(vector<typepack>& storeline, int& startline, vecstr& animdatafile, string filename, string header);
void AnimPackProcess(vector<animpack>& storeline, int& startline, vecstr& animdatafile, string filename, string header);
void CRC32Process(vector<crc32>& storeline, int& startline, vecstr& animdatafile, string filename, string header, string projectPath);
size_t wordFind(string line, string word, bool isLast = false);

AnimSetDataProject::AnimSetDataProject(int& startline, vecstr& animdatafile, string filename, string projectname)
{
	// data list
	DataPackProcess(datalist, startline, animdatafile, filename);
	project = projectname;
	++startline;
	string projectPath;

	for (auto it = datalist.begin(); it != datalist.end(); ++it)
	{
		++startline;

		if (startline >= int(animdatafile.size()))
		{
			cout << "Missing information in \"animationsetdatasinglefile.txt\" (Project: " << filename << ")" << endl;
			Error = true;
			return;
		}

		// equip list
		EquipPackProcess(it->second.equiplist, startline, animdatafile, filename, it->first);

		// type list
		TypePackProcess(it->second.typelist, startline, animdatafile, filename, it->first);

		// anim list
		AnimPackProcess(it->second.animlist, startline, animdatafile, filename, it->first);

		// crc3 list
		CRC32Process(it->second.crc32list, startline, animdatafile, filename, it->first, projectPath);
	}
}

void DataPackProcess(map<string, datapack, alphanum_less>& storeline, int& startline, vecstr& animdatafile, string filename)
{
	if (startline >= int(animdatafile.size()))
	{
		cout << "Missing information in \"animationsetdatasinglefile.txt\" (Project: " << filename << "Header: Header)" << endl;
		Error = true;
		return;
	}
	else if (!isOnlyNumber(animdatafile[startline]))
	{
		cout << "Invalid format has been detected in \"animationsetdatasinglefile.txt\" (Project: " << filename << "Header: Header)" << endl;
		Error = true;
		return;
	}

	for (int i = startline + 1; i < int(animdatafile.size()); ++i)
	{
		datapack newDataPack;
		newDataPack.proxy = false;
		storeline[animdatafile[i]] = newDataPack;

		if (i + 4 >= int(animdatafile.size()))
		{
			cout << "Missing information in \"animationsetdatasinglefile.txt\" (Project: " << filename << "Header: Header)" << endl;
			Error = true;
			return;
		}

		if (boost::iequals(animdatafile[i + 1], "V3"))
		{
			startline = i;
			break;
		}
	}
}

void EquipPackProcess(std::vector<equip>& storeline, int& startline, vecstr& animdatafile, string filename, string header)
{
	if (startline >= int(animdatafile.size()))
	{
		cout << "Missing information in \"animationsetdatasinglefile.txt\" (Project: " << filename << "Header: " << header << ")" << endl;
		Error = true;
		return;
	}
	else if (!isOnlyNumber(animdatafile[startline]))
	{
		cout << "Invalid format has been detected in \"animationsetdatasinglefile.txt\" (Project: " << filename << "Header: " << header << ")" << endl;
		Error = true;
		return;
	}

	for (int i = startline + 1; i < int(animdatafile.size()); ++i)
	{
		if (isOnlyNumber(animdatafile[i]))
		{
			startline = i;
			break;
		}

		equip tempEquip;
		tempEquip.proxy = false;
		tempEquip.name = animdatafile[i];
		storeline.push_back(tempEquip);

		if (i + 1 >= int(animdatafile.size()))
		{
			cout << "Missing information in \"animationsetdatasinglefile.txt\" (Project: " << filename << "Header: " << header << ")" << endl;
			Error = true;
			return;
		}
	}
}

void TypePackProcess(vector<typepack>& storeline, int& startline, vecstr& animdatafile, string filename, string header)
{
	if (startline >= int(animdatafile.size()))
	{
		cout << "Missing information in \"animationsetdatasinglefile.txt\" (Project: " << filename << "Header: " << header << ")" << endl;
		Error = true;
		return;
	}
	else if (!isOnlyNumber(animdatafile[startline]))
	{
		cout << "Invalid format has been detected in \"animationsetdatasinglefile.txt\" (Project: " << filename << "Header: " << header << ")" << endl;
		Error = true;
		return;
	}

	int counter = 0;

	for (int i = startline + 1; i < int(animdatafile.size()); ++i)
	{
		if (isOnlyNumber(animdatafile[i]))
		{
			if (i + 1 < int(animdatafile.size()) && hasAlpha(animdatafile[i + 1]))
			{
				startline = i;
				break;
			}
			else if (i + 4 < int(animdatafile.size()) && animdatafile[i + 4] == "7891816")
			{
				startline = i;
				break;
			}
			else if (i + 2 < int(animdatafile.size()) && boost::iequals(animdatafile[i + 2], "V3"))
			{
				startline = i;
				break;
			}
			else if (i + 2 == animdatafile.size())
			{
				startline = i;
				break;
			}
		}

		typepack tempTP;
		tempTP.proxy = false;

		if (isOnlyNumber(animdatafile[i]) || !hasAlpha(animdatafile[i]))
		{
			cout << "Invalid format has been detected in \"animationsetdatasinglefile.txt\" (Project: " << filename << "Header: " << header << ")" << endl;
			Error = true;
			return;
		}

		tempTP.name = animdatafile[i];

		if (++i >= int(animdatafile.size()))
		{
			cout << "Missing information in \"animationsetdatasinglefile.txt\" (Project: " << filename << "Header: " << header << ")" << endl;
			Error = true;
			return;
		}
		else if (!isOnlyNumber(animdatafile[i]))
		{
			cout << "Invalid format has been detected in \"animationsetdatasinglefile.txt\" (Project: " << filename << "Header: " << header << ")" << endl;
			Error = true;
			return;
		}

		tempTP.equiptype1 = animdatafile[i];

		if (++i >= int(animdatafile.size()))
		{
			cout << "Missing information in \"animationsetdatasinglefile.txt\" (Project: " << filename << "Header: " << header << ")" << endl;
			Error = true;
			return;
		}
		else if (!isOnlyNumber(animdatafile[i]))
		{
			cout << "Invalid format has been detected in \"animationsetdatasinglefile.txt\" (Project: " << filename << "Header: " << header << ")" << endl;
			Error = true;
			return;
		}

		tempTP.equiptype2 = animdatafile[i];
		storeline.push_back(tempTP);

		if (i + 1 >= int(animdatafile.size()))
		{
			cout << "Missing information in \"animationsetdatasinglefile.txt\" (Project: " << filename << "Header: " << header << ")" << endl;
			Error = true;
			return;
		}
	}
}

void AnimPackProcess(vector<animpack>& storeline, int& startline, vecstr& animdatafile, string filename, string header)
{
	if (startline >= int(animdatafile.size()))
	{
		cout << "Missing information in \"animationsetdatasinglefile.txt\" (Project: " << filename << "Header: " << header << ")" << endl;
		Error = true;
		return;
	}
	else if (!isOnlyNumber(animdatafile[startline]))
	{
		cout << "Invalid format has been detected in \"animationsetdatasinglefile.txt\" (Project: " << filename << "Header: " << header << ")" << endl;
		Error = true;
		return;
	}

	for (int i = startline + 1; i < int(animdatafile.size()); ++i)
	{
		if (animdatafile[i] == "0")
		{
			if (i + 1 < int(animdatafile.size()) && animdatafile[i + 1] == "V3")
			{
				startline = i;
				break;
			}
			else if (i + 2 < int(animdatafile.size()) && animdatafile[i + 2].find(".txt") != string::npos)
			{
				startline = i;
				break;
			}
		}

		if (animdatafile[i] == "1")
		{
			if (i + 4 < int(animdatafile.size()) && animdatafile[i + 4] == "V3")
			{
				startline = i;
				break;
			}
			else if (i + 5 < int(animdatafile.size()) && animdatafile[i + 5].find(".txt") != string::npos)
			{
				startline = i;
				break;
			}
		}

		if (isOnlyNumber(animdatafile[i]))
		{
			if (i + 3 < int(animdatafile.size()) && animdatafile[i + 3] == "7891816")
			{
				startline = i;
				break;
			}
			else if (i + 1 == animdatafile.size())
			{
				startline = i;
				break;
			}
		}

		animpack tempAP;
		tempAP.proxy = false;
		tempAP.name = animdatafile[i++];
		tempAP.unknown = animdatafile[i++];

		if (i >= int(animdatafile.size()))
		{
			cout << "Missing information in \"animationsetdatasinglefile.txt\" (Project: " << filename << "Header: " << header << ")" << endl;
			Error = true;
			return;
		}
		else if (!isOnlyNumber(animdatafile[i]))
		{
			cout << "Invalid format has been detected in \"animationsetdatasinglefile.txt\" (Project: " << filename << "Header: " << header << ")" << endl;
			Error = true;
			return;
		}

		while (i < int(animdatafile.size()))
		{
			++i;

			if (animdatafile[i] == "0")
			{
				if (i + 1 < int(animdatafile.size()) && animdatafile[i + 1] == "V3")
				{
					--i;
					break;
				}
				else if (i + 2 < int(animdatafile.size()) && animdatafile[i + 2].find(".txt") != string::npos)
				{
					--i;
					break;
				}
			}

			if (animdatafile[i] == "1")
			{
				if (i + 4 < int(animdatafile.size()) && animdatafile[i + 4] == "V3")
				{
					--i;
					break;
				}
				else if (i + 5 < int(animdatafile.size()) && animdatafile[i + 5].find(".txt") != string::npos)
				{
					--i;
					break;
				}
			}

			if (isOnlyNumber(animdatafile[i]))
			{
				if (i + 3 < int(animdatafile.size()) && animdatafile[i + 3] == "7891816")
				{
					--i;
					break;
				}
				else if (i + 1 == animdatafile.size())
				{
					startline = i;
					break;
				}
			}

			if (hasAlpha(animdatafile[i]))
			{
				bool out = false;

				for (int k = i + 1; k < i + 3; ++k)
				{
					if (!isOnlyNumber(animdatafile[k]))
					{
						out = false;
						break;
					}
					else
					{
						out = true;
					}
				}

				if (out)
				{
					if (i + 3 < int(animdatafile.size()) && hasAlpha(animdatafile[i + 3]) && animdatafile[i + 3] != "V3")
					{
						--i;
						break;
					}
				}
			}

			attackdata attack;
			attack.proxy = false;
			tempAP.attack.push_back(attack);
			tempAP.attack.back().data = animdatafile[i];
		}

		storeline.push_back(tempAP);

		if (i + 1 >= int(animdatafile.size()))
		{
			cout << "Missing information in \"animationsetdatasinglefile.txt\" (Project: " << filename << "Header: " << header << ")" << endl;
			Error = true;
			return;
		}
	}
}

void CRC32Process(vector<crc32>& storeline, int& startline, vecstr& animdatafile, string filename, string header, string projectPath)
{
	if (startline >= int(animdatafile.size()))
	{
		cout << "Missing information in \"animationsetdatasinglefile.txt\" (Project: " << filename << "Header: " << header << ")" << endl;
		Error = true;
		return;
	}
	else if (!isOnlyNumber(animdatafile[startline]))
	{
		cout << "Invalid format has been detected in \"animationsetdatasinglefile.txt\" (Project: " << filename << "Header: " << header << ")" << endl;
		Error = true;
		return;
	}

	vector<crc32> newCRC;

	for (int i = startline + 1; i < int(animdatafile.size()); ++i)
	{
		if (animdatafile[i] == "V3")
		{
			startline = i;
			break;
		}
		else if (i + 1 < int(animdatafile.size()) && animdatafile[i + 1].find(".txt") != string::npos)
		{
			startline = i;
			break;
		}

		crc32 tempCRC32;
		tempCRC32.proxy = false;

		if (!isOnlyNumber(animdatafile[i]))
		{
			cout << "Invalid format has been detected in \"animationsetdatasinglefile.txt\" (Project: " << filename << "Header: " << header << ")" << endl;
			Error = true;
			return;
		}

		tempCRC32.filepath = animdatafile[i];

		if (++i >= int(animdatafile.size()))
		{
			cout << "Missing information in \"animationsetdatasinglefile.txt\" (Project: " << filename << "Header: " << header << ")" << endl;
			Error = true;
			return;
		}
		else if (!isOnlyNumber(animdatafile[i]))
		{
			cout << "Invalid format has been detected in \"animationsetdatasinglefile.txt\" (Project: " << filename << "Header: " << header << ")" << endl;
			Error = true;
			return;
		}

		tempCRC32.filename = animdatafile[i];

		if (++i >= int(animdatafile.size()))
		{
			cout << "Missing information in \"animationsetdatasinglefile.txt\" (Project: " << filename << "Header: " << header << ")" << endl;
			Error = true;
			return;
		}
		else if (!isOnlyNumber(animdatafile[i]))
		{
			cout << "Invalid format has been detected in \"animationsetdatasinglefile.txt\" (Project: " << filename << "Header: " << header << ")" << endl;
			Error = true;
			return;
		}

		tempCRC32.fileformat = animdatafile[i];
		storeline.push_back(tempCRC32);

		if (i + 1 >= int(animdatafile.size()))
		{
			startline = i;
		}
	}

	storeline.insert(storeline.end(), newCRC.begin(), newCRC.end());
}

void animSetDataInitialize(string filepath, vecstr& catalyst, vector<shared_ptr<AnimSetDataProject>>& ASDProject)
{
	int projectcounter = 0;
	unsigned int headercounter = 0;

	string project = "	";
	string header = project;
	string line;

	vecstr newline;
	vecstr storeline;
	vecstr projectList;

	unordered_map<string, map<string, vecstr, alphanum_less>> ASDPack;

	while (catalyst.back().length() == 0)
	{
		catalyst.pop_back();
	}

	// add picked behavior and remove not picked behavior 
	// separation of all items for easier access and better compatibility
	for (unsigned int l = 0; l < catalyst.size(); ++l)
	{
		storeline.push_back(catalyst[l]);
	}

	for (unsigned int i = 1; i < catalyst.size(); ++i)
	{
		if (isOnlyNumber(catalyst[i]))
		{
			break;
		}

		projectList.push_back(catalyst[i]);
		newline.push_back(catalyst[i]);
	}

	unordered_map<string, vecstr> animDataSetHeader;
	animDataSetHeader[project].push_back(header);

	int num;

	for (unsigned int i = projectList.size() + 1; i < storeline.size(); ++i)
	{
		line = storeline[i];

		if (i != storeline.size() - 1 && wordFind(storeline[i + 1], ".txt") != string::npos)
		{
			if (i != projectList.size() + 1)
			{
				num = i;
				break;
			}
			else
			{
				newline.reserve(100);
				newline.clear();
				project = projectList[projectcounter];
				++projectcounter;
				headercounter = 0;
				animDataSetHeader[project].push_back("	");
				newline.push_back(storeline[i]);
				++i;
			}

			if (animDataSetHeader[project].size() != 1)
			{
				cout << "ERROR: BUG FOUND!! Report to Nemesis' author immediately (File: " << filepath << ", Line: " << i + 1 << endl;
				Error = true;
				return;
			}

			while (i < storeline.size())
			{
				if (wordFind(storeline[i], ".txt") != string::npos)
				{
					string curHeader = storeline[i];
					animDataSetHeader[project].push_back(curHeader);
				}
				else if (wordFind(storeline[i], "V3") != string::npos)
				{
					if (headercounter >= animDataSetHeader[project].size())
					{
						cout << "ERROR: Un-registered header has been detected in \"animationsetdatasinglefile.txt\" (File: " << filepath << ", Line: " << i + 1 << endl;
						Error = true;
						return;
					}

					header = animDataSetHeader[project][headercounter];
					++headercounter;

					if (header != "	" || project == "	")
					{
						newline.shrink_to_fit();
						ASDPack[project][header] = newline;
					}

					newline.reserve(100);
					newline.clear();
					break;
				}
				else
				{
					cout << "ERROR: Invalid format has been detected in \"animationsetdatasinglefile.txt\" (File: " << filepath << ", Line: " << i + 1 << ")" << endl;
					Error = true;
					return;
				}

				newline.push_back(storeline[i]);
				++i;
			}
		}
		else if (wordFind(storeline[i], "V3") != string::npos)
		{
			if (headercounter >= animDataSetHeader[project].size())
			{
				cout << "ERROR: Un-registered header has been detected in \"animationsetdatasinglefile.txt\" (File: " << filepath << ", Line: " << i + 1 << endl;
				Error = true;
				return;
			}

			header = animDataSetHeader[project][headercounter];
			++headercounter;

			if (header != "	" || project == "	")
			{
				newline.shrink_to_fit();
				ASDPack[project][header] = newline;
			}

			newline.reserve(100);
			newline.clear();
		}

		if (Error)
		{
			return;
		}

		newline.push_back(storeline[i]);
	}

	for (unsigned int i = num; i < storeline.size(); ++i)
	{
		line = storeline[i];

		if (i != storeline.size() - 1 && wordFind(storeline[i + 1], ".txt") != string::npos)
		{
			header = animDataSetHeader[project][headercounter];

			if (header != "	" || project == "	")
			{
				newline.shrink_to_fit();
				ASDPack[project][header] = newline;
			}

			newline.reserve(100);
			newline.clear();
			project = projectList[projectcounter];
			++projectcounter;
			headercounter = 0;
			animDataSetHeader[project].push_back("	");
			newline.push_back(storeline[i]);
			++i;

			if (animDataSetHeader[project].size() != 1)
			{
				cout << "ERROR: BUG FOUND!! Report to Nemesis' author immediately (File: " << filepath << ", Line: " << i + 1 << endl;
				Error = true;
				return;
			}

			while (i < storeline.size())
			{
				if (wordFind(storeline[i], ".txt") != string::npos)
				{
					string curHeader = storeline[i];
					animDataSetHeader[project].push_back(curHeader);
				}
				else if (wordFind(storeline[i], "V3") != string::npos)
				{
					if (headercounter >= animDataSetHeader[project].size())
					{
						cout << "ERROR: Un-registered header has been detected in \"animationsetdatasinglefile.txt\" (File: " << filepath << ", Line: " << i + 1 << endl;
						Error = true;
						return;
					}

					header = animDataSetHeader[project][headercounter];
					++headercounter;

					if (header != "	" || project == "	")
					{
						newline.shrink_to_fit();
						ASDPack[project][header] = newline;
					}

					newline.reserve(100);
					newline.clear();
					break;
				}
				else
				{
					cout << "ERROR: Invalid format has been detected in \"animationsetdatasinglefile.txt\" (File: " << filepath << ", Line: " << i + 1 << ")" << endl;
					Error = true;
					return;
				}

				newline.push_back(storeline[i]);
				++i;
			}
		}
		else if (wordFind(storeline[i], "V3") != string::npos)
		{
			if (headercounter >= animDataSetHeader[project].size() - 1)
			{
				cout << "ERROR: Un-registered header has been detected in \"animationsetdatasinglefile.txt\" (File: " << filepath << ", Line: " << i + 1 << endl;
				Error = true;
				return;
			}

			header = animDataSetHeader[project][headercounter];
			++headercounter;

			if (header != "	" || project == "	")
			{
				newline.shrink_to_fit();
				ASDPack[project][header] = newline;
			}

			newline.reserve(100);
			newline.clear();
		}

		if (Error)
		{
			return;
		}

		newline.push_back(storeline[i]);
	}

	if (Error)
	{
		return;
	}

	if (newline.size() != 0)
	{
		header = animDataSetHeader[project][headercounter];

		if (header != "	" || project == "	")
		{
			newline.shrink_to_fit();
			ASDPack[project][header] = newline;
		}
	}

	for (unsigned int i = 0; i < projectList.size(); ++i)
	{
		int startline = 0;
		vecstr projectline;
		projectline.push_back(to_string(ASDPack[projectList[i]].size()));

		for (auto it = ASDPack[projectList[i]].begin(); it != ASDPack[projectList[i]].end(); ++it)
		{
			projectline.push_back(it->first);
		}

		for (auto it = ASDPack[projectList[i]].begin(); it != ASDPack[projectList[i]].end(); ++it)
		{
			projectline.insert(projectline.end(), it->second.begin(), it->second.end());
		}

		AnimSetDataProject newProject(startline, projectline, filepath, projectList[i]);
		newProject.proxy = false;
		ASDProject.push_back(make_shared<AnimSetDataProject>(newProject));

		if (Error)
		{
			return;
		}
	}
}

size_t wordFind(string line, string word, bool isLast)
{
	boost::algorithm::to_lower(line);
	boost::algorithm::to_lower(word);

	if (isLast)
	{
		size_t pos = 0;
		int ref = sameWordCount(line, word);

		if (ref != 0)
		{
			for (int i = 0; i < ref; ++i)
			{
				pos = line.find(word, pos + 1);
			}

			return pos;
		}

		return string::npos;
	}

	return line.find(word);
}

