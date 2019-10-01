#include <boost\algorithm\\string.hpp>
#include "hkbmirroredskeletoninfo.h"
#include "Global.h"

using namespace std;

namespace mirroredskeletoninfo
{
	string key = "cb";
	string classname = "hkbMirroredSkeletonInfo";
	string signature = "0xc6c2da4f";
}

hkbmirroredskeletoninfo::hkbmirroredskeletoninfo(string filepath, string id, string preaddress, int functionlayer, bool compare)
{
	tempaddress = preaddress;
	address = preaddress + mirroredskeletoninfo::key + to_string(functionlayer) + ">";

	if ((!IsExist[id]) && (!Error))
	{
		if (compare)
		{
			Compare(filepath, id);
		}
		else
		{
			nonCompare(filepath, id);
		}
	}
	else if (!Error)
	{
		bool statusChange = false;

		if (IsForeign[id])
		{
			statusChange = true;
		}

		string dummyID = CrossReferencing(id, address, functionlayer, compare);

		if (compare)
		{
			if (statusChange)
			{
				Dummy(dummyID);
			}

			if (IsForeign[id])
			{
				address = preaddress;
			}
			else if (!statusChange)
			{
				IsNegated = true;
			}
		}
		else
		{
			IsNegated = true;
		}
	}
	else
	{
		return;
	}
}

void hkbmirroredskeletoninfo::nonCompare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbMirroredSkeletonInfo(ID: " << id << ") has been initialized!" << endl;
	}

	if (!FunctionLineOriginal[id].empty())
	{
		FunctionLineTemp[id] = FunctionLineOriginal[id];
	}
	else
	{
		cout << "ERROR: hkbMirroredSkeletonInfo Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "hkbMirroredSkeletonInfo(ID: " << id << ") is complete!" << endl;
	}
}

void hkbmirroredskeletoninfo::Compare(string filepath, string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "hkbMirroredSkeletonInfo(ID: " << id << ") has been initialized!" << endl;
	}

	// stage 1
	vecstr newline;

	if (!FunctionLineEdited[id].empty())
	{
		newline = FunctionLineEdited[id];
	}
	else
	{
		cout << "ERROR: hkbMirroredSkeletonInfo Inputfile(File: " << filepath << ", ID: " << id << ")" << endl;
		Error = true;
	}

	// stage 2	
	if (addressID[address] != "") // is this new function or old for non generator
	{
		IsForeign[id] = false;
		string tempid;

		if (addressChange.find(address) != addressChange.end())
		{
			tempaddress = addressChange[address];
			addressChange.erase(address);
			address = tempaddress;
		}

		tempid = addressID[address];
		exchangeID[id] = tempid;

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbMirroredSkeletonInfo(newID: " << id << ") with hkbMirroredSkeletonInfo(oldID: " << tempid << ")" << endl;
		}

		ReferenceReplacementExt(id, tempid); // replacing reference in previous functions that is using newID

		vecstr storeline = FunctionLineTemp[tempid];

		// stage 3
		int curline = 1;
		vecstr newstoreline;

		newstoreline.push_back(storeline[0]); // store old function header

		for (unsigned int i = 1; i < newline.size(); i++)
		{
			if (storeline[curline].find(newline[i], 0) != string::npos) // store function body
			{
				newstoreline.push_back(newline[i]);
				curline++;
			}
			else
			{
				newstoreline.push_back(newline[i]);
			}
		}

		FunctionLineNew[tempid] = newstoreline;

		if ((Debug) && (!Error))
		{
			cout << "Comparing hkbMirroredSkeletonInfo(newID: " << id << ") with hkbMirroredSkeletonInfo(oldID: " << tempid << ") is complete!" << endl;
		}
	}
	else
	{
		IsForeign[id] = true;
		FunctionLineNew[id] = newline;
		address = tempaddress;
	}

	RecordID(id, address, true);

	if ((Debug) && (!Error))
	{
		cout << "hkbMirroredSkeletonInfo(ID: " << id << ") is complete!" << endl;
	}
}

