
//#define TOTALTIME

#include <iostream>
#include <fstream>
#include <cstring>
#include <stdlib.h>
using namespace std;
#ifdef _WIN32
# include <float.h> // _finite
# define finite _finite
#endif

#include "Stopwatch.h"
#include "Fit.h"

/* Use Intel TBB for multithreading */
#ifdef TBB
//  #define DEBUG_THREADS  // tbb with output 
#include "openlab_mod/multithread.h"
#include "openlab_mod/parallel_for_simpleEqNThr.h"
#include "openlab_mod/parallel_for_simpleHT.h"
#include "openlab_mod/parallel_for_simple.h"

#include "tbb/task_scheduler_init.h"
#include "tbb/parallel_for.h"
#include "tbb/blocked_range.h"
using namespace tbb;
#endif // TBB

#ifdef OMP
#include <omp.h>
#include "pthread.h"
#include "errno.h"
#define handle_error_en(en, msg) do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)
#endif

#if defined(TBB) || defined(OMP)
#define COPY
const int NCopy = 1000;
#endif

#define MUTE

#include <omp.h>

#include <ctime>
#include <string>
string dataDir = "./data/";


int tasks = 80;  /* #threads <= #tasks */

#ifdef TBB
const bool useObserver = 1;  // 1 for use thread-cpu correspondent
const float speedUpHT = -1; //1.04; // = time1logCore / time2logCoreOn1PhysCore. Need for optimize proc. usage.
#endif // TBB

#ifdef DEBUG_THREADS
// const int coeff = 1;
const int MaxNTracks = 20000;//20000
const int NFits = 1000;//10;
const int Ntimes = 1;//int(3000.*coeff);//100;
#else // if DEBUG_THREADS
const int MaxNTracks = 1920000;//20000
const int NFits = 10;//10;
#ifdef TBB
const int Ntimes = 1;
#else
const int Ntimes = 1;//int(3000.*coeff);//100;
#endif // TBB
#endif //  ifndef DEBUG_THREADS

Station* vStations;

Track vTracks[MaxNTracks];
MCTrack vMCTracks[MaxNTracks];
int NStations = 0;
int NTracks = 0;
int NTracksV = 0;



FieldRegion field0;

