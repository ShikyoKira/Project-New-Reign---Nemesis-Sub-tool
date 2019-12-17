#include <boost\thread.hpp>

#include "variableinfopack.h"
#include "highestscore.h"

#include "src\hkx\generator\hkbbehaviorgraph.h"

using namespace std;

safeStringUMap<shared_ptr<variableinfopack>> variablePackNode;
safeStringUMap<shared_ptr<variableinfopack>> variablePackNode_E;

VariableId::VariableId(int n_id)
{
	dataBake(n_id);
}

VariableId::VariableId(string n_id)
{
	dataBake(stoi(n_id));
}

void VariableId::connectVariableInfo(std::string n_nodeID, hkbbehaviorgraph* n_graphroot)
{
	nodeID = n_nodeID;
	graphroot = n_graphroot;
}

bool VariableId::operator==(VariableId& n_evnt)
{
	return getName() == n_evnt.getName();
}

bool VariableId::operator!=(VariableId& n_evnt)
{
	return getName() != n_evnt.getName();
}

string VariableId::operator+(string line)
{
	return to_string(id) + line;
}

string VariableId::operator=(string line)
{
	id = stoi(line);
	return to_string(id);
}

string VariableId::getName()
{
	if (!errorCheck()) return "";

	if (id == -1) return "";

	variableinfopack& variableInfos = *graphroot->data->variableInfos;

	if (variableInfos.size() <= id)
	{
		cout << "Failed to retrieve event name. Variable ID not registered (ID: " << nodeID << ")";
		Error = true;
		return "";
	}

	return variableInfos[id].name;
}

string VariableId::getID()
{
	if (!errorCheck()) return "";

	string variableNodeId = graphroot->data->stringData->ID;

	if (IsForeign.find(variableNodeId) != IsForeign.end()) return to_string(id);

	if (variablePackNode.find(variableNodeId) == variablePackNode.end())
	{
		cout << "Failed to retrieve variable id. Variable pack not recognized (ID: " << nodeID << ")";
		Error = true;
		return "";
	}

	string variableName = getName();

	if (variableName.length() == 0) return "-1";

	return variablePackNode[variableNodeId]->hasVariableName(variableName) ? to_string(variablePackNode[variableNodeId]->getID(variableName)) : ("$variableID[" + variableName + "]$");
}

void VariableId::dataBake(int n_id)
{
	graphroot = nullptr;
	id = n_id;
}

bool VariableId::errorCheck()
{
	if (!graphroot)
	{
		cout << "Failed to retrieve event name. Behavior graph root not found (ID: " << nodeID << ")";
		Error = true;
		return false;
	}

	if (!graphroot->data)
	{
		cout << "Failed to retrieve event name. Behavior graph data not found (ID: " << nodeID << ")";
		Error = true;
		return false;
	}

	if (!graphroot->data->variableInfos)
	{
		cout << "Failed to retrieve event name. Behavior graph data not found (ID: " << nodeID << ")";
		Error = true;
		return false;
	}

	return true;
}

usize variableinfopack::d_size()
{
	return scan_done ? v_size : size();
}

void variableinfopack::regis_size(size_t num)
{
	v_size = num;
	scan_done = true;
}

bool variableinfopack::hasVariableName(string name)
{
	return locator.find(name) != locator.end();
}

int variableinfopack::getID(string name)
{
	if (!hasVariableName(name))
	{
		cout << "Failed to retrieve variable id. Variable name not registered (Variable Name: " << name << ")";
		Error = true;
		return 0x7fffffff;
	}

	return locator[name];
}

void variableinfopack::push_back(v_datainfo datainfo)
{
	locator[datainfo.name] = size();
	vector<v_datainfo>::push_back(datainfo);
}

