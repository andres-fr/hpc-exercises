#include <map>
using std::map;

#include "tbb/blocked_range.h"
#include "tbb/task_scheduler_observer.h"
#include "tbb/spin_mutex.h"
#include "tbb/tick_count.h"

using namespace tbb;
typedef spin_mutex TMyMutex;
TMyMutex mutex;
#ifdef DEBUG_THREADS
typedef spin_mutex TMyMutex2;
TMyMutex2 mutex2;
typedef spin_mutex TMyMutex3;
TMyMutex3 mutex3;
#endif // DEBUG_THREADS

int threads_counter = -1; // for count number of current threads. Need for cpu-thread correspondence making.
tick_count time0 = tick_count::now(); // reference time

map<int,long> threadToCpuMap; // let get cpuId by threadId
map<int,int> threadNumberToCpuMap; // let get cpuId by threadNumber (see threads_counter)


class TMyObserver: public  task_scheduler_observer
{
 public:
  void FInit(); // set cpu - thread correspondence
 protected:
  void on_scheduler_entry(bool Is_worker);  // run at begin of each thread execution
  void on_scheduler_exit(bool Is_worker);   // run at end of each thread execution
};

  /// set cpu - thread correspondence
void TMyObserver::FInit()
{
    //  default
  for (int i=0; i<16; i+=1){
    threadNumberToCpuMap[i+0] = 15-i;
  };

  for (int i=0; i<8; i++){
    threadNumberToCpuMap[2*i+0] = i;
    threadNumberToCpuMap[2*i+1] = i+8;
  };
// 
// 
//   for (int i=0; i<16; i+=8){
//     threadNumberToCpuMap[i+0] = 2;
//     threadNumberToCpuMap[i+1] = 3;
//     threadNumberToCpuMap[i+2] = 6;
//     threadNumberToCpuMap[i+3] = 7;
//     threadNumberToCpuMap[i+4] = 10;
//     threadNumberToCpuMap[i+5] = 11;
//     threadNumberToCpuMap[i+6] = 14;
//     threadNumberToCpuMap[i+7] = 15;
//   };
// 
//   for (int i=0; i<16; i+=8){
//     threadNumberToCpuMap[i+0] = 4;
//     threadNumberToCpuMap[i+1] = 5;
//     threadNumberToCpuMap[i+2] = 6;
//     threadNumberToCpuMap[i+3] = 7;
//     threadNumberToCpuMap[i+4] = 12;
//     threadNumberToCpuMap[i+5] = 13;
//     threadNumberToCpuMap[i+6] = 14;
//     threadNumberToCpuMap[i+7] = 15;
//   };
// 
//   for (int i=0; i<16; i+=4){  //70% overhead
//     threadNumberToCpuMap[i+0] = 4;
//     threadNumberToCpuMap[i+1] = 5;
//     threadNumberToCpuMap[i+2] = 6;
//     threadNumberToCpuMap[i+3] = 7;
//   };
// 
//   for (int i=0; i<16; i+=4){  //70% overhead
//     threadNumberToCpuMap[i+0] = 4;
//     threadNumberToCpuMap[i+1] = 5;
//     threadNumberToCpuMap[i+2] = 12;
//     threadNumberToCpuMap[i+3] = 13;
//   };
// 
//   for (int i=0; i<16; i+=4){ //70% overhead
//     threadNumberToCpuMap[i+0] = 4;
//     threadNumberToCpuMap[i+1] = 5;
//     threadNumberToCpuMap[i+2] = 14;
//     threadNumberToCpuMap[i+3] = 15;
//   };
// 
//   for (int i=0; i<16; i+=4){  //70% overhead
//     threadNumberToCpuMap[i+0] = 4;
//     threadNumberToCpuMap[i+1] = 6;
//     threadNumberToCpuMap[i+2] = 13;
//     threadNumberToCpuMap[i+3] = 15;
//   };
// 
//   for (int i=0; i<16; i+=4){ //25% overhead
//     threadNumberToCpuMap[i+0] = 4;
//     threadNumberToCpuMap[i+1] = 5;
//     threadNumberToCpuMap[i+2] = 10;
//     threadNumberToCpuMap[i+3] = 11;
//   };
// 

//   for (int i=0; i<16; i+=2){ //11% overhead
//     threadNumberToCpuMap[i+0] = 4;
//     threadNumberToCpuMap[i+1] = 11;
//   };
// 
//   for (int i=0; i<16; i+=2){ //11% overhead
//     threadNumberToCpuMap[i+0] = 4;
//     threadNumberToCpuMap[i+1] = 3;
//   };
// 
//   for (int i=0; i<16; i+=2){ //10% overhead
//     threadNumberToCpuMap[i+0] = 4;
//     threadNumberToCpuMap[i+1] = 9;
//   };
//   for (int i=0; i<16; i+=2){ //7% overhead
//     threadNumberToCpuMap[i+0] = 4;
//     threadNumberToCpuMap[i+1] = 15;
//   };
// 
//   for (int i=0; i<16; i+=2){ //7% overhead
//     threadNumberToCpuMap[i+0] = 4;
//     threadNumberToCpuMap[i+1] = 7;
//   };
//   for (int i=0; i<16; i+=2){
//     threadNumberToCpuMap[i+0] = 4;
//     threadNumberToCpuMap[i+1] = 12;
//   };
// 
//   for (int i=0; i<16; i+=2){ //7% overhead
//     threadNumberToCpuMap[i+0] = 2;
//     threadNumberToCpuMap[i+1] = 3;
//   };
//   for (int i=0; i<16; i+=2){ //7% overhead
//     threadNumberToCpuMap[i+0] = 2;
//     threadNumberToCpuMap[i+1] = 4;
//   };
} // TMyObserver::FInit()

  /// redefine function, which will be run at begining of execution of each thread
