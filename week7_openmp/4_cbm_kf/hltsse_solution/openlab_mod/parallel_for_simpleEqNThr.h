// for task long don't depend from number of threads -  all task proportional to nThreads

#ifndef PARALLEL_FOR_SIMPLEEQNTHT_H
#define PARALLEL_FOR_SIMPLEEQNTHT_H

#include "tbb/task.h"
#include "tbb/blocked_range.h"

namespace pfsent_internal {
  
  template<typename Body> class TParForTask; // Task for one portion

     //! Task for make portions and run it in parallel.
  template<typename Body>
  class TRootParForTask: public task
  {
   public:
    TRootParForTask(){};
  
    TRootParForTask(const int nTasksByThread, const int nThreads, const Body& body):
        fNTasksByThread(nTasksByThread),fNThreads(nThreads),fBody(body){};
  
    task* execute();
  
   private:
    typedef TParForTask<Body> TpfTask;
    const int fNTasksByThread;
    const int fNThreads;
    Body fBody;
  }; // class TRootParForTask
  

  template<typename Body>
  task* TRootParForTask<Body>::execute()
  {
        
      // create tasks
    int currentTask = 0;
    TpfTask *pfTasksArray[fNThreads];
    for (int i = 0; i < fNThreads; i++){
      TpfTask *a = new( allocate_child() ) TpfTask(currentTask,currentTask+fNTasksByThread-1,fBody);
      currentTask = currentTask+fNTasksByThread;
      pfTasksArray[i] = a;
    } 
    if (currentTask != fNTasksByThread*fNThreads) cout << "pfs Error." << endl;
//     cout << nPortions1 << " portions by " << portSize1 << " elements in portion and "<< nPortions2 << " portions by " << portSize2 << " elements in portion was created." << endl;
    
    
    set_ref_count(fNThreads+1);
    
      // run tasks
    for (int i = 0; i < fNThreads; i++){
      spawn(*pfTasksArray[i]);
    };

    wait_for_all();
    
      // free memory
//     for (int i = 0; i < fNPortions; i++){
//       TpfTask *a = pfTasksArray[i];
// //       cout << a << " 2 " << endl;
//       if (a) delete a;
//     }

    return NULL;
  }; // TRootParForTask<Body>::execute()


    //! Task for one portion
  template<typename Body>
  class TParForTask: public task
  {
   public:
    TParForTask(){};
    
    TParForTask(const int firstTask, const int endTask, const Body& body):
      fFirstTask(firstTask),fEndTask(endTask),fBody(&body){};
  
    task* execute();
  
   private:
    int fFirstTask;
    int fEndTask;
    const Body* fBody;
  }; // class TParForTask
  
  template<typename Body>
  task* TParForTask<Body>::execute()
  {   
    blocked_range<int> my_range(fFirstTask, fEndTask+1); // let to use same with parallel_for Body class.
    (*fBody)( my_range );
    
    return NULL;
  };

} // namespace pfsent_internal

  //! Parallel iteration over range 0-nTasks
template<typename Body>
    int parallel_for_simpleEqNThr(const int nTasksByThread, const int nThreads, const Body& body)
{
  pfsent_internal::TRootParForTask<Body>& a = *new (task::allocate_root()) pfsent_internal::TRootParForTask<Body>(nTasksByThread, nThreads, body);
  task::spawn_root_and_wait(a);

  return 1;
}


#endif // PARALLEL_FOR_SIMPLEEQNTHT_H