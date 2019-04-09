#include "../qclass.hh"
#include "../input_output.hh"
#include "../random.hh"

using namespace std;

/* Probability of accepting a worse solution in localImprovement */
const double WORSE_ACCEPTANCE_PROB = 0.0;
/* Number of Generations */
const int GENERATIONS = 1000000;
/* Number of local Improvements to perform */
const int LOCAL_IMPROVEMENT = 1000;
/* Size of the population */
const int POPULATION = 5;
/* If the global best does not change for 10 generations, the population is shuffled */
const int SHUFFLE_TOLERANCE = 10;
/* Number of exchanges to perform in a shuffle */
const int NUMBER_SHUFFLES = 15;


/* Does array contain the passed in value? Used while generating initial population */
bool contains(int * arr, int value)
{
	for(int i=0; i<NUMBER_OBJECTS; i++)
   {
   	if(arr[i] == value)
      {
      	return true;
      }
   }

   return false;
}

/* Generate random initial population */
vector<QNode*> * generateRandomPopulation(int pop)
{
	vector<QNode*> * population = new vector<QNode*>();
   population->reserve(pop);
   for(int i=0; i<pop; i++)
   {
   	int * assign = new int[NUMBER_OBJECTS];
      for(int n=0; n<NUMBER_OBJECTS; n++)
      {
      	assign[n] = -1;
      }
      
      for(int n=0; n<NUMBER_OBJECTS; n++)
      {
      	int rand = generateRandomNumber(0, NUMBER_OBJECTS-1);
         while(contains(assign, rand))
         {
         	rand = generateRandomNumber(0, NUMBER_OBJECTS-1);
         }

         assign[n] = rand;
      }

      QNode *node = new QNode(assign);
      delete [] assign;
      population->push_back(node);
   }

   return population;
}

/* Randomly shuffle the population */
void shufflePopulation(vector<QNode*> * population)
{
 	for(int i=0; i<POPULATION; i++)
   {
   	QNode * node = population->at(i);
      for(int j=0; j<NUMBER_SHUFFLES; j++)
      {
      	int rand1 = generateRandomNumber(0, NUMBER_OBJECTS-1);
	      int rand2 = generateRandomNumber(0, NUMBER_OBJECTS-1);
   	   while(rand1 == rand2)
      	{
				rand1 = generateRandomNumber(0, NUMBER_OBJECTS-1);
		      rand2 = generateRandomNumber(0, NUMBER_OBJECTS-1);
      	}

         node->exchange(rand1, rand2);
      }

		node->calculateValue();
   }
}

/* Functor used by std::sort */
struct DescendingSort
{
	bool operator () (QNode * start, QNode* end)
   {
   	return start->getValue() < end->getValue();
   }
};

/* Quadratic 2-OPT improvement */
void twoOptImprovement(QNode * node)
{
 	 for(int i=0; i<NUMBER_OBJECTS;i++)
    {
      for(int j=i+1;j<NUMBER_OBJECTS; j++)
      {
      	int value = node->getValue();
      	node->exchange(i, j);
	 	 	int newValue = node->getValue();
	      if(newValue > value)
   	   {
      		double rand = generateDoubleRandomNumber();
         	if(rand > WORSE_ACCEPTANCE_PROB)
	         {
		      	node->exchange(i, j);
         	}
         }
      }
    }
}

/* Randomized 3-OPT improvement */
void threeOptImprovement(QNode * node)
{
    for(int i=0; i<LOCAL_IMPROVEMENT; i++) {
      int value = node->getValue();
      int rand1 = generateRandomNumber(0, NUMBER_OBJECTS-1);
      int rand2 = generateRandomNumber(0, NUMBER_OBJECTS-1);
      int rand3 = generateRandomNumber(0, NUMBER_OBJECTS-1);
      while(rand1 == rand2 || rand2 == rand3 || rand1 == rand3)
      {
	rand1 = generateRandomNumber(0, NUMBER_OBJECTS-1);
      	rand2 = generateRandomNumber(0, NUMBER_OBJECTS-1);
	rand3 = generateRandomNumber(0, NUMBER_OBJECTS-1);
      }

      node->exchange(rand1, rand2);
      node->exchange(rand2, rand3);

      //node->calculateValue();
      int newValue = node->getValue();
      if(newValue > value)
      {
      	double rand = generateDoubleRandomNumber();
         if(rand > WORSE_ACCEPTANCE_PROB)
         {
            node->exchange(rand2, rand3);
	      	node->exchange(rand1, rand2);
            //node->setValue(value);
         }
      }
    }
}

