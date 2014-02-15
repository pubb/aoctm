#include "stdafx.h"
#include <math.h>
#include "Grouping.h"
#include "../playerdatabase.h"

/*
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <assert.h>
*/

//07-12-21, pubb
CGrouping::CGrouping(int num)
{
	if(num)
		Initialize(num);
}

CGrouping::~CGrouping()
{
}

void	CGrouping::Initialize(int num, BOOL prefer4v3)
{
	player_num = num;
	b4v3 = prefer4v3;
	memset(group1, 0, sizeof(group1));
	memset(group2, 0, sizeof(group2));

	memset(ratings, 0, sizeof(ratings));
	names.SetSize(8);
}

void	CGrouping::DoGroup(void)
{
	delta = 65536;

	if(player_num % 2)
		if(!b4v3)
			TryGroup2in1();	//generating 'delta, group1, group2', cooperators in group look like (player1 << 4 | player2)
		else
			TryGroup4v3(0, 1);
	else
		TryGrouping(0, 1, 0, true);	//generating 'delta, group1, group2'

	group1AverageScore = CalculateAverage(group1, group2);
	group2AverageScore = CalculateAverage(group2, group1);
	delta = group1AverageScore - group2AverageScore;
}

void CGrouping::SetRating(int num, int rating)
{
	if(num < player_num)
		ratings[num] = rating;
}

void CGrouping::SetName(int num, CString name)
{
	if(num < player_num)
		names[num] = name;
}

//return 1 to 8, not 0 to 7
int & CGrouping::GetGroup(int group_num, int num)
{
	int * group;
	if(group_num == 1)
		group = group1;
	else
		group = group2;

	return group[num];
}

CString CGrouping::GetGroupName(int group_num, int num)
{
	int * group;
	if(group_num == 1)
		group = group1;
	else
		group = group2;

	if((group[num] & 0x0f0) == 0)
		return names[group[num] - 1];
	CString str;
	str.Format(_T("%s | %s"), names[((group[num] & 0x0f0) >> 4) - 1], names[(group[num] & 0x0f) - 1]);
	return str;
}

int CGrouping::GetGroupRating(int * group, int num)
{
	//09-03-12, pubb, add for bug fix
	if(group[num] == 0)
		return 0;
	if((group[num] & 0x0f0) == 0)
		return ratings[group[num] - 1];
	return CPlayerDatabase::GetCooperateRating(ratings[((group[num] & 0x0f0) >> 4) - 1], ratings[(group[num] & 0x0f) -1]);
}

int CGrouping::CalculateAverage(int * thisgroup, int * thatgroup)
{
	float ratingf = 0.0;

	for(int i = 0; i < 4; i++)
	{
		ratingf = do_accumulate(ratingf, GetGroupRating(thisgroup, i));
	}
	
	int more = GetGroupCount(thisgroup), less = GetGroupCount(thatgroup);
	if(more > less)
		return CPlayerDatabase::GetOddMoreRating(do_average(ratingf, more), more, less);
	return do_average(ratingf, more);
}

int CGrouping::GetGroupCount(int * group)
{
	int count = 0;
	for(int i = 0; i < 4 ; i++)
		if(group[i] != 0)
			count++;
	return count;
}

void CGrouping::FillGroup(int *group1, int *group2)
{
	bool filled;
	int	index = 0;

	if(group1[0] == 0)
		return;

	for(int i = 1; i <= player_num; i++)
	{
		filled = false;
		for(int j = 3; j >= 0; j--)
		{
			if(group1[j] == 0)
				continue;
			if(group1[j] == i)
			{
				filled = true;
				break;
			}
			if(group1[j] < i)
				break;
		}
		if(!filled)
			group2[index++] = i;
	}
}

