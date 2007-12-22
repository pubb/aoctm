#include "stdafx.h"
#include "Grouping.h"
#include "../playerdatabase.h"

/*
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <assert.h>
*/

Intermediator::Intermediator(int playerCount, int* ratings, int computeSteps, int expectedDeltaRatingValve)
	: playerCount_(playerCount), ratings_(ratings), computeSteps_(computeSteps), expectedDeltaRatingValve_(expectedDeltaRatingValve)
{
	ASSERT(playerCount % 2 == 0  && playerCount <= 8);
	for(int i = 0; i < INIT_INDIVIDUAL_COUNT; i ++)
		chromosomeScores_[i] = 0x7fffffff;
}

void
Intermediator::Grouping(int* group1, int* group2, int& group1AverageScore, int& group2AverageScore, int& delta)
{
	InitColony();
	for(int i = 0; i < computeSteps_; i ++)
	{
#ifdef _DEBUG
		printf("Genetic %d\n", i);
#endif
		int delta = Genetic();
		if( delta <= expectedDeltaRatingValve_)
			break;
	}
	RetrieveInfoFromBestChromosome(group1, group2, group1AverageScore, group2AverageScore, delta);
}

void 
Intermediator::InitColony()
{
	srand((unsigned)time(NULL));
	for(int j = 0; j < INIT_INDIVIDUAL_COUNT; j++)
	{
		char chromosome;
		chromosome ^= chromosome;
		int groupOnePlayerCount = 0;;
		while(groupOnePlayerCount != playerCount_/2)
		{
			int position = rand() % (playerCount_);
			groupOnePlayerCount = 0;
			chromosome |= 1 << position;
			for(int i = 0; i < playerCount_; i ++)
			{
				groupOnePlayerCount += (chromosome >> i) & 0x01;
			}
		}
		chromosomes_[j] = chromosome;
	}
}


int 
Intermediator::Genetic()
{
	memcpy(tempChromosomes_, chromosomes_, INIT_INDIVIDUAL_COUNT * sizeof(char));

	for(int i = 0; i < INIT_INDIVIDUAL_COUNT; i++)
	{
		tempChromosomes_[INIT_INDIVIDUAL_COUNT + i] = Crossover(chromosomes_[i]);
	}

	NextGeneration();

	Mutation();

	//DiagnosePrintChromosomes();

	return chromosomeScores_[0];
}

char 
Intermediator::Crossover(char chromosome)
{
	int playerA;
	while(true)
	{
		playerA = rand() % playerCount_;
		if(((chromosome >> playerA) & 0x01) != 0)
			break;
	}
	int playerB;
	while(true)
	{
		playerB = rand() % playerCount_;
		if(((chromosome >> playerB) & 0x01) == 0)
			break;
	}
	
	chromosome &= ~(1 << playerA);
	chromosome |= (1 << playerB);
	return chromosome;
}

void 
Intermediator::Mutation()
{
	for(int j = 0; j < INIT_INDIVIDUAL_COUNT / 4; j++)
	{
		char chromosome;
		chromosome ^= chromosome;
		int groupOnePlayerCount = 0;;
		while(groupOnePlayerCount != playerCount_/2)
		{
			int position = rand() % (playerCount_);
			groupOnePlayerCount = 0;
			chromosome |= 1 << position;
			for(int i = 0; i < playerCount_; i ++)
			{
				groupOnePlayerCount += (chromosome >> i) & 0x01;
			}
		}
		chromosomes_[INIT_INDIVIDUAL_COUNT / 4 * 3 + j] = chromosome;
		chromosomeScores_[INIT_INDIVIDUAL_COUNT / 4 * 3 + j] = 0x7fffffff;
	}
}

void 
Intermediator::Eval()
{
	int i, j;

	for(i = 0; i < INIT_INDIVIDUAL_COUNT * 2; i ++)
	{
		int group1Rating = 0;
		for(j = 0; j < playerCount_; j ++)
		{
			if(((tempChromosomes_[i] >> j) & 0x01) != 0) 
			{
				group1Rating += ratings_[j];
			}
		}
		int group2Rating = 0;
		for(j = 0; j < playerCount_; j ++)
		{
			if(((tempChromosomes_[i] >> j) & 0x01) == 0) 
			{
				group2Rating += ratings_[j];
			}
		}
		tempChromosomeScores_[i] = abs(group1Rating - group2Rating);
	}
}

