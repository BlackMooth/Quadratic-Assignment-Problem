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

int EPSILON = 0.001; // tolerance for stopping, to be adjusted
int WORSE_ACCEPTANCE_PROB = 0.15; //To be adjusted
int LOCAL_IMPROVEMENT = 10000;  //Number of local Improvements to perform

void threeOptImprovement(vector<int>& v) {
    for(int i=0; i < LOCAL_IMPROVEMENT; i++) {
      int value = calculate_cost(v, NUMBER_OBJECTS-1);
      int rand1 = generateRandomNumber(0, NUMBER_OBJECTS-1);
      int rand2 = generateRandomNumber(0, NUMBER_OBJECTS-1);
      int rand3 = generateRandomNumber(0, NUMBER_OBJECTS-1);
      while(rand1 == rand2 || rand2 == rand3 || rand1 == rand3)
      {
	rand1 = generateRandomNumber(0, NUMBER_OBJECTS-1);
      	rand2 = generateRandomNumber(0, NUMBER_OBJECTS-1);
	rand3 = generateRandomNumber(0, NUMBER_OBJECTS-1);
      }

      exchange(v, rand1, rand2);
      exchange(v, rand2, rand3);

      int newValue = calculate_cost(v, NUMBER_OBJECTS-1);
      if(newValue > value)
      {
      	double rand = generateDoubleRandomNumber();
         if(rand > WORSE_ACCEPTANCE_PROB)
         {
            exchange(v, rand2, rand3);
	      	exchange(v, rand1, rand2);
         }
      }
    }
}

int main() {
	srand(time(NULL));
	inputData(DISTANCE, FLOW, NUMBER_OBJECTS);
	solution SOL(NUMBER_OBJECTS); //best solution
	generateRandomSolution(SOL, NUMBER_OBJECTS);
	double COST = calculate_cost(SOL, NUMBER_OBJECTS-1);
	int k = 1;
	solution current_sol = SOL; //current solution
	double current_cost;
	while (true) { //it's a matter of fixing a CPU time
		while (k < NUMBER_OBJECTS) {  
			current_sol = SOL;
			for (int i = 0; i < NUMBER_OBJECTS; i++) {   //Changing the neighbourhood
				int r = generateRandomNumber(0, NUMBER_OBJECTS-1);
				int s =  generateRandomNumber(0, NUMBER_OBJECTS-1);
				while (r == s) s = generateRandomNumber(0, NUMBER_OBJECTS-1);
				exchange(current_sol, r, s);
			}
			
			threeOptImprovement(current_sol);
			current_cost = calculate_cost(current_sol, NUMBER_OBJECTS-1);

			if (current_cost < COST - EPSILON*double(COST)) {	//Store improved solution
				SOL = current_sol;
				COST = current_cost;
				k = 1;
			}
		    else k++;
		}
		cout << "Best solution found is: ";
		for (int i = 0; i < NUMBER_OBJECTS; i++) cout << SOL[i] << ' ';
		cout << endl << "With cost: " << COST << endl;
	}
}