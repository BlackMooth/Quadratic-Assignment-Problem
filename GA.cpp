#include "../qclass.hh"
#include "../input_output.hh"
#include "../random.hh"
#include "../costs.hh"
#include "../local_Improvement.hh"
#include <limits>
#include <math.h>

using namespace std;

typedef vector<int> solution;
typedef vector<solution> population;

double COST = std::numeric_limits<double>::infinity();

/* Tournament selection of parents; ILS with all Pair Swap search;
 * Elitism in population, keeping 50% of best solutions at every generation */

    int pop_size;  //To be adjusted
    int MAX_GENERATIONS = 3000;
    int TOURNAMENT_SIZE = 4;
    double CROSS_P = 0.7;
    int WORSE_ACCEPTANCE_PROB = 0.1; //Prob of accepting worse solutions in local optimization process

bool comp(vector<int>& s1, vector<int>& s2){
    return calculate_cost(s1, NUMBER_OBJECTS-1) < calculate_cost(s2, NUMBER_OBJECTS-1);
}

void perturbation(solution& s, int u){
    int p = 0;
    
    while(p < u){
        p++;
        /*Pick two locations randomly*/
        int t = generateRandomNumber(0, NUMBER_OBJECTS-1);
        int v = generateRandomNumber(0, NUMBER_OBJECTS-1);
        while(v == t)
            v = generateRandomNumber(0, NUMBER_OBJECTS-1);

        /*Exhcange points (wheter it is better or not)*/
        exchange(s, t, v);
    }
}

solution limited_iterated_search(solution& s){ 
    solution best = s;
    /*Start picking a neighbor of s*/
    twoOptImprovement(s, WORSE_ACCEPTANCE_PROB);
    if(calculate_cost(s, NUMBER_OBJECTS-1) < calculate_cost(best, NUMBER_OBJECTS-1)) best = s;  

    int u = 2;
    int iter = 0;
    while(iter < MAX_GENERATIONS*0.1){
        iter++;
        /*Perturb the current solution s with u pair changes*/
        perturbation(s, u);
        
        /*Updates u variable*/
        u++;
        if(u > NUMBER_OBJECTS) u = 2;
        
        /*Improves s locally*/
        twoOptImprovement(s, WORSE_ACCEPTANCE_PROB);

        if(calculate_cost(s, NUMBER_OBJECTS-1) < calculate_cost(best, NUMBER_OBJECTS-1)) best = s;  
        else s = best;
    }   
    
    return best;
}

solution tournament(population& SET_CHROMOSOMES, int k, int& except) {
/*Selects a random individual from the population that is not the except one*/
    int r = generateRandomNumber(0, pop_size-1);
    while(r == except) r = generateRandomNumber(0, pop_size-1);
    
    solution selected(NUMBER_OBJECTS);
    for (int i = 0; i < NUMBER_OBJECTS; i++) selected[i] = SET_CHROMOSOMES[r][i];
    except = r;
    
    /*Runs the tournament k-1 times and selects the best from this tournament*/
    for(int i=1; i < k; i++) {
        r = generateRandomNumber(0, pop_size-1);
        while (r == except) r = generateRandomNumber(0, pop_size-1);
        if (SET_CHROMOSOMES[r] < selected){
            selected = SET_CHROMOSOMES[r];
            except = r;
        }
    }
 
    /*Returns the best permutation*/
    return selected;
}    

void shift_mutation(solution& s, int u){
    vector<int> p1(u);
    vector<int> p2(NUMBER_OBJECTS-u);

    for(int i=0; i < NUMBER_OBJECTS; i++){
        if (i < u) p1[i] = s[i];
        else p2[i-u] = s[i];
    }

    for(int i=0; i < NUMBER_OBJECTS ;i++){
        if(i < (NUMBER_OBJECTS-u)) s[i] = p2[i];
        else s[i] = p1[i-(NUMBER_OBJECTS-u)];
    }
}

