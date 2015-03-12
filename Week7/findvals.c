#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

int main(int argc, char **argv)
{
  char *refStr, *tolStr;
  float refFlt, tolFlt;
  //Putting the reference and tolerance into a char
   if(strcmp(argv[1], "-r") == 0) 
	{
   	refStr = argv[2];
   	tolStr = argv[4];
   	}
   else 
	{
   	tolStr = argv[2];
   	refStr = argv[4];
  	}
  //converting the char reference and tolerance to a float
  refFlt = strtof(refStr, NULL);
  tolFlt = strtof(tolStr, NULL);
  //Check if arguments were passed in correctly.
  if (argc != 5 || refStr == NULL || tolStr == NULL)
  	{
    	fprintf(stderr, "Usage: %s -r ref -t tol\n", argv[0]);
    	exit(1);
  	}
  time_t current_time;
  char* c_time_string;
  /*Get current time as seconds elapsed since the Epoch. */
  current_time = time(NULL);
   if (current_time == ((time_t)-1))
    	{
        (void) fprintf(stderr, "Failure to compute the current time.");
        return EXIT_FAILURE;
    	}
    /* Convert to local time format. */
    c_time_string = ctime(&current_time);
    if (c_time_string == NULL)
    	{
        (void) fprintf(stderr, "Failure to convert the current time.");
        return EXIT_FAILURE;
    	}
    /* Print to stdout. */
    (void) printf("# Start time and date: %s", c_time_string);
  int rct, cct;
  //getting the size of the matrix
  scanf("%d", &rct);
  scanf("%d", &cct);
  //allocating space for the array of arrays
  float** rows = (float **) malloc(rct * sizeof(float *));
  if (rows == 0)
  	{
  	fprintf(stderr, "Couldn't alocate sufficient space.\n");
  	exit(1);
  	}
  int i;
  for (i = 0; i < rct; i++)
  	{
  	float* row = (float *) malloc(cct * sizeof(float));
  	if (row == 0)
  		{
  		fprintf(stderr, "Couldn't alocate sufficient row space.\n");
  		exit(1);
  		}
  	rows[i] = row;
  	}
  int r,c;
  //reading in the matrix
  for(r = 0;r < rct;r++)
    	for(c = 0;c < cct;c++)
     		scanf("%f", &rows[r][c]);
  int result,count = 0;
  //going through the matrix checking if the value is within the tolerance
  for(r = 0;r < rct;r++)
    	for(c = 0;c < cct;c++)
   		{
  		result = approxEqual(rows[r][c],refFlt,tolFlt);
  		if(result == 1)
  			{
  			fprintf(stdout, "r=%d, c=%d: %.6f\n", r, c, rows[r][c]);
  			count++;
  			}
		}
  //printing out the total matches
  fprintf(stdout, "Found %d approximate matches.\n", count);
  time_t end_time;
  char* c_timeas_string;
  /*Get current time as seconds elapsed since the Epoch. */
  end_time = time(NULL);
  if (end_time == ((time_t)-1))
    	{
        (void) fprintf(stderr, "Failure to compute the current time.");
        return EXIT_FAILURE;
    	}
    /* Convert to local time format. */
    c_timeas_string = ctime(&end_time);
    if (c_timeas_string == NULL)
    	{
        (void) fprintf(stderr, "Failure to convert the current time.");
        return EXIT_FAILURE;
    	}
    /* Print to stdout. */
    (void) printf("# End time and date: %s", c_timeas_string);
    exit(0);
}
