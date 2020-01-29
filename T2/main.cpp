#include <iostream>
#include <fstream>
#include <thread>
#include <memory>
#include <algorithm>
#include <stdlib.h>
#include <atomic>
#include <conio.h>

#include <boost\date_time\posix_time\posix_time.hpp>
#include <boost\filesystem.hpp>
#include <boost\thread.hpp>

#include "src\atomiclock.h"
#include "src\FillFunction.h"
#include "AnimData\animdatacore.h"
#include "AnimSetData\animsetdatacore.h"

using namespace std;

bool isBehavior = false;
bool isAnimData = false;
bool same = false;
atomic<int> doneReading = 0;
double readtime = 0;
string checkOri;
string checkEdit;
safeStringMap<string> rootNode;;
boost::posix_time::ptime time1;

extern atomic<int> writtenFile;

void Clearing(string file, bool edited); // clear and store file in vector
void ClearIgnore(string file1, string file2); // clear serialized ignore
void compareNode(string ID, shared_ptr<hkbobject>& edit, boost::thread_group& multithreads);
void hkb_Initialize(string originalfile, bool compare);
void ModCode(); // enter author code
void DebugMode(); // debug on/off

void Clearing(string filename, bool edited)
{
	vecstr storeline;
	GetFunctionLines(filename, storeline);
	string* checker = &(edited ? checkEdit : checkOri);

	for (auto& line : storeline)
	{
		checker->append(line);
	}

	++doneReading;

	while (doneReading != 2);

	if (checkOri == checkEdit)
	{
		same = true;
		checkOri.clear();
		checkEdit.clear();
		return;
	}

	if (Error)
	{
		return;
	}

	if (!storeline.empty())
	{
		if (isOnlyNumber(storeline[0]))
		{
			if (storeline[1].find(".txt") == storeline[1].length() - 4)
			{
				if (storeline[1].find("\\") != string::npos)
				{
					AddAnimSetData(filename, storeline, edited);

					if (Error)
					{
						cout << "Filename : " << filename << endl;
						return;
					}
				}
				else
				{
					AddAnimData(filename, storeline, edited);
					isAnimData = true;

					if (Error)
					{
						cout << "Filename : " << filename << endl;
						return;
					}
				}
			}
			else
			{
				cout << "ERROR: Unrecognized file. Failed to read file(File: " << filename << ")" << endl;
				Error = true;
				return;
			}
		}
		else
		{
			int i = 0;

			while (i < 4)
			{
				if (storeline[i].find("<?xml version=\"1.0\" encoding=\"ascii\"?>") != string::npos)
				{
					isBehavior = true;
					break;
				}

				++i;
			}

			if (isBehavior)
			{
				//try
				{
					AddBehavior(filename, storeline, edited);
				}
				//catch (const exception& ex)
				{
					//cout << ex.what() << endl;
					//Error = true;
					//return;
				}
			}
			else
			{
				cout << "ERROR: Unrecognized file. Failed to read file (File: " << filename << ")" << endl;
				Error = true;
				return;
			}
		}
	}
	else
	{
		cout << "ERROR: Failed to open file while clearing (File: " << filename << ")" << endl;
		Error = true;
		return;
	}

}

void ClearIgnore(string file1, string file2)
{
	boost::posix_time::ptime mtime1 = boost::posix_time::microsec_clock::local_time();

	thread t1([=](){Clearing(file1, false); return 1; });
	Clearing(file2, true);

	t1.join();

	boost::posix_time::ptime mtime2 = boost::posix_time::microsec_clock::local_time();
	boost::posix_time::time_duration mdiff = mtime2 - mtime1;
	readtime = static_cast<double>(mdiff.total_milliseconds());

	if (Debug)
	{
		cout << "Reading time: " << mdiff.total_milliseconds() << endl;
		cout << mtime2 << endl << mtime1 << endl << endl;
	}

	if (same || Error) return;

	if (originalBehavior.size() == 0 && AnimDataOriginal.size() == 0 && AnimSetDataOriginal.size() == 0)
	{
		cout << "ERROR: Failed to read file (File: " + file1 + ")" << endl;
		Error = true;
		return;
	}

	if (editedBehavior.size() == 0 && AnimDataEdited.size() == 0 && AnimSetDataEdited.size() == 0)
	{
		cout << "ERROR: Failed to read file (File: " + file2 + ")" << endl;
		Error = true;
		return;
	}
}