void hkbmirroredskeletoninfo::Dummy(string id)
{
	if (Debug)
	{
		cout << "--------------------------------------------------------------" << endl << "Dummy hkbMirroredSkeletonInfo(ID: " << id << ") has been initialized!" << endl;
	}

	if (FunctionLineNew[id].empty())
	{
		cout << "ERROR: Dummy hkbMirroredSkeletonInfo Inputfile(ID: " << id << ")" << endl;
		Error = true;
	}

	RecordID(id, address, true); // record address for compare purpose and idcount without updating referenceID

	if ((Debug) && (!Error))
	{
		cout << "Dummy hkbMirroredSkeletonInfo(ID: " << id << ") is complete!" << endl;
	}
}

string hkbmirroredskeletoninfo::GetAddress()
{
	return address;
}

bool hkbmirroredskeletoninfo::IsNegate()
{
	return IsNegated;
}

safeStringUMap<shared_ptr<hkbmirroredskeletoninfo>> hkbmirroredskeletoninfoList;
safeStringUMap<shared_ptr<hkbmirroredskeletoninfo>> hkbmirroredskeletoninfoList_E;

void hkbmirroredskeletoninfo::regis(string id, bool isEdited)
{
	isEdited ? hkbmirroredskeletoninfoList_E[id] = shared_from_this() : hkbmirroredskeletoninfoList[id] = shared_from_this();
	ID = id;
}

void hkbmirroredskeletoninfo::dataBake(string filepath, vecstr& nodelines, bool isEdited)
{
	usize type = 0;

	for (auto& line : nodelines)
	{
		switch (type)
		{
			case 0:
			{
				if (readParam("mirrorAxis", line, mirrorAxis)) ++type;

				break;
			}
			case 1:
			{
				usize numelement;

				if (readEleParam("bonePairMap", line, numelement))
				{
					bonePairMap.reserve(numelement);
					++type;
				}
			}
			case 2:
			{
				string temp = line;
				vecstr list;
				boost::trim_if(temp, boost::is_any_of("\t "));
				boost::split(list, temp, boost::is_any_of("\t "), boost::token_compress_on);

				for (auto& each : list)
				{
					if (!isOnlyNumber(each))
					{
						++type;
						break;
					}

					bonePairMap.push_back(stoi(each));
				}
			}
		}
	}

	if ((Debug) && (!Error))
	{
		cout << mirroredskeletoninfo::classname + " (ID: " << ID << ") is complete!" << endl;
	}	
}

void hkbmirroredskeletoninfo::connect(string filepath, string preaddress, int functionlayer, bool compare, hkbbehaviorgraph* graphroot)
{
	if (Error) return;

	boost::lock_guard<boost::mutex> curLock(nodeMutex);
	address = preaddress + mirroredskeletoninfo::key + to_string(functionlayer) + ">";
	poolAddress.push_back(address);

	if (IsExist.find(ID) == IsExist.end())
	{
		if (!compare)
		{
			RecordID(ID, address);
		}
		else
		{
			// existed
			if (addressID.find(address) != addressID.end())
			{
				if (addressChange.find(address) != addressChange.end())
				{
					string tempadd = addressChange[address];
					addressChange.erase(address);
					address = tempadd;
				}

				auto protect = shared_from_this();			// it is here to protect the current object from being destroyed due to out of reference
				hkbmirroredskeletoninfoList_E.erase(ID);
				editedBehavior.erase(ID);
				ID = addressID[address];
				hkbmirroredskeletoninfoList_E[ID] = protect;
				editedBehavior[ID] = protect;
				RecordID(ID, address, true);
				nextNode(filepath, functionlayer, true, graphroot);
			}
			// newly created
			else
			{
				IsForeign[ID] = true;
				address = preaddress;
				RecordID(ID, address, true);
				nextNode(filepath, functionlayer, false, graphroot);
			}
		}
	}
	else if (!compare)
	{
		CrossReferencing(shared_from_this(), address, functionlayer, compare);
	}
	else if (IsForeign.find(ID) != IsForeign.end())
	{
		string newID = CrossReferencing(shared_from_this(), address, functionlayer, compare);

		// comparing
		if (compare)
		{
			if (ID != newID)
			{
				if (addressChange.find(address) != addressChange.end())
				{
					string tempadd = addressChange[address];
					addressChange.erase(address);
					address = tempadd;
				}

				auto foreign_itr = IsForeign.find(ID);

				if (foreign_itr != IsForeign.end()) IsForeign.erase(foreign_itr);

				auto protect = shared_from_this();			// it is here to protect the current object from being destroyed due to out of reference
				hkbmirroredskeletoninfoList_E.erase(ID);
				editedBehavior.erase(ID);
				IsExist.erase(ID);
				ID = addressID[address];
				hkbmirroredskeletoninfoList_E[ID] = protect;
				editedBehavior[ID] = protect;
				RecordID(ID, address, true);
				nextNode(filepath, functionlayer, true, graphroot);
			}
			else
			{
				address = preaddress;
				nextNode(filepath, functionlayer, false, graphroot);
			}
		}
	}
	else if (IsForeign.find(hkb_parent[shared_from_this()]->ID) != IsForeign.end())
	{
		if (count(address.begin(), address.end(), '>') == 3)
		{
			if (address.find("(cj", 0) != string::npos || address.find("(i", 0) != string::npos)
			{
				IsOldFunction(filepath, shared_from_this(), address);
			}
		}
	}
}

