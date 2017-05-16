#ifndef PARALLEL_FOR_SIMPLE_H
#define PARALLEL_FOR_SIMPLE_H

#include "tbb/task.h"
#include "tbb/blocked_range.h"

namespace pfs_internal {
  
  template<typename Body> class TParForTask; // Task for one portion

     //! Task for make portions and run it in parallel.
  template<typename Body>
  class TRootParForTask: public task
  {
   public:
    TRootParForTask(){};
  
    TRootParForTask(const int nTasks, const int nPortions, const int nThreads, const Body& body):
        fNTasks(nTasks),fNPortions(nPortions),fNThreads(nThreads),fBody(body){};
  
    task* execute();
  
   private:
    typedef TParForTask<Body> TpfTask;
    const int fNPortions;
    const int fNThreads;
    const int fNTasks;
    Body fBody;
  }; // class TRootParForTask
  

  template<typename Body>
  task* TRootParForTask<Body>::execute()
  {
      // divide range
    const int portSize1 = fNTasks / fNPortions;
    const int portSize2 = portSize1+1;
    const int nPortions2 = fNTasks - portSize1*fNPortions; // portions with size portSize2
    const int nPortions1 = fNPortions - nPortions2; // portions with size portSize1
        
      // create tasks
    int currentTask = 0;
    TpfTask *pfTasksArray[fNPortions];
    for (int i = 0; i < nPortions1; i++){
      TpfTask *a = new( allocate_child() ) TpfTask(currentTask,currentTask+portSize1-1,fBody);
      currentTask = currentTask+portSize1;
      pfTasksArray[i] = a;
    }  // nPortions1
    for (int i = nPortions1; i < fNPortions; i++){
      TpfTask *a = new( allocate_child() ) TpfTask(currentTask,currentTask+portSize2-1,fBody);
      currentTask = currentTask+portSize2;
      pfTasksArray[i] = a;
    }  // nPortions2
    if (currentTask != fNTasks) cout << "pfs Error." << endl;
//     cout << nPortions1 << " portions by " << portSize1 << " elements in portion and "<< nPortions2 << " portions by " << portSize2 << " elements in portion was created." << endl;
    
    
    set_ref_count(fNPortions+1);
    
      // run tasks
//     for (int i = 0; i < fNPortions; i++){
//       spawn(*pfTasksArray[i]);
//     };
//     const float delTime = 1*2e4*0.025e-6*fNThreads/fNPortions;
    for (int i = 0; i < fNPortions/fNThreads; i++){
      for (int j = 0; j < (fNThreads && (fNThreads*i+j < fNPortions)); j++){
        spawn(*pfTasksArray[fNThreads*i+j]);
      }
//       for (tick_count time1 = tick_count::now(); (tick_count::now()-time1).seconds() < delTime ;);  // delay
//       float tbb_sec = (tick_count::now()-time0).seconds();
//       cout << tbb_sec << endl;
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

} // namespace pfs_internal

  //! Parallel iteration over range 0-nTasks
template<typename Body>
    int parallel_for_simple(const int nTasks, const int nPortions, const int nThreads, const Body& body)
{
  pfs_internal::TRootParForTask<Body>& a = *new (task::allocate_root()) pfs_internal::TRootParForTask<Body>(nTasks, nPortions, nThreads, body);
  task::spawn_root_and_wait(a);

  return 1;
}


#endif // PARALLEL_FOR_SIMPLE_H