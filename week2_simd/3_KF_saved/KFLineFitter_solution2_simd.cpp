  /// Fit of straight, linear tracks based on the Kalman Filter
  ///
  /// @author I.Kulakov
  /// @e-mail I.Kulakov@gsi.de
  /// 
  /// use "g++ KFLineFitter_solution2_simd.cpp -O3 -msse; ./a.out" to run
 
#define SIMDIZED // uncomment this for vectorized code. Scalar code is default.
#define OUTPUT // output reconstruction results
#define TIME // count time



#include <stdlib.h> // rand
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>

#ifdef SIMDIZED
#include "../../../vectors/P4_F32vec4.h" // SSE instructions ( 4 floats per vector )
//#include "../../../vectors/PSEUDO_F32vec4.h" // simulation of the SSE instruction
//#include "../../../vectors/PSEUDO_F32vec1.h" // simulation with 1 floats per vector
#endif

#ifdef TIME
#include "../../../TStopwatch.h"
#endif

using namespace std;

enum LFDistributionType {
  kUniform
};

const int NAN0 = -100000;


const float InfX = 1000;

const float InfTx  = 1000;


  // standard parameters. (Are changed in the main function!)
const int   DNStations = 6; // number of stations
const float DDZ = 10;     // distance between stations in standard geometry
const float DSigma = 0.5; // sigma of hit distribution

template< typename T >
struct LFPoint {
  LFPoint():x(NAN0),z(NAN0){};
  LFPoint( T x_, float z_ ): x(x_),z(z_) {};
  
  T x; // x-position of the hit
  float z; // coordinate of station // all points on one station have same z-position
};

template< typename T >
struct LFTrackParam {
  LFTrackParam():z(0){p[0] = 0; p[1] = 0;};
  LFTrackParam( T x_, T tx_, float z_ ):z(z_){p[0] = x_; p[1] = tx_;};
  T X( float z_ ) const { return p[1]*(z_-z)+p[0]; };
  
  const T &X() const { return p[0]; };
  const T &Tx() const { return p[1]; };
  const T &Z() const { return z; };

  
  T &X() { return p[0]; };
  T &Tx() { return p[1]; };
  float &Z() { return z; };
  
  T p[2]; // x, tx.
  float z;

  void Print() { cout << z << " " << p[0] << " " << p[1] << endl;};
};

template< typename T >
struct LFTrackCovMatrix {
  T &C00() { return c[0]; };
  T &C10() { return c[1]; };
  T &C11() { return c[2]; };
  
  T c[3]; // C00, C10, C11

  void Print() { cout << c[0] << " " << c[1] << " " << c[2] << endl;};
};

template< typename T, typename I >
struct LFTrack {
  vector< LFPoint<T> > hits;
  
  LFTrackParam<T> rParam; // reconstructed by the fitter track parameters
  LFTrackCovMatrix<T> rCovMatrix; // error (or covariance) matrix
  T chi2;   // chi-squared deviation between points and trajectory
  I ndf;    // number degrees of freedom

  vector< LFTrackParam<T> > mcPoints; // simulated track parameters
};

#ifdef SIMDIZED
void CopyTrackHits( const LFTrack<float,int>* sTracks, LFTrack<fvec,fvec>* vTracks, int nVTracks ){
  const int NHits = sTracks[0].hits.size(); // all tracks have the same number of hits
  
      
  for( int iV = 0; iV < nVTracks; ++iV ) {
    LFTrack<fvec,fvec>& vTrack = vTracks[iV];
    vTrack.hits.resize(NHits);
    vTrack.mcPoints.resize(NHits);
    for( int i = 0; i < fvecLen; ++i ) {
      const LFTrack<float,int>& sTrack = sTracks[ iV*fvecLen + i ];

      for( int iH = 0; iH < NHits; ++iH ) {
        vTrack.hits[iH].x[i] = sTrack.hits[iH].x;
        vTrack.hits[iH].z = sTrack.hits[iH].z;
      }

      vTrack.mcPoints[NHits-1].z = sTrack.mcPoints[NHits-1].z; // need this info for comparison of reco and MC
    }
  }
}

void CopyTrackParams( const LFTrack<fvec,fvec>* vTracks, LFTrack<float,int>* sTracks, int nVTracks ){


  for( int iV = 0; iV < nVTracks; ++iV ) {
    const LFTrack<fvec,fvec>& vTrack = vTracks[iV];
    for( int i = 0; i < fvecLen; ++i ) {
      LFTrack<float,int>& sTrack = sTracks[ iV*fvecLen + i ];

      for( int ip = 0; ip < 2; ++ip )
        sTrack.rParam.p[ip] = vTrack.rParam.p[ip][i];
      sTrack.rParam.z = vTrack.rParam.z;
      for( int ic = 0; ic < 3; ++ic )
        sTrack.rCovMatrix.c[ic] = vTrack.rCovMatrix.c[ic][i];
      sTrack.chi2 = vTrack.chi2[i];
      sTrack.ndf = vTrack.ndf[i];
    }
  }
}
#endif