string hkbmirroredskeletoninfo::getClassCode()
{
	return mirroredskeletoninfo::key;
}

void hkbmirroredskeletoninfo::match(shared_ptr<hkbobject> counterpart)
{
	if (Error) return;

	bool open = false;
	bool isEdited = false;
	vecstr storeline;
	vecstr output;
	storeline.reserve(10);
	output.reserve(100);
	usize base = 2;
	usize size = bonePairMap.size();
	usize orisize = size;
	hkbmirroredskeletoninfo* ctrpart = static_cast<hkbmirroredskeletoninfo*>(counterpart.get());

	output.push_back(openObject(base, ID, mirroredskeletoninfo::classname, mirroredskeletoninfo::signature));		// 1
	paramMatch("mirrorAxis", mirrorAxis, ctrpart->mirrorAxis, output, storeline, base, true, open, isEdited);

	if (size == 0)
	{
		output.push_back(openParam(base, "bonePairMap", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "bonePairMap", size));		// 2

	for (usize i = 0; i < size; ++i)
	{
		if (i >= ctrpart->bonePairMap.size())
		{
			while (i < size)
			{
				storeline.push_back(to_string(bonePairMap[i]));
				output.push_back("");
				++i;
			}
		}
		else
		{
			voidMatch(bonePairMap[i], ctrpart->bonePairMap[i], output, storeline, base, false, open, isEdited);
		}
	}

	if (size < ctrpart->bonePairMap.size())
	{
		usize i = size;
		size = ctrpart->bonePairMap.size();
		nemesis::try_open(open, isEdited, output);

		while (i < size)
		{
			output.push_back(to_string(ctrpart->bonePairMap[i]));
			++i;
		}
	}

	if (size > 0)
	{
		if (orisize == 0)
		{
			nemesis::try_open(open, isEdited, output);
			output.push_back(closeParam(base));		// 2
			nemesis::try_close(open, output, storeline);
		}
		else
		{
			nemesis::try_close(open, output, storeline);
			output.push_back(closeParam(base));		// 2
		}
	}
	else nemesis::try_close(open, output, storeline);

	output.push_back(closeObject(base));	// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction("mod/" + modcode + "/" + shortFileName + "/" + ID + ".txt", mirroredskeletoninfo::classname, output, isEdited);
}

void hkbmirroredskeletoninfo::newNode()
{
	string modID = NodeIDCheck(ID);
	usize base = 2;
	usize size = bonePairMap.size();
	usize counter = 0;
	string line = "				";
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + modID + ".txt";
	vecstr output;
	output.reserve(100);

	output.push_back(openObject(base, ID, mirroredskeletoninfo::classname, mirroredskeletoninfo::signature));		// 1
	output.push_back(autoParam(base, "mirrorAxis", mirrorAxis.getString()));

	if (size == 0)
	{
		output.push_back(openParam(base, "bonePairMap", size) + closeParam());		// 2
		--base;
	}
	else output.push_back(openParam(base, "bonePairMap", size));		// 2

	for (auto& bone : bonePairMap)
	{
		line.append(to_string(bone) + " ");
		
		if (++counter == 16)
		{
			line.pop_back();
			output.push_back(line);
			line = "				";
			counter = 0;
		}
	}

	if (counter != 0)
	{
		line.pop_back();
		output.push_back(line);
	}

	if (size > 0) output.push_back(closeParam(base));		// 2

	output.push_back(closeObject(base));	// 1
	NemesisReaderFormat(stoi(ID.substr(1)), output);
	outputExtraction(filename, mirroredskeletoninfo::classname, output, true);
}

void hkbMirroredSkeletonInfoExport(string id)
{
	//stage 1 reading
	vecstr storeline1;
	storeline1.reserve(FunctionLineTemp[id].size());
	string line;

	if (FunctionLineTemp[id].size() > 0)
	{
		for (unsigned int i = 0; i < FunctionLineTemp[id].size(); ++i)
		{
			line = FunctionLineTemp[id][i];

			if ((line.find("<hkobject>", 0) == string::npos) && (line.find("</hkobject>", 0) == string::npos) && (line.find("</hkparam>", 0) == string::npos || line.find("</hkparam>", 0) > 10))
			{
				storeline1.push_back(line);
			}
		}
	}
	else
	{
		cout << "ERROR: Edit hkbMirroredSkeletonInfo Input Not Found (ID: " << id << ")" << endl;
		Error = true;
	}

	//stage 2 reading and identifying edits
	vecstr storeline2;
	storeline2.reserve(FunctionLineNew[id].size());

	if (FunctionLineNew[id].size() > 0)
	{
		for (unsigned int i = 0; i < FunctionLineNew[id].size(); ++i)
		{
			line = FunctionLineNew[id][i];

			if ((line.find("<hkobject>", 0) == string::npos) && (line.find("</hkobject>", 0) == string::npos) && (line.find("</hkparam>", 0) == string::npos || line.find("</hkparam>", 0) > 10))
			{
				storeline2.push_back(line);
			}
		}
	}
	else
	{
		cout << "ERROR: Edit hkbMirroredSkeletonInfo Output Not Found (ID: " << id << ")" << endl;
		Error = true;
	}

	vecstr output;
	bool open = false;
	bool IsChanged = false;
	bool IsEdited = false;
	int curline = 0;
	int openpoint;
	int closepoint;

	for (unsigned int i = 0; i < storeline2.size(); i++)
	{
		if (i < storeline1.size()) // existing variable value
		{
			if ((storeline1[curline].find(storeline2[i], 0) != string::npos) && (storeline1[curline].length() == storeline2[i].length()))
			{
				if (open)
				{
					if (IsChanged)
					{
						closepoint = curline;

						if (closepoint != openpoint)
						{
							output.push_back("<!-- ORIGINAL -->");

							for (int j = openpoint; j < closepoint; j++)
							{
								output.push_back(storeline1[j]);
							}
						}

						IsChanged = false;
					}

					output.push_back("<!-- CLOSE -->");
					open = false;
				}
			}
			else
			{
				if (!open)
				{
					output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
					openpoint = curline;
					IsChanged = true;
					IsEdited = true;
					open = true;
				}
			}

			output.push_back(storeline2[i]);
			curline++;
		}
		else // new data
		{
			if (!open)
			{
				output.push_back("<!-- MOD_CODE ~" + modcode + "~ OPEN -->");
				openpoint = curline;
				IsEdited = true;
				open = true;
			}

			output.push_back(storeline2[i]);
		}
	}

	if (open) // close unclosed edits
	{
		if (IsChanged)
		{
			closepoint = curline;

			if (closepoint != openpoint)
			{
				output.push_back("<!-- ORIGINAL -->");

				for (int j = openpoint; j < closepoint; j++)
				{
					output.push_back(storeline1[j]);
				}
			}

			IsChanged = false;
		}

		output.push_back("<!-- CLOSE -->");
		open = false;
	}

	NemesisReaderFormat(stoi(id.substr(1)), output);

	// stage 3 output if it is edited
	string filename = "mod/" + modcode + "/" + shortFileName + "/" + id + ".txt";

	if (IsEdited)
	{
		ofstream outputfile(filename);

		if (outputfile.is_open())
		{
			FunctionWriter fwrite(&outputfile);

			for (unsigned int i = 0; i < output.size(); i++)
			{
				fwrite << output[i] << "\n";
			}

			fwrite << "			</hkparam>\n";
			fwrite << "		</hkobject>\n";
			outputfile.close();
		}
		else
		{
			cout << "ERROR: Edit hkbMirroredSkeletonInfo Output Not Found (File: " << filename << ")" << endl;
			Error = true;
		}
	}
	else
	{
		if (IsFileExist(filename))
		{
			if (remove(filename.c_str()) != 0)
			{
				perror("Error deleting file");
				Error = true;
			}
		}
	}
}
