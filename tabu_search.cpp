#include <stdio.h>
#include <algorithm>
#include <vector>
#include <string.h>
#include <time.h> 
#include <limits>

#include "../input_output.hh"
#include "../random.hh"
#include "../Qclass.hh"
#include "../costs.hh"

using namespace std;

typedef int*   type_vector;
typedef double** type_matrix;

const double infinity = std::numeric_limits<double>::infinity();

const double NUMBER_ITERATIONS = 1000000;

void transpose(int & a, int & b) {double temp = a; a = b; b = temp;}

int min(double a, double b) {if (a < b) return(a); else return(b);}


/*--------------------------------------------------------------------------*/
/*      As calculateValueSwap, but the value of delta[i][j] is supposed to-*/
/*    be known before the transposition of elements r and s     ----------*/
/*-----------------------------------------------------------------------*/
double compute_delta_part(vector<int>& p, type_matrix & delta, int i, int j, int r, int s) {
     return(delta[i][j]+(DISTANCE[r][i]-DISTANCE[r][j]+DISTANCE[s][j]-DISTANCE[s][i])*
     (FLOW[p[s]][p[i]]-FLOW[p[s]][p[j]]+FLOW[p[r]][p[j]]-FLOW[p[r]][p[i]])+
     (DISTANCE[i][r]-DISTANCE[j][r]+DISTANCE[j][s]-DISTANCE[i][s])*
     (FLOW[p[i]][p[s]]-FLOW[p[j]][p[s]]+FLOW[p[j]][p[r]]-FLOW[p[i]][p[r]]) );
  }

void tabu_search(vector<int>& best_sol, double& best_cost, double min_size, double max_size, double aspiration) {
  vector<int> p(NUMBER_OBJECTS);                        // current solution
  type_matrix delta;                    // store move costs
  type_matrix tabu_list;                // tabu status
  double current_iteration;               
  double current_cost;                    
  int i, j, k, i_retained, j_retained;  // indices
  delta = new double* [NUMBER_OBJECTS];
  for (i = 0; i < NUMBER_OBJECTS; i++) delta[i] = new double[NUMBER_OBJECTS];
  tabu_list = new double* [NUMBER_OBJECTS];
  for (i = 0; i < NUMBER_OBJECTS; i++) tabu_list[i] = new double[NUMBER_OBJECTS];

  /* Current solution initialization */
  for (i = 0; i < NUMBER_OBJECTS; i++) p[i] = best_sol[i];

  /* Initialization of current solution value and matrix of cost of moves */
  current_cost = 0;
  for (i = 0; i < NUMBER_OBJECTS; i++) for (j = 0; j < NUMBER_OBJECTS; j++){
    current_cost = current_cost + DISTANCE[i][j] * FLOW[p[i]][p[j]];
    if (i < j) delta[i][j] = calculateValueSwap(p, i, j, NUMBER_OBJECTS);
   }
  best_cost = current_cost;

  /****************** tabu list initialization *******************/
  for (i = 0; i < NUMBER_OBJECTS; i++) for (j = 0; j < NUMBER_OBJECTS; j++)
    tabu_list[i][j] = -(NUMBER_OBJECTS*i + j);

  /* main tabu search loop */
  for (current_iteration = 0; current_iteration < NUMBER_ITERATIONS; current_iteration++) {
  /** find best move (i_retained, j_retained) **/
    i_retained = infinity;       // in case all moves are tabu
    double min_delta = infinity;   // retained move cost
    bool autorized;               // move not tabu?
    bool aspired;                 // move forced?
    bool already_aspired = false; // in case many moves forced

    for (i = 0; i < NUMBER_OBJECTS; i++) 
      for (j = i+1; j < NUMBER_OBJECTS; j++)
       {
	autorized = (tabu_list[i][p[j]] < current_iteration) or (tabu_list[j][p[i]] < current_iteration);
        aspired = (tabu_list[i][p[j]] < current_iteration-aspiration) or (tabu_list[j][p[i]] < current_iteration-aspiration) or (current_cost + delta[i][j] < best_cost);                

        if ((aspired and !already_aspired) or // first move aspired
           (aspired and already_aspired and    // many move aspired
            (delta[i][j] < min_delta)   ) or // => take best one
           (!aspired and !already_aspired and  // no move aspired yet
            (delta[i][j] < min_delta) and autorized))
          {i_retained = i; j_retained = j;
           min_delta = delta[i][j];
           if (aspired) already_aspired = true;
          }
       }

    if (i_retained == infinity) cout << "All moves are tabu! \n"; 
    else 
     {/** transpose elements in pos. i_retained and j_retained **/
      transpose(p[i_retained], p[j_retained]);
      // update solution value
      current_cost = current_cost + delta[i_retained][j_retained];
      // forbid reverse move for a random number of iterations
      tabu_list[i_retained][p[j_retained]] = 
        current_iteration + generateRandomNumber(min_size,max_size);
      tabu_list[j_retained][p[i_retained]] = 
        current_iteration + generateRandomNumber(min_size,max_size);

      // best solution improved ?
      if (current_cost < best_cost)
       {best_cost = current_cost;
        for (k = 0; k < NUMBER_OBJECTS; k++) best_sol[k] = p[k];
        cout << "Solution of value " << best_cost 
             << " found at iteration " << current_iteration << '\n';
       }

      // update matrix of the move costs
      for (i = 0; i < NUMBER_OBJECTS; i++) for (j = i+1; j < NUMBER_OBJECTS; j++)
        if (i != i_retained && i != j_retained && 
            j != i_retained && j != j_retained)
         {delta[i][j] = 
            compute_delta_part(p, delta, 
                               i, j, i_retained, j_retained);}
        else
         {delta[i][j] = calculateValueSwap(p, i, j, NUMBER_OBJECTS);}
     }
      
   }
  // free memory
  for (i=0; i < NUMBER_OBJECTS; i++) delete[] delta[i]; delete[] delta;
  for (i=0; i < NUMBER_OBJECTS; i++) delete[] tabu_list[i]; 
  delete[] tabu_list;
} // tabu

vector<int> solution;     // solution (permutation) 
double cost;                // solution cost

int main() {
  /* Starting random number generator */
  srand(time(NULL));
  inputData(DISTANCE, FLOW, NUMBER_OBJECTS);
  solution = vector<int>(NUMBER_OBJECTS);
  generateRandomSolution(solution, NUMBER_OBJECTS);
  tabu_search(solution, cost, 9*NUMBER_OBJECTS/10, 11*NUMBER_OBJECTS/10, NUMBER_OBJECTS*NUMBER_OBJECTS*2); 
  cout << "Solution found by tabu search :\n";
  for (int i = 0; i < NUMBER_OBJECTS; i++) cout << solution[i] << ' '; 
  cout << endl << "With cost: " << calculate_cost(solution, NUMBER_OBJECTS-1) << endl;
 }
