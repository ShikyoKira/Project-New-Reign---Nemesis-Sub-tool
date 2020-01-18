#include "animdatacore.h"
#include "animdatamatch.h"
#include "readdirectory.hpp"
#include <boost\thread.hpp>

using namespace std;

void ProxyProject(int counter, string folder);
void ProjectProcessing(int i, string folder);
void closeBracket(vecstr& output, vecstr& storeline, bool& open);
void datalistProcess(vecstr& oriSource, vecstr& editSource, vecstr& output, vecstr& storeline, bool& IsEdited, bool& open);

void animDataProcess()
{
	int projectcount = 0;
	bool IsEdited = false;
	bool open = false;
	vecstr output;
	vecstr storeline;
	string folder = "mod\\" + modcode + "\\animationdatasinglefile\\";

	if (!matchProjectScoring(AnimDataOriginal, AnimDataEdited, targetfilenameedited))
	{
		return;
	}

	for (unsigned int i = 0; i < AnimDataOriginal.size(); ++i)
	{
		if (AnimDataOriginal[i]->proxy)		// new project
		{
			if (!open)
			{
				output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
				IsEdited = true;
				open = true;
			}

			while (i < AnimDataOriginal.size())
			{
				if (AnimDataOriginal[i]->proxy)
				{
					if (storeline.size() > 0)
					{
						output.push_back("<!-- ORIGINAL -->");
						output.insert(output.end(), storeline.begin(), storeline.end());
						storeline.clear();
					}

					--i;
					break;
				}

				output.push_back(AnimDataEdited[i]->name);
				++i;
			}

			closeBracket(output, storeline, open);
		}
		else if (AnimDataEdited[i]->proxy)		// deleted project
		{
			if (!open)
			{
				output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
				IsEdited = true;
				open = true;
			}

			while (i < AnimDataEdited.size())
			{
				if (AnimDataEdited[i]->proxy)
				{
					--i;
					break;
				}

				output.push_back("//* delete this line *//");
				storeline.push_back(AnimDataOriginal[i]->name);
				++i;
			}
		}
		else
		{
			closeBracket(output, storeline, open);
			output.push_back(AnimDataOriginal[i]->name);
		}
	}

	closeBracket(output, storeline, open);

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
			output.clear();
			IsEdited = false;
		}
		else
		{
			cout << "ERROR: Failed to output animationdata header file (Project: $header$, Header: $header$)" << endl;
			Error = true;
			return;
		}
	}

	for (unsigned int i = 0; i < AnimDataOriginal.size();)
	{
		boost::thread_group multithreads;
		size_t size = i + min(AnimDataOriginal.size() - i, thread::hardware_concurrency());

		for (unsigned int j = i; j < size; ++j)
		{
			if (AnimDataOriginal[j]->proxy)		// added project
			{
				multithreads.create_thread(boost::bind(ProxyProject, j, folder));
			}
			else if (AnimDataEdited[j]->proxy)		// deleted project
			{
				// project list will take care of this during main tool runtime
				// it will be done by detecting //* delete this line *//
				// if detected, that missing/deleted project will be omitted from data filling

				// increase size as there is no function to be initialized
				++size;
			}
			else
			{
				multithreads.create_thread(boost::bind(ProjectProcessing, j, folder));
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

void ProxyProject(int counter, string folder)
{
	size_t i = counter;

	while (i < AnimDataOriginal.size())
	{
		if (!AnimDataOriginal[i]->proxy)
		{
			--i;
			break;
		}

		string projectname = AnimDataEdited[i]->name.substr(0, AnimDataEdited[i]->name.find_last_of("."));

		if (createDirectories(folder + projectname))
		{
			ofstream outputfile(folder + projectname + "\\" + "$header$.txt");

			if (outputfile.is_open())
			{
				FunctionWriter fwriter(&outputfile);

				fwriter << to_string(AnimDataEdited[i]->GetAnimTotalLine()) << "\n";
				fwriter << AnimDataEdited[i]->unknown1 << "\n";
				fwriter << to_string(AnimDataEdited[i]->behaviorlist.size()) << "\n";

				for (unsigned int j = 0; j < AnimDataEdited[i]->behaviorlist.size(); ++j)
				{
					fwriter << AnimDataEdited[i]->behaviorlist[j] << "\n";
				}

				fwriter << AnimDataEdited[i]->unknown2 << "\n";
				outputfile.close();
			}
			else
			{
				cout << "ERROR: Failed to output animationdata header file (Project: " << AnimDataEdited[i]->name << ", Header: $header$)" << endl;
				Error = true;
				return;
			}

			outputfile.close();

			if (AnimDataEdited[i]->unknown2 != "0")
			{
				unordered_map<string, string> namecode;

				for (unsigned int j = 0; j < AnimDataEdited[i]->animdatalist.size(); ++j)
				{
					string name = AnimDataEdited[i]->animdatalist[j].name;
					namecode[AnimDataEdited[i]->animdatalist[j].uniquecode] = name;
					ofstream output(folder + projectname + "\\" + name + ".txt");

					if (output.is_open())
					{
						FunctionWriter fwriter(&output);

						fwriter << name << "\n";
						fwriter << AnimDataEdited[i]->animdatalist[j].uniquecode << "\n";
						fwriter << AnimDataEdited[i]->animdatalist[j].unknown1 << "\n";
						fwriter << AnimDataEdited[i]->animdatalist[j].unknown2 << "\n";
						fwriter << AnimDataEdited[i]->animdatalist[j].unknown3 << "\n";
						fwriter << to_string(AnimDataEdited[i]->animdatalist[j].eventname.size()) << "\n";

						for (unsigned int k = 0; k < AnimDataEdited[i]->animdatalist[j].eventname.size(); ++k)
						{
							fwriter << AnimDataEdited[i]->animdatalist[j].eventname[k] << "\n";
						}

						fwriter << "\n";
						output.close();
					}
					else
					{
						cout << "ERROR: Failed to output animationdata header file (Project: " << AnimDataEdited[i]->name << ", Header: " << name << ")" << endl;
						Error = true;
						return;
					}
				}

				for (unsigned int j = 0; j < AnimDataEdited[i]->infodatalist.size(); ++j)
				{
					if (namecode[AnimDataEdited[i]->infodatalist[j].uniquecode].length() == 0)
					{
						cout << "WARNING: Non-registered unique code detected. Current data will not be utilized (Project: " << AnimDataEdited[i]->name << ", Unique Code: " << AnimDataEdited[i]->infodatalist[j].uniquecode << ")" << endl;
					}

					ofstream output(folder + projectname + "\\" + AnimDataEdited[i]->infodatalist[j].uniquecode + ".txt");

					if (output.is_open())
					{
						FunctionWriter fwriter(&output);

						fwriter << AnimDataEdited[i]->infodatalist[j].uniquecode << "\n";
						fwriter << AnimDataEdited[i]->infodatalist[j].duration << "\n";
						fwriter << to_string(AnimDataEdited[i]->infodatalist[j].motiondata.size()) << "\n";

						for (unsigned int k = 0; k < AnimDataEdited[i]->infodatalist[j].motiondata.size(); ++k)
						{
							fwriter << AnimDataEdited[i]->infodatalist[j].motiondata[k] << "\n";
						}

						fwriter << to_string(AnimDataEdited[i]->infodatalist[j].rotationdata.size()) << "\n";

						for (unsigned int k = 0; k < AnimDataEdited[i]->infodatalist[j].rotationdata.size(); ++k)
						{
							fwriter << AnimDataEdited[i]->infodatalist[j].rotationdata[k] << "\n";
						}

						fwriter << "\n";
						output.close();
					}
					else
					{
						cout << "ERROR: Failed to output animationdata header file (Project: " << AnimDataEdited[i]->name << ", Unique Code: " << AnimDataEdited[i]->infodatalist[j].uniquecode << ")" << endl;
						Error = true;
						return;
					}
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

void ProjectProcessing(int i, string folder)
{
	string projectname = AnimDataOriginal[i]->filename;
	vecstr storeline;
	vecstr output;
	bool IsEdited = false;
	bool open = false;

	if (createDirectories(folder + projectname))
	{
		{
			if (AnimDataOriginal[i]->unknown1 != AnimDataEdited[i]->unknown1)
			{
				output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
				IsEdited = true;
				open = true;

				output.push_back(AnimDataEdited[i]->unknown1);
				storeline.push_back(AnimDataOriginal[i]->unknown1);
			}
			else
			{
				output.push_back(AnimDataOriginal[i]->unknown1);
			}

			if (AnimDataOriginal[i]->behaviorlist.size() != AnimDataEdited[i]->behaviorlist.size())
			{
				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					IsEdited = true;
					open = true;
				}

				output.push_back(to_string(AnimDataEdited[i]->behaviorlist.size()));
				storeline.push_back(to_string(AnimDataOriginal[i]->behaviorlist.size()));
			}
			else
			{
				closeBracket(output, storeline, open);
				output.push_back(to_string(AnimDataOriginal[i]->behaviorlist.size()));
			}

			if (Error)
			{
				return;
			}

			vecstr oribehaviorlist = AnimDataOriginal[i]->behaviorlist;
			vecstr editbehaviorlist = AnimDataEdited[i]->behaviorlist;

			if (!matchProjectScoring(oribehaviorlist, editbehaviorlist, targetfilenameedited))
			{
				return;
			}

			for (unsigned int j = 0; j < oribehaviorlist.size(); ++j)
			{
				if (oribehaviorlist[j] != editbehaviorlist[j])
				{
					if (!open)
					{
						output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
						IsEdited = true;
						open = true;
					}

					output.push_back(editbehaviorlist[j]);
					storeline.push_back(oribehaviorlist[j]);
				}
				else
				{
					closeBracket(output, storeline, open);
					output.push_back(oribehaviorlist[j]);
				}
			}

			if (AnimDataOriginal[i]->unknown2 != AnimDataEdited[i]->unknown2)
			{
				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					IsEdited = true;
					open = true;
				}

				output.push_back(AnimDataEdited[i]->unknown2);
				storeline.push_back(AnimDataOriginal[i]->unknown2);
			}
			else
			{
				closeBracket(output, storeline, open);
				output.push_back(AnimDataOriginal[i]->unknown2);
			}
		}

		if (IsEdited)
		{
			ofstream outputfile(folder + projectname + "\\" + "$header$.txt");

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
				cout << "ERROR: Failed to output animationdata header file (Project: " << AnimDataEdited[i]->name << ", Header: $header$)" << endl;
				Error = true;
				return;
			}
		}

		output.clear();

		if (Error)
		{
			return;
		}

		int modUCCount = 0;

		if (AnimDataOriginal[i]->unknown2 == "0")
		{
			if (AnimDataEdited[i]->unknown2 != "0")
			{
				unordered_map<string, string> modUC;

				for (auto& animdata : AnimDataEdited[i]->animdatalist)
				{
					string name = animdata.name;
					string uniquecode = modcode + "$" + to_string(modUCCount++);
					modUC[animdata.uniquecode] = uniquecode;
					ofstream outputfile(folder + projectname + "\\" + name + "~" + uniquecode + ".txt");

					if (outputfile.is_open())
					{
						FunctionWriter fwriter(&outputfile);

						fwriter << name << "\n";
						fwriter << uniquecode << "\n";
						fwriter << animdata.unknown1 << "\n";
						fwriter << animdata.unknown2 << "\n";
						fwriter << animdata.unknown3 << "\n";
						fwriter << to_string(animdata.eventname.size()) << "\n";

						for (unsigned int k = 0; k < animdata.eventname.size(); ++k)
						{
							fwriter << animdata.eventname[k] << "\n";
						}

						fwriter << "\n";
						outputfile.close();
					}
					else
					{
						cout << "ERROR: Failed to output animationdata header file (Project: " << AnimDataEdited[i]->name << ", Header: " << name << ")" << endl;
						Error = true;
						return;
					}
				}

				for (auto& infodata : AnimDataEdited[i]->infodatalist)
				{
					string uniquecode;

					if (modUC[infodata.uniquecode].length() == 0)
					{
						uniquecode = modcode + "$" + to_string(modUCCount++);
						cout << "WARNING: Non-registered unique code detected. Current data will not be utilized (Project: " << AnimDataEdited[i]->name << ", Unique Code: " << infodata.uniquecode << ")" << endl;
					}
					else
					{
						uniquecode = modUC[infodata.uniquecode];
					}

					ofstream outputfile(folder + projectname + "\\" + uniquecode + ".txt");

					if (outputfile.is_open())
					{
						FunctionWriter fwriter(&outputfile);

						fwriter << uniquecode << "\n";
						fwriter << infodata.duration << "\n";
						fwriter << to_string(infodata.motiondata.size()) << "\n";

						for (unsigned int k = 0; k < infodata.motiondata.size(); ++k)
						{
							fwriter << infodata.motiondata[k] << "\n";
						}

						fwriter << to_string(infodata.rotationdata.size()) << "\n";

						for (unsigned int k = 0; k < infodata.rotationdata.size(); ++k)
						{
							fwriter << infodata.rotationdata[k] << "\n";
						}

						fwriter << "\n";
						outputfile.close();
					}
					else
					{
						cout << "ERROR: Failed to output animationdata header file (Project: " << AnimDataEdited[i]->name << ", Unique Code: " << infodata.uniquecode << ")" << endl;
						Error = true;
						return;
					}
				}
			}
		}
		else if (AnimDataEdited[i]->unknown2 != "0")
		{
			unordered_map<string, string> modUC;
			unordered_map<string, bool> oriUC;
			vector<AnimDataPack> oriAnimDataList = AnimDataOriginal[i]->animdatalist;
			vector<AnimDataPack> editAnimDataList = AnimDataEdited[i]->animdatalist;

			if (!matchProjectScoring(oriAnimDataList, editAnimDataList, targetfilenameedited))
			{
				return;
			}

			for (auto& animData : oriAnimDataList)
			{
				if (!animData.proxy)
				{
					oriUC[animData.uniquecode] = true;
				}
			}

			for (unsigned int j = 0; j < oriAnimDataList.size(); ++j)
			{
				AnimDataPack* oriAnimData = &oriAnimDataList[j];
				AnimDataPack* editAnimData = &editAnimDataList[j];
				string name;
				string uniquecode;

				if (oriAnimData->proxy)		// new animation data
				{
					name = editAnimData->name;

					if (oriUC[editAnimData->uniquecode])
					{
						uniquecode = editAnimData->uniquecode;
					}
					else if (modUC.find(editAnimData->uniquecode) == modUC.end())
					{
						uniquecode = modcode + "$" + to_string(modUCCount++);
						modUC[editAnimData->uniquecode] = uniquecode;
					}
					else
					{
						uniquecode = modUC[editAnimData->uniquecode];
					}

					IsEdited = true;

					output.push_back(name);
					output.push_back(uniquecode);
					output.push_back(editAnimData->unknown1);
					output.push_back(editAnimData->unknown2);
					output.push_back(editAnimData->unknown3);
					output.push_back(to_string(editAnimData->eventname.size()));

					for (unsigned int k = 0; k < editAnimData->eventname.size(); ++k)
					{
						output.push_back(editAnimData->eventname[k]);
					}
				}
				else if (editAnimData->proxy)		// deleted animation data
				{
					name = oriAnimData->name;
					uniquecode = oriAnimData->uniquecode;
					IsEdited = true;
					int counter = 0;

					if (!open)
					{
						output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
						open = true;
					}

					while (counter < 7)
					{
						output.push_back("//* delete this line *//");
						++counter;
					}

					storeline.push_back(name);
					storeline.push_back(uniquecode);
					storeline.push_back(oriAnimData->unknown1);
					storeline.push_back(oriAnimData->unknown2);
					storeline.push_back(oriAnimData->unknown3);
					storeline.push_back(to_string(oriAnimData->eventname.size()));

					for (unsigned int k = 0; k < oriAnimData->eventname.size(); ++k)
					{
						storeline.push_back(oriAnimData->eventname[k]);
						output.push_back("//* delete this line *//");
					}

					storeline.push_back("");
				}
				else
				{
					if (oriAnimData->name != editAnimData->name)
					{
						cout << "ERROR: AnimData name inconsistency detected. Please report to Nemesis' author" << endl;
						Error = true;
						return;
					}
					else
					{
						name = oriAnimData->name;
						uniquecode = oriAnimData->uniquecode;
						output.push_back(name);
					}

					if (uniquecode != editAnimData->uniquecode)
					{
						if (!open)
						{
							output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
							IsEdited = true;
							open = true;
						}

						if (oriUC[editAnimData->uniquecode])
						{
							output.push_back(editAnimData->uniquecode);
						}
						else
						{
							output.push_back(modcode + "$" + to_string(modUCCount++));
							modUC[editAnimData->uniquecode] = output.back();
						}

						storeline.push_back(uniquecode);
					}
					else
					{
						closeBracket(output, storeline, open);
						output.push_back(uniquecode);
					}

					if (oriAnimData->unknown1 != editAnimData->unknown1)
					{
						if (!open)
						{
							output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
							IsEdited = true;
							open = true;
						}

						output.push_back(editAnimData->unknown1);
						storeline.push_back(oriAnimData->unknown1);
					}
					else
					{
						closeBracket(output, storeline, open);
						output.push_back(oriAnimData->unknown1);
					}

					if (oriAnimData->unknown2 != editAnimData->unknown2)
					{
						if (!open)
						{
							output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
							IsEdited = true;
							open = true;
						}

						output.push_back(editAnimData->unknown2);
						storeline.push_back(oriAnimData->unknown2);
					}
					else
					{
						closeBracket(output, storeline, open);
						output.push_back(oriAnimData->unknown2);
					}

					if (oriAnimData->unknown3 != editAnimData->unknown3)
					{
						if (!open)
						{
							output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
							IsEdited = true;
							open = true;
						}

						output.push_back(editAnimData->unknown3);
						storeline.push_back(oriAnimData->unknown3);
					}
					else
					{
						closeBracket(output, storeline, open);
						output.push_back(oriAnimData->unknown3);
					}

					if (oriAnimData->eventname.size() != editAnimData->eventname.size())
					{
						if (!open)
						{
							output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
							IsEdited = true;
							open = true;
						}

						output.push_back(to_string(editAnimData->eventname.size()));
						storeline.push_back(to_string(oriAnimData->eventname.size()));
					}
					else
					{
						closeBracket(output, storeline, open);
						output.push_back(to_string(oriAnimData->eventname.size()));
					}

					if (Error)
					{
						return;
					}

					vecstr oriEvents = oriAnimData->eventname;
					vecstr editEvents = editAnimData->eventname;

					if (!matchProjectScoring(oriEvents, editEvents, targetfilenameedited))
					{
						return;
					}

					for (unsigned int k = 0; k < oriEvents.size(); ++k)
					{
						if (oriEvents[k] == "//* delete this line *//")		// new event
						{
							if (!open)
							{
								output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
								IsEdited = true;
								open = true;
							}

							while (k < oriEvents.size())
							{
								if (oriEvents[k] != "//* delete this line *//")
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

								output.push_back(editEvents[k]);
								++k;
							}

							closeBracket(output, storeline, open);
						}
						else if (editEvents[k] == "//* delete this line *//")		// deleted event
						{
							if (!open)
							{
								output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
								IsEdited = true;
								open = true;
							}

							while (k < oriEvents.size())
							{
								if (editEvents[k] != "//* delete this line *//")
								{
									--k;
									break;
								}

								output.push_back(editEvents[k]);
								storeline.push_back(oriEvents[k]);
								++k;
							}
						}
						else
						{
							if (oriEvents[k] != editEvents[k])
							{
								if (!open)
								{
									output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
									IsEdited = true;
									open = true;
								}

								output.push_back(editEvents[k]);
								storeline.push_back(oriEvents[k]);
							}
							else
							{
								closeBracket(output, storeline, open);
								output.push_back(oriEvents[k]);
							}
						}
					}

					closeBracket(output, storeline, open);
				}

				closeBracket(output, storeline, open);

				if (IsEdited)
				{
					ofstream outputfile(folder + projectname + "\\" + name + "~" + uniquecode + ".txt");

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
						cout << "ERROR: Failed to output infodata file (Project: " << AnimDataEdited[i]->name << ", Header: " << name << ")" << endl;
						Error = true;
						return;
					}
				}

				output.clear();
			}

			if (Error)
			{
				return;
			}

			vector<InfoDataPack> oriInfoDataList = AnimDataOriginal[i]->infodatalist;
			vector<InfoDataPack> editInfoDataList = AnimDataEdited[i]->infodatalist;

			if (!matchProjectScoring(oriInfoDataList, editInfoDataList, targetfilenameedited))
			{
				return;
			}

			for (unsigned int j = 0; j < oriInfoDataList.size(); ++j)
			{
				open = false;
				output.clear();
				storeline.clear();
				InfoDataPack* oriInfoData = &oriInfoDataList[j];
				InfoDataPack* editInfoData = &editInfoDataList[j];
				string uniquecode;

				if (oriInfoData->proxy)		// new info data
				{
					if (oriUC[editInfoData->uniquecode])
					{
						uniquecode = editInfoData->uniquecode;
					}
					else
					{
						if (modUC[editInfoData->uniquecode].length() > 0)
						{
							uniquecode = modUC[editInfoData->uniquecode];
						}
						else
						{
							uniquecode = modcode + "$" + to_string(modUCCount++);
							modUC[editInfoData->uniquecode] = uniquecode;
						}
					}

					IsEdited = true;

					output.push_back(uniquecode);
					output.push_back(editInfoData->duration);
					output.push_back(to_string(editInfoData->motiondata.size()));

					for (unsigned int k = 0; k < editInfoData->motiondata.size(); ++k)
					{
						output.push_back(editInfoData->motiondata[k]);
					}

					output.push_back(to_string(editInfoData->rotationdata.size()));

					for (unsigned int k = 0; k < editInfoData->rotationdata.size(); ++k)
					{
						output.push_back(editInfoData->rotationdata[k]);
					}

					output.push_back("");
				}
				else if (editInfoData->proxy)		// deleted info data
				{
					uniquecode = oriInfoData->uniquecode;
					IsEdited = true;
					int counter = 0;

					if (!open)
					{
						output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
						open = true;
					}

					storeline.push_back(oriInfoData->uniquecode);
					storeline.push_back(oriInfoData->duration);
					storeline.push_back(to_string(oriInfoData->motiondata.size()));

					for (unsigned int k = 0; k < oriInfoData->motiondata.size(); ++k)
					{
						storeline.push_back(oriInfoData->motiondata[k]);
						output.push_back("//* delete this line *//");
					}

					storeline.push_back(to_string(oriInfoData->rotationdata.size()));

					for (unsigned int k = 0; k < oriInfoData->rotationdata.size(); ++k)
					{
						storeline.push_back(oriInfoData->rotationdata[k]);
						output.push_back("//* delete this line *//");
					}

					while (counter < 5)
					{
						output.push_back("//* delete this line *//");
						++counter;
					}

					storeline.push_back("");
				}
				else
				{
					if (oriInfoData->uniquecode != editInfoData->uniquecode)
					{
						cout << "ERROR: AnimData name inconsistency detected. Please report to Nemesis' author" << endl;
						Error = true;
						return;
					}
					else
					{
						uniquecode = oriInfoData->uniquecode;
						output.push_back(oriInfoData->uniquecode);
					}

					if (oriInfoData->duration != editInfoData->duration)
					{
						if (!open)
						{
							output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
							IsEdited = true;
						}

						output.push_back(editInfoData->duration);
						output.push_back("<!-- ORIGINAL -->");
						output.push_back(oriInfoData->duration);
						output.push_back("<!-- CLOSE -->");
						open = false;
					}
					else
					{
						closeBracket(output, storeline, open);
						output.push_back(oriInfoData->duration);
					}

					if (Error)
					{
						return;
					}

					output.push_back(to_string(oriInfoData->motiondata.size()));
					datalistProcess(oriInfoData->motiondata, editInfoData->motiondata, output, storeline, IsEdited, open);

					if (Error)
					{
						return;
					}

					output.push_back(to_string(oriInfoData->rotationdata.size()));
					datalistProcess(oriInfoData->rotationdata, editInfoData->rotationdata, output, storeline, IsEdited, open);

					if (Error)
					{
						return;
					}

					output.push_back("");
				}

				closeBracket(output, storeline, open);

				if (IsEdited)
				{
					ofstream outputfile(folder + projectname + "\\" + uniquecode + ".txt");

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
						cout << "ERROR: Failed to output infodata file (Project: " << AnimDataEdited[i]->name << ", Unique Code: " << uniquecode << ")" << endl;
						Error = true;
						return;
					}
				}

				output.clear();
			}
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

void closeBracket(vecstr& output, vecstr& storeline, bool& open)
{
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

void datalistProcess(vecstr& oriSource, vecstr& editSource, vecstr& output, vecstr& storeline, bool& IsEdited, bool& open)
{
	vecstr ori = oriSource;
	vecstr edit = editSource;

	if (!matchDetailedScoring(ori, edit, targetfilenameedited))
	{
		return;
	}

	for (unsigned int k = 0; k < ori.size(); ++k)
	{
		if (ori[k] == "//* delete this line *//")		// new motion data
		{
			if (!open)
			{
				output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
				IsEdited = true;
				open = true;
			}

			while (k < ori.size())
			{
				if (ori[k] != "//* delete this line *//")
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

				output.push_back(edit[k]);
				++k;
			}

			closeBracket(output, storeline, open);
		}
		else if (edit[k] == "//* delete this line *//")		// deleted event
		{
			if (!open)
			{
				output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
				IsEdited = true;
				open = true;
			}

			while (k < ori.size())
			{
				if (edit[k] != "//* delete this line *//")
				{
					--k;
					break;
				}

				output.push_back(edit[k]);
				storeline.push_back(ori[k]);
				++k;
			}
		}
		else
		{
			if (ori[k] != edit[k])
			{
				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					IsEdited = true;
					open = true;
				}

				output.push_back(edit[k]);
				storeline.push_back(ori[k]);
			}
			else
			{
				closeBracket(output, storeline, open);
				output.push_back(ori[k]);
			}
		}
	}

	closeBracket(output, storeline, open);
}