void hkb_Initialize(string originalfilename, bool compare)
{
	string search = rootNode[originalfilename];

	hkRefPtr* curBehavior;

	if (compare)
	{
		search = "#9" + search;
		curBehavior = &editedBehavior;
	}
	else
	{
		search = "#" + search;
		curBehavior = &originalBehavior;
	}

	if (!(*curBehavior)[search])
	{
		cout << "ERROR: Missing root node" << endl;
		Error = true;
		return;
	}

	(*curBehavior)[search]->connect(originalfilename, "root=", 0, compare, nullptr);

	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl;
	}
}

void ContainerClearing()
{
	if (Debug)
	{
		cout << "Clearing container" << endl;
	}

	idcount.clear();
	IsExist.clear();
	elements.clear();
	exchangeID.clear();
	referencingIDs.clear();

	if (Debug)
	{
		cout << "Clearing Done" << endl;
	}
}

void GetExistingNode(shared_ptr<hkbobject>& ori, shared_ptr<hkbobject>& edit)
{
	try
	{
		try
		{
			ori->match(edit);
		}
		catch (const exception& ex)
		{
			cout << ex.what() << endl;
			Error = true;
		}
	}
	catch (...) {}
}

void GetNewNode(shared_ptr<hkbobject>& node)
{
	try
	{
		try
		{
			node->newNode();
		}
		catch (const exception& ex)
		{
			cout << ex.what() << endl;
			Error = true;
		}
	}
	catch (...) {}
}

void compareNode(string ID, shared_ptr<hkbobject>& edit)
{
	try
	{
		auto o_obj = originalBehavior[ID];

		// newly created node
		if (!o_obj) GetNewNode(edit);			// newly created node
		else GetExistingNode(o_obj, edit);		// existing node
	}
	catch (const std::exception& ex)
	{
		cout << "EXCEPTION: " << ex.what() << endl;
		Error = true;
		throw 5;
	}

	if (Error)
	{
		cout << "ERROR detected. Unable to complete task" << endl;
		throw 5;
	}
}

void compareNode(string ID, shared_ptr<hkbobject>& edit, boost::thread_group& multithreads)
{
	try
	{
		auto o_obj = originalBehavior[ID];

		// newly created node
		if (!o_obj) multithreads.create_thread(boost::bind(GetNewNode, edit));			// newly created node
		else multithreads.create_thread(boost::bind(GetExistingNode, o_obj, edit));		// existing node
	}
	catch (const std::exception& ex)
	{
		cout << "EXCEPTION: " << ex.what() << endl;
		Error = true;
		throw 5;
	}

	if (Error)
	{
		cout << "ERROR detected. Unable to complete task" << endl;
		throw 5;
	}
}