struct LFGeometry {
  // LFGeometry():zs(){ zs.clear(); };
  // ~LFGeometry(){};
  
  int GetNStations() const { return zs.size(); }
  vector<float> zs; // z of a station
};

  /// ----------- SIMULATOR -------------------



  // return simulated track with given parameters
class LFSimulator {
 public:
  LFSimulator():fGeometry(), fSigma(DSigma){
    fGeometry.zs.resize(DNStations);
    for( int i = 0; i < DNStations; ++i ) fGeometry.zs[i] = ( DDZ*static_cast<float>(i) );
  };

  ~LFSimulator(){};
  
  inline void Simulate( const LFTrackParam<float>& param, LFTrack<float,int>& track ) ;
  inline void Simulate( const LFTrackParam<float>& param, LFTrack<float,int>* track, int N ) ;

  void SetGeometry( const LFGeometry &g ) { fGeometry = g;  }
  void SetSigma   ( const float      &s ) { fSigma = s;  }
  
 private:
  LFGeometry fGeometry;
  float fSigma;
};

inline void LFSimulator::Simulate( const LFTrackParam<float>& param, LFTrack<float,int>& track )  
{
  track.mcPoints.clear();
  LFTrackParam<float> curMCPoint = param;
  for ( int i = 0; i < fGeometry.GetNStations(); ++i ) {
    const float z = fGeometry.zs[i];
      // propagate to next station
    curMCPoint.X() = curMCPoint.X(z);
    curMCPoint.Z() = z;
      // save
    track.mcPoints.push_back( curMCPoint );
  }

  track.hits.clear();
  for ( int i = 0; i < fGeometry.GetNStations(); ++i ) {
    const float z = track.mcPoints[i].z;
    float x = NAN0;
      // distribute hits uniformly
    {
      const float width = 2*fSigma*sqrt(3); // sqrt(12)
      x = track.mcPoints[i].X() + (float(rand())/RAND_MAX-0.5)*width;
    }
    track.hits.push_back( LFPoint<float>( x, z ) );
  }
}

inline void LFSimulator::Simulate( const LFTrackParam<float>& param, LFTrack<float,int>* track, int N )  
{
  for ( int i = 0; i < N; ++i ) {
    Simulate( param, track[i] );
  }
}


  /// ------------ FITTER ------------

template< typename T, typename I >
class LFFitter {
 public:
  LFFitter(): fSigma(DSigma) {
  };
  
  inline void Fit( LFTrack<T,I>& track ) const;
  
  void SetSigma( const float &s ) { fSigma = s; };
  
 private:
  inline void Initialize( LFTrack<T,I>& track ) const;
  inline void Extrapolate( LFTrack<T,I>& track, float z ) const;
  inline void Filter( LFTrack<T,I>& track, T x ) const;

  float fSigma;
};

template< typename T, typename I >
void LFFitter<T,I>::Fit( LFTrack<T,I>& track ) const
{
  Initialize( track );
  const int NHits = track.hits.size();
  for ( int i = 0; i < NHits; ++i ) {
    Extrapolate( track, track.hits[i].z );
    Filter( track, track.hits[i].x );
  }

  Extrapolate( track, track.mcPoints.back().z ); // just for pulls
}

template< typename T, typename I >
void LFFitter<T,I>::Initialize( LFTrack<T,I>& track ) const
{
  track.rParam.Z() = 0;
  track.rParam.X() = 0;
  track.rParam.Tx() = 0;
  track.chi2 = 0;
  track.ndf = -2;

  track.rCovMatrix.C00() = InfX;
  track.rCovMatrix.C10() = 0;
  track.rCovMatrix.C11() = InfTx;
}

template< typename T, typename I >
void LFFitter<T,I>::Extrapolate( LFTrack<T,I>& track, float z_ ) const
{
  float &z = track.rParam.Z();
  T &x = track.rParam.X();
  T &tx = track.rParam.Tx();
  T &C00 = track.rCovMatrix.C00();
  T &C10 = track.rCovMatrix.C10();
  T &C11 = track.rCovMatrix.C11();
      
  const float dz = z_ - z;

  x += dz * tx;
  z = z_;

  // F = 1  dz
  //     0  1 

  const T C10_in = C10;
  C10 += dz * C11;
  C00 += dz * ( C10 + C10_in );
}