void 
Intermediator::Sort()
{
	for(int i = 0; i < INIT_INDIVIDUAL_COUNT * 2 - 1; i++)
	{
		bool flag = false;
		for(int j = 0; j < INIT_INDIVIDUAL_COUNT * 2 - i - 1; j ++)
		{
			if(tempChromosomeScores_[j] > tempChromosomeScores_[j + 1])
			{
				flag = true;
				int score = tempChromosomeScores_[j];
				char chromosome = tempChromosomes_[j];
				tempChromosomeScores_[j] = tempChromosomeScores_[j + 1];
				tempChromosomes_[j] = tempChromosomes_[j + 1];
				tempChromosomeScores_[j + 1] = score;
				tempChromosomes_[j + 1] = chromosome;
			}	
		}
		if(flag == false)
			break;
	}
}

void
Intermediator::NextGeneration()
{
	Eval();
	Sort();

	int identicalCount = 0;
	int j = 0;
	for(int i = 0; i < INIT_INDIVIDUAL_COUNT * 2; i ++)
	{
		bool identical = false;
		for(int k = 0; k < j; k ++)
		{
			if(chromosomes_[k] == tempChromosomes_[i])
			{
				identical = true;
				identicalCount ++;
			}
		}
		if(!identical)
		{
			chromosomes_[j] = tempChromosomes_[i];
			chromosomeScores_[j ++] = tempChromosomeScores_[i];
		}
		
		if(j == INIT_INDIVIDUAL_COUNT)
			break;
	}
}

void
Intermediator::RetrieveInfoFromBestChromosome(int* group1, int* group2, int& group1TotalScore, int& group2TotalScore, int& delta)
{
	int group1Index = 0;
	int group2Index = 0;
	group1TotalScore = 0;
	group2TotalScore = 0;

	int i;

	for(i = 0; i < playerCount_; i ++)
	{
		if(((chromosomes_[0] >> i) & 0x01) != 0) 
		{
			group1[group1Index ++] = i + 1;
			group1TotalScore += ratings_[i];
		}
	}
	for(i = 0; i < playerCount_; i ++)
	{
		if(((chromosomes_[0] >> i) & 0x01) == 0) 
		{
			group2[group2Index ++] = i + 1;
			group2TotalScore += ratings_[i];
		}
	}

	delta = group1TotalScore - group2TotalScore;
}

void 
Intermediator::DiagnosePrintChromosomes()
{
	int i, j;

	for(i = 0; i < INIT_INDIVIDUAL_COUNT; i ++)
	{
		int group1AverageScore = 0;
		int group2AverageScore = 0;

		printf("No.%d ", i);
		printf("Gourp 1: ");
		for(j = 0; j < playerCount_; j ++)
		{
			if(((chromosomes_[i] >> j) & 0x01) != 0) 
			{
				printf("%d ", j+1);
				group1AverageScore += ratings_[j];
			}
		}
		printf("Gourp 2: ");
		for(j = 0; j < playerCount_; j ++)
		{
			if(((chromosomes_[i] >> j) & 0x01) == 0) 
			{
				printf("%d ", j+1);
				group2AverageScore += ratings_[j];
			}
		}
		printf("%d ---- ", abs(group1AverageScore - group2AverageScore));
		printf("%d\n", chromosomeScores_[i]);
	}
}

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
	memset(group1, 0, sizeof(int) * 4);
	memset(group2, 0, sizeof(int) * 4);

	memset(ratings, 0, sizeof(int) * 8);
	names.SetSize(8);
}

