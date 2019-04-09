#ifndef QCLASS_HH
#define QCLASS_HH

#include <stdio.h>
#include <algorithm>
#include <vector>
#include <string.h>
#include <time.h> 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>

using namespace std; 

/* Globals */
int ** DISTANCE; 
int ** FLOW; 
int OPTIMUM; 
int NUMBER_OBJECTS; 

/* A QAP solution */
class QNode
{
	private:

   	/* Array of assignments - The index represents the facility and the value of that index
      represents the location at which the facility is assigned */
   	int * assignments;

      /* The objective function value */
      int value;

   public:
	   /* Constructor */
   	QNode()
      {
      	assignments = NULL;
      }

   	/* Destructor */
      ~QNode()
      {
      	if(assignments != NULL)
         	delete [] assignments;

         assignments = NULL;
      }

      /* Constructor */
      QNode (int * as)
      {
   	assignments = new int[NUMBER_OBJECTS];
         for(int i=0; i<NUMBER_OBJECTS; i++)
         {
         	assignments[i] = as[i];
            if(assignments[i] >= NUMBER_OBJECTS)
            {
            	printf("\n\nInvalid Data in Copy Constructor");
               exit(1);
            }
         }
         calculateValue();
      }

      /* Assignment Operator */
      QNode & operator = (const QNode & copy)
      {
      	if(assignments != NULL)
         	delete [] assignments;

         assignments = new int[NUMBER_OBJECTS];
         int * as = copy.getAssignments();
         for(int i=0; i<NUMBER_OBJECTS; i++)
         {
         	assignments[i] = as[i];
         }

         calculateValue();
      }

      /* Operator '<' Used by std::sort */
      bool operator < (QNode* right) 
      {
      	 int av = this -> getValue();
         int bv = right->getValue();
         return av < bv;
      }

      /* Operator '>' */
      bool operator > (QNode* right)     
      {
         int av = this -> getValue();
         int bv = right->getValue();
         return av > bv;
      }

      /* Exchange the assignments at indices i and j */
      void exchange(int i, int j)
      {
      	 calculateValueSwap(i, j);
         int temp = assignments[i];
         assignments[i] = assignments[j];
         assignments[j] = temp;
      }

      /* Calculate the objective function value for this solution */
     void calculateValue() {
      	value = 0;
      	for(int i=0; i<NUMBER_OBJECTS; i++) {
      	    int as = assignments[i];
            for(int j=0; j<NUMBER_OBJECTS; j++)
            {
               int aas = assignments[j];
               int distance = DISTANCE[as][aas];
               int flow = FLOW[i][j];
               int vv = distance*flow;
               value+=vv;
            }
         }
      }

      /* Calculate the changed value when facilities r and s are swapped (O(n)) */
      void calculateValueSwap(int r, int s)
      {
      	 int as = assignments[r];
          int aas = assignments[s];

          int delta = 0;

          delta = FLOW[r][r] * (DISTANCE[aas][aas]-DISTANCE[as][as]) +
          			FLOW[r][s] * (DISTANCE[aas][as]-DISTANCE[as][aas]) +
                  FLOW[s][r] * (DISTANCE[as][aas]-DISTANCE[aas][as])+
                  FLOW[s][s] * (DISTANCE[as][as]-DISTANCE[aas][aas]);

          for(int k=0; k<NUMBER_OBJECTS; k++)
          {
            int ak = assignments[k];
          	if(k != r && k != s)
            {
            	delta = delta + (FLOW[k][r] * (DISTANCE[ak][aas]-DISTANCE[ak][as]) +
               						FLOW[k][s] * (DISTANCE[ak][as]-DISTANCE[ak][aas]) +
                                 FLOW[r][k] * (DISTANCE[aas][ak]-DISTANCE[as][ak]) +
                                 FLOW[s][k] * (DISTANCE[as][ak]-DISTANCE[aas][ak]));
            }
          }

          value+=delta;
      }

      /* Get the assignments of this solution */
      int * getAssignments() const
      {
      	return assignments;
      }

      /* Get the objective function value of this solution */
      int getValue()
      {
      	return value;
      }

      /* Set the objective function value of this solution */
      void setValue(int v)
      {
      	value = v;
      }

		/* Return a deep copied clone of this solution */
      QNode * clone()
      {
      	int * asn = new int[NUMBER_OBJECTS];
         for(int i=0; i<NUMBER_OBJECTS; i++)
         {
         	asn[i] = assignments[i];
         }

         QNode * clone = new QNode(asn);
         delete [] asn;
         return clone;
      }
};


#endif