/*Crossover of two parents producing one child. Equal positions are kept, while the rest is randomly chosen (the best solution in N*0.2 runs is returned). There is a pc change that crossober occurs, otherwise the best parent is returned*/
solution crossover_opt(solution& p1, solution& p2){
    /*probability of crossover occurring*/
    double pcs = generateDoubleRandomNumber();
    if (pcs > CROSS_P){ /*if crossover does not occur, return the best parent*/

        if(calculate_cost(p1, NUMBER_OBJECTS-1) > calculate_cost(p2, NUMBER_OBJECTS-1)) return p2;
        else return p1;
    }

    /*The new child*/
    vector<int> child(NUMBER_OBJECTS, -1);
    /*Vector indicating wheter a given facility was already placed or not*/
    vector<bool> placed(NUMBER_OBJECTS, false);    

    /*Copy equal positions from parents into child*/
    for(int i=0; i < NUMBER_OBJECTS; i++){
        if (p1[i] == p2[i]){
            child[i] = p1[i];
            placed[p1[i]] = true;
        }
    }
    
    /*Save the equal positions stored*/
    vector<int> childm = child;
    vector<bool> placedm = placed;
    solution best = solution(NUMBER_OBJECTS, -1);
    /*Tries N*0.2 times*/
    int lim = NUMBER_OBJECTS*0.2;
    
    for(int m=0; m < lim; m++) {
        child = childm;
        placed = placedm;

        /*Assign the rest of the positions according to parents or randomly*/
        for(int i=0; i < NUMBER_OBJECTS; i++){
            if(child[i] == -1){
                if(not placed[p1[i]] and !placed[p2[i]]){
                    /*If none of the parents position was already placed, choose probabilistically*/
                    double p = generateDoubleRandomNumber();
                    if (p < 0.5) child[i] = p1[i];
                    else child[i] = p2[i];
                } else if(not placed[p1[i]]){
                    child[i] = p1[i];
                } else if(not placed[p2[i]]){
                    child[i] = p2[i];
                } else{
                    /*If both positions were placed, choose another randomly*/
                    int r = generateRandomNumber(0, NUMBER_OBJECTS-1);
                    while(placed[r]) r = generateRandomNumber(0, NUMBER_OBJECTS-1);
                    child[i] = r;
                }
            
                /*Mark this place as used*/
                placed[child[i]] = true;
            }
        }
        if(best[0] == -1){
            best = child;
        }else if(calculate_cost((best), NUMBER_OBJECTS-1) > calculate_cost(child, NUMBER_OBJECTS-1)){
            best = child;
        }
        
    }
    return best;
}         


