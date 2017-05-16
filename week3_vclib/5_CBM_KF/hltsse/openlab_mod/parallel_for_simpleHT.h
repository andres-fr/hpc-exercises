#ifndef PARALLEL_FOR_SIMPLEHT_H
#define PARALLEL_FOR_SIMPLEHT_H

#include "tbb/task.h"
#include "tbb/blocked_range.h"

namespace pfsht_internal {
  
  template<typename Body> class TParForTask; // Task for one portion

     //! Task for make portions and run it in parallel.
  template<typename Body>
  class TRootParForTask: public task
  {
   public:
    TRootParForTask(){};
  
//     TRootParForTask(const int nTasks, const int nPortions, const Body& body, float speedUpHT):
//         fNTasks(nTasks),fNPortions(nPortions),fBody(body),fSpeedUpHT(speedUpHT)
//         {  // case of use HT in last turn
//             // init cores usage. 
//           if (nPortions-8 > 0){
//             int nHT = nPortions-8;
//             for (int i = 0; i < nHT; i++){
//               fCoresUsage[i] = (fSpeedUpHT)/2;
//               fCoresUsage[i+8] = (fSpeedUpHT)/2;
//             };
//             for (int i = nHT; i < 8; i++){
//               fCoresUsage[i] = 1;
//               fCoresUsage[i+8] = 0;
//             };
//           }
//           else{
//             for (int i = 0; i < nPortions; i++){
//               fCoresUsage[i] = 1;
//             };
//             for (int i = nPortions; i < 16; i++){
//               fCoresUsage[i] = 0;
//             };
//           }
//             // normalize it
//           float kNormal = 0;
//           for (int i = 0; i < 16; i++){
//             kNormal+=fCoresUsage[i];
//           };
//           for (int i = 0; i < 16; i++){
//             fCoresUsage[i]/=kNormal;
//           };
// #ifdef DEBUG_THREADS
//           cout << " fCoresUsage =  ";
//           for (int i = 0; i < 16; i++){
//             cout << fCoresUsage[i] << " ";// check
//           }
//           cout << endl;
// #endif
//         };
    
        TRootParForTask(const int nTasks, const int nPortions, const Body& body, float speedUpHT):
            fNTasks(nTasks),fNPortions(nPortions),fBody(body),fSpeedUpHT(speedUpHT)
            {   // case of run HT on each core consecutive
            // init cores usage. 
              for (int i = 0; i < 16; i++){
                fCoresUsage[i] = 0;
              };
              for (int i = 0; i < nPortions/2; i++){
                fCoresUsage[i] = (fSpeedUpHT)/2;
                fCoresUsage[i+8] = (fSpeedUpHT)/2;
              };
              
              if (nPortions%2){
                fCoresUsage[nPortions/2] = 1;
              }
            // normalize it
              float kNormal = 0;
              for (int i = 0; i < 16; i++){
                kNormal+=fCoresUsage[i];
              };
              for (int i = 0; i < 16; i++){
                fCoresUsage[i]/=kNormal;
              };
#ifdef DEBUG_THREADS
              cout << " fCoresUsage =  ";
              for (int i = 0; i < 16; i++){
                cout << fCoresUsage[i] << " ";// check
              }
              cout << endl;
#endif
            };
  
    task* execute();
  
   private:
    typedef TParForTask<Body> TpfTask;
    const int fNPortions;
    const int fNTasks;
    Body fBody;
    float fSpeedUpHT;
    float fCoresUsage[16];
  }; // class TRootParForTask
  