template< typename T, typename I >
void LFFitter<T,I>::Filter( LFTrack<T,I>& track, T x_ ) const
{
 
  T &x = track.rParam.X();
  T &tx = track.rParam.Tx();
  T &C00 = track.rCovMatrix.C00();
  T &C10 = track.rCovMatrix.C10();
  T &C11 = track.rCovMatrix.C11();

    // H = { 1, 0 }
    // zeta = Hr - r // P.S. not "r - Hr" here becase later will be rather "r = r - K * zeta" then  "r = r + K * zeta"
  T zeta = x - x_;

  // F = C*H'
  T F0 = C00;
  T F1 = C10;
  
    // H*C*H'
  T HCH = F0;

    // S = 1. * ( V + H*C*H' )^-1
  T wi = 1./( fSigma*fSigma + HCH );
  T zetawi = zeta * wi;
  
  track.chi2 += zeta * zetawi ;
  track.ndf  += 1;

    // K = C*H'*S = F*S
  T K0 = F0*wi;
  T K1 = F1*wi;

    // r = r - K * zeta
  x  -= K0*zeta;
  tx -= K1*zeta;

    // C = C - K*H*C = C - K*F
  C00 -= K0*F0;
  C10 -= K1*F0;
  C11 -= K1*F1;

}

/// ------------ RUNNER ------------

int main(){
  std::cout << " Begin " << std::endl;

    ///  ---- SIMULATE TRACKS -----
  
  const int   NStations = 10; // number of stations
  const float DZ = 10;     // distance between stations in standard geometry
  const float Sigma = 0.5; // sigma of hit distribution
  
  LFGeometry geo;
  geo.zs.resize(NStations);
  for( int i = 0; i < NStations; ++i ) geo.zs[i] = ( DZ * static_cast<float>(i) );

  LFSimulator sim;

  sim.SetGeometry(geo);
  sim.SetSigma(Sigma);
  
  const int NTracks = 10000;
  LFTrack<float,int> tracks[NTracks];

  for ( int i = 0; i < NTracks; ++i ) {
    LFTrack<float,int> &track = tracks[i];
    
    LFTrackParam<float> par;
    par.Z() = 0;
    par.X()  = (float(rand())/RAND_MAX-0.5)*5;
    par.Tx() = (float(rand())/RAND_MAX-0.5)*0.2;
    
    sim.Simulate( par, track );

  }

    ///  ---- GET FITTED TRACKS -----  
  TStopwatch timer;
#ifndef SIMDIZED
  
  LFFitter<float,int> fit;

  fit.SetSigma( Sigma );
  
#ifdef TIME
  timer.Start(1);
#endif 
  for ( int i = 0; i < NTracks; ++i ) {
    LFTrack<float,int> &track = tracks[i];
    fit.Fit( track );
  }
  #ifdef TIME
  timer.Stop();
  #endif
  
#else
  
    // Convert scalar Tracks to SIMD-tracks
  const int NVTracks = NTracks/fvecLen;
  LFTrack<fvec,fvec> vTracks[NVTracks];
  
  CopyTrackHits( tracks, vTracks, NVTracks );
  
    // fit
  LFFitter<fvec,fvec> fit;

  fit.SetSigma( Sigma );
  
#ifdef TIME
  timer.Start(1);
#endif
  for ( int i = 0; i < NVTracks; ++i ) {
    LFTrack<fvec,fvec> &track = vTracks[i];
    fit.Fit( track );
  }
#ifdef TIME
  timer.Stop();
#endif
  
    // Convert  SIMD-tracks to scalar Tracks
  CopyTrackParams( vTracks, tracks, NVTracks );
  
#endif // SIMDIZED
    
      /// ---- SAVE RESULTS ---

#ifdef OUTPUT
  ofstream fout;
  fout.open("output",fstream::out);
  
  for ( int i = 0; i < NTracks; ++i ) {
    LFTrack<float,int> &track = tracks[i];
    fout << i << endl; // << "Track: " 
    fout  << track.mcPoints.back().Z() << " " << track.mcPoints.back().X() << " " << track.mcPoints.back().Tx() << endl; // << "MC   : "
    fout << track.rParam.Z() << " " << track.rParam.X() << " " << track.rParam.Tx() << endl; // << "Reco: "
    fout << track.rCovMatrix.C00() << " " << track.rCovMatrix.C10() << " " << track.rCovMatrix.C11() << endl;
  }
  fout.close();
#endif

#ifdef TIME
  cout << "Time: " << timer.RealTime()*1000 << " ms " << endl;
#endif
  
  std::cout << " End " << std::endl;
  return 1;
}
