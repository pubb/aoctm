/**************************************************************************
*
*	Author : Huang Jie, huangjie@nudt.edu.cn
*	version : 1.0
*	Genetic Algorithm for teaming AOC players by chromosome ratings  
*
**************************************************************************/

#ifndef Grouping_H__
#define Grouping_H__

class	CGrouping
{
public:
	int group1AverageScore, group2AverageScore, delta;
	int group1[4];
	int group2[4];

	CGrouping(int num = 0);
	~CGrouping();

	void Initialize(int num, BOOL prefer4v3 = FALSE);
	void DoGroup(void);
	void SetRating(int num, int rating);
	void SetName(int num, CString name);
	int & GetGroup(int group_num, int num);
	CString GetGroupName(int group_num, int num);
	int GetGroupRating(int * group, int num);
	int CalculateAverage(int * thisgroup, int * thatgroup);
	static float	do_accumulate(float base, float value);
	static float	do_average(float sum, int count);

private:
	int player_num;
	BOOL b4v3;
	int ratings[8];
	CStringArray names;

	void TryGrouping(int index, int min, int odd = 0, bool clear = false);
	void TryGroup4v3(int index, int min);
	void TryGroup2in1(void);
	void FillGroup(int *group1, int *group2);
	int GetGroupCount(int * group);
	void AdjustCooperateGroup(int * group, int cooperator1, int cooperator2);
};

#endif