void hkx_GetEdits()
{
	if (Debug && !Error)
	{
		cout << "Identifying changes made to the behavior" << endl;
	}

	// modify new node's ID
	for (auto node : editedBehavior)
	{
		auto o_obj = originalBehavior[node.first];

		if (!o_obj)
		{
			NodeIDCheck(node.second->ID);
		}
	}

	// clear off all string data first
	if (hkbbehaviorgraphstringdataList_E.size() > 1)
	{
		boost::thread_group multithreads;

		for (auto each : hkbbehaviorgraphstringdataList_E)
		{
			compareNode(each.first, static_cast<shared_ptr<hkbobject>>(each.second), multithreads);
		}

		multithreads.join_all();
	}
	else if (hkbbehaviorgraphstringdataList_E.size() == 1)
	{
		compareNode(hkbbehaviorgraphstringdataList_E.begin()->first, static_cast<shared_ptr<hkbobject>>(hkbbehaviorgraphstringdataList_E.begin()->second));
	}

	// clear off all string data first
	if (hkbcharacterstringdataList_E.size() > 1)
	{
		boost::thread_group multithreads;

		for (auto each : hkbcharacterstringdataList_E)
		{
			compareNode(each.first, static_cast<shared_ptr<hkbobject>>(each.second), multithreads);
		}

		multithreads.join_all();
	}
	else if (hkbcharacterstringdataList_E.size() == 1)
	{
		compareNode(hkbcharacterstringdataList_E.begin()->first, static_cast<shared_ptr<hkbobject>>(hkbcharacterstringdataList_E.begin()->second));
	}

	for (auto it = editedBehavior.begin(); it != editedBehavior.end();)
	{
		unsigned int threadcount = 0;
		boost::thread_group multithreads;

		while (threadcount < std::thread::hardware_concurrency() && it != editedBehavior.end())
		{
			if (it->second && it->second->getClassCode() != "b" || it->second->getClassCode() != "cc")
			{
				try
				{
					auto o_obj = originalBehavior[it->first];

					// newly created node
					if (!o_obj)
					{
						multithreads.create_thread(boost::bind(GetNewNode, it->second));
					}

					// existing node
					else
					{
						multithreads.create_thread(boost::bind(GetExistingNode, o_obj, it->second));
					}

					++threadcount;
				}
				catch (const std::exception& ex)
				{
					cout << "EXCEPTION: " << ex.what() << endl;
					Error = true;
					throw 5;
				}

				if (Error)
				{
					cout << "ERROR detected. Unable to complete task" << endl;
					throw 5;
				}
			}

			++it;
		}

		multithreads.join_all();
	}

	if (Debug && !Error)
	{
		cout << "Identification of changes is complete" << endl;
	}
}

bool isAlphaNum(string tempcode)
{
	for (auto& ch : tempcode)
	{
		if (!isalnum(ch)) return false;
	}

	return true;
}


void ModCode()
{
	string tempcode;
	system("cls");
	cout << "Mod code must be at least 4 but not more than 6 alphanumerical characters long and must not be \"vanilla\"" << endl << endl;
	cout << "Enter \"-1\" to exit" << endl << "--------------------------------------------------------------------------------" << endl;
	cout << "Enter your unique mod code: ";
	cin >> tempcode;

	if (tempcode == "-1")
	{
		modcode = tempcode;
		cout << endl << "--------------------------------------------------------------------------------" << endl;
		return;
	}

	if (tempcode.length() < 4 || tempcode.length() > 6 || !isAlphaNum(tempcode) || tempcode == "vanilla")
	{
		ModCode();
	}
	else
	{
		modcode = tempcode;
		cout << endl << "--------------------------------------------------------------------------------" << endl;
	}
}

void DebugMode()
{
	cout << "Do you want to turn on debug mode? (Y/N)";
	string tempstr;
	cin >> tempstr;

	if (tempstr == "y" || tempstr == "Y")
	{
		Debug = true;
		cout << endl;
		return;
	}
	else if (tempstr == "n" || tempstr == "N")
	{
		Debug = false;
		return;
	}

	DebugMode();
}

