#include <boost\thread.hpp>
#include <boost\asio\thread_pool.hpp>
#include <boost\asio\post.hpp>

#include "animdatacore.h"
#include "animdatamatch.h"
#include "readdirectory.hpp"

#include "src\utilities\nemesisformat.h"

using namespace std;

void ProxyProject(int counter, string folder);
void ProjectProcessing(int i, string folder);
void datalistProcess(vecstr& oriSource, vecstr& editSource, vecstr& output, vecstr& storeline, bool& IsEdited, bool& open);

void animDataProcess()
{
	int projectcount = 0;
	bool IsEdited = false;
	bool open = false;
	vecstr output;
	vecstr storeline;
	string folder = "mod\\" + modcode + "\\animationdatasinglefile\\";

	if (!matchProjectScoring(AnimDataOriginal, AnimDataEdited, targetfilenameedited)) return;

	unsigned int size = AnimDataOriginal.size();

	for (unsigned int i = 0; i < AnimDataOriginal.size(); ++i)
	{
		if (AnimDataOriginal[i]->proxy)		// new project
		{
			nemesis::try_open(open, IsEdited, output);

			while (i < size)
			{
				output.push_back(AnimDataEdited[i]->name);
				++i;
			}

			nemesis::try_close(open, output, storeline);
		}
		else if (AnimDataEdited[i]->proxy)		// deleted project
		{
			nemesis::try_open(open, IsEdited, output);

			while (i < size)
			{
				if (!AnimDataEdited[i]->proxy)
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
			nemesis::try_close(open, output, storeline);
			output.push_back(AnimDataOriginal[i]->name);
		}
	}

	nemesis::try_close(open, output, storeline);

	if (IsEdited)
	{
		if (!createDirectories(folder + "\\$header$"))
		{
			cout << "ERROR: Failed to create directory for animationdata header file (Project: $header$, Header: $header$)" << endl;
			Error = true;
			return;
		}

		ofstream outputfile(folder + "\\$header$\\$header$.txt");

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

	boost::asio::thread_pool multithreads;

	for (unsigned int i = 0; i < AnimDataOriginal.size(); ++i)
	{
		if (AnimDataOriginal[i]->proxy)		// added project
		{
			boost::asio::post(multithreads, boost::bind(ProxyProject, i, folder));
		}
		else if (AnimDataEdited[i]->proxy)		// deleted project
		{
			// project list will take care of this during main tool runtime
			// it will be done by detecting //* delete this line *//
			// if detected, that missing/deleted project will be omitted from data filling

			// increase size as there is no function to be initialized
		}
		else
		{
			boost::asio::post(multithreads, boost::bind(ProjectProcessing, i, folder));
		}

		if (Error) break;
	}

	multithreads.join();
}

void ProxyProject(int counter, string folder)
{
	auto animData = AnimDataEdited[counter];
	string projectname = animData->filename;

	if (!createDirectories(folder + projectname))
	{
		cout << "ERROR: Failed to create output directory for animationdata (Project: " + projectname + ", Path: " + folder + projectname + ")" << endl;
		Error = true;
		return;
	}

	ofstream outputfile(folder + projectname + "\\" + "$header$.txt");

	if (outputfile.is_open())
	{
		FunctionWriter fwriter(&outputfile);

		fwriter << to_string(animData->GetAnimTotalLine()) << "\n";
		fwriter << animData->unknown1 << "\n";
		fwriter << to_string(animData->behaviorlist.size()) << "\n";

		for (unsigned int j = 0; j < animData->behaviorlist.size(); ++j)
		{
			fwriter << animData->behaviorlist[j] << "\n";
		}

		fwriter << animData->unknown2 << "\n";
		outputfile.close();
	}
	else
	{
		cout << "ERROR: Failed to output animationdata header file (Project: " << animData->name << ", Header: $header$)" << endl;
		Error = true;
		return;
	}

	outputfile.close();

	if (animData->unknown2 != "0")
	{
		unordered_map<string, string> namecode;

		for (auto data : animData->animdatalist)
		{
			string name = data.name;
			namecode[data.uniquecode] = name;
			ofstream output(folder + projectname + "\\" + name + ".txt");

			if (output.is_open())
			{
				FunctionWriter fwriter(&output);

				fwriter << name << "\n";
				fwriter << data.uniquecode << "\n";
				fwriter << data.unknown1 << "\n";
				fwriter << data.unknown2 << "\n";
				fwriter << data.unknown3 << "\n";
				fwriter << to_string(data.eventname.size()) << "\n";

				for (auto name : data.eventname)
				{
					fwriter << name << "\n";
				}

				fwriter << "\n";
				output.close();
			}
			else
			{
				cout << "ERROR: Failed to output animationdata header file (Project: " << animData->name << ", Header: " << name << ")" << endl;
				Error = true;
				return;
			}
		}

		for (auto infodata : animData->infodatalist)
		{
			if (namecode[infodata.uniquecode].length() == 0)
			{
				cout << "WARNING: Non-registered unique code detected. Current data will not be utilized (Project: " << animData->name << ", Unique Code: " << infodata.uniquecode << ")" << endl;
			}

			ofstream output(folder + projectname + "\\" + infodata.uniquecode + ".txt");

			if (!output.is_open())
			{
				cout << "ERROR: Failed to output animationdata header file (Project: " << animData->name << ", Unique Code: " << infodata.uniquecode << ")" << endl;
				Error = true;
				return;
			}

			FunctionWriter fwriter(&output);

			fwriter << infodata.uniquecode << "\n";
			fwriter << infodata.duration << "\n";
			fwriter << to_string(infodata.motiondata.size()) << "\n";

			for (auto motion : infodata.motiondata)
			{
				fwriter << motion << "\n";
			}

			fwriter << to_string(infodata.rotationdata.size()) << "\n";

			for (auto rotation : infodata.rotationdata)
			{
				fwriter << rotation << "\n";
			}

			fwriter << "\n";
			output.close();
		}
	}
}

void ProjectProcessing(int i, string folder)
{
	string projectname = AnimDataOriginal[i]->filename;
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

	if (AnimDataOriginal[i]->unknown1 != AnimDataEdited[i]->unknown1)
	{
		nemesis::try_open(open, IsEdited, output);
		output.push_back(AnimDataEdited[i]->unknown1);
		storeline.push_back(AnimDataOriginal[i]->unknown1);
	}
	else
	{
		output.push_back(AnimDataOriginal[i]->unknown1);
	}

	if (AnimDataOriginal[i]->behaviorlist.size() != AnimDataEdited[i]->behaviorlist.size())
	{
		nemesis::try_open(open, IsEdited, output);
		output.push_back(to_string(AnimDataEdited[i]->behaviorlist.size()));
		storeline.push_back(to_string(AnimDataOriginal[i]->behaviorlist.size()));
	}
	else
	{
		nemesis::try_close(open, output, storeline);
		output.push_back(to_string(AnimDataOriginal[i]->behaviorlist.size()));
	}

	if (Error) return;

	vecstr oribehaviorlist = AnimDataOriginal[i]->behaviorlist;
	vecstr editbehaviorlist = AnimDataEdited[i]->behaviorlist;

	if (!matchProjectScoring(oribehaviorlist, editbehaviorlist, targetfilenameedited)) return;

	for (unsigned int j = 0; j < oribehaviorlist.size(); ++j)
	{
		if (oribehaviorlist[j] != editbehaviorlist[j])
		{
			nemesis::try_open(open, IsEdited, output);
			output.push_back(editbehaviorlist[j]);
			storeline.push_back(oribehaviorlist[j]);
		}
		else
		{
			nemesis::try_close(open, output, storeline);
			output.push_back(oribehaviorlist[j]);
		}
	}

	if (AnimDataOriginal[i]->unknown2 != AnimDataEdited[i]->unknown2)
	{
		nemesis::try_open(open, IsEdited, output);
		output.push_back(AnimDataEdited[i]->unknown2);
		storeline.push_back(AnimDataOriginal[i]->unknown2);
	}
	else
	{
		nemesis::try_close(open, output, storeline);
		output.push_back(AnimDataOriginal[i]->unknown2);
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

	if (Error) return;

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

		if (!matchProjectScoring(oriAnimDataList, editAnimDataList, targetfilenameedited)) return;

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
					nemesis::try_open(open, IsEdited, output);

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
					nemesis::try_close(open, output, storeline);
					output.push_back(uniquecode);
				}

				if (oriAnimData->unknown1 != editAnimData->unknown1)
				{
					nemesis::try_open(open, IsEdited, output);
					output.push_back(editAnimData->unknown1);
					storeline.push_back(oriAnimData->unknown1);
				}
				else
				{
					nemesis::try_close(open, output, storeline);
					output.push_back(oriAnimData->unknown1);
				}

				if (oriAnimData->unknown2 != editAnimData->unknown2)
				{
					nemesis::try_open(open, IsEdited, output);
					output.push_back(editAnimData->unknown2);
					storeline.push_back(oriAnimData->unknown2);
				}
				else
				{
					nemesis::try_close(open, output, storeline);
					output.push_back(oriAnimData->unknown2);
				}

				if (oriAnimData->unknown3 != editAnimData->unknown3)
				{
					nemesis::try_open(open, IsEdited, output);
					output.push_back(editAnimData->unknown3);
					storeline.push_back(oriAnimData->unknown3);
				}
				else
				{
					nemesis::try_close(open, output, storeline);
					output.push_back(oriAnimData->unknown3);
				}

				if (oriAnimData->eventname.size() != editAnimData->eventname.size())
				{
					nemesis::try_open(open, IsEdited, output);
					output.push_back(to_string(editAnimData->eventname.size()));
					storeline.push_back(to_string(oriAnimData->eventname.size()));
				}
				else
				{
					nemesis::try_close(open, output, storeline);
					output.push_back(to_string(oriAnimData->eventname.size()));
				}

				if (Error) return;

				vecstr oriEvents = oriAnimData->eventname;
				vecstr editEvents = editAnimData->eventname;

				if (!matchProjectScoring(oriEvents, editEvents, targetfilenameedited)) return;

				for (unsigned int k = 0; k < oriEvents.size(); ++k)
				{
					if (oriEvents[k] == "//* delete this line *//")		// new event
					{
						nemesis::try_open(open, IsEdited, output);

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

						nemesis::try_close(open, output, storeline);
					}
					else if (editEvents[k] == "//* delete this line *//")		// deleted event
					{
						nemesis::try_open(open, IsEdited, output);

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
							nemesis::try_open(open, IsEdited, output);
							output.push_back(editEvents[k]);
							storeline.push_back(oriEvents[k]);
						}
						else
						{
							nemesis::try_close(open, output, storeline);
							output.push_back(oriEvents[k]);
						}
					}
				}

				nemesis::try_close(open, output, storeline);
			}

			nemesis::try_close(open, output, storeline);

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

		if (Error) return;

		vector<InfoDataPack> oriInfoDataList = AnimDataOriginal[i]->infodatalist;
		vector<InfoDataPack> editInfoDataList = AnimDataEdited[i]->infodatalist;

		if (!matchProjectScoring(oriInfoDataList, editInfoDataList, targetfilenameedited)) return;

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
					nemesis::try_close(open, output, storeline);
					output.push_back(oriInfoData->duration);
				}

				if (Error) return;

				output.push_back(to_string(oriInfoData->motiondata.size()));
				datalistProcess(oriInfoData->motiondata, editInfoData->motiondata, output, storeline, IsEdited, open);

				if (Error) return;

				output.push_back(to_string(oriInfoData->rotationdata.size()));
				datalistProcess(oriInfoData->rotationdata, editInfoData->rotationdata, output, storeline, IsEdited, open);

				if (Error) return;

				output.push_back("");
			}

			nemesis::try_close(open, output, storeline);

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

	vecstr filelist;
	read_directory(folder + projectname, filelist);

	if (filelist.size() == 0)
	{
		if (!boost::filesystem::remove(folder + projectname))
		{
			cout << "WARNING: Failed to remove folder" << endl;
		}
	}
}

void datalistProcess(vecstr& oriSource, vecstr& editSource, vecstr& output, vecstr& storeline, bool& IsEdited, bool& open)
{
	vecstr ori = oriSource;
	vecstr edit = editSource;

	if (!matchDetailedScoring(ori, edit, targetfilenameedited)) return;

	for (unsigned int k = 0; k < ori.size(); ++k)
	{
		if (ori[k] == "//* delete this line *//")		// new motion data
		{
			nemesis::try_open(open, IsEdited, output);

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

			nemesis::try_close(open, output, storeline);
		}
		else if (edit[k] == "//* delete this line *//")		// deleted event
		{
			nemesis::try_open(open, IsEdited, output);

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
				nemesis::try_open(open, IsEdited, output);
				output.push_back(edit[k]);
				storeline.push_back(ori[k]);
			}
			else
			{
				nemesis::try_close(open, output, storeline);
				output.push_back(ori[k]);
			}
		}
	}

	nemesis::try_close(open, output, storeline);
}