void variablefunc::matchScoring(variableinfopack& ori, variableinfopack& edit, string id, string classname)
{
	if (ori.scan_done) return;

	if (ori.size() == 0)
	{
		variableinfopack newOri;
		variableinfopack newEdit;
		newOri.reserve(edit.size());
		newEdit.reserve(edit.size());

		for (auto& ed : edit)
		{
			newOri.push_back(v_datainfo());
			newEdit.push_back(ed);
		}

		ori = newOri;
		edit = newEdit;
		return;
	}

	map<int, map<int, double>> scorelist;
	usize avg_threadsize = ori.size() / 5;
	bool dup = false;
	set<string> dupname;
	set<string> dupcheck;

	for (auto& each : edit)
	{
		if (dupcheck.find(each.name) == dupcheck.end())
		{
			dupcheck.insert(each.name);
		}
		else
		{
			dup = true;
			dupname.insert(each.name);
		}
	}

	if (!dup)
	{
		dupcheck.clear();

		for (auto& each : ori)
		{
			if (dupcheck.find(each.name) == dupcheck.end())
			{
				dupcheck.insert(each.name);
			}
			else
			{
				dup = true;
				dupname.insert(each.name);
			}
		}
	}

	if (avg_threadsize < 100)
	{
		fillScore(ori, edit, 0, ori.size(), scorelist, dup, dupname);
	}
	else
	{
		map<int, map<int, double>> scorelist1;
		map<int, map<int, double>> scorelist2;
		map<int, map<int, double>> scorelist3;
		map<int, map<int, double>> scorelist4;
		map<int, map<int, double>> scorelist5;
		boost::thread_group multi_t;

		usize start = 0;
		usize cap = start + avg_threadsize;
		multi_t.create_thread(boost::bind(&variablefunc::fillScore, boost::ref(ori), boost::ref(edit), start, cap, scorelist1, dup, dupname));

		start = cap;
		cap = start + avg_threadsize;
		multi_t.create_thread(boost::bind(&variablefunc::fillScore, boost::ref(ori), boost::ref(edit), start, cap, scorelist2, dup, dupname));

		start = cap;
		cap = start + avg_threadsize;
		multi_t.create_thread(boost::bind(&variablefunc::fillScore, boost::ref(ori), boost::ref(edit), start, cap, scorelist3, dup, dupname));

		start = cap;
		cap = start + avg_threadsize;
		multi_t.create_thread(boost::bind(&variablefunc::fillScore, boost::ref(ori), boost::ref(edit), start, cap, scorelist4, dup, dupname));

		start = cap;
		cap = ori.size();
		fillScore(ori, edit, start, cap, scorelist5, dup, dupname);
		multi_t.join_all();

		scorelist.insert(scorelist1.begin(), scorelist1.end());
		scorelist.insert(scorelist2.begin(), scorelist2.end());
		scorelist.insert(scorelist3.begin(), scorelist3.end());
		scorelist.insert(scorelist4.begin(), scorelist4.end());
		scorelist.insert(scorelist5.begin(), scorelist5.end());
	}

	vector<orderPair> pairing = highestScore(scorelist, ori.size(), edit.size());
	variableinfopack newOri;
	variableinfopack newEdit;
	newOri.reserve(ori.size());
	newEdit.reserve(edit.size());

	// assigning
	for (auto& order : pairing)
	{
		if (order.original == -1) newOri.push_back(v_datainfo());
		else newOri.push_back(ori[order.original]);

		if (order.edited == -1) newEdit.push_back(v_datainfo());
		else newEdit.push_back(edit[order.edited]);
	}

	newOri.regis_size(ori.size());
	newEdit.regis_size(edit.size());

	ori = newOri;
	edit = newEdit;
}