void	CGrouping::DoGroup(void)
{
	if(player_num % 2 == 0)
	{
		Intermediator	inter(player_num, ratings);
		inter.Grouping(group1, group2, group1TotalScore, group2TotalScore, delta);
	}
	else
	{
		if(player_num < 5 && b4v3)
			return;

		int bestgroup1[4], bestgroup2[4], bestdelta;
		memset(bestgroup1, 0 ,sizeof(bestgroup1));
		memset(bestgroup2, 0, sizeof(bestgroup2));
		bestdelta = 65536;

		if(player_num >= 5)		//do not support 2v1
		{
			TryGroup4v3();	//generating 'delta, group1, group2'
			if(abs(bestdelta) > abs(delta))
			{
				bestdelta = delta;
				memcpy(bestgroup1, group1, sizeof(group1));
				memcpy(bestgroup2, group2, sizeof(group2));
			}
		}

		if(!b4v3)
		{
			TryGroup2in1();	//generating 'delta, group1, group2', cooperators in group look like (player1 << 4 | player2)
			if(abs(bestdelta) > abs(delta))
			{
				bestdelta = delta;
				memcpy(bestgroup1, group1, sizeof(group1));
				memcpy(bestgroup2, group2, sizeof(group2));
			}
		}
		
		memcpy(group1, bestgroup1, sizeof(group1));
		memcpy(group2, bestgroup2, sizeof(group2));
		group1TotalScore = CalculateTotal(1);
		group2TotalScore = CalculateTotal(2);
		delta = group1TotalScore - group2TotalScore;
	}
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

int CGrouping::GetGroupRating(int group_num, int num)
{
	int * group;
	if(group_num == 1)
		group = group1;
	else
		group = group2;

	if((group[num] & 0x0f0) == 0)
		return ratings[group[num] - 1];
	return CPlayerDatabase::GetCooperateRating(ratings[((group[num] & 0x0f0) >> 4) - 1], ratings[(group[num] & 0x0f) -1]);
}

int CGrouping::CalculateTotal(int group_num)
{
	int rating = 0;

	for(int i = 0; i < 4; i++)
	{
		rating += GetGroupRating(group_num, i);
	}
	
	int more = GetGroupCount(group_num), less = GetGroupCount(group_num == 1 ? 2 : 1);
	if(more > less)
		return CPlayerDatabase::GetOddMoreRating(rating, more, less) * less / more;
	return rating;
}

int CGrouping::GetGroupCount(int group_num)
{
	int * group;
	if(group_num == 1)
		group = group1;
	else
		group = group2;

	int count = 0;
	for(int i = 0; i < 4 ; i++)
		if(group[i] != 0)
			count++;
	return count;
}

void CGrouping::TryGroup4v3(void)
{
	int tmp_group[8];
	int bestgroup1[4], bestgroup2[4], bestdelta = 65536;

	for(int i = 1; i <= player_num; i++)
	{
		for(int j = i + 1; j <= player_num; j++)
		{
			for(int k = j + 1; k <= player_num; k++)
			{
				memset(tmp_group, 0, sizeof(tmp_group));
				memset(group1, 0, sizeof(group1));
				memset(group2, 0, sizeof(group2));

				for(int t = 1; t <= player_num; t++)
				{
					if(t != i && t != j && t != k)
						tmp_group[t - 1] = 2;
					else
						tmp_group[t - 1] = 1;
				}
				int j1 = 0, j2 = 0;
				for(int t = 1; t <= player_num; t++)
				{
					if(tmp_group[t - 1] == 1)
						group1[j1++] = t;
					else
						group2[j2++] = t;
				}
				delta = CalculateTotal(1) - CalculateTotal(2);
				if(abs(bestdelta) > abs(delta))
				{
					bestdelta = delta;
					memcpy(bestgroup1, group1, sizeof(group1));
					memcpy(bestgroup2, group2, sizeof(group2));
				}
			}
		}
	}
	delta = bestdelta;
	memcpy(group1, bestgroup1, sizeof(group1));
	memcpy(group2, bestgroup2, sizeof(group2));
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
			Intermediator	inter(player_num - 1, tmp_ratings);
			inter.Grouping(group1, group2, group1TotalScore, group2TotalScore, delta);
			if(abs(bestdelta) > abs(delta))
			{
				bestdelta = delta;
				AdjustCooperateGroup(group1, i, j);
				AdjustCooperateGroup(group2, i, j);
				memcpy(bestgroup1, group1, sizeof(group1));
				memcpy(bestgroup2, group2, sizeof(group2));
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