/* Randomized 4-OPT improvement */
void fourOptImprovement(QNode * node)
{
	 for(int i=0; i<LOCAL_IMPROVEMENT; i++)
    {
      int value = node->getValue();
      int rand1 = generateRandomNumber(0, NUMBER_OBJECTS-1);
      int rand2 = generateRandomNumber(0, NUMBER_OBJECTS-1);
      int rand3 = generateRandomNumber(0, NUMBER_OBJECTS-1);
      int rand4 = generateRandomNumber(0, NUMBER_OBJECTS-1);

      while(rand1 == rand2 || rand1 == rand3 || rand1 == rand4 || rand2 == rand3 || rand2 == rand4 || rand3 == rand4)
      {
	rand1 = generateRandomNumber(0, NUMBER_OBJECTS-1);
      	rand2 = generateRandomNumber(0, NUMBER_OBJECTS-1);
	rand3 = generateRandomNumber(0, NUMBER_OBJECTS-1);
        rand4 = generateRandomNumber(0, NUMBER_OBJECTS-1);
      }

      node->exchange(rand1, rand2);
      node->exchange(rand2, rand3);
      node->exchange(rand3, rand4);

      //node->calculateValue();
      int newValue = node->getValue();
      if(newValue > value)
      {
      	double rand = generateDoubleRandomNumber();
         if(rand > WORSE_ACCEPTANCE_PROB)
         {
         	node->exchange(rand3, rand4);
            node->exchange(rand2, rand3);
	      	node->exchange(rand1, rand2);
				//node->setValue(value);
         }
      }
    }
}

/* Generic function for local improvement.
 	Calls 2,3 and 4 OPT improvement functions
*/
void localImprovement(QNode * node)
{
	twoOptImprovement(node);
   //threeOptImprovement(node);
   //fourOptImprovement(node);
}

/* Main */
int main(void)
{
   /* Initialize the random number generator */
   srand(time(NULL));

   /* Read the number of locations, flow and distance matrices from a file */
   inputData(DISTANCE, FLOW, NUMBER_OBJECTS);

   /* Generate random initial population */
   vector<QNode*> * population = generateRandomPopulation(POPULATION);

   /* Sort the population and find the global best individual */
   sort(population->begin(), population->end(), DescendingSort());
   QNode * gBest = population->at(0)->clone();
   int value = gBest->getValue();

   int count = 0;
   for(int i=0; i<GENERATIONS; i++)
   {
      sort(population->begin(), population->end(), DescendingSort());
      QNode * newBest = population->at(0);
      int newValue = newBest->getValue();

      if(newBest->getValue() < gBest->getValue())
      {
      	delete gBest;
         gBest = newBest->clone();
         count = 0;
      }
      else
      {
      	count++;
         if(count > SHUFFLE_TOLERANCE)
         {
         	shufflePopulation(population);
            count = 0;
         }
      }

      printf("\n%d : %d", i, gBest->getValue());
    	for(int j=0; j<POPULATION; j++)
      {
      	QNode * node = population->at(j);
         localImprovement(node);
      }
   }

   /* Print the final assignments */
   int * assignments = gBest->getAssignments();
   printf("\n\n");
   for(int i=0; i<NUMBER_OBJECTS; i++)
   {
   	printf("%d:", assignments[i]+1);
   }

   for(int i=0; i<POPULATION; i++)
   {
   	QNode * node = population->at(i);
      delete node;
   }

   delete population;

   for(int i=0; i<NUMBER_OBJECTS; i++)
   {
   	delete [] DISTANCE[i];
      delete [] FLOW[i];
   }

   delete [] DISTANCE;
   delete [] FLOW;

   return 0;
}   
