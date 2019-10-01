#include <boost\thread.hpp>
#include "attributeinfopack.h"
#include "highestscore.h"

using namespace std;

safeStringUMap<shared_ptr<attributeinfopack>> attributePackNode;
safeStringUMap<shared_ptr<attributeinfopack>> attributePackNode_E;

usize attributeinfopack::d_size()
{
	return scan_done ? v_size : size();
}

void attributefunc::matchScoring(attributeinfopack& ori, attributeinfopack& edit, string id, string classname)
{
	if (ori.scan_done) return;

	if (ori.size() == 0)
	{
		attributeinfopack newOri;
		attributeinfopack newEdit;
		newOri.reserve(edit.size());
		newEdit.reserve(edit.size());

		for (auto& ed : edit)
		{
			newOri.push_back(a_datainfo());
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
		multi_t.create_thread(boost::bind(&attributefunc::fillScore, boost::ref(ori), boost::ref(edit), start, cap, scorelist1, dup, dupname));

		start = cap;
		cap = start + avg_threadsize;
		multi_t.create_thread(boost::bind(&attributefunc::fillScore, boost::ref(ori), boost::ref(edit), start, cap, scorelist2, dup, dupname));

		start = cap;
		cap = start + avg_threadsize;
		multi_t.create_thread(boost::bind(&attributefunc::fillScore, boost::ref(ori), boost::ref(edit), start, cap, scorelist3, dup, dupname));

		start = cap;
		cap = start + avg_threadsize;
		multi_t.create_thread(boost::bind(&attributefunc::fillScore, boost::ref(ori), boost::ref(edit), start, cap, scorelist4, dup, dupname));

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
	attributeinfopack newOri;
	attributeinfopack newEdit;
	newOri.reserve(ori.size());
	newEdit.reserve(edit.size());

	// assigning
	for (auto& order : pairing)
	{
		if (order.original == -1) newOri.push_back(a_datainfo());
		else newOri.push_back(ori[order.original]);

		if (order.edited == -1) newEdit.push_back(a_datainfo());
		else newEdit.push_back(edit[order.edited]);
	}

	newOri.regis_size(ori.size());
	newEdit.regis_size(edit.size());

	ori = newOri;
	edit = newEdit;
}

void attributefunc::fillScore(attributeinfopack& ori, attributeinfopack& edit, usize start, usize cap, map<int, map<int, double>>& scorelist, bool dup, set<string> dupname)
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

				if (ori[i].value == edit[j].value)
				{
					curscore += 7;
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

				if (ori[i].value == edit[j].value)
				{
					curscore += 7;
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

void attributeinfopack::regis_size(usize num)
{
	v_size = num;
	scan_done = true;
}
