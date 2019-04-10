#include "../qclass.hh"
#include "../input_output.hh"
#include "../random.hh"
#include <list>
#include <limits>
#include <iostream>

using namespace std;

typedef vector<int> solution;
typedef vector<solution> set_of_solutions;

const double infinity = std::numeric_limits<double>::infinity();

/* Parameters for GRASP algorithm */
int POPULATION = 15; //K solutions
int MAX_ITERATIONS = 10000;
double MAX_NO_IMPROVEMENT = 100;
double alpha = 0.3;
int WORSE_ACCEPTANCE_PROB = 0;

int random_RCL (vector<int>& RCL) {
    int n = RCL.size();
    if (n == 0) cout << "BITCH" << endl;
    int r = generateRandomNumber(0, n-1);
    return r;
}

/* Calculates the index first installation costs */
double calculate_cost(const vector<int>& p, int index) {
  int i, j;
  double c = 0;
  for (i = 0; i <= index; i++)
    for (j = 0; j <= index; j++)
       c = c + DISTANCE[i][j] * FLOW[p[i]][p[j]];
  return(c);
 }
 
void exchange(vector<int>& v, int i, int j) {
    int temp = v[i];
    v[i] = v[j];
    v[j] = temp;
}
      
void twoOptImprovement(vector<int>& v) {
    for(int i=0; i < NUMBER_OBJECTS; i++) {
      for(int j=i+1; j<NUMBER_OBJECTS; j++) {
      	int value = calculate_cost(v, NUMBER_OBJECTS-1);
      	exchange(v, i, j);
        int newValue = calculate_cost(v, NUMBER_OBJECTS-1);
        if(newValue > value) {
            double rand = generateDoubleRandomNumber();
            if(rand > WORSE_ACCEPTANCE_PROB) exchange(v, i, j);
        }
      }
    }
}

void localImprovement (vector<int>& s) {
    twoOptImprovement(s);
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
    inputData(DISTANCE, FLOW, NUMBER_OBJECTS);
    int sol_index; // index
    set_of_solutions K_SOLUTIONS = set_of_solutions(POPULATION, solution(NUMBER_OBJECTS, -1)); 
    /* Generating initial K (greedy) solutions */
    for (sol_index = 0; sol_index < POPULATION; sol_index++)  {
        generateGreedySol(K_SOLUTIONS[sol_index]);
    }
    double min_cost = infinity;
    int solution;
    for (sol_index = 0; sol_index < POPULATION; sol_index++) {
        localImprovement(K_SOLUTIONS[sol_index]);
        double min_aux = calculate_cost(K_SOLUTIONS[sol_index], NUMBER_OBJECTS-1);
        if (min_aux < min_cost) {
            min_cost = min_aux;
            solution = sol_index;
        }
    }
    /* Print result */
    cout << "Best solution found is: ";
    for (int i = 0; i < NUMBER_OBJECTS; i++) cout << K_SOLUTIONS[solution][i] << ' ';
    cout << endl;
    cout << "Cost of the solution is: " << min_cost << endl;
}     
