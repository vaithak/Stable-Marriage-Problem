#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <random>
#include <chrono> 
#include <map>

using namespace std;

//  Trim whitespaces from a string 
void trim(string& s)
{
	const char* ws = " :\t\n\r\f\v";
	s.erase(s.find_last_not_of(ws) + 1);
    s.erase(0, s.find_first_not_of(ws));
}


// Print preferences string vectors
void printPref(vector<vector<string> > &matrix)
{
	for(int i=0; i<matrix.size(); i++)
	{
		for(int j=0; j<matrix[i].size(); j++)
		{
			cout<<matrix[i][j]<<" ";
		}
		cout<<endl;
	}
}


// Print all candidate's name
void printCandidates(vector<vector<string> > &men, vector<vector<string> > &women)
{
	cout<<"Men Candidates: ";
	for (int i = 0; i < men.size(); ++i)
	{
		cout<<men[i][0];
		if(i != men.size()-1)
			cout<<", ";
	}

	cout<<"\nWomen Candidates: ";
	for (int i = 0; i < women.size(); ++i)
	{
		cout<<women[i][0];
		if(i != women.size()-1)
			cout<<", ";
	}

	cout<<endl;
}


// Read Preferences from file
void readPref(vector<vector<string> > &Pref, ifstream &Fin)
{
	string temp;
	while(getline(Fin,temp,'\n'))
	{	
		stringstream ss;
		ss<<temp;

		string name;
		getline(ss,name,':');
		trim(name);

		vector<string> tempVec;
		tempVec.push_back(name);
		while(getline(ss,name,','))
		{
			trim(name);
			tempVec.push_back(name);
		}

		Pref.push_back(tempVec);
	}
}


// Checking if a women will change her current matching to newMan
bool willWomanChange(vector<vector<string> > &womenPref, string currWoman, string prevMan, string newMan)
{
	int i=0;
	while(womenPref[i][0] != currWoman)
	{
		i++;
	}

	for(int j=1;j<womenPref[i].size();j++)
	{
		if(womenPref[i][j] == prevMan)
			break;
		else if(womenPref[i][j] == newMan)
			return true;
	}	

	return false;
}


// Function to find a stable matching
void findMatching(vector<vector<string> > &menPref, vector<vector<string> > &womenPref, vector<vector<string> > &result)
{	
	map<string, string> menMatches;
	map<string, string> womenMatches;

	int n = menPref.size();  // also equal to womenPref.size()
	for(int i=0; i<n; i++)
	{
		menMatches[menPref[i][0]] = "NULL";
		womenMatches[womenPref[i][0]] = "NULL";
	}

	int freeMenCount = menPref.size();
	while(freeMenCount > 0)
	{
		int freeMenIndex=0;
		for(; freeMenIndex<menPref.size(); freeMenIndex++)
		{
			if(menMatches[menPref[freeMenIndex][0]] == "NULL")
				break;
		}

		int itr = 1;
		string currMan = menPref[freeMenIndex][0];
		while( itr < menPref[freeMenIndex].size() )
		{
			string currWoman = menPref[freeMenIndex][itr];
			if( womenMatches[currWoman] == "NULL")
			{
				womenMatches[currWoman] = currMan;
				menMatches[currMan]		= currWoman;
				menPref[freeMenIndex].erase( menPref[freeMenIndex].begin() + 1);
				freeMenCount -- ;
				break;
			}
			else
			{
				string prevMan = womenMatches[currWoman] ;
				if( willWomanChange(womenPref, currWoman, prevMan, currMan) )
				{
					menMatches[prevMan] = "NULL";
					menMatches[currMan] = currWoman;
					womenMatches[currWoman] = currMan;
					menPref[freeMenIndex].erase( menPref[freeMenIndex].begin() + 1);
					break;
				}	
				else
				{
					menPref[freeMenIndex].erase( menPref[freeMenIndex].begin() + 1);
				}
			}
		}
	}

	for(int i=0;i<n;i++)
	{
		vector<string> tempVec;
		tempVec.push_back(menPref[i][0]);
		tempVec.push_back(menMatches[menPref[i][0]]);
		result.push_back(tempVec);
	}
}


// Write a Stable Matching to file
void writePref(vector<vector<string> > &Match, ofstream &Fout)
{
	for(int i=0; i<Match.size(); i++)
	{
		Fout<<Match[i][0]<<" - "<<Match[i][1]<<"\n";
	}
}


//  Main Function
int main(int argc, char const *argv[])
{
	ifstream menFin,womenFin;
	menFin.open("menPref.txt");
	womenFin.open("womenPref.txt");

	vector<vector<string> > menPref;
	readPref(menPref, menFin);

	vector<vector<string> > womenPref;
	readPref(womenPref, womenFin);

	// printPref(menPref);
	// printPref(womenPref);

	printCandidates(menPref,womenPref);

	if(menPref.size() != womenPref.size())
	{
		cout<<"ERROR! Number of Men and Women should be equal\n";
	}
	else
	{
		unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    	default_random_engine rng(seed);
		shuffle(begin(menPref), end(menPref), rng);
		shuffle(begin(womenPref), end(womenPref), rng);

		vector<vector<string> > result;
		findMatching(menPref, womenPref, result);

		cout<<"\nStable Matching:\n";
		for(int i=0; i<result.size(); i++)
		{
			cout<<"\t"<<result[i][0]<<" - "<<result[i][1]<<endl;
		}

		ofstream fout;
		fout.open("stableMatching.txt");
		writePref(result,fout);
	}
	
	menFin.close();
	womenFin.close();

	return 0;
}