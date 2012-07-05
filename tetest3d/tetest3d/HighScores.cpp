#include "HighScores.h"

using namespace std;

bool SortPredicate_HighScorePair_Desc(HighScorePair& a, HighScorePair& b)
{
	if(a.s != b.s)
		return a.s > b.s;
	else
		return 1;
}

bool SortPredicate_HighScorePair_Asc(HighScorePair& a, HighScorePair& b)
{
	if(a.s != b.s)
		return a.s < b.s;
	else
		return 1;
}

void SpewPairVector(vector<HighScorePair>& sc)
{
	printf("\n");
	
	vector<HighScorePair>::iterator Iter;

	for(Iter = sc.begin();Iter!=sc.end();Iter++)
	{
		HighScorePair sp = (HighScorePair)(*Iter);
		printf("%s %i\n",sp.n,sp.s);
	}
}

HighScorePair NewHighScorePair(char s[], int i)
{
	HighScorePair sc;
	CopyStrings(s,sc.n,15);
	sc.s = i;
	return sc;
}

void SaveSc(vector<HighScorePair> sc, string filename)
{
	if(filename.length() <1)
		filename = HIGHSCORE_DEFAULT_FILEPATH;

	FILE*	SaveFile;

	SaveFile = fopen(filename.c_str(), "wb"); 

	if (SaveFile == NULL || sc.size() < 1)
	{ 
	  printf("BROKEN!");
	  return;
	}

	//write head
	const char	SIGNATURE[] = "HIS";
	int siz = sc.size();
	fwrite (SIGNATURE, sizeof(const char), sizeof(SIGNATURE), SaveFile);
	fwrite (&siz,sizeof(int),1,SaveFile);

	for(int i = 0;i<sc.size();i++)
	{
		fwrite(&sc[i],sizeof(HighScorePair),1,SaveFile);
	}

	fclose(SaveFile);
}

vector<HighScorePair> LoadSc(string filename)
{
	if(filename.length() <1)
		filename = HIGHSCORE_DEFAULT_FILEPATH;

	FILE*	LoadFile;
	vector<HighScorePair> retvec;
	
	LoadFile = fopen (filename.c_str(), "rb"); 

	if (LoadFile == NULL)	
	{
		printf("BROKEN");
		return retvec;
	}

	char  LOADSIGNATURE[4];
	int   vectorsize;	
	HighScorePair newpair;

	fread (LOADSIGNATURE, sizeof (char), 4, LoadFile);
	fread (&vectorsize, sizeof(int), 1, LoadFile);	

	for(int l=0;l<vectorsize;l++)
	{
		fread (&newpair, sizeof (HighScorePair), 1, LoadFile);
		retvec.push_back(newpair);
	}

	fclose(LoadFile);

	return retvec;
}

int CopyStrings(char in[], char out[],int size)
{			
	int warnings =0;
	int count =0;

	while(count<size && (*(out++) = *(in++))) //That c++ pointer strcpy thing	
		count++;	

	if(count==size)
	{		
		*(out++) = *"\0";		//Make sure string ends
		if(*(in++) != *"\0")	//String truncated
			warnings++;		
	}
	
	return warnings;
}

//Sort highscore pair descending
void SortHighScorePair(vector<HighScorePair>* pair)
{
	sort(pair->begin(),pair->end(),SortPredicate_HighScorePair_Desc);
}

//Sort highscore pair specifying direction
void SortHighScorePair(vector<HighScorePair>* pair, bool ascending)
{
	if(ascending)
		sort(pair->begin(),pair->end(),SortPredicate_HighScorePair_Asc);
	else
		sort(pair->begin(),pair->end(),SortPredicate_HighScorePair_Desc);
}


