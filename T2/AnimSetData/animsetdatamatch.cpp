#include "animationsetdata.h"
#include "animsetdatamatch.h"
#include "highestscore.h"

using namespace std;

bool matchProjectScoring(vector<shared_ptr<AnimSetDataProject>>& ori, vector<shared_ptr<AnimSetDataProject>>& edit, string filename)
{
	if (ori.size() == 0)
	{
		cout << "ERROR: Empty project detected (File: " << filename << ")" << endl;
		Error = true;
		return false;
	}

	int counter = 0;
	map<int, map<int, double>> scorelist;
	vector<shared_ptr<AnimSetDataProject>> newOri;
	vector<shared_ptr<AnimSetDataProject>> newEdit;

	// match scoring
	for (unsigned int i = 0; i < ori.size(); ++i)
	{
		for (unsigned int j = 0; j < edit.size(); ++j)
		{
			scorelist[i][j] = 0;

			if (ori[i]->project == edit[j]->project)
			{
				scorelist[i][j] += 100;
			}
		}
	}

	vector<orderPair> pairing = highestScore(scorelist, ori.size(), edit.size());

	if (Error)
	{
		return false;
	}

	// assigning
	for (auto& order : pairing)
	{
		if (order.original == -1)
		{
			newOri.push_back(make_shared<AnimSetDataProject>());
		}
		else
		{
			newOri.push_back(ori[order.original]);
		}

		if (order.edited == -1)
		{
			newEdit.push_back(make_shared<AnimSetDataProject>());
		}
		else
		{
			newEdit.push_back(edit[order.edited]);
		}
	}

	ori = newOri;
	edit = newEdit;
	return true;
}

bool matchDataScoring(map<string, datapack, alphanum_less>& ori, map<string, datapack, alphanum_less>& edit, string filename)
{
	if (ori.size() == 0)
	{
		cout << "ERROR: Empty datalist detected (File: " << filename << ")" << endl;
		Error = true;
		return false;
	}

	set<string, alphanum_less> order;

	// name ordering
	for (auto& file : ori)
	{
		order.insert(file.first);
	}

	for (auto& file : edit)
	{
		order.insert(file.first);
	}

	// assigning
	for (auto& file : order)
	{
		if (ori.find(file) == ori.end())
		{
			datapack dummy;
			ori[file] = dummy;
		}

		if (edit.find(file) == edit.end())
		{
			datapack dummy;
			edit[file] = dummy;
		}
	}

	return true;
}

bool matchNameScoring(vector<equip>& ori, vector<equip>& edit, string filename)
{
	int counter = 0;
	map<int, map<int, double>> scorelist;
	vector<equip> newOri;
	vector<equip> newEdit;

	// match scoring
	for (unsigned int i = 0; i < ori.size(); ++i)
	{
		for (unsigned int j = 0; j < edit.size(); ++j)
		{
			scorelist[i][j] = 0;

			if (ori[i].name == edit[j].name)
			{
				scorelist[i][j] += 100;
			}

			if (i == j)
			{
				scorelist[i][j] += 50;
			}
			else
			{
				int oriindex = i + 1;
				int newindex = j + 1;
				double difference = max(oriindex, newindex) - min(oriindex, newindex);
				difference = (ori.size() - difference) / ori.size();
				difference = difference * 50;
				scorelist[i][j] += difference;
			}
		}
	}

	vector<orderPair> pairing = highestScore(scorelist, ori.size(), edit.size());

	if (Error)
	{
		return false;
	}

	// assigning
	for (auto& order : pairing)
	{
		if (order.original == -1)
		{
			equip dummy;
			newOri.push_back(dummy);
		}
		else
		{
			newOri.push_back(ori[order.original]);
		}

		if (order.edited == -1)
		{
			equip dummy;
			newEdit.push_back(dummy);
		}
		else
		{
			newEdit.push_back(edit[order.edited]);
		}
	}

	ori = newOri;
	edit = newEdit;
	return true;
}

bool matchNameScoring(vector<typepack>& ori, vector<typepack>& edit, string filename)
{
	int counter = 0;
	map<int, map<int, double>> scorelist;
	vector<typepack> newOri;
	vector<typepack> newEdit;

	// match scoring
	for (unsigned int i = 0; i < ori.size(); ++i)
	{
		for (unsigned int j = 0; j < edit.size(); ++j)
		{
			scorelist[i][j] = 0;

			if (ori[i].name == edit[j].name)
			{
				scorelist[i][j] += 100;
			}

			if (i == j)
			{
				scorelist[i][j] += 50;
			}
			else
			{
				int oriindex = i + 1;
				int newindex = j + 1;
				double difference = max(oriindex, newindex) - min(oriindex, newindex);
				difference = (ori.size() - difference) / ori.size();
				difference = difference * 50;
				scorelist[i][j] += difference;
			}
		}
	}

	vector<orderPair> pairing = highestScore(scorelist, ori.size(), edit.size());

	if (Error)
	{
		return false;
	}

	// assigning
	for (auto& order : pairing)
	{
		if (order.original == -1)
		{
			typepack dummy;
			newOri.push_back(dummy);
		}
		else
		{
			newOri.push_back(ori[order.original]);
		}

		if (order.edited == -1)
		{
			typepack dummy;
			newEdit.push_back(dummy);
		}
		else
		{
			newEdit.push_back(edit[order.edited]);
		}
	}

	ori = newOri;
	edit = newEdit;
	return true;
}

