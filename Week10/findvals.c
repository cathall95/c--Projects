#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "utils.h"
#include <omp.h>
#include <sys/time.h>	/* Include clock time */

/* typedef command defines a new data type. 
   Below it creates a 'struct': think of it as a class
   without methods and only public data members. 

   As a result of this typedef, so_t is a new data type
   which referes to the struct as defined below. 

   Note: since the thread functions can only have a single argument, 
   we use a struct to encapsulate a bunch of different things and 
   pass it on to the thread together. 
*/
 /* Different threads can read the thread global data from this variable. 
  Since the threads only "read" from it, do you think access to shared_data
  should be synchorised with mutex locks?" */


/* See below an example of how to share writable data between multiple threads 
   that require synchronised access with "mutex locks". Tailor the example to 
   this problem.
   https://computing.llnl.gov/tutorials/pthreads/#MutexOverview  */

/* create a mutex lock variable */ 

/* Below, define a global variable to maintain the count approximate matches. This variable 
 needs to be accessed by the threads only with a proper lock. */
int sh_count=0;
 /* the thread prototype. You have to provide the definition */

/* BEGINNING OF MAIN */
int main(int argc, char **argv)
{
  struct tm *local;
  time_t start;
  time(&start);
  local = localtime(&start);
  printf("# Start time and date: %s", asctime(local));
  char *refStr, *tolStr; 
  int i, j,t;
  int verbose = 0;
  refStr = tolStr = NULL;
  /* 
    Add code to handle command line parameters. Use compact code. 
    Longwinded code with an if block for each combination of parameters
    is inelegant and will be penalised. 
  */

  /* check if -v flag was entered and get the locations of ref and tol*/
  for(t = 0;t<argc;t++)
	{
	if(strcmp(argv[t], "-v") == 0)
		verbose = 1;
			else if(strcmp(argv[t], "-r") == 0)
				refStr = argv[t + 1];
					else if(strcmp(argv[t], "-t") == 0)
						tolStr = argv[t + 1];
	}

  /* Check if arguments were passed in correctly. */
  if ((argc != 5 && argc != 6) || refStr == NULL || tolStr == NULL)
  	{
    	fprintf(stderr, "Usage: %s -r ref -t tol\n", argv[0]);
    	exit(1);
  	}

  /* Add code to notice time and then modify and uncomment the 
     printf statement below to suite your code. */

  /* Convert Strings to Floats */ 
  float ref=strtof(refStr, NULL);
  float tol=strtof(tolStr, NULL);
  /*scan in the number of rows and columns*/
  int rct, cct;
  scanf("%d", &rct);
  scanf("%d", &cct);
  /* malloc the required memory and then write code to read all the data into sh_rows. */
  float** sh_rows = (float **) malloc(rct*sizeof(float*));

  if(sh_rows == 0)
  {
    fprintf(stderr, "Can't allocate enough space.\n");
    exit(1);
  } 

  for(i = 0; i < rct; i++)
  {
    float* row = (float *) malloc(cct * sizeof(float));
    if (row == 0)
    {
      fprintf(stderr, "Couldn't alocate sufficient row space.\n");
      exit(1);
    }
    sh_rows[i] = row;
  }
      
  /*Two Nested for loops that ask for each number and read it into the matrix. */
  for(i = 0; i<rct; i++)
  {
    for(j = 0; j<cct; j++)
    {
       scanf("%f", &sh_rows[i][j]);
    }
  }

  struct timeval milliseconds;
  gettimeofday(&milliseconds, NULL); 	// read and record clock
  double t1=milliseconds.tv_sec+(milliseconds.tv_usec/1000000.0);
  int r,c;
  llist* plist=NULL;
  /*check if each position of the array is a match*/
  #pragma omp parallel for private(r,c)
  for (r = 0; r < rct; r++)
     for (c = 0; c < cct; c++)
      {
	 if((approxEqual(sh_rows[r][c], ref, tol) == 1))
	 {
            if(verbose)
              plist = add_list(plist, r, c, sh_rows[r][c],omp_get_thread_num());
 	   #pragma omp atomic
	   sh_count++;	   
    /*if verbose is true add the match to a list that will be printed out*/
	 }
}
    if(verbose)
	   {
	     print_list(plist);
	   }

  
  /*print the total count of matches*/
  fprintf(stdout, "Found %d approximate matches.\n", sh_count);


  /* Add code to correctly print out elapsed seconds */ 
  gettimeofday(&milliseconds, NULL);
  double t2=milliseconds.tv_sec+(milliseconds.tv_usec/1000000.0);
  fprintf(stdout, "Search time: %.6f(s)\n", t2-t1);

  /* time to destroy lock. See the link given above for example */

  /* free all memory you created i.e. everything that your code malloc_ed */
  free(sh_rows);
  plist = clear_list(plist);
  exit(0);
}