void variablefunc::fillScore(variableinfopack& ori, variableinfopack& edit, usize start, usize cap, map<int, map<int, double>>& scorelist, bool dup, set<string> dupname)
{
	if (dup)
	{
		// match scoring
		for (unsigned int i = start; i < cap; ++i)
		{
			for (unsigned int j = 0; j < edit.size(); ++j)
			{
				double curscore = 0;

				if (i == j)
				{
					curscore += 10;
				}
				else
				{
					int oriindex = i + 1;
					int newindex = j + 1;
					double difference = max(oriindex, newindex) - min(oriindex, newindex);
					difference = ((ori.size() - difference) / ori.size()) * 10;
					curscore += difference;
				}

				if (j < ori.size())
				{
					if (ori[i].proxy == edit[j].proxy)
					{
						++curscore;
					}

					if (ori[i].role.role == edit[j].role.role)
					{
						++curscore;
					}

					if (ori[i].role.flags == edit[j].role.flags)
					{
						++curscore;
					}

					if (ori[i].type == edit[j].type)
					{
						++curscore;
					}

					if (ori[i].value == edit[j].value)
					{
						if (ori[i].value <= 10)
						{
							++curscore;
						}
						else
						{
							curscore += 5;
						}
					}
				}

				if (ori[i].name == edit[j].name)
				{
					curscore += 13;
					scorelist[i][j] = curscore;

					if (i + 1 == ori.size())
					{
						while (++j < edit.size())
						{
							int oriindex = i + 1;
							int newindex = j + 1;
							double difference = max(oriindex, newindex) - min(oriindex, newindex);
							difference = ((ori.size() - difference) / ori.size()) * 10;
							scorelist[i][j] = difference;
						}

						break;
					}

					if (dupname.find(ori[i].name) == dupname.end()) break;
				}

				scorelist[i][j] = curscore;
			}
		}
	}
	else
	{
		// match scoring
		for (unsigned int i = start; i < cap; ++i)
		{
			if (i < edit.size())
			{
				if (ori[i].proxy == edit[i].proxy && ori[i].role.role == edit[i].role.role && ori[i].role.flags == edit[i].role.flags && ori[i].type == edit[i].type &&
					ori[i].value == edit[i].value && ori[i].name == edit[i].name)
				{
					scorelist[i][i] = 32;

					if (i + 1 == ori.size())
					{
						unsigned int j = 0;

						while (++j < edit.size())
						{
							int oriindex = i + 1;
							int newindex = j + 1;
							double difference = max(oriindex, newindex) - min(oriindex, newindex);
							difference = ((ori.size() - difference) / ori.size()) * 10;
							scorelist[i][j] = difference;
						}

						break;
					}

					continue;
				}
			}

			for (unsigned int j = 0; j < edit.size(); ++j)
			{
				double curscore = 0;

				if (i == j)
				{
					curscore += 10;
				}
				else
				{
					int oriindex = i + 1;
					int newindex = j + 1;
					double difference = max(oriindex, newindex) - min(oriindex, newindex);
					difference = ((ori.size() - difference) / ori.size()) * 10;
					curscore += difference;
				}

				if (j < ori.size())
				{
					if (ori[i].proxy == edit[j].proxy)
					{
						++curscore;
					}

					if (ori[i].role.role == edit[j].role.role)
					{
						++curscore;
					}

					if (ori[i].role.flags == edit[j].role.flags)
					{
						++curscore;
					}

					if (ori[i].type == edit[j].type)
					{
						++curscore;
					}

					if (ori[i].value == edit[j].value)
					{
						if (ori[i].value <= 10)
						{
							++curscore;
						}
						else
						{
							curscore += 5;
						}
					}
				}

				if (ori[i].name == edit[j].name)
				{
					curscore += 13;
					scorelist[i][j] = curscore;
					break;
				}

				scorelist[i][j] = curscore;

				if (i + 1 == ori.size())
				{
					while (++j < edit.size())
					{
						int oriindex = i + 1;
						int newindex = j + 1;
						double difference = max(oriindex, newindex) - min(oriindex, newindex);
						difference = ((ori.size() - difference) / ori.size()) * 10;
						scorelist[i][j] = difference;
					}
				}
			}
		}
	}
}
