#include <boost/thread.hpp>
#include <boost/asio/post.hpp>
#include <boost/asio/thread_pool.hpp>

#include "animsetdatacore.h"
#include "animsetdatamatch.h"
#include "readdirectory.hpp"

#include "src/utilities/nemesisformat.h"


using namespace std;

void ProxyProjectSet(int counter, string folder);
void ProjectSetProcessing(int i, string folder);
void AnimSetDataInput(datapack& data, vecstr& output, vecstr& storeline);

void animSetDataProcess()
{
	int projectcount = 0;
	bool IsEdited = false;
	bool open = false;
	vecstr output;
	vecstr storeline;
	string folder = "mod\\" + modcode + "\\animationsetdatasinglefile\\";

	if (!matchProjectScoring(AnimSetDataOriginal, AnimSetDataEdited, targetfilenameedited)) return;
	
	unsigned int size = AnimSetDataOriginal.size();

	for (unsigned int i = 0; i < size; ++i)
	{
		if (AnimSetDataOriginal[i]->proxy)		// newly added
		{
			nemesis::try_open(open, IsEdited, output);

			while (i < size)
			{
				output.push_back(AnimSetDataEdited[i]->project);
				++i;
			}
		}
		else if (AnimSetDataEdited[i]->proxy)	// deleted
		{
			nemesis::try_open(open, IsEdited, output);

			while (i < size)
			{
				if (!AnimSetDataEdited[i]->proxy)
				{
					--i;
					break;
				}

				output.push_back("//* delete this line *//");
				storeline.push_back(AnimSetDataOriginal[i]->project);
				++i;
			}
		}
		else
		{
			nemesis::try_close(open, output, storeline);
			output.push_back(AnimSetDataOriginal[i]->project);
		}
	}

	nemesis::try_close(open, output, storeline);

	if (IsEdited)
	{
		if (!createDirectories(folder + "\\$header$"))
		{
			cout << "ERROR: Failed to create directory for animationsetdata header file (Project: $header$, Header: $header$)" << endl;
			Error = true;
			return;
		}

		ofstream outputfile(folder + "\\$header$\\" + "$header$.txt");

		if (outputfile.is_open())
		{
			FunctionWriter fwriter(&outputfile);

			for (auto& line : output)
			{
				fwriter << line << "\n";
			}

			outputfile.close();
			IsEdited = false;
		}
		else
		{
			cout << "ERROR: Failed to output animatiosetndata header file (Project: $header$, Header: $header$)" << endl;
			Error = true;
			return;
		}
	}

	output.clear();
	boost::asio::thread_pool multithreads;

	for (unsigned int i = 0; i < AnimSetDataOriginal.size(); ++i)
	{
		if (AnimSetDataOriginal[i]->proxy)		// added project
		{
			boost::asio::post(multithreads, boost::bind(ProxyProjectSet, i, folder));
		}
		else if (AnimSetDataEdited[i]->proxy)		// deleted project
		{
			// project list will take care of this during main tool runtime
			// it will be done by detecting //* delete this line *//
			// if detected, that missing/deleted project will be omitted from data filling

			// increase size as there is no function to be initialized
		}
		else
		{
			boost::asio::post(multithreads, boost::bind(ProjectSetProcessing, i, folder));
		}

		if (Error) break;
	}

	multithreads.join();
}

