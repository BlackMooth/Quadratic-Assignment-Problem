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

int EPSILON = 0; // tolerance for stopping, to be adjusted
int WORSE_ACCEPTANCE_PROB = 0; //To be adjusted
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
	vector<string> benchmarck;   //NEW
	listing(benchmarck);		//NEW
	bool print_data = false;
	for (int ind = 0; ind < benchmarck.size(); ind++) {
		std::stringstream ind_outp; 
		ind_outp << benchmarck[ind] << ".result";  //output file will have .result extension
		ofstream fout(ind_outp.str().c_str());
		auto start = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed;
		auto start_10secs = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed_10secs;
		inputData(DISTANCE, FLOW, NUMBER_OBJECTS, benchmarck[ind]);
		/* Initial solution */
		solution SOL(NUMBER_OBJECTS); //best solution
		generateRandomSolution(SOL, NUMBER_OBJECTS);
		double COST = calculate_cost(SOL, NUMBER_OBJECTS-1);
		solution current_sol = SOL; //current solution
		double current_cost;
		do {	
			if (print_data) {start_10secs = std::chrono::high_resolution_clock::now(); print_data = false; fout << COST << endl;}

			int k = 0;
			while (k < NUMBER_OBJECTS) {  
				current_sol = SOL;
				for (int i = 0; i < NUMBER_OBJECTS; i++) {   //Changing the neighbourhood
					int r = generateRandomNumber(0, NUMBER_OBJECTS-1);
					int s =  generateRandomNumber(0, NUMBER_OBJECTS-1);
					while (r == s) s = generateRandomNumber(0, NUMBER_OBJECTS-1);
					exchange(current_sol, r, s);
				}
				
				twoOptImprovement(current_sol, WORSE_ACCEPTANCE_PROB);
				current_cost = calculate_cost(current_sol, NUMBER_OBJECTS-1);

				if (current_cost < COST) {	//Store improved solution
					SOL = current_sol;
					COST = current_cost;
					k = 1;
				}
			    else k++;
			    auto finish_10secs = std::chrono::high_resolution_clock::now();
				auto finish = std::chrono::high_resolution_clock::now();
				elapsed_10secs = finish_10secs - start_10secs;
				elapsed = finish - start;
				if (elapsed_10secs.count() >= 10) {print_data = true;}
			}
		} while (elapsed.count() <=  30); //Mesurem 10 minuts
		fout << endl << "Best solution found is: ";
		for (int i = 0; i < NUMBER_OBJECTS; i++) fout << SOL[i] << ' ';
		fout << endl << "With cost: " << COST << endl;
    }
}
