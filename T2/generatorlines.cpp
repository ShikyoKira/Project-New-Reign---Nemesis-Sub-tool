#include "generatorlines.h"

using namespace std;

bool generatorMatch(vector<string>& ori, vector<string>& edit, string id, string classname);

bool generatorLines(vector<string>& storeline1, vector<string>& storeline2, string id, string classname)
{
	storeline1.reserve(FunctionLineTemp[id].size());
	vector<string> origen;
	int contline;
	bool start = false;
	string line;

	if (FunctionLineTemp[id].size() > 0)
	{
		for (unsigned int i = 0; i < FunctionLineTemp[id].size(); ++i)
		{
			line = FunctionLineTemp[id][i];

			if (start)
			{
				if (line.find("</hkparam>") != string::npos)
				{
					contline = i;
					break;
				}

				stringstream sstream(line);
				istream_iterator<string> ssbegin(sstream);
				istream_iterator<string> ssend;
				vector<string> curElements(ssbegin, ssend);
				copy(curElements.begin(), curElements.end(), curElements.begin());
				origen.insert(origen.end(), curElements.begin(), curElements.end());
			}
			else if ((line.find("<hkobject>", 0) == string::npos) && (line.find("</hkobject>", 0) == string::npos) && (line.find("</hkparam>", 0) == string::npos || line.find("</hkparam>", 0) > 10))
			{
				storeline1.push_back(line);
			}

			if (line.find("numelements=\"") != string::npos)
			{
				start = true;
			}
		}
	}
	else
	{
		cout << "ERROR: Edit " << classname << " Input Not Found (ID: " << id << ")" << endl;
		Error = true;
	}

	//stage 2 reading and identifying edits
	vector<string> newgen;
	start = false;
	int contline2;
	storeline2.reserve(FunctionLineNew[id].size());

	if (FunctionLineNew[id].size() > 0)
	{
		for (unsigned int i = 0; i < FunctionLineNew[id].size(); ++i)
		{
			line = FunctionLineNew[id][i];

			if (start)
			{
				if (line.find("</hkparam>") != string::npos)
				{
					contline2 = i;
					break;
				}

				stringstream sstream(line);
				istream_iterator<string> ssbegin(sstream);
				istream_iterator<string> ssend;
				vector<string> curElements(ssbegin, ssend);
				copy(curElements.begin(), curElements.end(), curElements.begin());
				newgen.insert(newgen.end(), curElements.begin(), curElements.end());
			}
			else if ((line.find("<hkobject>", 0) == string::npos) && (line.find("</hkobject>", 0) == string::npos) && (line.find("</hkparam>", 0) == string::npos || line.find("</hkparam>", 0) > 10))
			{
				storeline2.push_back(line);
			}

			if (line.find("numelements=\"") != string::npos)
			{
				start = true;
			}
		}
	}
	else
	{
		cout << "ERROR: Edit " << classname << " Output Not Found (ID: " << id << ")" << endl;
		Error = true;
		return false;
	}

	if (!generatorMatch(origen, newgen, id, classname))
	{
		return false;
	}

	int counter = 0;
	vector<string> generator1;
	vector<string> generator2;
	vector<string> extra;

	if (origen.size() != newgen.size())
	{
		cout << classname << " generator bug detected (ID: " << id << ")" << endl;
		Error = true;
		return false;
	}

	for (unsigned int i = 0; i < origen.size(); ++i)
	{
		generator1.push_back("				");
		generator2.push_back("				");

		if (origen[i].length() > 0)
		{
			generator1.back().append(origen[i]);
		}

		if (newgen[i].length() > 0)
		{
			if (origen[i].length() > 0)
			{
				generator2.back().append(newgen[i]);
			}
			else
			{
				generator1.pop_back();
				generator2.pop_back();

				if (counter == 0)
				{
					extra.push_back("				");
					extra.back().append(newgen[i]);
				}
				else
				{
					extra.back().append(" " + newgen[i]);
				}

				++counter;

				if (counter == 16)
				{
					counter == 0;
				}
			}
		}
	}

	generator2.insert(generator2.end(), extra.begin(), extra.end());
	storeline1.insert(storeline1.end(), generator1.begin(), generator1.end());
	storeline2.insert(storeline2.end(), generator2.begin(), generator2.end());

	for (unsigned int i = contline; i < FunctionLineTemp[id].size(); ++i)
	{
		line = FunctionLineTemp[id][i];

		if ((line.find("<hkobject>", 0) == string::npos) && (line.find("</hkobject>", 0) == string::npos) && (line.find("</hkparam>", 0) == string::npos || line.find("</hkparam>", 0) > 10))
		{
			storeline1.push_back(line);
		}
	}

	for (unsigned int i = contline2; i < FunctionLineNew[id].size(); ++i)
	{
		line = FunctionLineNew[id][i];

		if ((line.find("<hkobject>", 0) == string::npos) && (line.find("</hkobject>", 0) == string::npos) && (line.find("</hkparam>", 0) == string::npos || line.find("</hkparam>", 0) > 10))
		{
			storeline2.push_back(line);
		}
	}

	return true;
}

bool generatorMatch(vector<string>& ori, vector<string>& edit, string id, string classname)
{
	if (ori.size() == 0)
	{
		cout << "ERROR: " << classname << " empty original trigger (ID: " << id << ")" << endl;
		Error = true;
		return false;
	}

	int counter = 0;
	map<int, map<int, int>> scorelist;
	map<int, bool> taken;
	vector<string> newOri;
	vector<string> newEdit;

	// match scoring
	for (unsigned int i = 0; i < ori.size(); ++i)
	{
		for (unsigned int j = 0; j < edit.size(); ++j)
		{
			scorelist[i][j] = 0;

			if (ori[i] == edit[j])
			{
				++scorelist[i][j];

				if (i == j)
				{
					++scorelist[i][j];
				}
			}
		}
	}

	vector<orderPair> pairing = highestScore(scorelist, ori.size(), edit.size());

	// assigning
	for (auto& order : pairing)
	{
		if (order.original == -1)
		{
			newOri.push_back("");
		}
		else
		{
			newOri.push_back(ori[order.original]);
		}

		if (order.edited == -1)
		{
			newEdit.push_back("");
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