void ProxyProjectSet(int counter, string folder)
{
	auto animSet = AnimSetDataEdited[counter];
	string projectname = animSet->project.substr(0, animSet->project.find_last_of("."));

	while (projectname.find("\\") != string::npos)
	{
		projectname.replace(projectname.find("\\"), 1, "~");
	}

	while (projectname.find("/") != string::npos)
	{
		projectname.replace(projectname.find("/"), 1, "~");
	}

	if (!createDirectories(folder + projectname))
	{
		cout << "ERROR: Failed to create output directory for animationsetdata (Project: " + projectname + ", Path: " + folder + projectname + ")" << endl;
		Error = true;
		return;
	}

	ofstream outputfile(folder + projectname + "\\" + "$header$.txt");

	if (outputfile.is_open())
	{
		FunctionWriter fwriter(&outputfile);

		fwriter << to_string(animSet->datalist.size()) << "\n";

		for (auto& data : animSet->datalist)
		{
			fwriter << data.first << "\n";
		}

		outputfile.close();
	}
	else
	{
		cout << "ERROR: Failed to output animationsetdata header file (Project: " << animSet->project << ", Header: $header$)" << endl;
		Error = true;
		return;
	}

	outputfile.close();

	for (auto& data : animSet->datalist)
	{
		string name = data.first;
		ofstream output(folder + projectname + "\\" + name);

		if (!output.is_open())
		{
			cout << "ERROR: Failed to output animationsetdata header file (Project: " << animSet->project << ", Header: " << name << ")" << endl;
			Error = true;
			return;
		}

		FunctionWriter fwriter(&output);

		fwriter << "V3" << "\n";
		fwriter << to_string(data.second.equiplist.size()) << "\n";

		for (auto& equip : data.second.equiplist)
		{
			fwriter << equip.name << "\n";
		}

		fwriter << to_string(data.second.typelist.size()) << "\n";

		for (auto& type : data.second.typelist)
		{
			fwriter << type.name << "\n";
			fwriter << type.equiptype1 << "\n";
			fwriter << type.equiptype2 << "\n";
		}

		fwriter << to_string(data.second.animlist.size()) << "\n";

		for (auto& anim : data.second.animlist)
		{
			fwriter << anim.name << "\n";
			fwriter << anim.unknown << "\n";
			fwriter << to_string(anim.attack.size()) << "\n";

			for (auto& atk : anim.attack)
			{
				fwriter << atk.data << "\n";
			}
		}

		fwriter << to_string(data.second.crc32list.size()) << "\n";

		for (auto& crc32 : data.second.crc32list)
		{
			fwriter << crc32.filepath << "\n";
			fwriter << crc32.filename << "\n";
			fwriter << crc32.fileformat << "\n";
		}

		output.close();
	}
}