int main() {
	srand(time(NULL));
	vector<string> benchmarck;   //NEW
	listing(benchmarck);		//NEW
	for (int ind = 0; ind < benchmarck.size(); ind++) {
		bool print_data = false;
		std::stringstream ind_outp; 
		ind_outp << benchmarck[ind] << ".result";  //output file will have .result extension
		ofstream fout(ind_outp.str().c_str());
		auto start = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed;
	    inputData(DISTANCE, FLOW, NUMBER_OBJECTS, benchmarck[ind]);  
	    solution SOL(NUMBER_OBJECTS);
	    pop_size = 3; 
	    population SET_CHROMOSOMES = population (pop_size, solution(NUMBER_OBJECTS));
	    do {
		    for (int i = 0; i < pop_size; i++) generateRandomSolution(SET_CHROMOSOMES[i], NUMBER_OBJECTS);
		    for (int i = 0; i < pop_size; i++) twoOptImprovement(SET_CHROMOSOMES[i], WORSE_ACCEPTANCE_PROB);
		    sort(SET_CHROMOSOMES.begin(), SET_CHROMOSOMES.end(), comp);
		    /*Selects the best solution*/  
		    solution best_solution(NUMBER_OBJECTS);
		    for (int i = 0; i < NUMBER_OBJECTS; i++) best_solution[i] = SET_CHROMOSOMES[0][i];
		    SOL = best_solution;
		    int iter_best = 0;
		    int u = 1, last_mutation = 0, mutation_type = 1, generation = 0;

		    auto start_10secs = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> elapsed_10secs;
		    /* Main Loop */
		    while (generation < MAX_GENERATIONS) {
		    	if (print_data) {start_10secs = std::chrono::high_resolution_clock::now(); print_data = false; fout << COST << endl;}
		        generation++;
		        vector<solution> offspring; //descendence of original chromosomes
		        while(offspring.size() < pop_size*0.5){
		            int except = -1;
		            /*Tournament to select parents*/
		            solution p1 = tournament(SET_CHROMOSOMES, MAX_GENERATIONS, except);
		            solution p2 = tournament(SET_CHROMOSOMES, MAX_GENERATIONS, except);

		            /*crossover of parents*/
		            solution c = crossover_opt(p1, p2);
		            
		            /*Run a limited local search on the new child*/
		            c = limited_iterated_search(c);   

		            offspring.push_back(c);
		        }

		        /*Sorts offspring in increasing order*/
		        sort(offspring.begin(), offspring.end(), comp);
		        /*Inserts the best individuals from offspring in the population as long as they are better than the latter*/
		        for(int i=0; i < offspring.size(); i++){
		            if(offspring[i] < SET_CHROMOSOMES[pop_size-1-i])
		                SET_CHROMOSOMES[pop_size-1-i] = offspring[i];
		        }
		        
		        /*Sorts population in increasing order*/
		        sort(SET_CHROMOSOMES.begin(), SET_CHROMOSOMES.end(), comp);
		        
		        /*In case population has converged*/
		        if(calculate_cost(SET_CHROMOSOMES[0], NUMBER_OBJECTS-1) >= calculate_cost(best_solution, NUMBER_OBJECTS-1) && generation - last_mutation > 100 && generation - iter_best > 200 && mutation_type==1){ 
		            /*First try the shift mutation approach*/
		            for(int i=0; i < pop_size; i++){
		                shift_mutation(SET_CHROMOSOMES[i], u);
		            }

		            u++;
		            if(u > NUMBER_OBJECTS){
		                /*In case the shift mutation has not done the job, go to the second type*/
		                u=1;
		                mutation_type=2;
		            }

		            sort(SET_CHROMOSOMES.begin(), SET_CHROMOSOMES.end(), comp);
		            last_mutation = generation;
		        }else if(calculate_cost(SET_CHROMOSOMES[0], NUMBER_OBJECTS-1) >= calculate_cost(best_solution, NUMBER_OBJECTS-1) and generation - last_mutation > 100 && generation - iter_best > 200 && mutation_type==2){
		            
		            for(int i=0; i < pop_size; i++){
		                for(int j=0; j < NUMBER_OBJECTS*0.2; j++){
		                    int r = generateRandomNumber(0, NUMBER_OBJECTS-1);
		                    int s = generateRandomNumber(0, NUMBER_OBJECTS-1);
		                    while(r == s) r = generateRandomNumber(0, NUMBER_OBJECTS-1);

		                    exchange(SET_CHROMOSOMES[i], r, s);
		                }
		                
		                /*Improves the solutions locally*/
		                twoOptImprovement(SET_CHROMOSOMES[i], WORSE_ACCEPTANCE_PROB);
		            }
		            
		            sort(SET_CHROMOSOMES.begin(), SET_CHROMOSOMES.end(), comp);
		            last_mutation = generation;
		            mutation_type = 1;
		        }
		        
		        /*If the new best solution is better than the current one*/
		        if(calculate_cost(SET_CHROMOSOMES[0], NUMBER_OBJECTS-1) < calculate_cost(SOL, NUMBER_OBJECTS-1)){
		            /*Update the current best solution*/
		            SOL = SET_CHROMOSOMES[0];
		            COST = calculate_cost(SOL, NUMBER_OBJECTS-1);
		            iter_best = generation;
		            u = 1;
		            /* Print results */ 
		        }
				auto finish_10secs = std::chrono::high_resolution_clock::now();
				elapsed_10secs = finish_10secs - start_10secs; 
		        if (elapsed_10secs.count() >= 10) {print_data = true;}
		    }
		    auto finish = std::chrono::high_resolution_clock::now();
		    elapsed = finish - start;
		} while (elapsed.count() <=  600); //Mesurem 10 minuts  
		fout << endl << "Best solution found is: ";
		for (int i = 0; i < NUMBER_OBJECTS; i++) fout << SOL[i] << ' ';
		fout << endl << "With cost: " << COST << endl;    
    }
}   
