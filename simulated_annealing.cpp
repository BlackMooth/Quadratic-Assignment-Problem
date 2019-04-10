#include "../qclass.hh"
#include "../input_output.hh"
#include "../random.hh"
#include <math.h>

/* Number of Generations */
const int NUMBER_ITERATIONS = 1000000;
  
const long n_max = 851;
const long infini = 1399999999;
const long nb_iter_initialisation = 1000; // Connolly proposes nb_iterations/100

long max(int a, int b) {if (a > b) return(a); else return(b);};
double max(double a, double b) {if (a > b) return(a); else return(b);}
long min(int a, int b) {if (a < b) return(a); else return(b);}
double min(double a, double b) {if (a < b) return(a); else return(b);}
void swap(int &a, int &b) {int temp = a; a = b; b = temp;}


int calculateValueSwap(vector<int>& p, int r, int s) {
  int d;
  d = (DISTANCE[r][r]-DISTANCE[s][s])*(FLOW[p[s]][p[s]]-FLOW[p[r]][p[r]]) +
      (DISTANCE[r][s]-DISTANCE[s][r])*(FLOW[p[s]][p[r]]-FLOW[p[r]][p[s]]);
  for (int k = 0; k < NUMBER_OBJECTS; k++) if (k!=r and k!=s)
    d = d + (DISTANCE[k][r]-DISTANCE[k][s])*(FLOW[p[k]][p[s]]-FLOW[p[k]][p[r]]) +
            (DISTANCE[r][k]-DISTANCE[s][k])*(FLOW[p[s]][p[k]]-FLOW[p[r]][p[k]]);
  return(d);
 }

long calculate_cost(vector<int>& p) {
  int i, j;
  long c = 0;
  for (i = 0; i < NUMBER_OBJECTS; i++)
    for (j = 0; j < NUMBER_OBJECTS; j = j + 1)
       c = c + DISTANCE[i][j] * FLOW[p[i]][p[j]];
  return(c);
 }

void random_solution(vector<int>& p) {
  int i;
  for (int i = 0; i < NUMBER_OBJECTS; i++) p[i] = i;
  for (i = 1; i < NUMBER_OBJECTS; i++) swap(p[i-1], p[generateRandomNumber(i-1, NUMBER_OBJECTS-1)]);
 }

void annealing(vector<int>& best_sol, long & best_cost) {
  vector<int> p(NUMBER_OBJECTS);
  int i, r, s;
  int delta;
  long k = NUMBER_OBJECTS*(NUMBER_OBJECTS-1)/2, mxfail = k, nb_fail, n_iteration;
  long dmin = infini, dmax = 0;
  double t0, tf, beta, tfound, temperature;

  for (i = 0; i < NUMBER_OBJECTS; i++) p[i] = best_sol[i];
  long cost = calculate_cost(p);
  best_cost = cost;

  for (n_iteration = 0; n_iteration < nb_iter_initialisation; n_iteration++){
    r = generateRandomNumber(0, NUMBER_OBJECTS-1);
    s = generateRandomNumber(0, NUMBER_OBJECTS-2);
    if (s >= r) s++;

    delta = calculateValueSwap(p, r, s);
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

      delta = calculateValueSwap(p, r, s);
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
  cout << "Cost of the solution is: " << calculate_cost(best_sol) << endl;  
 }

long nb_res = 1000, no_res, cost;

int main() {
  inputData(DISTANCE, FLOW, NUMBER_OBJECTS);
  //cout << "Enter the number K of maximum steps for the algorithm: \n";
  //cin >> nb_res;
  vector<int> p(NUMBER_OBJECTS, -1);
  for (no_res = 0; no_res < nb_res; no_res++) {
    random_solution(p);
    annealing(p, cost);
   }
 }