void ProjectSetProcessing(int i, string folder)
{
	string projectname = AnimSetDataOriginal[i]->project.substr(0, AnimSetDataOriginal[i]->project.find_last_of("."));

	while (projectname.find("\\") != string::npos)
	{
		projectname.replace(projectname.find("\\"), 1, "~");
	}

	while (projectname.find("/") != string::npos)
	{
		projectname.replace(projectname.find("/"), 1, "~");
	}

	projectname = projectname.substr(0, projectname.find_last_of("."));

	vecstr storeline;
	vecstr output;
	bool IsEdited = false;
	bool open = false;

	if (!createDirectories(folder + projectname))
	{
		cout << "ERROR: Failed to create new folder (Folder: " << folder << projectname << ")" << endl;
		Error = true;
		return;
	}

	output.push_back(to_string(AnimSetDataOriginal[i]->datalist.size()));

	if (!matchDataScoring(AnimSetDataOriginal[i]->datalist, AnimSetDataEdited[i]->datalist, targetfilenameedited)) return;

	for (auto it = AnimSetDataOriginal[i]->datalist.begin(); it != AnimSetDataOriginal[i]->datalist.end(); ++it)
	{
		if (it->second.proxy)
		{
			nemesis::try_open(open, IsEdited, output);

			while (it != AnimSetDataOriginal[i]->datalist.end())
			{
				if (!it->second.proxy)
				{
					--it;
					break;
				}

				output.push_back(it->first);
				++it;
			}
		}
		else if (AnimSetDataEdited[i]->datalist[it->first].proxy)
		{
			nemesis::try_open(open, IsEdited, output);

			while (it != AnimSetDataEdited[i]->datalist.end())
			{
				if (!AnimSetDataEdited[i]->datalist[it->first].proxy)
				{
					--it;
					break;
				}

				output.push_back("//* delete this line *//");
				storeline.push_back(it->first);
				++it;
			}
		}
		else
		{
			nemesis::try_close(open, output, storeline);
			output.push_back(it->first);
		}
	}

	nemesis::try_close(open, output, storeline);

	if (IsEdited)
	{
		ofstream outputfile(folder + projectname + "\\" + "$header$.txt");

		if (outputfile.is_open())
		{
			FunctionWriter fwriter(&outputfile);

			for (auto& curline : output)
			{
				fwriter << curline << "\n";
			}

			outputfile.close();
			IsEdited = false;
		}
		else
		{
			cout << "ERROR: Failed to output animationsetdata header file (Project: " << AnimSetDataEdited[i]->project << ", Header: $header$)" << endl;
			Error = true;
			return;
		}
	}

	if (Error) return;

	output.clear();
	storeline.clear();

	for (auto& data : AnimSetDataOriginal[i]->datalist)
	{
		if (data.second.proxy)		// new data
		{
			IsEdited = true;
			AnimSetDataInput(AnimSetDataEdited[i]->datalist[data.first], storeline, output);
			storeline.clear();
		}
		else if (AnimSetDataEdited[i]->datalist[data.first].proxy)		// deleted data
		{
			output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
			IsEdited = true;
			open = false;
			AnimSetDataInput(data.second, output, storeline);
		}
		else
		{
			output.push_back("V3");
			output.push_back(to_string(data.second.equiplist.size()));

			if (!matchNameScoring(data.second.equiplist, AnimSetDataEdited[i]->datalist[data.first].equiplist, targetfilenameedited)) return;

			for (unsigned int j = 0; j < data.second.equiplist.size(); ++j)
			{
				if (data.second.equiplist[j].proxy)		// new equip
				{
					nemesis::try_open(open, IsEdited, output);

					while (j < data.second.equiplist.size())
					{
						if (!data.second.equiplist[j].proxy)
						{
							--j;
							break;
						}

						output.push_back(AnimSetDataEdited[i]->datalist[data.first].equiplist[j].name);
						++j;
					}

					nemesis::try_close(open, output, storeline);
				}
				else if (AnimSetDataEdited[i]->datalist[data.first].equiplist[j].proxy)		// deleted equip
				{
					nemesis::try_open(open, IsEdited, output);

					while (j < data.second.equiplist.size())
					{
						if (!AnimSetDataEdited[i]->datalist[data.first].equiplist[j].proxy)
						{
							--j;
							break;
						}

						output.push_back("//* delete this line *//");
						storeline.push_back(data.second.equiplist[j].name);
						++j;
					}
				}
				else
				{
					if (data.second.equiplist[j].name != AnimSetDataEdited[i]->datalist[data.first].equiplist[j].name)
					{
						nemesis::try_open(open, IsEdited, output);
						output.push_back(AnimSetDataEdited[i]->datalist[data.first].equiplist[j].name);
						storeline.push_back(data.second.equiplist[j].name);
					}
					else
					{
						nemesis::try_close(open, output, storeline);
						output.push_back(data.second.equiplist[j].name);
					}
				}
			}

			nemesis::try_close(open, output, storeline);

			// typelist
			// size of typelist
			// name
			// equiptype1
			// equiptype2
			// name2
			// ...

			output.push_back(to_string(data.second.typelist.size()));

			if (!matchNameScoring(data.second.typelist, AnimSetDataEdited[i]->datalist[data.first].typelist, targetfilenameedited)) return;

			for (unsigned int j = 0; j < data.second.typelist.size(); ++j)
			{
				if (data.second.typelist[j].proxy)		// new typelist
				{
					nemesis::try_open(open, IsEdited, output);

					while (j < data.second.typelist.size())
					{
						if (!data.second.typelist[j].proxy)
						{
							--j;
							break;
						}

						output.push_back(AnimSetDataEdited[i]->datalist[data.first].typelist[j].name);
						output.push_back(AnimSetDataEdited[i]->datalist[data.first].typelist[j].equiptype1);
						output.push_back(AnimSetDataEdited[i]->datalist[data.first].typelist[j].equiptype2);
						++j;
					}

					nemesis::try_close(open, output, storeline);
				}
				else if (AnimSetDataEdited[i]->datalist[data.first].typelist[j].proxy)		// deleted typelist
				{
					nemesis::try_open(open, IsEdited, output);

					while (j < data.second.typelist.size())
					{
						if (!AnimSetDataEdited[i]->datalist[data.first].typelist[j].proxy)
						{
							--j;
							break;
						}

						output.push_back("//* delete this line *//");
						output.push_back("//* delete this line *//");
						output.push_back("//* delete this line *//");
						storeline.push_back(data.second.typelist[j].name);
						storeline.push_back(data.second.typelist[j].equiptype1);
						storeline.push_back(data.second.typelist[j].equiptype2);
						++j;
					}
				}
				else
				{
					if (data.second.typelist[j].name != AnimSetDataEdited[i]->datalist[data.first].typelist[j].name)
					{
						nemesis::try_open(open, IsEdited, output);
						output.push_back(AnimSetDataEdited[i]->datalist[data.first].typelist[j].name);
						storeline.push_back(data.second.typelist[j].name);
					}
					else
					{
						nemesis::try_close(open, output, storeline);
						output.push_back(data.second.typelist[j].name);
					}

					if (data.second.typelist[j].equiptype1 != AnimSetDataEdited[i]->datalist[data.first].typelist[j].equiptype1)
					{
						nemesis::try_open(open, IsEdited, output);
						output.push_back(AnimSetDataEdited[i]->datalist[data.first].typelist[j].equiptype1);
						storeline.push_back(data.second.typelist[j].equiptype1);
					}
					else
					{
						nemesis::try_close(open, output, storeline);
						output.push_back(data.second.typelist[j].equiptype1);
					}

					if (data.second.typelist[j].equiptype2 != AnimSetDataEdited[i]->datalist[data.first].typelist[j].equiptype2)
					{
						nemesis::try_open(open, IsEdited, output);
						output.push_back(AnimSetDataEdited[i]->datalist[data.first].typelist[j].equiptype2);
						storeline.push_back(data.second.typelist[j].equiptype2);
					}
					else
					{
						nemesis::try_close(open, output, storeline);
						output.push_back(data.second.typelist[j].equiptype2);
					}
				}
			}

			nemesis::try_close(open, output, storeline);

			// animlist
			// size of animlist
			// name
			// unknown
			// size of attack
			// data
			// name2
			// ...

			output.push_back(to_string(data.second.animlist.size()));

			if (!matchNameScoring(data.second.animlist, AnimSetDataEdited[i]->datalist[data.first].animlist, targetfilenameedited)) return;

			for (unsigned int j = 0; j < data.second.animlist.size(); ++j)
			{
				if (data.second.animlist[j].proxy)		// new animlist
				{
					nemesis::try_open(open, IsEdited, output);

					while (j < data.second.animlist.size())
					{
						if (!data.second.animlist[j].proxy)
						{
							--j;
							break;
						}

						output.push_back(AnimSetDataEdited[i]->datalist[data.first].animlist[j].name);
						output.push_back(AnimSetDataEdited[i]->datalist[data.first].animlist[j].unknown);
						output.push_back(to_string(AnimSetDataEdited[i]->datalist[data.first].animlist[j].attack.size()));

						for (auto& atk : AnimSetDataEdited[i]->datalist[data.first].animlist[j].attack)
						{
							output.push_back(atk.data);
						}

						++j;
					}

					nemesis::try_close(open, output, storeline);
				}
				else if (AnimSetDataEdited[i]->datalist[data.first].animlist[j].proxy)		// deleted animlist
				{
					nemesis::try_open(open, IsEdited, output);

					while (j < data.second.animlist.size())
					{
						if (!AnimSetDataEdited[i]->datalist[data.first].animlist[j].proxy)
						{
							--j;
							break;
						}

						output.push_back("//* delete this line *//");
						output.push_back("//* delete this line *//");
						output.push_back("//* delete this line *//");
						storeline.push_back(data.second.animlist[j].name);
						storeline.push_back(data.second.animlist[j].unknown);
						storeline.push_back(to_string(data.second.animlist[j].attack.size()));

						for (auto& atk : data.second.animlist[j].attack)
						{
							output.push_back("//* delete this line *//");
							storeline.push_back(atk.data);
						}

						++j;
					}
				}
				else
				{
					if (data.second.animlist[j].name != AnimSetDataEdited[i]->datalist[data.first].animlist[j].name)
					{
						nemesis::try_open(open, IsEdited, output);
						output.push_back(AnimSetDataEdited[i]->datalist[data.first].animlist[j].name);
						storeline.push_back(data.second.animlist[j].name);
					}
					else
					{
						nemesis::try_close(open, output, storeline);
						output.push_back(data.second.animlist[j].name);
					}

					if (data.second.animlist[j].unknown != AnimSetDataEdited[i]->datalist[data.first].animlist[j].unknown)
					{
						nemesis::try_open(open, IsEdited, output);
						output.push_back(AnimSetDataEdited[i]->datalist[data.first].animlist[j].unknown);
						storeline.push_back(data.second.animlist[j].unknown);
					}
					else
					{
						nemesis::try_close(open, output, storeline);
						output.push_back(data.second.animlist[j].unknown);
					}

					nemesis::try_close(open, output, storeline);
					output.push_back(to_string(data.second.animlist[j].attack.size()));

					if (!matchAtkScoring(data.second.animlist[j].attack, AnimSetDataEdited[i]->datalist[data.first].animlist[j].attack, targetfilename)) return;

					for (unsigned int k = 0; k < data.second.animlist[j].attack.size(); ++k)
					{
						if (data.second.animlist[j].attack[k].proxy)		// new attack
						{
							nemesis::try_open(open, IsEdited, output);

							while (k < data.second.animlist[j].attack.size())
							{
								if (!data.second.animlist[j].attack[k].proxy)
								{
									--k;
									break;
								}

								output.push_back(AnimSetDataEdited[i]->datalist[data.first].animlist[j].attack[k].data);
								++k;
							}

							nemesis::try_close(open, output, storeline);
						}
						else if (AnimSetDataEdited[i]->datalist[data.first].animlist[j].attack[k].proxy)		// deleted attack
						{
							nemesis::try_open(open, IsEdited, output);

							while (k < data.second.equiplist.size())
							{
								if (!AnimSetDataEdited[i]->datalist[data.first].animlist[j].attack[k].proxy)
								{
									--k;
									break;
								}

								output.push_back("//* delete this line *//");
								storeline.push_back(data.second.animlist[j].attack[k].data);
								++k;
							}
						}
						else
						{
							if (data.second.animlist[j].attack[k].data != AnimSetDataEdited[i]->datalist[data.first].animlist[j].attack[k].data)
							{
								nemesis::try_open(open, IsEdited, output);
								output.push_back(AnimSetDataEdited[i]->datalist[data.first].animlist[j].attack[k].data);
								storeline.push_back(data.second.animlist[j].attack[k].data);
							}
							else
							{
								nemesis::try_close(open, output, storeline);
								output.push_back(data.second.animlist[j].attack[k].data);
							}
						}
					}

					nemesis::try_close(open, output, storeline);
				}
			}

			nemesis::try_close(open, output, storeline);
			output.push_back(to_string(data.second.crc32list.size()));

			if (!matchCRC32Scoring(data.second.crc32list, AnimSetDataEdited[i]->datalist[data.first].crc32list, targetfilenameedited)) return;

			for (unsigned int j = 0; j < data.second.crc32list.size(); ++j)
			{
				if (data.second.crc32list[j].proxy)		// new crc32list
				{
					nemesis::try_open(open, IsEdited, output);

					while (j < data.second.crc32list.size())
					{
						if (!data.second.crc32list[j].proxy)
						{
							--j;
							break;
						}

						output.push_back(AnimSetDataEdited[i]->datalist[data.first].crc32list[j].filepath);
						output.push_back(AnimSetDataEdited[i]->datalist[data.first].crc32list[j].filename);
						output.push_back(AnimSetDataEdited[i]->datalist[data.first].crc32list[j].fileformat);
						++j;
					}

					nemesis::try_close(open, output, storeline);
				}
				else if (AnimSetDataEdited[i]->datalist[data.first].crc32list[j].proxy)		// deleted crc32list
				{
					nemesis::try_open(open, IsEdited, output);

					while (j < data.second.crc32list.size())
					{
						if (!AnimSetDataEdited[i]->datalist[data.first].crc32list[j].proxy)
						{
							--j;
							break;
						}

						output.push_back("//* delete this line *//");
						output.push_back("//* delete this line *//");
						output.push_back("//* delete this line *//");
						storeline.push_back(data.second.crc32list[j].filepath);
						storeline.push_back(data.second.crc32list[j].filename);
						storeline.push_back(data.second.crc32list[j].fileformat);
						++j;
					}
				}
				else
				{
					if (data.second.crc32list[j].filepath != AnimSetDataEdited[i]->datalist[data.first].crc32list[j].filepath)
					{
						nemesis::try_open(open, IsEdited, output);
						output.push_back(AnimSetDataEdited[i]->datalist[data.first].crc32list[j].filepath);
						storeline.push_back(data.second.crc32list[j].filepath);
					}
					else
					{
						nemesis::try_close(open, output, storeline);
						output.push_back(data.second.crc32list[j].filepath);
					}

					if (data.second.crc32list[j].filename != AnimSetDataEdited[i]->datalist[data.first].crc32list[j].filename)
					{
						nemesis::try_open(open, IsEdited, output);
						output.push_back(AnimSetDataEdited[i]->datalist[data.first].crc32list[j].filename);
						storeline.push_back(data.second.crc32list[j].filename);
					}
					else
					{
						nemesis::try_close(open, output, storeline);
						output.push_back(data.second.crc32list[j].filename);
					}

					if (data.second.crc32list[j].fileformat != AnimSetDataEdited[i]->datalist[data.first].crc32list[j].fileformat)
					{
						nemesis::try_open(open, IsEdited, output);
						output.push_back(AnimSetDataEdited[i]->datalist[data.first].crc32list[j].fileformat);
						storeline.push_back(data.second.crc32list[j].fileformat);
					}
					else
					{
						nemesis::try_close(open, output, storeline);
						output.push_back(data.second.crc32list[j].fileformat);
					}
				}
			}

			nemesis::try_close(open, output, storeline);
		}

		nemesis::try_close(open, output, storeline);

		if (IsEdited)
		{
			ofstream outputfile(folder + projectname + "\\" + (data.second.proxy ? modcode + "$" : "") + data.first);

			if (outputfile.is_open())
			{
				FunctionWriter fwriter(&outputfile);

				for (auto& curline : output)
				{
					fwriter << curline << "\n";
				}

				outputfile.close();
				IsEdited = false;
			}
			else
			{
				cout << "ERROR: Failed to output animationsetdata header file (Project: " << AnimSetDataEdited[i]->project << ", Header: $header$)" << endl;
				Error = true;
				return;
			}
		}

		if (Error) return;

		output.clear();
	}

	vecstr filelist;
	read_directory(folder + projectname, filelist);

	if (filelist.size() == 0)
	{
		if (!boost::filesystem::remove(folder + projectname))
		{
			cout << "WARNING: Failed to remove folder" << endl;
		}
	}

	if (Error) return;
}

