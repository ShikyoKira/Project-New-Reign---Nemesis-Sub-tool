#include "animsetdatacore.h"
#include "animsetdatamatch.h"
#include "readdirectory.hpp"
#include <boost\thread.hpp>

using namespace std;

void ProxyProjectSet(int counter, string folder);
void ProjectSetProcessing(int i, string folder);
void AnimSetDataInput(datapack& data, vecstr& output, vecstr& storeline = *new vecstr);

void animSetDataProcess()
{
	int projectcount = 0;
	bool IsEdited = false;
	bool open = false;
	vecstr output;
	vecstr storeline;
	string folder = "mod\\" + modcode + "\\animationsetdatasinglefile\\";

	if (!matchProjectScoring(AnimSetDataOriginal, AnimSetDataEdited, targetfilenameedited))
	{
		return;
	}
	
	for (unsigned int i = 0; i < AnimSetDataOriginal.size(); ++i)
	{
		if (AnimSetDataOriginal[i]->proxy)
		{
			if (!open)
			{
				output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
				IsEdited = true;
				open = true;
			}

			while (AnimSetDataOriginal[i]->proxy)
			{
				output.push_back(AnimSetDataEdited[i]->project);
				++i;
			}

			--i;
		}
		else if (AnimSetDataEdited[i]->proxy)
		{
			if (!open)
			{
				output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
				IsEdited = true;
				open = true;
			}

			while (AnimSetDataEdited[i]->proxy)
			{
				output.push_back("//* delete this line *//");
				storeline.push_back(AnimSetDataOriginal[i]->project);
				++i;
			}

			--i;
		}
		else
		{
			if (open)
			{
				if (storeline.size() > 0)
				{
					output.push_back("<!-- ORIGINAL -->");
					output.insert(output.end(), storeline.begin(), storeline.end());
				}

				output.push_back("<!-- CLOSE -->");
				storeline.clear();
				open = false;
			}

			output.push_back(AnimSetDataOriginal[i]->project);
		}
	}

	if (open)
	{
		if (storeline.size() > 0)
		{
			output.push_back("<!-- ORIGINAL -->");
			output.insert(output.end(), storeline.begin(), storeline.end());
		}

		output.push_back("<!-- CLOSE -->");
		storeline.clear();
		open = false;
	}

	if (IsEdited)
	{
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

	for (unsigned int i = 0; i < AnimSetDataOriginal.size();)
	{
		boost::thread_group multithreads;
		size_t size = i + min(AnimSetDataOriginal.size() - i, thread::hardware_concurrency());

		for (unsigned int j = i; j < size; ++j)
		{
			if (AnimSetDataOriginal[j]->proxy)		// added project
			{
				multithreads.create_thread(boost::bind(ProxyProjectSet, j, folder));
			}
			else if (AnimSetDataEdited[j]->proxy)		// deleted project
			{
				// project list will take care of this during main tool runtime
				// it will be done by detecting //* delete this line *//
				// if detected, that missing/deleted project will be omitted from data filling

				// increase size as there is no function to be initialized
				++size;
			}
			else
			{
				multithreads.create_thread(boost::bind(ProjectSetProcessing, j, folder));
			}
		}

		multithreads.join_all();

		if (Error)
		{
			break;
		}

		i = size;
	}
}

void ProxyProjectSet(int counter, string folder)
{
	int i = counter;

	while (AnimSetDataOriginal[i]->proxy)
	{
		string projectname = AnimSetDataEdited[i]->project.substr(0, AnimSetDataEdited[i]->project.find_last_of("."));

		while (projectname.find("\\") != string::npos)
		{
			projectname.replace(projectname.find("\\"), 1, "~");
		}

		while (projectname.find("/") != string::npos)
		{
			projectname.replace(projectname.find("/"), 1, "~");
		}

		if ((CreateDirectory((folder + projectname).c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError()))
		{
			{
				ofstream outputfile(folder + projectname + "\\" + "$header$.txt");

				if (outputfile.is_open())
				{
					FunctionWriter fwriter(&outputfile);

					fwriter << to_string(AnimSetDataEdited[i]->datalist.size()) << "\n";

					for (auto& data : AnimSetDataEdited[i]->datalist)
					{
						fwriter << data.first << "\n";
					}

					outputfile.close();
				}
				else
				{
					cout << "ERROR: Failed to output animationsetdata header file (Project: " << AnimSetDataEdited[i]->project << ", Header: $header$)" << endl;
					Error = true;
					return;
				}
			}

			for (auto& data : AnimSetDataEdited[i]->datalist)
			{
				string name = data.first;
				ofstream output(folder + projectname + "\\" + name + ".txt");

				if (output.is_open())
				{
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
				else
				{
					cout << "ERROR: Failed to output animationsetdata header file (Project: " << AnimSetDataEdited[i]->project << ", Header: " << name << ")" << endl;
					Error = true;
					return;
				}
			}
		}

		++i;
	}

	if (Error)
	{
		return;
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

	if ((CreateDirectory((folder + projectname).c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError()))
	{
		output.push_back(to_string(AnimSetDataOriginal[i]->datalist.size()));

		if (!matchDataScoring(AnimSetDataOriginal[i]->datalist, AnimSetDataEdited[i]->datalist, targetfilenameedited))
		{
			return;
		}

		for (auto it = AnimSetDataOriginal[i]->datalist.begin(); it != AnimSetDataOriginal[i]->datalist.end(); ++it)
		{
			if (it->second.proxy)
			{
				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					IsEdited = true;
					open = true;
				}

				while (it != AnimSetDataOriginal[i]->datalist.end() && it->second.proxy)
				{
					output.push_back(it->first);
					++it;
				}

				--it;
			}
			else if (AnimSetDataEdited[i]->datalist[it->first].proxy)
			{
				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					IsEdited = true;
					open = true;
				}

				while (it != AnimSetDataEdited[i]->datalist.end() && AnimSetDataEdited[i]->datalist[it->first].proxy)
				{
					output.push_back("//* delete this line *//");
					storeline.push_back(it->first);
					++it;
				}

				--it;
			}
			else
			{
				if (open)
				{
					if (storeline.size() > 0)
					{
						output.push_back("<!-- ORIGINAL -->");
						output.insert(output.end(), storeline.begin(), storeline.end());
					}

					output.push_back("<!-- CLOSE -->");
					storeline.clear();
					open = false;
				}

				output.push_back(it->first);
			}
		}

		if (open)
		{
			if (storeline.size() > 0)
			{
				output.push_back("<!-- ORIGINAL -->");
				output.insert(output.end(), storeline.begin(), storeline.end());
			}

			output.push_back("<!-- CLOSE -->");
			storeline.clear();
			open = false;
		}

		if (IsEdited)
		{
			ofstream outputfile(folder + projectname + "\\" + "$header$.txt");

			if (outputfile.is_open())
			{
				FunctionWriter fwriter(&outputfile);

				fwriter << to_string(AnimSetDataEdited[i]->datalist.size()) << "\n";

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

		if (Error)
		{
			return;
		}

		output.clear();
		
		for (auto& data : AnimSetDataOriginal[i]->datalist)
		{
			if (data.second.proxy)		// new data
			{
				output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
				IsEdited = true;
				AnimSetDataInput(AnimSetDataEdited[i]->datalist[data.first], output);
				output.push_back("<!-- CLOSE -->");
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

				if (!matchNameScoring(data.second.equiplist, AnimSetDataEdited[i]->datalist[data.first].equiplist, targetfilenameedited))
				{
					return;
				}

				for (unsigned int j = 0; j < data.second.equiplist.size(); ++j)
				{
					if (data.second.equiplist[j].proxy)		// new equip
					{
						if (!open)
						{
							output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
							IsEdited = true;
						}

						while (j < data.second.equiplist.size())
						{
							if (!data.second.equiplist[j].proxy)
							{
								if (storeline.size() > 0)
								{
									output.push_back("<!-- ORIGINAL -->");
									output.insert(output.end(), storeline.begin(), storeline.end());
									storeline.clear();
								}

								--j;
								break;
							}

							output.push_back(AnimSetDataEdited[i]->datalist[data.first].equiplist[j].name);
							++j;
						}

						output.push_back("<!-- CLOSE -->");
						open = false;
					}
					else if (AnimSetDataEdited[i]->datalist[data.first].equiplist[j].proxy)		// deleted equip
					{
						if (!open)
						{
							output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
							IsEdited = true;
							open = true;
						}

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
							if (!open)
							{
								output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
								IsEdited = true;
								open = true;
							}

							output.push_back(AnimSetDataEdited[i]->datalist[data.first].equiplist[j].name);
							storeline.push_back(data.second.equiplist[j].name);
						}
						else
						{
							if (open)
							{
								output.push_back("<!-- ORIGINAL -->");
								output.insert(output.end(), storeline.begin(), storeline.end());
								output.push_back("<!-- CLOSE -->");
								storeline.clear();
								open = false;
							}

							output.push_back(data.second.equiplist[j].name);
						}
					}
				}

				if (open)
				{
					if (storeline.size() > 0)
					{
						output.push_back("<!-- ORIGINAL -->");
						output.insert(output.end(), storeline.begin(), storeline.end());
						storeline.clear();
					}

					output.push_back("<!-- CLOSE -->");
					open = false;
				}

				// typelist
				// size of typelist
				// name
				// equiptype1
				// equiptype2
				// name2
				// ...

				output.push_back(to_string(data.second.typelist.size()));

				if (!matchNameScoring(data.second.typelist, AnimSetDataEdited[i]->datalist[data.first].typelist, targetfilenameedited))
				{
					return;
				}

				for (unsigned int j = 0; j < data.second.typelist.size(); ++j)
				{
					if (data.second.typelist[j].proxy)		// new typelist
					{
						if (!open)
						{
							output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
							IsEdited = true;
						}

						while (j < data.second.typelist.size())
						{
							if (!data.second.typelist[j].proxy)
							{
								if (storeline.size() > 0)
								{
									output.push_back("<!-- ORIGINAL -->");
									output.insert(output.end(), storeline.begin(), storeline.end());
									storeline.clear();
								}

								--j;
								break;
							}

							output.push_back(AnimSetDataEdited[i]->datalist[data.first].typelist[j].name);
							output.push_back(AnimSetDataEdited[i]->datalist[data.first].typelist[j].equiptype1);
							output.push_back(AnimSetDataEdited[i]->datalist[data.first].typelist[j].equiptype2);
							++j;
						}

						output.push_back("<!-- CLOSE -->");
						open = false;
					}
					else if (AnimSetDataEdited[i]->datalist[data.first].typelist[j].proxy)		// deleted typelist
					{
						if (!open)
						{
							output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
							IsEdited = true;
							open = true;
						}

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
							if (!open)
							{
								output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
								IsEdited = true;
								open = true;
							}

							output.push_back(AnimSetDataEdited[i]->datalist[data.first].typelist[j].name);
							storeline.push_back(data.second.typelist[j].name);
						}
						else
						{
							if (open)
							{
								output.push_back("<!-- ORIGINAL -->");
								output.insert(output.end(), storeline.begin(), storeline.end());
								output.push_back("<!-- CLOSE -->");
								storeline.clear();
								open = false;
							}

							output.push_back(data.second.typelist[j].name);
						}

						if (data.second.typelist[j].equiptype1 != AnimSetDataEdited[i]->datalist[data.first].typelist[j].equiptype1)
						{
							if (!open)
							{
								output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
								IsEdited = true;
								open = true;
							}

							output.push_back(AnimSetDataEdited[i]->datalist[data.first].typelist[j].equiptype1);
							storeline.push_back(data.second.typelist[j].equiptype1);
						}
						else
						{
							if (open)
							{
								output.push_back("<!-- ORIGINAL -->");
								output.insert(output.end(), storeline.begin(), storeline.end());
								output.push_back("<!-- CLOSE -->");
								storeline.clear();
								open = false;
							}

							output.push_back(data.second.typelist[j].equiptype1);
						}

						if (data.second.typelist[j].equiptype2 != AnimSetDataEdited[i]->datalist[data.first].typelist[j].equiptype2)
						{
							if (!open)
							{
								output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
								IsEdited = true;
								open = true;
							}

							output.push_back(AnimSetDataEdited[i]->datalist[data.first].typelist[j].equiptype2);
							storeline.push_back(data.second.typelist[j].equiptype2);
						}
						else
						{
							if (open)
							{
								output.push_back("<!-- ORIGINAL -->");
								output.insert(output.end(), storeline.begin(), storeline.end());
								output.push_back("<!-- CLOSE -->");
								storeline.clear();
								open = false;
							}

							output.push_back(data.second.typelist[j].equiptype2);
						}
					}
				}

				if (open)
				{
					if (storeline.size() > 0)
					{
						output.push_back("<!-- ORIGINAL -->");
						output.insert(output.end(), storeline.begin(), storeline.end());
						storeline.clear();
					}

					output.push_back("<!-- CLOSE -->");
					open = false;
				}

				// animlist
				// size of animlist
				// name
				// unknown
				// size of attack
				// data
				// name2
				// ...

				output.push_back(to_string(data.second.animlist.size()));

				if (!matchNameScoring(data.second.animlist, AnimSetDataEdited[i]->datalist[data.first].animlist, targetfilenameedited))
				{
					return;
				}

				for (unsigned int j = 0; j < data.second.animlist.size(); ++j)
				{
					if (data.second.animlist[j].proxy)		// new animlist
					{
						if (!open)
						{
							output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
							IsEdited = true;
						}

						while (j < data.second.animlist.size())
						{
							if (!data.second.animlist[j].proxy)
							{
								if (storeline.size() > 0)
								{
									output.push_back("<!-- ORIGINAL -->");
									output.insert(output.end(), storeline.begin(), storeline.end());
									storeline.clear();
								}

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

						output.push_back("<!-- CLOSE -->");
						open = false;
					}
					else if (AnimSetDataEdited[i]->datalist[data.first].animlist[j].proxy)		// deleted animlist
					{
						if (!open)
						{
							output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
							IsEdited = true;
							open = true;
						}

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
							if (!open)
							{
								output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
								IsEdited = true;
								open = true;
							}

							output.push_back(AnimSetDataEdited[i]->datalist[data.first].animlist[j].name);
							storeline.push_back(data.second.animlist[j].name);
						}
						else
						{
							if (open)
							{
								output.push_back("<!-- ORIGINAL -->");
								output.insert(output.end(), storeline.begin(), storeline.end());
								output.push_back("<!-- CLOSE -->");
								storeline.clear();
								open = false;
							}

							output.push_back(data.second.animlist[j].name);
						}

						if (data.second.animlist[j].unknown != AnimSetDataEdited[i]->datalist[data.first].animlist[j].unknown)
						{
							if (!open)
							{
								output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
								IsEdited = true;
								open = true;
							}

							output.push_back(AnimSetDataEdited[i]->datalist[data.first].animlist[j].unknown);
							storeline.push_back(data.second.animlist[j].unknown);
						}
						else
						{
							if (open)
							{
								output.push_back("<!-- ORIGINAL -->");
								output.insert(output.end(), storeline.begin(), storeline.end());
								output.push_back("<!-- CLOSE -->");
								storeline.clear();
								open = false;
							}

							output.push_back(data.second.animlist[j].unknown);
						}

						if (open)
						{
							if (storeline.size() > 0)
							{
								output.push_back("<!-- ORIGINAL -->");
								output.insert(output.end(), storeline.begin(), storeline.end());
								storeline.clear();
							}

							output.push_back("<!-- CLOSE -->");
							open = false;
						}

						output.push_back(to_string(data.second.animlist[j].attack.size()));

						if (!matchAtkScoring(data.second.animlist[j].attack, AnimSetDataEdited[i]->datalist[data.first].animlist[j].attack, targetfilename))
						{
							return;
						}

						for (unsigned int k = 0; k < data.second.animlist[j].attack.size(); ++k)
						{
							if (data.second.animlist[j].attack[k].proxy)		// new attack
							{
								if (!open)
								{
									output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
									IsEdited = true;
								}

								while (k < data.second.animlist[j].attack.size())
								{
									if (!data.second.animlist[j].attack[k].proxy)
									{
										if (storeline.size() > 0)
										{
											output.push_back("<!-- ORIGINAL -->");
											output.insert(output.end(), storeline.begin(), storeline.end());
											storeline.clear();
										}

										--k;
										break;
									}

									output.push_back(AnimSetDataEdited[i]->datalist[data.first].animlist[j].attack[k].data);
									++k;
								}

								output.push_back("<!-- CLOSE -->");
								open = false;
							}
							else if (AnimSetDataEdited[i]->datalist[data.first].animlist[j].attack[k].proxy)		// deleted attack
							{
								if (!open)
								{
									output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
									IsEdited = true;
									open = true;
								}

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
									if (!open)
									{
										output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
										IsEdited = true;
										open = true;
									}

									output.push_back(AnimSetDataEdited[i]->datalist[data.first].animlist[j].attack[k].data);
									storeline.push_back(data.second.animlist[j].attack[k].data);
								}
								else
								{
									if (open)
									{
										output.push_back("<!-- ORIGINAL -->");
										output.insert(output.end(), storeline.begin(), storeline.end());
										output.push_back("<!-- CLOSE -->");
										storeline.clear();
										open = false;
									}

									output.push_back(data.second.animlist[j].attack[k].data);
								}
							}
						}

						if (open)
						{
							if (storeline.size() > 0)
							{
								output.push_back("<!-- ORIGINAL -->");
								output.insert(output.end(), storeline.begin(), storeline.end());
								storeline.clear();
							}

							output.push_back("<!-- CLOSE -->");
							open = false;
						}
					}
				}

				if (open)
				{
					if (storeline.size() > 0)
					{
						output.push_back("<!-- ORIGINAL -->");
						output.insert(output.end(), storeline.begin(), storeline.end());
						storeline.clear();
					}

					output.push_back("<!-- CLOSE -->");
					open = false;
				}

				output.push_back(to_string(data.second.crc32list.size()));

				if (!matchCRC32Scoring(data.second.crc32list, AnimSetDataEdited[i]->datalist[data.first].crc32list, targetfilenameedited))
				{
					return;
				}

				for (unsigned int j = 0; j < data.second.crc32list.size(); ++j)
				{
					if (data.second.crc32list[j].proxy)		// new crc32list
					{
						if (!open)
						{
							output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
							IsEdited = true;
						}

						while (j < data.second.crc32list.size())
						{
							if (!data.second.crc32list[j].proxy)
							{
								if (storeline.size() > 0)
								{
									output.push_back("<!-- ORIGINAL -->");
									output.insert(output.end(), storeline.begin(), storeline.end());
									storeline.clear();
								}

								--j;
								break;
							}

							output.push_back(AnimSetDataEdited[i]->datalist[data.first].crc32list[j].filepath);
							output.push_back(AnimSetDataEdited[i]->datalist[data.first].crc32list[j].filename);
							output.push_back(AnimSetDataEdited[i]->datalist[data.first].crc32list[j].fileformat);
							++j;
						}

						output.push_back("<!-- CLOSE -->");
						open = false;
					}
					else if (AnimSetDataEdited[i]->datalist[data.first].crc32list[j].proxy)		// deleted crc32list
					{
						if (!open)
						{
							output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
							IsEdited = true;
							open = true;
						}

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
							if (!open)
							{
								output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
								IsEdited = true;
								open = true;
							}

							output.push_back(AnimSetDataEdited[i]->datalist[data.first].crc32list[j].filepath);
							storeline.push_back(data.second.crc32list[j].filepath);
						}
						else
						{
							if (open)
							{
								output.push_back("<!-- ORIGINAL -->");
								output.insert(output.end(), storeline.begin(), storeline.end());
								output.push_back("<!-- CLOSE -->");
								storeline.clear();
								open = false;
							}

							output.push_back(data.second.crc32list[j].filepath);
						}

						if (data.second.crc32list[j].filename != AnimSetDataEdited[i]->datalist[data.first].crc32list[j].filename)
						{
							if (!open)
							{
								output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
								IsEdited = true;
								open = true;
							}

							output.push_back(AnimSetDataEdited[i]->datalist[data.first].crc32list[j].filename);
							storeline.push_back(data.second.crc32list[j].filename);
						}
						else
						{
							if (open)
							{
								output.push_back("<!-- ORIGINAL -->");
								output.insert(output.end(), storeline.begin(), storeline.end());
								output.push_back("<!-- CLOSE -->");
								storeline.clear();
								open = false;
							}

							output.push_back(data.second.crc32list[j].filename);
						}

						if (data.second.crc32list[j].fileformat != AnimSetDataEdited[i]->datalist[data.first].crc32list[j].fileformat)
						{
							if (!open)
							{
								output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
								IsEdited = true;
								open = true;
							}

							output.push_back(AnimSetDataEdited[i]->datalist[data.first].crc32list[j].fileformat);
							storeline.push_back(data.second.crc32list[j].fileformat);
						}
						else
						{
							if (open)
							{
								output.push_back("<!-- ORIGINAL -->");
								output.insert(output.end(), storeline.begin(), storeline.end());
								output.push_back("<!-- CLOSE -->");
								storeline.clear();
								open = false;
							}

							output.push_back(data.second.crc32list[j].fileformat);
						}
					}
				}

				if (open)
				{
					if (storeline.size() > 0)
					{
						output.push_back("<!-- ORIGINAL -->");
						output.insert(output.end(), storeline.begin(), storeline.end());
						storeline.clear();
					}

					output.push_back("<!-- CLOSE -->");
					open = false;
				}
			}

			if (open)
			{
				if (storeline.size() > 0)
				{
					output.push_back("<!-- ORIGINAL -->");
					output.insert(output.end(), storeline.begin(), storeline.end());
					storeline.clear();
				}

				output.push_back("<!-- CLOSE -->");
				open = false;
			}

			if (IsEdited)
			{
				ofstream outputfile(folder + projectname + "\\" + data.first);

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

			if (Error)
			{
				return;
			}

			output.clear();
		}
	}
	else
	{
		cout << "ERROR: Failed to create new folder (Folder: " << folder << projectname << ")" << endl;
		Error = true;
		return;
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

	if (Error)
	{
		return;
	}
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