bool matchNameScoring(vector<animpack>& ori, vector<animpack>& edit, string filename)
{
	int counter = 0;
	map<int, map<int, double>> scorelist;
	vector<animpack> newOri;
	vector<animpack> newEdit;

	// match scoring
	for (unsigned int i = 0; i < ori.size(); ++i)
	{
		for (unsigned int j = 0; j < edit.size(); ++j)
		{
			scorelist[i][j] = 0;

			if (ori[i].name == edit[j].name)
			{
				scorelist[i][j] += 100;
			}

			if (i == j)
			{
				scorelist[i][j] += 50;
			}
			else
			{
				int oriindex = i + 1;
				int newindex = j + 1;
				double difference = max(oriindex, newindex) - min(oriindex, newindex);
				difference = (ori.size() - difference) / ori.size();
				difference = difference * 50;
				scorelist[i][j] += difference;
			}
		}
	}

	vector<orderPair> pairing = highestScore(scorelist, ori.size(), edit.size());

	if (Error)
	{
		return false;
	}

	// assigning
	for (auto& order : pairing)
	{
		if (order.original == -1)
		{
			animpack dummy;
			newOri.push_back(dummy);
		}
		else
		{
			newOri.push_back(ori[order.original]);
		}

		if (order.edited == -1)
		{
			animpack dummy;
			newEdit.push_back(dummy);
		}
		else
		{
			newEdit.push_back(edit[order.edited]);
		}
	}

	ori = newOri;
	edit = newEdit;
	return true;
}

bool matchAtkScoring(vector<attackdata>& ori, vector<attackdata>& edit, string filename)
{
	int counter = 0;
	map<int, map<int, double>> scorelist;
	vector<attackdata> newOri;
	vector<attackdata> newEdit;

	// match scoring
	for (unsigned int i = 0; i < ori.size(); ++i)
	{
		for (unsigned int j = 0; j < edit.size(); ++j)
		{
			scorelist[i][j] = 0;

			if (ori[i].data == edit[j].data)
			{
				scorelist[i][j] += 100;
			}

			if (i == j)
			{
				scorelist[i][j] += 50;
			}
			else
			{
				int oriindex = i + 1;
				int newindex = j + 1;
				double difference = max(oriindex, newindex) - min(oriindex, newindex);
				difference = (ori.size() - difference) / ori.size();
				difference = difference * 50;
				scorelist[i][j] += difference;
			}
		}
	}

	vector<orderPair> pairing = highestScore(scorelist, ori.size(), edit.size());

	if (Error)
	{
		return false;
	}

	// assigning
	for (auto& order : pairing)
	{
		if (order.original == -1)
		{
			attackdata dummy;
			newOri.push_back(dummy);
		}
		else
		{
			newOri.push_back(ori[order.original]);
		}

		if (order.edited == -1)
		{
			attackdata dummy;
			newEdit.push_back(dummy);
		}
		else
		{
			newEdit.push_back(edit[order.edited]);
		}
	}

	ori = newOri;
	edit = newEdit;
	return true;
}

bool matchCRC32Scoring(vector<crc32>& ori, vector<crc32>& edit, string filename)
{
	int counter = 0;
	map<int, map<int, double>> scorelist;
	vector<crc32> newOri;
	vector<crc32> newEdit;

	// match scoring
	for (unsigned int i = 0; i < ori.size(); ++i)
	{
		for (unsigned int j = 0; j < edit.size(); ++j)
		{
			scorelist[i][j] = 0;

			if (ori[i].filepath == edit[j].filepath)
			{
				scorelist[i][j] += 50;
			}

			if (ori[i].filename == edit[j].filename)
			{
				scorelist[i][j] += 100;
			}

			if (i == j)
			{
				scorelist[i][j] += 20;
			}
			else
			{
				int oriindex = i + 1;
				int newindex = j + 1;
				double difference = max(oriindex, newindex) - min(oriindex, newindex);
				difference = (ori.size() - difference) / ori.size();
				difference = difference * 20;
				scorelist[i][j] += difference;
			}
		}
	}

	vector<orderPair> pairing = highestScore(scorelist, ori.size(), edit.size());

	if (Error)
	{
		return false;
	}

	// assigning
	for (auto& order : pairing)
	{
		if (order.original == -1)
		{
			crc32 dummy;
			newOri.push_back(dummy);
		}
		else
		{
			newOri.push_back(ori[order.original]);
		}

		if (order.edited == -1)
		{
			crc32 dummy;
			newEdit.push_back(dummy);
		}
		else
		{
			newEdit.push_back(edit[order.edited]);
		}
	}

	ori = newOri;
	edit = newEdit;
	return true;
}
