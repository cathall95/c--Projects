#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "utils.h"
#include <sys/time.h>	/* Include clock time */

#define NUM_THREADS 2

/* typedef command defines a new data type. 
   Below it creates a 'struct': think of it as a class
   without methods and only public data members. 

   As a result of this typedef, so_t is a new data type
   which referes to the struct as defined below. 

   Note: since the thread functions can only have a single argument, 
   we use a struct to encapsulate a bunch of different things and 
   pass it on to the thread together. 
*/

typedef struct sharedobject 
{
 float** d_matrix; 	/* shared data   */
 int sh_rct;   		/* shared row count */
 int sh_cct;   		/* shared col count */
 float sh_ref; 		/* shared */
 float sh_tol;
 int verbose;  		/* shared flag */
} 

so_t;

so_t* shared_data; 
 /* Different threads can read the thread global data from this variable. 
  Since the threads only "read" from it, do you think access to shared_data
  should be synchorised with mutex locks?" */


/* See below an example of how to share writable data between multiple threads 
   that require synchronised access with "mutex locks". Tailor the example to 
   this problem.
   https://computing.llnl.gov/tutorials/pthreads/#MutexOverview  */

/* create a mutex lock variable */ 
pthread_mutex_t mutexsum;

/* Below, define a global variable to maintain the count approximate matches. This variable 
 needs to be accessed by the threads only with a proper lock. */
int sh_count=0;

void *runner(void *param); /* the thread prototype. You have to provide the definition */

/* BEGINNING OF MAIN */
int main(int argc, char **argv)
{
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

  /* Sets clock time */
  struct tm *local;
  time_t start;
  time(&start);
  local = localtime(&start);
  printf("# Start time and date: %s", asctime(local));

  /* Notes number of milliseconds between now and 1970 and convert it to seconds */
  struct timeval milliseconds;
  gettimeofday(&milliseconds, NULL); 	// read and record clock
  double t1=milliseconds.tv_sec+(milliseconds.tv_usec/1000000.0);

  /* Convert Strings to Floats */ 
  float ref=strtof(refStr, NULL);
  float tol=strtof(tolStr, NULL);
  /*scan in the number of rows and columns*/
  int rct, cct;
  scanf("%d", &rct);
  scanf("%d", &cct);

  shared_data = malloc(sizeof(so_t));   /* allocate memory for shared data */
  
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

  /* load all the input data into shared_data */
  shared_data->sh_rct = rct;
  shared_data->sh_cct = cct;
  shared_data->d_matrix = sh_rows;
  shared_data->sh_ref = ref;
  shared_data->sh_tol = tol;
  shared_data->verbose = verbose;

  /* multi-threading begins */
  pthread_t tid[NUM_THREADS];   	/* an array to hold the thread IDs */
  int t_index[NUM_THREADS] = {0, 1};  	/* an array to pass an index to each thread indicating the thread number.
                                 Use it to determine which thread scans which part of input data rows. */

  llist* plist=NULL;			/* to hold linked list to print out if -v option is used*/

  /* Add code to initialise lock. Again, look at the link given above if you are unsure. */
  pthread_mutex_init(&mutexsum, NULL);

  /* Create threads */
  int ti=0;
  for (ti = 0;ti < NUM_THREADS; ti++)
  {
      /* Does the code below pass on the correct thread index?. Check it and fix it if necessary. */
      pthread_create(&tid[ti],NULL,runner,&t_index[ti]);
  } 

  for (ti = 0; ti < NUM_THREADS; ti++)
  {
        
        /* When the threads exit, they MUST return a list of matches
           found. This list can be NULL if verbose option is NOT specified.
           Otherwise, it is a linked list of matching items, where each item is of 
           the form: row index, col index, value.

           
           Print the list ONLY if verbose (-v) is specified. HINT: look at utils.c and linkedlist.c.
           Where to put list printing code? Before or after pthread_join? */
           
           pthread_join(tid[ti], (void **) &plist);                     
	   if(verbose)
	   {
	     print_list(plist);
	   }
  }
  /*print the total count of matches*/
  fprintf(stdout, "Found %d approximate matches.\n", sh_count);


  /* Add code to correctly print out elapsed seconds */ 
  gettimeofday(&milliseconds, NULL);
  double t2=milliseconds.tv_sec+(milliseconds.tv_usec/1000000.0);
  fprintf(stdout, "Elapsed time: %.6f(s)\n", t2-t1);

  /* time to destroy lock. See the link given above for example */
  int pthread_mutex_destroy(pthread_mutex_t *mutexsum);

  /* free all memory you created i.e. everything that your code malloc_ed */
  free(sh_rows);
  free(shared_data);
  plist = clear_list(plist);
  exit(0);
}

void *runner(void *param)
{
  int tid = *((int *) param);

  /* Based on the thread index passed, decide which data rows 
    to scan in this thread. That is, decide the start-row 
    and end-row for each thread. 
    
    NOTE: Consider the possibility that number of rows is ODD. 
    Try to balance load between all the threads but if exact load 
    balancing is not possible, aim for maximum balance possible.
   */

  int s_row = 0;
  int e_row = 0;
  if(tid==0)
  {
    //First thread so s_row should be 0
    e_row = shared_data->sh_rct/2;
  }

  else
  {
    s_row =  shared_data->sh_rct/2;
    e_row = shared_data->sh_rct;
  }

  int r=0,c=0;
  float** sh_rows = shared_data->d_matrix;
  float ref = shared_data->sh_ref;
  float tol = shared_data->sh_tol;
  int verbose = shared_data->verbose;
  llist* print_list=NULL;  
  int equals = 0;
  /*check if each position of the array is a match*/
  for (r = s_row; r < e_row; r++)
    for (c = 0; c < shared_data->sh_cct; c++)
      {
	 equals = (approxEqual(sh_rows[r][c], ref, tol) == 1);
	 if(equals)
	 {
	   pthread_mutex_lock(&mutexsum);
	   sh_count++;	   
	   pthread_mutex_unlock(&mutexsum);
    /*if verbose is true add the match to a list that will be printed out*/
	   if(verbose)
 	   {
	     print_list = add_list(print_list, r, c, sh_rows[r][c]);
	   }
	 }
	/* Check if sh_rows{r][c] matches the given test. 

	   If yes, add code to increment count in a "thread safe" 
           manner i.e. avoid race conditions. 

	   Also, if -v option is given, add the matching record to 
           plist. */
      }

  pthread_exit((void*)print_list);  /* Modify code to return the list back to the calling thread */

}