void ReadInput(){

  fstream FileGeo, FileTracks;

  FileGeo.open( (dataDir+"geo.dat").c_str(), ios::in );
  FileTracks.open( (dataDir+"tracks.dat").c_str(), ios::in );
  {
    FieldVector H[3];
    Fvec_t Hz[3];
    for( int i=0; i<3; i++){
      float Bx, By, Bz, z;
      FileGeo >> z >> Bx >> By >> Bz;
      Hz[i] = z; H[i].X = Bx;   H[i].Y = By; H[i].Z = Bz; 
#ifndef MUTE
      cout<<"Input Magnetic field:"<<z<<" "<<Bx<<" "<<By<<" "<<Bz<<endl;
#endif
    }
    field0.Set(H[0],Hz[0], H[1],Hz[1], H[2],Hz[2] );
  }
  FileGeo >> NStations;
#ifndef MUTE
  cout<<"Input "<<NStations<<" Stations:"<<endl;
#endif
  for( int i=0; i<NStations; i++ ){
    int ist;    
    FileGeo >> ist;
    if( ist!=i ) break;
    Station &st = vStations[i];
    FileGeo >> st.z >> st.thick>> st.RL;
#ifndef MUTE
#ifdef X87
    cout<<"    "<<st.z <<" "<<st.thick<<" "<<st.RL<<", ";
#else
    cout<<"    "<<st.z[0] <<" "<<st.thick[0]<<" "<<st.RL[0]<<", ";
#endif
#endif
    st.zhit = st.z - st.thick/2.f;
    st.RadThick = st.thick/st.RL;
    st.logRadThick = log(st.RadThick);
    int N=0;
    FileGeo >> N;
#ifndef MUTE
    cout<<N<<" field coeff."<<endl;
#endif
    for( int i=0; i<N; i++ ) FileGeo >> st.Map.X[i];
    for( int i=0; i<N; i++ ) FileGeo >> st.Map.Y[i];
    for( int i=0; i<N; i++ ) FileGeo >> st.Map.Z[i];
    //for( int i=0; i<10; i++ ){ st.Map.X[i] = st.Map.Y[i] = st.Map.Z[i] = 0.;}
    //st.Map.X[0]=Bx;
    //st.Map.Y[0]=By;
    //st.Map.Z[0]=Bz;
  }
  {
    Fvec_t Hy0 = vStations[NStations-1].Map.Y[0];
    Fvec_t z0  = vStations[NStations-1].z;
    Fvec_t sy = 0.f, Sy = 0.f;
    for( int i=NStations-1; i>=0; i-- ){
      Station &st = vStations[i];
      Fvec_t dz = st.z-z0;
      Fvec_t Hy = vStations[i].Map.Y[0];
      Sy += dz*sy + dz*dz*Hy/2.f;
      sy += dz*Hy;
      st.Sy = Sy;
      z0 = st.z;
    }
  }

  FileGeo.close();

  NTracks = 0;
  while( !FileTracks.eof() ){
    
    int itr;
    FileTracks>>itr;
    //if( itr!=NTracks ) break;
    if( NTracks>=MaxNTracks ) break;

    Track &t = vTracks[NTracks];
    MCTrack &mc = vMCTracks[NTracks];
    FileTracks >> mc.MC_x   >> mc.MC_y  >> mc.MC_z 
         >> mc.MC_px >> mc.MC_py >> mc.MC_pz >> mc.MC_q
         >> t.NHits;
    for( int i=0; i<t.NHits; i++ ){
      int ist;
      FileTracks >> ist;
      t.vHits[i].ista = ist;
      FileTracks >> t.vHits[i].x >> t.vHits[i].y;
    }
    if( t.NHits==NStations )   NTracks++;
  }
// 	cout << NTracks << " tracks have been read" << endl;
  FileTracks.close();

#ifdef OMP
    int threadNumberToCpuMap[80];
/*    for (int i=0; i<8; i++){
      threadNumberToCpuMap[2*i+0] = i;
      threadNumberToCpuMap[2*i+1] = i+8;
    }*/
    for ( int iProc = 0; iProc < 4; iProc++ ) {
        for ( int i = 0; i < 8; i++ ) {
            threadNumberToCpuMap[2 * i + 0 + iProc * 20] = 4 * i + iProc;
            threadNumberToCpuMap[2 * i + 1 + iProc * 20] = 4 * i + 32 + iProc;
        }
        for ( int i = 0; i < 2; i++ ) {
            threadNumberToCpuMap[2 * i + 0 + 16 + iProc * 20] = 4 * i + iProc + 64;
            threadNumberToCpuMap[2 * i + 1 + 16 + iProc * 20] = 4 * i + 8 + iProc + 64;
        }
    }
#endif

#ifdef COPY

    NTracks = 0;

#ifdef TBB
//#if 0
//   static simple_partitioner ap;
//   parallel_for(blocked_range<int>(0, tasks,1), ApplyMem(vTracks, vMCTracks, &NTracks, NCopy, MaxNTracks), ap);
    parallel_for_simpleEqNThr( 1, tasks, ApplyMem( vTracks, vMCTracks, &NTracks, NCopy, MaxNTracks ) );
#else //TBB

    //TODO parallelize data copying
    {
        for ( int j = 0; j < tasks; ++j ) {
            for ( int i = 0; i < NCopy; ++i ) {
                if ( j * NCopy + i > MaxNTracks ) {
                    continue;
                }
                vTracks[j * NCopy + i] = vTracks[i];
                vMCTracks[j * NCopy + i] = vMCTracks[i];
                NTracks++;
            }
        }
    }
#endif //TBB
#endif // COPY
    NTracksV = NTracks / vecN;
    NTracks =  NTracksV * vecN;
}

void WriteOutput(){

  fstream Out, Diff;

  Out.open( (dataDir + "fit.dat").c_str(), ios::out );
  Diff.open( (dataDir + "fitdiff.dat").c_str(), ios::out );

  for( int it=0, itt=0; itt<NTracks; itt++ ){
    Track &t = vTracks[itt];
    MCTrack &mc = vMCTracks[itt];

    // convert matrix 
    double C[15];
    {
      Single_t *tC = (Single_t *) &t.C;
      for( int i=0, n=0; i<5; i++)
        for( int j=0; j<=i; j++, n++ ){
          C[n]=0;
          //for(int k=0; k<5;k++) C[n]+=tC[i*5+k] * tC[j*5+k];
          C[n] = tC[n];
        }
    }

    bool ok = 1;
    for( int i=0; i<6; i++ ){
      ok = ok && finite(t.T[i]);
    }
    for( int i=0; i<15; i++ ) ok = ok && finite(C[i]);

    if(!ok){ cout<<" infinite "<<endl; continue; }

    Out <<it<<endl<<"   "
        << " " << mc.MC_x  << " " << mc.MC_y  << " " << mc.MC_z 
        << " " << mc.MC_px << " " << mc.MC_py << " " << mc.MC_pz 
        << " " << mc.MC_q<<endl;
    Out <<"   ";
    for( int i=0; i<6; i++ ) Out<< " " <<t.T[i];
    Out <<endl<<"   ";
    for( int i=0; i<15; i++ ) Out<< " " <<C[i];
    Out <<endl;

    float tmc[6] = { mc.MC_x, mc.MC_y, mc.MC_px/mc.MC_pz, mc.MC_py/mc.MC_pz, 
              mc.MC_q/sqrt(mc.MC_px*mc.MC_px+mc.MC_py*mc.MC_py+mc.MC_pz*mc.MC_pz), mc.MC_z};
    Diff <<it<<endl;
    Diff<<"   ";
    for( int i=0; i<6; i++ ) Diff<< " " <<t.T[i]-tmc[i];
    Diff<<endl<<"   ";
    for( int i=0; i<15; i++ ) Diff<< " " <<C[i];
    Diff<<endl;
    it++;
  }
  Out.close();
  Diff.close();
}

