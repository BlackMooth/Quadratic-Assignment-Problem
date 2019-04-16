#include "../qclass.hh"
#include "../input_output.hh"
#include "../random.hh"
#include "../costs.hh"
#include "../local_Improvement.hh"
#include <list>
#include <limits>
#include <iostream>

typedef vector<int> solution;
typedef vector<solution> population;

const double infinity = std::numeric_limits<double>::infinity();

int K_sols = 15; //to be adjusted
int MAX_ITERATIONS = 100000; //max #iterations in local improvement process, to be adjusted...
int WORSE_ACCEPTANCE_PROB = 0; //to be adjusted

solution SOL;
double COST = infinity;

int main() {
	srand(time(NULL));
	inputData(DISTANCE, FLOW, NUMBER_OBJECTS);
	solution Z(NUMBER_OBJECTS);
	generateRandomSolution(Z, NUMBER_OBJECTS-1);
	SOL = Z;
	COST = calculate_cost(Z, NUMBER_OBJECTS-1);

	for (int s = 1; s < K_sols; s++) {
		generateRandomSolution(Z, NUMBER_OBJECTS-1);
		for (int j = 0; j < MAX_ITERATIONS; j++) {
			twoOptImprovement(Z, WORSE_ACCEPTANCE_PROB);
			int X = calculate_cost(Z, NUMBER_OBJECTS-1); 
			if (X < COST) COST = X; 
		cout << "Best solution found is: ";
		for (int i = 0; i < NUMBER_OBJECTS; i++) cout << SOL[i] << ' ';
		cout << endl << "With cost: " << COST << endl;
		}
	}
}