  template<typename Body>
  task* TRootParForTask<Body>::execute()
  {
      // divide range
    int portSize[16];
    int cpuId[16];
    int rest = fNTasks;
    for (int i = 0; i < 16; i++){
      portSize[i] = int(float(fNTasks)*fCoresUsage[i]);
      rest-=portSize[i];
      cpuId[i] = 15-i;
    };
    for (int i = 0; rest > 0; i++){
      if (portSize[i] > 0){ portSize[i]++; rest--; }
    };
//     for (int i = 0; i > rest; i--){
//       portSize[0-i]--;
//     };

#ifdef DEBUG_THREADS
    cout << " portSize =  " << endl;
    for (int i = 0; i < 16; i++){
      cout << portSize[i] << endl;
    };
#endif
    {
        // check rest
      rest = fNTasks;
      for (int i = 0; i < 16; i++){
        rest-=portSize[i];
      };
      if (rest != 0) cout << "Portion error" << endl;
    }
      // create tasks
    int currentTask = 0;
    TpfTask *pfTasksArray[fNPortions];
    int iTask;
    for (int i = 0, iTask = 0; i < 16; i++){
      if (portSize[i] > 0){
        TpfTask *a = new( allocate_child() ) TpfTask(currentTask,currentTask+portSize[i]-1,fBody,cpuId[i]);
        currentTask = currentTask+portSize[i];
        pfTasksArray[iTask] = a;
        iTask++;
//         cout << iTask << " " << portSize[i] << "    "  << cpuId[i];
      }
    } 


//     cout << nPortions1 << " portions by " << portSize1 << " elements in portion and "<< nPortions2 << " portions by " << portSize2 << " elements in portion was created." << endl;
    
    
    set_ref_count(fNPortions+1);
    
      // run tasks
    for (int i = 0; i < fNPortions; i++){
      spawn(*pfTasksArray[i]);
    };
//     { // last iteration
//       int i = fNPortions -1;
//       TpfTask *a = new( allocate_child() ) TpfTask(currentTask,currentTask+portSize[i]-1,fBody/*,cpuId[i]*/);
//       currentTask = currentTask+portSize[i];
// //       TpfTask *a = new( allocate_child() ) TpfTask(currentTask,currentTask+625-1,fBody,15-i);
// //       currentTask = currentTask+625;
//       pfTasksArray[i] = a;
//       
//       spawn_and_wait_for_all(*pfTasksArray[i]);
//     }
    
    if (currentTask != fNTasks) cout << "pfs Error." << endl;
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
    
    TParForTask(const int firstTask, const int endTask, const Body& body, int cpuId):
        fFirstTask(firstTask),fEndTask(endTask),fBody(&body),fCpuId(cpuId){};
  
    task* execute();
  
   private:
    int fFirstTask;
    int fEndTask;
    const Body* fBody;
    int fCpuId;
  }; // class TParForTask
  
  template<typename Body>
  task* TParForTask<Body>::execute()
  {   
    

    if(1){ // set cpu here
      int cpuId = fCpuId;
      cpu_set_t cpuset;
      pthread_t thread = pthread_self();
      CPU_ZERO(&cpuset);
      CPU_SET(cpuId, &cpuset); 
    
      int s = pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
      if (s != 0){ cout << " pthread_setaffinity_np " << endl; handle_error_en(s, "pthread_setaffinity_np");}
#ifdef DEBUG_THREADS
      TMyMutex::scoped_lock lock;
      lock.acquire(mutex);
      cout << "ThrId=" << thread << " thread have been created.";
      cout << " And was run on cpu " << cpuId << endl;
      lock.release();
#endif // DEBUG_THREADS
    } 
    
    blocked_range<int> my_range(fFirstTask, fEndTask+1); // let to use same with parallel_for Body class.
    (*fBody)( my_range);
    
    return NULL;
  };

} // namespace pfsht_internal

  //! Parallel iteration over range 0-nTasks
template<typename Body>
int parallel_for_simpleHT(const int nTasks, const int nPortions, const Body& body, float speedUpHT)
{
  pfsht_internal::TRootParForTask<Body>& a = *new (task::allocate_root()) pfsht_internal::TRootParForTask<Body>(nTasks, nPortions, body, speedUpHT);
  task::spawn_root_and_wait(a);

  return 1;
}


#endif // PARALLEL_FOR_SIMPLEHT_H