void FitTracksV(){

  double TimeTable[Ntimes];

  TrackV *TracksV = new TrackV[NTracksV];
  Fvec_t *Z0      = new Fvec_t[NTracksV];
  //TrackV *TracksV = Vc::malloc<TrackV, Vc::AlignOnVector>(MaxNTracks / vecN + 1);
  //Fvec_t *Z0      = Vc::malloc<Fvec_t, Vc::AlignOnVector>(MaxNTracks/vecN+1);

#ifdef VC
  Fvec_t::Memory Z0mem;
#endif
#ifndef MUTE
  cout<<"Prepare data..."<<endl;
#endif
  Stopwatch timer1;

  for( int iV=0; iV<NTracksV; iV++ ){ // loop on set of 4 tracks
#ifndef MUTE
    if( iV*vecN%100==0 ) cout<<iV*vecN<<endl;
#endif
    TrackV &t = TracksV[iV];
    for( int ist=0; ist<NStations; ist++ ){
      HitV &h = t.vHits[ist];

//       if((((unsigned long)(&h.x)) & 0x3f) != 0) {
//           std::cout << iV << ", " << ist << ": " << std::hex << (unsigned long)&h.x << std::endl;
//           std::cout << std::hex << (unsigned long)&h << std::endl;
//           std::cout << std::hex << (unsigned long)&h.y << std::endl;
//           std::cout << std::hex << (unsigned long)&h.w << std::endl;
//           std::cout << std::hex << (unsigned long)&h.H.X << std::endl;
//           std::cout << std::hex << (unsigned long)&h.H.Y << std::endl;
//           std::cout << std::hex << (unsigned long)&h.H.Z << std::endl;
//       }

      h.x = 0.;
      h.y = 0.;
      h.w = 0.;
      h.H.X = 0.;
      h.H.Y = 0.;
      h.H.Z = 0.;
    }

#ifdef VC
    Fvec_t::Memory hxmem[NStations],hymem[NStations],hwmem[NStations];
    for( int it=0; it<vecN; it++ ){
      Track &ts = vTracks[iV*vecN+it];

      Z0mem[it] = vMCTracks[iV*vecN+it].MC_z;

      for( int ista=0, ih=0; ista<NStations; ista++ ){
        Hit &hs = ts.vHits[ih];
        if (hs.ista != ista) continue;
        ih++;
        HitV &h = t.vHits[hs.ista];

        hxmem[ista][it] = hs.x;
        hymem[ista][it] = hs.y;
        hwmem[ista][it] = 1.;
      }

    }
    for( int ista=0; ista<NStations; ista++ ){
      Fvec_t hxtemp( hxmem[ista] );
      Fvec_t hytemp( hymem[ista] );
      Fvec_t hwtemp( hwmem[ista] );
      t.vHits[ista].x = hxtemp;
      t.vHits[ista].y = hytemp;
      t.vHits[ista].w = hwtemp;
    }


    Fvec_t Z0temp(Z0mem);
    Z0[iV] = Z0temp;
#else // VC
    for( int it=0; it<vecN; it++ ){
      Track &ts = vTracks[iV*vecN+it];
#ifdef X87
      Z0[iV] = vMCTracks[iV*vecN+it].MC_z;
#else
      Z0[iV][it] = vMCTracks[iV*vecN+it].MC_z;
#endif
      for( int ih=0; ih<ts.NHits; ih++ ){
        Hit &hs = ts.vHits[ih];
        HitV &h = t.vHits[hs.ista];
    #ifdef X87
        h.x = hs.x;
        h.y = hs.y;
        h.w = 1.;
    #else
        h.x[it] = hs.x;
        h.y[it] = hs.y;
        h.w[it] = 1.;
    #endif
      }
    }
#endif // else VC


    if (0){    // output for check
      cout << "track " << iV << "  ";
      for( int ista=0, ih=0; ista<NStations; ista++ )
        cout << t.vHits[ista].x << " ";
      cout << endl;
    }


    for( int ist=0; ist<NStations; ist++ ){
      HitV &h = t.vHits[ist];
      vStations[ist].Map.GetField(h.x, h.y, h.H);
    }
  }
  timer1.Stop();
#ifndef MUTE
  cout<<"Start fit..."<<endl;
#endif
  Stopwatch timer;
  Stopwatch timer2;
  
  int ifit;
  int iV;

  int nFittedTracks = 0;
  Stopwatch timerLocal;
#ifndef OMP
  tasks = 1;
#endif
  double *fitSpeedCPU  = new double[tasks];
  double *fitSpeedReal = new double[tasks];

//   Stopwatch timer_test;
  timer.Start();
  for( int times=0; times<Ntimes; times++){
    timer2.Start();
#ifdef TBB
#ifdef DEBUG_THREADS
  cout << NTracksV << " tracks by " << NTracksV / tasks << " tracks in group are run." << endl;
#endif // DEBUG_THREADS

  // FIXME: using affinity_partitioner only gives performance gain with #CPUs > 2
    // use affinity_partitioner
//   static simple_partitioner ap;
//   static affinity_partitioner ap;  // standart
//   parallel_for(blocked_range<int>(0, NTracksV, NTracksV / tasks), ApplyFit(TracksV, vStations, NStations, NFits), ap);
    // use auto partitioning. Good then tasks != 2^N
//   static auto_partitioner ap;
//   parallel_for(blocked_range<int>(0, NTracksV), ApplyFit(TracksV, vStations, NStations, NFits), ap);
  {      // use special partitioning.  Good used with fixed thread-cpu correspondent by observer
//     if (speedUpHT >= 0){
//       parallel_for_simpleHT(NTracksV, tasks, ApplyFit(TracksV, vStations, NStations, NFits),speedUpHT);
//     }
//     else{
//       parallel_for_simple(NTracksV, 10, tasks, ApplyFit(TracksV, vStations, NStations, NFits));
//     };

    {   // fix nTask on thread
      int nTracksVOnThread = NCopy/vecN;
      parallel_for_simpleEqNThr(nTracksVOnThread, tasks, ApplyFit(TracksV, vStations, NStations, NFits));
      NTracks = nTracksVOnThread*vecN*tasks; // for time counters
    }
  }
#else 
  //TODO parallelize calculations
  {
    timerLocal.Start();
    for( iV=0; iV<NTracksV; iV++ ){ // loop on set of 4 tracks
      for( ifit=0; ifit<NFits; ifit++){
         Fit( TracksV[iV], vStations, NStations );      
       }
       nFittedTracks += vecN;
    }
    timerLocal.Stop();
    double cpuTimeLocal = timerLocal.CpuTime();
    double realTimeLocal = timerLocal.RealTime();
    
    if(fabs(cpuTimeLocal)<1.e-8) cpuTimeLocal = realTimeLocal;
    
#ifdef OMP
    int curThredNum = omp_get_thread_num();
#else
    int curThredNum = 0;
#endif
    if(cpuTimeLocal > 0)
      fitSpeedCPU[curThredNum] = nFittedTracks/cpuTimeLocal;
    else
      fitSpeedCPU[curThredNum] = 0;
    
    if(realTimeLocal > 0)
      fitSpeedReal[curThredNum] = nFittedTracks/realTimeLocal;
    else
      fitSpeedReal[curThredNum] = 0;
  }
#endif
    timer2.Stop();
    TimeTable[times]=timer2.RealTime();
  }
  timer.Stop();

  for( int iV=0; iV<NTracksV; iV++ ){ // loop on set of 4 tracks
    TrackV &t = TracksV[iV];
    ExtrapolateALight( t.T, t.C, Z0[iV], TracksV[iV].T[4], field0 );
  }

  double realtime=0;
  fstream TimeFile;
  TimeFile.open( (dataDir + "time.dat").c_str(), ios::out );
  for( int times=0; times<Ntimes; times++ ){
    TimeFile << TimeTable[times]*1.e6/(NTracks*NFits)<<endl;
    realtime += TimeTable[times]*1.e6/(NTracks*NFits);
  }  
  TimeFile.close();
  realtime /= Ntimes;

#ifndef MUTE
  cout<<"Preparation time/track = "<<timer1.CpuTime()*1.e6/NTracks/NFits<<" [us]"<<endl;
  cout<<"CPU  fit time/track = "<<timer.CpuTime()*1.e6/(NTracks*NFits)/Ntimes<<" [us]"<<endl;
  cout<<"Real fit time/track = "<<realtime <<" [us]"<<endl;
  cout<<"Total fit time = "<<timer.CpuTime()<<" [sec]"<<endl;
  cout<<"Total fit real time = "<<timer.RealTime()<<" [sec]"<<endl;
#else
#ifdef TOTALTIME
  
  double realSpeedTotal = 0.;
  double cpuSpeedTotal = 0.;
  
  for(int iThread=0; iThread<tasks; iThread++)
  {
    realSpeedTotal += fitSpeedReal[iThread];
    cpuSpeedTotal += fitSpeedCPU[iThread];
  }
  
  cout<<"Prep[us], CPU fit/tr[us], Real fit/tr[us], CPU[sec], Real[sec] = "<<timer1.CpuTime()*1.e6/NTracks/NFits<<"\t";
  cout<< (1.e6/cpuSpeedTotal/NFits) <<"\t";
  cout<< (1.e6/realSpeedTotal/NFits) <<"\t";
//   cout<<timer.CpuTime()*1.e6/(NTracks*NFits)/Ntimes<<"\t";
//   cout<<realtime <<"\t";
  cout<<timer.CpuTime()<<"\t";
  cout<<timer.RealTime()<<endl;
#else //TOTALTIME
  cout<<"Prep[us], CPU fit/tr[us], Real fit/tr[us], CPU[sec], Real[sec] = "<<timer1.CpuTime()*1.e6/NTracks/NFits<<"\t";
  cout<<timer.CpuTime()*1.e6/(NTracks*NFits)/Ntimes<<"\t";
  cout<<realtime <<"\t";
  cout<<timer.CpuTime()<<"\t";
  cout<<timer.RealTime()<<endl;
#endif //OMP
#endif
  
  if(fitSpeedCPU)  delete[] fitSpeedCPU;
  if(fitSpeedReal) delete[] fitSpeedReal;
  
  for( int iV=0; iV<NTracksV; iV++ ){ // loop on set of 4 tracks
    TrackV &t = TracksV[iV];
    for( int it=0; it<vecN; it++ ){
      Track &ts = vTracks[iV*vecN+it];
      Fvec_t *C = (Fvec_t*)&t.C;
      Single_t *sC = (Single_t*) &ts.C;
#ifdef X87
      for( int i=0; i<6; i++ ) ts.T[i] = t.T[i];
#else
      for( int i=0; i<6; i++ ) ts.T[i] = t.T[i][it];
#endif
#ifdef SQRT_FILTER
      for( int i=0,n=0; i<5; i++ )
        for( int j=0; j<=i; j++,n++){
          sC[n]=0;
          for(int k=0;k<5;k++) sC[n]+= C[i*5+k][it]*C[j*5+k][it];
        }
#else
#ifdef X87
      for( int i=0; i<15; i++ ) sC[i] = C[i];
#else
      for( int i=0; i<15; i++ ) sC[i] = C[i][it];
#endif // X87
#endif // SQRT_FILTER
    }
  }

  delete [] Z0;
  delete [] TracksV;
}