void start(bool skip)
{
	string directory = "mod\\";
	num_thread = 99;		// block multithreading

	if (!skip)
	{
		targetfilename = "output.txt";
		shortFileName = targetfilename.substr(0, targetfilename.find_last_of("."));
		targetfilenameedited = "output2.txt";
		shortFileNameEdited = targetfilenameedited.substr(0, targetfilenameedited.find_last_of("."));
	}

	thread t([=]() {ClearIgnore(targetfilename, targetfilenameedited); return 1; });

	if (Error)
	{
		cout << "ERROR: Failed to complete processing files" << endl;
		t.join();
		return;
	}

	if (!skip)
	{
		ModCode();

		if (modcode == "-1") exit(0);

		DebugMode();
	}

	if (Error)
	{
		cout << "ERROR: Failed to complete processing files" << endl;
		t.join();
		return;
	}

	t.join();
	time1 = boost::posix_time::microsec_clock::local_time();

	if (same)
	{
		cout << "Both files are identical" << endl;
		return;
	}

	if (Error)
	{
		cout << "ERROR: Failed to complete processing files" << endl;
		return;
	}

	if (createDirectories(directory + modcode))
	{
		if (isBehavior)
		{
			if (createDirectories(directory + modcode + "\\" + shortFileName))
			{
				try
				{
					try
					{
						hkb_Initialize(targetfilename, false);

					}
					catch (const exception& ex)
					{
						cout << "AN EXCEPTION OCCURRED: " << ex.what() << endl;
						Error = true;
						return;
					}
				}
				catch (...) {}

				if (!Error)
				{
					ContainerClearing();

					if (Debug && !Error)
					{
						cout << endl << endl << endl << endl << "Initializing Phase 2....." << endl << endl << endl;
						Sleep(1000);
					}

					try
					{
						try
						{
							hkb_Initialize(targetfilenameedited, true);
						}
						catch (const exception& ex)
						{
							cout << "AN EXCEPTION OCCURRED: " << ex.what() << endl;
							Error = true;
							return;
						}
					}
					catch (...) {}
				}
			}
			else
			{
				cout << "ERROR: Failed to create folder for target file (folder: " << shortFileName << ")" << endl;
				cout << GetLastError() << endl;
				Error = true;
				return;
			}
		}
		else if (isAnimData)
		{
			if (createDirectories(directory + modcode + "\\animationdatasinglefile"))
			{
				animDataProcess();
			}
			else
			{
				cout << "ERROR: Failed to create folder for target file (folder: " << shortFileName << ")" << endl;
				cout << GetLastError() << endl;
				Error = true;
				return;
			}
		}
		else if (createDirectories(directory + modcode + "\\animationsetdatasinglefile"))
		{
			animSetDataProcess();
		}
		else
		{
			cout << "ERROR: Failed to create folder for target file (folder: " << shortFileName << ")" << endl;
			cout << GetLastError() << endl;
			Error = true;
			return;
		}
	}
	else
	{
		cout << "ERROR: Failed to create directory for target modcode (ModCode: " << modcode << ")" << endl;
		cout << GetLastError() << endl;
		Error = true;
		return;
	}
	
	if (Error)
	{
		cout << "ERROR: Failed to complete processing files" << endl;
		return;
	}

	if (isBehavior)
	{
		try
		{
			hkx_GetEdits();
		}
		catch (...) {}

		if (writtenFile == 0)
		{
			cout << "Both files are identical" << endl;

			if (boost::filesystem::exists(directory + modcode + "\\" + shortFileName))
			{
				try
				{
					boost::filesystem::remove_all(directory + modcode + "\\" + shortFileName);
				}
				catch (...) {}
			}

			return;
		}
	}

	if (!Error)
	{
		for (int i = 0; i < 15; i++)
		{
			cout << endl << endl;
		}

		cout << endl << "                                      DONE!" << endl;

		for (int i = 0; i < 10; i++)
		{
			cout << endl;
		}
	}
	else
	{
		cout << "ERROR: Failed to complete processing files" << endl;
	}
}

int main(int argc, char* argv[])
{
	if (argc == 4)
	{
		modcode = argv[1];

		if (modcode.length() < 4 && modcode != "-1" || modcode.length() > 6 || !isAlphaNum(modcode) || modcode == "vanilla")
		{
			return 1;
		}

		targetfilename = argv[2];
		shortFileName = boost::filesystem::path(targetfilename).stem().string();
		targetfilenameedited = argv[3];
		shortFileNameEdited = boost::filesystem::path(targetfilenameedited).stem().string();
		Debug = false;
		start(true);
	}
	else
	{
		start(false);
	}

	if (!Error)
	{
		boost::posix_time::ptime time2 = boost::posix_time::microsec_clock::local_time();
		boost::posix_time::time_duration diff = time2 - time1;

		double duration = static_cast<double>(diff.total_milliseconds()) + readtime;

		cout << "Total processing time: " << duration / 1000 << " seconds" << endl;
	}

	if (argc != 4)
	{
		cout << '\a';
		cout << "Press any key to continue..." << endl;
		_getch();
	}

	return 0;
}