#define handle_error_en(en, msg) do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)
void TMyObserver::on_scheduler_entry(bool Is_worker)
{
  pthread_t I = pthread_self();
  TMyMutex::scoped_lock lock;
  lock.acquire(mutex);
    ++threads_counter;
    int cpuId = threadNumberToCpuMap[threads_counter%16];
//     int cpuId = 4+((threads_counter/4)%2)*8+threads_counter%4;
//     int cpuId = 2+((threads_counter/2)%4)*4+threads_counter%2;
//     int cpuId = 15-threads_counter%8;
#ifdef DEBUG_THREADS
    cout << "ThrId=" << I << " thread have been created " << threads_counter << "-th.";
    cout << " And was run on cpu " << cpuId << endl;
#endif // DEBUG_THREADS
  lock.release();
  threadToCpuMap[I] = cpuId;

  int s, j;
  cpu_set_t cpuset;
  pthread_t thread = I;
  CPU_ZERO(&cpuset);
  CPU_SET(cpuId, &cpuset); 
// cout << "before" << endl; //FIXME: segmentation fault somethere.
  s = pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
    if (s != 0){ cout << " pthread_setaffinity_np " << endl; handle_error_en(s, "pthread_setaffinity_np");}
// cout << "end" << endl;

//   s = pthread_getaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
// //   if (s != 0)  handle_error_en(s, "pthread_getaffinity_np");
// 
// //  printf("Set returned by pthread_getaffinity_np() contained:\n");
//   for (j = 0; j < CPU_SETSIZE; j++)
//       if (CPU_ISSET(j, &cpuset))
//           printf("    CPU %d uses\n ", j);

}; // TMyObserver::on_scheduler_entry(bool Is_worker)

  /// run at end of each thread execution
void TMyObserver::on_scheduler_exit(bool Is_worker)  //FIXME: don't run
{
  pthread_t I = pthread_self();
  cout << "Thread with number " << I << " was ended " << --threads_counter;
}; // TMyObserver::on_scheduler_exit(bool Is_worker)




class ApplyFit{

public:

  TrackV *_track_vectors;
  Station *_vStations;
  int _NStations;
  int _NFits;

  void operator()(const blocked_range<int> &range, int cpuId = -1) const {

// {
//   pthread_t I = pthread_self();
//   TMyMutex::scoped_lock lock;
//   lock.acquire(mutex);
//     ++threads_counter;
//     int cpuId = threadNumberToCpuMap[threads_counter%16];
// //     int cpuId = 4+((threads_counter/4)%2)*8+threads_counter%4;
// //     int cpuId = 2+((threads_counter/2)%4)*4+threads_counter%2;
// //     int cpuId = 15-threads_counter%8;
// #ifdef DEBUG_THREADS
//     cout << "ThrId=" << I << " thread have been created " << threads_counter << "-th.";
//     cout << " And was run on cpu " << cpuId << endl;
// #endif // DEBUG_THREADS
//   lock.release();
//   threadToCpuMap[I] = cpuId;
// 
//   int s, j;
//   cpu_set_t cpuset;
//   pthread_t thread = I;
//   CPU_ZERO(&cpuset);
//   CPU_SET(cpuId, &cpuset); 
// 
//   s = pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
//     if (s != 0){ cout << " pthread_setaffinity_np " << endl; handle_error_en(s, "pthread_setaffinity_np");}
// 
// //   s = pthread_getaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
// // //   if (s != 0)  handle_error_en(s, "pthread_getaffinity_np");
// // 
// // //  printf("Set returned by pthread_getaffinity_np() contained:\n");
// //   for (j = 0; j < CPU_SETSIZE; j++)
// //       if (CPU_ISSET(j, &cpuset))
// //           printf("    CPU %d uses\n ", j);
// 
// }


#ifdef DEBUG_THREADS
    pthread_t I = pthread_self();
//     time_t seconds = time (NULL);
    float tbb_sec = (tick_count::now()-time0).seconds();
    TMyMutex::scoped_lock lock2;
    lock2.acquire(mutex);
      cout << "--------Begin time = " /*<< seconds << " (" */<< tbb_sec /*<< ")"*/ << " \top() ThrId=" << I << "\tCpuId=" << threadToCpuMap[I] << " \t"<< range.begin() << "-" << range.end() << "=" << range.end()-range.begin() << endl;
    lock2.release();
#endif // DEBUG_THREADS

    for(int i = range.begin(); i != range.end(); ++i){
      for(int j = 0; j < _NFits; j++){
        Fit(_track_vectors[i], _vStations, _NStations);
      }
    }

#ifdef DEBUG_THREADS
//     seconds = time (NULL);
    tbb_sec = (tick_count::now()-time0).seconds();
    TMyMutex3::scoped_lock lock3;
    lock3.acquire(mutex3);
      cout << "----------------End time = " /*<< seconds << " (" */<< tbb_sec /*<< ") "*/ << "op() ThrId=" << I  << endl;
    lock3.release();
#endif // DEBUG_THREADS
  }

  ApplyFit(TrackV *track_vectors, Station *vStations, int NStations, int NFits){
    _track_vectors = track_vectors;
//     _vStations = (Station*) malloc(sizeof(Station) * NStations);  // FIXME: some problems with memory here
//     _vStations = new Station[NStations];
//     for (int i = 0; i < NStations; i++) _vStations[i] = vStations[i];
    _vStations = vStations;
    _NStations = NStations;
    _NFits = NFits;
  }

  ~ApplyFit(){
//     free (_vStations);  // FIXME: some problems with memory here
//     delete[] _vStations;
  }
};

