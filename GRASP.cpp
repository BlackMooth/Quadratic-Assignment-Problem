#include "../qclass.hh"
#include "../input_output.hh"
#include "../random.hh"
#include "../costs.hh"
#include "../local_Improvement.hh"
#include <list>
#include <limits>
#include <iostream>

using namespace std;

typedef vector<int> solution;
typedef vector<solution> set_of_solutions;

const double infinity = std::numeric_limits<double>::infinity();

/* Parameters for GRASP algorithm */
int POPULATION = 10; //K solutions each loop iteration
double alpha = 0.3;
int WORSE_ACCEPTANCE_PROB = 0;  //To be adjusted

double COST = infinity;
solution SOL;

int random_RCL (vector<int>& RCL) {
    int n = RCL.size();
    int r = generateRandomNumber(0, n-1);
    return r;
}

void localImprovement (vector<int>& s) {
    twoOptImprovement(s, WORSE_ACCEPTANCE_PROB);
}

/* Generates a Restricted Candidate List for each element in a solution */ 
vector<int> make_candidate_list (vector<int>& sol, vector<bool>& visited, int index) {
    double c_max = 0, c_min = infinity;
    list<pair<int, double> > costs;
    vector<int> RCL; //Restricted Candidate List
    /* Calculate all costs */
    for (int i = 0; i < NUMBER_OBJECTS; i++) {
        if (!visited[i]) {
            sol[index] = i;
            double c = calculate_cost(sol, index);
            costs.push_back(make_pair(i, c));
            if (c >= c_max) c_max = c;
            if (c <= c_min) c_min = c;
        }
    }

    list <pair<int, double> >::iterator it; 
    for (it = costs.begin(); it != costs.end(); it++) {
        /* Alpha criterion for building RCL */
        if ((*it).second <= (c_min + alpha*(c_max-c_min))) RCL.push_back((*it).first);
    }
    return RCL;
}

/* Generates, for each of K solutions, a random orientated initial configuration to be optimized */
void generateGreedySol (vector<int>& sol) {
    int number_elements = 0;
    double cost_aux = 0;
    vector<bool> visited(NUMBER_OBJECTS, false);
    int r = generateRandomNumber(0, NUMBER_OBJECTS-1); //Initial random first element
    sol[0] = r;
    visited[r] = true;
    number_elements++;
    while (number_elements < NUMBER_OBJECTS) {
            vector<int> RCL = make_candidate_list(sol, visited, number_elements);
            int selected = random_RCL(RCL);  //RCL index of element chosen
            sol[number_elements] = RCL[selected]; 
            visited[RCL[selected]] = true;
       number_elements++;
    }
}

int main() {
    /* Starting random number generator */
    srand(time(NULL));
    /* Input data */
    srand(time(NULL));
    vector<string> benchmarck;   //NEW
    listing(benchmarck);        //NEW
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

        int sol_index; // index
        do {
            if (print_data) {start_10secs = std::chrono::high_resolution_clock::now(); print_data = false; fout << COST << endl;}
            set_of_solutions K_SOLUTIONS = set_of_solutions(POPULATION, solution(NUMBER_OBJECTS, -1)); 
            /* Generating initial K (greedy) solutions */
            for (sol_index = 0; sol_index < POPULATION; sol_index++)  {
                generateGreedySol(K_SOLUTIONS[sol_index]);
            }
            int solution;
            for (sol_index = 0; sol_index < POPULATION; sol_index++) {
                localImprovement(K_SOLUTIONS[sol_index]);
                double min_aux = calculate_cost(K_SOLUTIONS[sol_index], NUMBER_OBJECTS-1);
                if (min_aux < COST) {
                    COST = min_aux;
                    SOL = K_SOLUTIONS[sol_index];
                    solution = sol_index;
                }

            auto finish_10secs = std::chrono::high_resolution_clock::now();
            auto finish = std::chrono::high_resolution_clock::now();
            elapsed_10secs = finish_10secs - start_10secs;
            elapsed = finish - start;
            if (elapsed_10secs.count() >= 10) {print_data = true;}                    
            }
        }  while (elapsed.count() <=  30); //Mesurem 10 minuts
        fout << endl << "Best solution found is: ";
        for (int i = 0; i < NUMBER_OBJECTS; i++) fout << SOL[i] << ' ';
        fout << endl << "With cost: " << COST << endl;
    }
}     
