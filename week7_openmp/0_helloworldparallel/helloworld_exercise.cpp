/* ==================================================
   Exersice. Hello World. OpenMP
   Authors: I.Kulakov; M.Zyzak
   ================================================== */


// g++ -O3 -fopenmp -std=c++14 -Wall -Wextra /home/afr/SS17/hpc-praktikum/week7_openmp/0_helloworldparallel/helloworld_exercise.cpp -o helloworld_exercise && ./helloworld_exercise

// 1. Parallelize the program with OpenMP, create 10 threads
// 2. Synchronize threads using omp critical. Compare the results with and without sinchronization
// 3. Get the id of a current thread and print it out

#include <omp.h>
#include <assert.h>
#include <iostream>

using namespace std;

int main_original(const int numThreads) {
  // not synch at all
  omp_set_num_threads(numThreads);
  int id = 0;
  #pragma omp parallel
  {
    cout << " Hello world " << id << endl;
    id++;
  }
  return 0;
}

int main_option1(const int numThreads) {
  // important: this does NOT guarantee synch., just atomic execution of block
  omp_set_num_threads(numThreads);
  int id = 0;
  #pragma omp parallel
  {
    #pragma omp critical
    {
    cout << " Hello world " << id << endl;
    id++;
   }
  }
  return 0;
}



int main_option2(const int numThreads) {
  // important: this does NOT guarantee synch., just atomic execution of block
  // different syntax as option1
  int id = 0;
  #pragma omp parallel num_threads(numThreads)
  {
    #pragma omp critical // important: this does NOT guarantee synch., just atomic
    {
    cout << " Hello world " << id << endl;
    id++;
    }
  }
  return 0;
}


int main_option3(const int numThreads) {
  // this version is atomic AND synched
  #pragma omp parallel num_threads(numThreads)
  {
    int id = omp_get_thread_num();
    #pragma omp for ordered schedule(static,1)
    for( int t=0; t<omp_get_num_threads(); ++t){
      assert( t==id );
      #pragma omp ordered
      {
        cout << " Hello world " << id << endl;
      }
    }
  }
  return 0;
}


int main(){
  const int OPTION = 3;
  const int NUM_THREADS = 100;
  switch(OPTION){
    case 0 : {
      main_original(NUM_THREADS);
      break;
    }
    case 1 : {
      main_option1(NUM_THREADS);
      break;
    }
    case 2 : {
      main_option2(NUM_THREADS);
      break;
    }
    case 3 : {
      main_option3(NUM_THREADS);
      break;
    }
  }  
}
