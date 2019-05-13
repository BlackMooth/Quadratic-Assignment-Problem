#include "../qclass.hh"
#include "../input_output.hh"
#include "../random.hh"
#include "../costs.hh"
#include <limits>
#include <math.h>

/* Number of Generations */
const int NUMBER_ITERATIONS = 1000000;
  
const long infinity = std::numeric_limits<long>::infinity();
const long n_max = 851;
const long nb_iter_initialisation = 1000; // Connolly proposes nb_iterations/100

long max(int a, int b) {if (a > b) return(a); else return(b);};
double max(double a, double b) {if (a > b) return(a); else return(b);}
long min(int a, int b) {if (a < b) return(a); else return(b);}
double min(double a, double b) {if (a < b) return(a); else return(b);}


void annealing(vector<int>& best_sol, long & best_cost) {
  vector<int> p(NUMBER_OBJECTS);
  int i, r, s;
  int delta;
  long k = NUMBER_OBJECTS*(NUMBER_OBJECTS-1)/2, mxfail = k, nb_fail, n_iteration;
  long dmin = infinity, dmax = 0;
  double t0, tf, beta, tfound, temperature;

  for (i = 0; i < NUMBER_OBJECTS; i++) p[i] = best_sol[i];
  double cost = calculate_cost(p, NUMBER_OBJECTS-1);
  best_cost = cost;

  for (n_iteration = 0; n_iteration < nb_iter_initialisation; n_iteration++){
    r = generateRandomNumber(0, NUMBER_OBJECTS-1);
    s = generateRandomNumber(0, NUMBER_OBJECTS-2);
    if (s >= r) s++;

    delta = calculateValueSwap(p, r, s, NUMBER_OBJECTS);
    if (delta > 0)
     {dmin = min(dmin, delta); dmax = max(dmax, delta);} 
    cost = cost + delta;
    swap(p[r], p[s]);
   };
  t0 = dmin + (dmax - dmin)/10.0;
  tf = dmin;
  beta = (t0 - tf)/(NUMBER_ITERATIONS*t0*tf);

  nb_fail = 0;
  tfound = t0;
  temperature = t0;
  r = 1; s = 2;
  for (n_iteration = 0; n_iteration < NUMBER_ITERATIONS - nb_iter_initialisation; n_iteration++) {
      temperature = temperature / (1.0 + beta*temperature);

      s = s + 1;
      if (s >= NUMBER_OBJECTS)
       {r = r + 1; 
        if (r >= NUMBER_OBJECTS - 1) r = 1;
        s = r + 1;
       };

      delta = calculateValueSwap(p, r, s, NUMBER_OBJECTS);
      if ((delta < 0) or (generateDoubleRandomNumber() < exp(-double(delta)/temperature)) or mxfail == nb_fail){
	cost = cost + delta; 
	swap(p[r], p[s]);
	 nb_fail = 0;
	}
      else nb_fail++;

      if (mxfail == nb_fail) {beta = 0; temperature = tfound;};
      if (cost < best_cost) {
	best_cost = cost;
        for (i = 0; i < NUMBER_OBJECTS; i++) best_sol[i] = p[i];
        tfound = temperature;
      }
   }
  cout << "Best solution found : \n";
  for (i = 0; i < NUMBER_OBJECTS; i++) cout << best_sol[i] << ' ';
  cout << endl;
  cout << "Cost of the solution is: " << calculate_cost(best_sol, NUMBER_OBJECTS-1) << endl;  
 }

long nb_res = 1000, no_res, cost;

int main() {
  /* Starting random number generator */
  srand(time(NULL));
  inputData(DISTANCE, FLOW, NUMBER_OBJECTS, "data.dat");
  //cout << "Enter the number K of maximum steps for the algorithm: \n";
  //cin >> nb_res;
  vector<int> p(NUMBER_OBJECTS, -1);
  for (no_res = 0; no_res < nb_res; no_res++) {
    generateRandomSolution(p, NUMBER_OBJECTS);
    annealing(p, cost);
   }
 }
