#include "stdafx.h"
#include "Grouping.h"

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

