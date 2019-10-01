#include "animationdata.h"
#include "highestscore.h"

using namespace std;

bool matchProjectScoring(vector<shared_ptr<AnimDataProject>>& ori, vector<shared_ptr<AnimDataProject>>& edit, string filename)
{
	if (ori.size() == 0)
	{
		cout << "ERROR: Empty project detected (File: " << filename << ")" << endl;
		Error = true;
		return false;
	}

	int counter = 0;
	map<int, map<int, double>> scorelist;

	// match scoring
	for (unsigned int i = 0; i < ori.size(); ++i)
	{
		for (unsigned int j = 0; j < edit.size(); ++j)
		{
			scorelist[i][j] = 0;

			if (ori[i]->name == edit[j]->name)
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
	vector<shared_ptr<AnimDataProject>> newOri;
	vector<shared_ptr<AnimDataProject>> newEdit;
	newOri.reserve(ori.size());
	newEdit.reserve(edit.size());

	if (Error)
	{
		return false;
	}

	// assigning
	for (auto& order : pairing)
	{
		if (order.original == -1)
		{
			newOri.push_back(make_shared<AnimDataProject>());
		}
		else
		{
			newOri.push_back(ori[order.original]);
		}

		if (order.edited == -1)
		{
			newEdit.push_back(make_shared<AnimDataProject>());
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

bool matchProjectScoring(vector<AnimDataPack>& ori, vector<AnimDataPack>& edit, string filename)
{
	size_t counter = 0;
	int adjustment = 0;
	vector<int> tryagain;
	unordered_map<int, bool> taken;
	unordered_map<int, bool> taken2;
	unordered_map<int, bool> removed;
	unordered_map<int, int> oldToNew;
	unordered_map<string, unordered_map<string, int>> SSIMap;
	unordered_map<string, unordered_map<string, bool>> SSBMap;
	unordered_map<string, unordered_map<string, bool>> IsExist;
	vector<AnimDataPack> newOri;
	vector<AnimDataPack> newEdit;
	vector<AnimDataPack> extraEdit;
	size_t size = max(ori.size(), edit.size());

	// match scoring
	for (unsigned int j = 0; j < edit.size(); ++j)
	{
		SSBMap[edit[j].name][edit[j].uniquecode] = true;
		SSIMap[edit[j].name][edit[j].uniquecode] = j;
		taken2[j] = true;
	}

	for (unsigned int i = 0; i < ori.size(); ++i)
	{
		if (SSBMap[ori[i].name].size() != 0)
		{
			if (SSBMap[ori[i].name][ori[i].uniquecode])
			{
				oldToNew[i] = SSIMap[ori[i].name][ori[i].uniquecode];
				IsExist[ori[i].name][ori[i].uniquecode] = true;
				taken2[oldToNew[i]] = false;
				taken[i] = true;
			}
			else
			{
				if (IsExist[ori[i].name][ori[i].uniquecode])
				{
					cout << "ERROR: Duplicated anim data detected" << endl;
					Error = true;
					return false;
				}

				tryagain.push_back(i);
				SSBMap[ori[i].name].erase(SSBMap[ori[i].name].find(ori[i].uniquecode));
			}
		}
		else
		{
			SSBMap.erase(SSBMap.find(ori[i].name));
		}
	}

	for (int& trying : tryagain)
	{
		bool nottaken = true;

		for (auto& result : SSBMap[ori[trying].name])
		{
			if (result.second && taken2[SSIMap[ori[trying].name][result.first]])
			{
				oldToNew[trying] = SSIMap[ori[trying].name][result.first];
				taken2[oldToNew[trying]] = false;
				nottaken = false;
				taken[trying] = true;
				break;
			}
		}

		if (nottaken)
		{
			removed[trying] = true;
		}
	}

	// assigning
	while (counter < size)
	{
		if (counter < ori.size())
		{
			newOri.push_back(ori[counter]);
		}
		else
		{
			newOri.push_back(*new AnimDataPack);
		}

		if (!removed[counter])
		{
			if (counter < ori.size() && taken[counter])
			{
				newEdit.push_back(edit[oldToNew[counter]]);
			}
		}
		else
		{
			newEdit.push_back(*new AnimDataPack);
		}

		++counter;
	}

	for (auto& num : taken2)
	{
		if (num.second)
		{
			extraEdit.push_back(edit[num.first]);
		}
	}

	newEdit.insert(newEdit.end(), extraEdit.begin(), extraEdit.end());

	ori = newOri;
	edit = newEdit;
	return true;
}

bool matchProjectScoring(vector<InfoDataPack>& ori, vector<InfoDataPack>& edit, string filename)
{
	size_t counter = 0;
	unordered_map<int, bool> taken;
	unordered_map<int, bool> taken2;
	unordered_map<int, bool> removed;
	unordered_map<int, int> oldToNew;
	unordered_map<string, bool> IsExist;
	unordered_map<string, string> uniquematch;
	vector<InfoDataPack> newOri;
	vector<InfoDataPack> newEdit;
	vector<InfoDataPack> extraEdit;
	size_t size = max(ori.size(), edit.size());

	// match scoring
	for (unsigned int j = 0; j < edit.size(); ++j)
	{
		uniquematch[edit[j].uniquecode] = to_string(j);
		taken2[j] = true;
	}

	for (unsigned int i = 0; i < ori.size(); ++i)
	{
		if (uniquematch[ori[i].uniquecode].length() > 0)
		{
			if (IsExist[ori[i].uniquecode])
			{
				cout << "ERROR: Duplicated info data detected" << endl;
				Error = true;
				return false;
			}

			int match = stoi(uniquematch[ori[i].uniquecode]);
			IsExist[ori[i].uniquecode] = true;
			oldToNew[i] = match;
			taken2[match] = false;
		}
		else
		{
			removed[i] = true;
		}

		taken[i] = true;
	}

	// assigning
	while (counter < size)
	{
		if (taken[counter])
		{
			newOri.push_back(ori[counter]);
		}
		else
		{
			newOri.push_back(*new InfoDataPack);
		}

		if (!removed[counter])
		{
			if (counter < ori.size() && uniquematch[ori[counter].uniquecode].length() > 0)
			{
				newEdit.push_back(edit[oldToNew[counter]]);
			}
		}
		else
		{
			newEdit.push_back(*new InfoDataPack);
		}

		++counter;
	}

	for (auto& num : taken2)
	{
		if (num.second)
		{
			extraEdit.push_back(edit[num.first]);
		}
	}

	newEdit.insert(newEdit.end(), extraEdit.begin(), extraEdit.end());
	
	ori = newOri;
	edit = newEdit;
	return true;
}

bool matchProjectScoring(vecstr& ori, vecstr& edit, string filename)
{
	map<int, map<int, double>> scorelist;

	// match scoring
	for (unsigned int i = 0; i < ori.size(); ++i)
	{
		for (unsigned int j = 0; j < edit.size(); ++j)
		{
			scorelist[i][j] = 0;

			if (ori[i] == edit[j])
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
	vecstr newOri;
	vecstr newEdit;
	newOri.reserve(ori.size());
	newEdit.reserve(edit.size());

	if (Error)
	{
		return false;
	}

	// assigning
	for (auto& order : pairing)
	{
		if (order.original == -1)
		{
			newOri.push_back("//* delete this line *//");
		}
		else
		{
			newOri.push_back(ori[order.original]);
		}

		if (order.edited == -1)
		{
			newEdit.push_back("//* delete this line *//");
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

bool matchDetailedScoring(vecstr& ori, vecstr& edit, string filename)
{
	map<int, map<int, double>> scorelist;

	// match scoring
	for (unsigned int i = 0; i < ori.size(); ++i)
	{
		for (unsigned int j = 0; j < edit.size(); ++j)
		{
			scorelist[i][j] = 0;

			if (stod(ori[i]) == stod(edit[j]))
			{
				scorelist[i][j] += 200;
			}
			else
			{
				double oriindex = stod(boost::regex_replace(string(ori[i]), boost::regex("[^0-9]*([0-9]+(\\.([0-9]+)?)?).*"), string("\\1"))) + 1;
				double newindex = stod(boost::regex_replace(string(edit[j]), boost::regex("[^0-9]*([0-9]+(\\.([0-9]+)?)?).*"), string("\\1"))) + 1;
				double max = max(oriindex, newindex);
				double difference = max - min(oriindex, newindex);
				difference = (max - difference) / max;
				difference = difference * 100;
				scorelist[i][j] += difference;
				max = max(i, j);
				difference = max - min(i, j);
				difference = (max - difference) / max;
				difference = difference * 100;
				scorelist[i][j] += difference;
			}
		}
	}

	vector<orderPair> pairing = highestScore(scorelist, ori.size(), edit.size());
	vecstr newOri;
	vecstr newEdit;
	newOri.reserve(ori.size());
	newEdit.reserve(edit.size());

	if (Error)
	{
		return false;
	}

	// assigning
	for (auto& order : pairing)
	{
		if (order.original == -1)
		{
			newOri.push_back("//* delete this line *//");
		}
		else
		{
			newOri.push_back(ori[order.original]);
		}

		if (order.edited == -1)
		{
			newEdit.push_back("//* delete this line *//");
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