int main(int argc, char *argv[]){

//   time_t seconds;
//   seconds = time (NULL);
//   cout << "Begin NowIs(h:m:s) " << seconds/3600 << ":"  << (seconds/60)%60 << ":" << seconds%60 << endl;

  // RRMOD
  vStations = new Station[8];
//  vStations = Vc::malloc<Station, Vc::AlignOnVector>(8);

  // parse cmdline and set #tasks if TBB or OpenMP
  if(string(argv[0]).find("omp") != string::npos || string(argv[0]).find("tbb") != string::npos){
      if(argc != 2){
      cout << "Please specify #parallel tasks" << endl;
    return 1;
    }
    tasks = atoi( argv[1] );
  }

#ifdef TBB
  task_scheduler_init init(tasks);
  TMyObserver obs;
  obs.FInit();    // set cpu-threads correspondence
  obs.observe(useObserver); // Turn on observer. It's turn off by default.
#endif

  ReadInput();
  FitTracksV();
//  WriteOutput();
#ifndef MUTE
  cout<<"track size = "<<sizeof(TrackV)<<endl;
  cout<<"char size = "<<sizeof(char)<<endl;
#endif

  // RRMOD
  delete[] vStations;

//   seconds = time (NULL);
//   cout << "End NowIs(h:m:s) " << seconds/3600 << ":"  << (seconds/60)%60 << ":" << seconds%60 << endl;
  return 0;
}
