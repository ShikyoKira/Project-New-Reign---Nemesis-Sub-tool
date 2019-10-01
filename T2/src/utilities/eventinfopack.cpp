#include <boost\thread.hpp>

#include "eventinfopack.h"
#include "highestscore.h"

#include "src\hkx\generator\hkbbehaviorgraph.h"

using namespace std;

safeStringUMap<shared_ptr<eventinfopack>> eventPackNode;
safeStringUMap<shared_ptr<eventinfopack>> eventPackNode_E;

extern vector<usize> datapacktracker;

EventId::EventId(int n_id)
{
	dataBake(n_id);
}

EventId::EventId(string n_id)
{
	dataBake(stoi(n_id));
}

void EventId::connectEventInfo(std::string n_nodeID, hkbbehaviorgraph* n_graphroot)
{
	nodeID = n_nodeID;
	graphroot = n_graphroot;
}

bool EventId::operator==(EventId& n_evnt)
{
	return getName() == n_evnt.getName();
}

bool EventId::operator!=(EventId& n_evnt)
{
	return getName() != n_evnt.getName();
}

string EventId::operator+(string line)
{
	return to_string(id) + line;
}

string EventId::operator=(string line)
{
	id = stoi(line);
	return to_string(id);
}

string EventId::getName()
{
	if (!errorCheck()) return "";

	if (id == -1) return "";

	eventinfopack& eventInfos = *graphroot->data->eventInfos;

	if (eventInfos.size() <= id)
	{
		cout << "Failed to retrieve event name. Event ID not registered (ID: " << nodeID << ")";
		Error = true;
		return "";
	}

	return eventInfos[id].name;
}

string EventId::getID()
{
	if (!errorCheck()) return "";

	string eventNodeId = graphroot->data->stringData->ID;

	if (IsForeign.find(eventNodeId) == IsForeign.end())
	{
		return to_string(id);
	}

	if (eventPackNode.find(eventNodeId) == eventPackNode.end())
	{
		cout << "Failed to retrieve event id. Event pack not recognized (ID: " << nodeID << ")";
		Error = true;
		return "";
	}

	string eventName = getName();

	return eventPackNode[eventNodeId]->hasEventName(eventName) ? to_string(eventPackNode[eventNodeId]->getID(eventName)) : ("$eventID[" + eventName + "]$");
}

void EventId::dataBake(int n_id)
{
	graphroot = nullptr;
	id = n_id;
}

bool EventId::errorCheck()
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

	if (!graphroot->data->eventInfos)
	{
		cout << "Failed to retrieve event name. Behavior graph data not found (ID: " << nodeID << ")";
		Error = true;
		return false;
	}

	return true;
}

size_t eventinfopack::d_size()
{
	return scan_done ? v_size : size();
}

void eventinfopack::regis_size(size_t num)
{
	v_size = num;
	scan_done = true;
}

bool eventinfopack::hasEventName(string name)
{
	return locator.find(name) != locator.end();
}

size_t eventinfopack::getID(string name)
{
	if (!hasEventName(name))
	{
		cout << "Failed to retrieve event id. Event name not registered (Event Name: " << name << ")";
		Error = true;
		return 0x7fffffff;
	}

	return locator[name];
}

void eventinfopack::push_back(e_datainfo datainfo)
{
	locator[datainfo.name] = size();
	vector<e_datainfo>::push_back(datainfo);
}

void eventfunc::matchScoring(eventinfopack& ori, eventinfopack& edit, string id, string classname)
{
	if (ori.scan_done) return;

	if (ori.size() == 0)
	{
		eventinfopack newOri;
		eventinfopack newEdit;
		newOri.reserve(edit.size());
		newEdit.reserve(edit.size());

		for (auto& ed : edit)
		{
			newOri.push_back(e_datainfo());
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
		multi_t.create_thread(boost::bind(&eventfunc::fillScore, boost::ref(ori), boost::ref(edit), start, cap, scorelist1, dup, dupname));

		start = cap;
		cap = start + avg_threadsize;
		multi_t.create_thread(boost::bind(&eventfunc::fillScore, boost::ref(ori), boost::ref(edit), start, cap, scorelist2, dup, dupname));

		start = cap;
		cap = start + avg_threadsize;
		multi_t.create_thread(boost::bind(&eventfunc::fillScore, boost::ref(ori), boost::ref(edit), start, cap, scorelist3, dup, dupname));

		start = cap;
		cap = start + avg_threadsize;
		multi_t.create_thread(boost::bind(&eventfunc::fillScore, boost::ref(ori), boost::ref(edit), start, cap, scorelist4, dup, dupname));

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
	eventinfopack newOri;
	eventinfopack newEdit;
	newOri.reserve(ori.size());
	newEdit.reserve(edit.size());

	// assigning
	for (auto& order : pairing)
	{
		if (order.original == -1) newOri.push_back(e_datainfo());
		else newOri.push_back(ori[order.original]);

		if (order.edited == -1) newEdit.push_back(e_datainfo());
		else newEdit.push_back(edit[order.edited]);
	}

	newOri.regis_size(ori.size());
	newEdit.regis_size(edit.size());

	ori = newOri;
	edit = newEdit;
}

void eventfunc::fillScore(eventinfopack& ori, eventinfopack& edit, usize start, usize cap, map<int, map<int, double>>& scorelist, bool dup, set<string> dupname)
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
					curscore += 5;
				}
				else
				{
					int oriindex = i + 1;
					int newindex = j + 1;
					double difference = max(oriindex, newindex) - min(oriindex, newindex);
					difference = ((ori.size() - difference) / ori.size()) * 5;
					curscore += difference;
				}

				if (ori[i].flags.FLAG_SILENT == edit[j].flags.FLAG_SILENT)
				{
					++curscore;
				}

				if (ori[i].flags.FLAG_SYNC_POINT == edit[j].flags.FLAG_SYNC_POINT)
				{
					++curscore;
				}

				if (ori[i].name == edit[j].name)
				{
					curscore += 10;
					scorelist[i][j] = curscore;

					if (i + 1 == ori.size())
					{
						while (++j < edit.size())
						{
							int oriindex = i + 1;
							int newindex = j + 1;
							double difference = max(oriindex, newindex) - min(oriindex, newindex);
							difference = ((ori.size() - difference) / ori.size()) * 5;
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
		for (unsigned int i = 0; i < ori.size(); ++i)
		{
			for (unsigned int j = 0; j < edit.size(); ++j)
			{
				double curscore = 0;

				if (i == j)
				{
					curscore += 5;
				}
				else
				{
					int oriindex = i + 1;
					int newindex = j + 1;
					double difference = max(oriindex, newindex) - min(oriindex, newindex);
					difference = ((ori.size() - difference) / ori.size()) * 5;
					curscore += difference;
				}

				if (ori[i].flags.FLAG_SILENT == edit[j].flags.FLAG_SILENT)
				{
					++curscore;
				}

				if (ori[i].flags.FLAG_SYNC_POINT == edit[j].flags.FLAG_SYNC_POINT)
				{
					++curscore;
				}

				if (ori[i].name == edit[j].name)
				{
					curscore += 10;
					scorelist[i][j] = curscore;

					if (i + 1 == ori.size())
					{
						while (++j < edit.size())
						{
							int oriindex = i + 1;
							int newindex = j + 1;
							double difference = max(oriindex, newindex) - min(oriindex, newindex);
							difference = ((ori.size() - difference) / ori.size()) * 5;
							scorelist[i][j] = difference;
						}
					}

					break;
				}

				scorelist[i][j] = curscore;
			}
		}
	}
}
