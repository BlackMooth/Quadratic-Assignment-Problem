/* ========================================================================== */
/*                                                                            */
/*   Input Data Processing						      */						  
/*	 Obs: data is expected to be a number N, and 2 NxN matrices           */
/*   (c++) 2019 Daniel Donate                                                 */
/*                                                                            */
/*   Description: Input and other auxiliary functions                         */
/*                                                                            */
/* ========================================================================== */

#ifndef INPUT_OUTPUT_HH
#define INPUT_OUTPUT_HH

using namespace std;


inline void inputData(int **& DISTANCE, int **& FLOW, int& NUMBER_OBJECTS) {
	FILE * file;
	file = fopen("data.dat", "r");
   if(file == NULL)
   {
   	printf("File Not Found in Current Directory.");
      exit(1);
   }

	char * line = new char [1000];
	fgets(line, 1000, file);
	NUMBER_OBJECTS = atoi(line);
    fgets(line, 1000, file);

	int i = 0;
    char * tok = NULL;

	/* FLOW MATRIX */
	FLOW = new int*[NUMBER_OBJECTS];
	for(int n=0; n<NUMBER_OBJECTS; n++)
	{
		i=0;
		fgets(line, 1000, file);
		FLOW[n] = new int[NUMBER_OBJECTS];
		tok = strtok(line, " \t");
		int vv = atoi(tok);
		FLOW[n][i] = vv;
		i++;
		while(true)
		{
			tok = strtok(NULL, " \t");
			while(tok != NULL && i < NUMBER_OBJECTS)
			{
				vv = atoi(tok);
				FLOW[n][i] = vv;
				tok = strtok(NULL, " \t");
            i++;
            if(tok == NULL)
            {
            	break;
            }
            if(strcmp(tok, " ") == 0 || strcmp(tok, "\n") == 0)
         	{
         		break;
         	}
			}

			if(i < NUMBER_OBJECTS)
			{
				fgets(line, 1000, file);
				tok = strtok(line, " \t");
				int vv = atoi(tok);
				FLOW[n][i] = vv;
				i++;
			}
			else
			{
				break;
			}
		}
	}

   fgets(line, 1000, file);

  	/* DISTANCE MATRIX */
	DISTANCE = new int*[NUMBER_OBJECTS];
	for(int n=0; n < NUMBER_OBJECTS; n++)
	{
		i=0;
		fgets(line, 1000, file);
		DISTANCE[n] = new int[NUMBER_OBJECTS];
		tok = strtok(line, " \t");
		int vv = atoi(tok);
		DISTANCE[n][i] = vv;
		i++;
		while(true)
		{
			tok = strtok(NULL, " \t");
			while(tok != NULL && i < NUMBER_OBJECTS)
			{
				vv = atoi(tok);
				DISTANCE[n][i] = vv;
				tok = strtok(NULL, " \t");
            i++;
            if(tok == NULL)
            {
            	break;
            }
            if(strcmp(tok, " ") == 0 || strcmp(tok, "\n") == 0)
         	{
         		break;
         	}
			}

			if(i < NUMBER_OBJECTS)
			{
				fgets(line, 1000, file);
				tok = strtok(line, " \t");
				int vv = atoi(tok);
				DISTANCE[n][i] = vv;
				i++;
			}
			else
			{
				break;
			}
		}
	}

	delete [] line;
}

#endif


