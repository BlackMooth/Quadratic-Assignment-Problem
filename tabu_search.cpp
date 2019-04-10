#include <stdio.h>
#include <algorithm>
#include <vector>
#include <string.h>
#include <time.h> 

#include "../input_output.hh"
#include "../random.hh"
#include "../Qclass.hh"

using namespace std;

typedef int*   type_vector;
typedef long** type_matrix;

const long infinity = std::numeric_limits<long>::infinity();

const long NUMBER_ITERATIONS = 1000000;

void transpose(int & a, int & b) {long temp = a; a = b; b = temp;}

int min(long a, long b) {if (a < b) return(a); else return(b);}



/*--------------------------------------------------------------*/
/*       compute the cost difference if elements i and j        */
/*         are transposed in permutation (solution) p           */
/*--------------------------------------------------------------*/
long compute_delta(vector<int>& p, int i, int j) {
  long d; int k;
  d = (DISTANCE[i][i]-DISTANCE[j][j])*(FLOW[p[j]][p[j]]-FLOW[p[i]][p[i]]) +
      (DISTANCE[i][j]-DISTANCE[j][i])*(FLOW[p[j]][p[i]]-FLOW[p[i]][p[j]]);
  for (k = 0; k < NUMBER_OBJECTS; k++) if (k!=i and k!=j)
    d = d + (DISTANCE[k][i]-DISTANCE[k][j])*(FLOW[p[k]][p[j]]-FLOW[p[k]][p[i]]) +
            (DISTANCE[i][k]-DISTANCE[j][k])*(FLOW[p[j]][p[k]]-FLOW[p[i]][p[k]]);
  return(d);
 }

/*--------------------------------------------------------------*/
/*      Idem, but the value of delta[i][j] is supposed to       */
/*    be known before the transposition of elements r and s     */
/*--------------------------------------------------------------*/
long compute_delta_part(vector<int>& p, type_matrix & delta, int i, int j, int r, int s) {
     return(delta[i][j]+(DISTANCE[r][i]-DISTANCE[r][j]+DISTANCE[s][j]-DISTANCE[s][i])*
     (FLOW[p[s]][p[i]]-FLOW[p[s]][p[j]]+FLOW[p[r]][p[j]]-FLOW[p[r]][p[i]])+
     (DISTANCE[i][r]-DISTANCE[j][r]+DISTANCE[j][s]-DISTANCE[i][s])*
     (FLOW[p[i]][p[s]]-FLOW[p[j]][p[s]]+FLOW[p[j]][p[r]]-FLOW[p[i]][p[r]]) );
  }

void tabu_search(vector<int>& best_sol, long& best_cost, long min_size, long max_size, long aspiration) {
  vector<int> p(NUMBER_OBJECTS);                        // current solution
  type_matrix delta;                    // store move costs
  type_matrix tabu_list;                // tabu status
  long current_iteration;               
  long current_cost;                    
  int i, j, k, i_retained, j_retained;  // indices
  delta = new long* [NUMBER_OBJECTS];
  for (i = 0; i < NUMBER_OBJECTS; i++) delta[i] = new long[NUMBER_OBJECTS];
  tabu_list = new long* [NUMBER_OBJECTS];
  for (i = 0; i < NUMBER_OBJECTS; i++) tabu_list[i] = new long[NUMBER_OBJECTS];

  /* Current solution initialization */
  for (i = 0; i < NUMBER_OBJECTS; i++) p[i] = best_sol[i];

  /* Initialization of current solution value and matrix of cost of moves */
  current_cost = 0;
  for (i = 0; i < NUMBER_OBJECTS; i++) for (j = 0; j < NUMBER_OBJECTS; j++){
    current_cost = current_cost + DISTANCE[i][j] * FLOW[p[i]][p[j]];
    if (i < j) delta[i][j] = compute_delta(p, i, j);
   }
  best_cost = current_cost;

  /****************** tabu list initialization *******************/
  for (i = 0; i < NUMBER_OBJECTS; i++) for (j = 0; j < NUMBER_OBJECTS; j++)
    tabu_list[i][j] = -(NUMBER_OBJECTS*i + j);

  /* main tabu search loop */
  for (current_iteration = 0; current_iteration < NUMBER_ITERATIONS; current_iteration++) {
  /** find best move (i_retained, j_retained) **/
    i_retained = infinity;       // in case all moves are tabu
    long min_delta = infinity;   // retained move cost
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
         {delta[i][j] = compute_delta(p, i, j);}
     }
      
   }
  // free memory
  for (i=0; i < NUMBER_OBJECTS; i++) delete[] delta[i]; delete[] delta;
  for (i=0; i < NUMBER_OBJECTS; i++) delete[] tabu_list[i]; 
  delete[] tabu_list;
} // tabu

void generate_random_solution(vector<int>& p) {
  int i;
  for (i = 0; i < NUMBER_OBJECTS; i++) p[i] = i;
  for (i = 1; i <  NUMBER_OBJECTS; i++) transpose(p[i], p[generateRandomNumber(i, NUMBER_OBJECTS-1)]);
 }

vector<int> solution;     // solution (permutation) 
long cost;                // solution cost

int main() {
  inputData(DISTANCE, FLOW, NUMBER_OBJECTS);
  solution = vector<int>(NUMBER_OBJECTS);
  generate_random_solution(solution);
  tabu_search(solution, cost, 9*NUMBER_OBJECTS/10, 11*NUMBER_OBJECTS/10, NUMBER_OBJECTS*NUMBER_OBJECTS*2); 
  cout << "Solution found by tabu search :\n";
  for (int i = 0; i < NUMBER_OBJECTS; i++) cout << solution[i] << ' '; 
  cout << endl;
 }