void CGrouping::TryGrouping(int index, int min, int odd, bool clear)
{
	static int tmp_group1[4], tmp_group2[4], tmp_delta;
	int i, max = player_num - (player_num / 2 + odd - (index + 1));

	if(clear)
	{
		memset(tmp_group1, 0, sizeof(tmp_group1));
		memset(tmp_group2, 0, sizeof(tmp_group2));
	}

	while(index < player_num / 2 + odd)
	{
		tmp_group1[index] = min;
		
		if(index + 1 >= player_num / 2 + odd)
			break;

		for(i = min + 1; i <= max + 1; i++)
			TryGrouping(index + 1, i, odd);
		return;
	}
	FillGroup(tmp_group1, tmp_group2);
	tmp_delta = CalculateAverage(tmp_group1, tmp_group2) - CalculateAverage(tmp_group2, tmp_group1);
	if(abs(delta) > abs(tmp_delta))
	{
		delta = tmp_delta;
		memcpy(group1, tmp_group1, sizeof(tmp_group1));
		memcpy(group2, tmp_group2, sizeof(tmp_group2));
	}
}

void CGrouping::TryGroup4v3(int index, int min)
{
	int tmp_group1[4], tmp_group2[4], tmp_delta;

	TryGrouping(index, min, 0, true);
	
	tmp_delta = delta;
	memcpy(tmp_group1, group1, sizeof(tmp_group1));
	memcpy(tmp_group2, group2, sizeof(tmp_group2));

	TryGrouping(index, min, 1, true);

	if(abs(delta) > abs(tmp_delta))
	{
		delta = tmp_delta;
		memcpy(group1, tmp_group1, sizeof(tmp_group1));
		memcpy(group2, tmp_group2, sizeof(tmp_group2));
	}
}

void CGrouping::TryGroup2in1(void)
{
	int tmp_ratings[8];
	int bestgroup1[4], bestgroup2[4], bestdelta = 65536;

	for(int i = 1; i <= player_num; i++)
	{
		for(int j = i + 1; j <= player_num; j++)
		{
			memset(group1, 0, sizeof(group1));
			memset(group2, 0, sizeof(group2));

			int j1 = 0;
			for(int k = 1; k <= player_num; k++)
			{
				if(k == i)		//slot 'cooperator1' stores the cooperators' rating
					tmp_ratings[j1++] = CPlayerDatabase::GetCooperateRating(ratings[i - 1], ratings[j - 1]);
				else if(k != j)		//slot 'cooperator2' stores the next normal rating
					tmp_ratings[j1++] = ratings[k - 1];
			}
			CGrouping group_2in1;
			group_2in1.Initialize(player_num - 1);
			for(int t = 0; t < player_num - 1; t++)
			{
				group_2in1.SetRating(t, tmp_ratings[t]);
			}
			group_2in1.TryGrouping(0, 1, 0, true);
			if(abs(bestdelta) > abs(group_2in1.delta))
			{
				bestdelta = group_2in1.delta;
				group_2in1.AdjustCooperateGroup(group_2in1.group1, i, j);
				group_2in1.AdjustCooperateGroup(group_2in1.group2, i, j);
				memcpy(bestgroup1, group_2in1.group1, sizeof(group_2in1.group1));
				memcpy(bestgroup2, group_2in1.group2, sizeof(group_2in1.group2));
			}
		}
	}
	delta = bestdelta;
	memcpy(group1, bestgroup1, sizeof(group1));
	memcpy(group2, bestgroup2, sizeof(group2));
}

void CGrouping::AdjustCooperateGroup(int * group, int cooperator1, int cooperator2)
{
	for(int i = 0; i < 4; i++)
	{
		if(group[i] == cooperator1)	//cooperators
		{
			group[i] = (cooperator1 << 4) | cooperator2;
		}
		else if(group[i] >= cooperator2)
			group[i]++;
	}
}

//by wordless, different methods for averaging
const float average_grade = 1;	//pubb, 14-02-15, 2 for SQ
#define	ACCUMULATE_SIGMA		//pubb, 14-02-15, undefined for PI
float CGrouping::do_accumulate(float base, float value)
{
#ifdef	ACCUMULATE_SIGMA
	if(value < 0)
		return base - powf(-value, average_grade);
	else
		return base + powf(value, average_grade);
#else
	if(base == 0)
		base = 1;
	if(value == 0)
		value = 1;
	if(value < 0)
		return base / -value;
	else
		return base * value;
#endif
}

float CGrouping::do_average(float sum, int count)
{
#ifdef	ACCUMULATE_SIGMA
	return powf(sum / count, 1 / average_grade);
#else
	return powf(sum, 1.0 / count);
#endif
}