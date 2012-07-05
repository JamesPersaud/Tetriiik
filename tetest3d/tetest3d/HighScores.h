#include <string>
#include <stdio.h>
#include <vector>
#include <algorithm>

using namespace std;

const string HIGHSCORE_DEFAULT_FILEPATH = "high.scores";

//A highscore pair - a name and score.
typedef struct {
	char n[15];
	int s;
} HighScorePair;

//Copy char arrays
int CopyStrings(char in[], char out[],int size);
//Save highscores to file
void SaveSc(vector<HighScorePair> sc, string filename);
//Output highscores to std out
void SpewPairVector(vector<HighScorePair>& sc);
//Swap two highscores
void SwapScrs(HighScorePair one, HighScorePair two);
//Sort predicate for score pair
bool SortPredicate_HighScorePair_Desc(HighScorePair& a, HighScorePair& b);
bool SortPredicate_HighScorePair_Asc(HighScorePair& a, HighScorePair& b);
//Load a highscore pair vector from a file
vector<HighScorePair> LoadSc(string filename);
//Construct a new highscore pair 
HighScorePair NewHighScorePair(char s[], int i);
//Sort the pair
void SortHighScorePair(vector<HighScorePair>* pair);
//Sort the pair
void SortHighScorePair(vector<HighScorePair>* pair,bool ascending);