void AnimSetDataInput(datapack& data, vecstr& output, vecstr& storeline)
{
	output.push_back("//* delete this line *//");
	storeline.push_back("V3");
	output.push_back("//* delete this line *//");
	storeline.push_back(to_string(data.equiplist.size()));

	for (auto& equip : data.equiplist)
	{
		output.push_back("//* delete this line *//");
		storeline.push_back(equip.name);
	}

	output.push_back("//* delete this line *//");
	storeline.push_back(to_string(data.typelist.size()));

	for (auto& typelist : data.typelist)
	{
		output.push_back("//* delete this line *//");
		storeline.push_back(typelist.name);
		output.push_back("//* delete this line *//");
		storeline.push_back(typelist.equiptype1);
		output.push_back("//* delete this line *//");
		storeline.push_back(typelist.equiptype2);
	}

	output.push_back("//* delete this line *//");
	storeline.push_back(to_string(data.animlist.size()));

	for (auto& animlist : data.animlist)
	{
		output.push_back("//* delete this line *//");
		output.push_back("//* delete this line *//");
		output.push_back("//* delete this line *//");
		storeline.push_back(animlist.name);
		storeline.push_back(animlist.unknown);
		storeline.push_back(to_string(animlist.attack.size()));

		for (auto& atk : animlist.attack)
		{
			output.push_back("//* delete this line *//");
			storeline.push_back(atk.data);
		}
	}

	output.push_back("//* delete this line *//");
	storeline.push_back(to_string(data.crc32list.size()));

	for (auto& crc32list : data.crc32list)
	{
		output.push_back("//* delete this line *//");
		output.push_back("//* delete this line *//");
		output.push_back("//* delete this line *//");
		storeline.push_back(crc32list.filepath);
		storeline.push_back(crc32list.filename);
		storeline.push_back(crc32list.fileformat);
	}
}
