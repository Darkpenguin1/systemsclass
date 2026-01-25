/*
 @file: pthreads_skeleton.cpp
 
 @author: Frank Peraza, fperaza@charlotte.edu
 @author: student name2, student2@uncc.edu
 @author: student name3, student3@uncc.edu
 
 @description: A program that demonstrates processes.
 
 @course: ITSC 3146
 @assignment: in-class activity [n]
 */

#include <pthread.h>
#include <iostream>

using namespace std;

// Init the messages array 
const char* my_messages[4] = {"Italian: Ciao!", 
	"English: Hello!", "Hindi: Namaste!", 
	"Spanish: Hola!" }; 


// This function shows the skeleton that pthread 
// functions must adhere to. 
// Copy this skeleton for any pthread function 
// you need to define. 
// In the copy, modify the name of the function 
// and the function body as needed. 
void *routineName(void *arg)
{
   // TODO: Add code that implements
   //       the thread's functionality
   cout << "Thread is running..." << endl;
   return 0;
}

void *printMessage(void *arg)
{
   // TODO: Add code that implements
   //       the thread's functionality
   int *index = (int*)arg; // type casting the argument passed in as an int named index
   cout << endl << my_messages[*index] << endl;              // dereference the index ptr and print the message at the index
   cout << "Thread is running..." << endl;
   return 0;
}

int main()
{
   // id is used to store a unique thread identifier,
   // returned by the call to create a new POSIX thread
   pthread_t id;
   int numThreads = sizeof(my_messages)/sizeof(my_messages[0]); // div the size of the arr by the first element 
   pthread_t threadArray[numThreads];
   
   // rc is used to store the code returned by the
   // call to create a new POSIX thread. The value is
   // zero (0) if the call succeeds.
   int rc;
   
   
   // TODO: Add code to perform any needed initialization
   //       or to process user input


   // Create thread(s)
   // TODO: Modify according to assignment requirements

   for (int i = 0; i < numThreads; i++) {
      int* index = (int*)malloc(sizeof(int));   // since each thread has its own stack but shares the same HEAP memory 
      // we can use the heap to allocate memory for the index ptr so threads dont execute the same index !
      
      *index = i; // dereference value of index equals the current iteration of the loop (i)

      rc = pthread_create(&threadArray[i], NULL, printMessage, (void*)index);

      if (rc){ // if the newly created thread returns anything other than 0 it means there was an error 
      cout << "ERROR; return code from pthread_create() is " << rc << endl;
      return -1;
      }
   }
 
   // NOTE: Using exit here will immediately end execution of all threads
   pthread_exit(0);
}
