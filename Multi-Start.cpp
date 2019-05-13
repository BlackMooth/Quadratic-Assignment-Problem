#include "../qclass.hh"
#include "../input_output.hh"
#include "../random.hh"
#include "../costs.hh"
#include "../local_Improvement.hh"
#include <list>
#include <limits>

typedef vector<int> solution;
typedef vector<solution> population;

const double infinity = std::numeric_limits<double>::infinity();

int WORSE_ACCEPTANCE_PROB = 0; //to be adjusted

solution SOL;
double COST = infinity;

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
		solution Z(NUMBER_OBJECTS);
		generateRandomSolution(Z, NUMBER_OBJECTS);
		SOL = Z;
		COST = calculate_cost(Z, NUMBER_OBJECTS-1);
		do {	
			if (print_data) {start_10secs = std::chrono::high_resolution_clock::now(); print_data = false; fout << COST << endl;}

			generateRandomSolution(Z, NUMBER_OBJECTS);
			twoOptImprovement(Z, WORSE_ACCEPTANCE_PROB);
			int X = calculate_cost(Z, NUMBER_OBJECTS-1); 
			if (X < COST) {COST = X; SOL = Z;} 
			
			auto finish_10secs = std::chrono::high_resolution_clock::now();
			auto finish = std::chrono::high_resolution_clock::now();
			elapsed_10secs = finish_10secs - start_10secs;
			elapsed = finish - start;
			if (elapsed_10secs.count() >= 10) {print_data = true;}
		} while (elapsed.count() <=  600); //Mesurem 10 minuts
		fout << endl << "Best solution found is: ";
		for (int i = 0; i < NUMBER_OBJECTS; i++) fout << SOL[i] << ' ';
		fout << endl << "With cost: " << COST << endl;
	}
}
