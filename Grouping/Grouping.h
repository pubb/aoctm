/**************************************************************************
*
*	Author : Huang Jie, huangjie@nudt.edu.cn
*	version : 1.0
*	Genetic Algorithm for teaming AOC players by chromosome ratings  
*
**************************************************************************/

#ifndef Grouping_H__
#define Grouping_H__

//initialize chromosome count
//if number of player is 8 then ...
//128 can get best result in 1 steps in most case
//64 can get best result in 20 steps in most case
//if set INIT_INDIVIDUAL_COUNT a larger number, it always gets best result in 1 steps
//maybe the scale of aoc teaming problem is too small for genetic algorithm 
//if number of player is 32 then ...
//it can get best result from 0 - 31 steps in most case.
#define INIT_INDIVIDUAL_COUNT 128

class Intermediator  
{
	int* ratings_; //the player ratings
	int playerCount_; //number of players
	int computeSteps_; //maxinum steps when running genetic algorithm
	int expectedDeltaRatingValve_; //valve value for stopping algorithm
	char chromosomes_[INIT_INDIVIDUAL_COUNT]; //a generation for one genetic algorithm step
	int chromosomeScores_[INIT_INDIVIDUAL_COUNT]; //chromosome score in a generation
	char tempChromosomes_[INIT_INDIVIDUAL_COUNT * 2]; //a temporary generation for middle of a step
	int tempChromosomeScores_[INIT_INDIVIDUAL_COUNT * 2];//idividual score in a temporary generation
	void InitColony(); //generate initial generation
	int Genetic(); //step genetic algorithm and return the current best result 
	void Eval(); //evaluate chromosome of temporary generation
	void Sort(); //sort temporary generation by delta ratings of two team
	char Crossover(char chromosome); //crossover operation of genetic algorithm
	void Mutation(); //mutation operation of genetic algorithm, it makes the generation various
	void NextGeneration(); //generate next generation
	//retrieve best chromosome information
	void RetrieveInfoFromBestChromosome(int* group1, int* group2, int& group1AverageScore, int& group2AverageScore, int& delta);
	void DiagnosePrintChromosomes(); //debug function, it prints the current generation information
public:
	//construct a new intermediator
	Intermediator(int playerCount, int* ratings, int computeSteps = 10, int expectedDeltaRatingValve = 1);
	//group AOC teams by players' ratings
	void Grouping(/*out*/int* group1, /*out*/int* group2, /*out*/int& group1TotalScore, /*out*/int& group2TotalScore, /*out*/int& delta);
};

#endif