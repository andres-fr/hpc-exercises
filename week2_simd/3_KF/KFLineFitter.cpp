  /// Fit of straight, linear tracks based on the Kalman Filter
  ///
  /// @authors: I.Kulakov; M.Zyzak
  /// @e-mail I.Kulakov@gsi.de
  /// 
  /// use "g++ KFLineFitter.cpp -O3; ./a.out" to run
 
// TODO SIMDIZE track fitting procedure using templates. Compare results and time

#define OUTPUT // output reconstruction results
#define TIME // count time

#include <stdlib.h> // rand
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>

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

struct LFPoint {
  LFPoint():x(NAN0),z(NAN0){};
  LFPoint( float x_, float z_ ): x(x_),z(z_) {};
  
  float x; // x-position of the hit
  float z; // coordinate of station
};

struct LFTrackParam {
  LFTrackParam():z(0){p[0] = 0; p[1] = 0;};
  LFTrackParam( float x_, float tx_, float z_ ):z(z_){p[0] = x_; p[1] = tx_;};
  float X( float z_ ) const { return p[1]*(z_-z)+p[0]; };
  
  const float &X() const { return p[0]; };
  const float &Tx() const { return p[1]; };
  const float &Z() const { return z; };

  
  float &X() { return p[0]; };
  float &Tx() { return p[1]; };
  float &Z() { return z; };
  
  float p[2]; // x, tx.
  float z;

  void Print() { cout << z << " " << p[0] << " " << p[1] << endl;};
};

struct LFTrackCovMatrix {
  float &C00() { return c[0]; };
  float &C10() { return c[1]; };
  float &C11() { return c[2]; };
  
  float c[3]; // C00, C10, C11

  void Print() { cout << c[0] << " " << c[1] << " " << c[2] << endl;};
};

struct LFTrack {
  vector<LFPoint> hits;
  
  LFTrackParam rParam; // track parameters reconstructed by the fitter
  LFTrackCovMatrix rCovMatrix; // error (or covariance) matrix
  float chi2; // chi-squared deviation between points and trajectory
  int ndf;    // number degrees of freedom

  vector<LFTrackParam> mcPoints; // simulated track parameters
};

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
  
  inline void Simulate( const LFTrackParam& param, LFTrack& track ) ;
  inline void Simulate( const LFTrackParam& param, LFTrack* track, int N ) ;

  void SetGeometry( const LFGeometry &g ) { fGeometry = g;  }
  void SetSigma   ( const float      &s ) { fSigma = s;  }
  
 private:
  LFGeometry fGeometry;
  float fSigma;
};

inline void LFSimulator::Simulate( const LFTrackParam& param, LFTrack& track )  
{
  track.mcPoints.clear();
  LFTrackParam curMCPoint = param;
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
    track.hits.push_back( LFPoint( x, z ) );
  }
}

inline void LFSimulator::Simulate( const LFTrackParam& param, LFTrack* track, int N )  
{
  for ( int i = 0; i < N; ++i ) {
    Simulate( param, track[i] );
  }
}


  /// ------------ FITTER ------------

class LFFitter {
 public:
  LFFitter(): fSigma(DSigma) {
  };
  
  inline void Fit( LFTrack& track ) const;
  
  void SetSigma( const float &s ) { fSigma = s; };
  
 private:

  inline void Initialize( LFTrack& track ) const;
  inline void Extrapolate( LFTrack& track, float z ) const;
  inline void Filter( LFTrack& track, float x ) const;

  float fSigma;
};

void LFFitter::Fit( LFTrack& track ) const
{
  Initialize( track );
  
  const int NHits = track.hits.size();
  for ( int i = 0; i < NHits; ++i ) {
    Extrapolate( track, track.hits[i].z );
    Filter( track, track.hits[i].x );
  }

  Extrapolate( track, track.mcPoints.back().z ); // exptrapolate to MC point for comparison with MC info
}

void LFFitter::Initialize( LFTrack& track ) const
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

void LFFitter::Extrapolate( LFTrack& track, float z_ ) const
{
  float &z = track.rParam.Z();
  float &x = track.rParam.X();
  float &tx = track.rParam.Tx();
  float &C00 = track.rCovMatrix.C00();
  float &C10 = track.rCovMatrix.C10();
  float &C11 = track.rCovMatrix.C11();
      
  const float dz = z_ - z;

  x += dz * tx;
  z = z_;

  // F = 1  dz
  //     0  1 

  const float C10_in = C10;
  C10 += dz * C11;
  C00 += dz * ( C10 + C10_in );
}

void LFFitter::Filter( LFTrack& track, float x_ ) const
{
 
  float &x = track.rParam.X();
  float &tx = track.rParam.Tx();
  float &C00 = track.rCovMatrix.C00();
  float &C10 = track.rCovMatrix.C10();
  float &C11 = track.rCovMatrix.C11();

    // H = { 1, 0 }
    // zeta = Hr - r // P.S. not "r - Hr" here becase later will be rather "r = r - K * zeta" then  "r = r + K * zeta"
  float zeta = x - x_;

    // F = C*H'
  float F0 = C00;
  float F1 = C10;
  
    // H*C*H'
  float HCH = F0;

    // S = 1. * ( V + H*C*H' )^-1
  float wi = 1./( fSigma*fSigma + HCH );
  float zetawi = zeta * wi;
  
  track.chi2 += zeta * zetawi ;
  track.ndf  += 1;

    // K = C*H'*S = F*S
  float K0 = F0*wi;
  float K1 = F1*wi;

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

      ///  ---- CREATE SIMULATOR & RECONSTRUCTOR -----

  const int   NStations = 10; // number of stations
  const float DZ = 10;     // distance between stations in standard geometry
  const float Sigma = 0.5; // sigma of hit distribution
  
  LFGeometry geo;
  geo.zs.resize(NStations);
  for( int i = 0; i < NStations; ++i ) geo.zs[i] = ( DZ * static_cast<float>(i) );

  LFSimulator sim;

  sim.SetGeometry(geo);
  sim.SetSigma(Sigma);

    ///  ---- SIMULATE TRACKS -----
  
  const int NTracks = 10000;
  LFTrack tracks[NTracks];

  for ( int i = 0; i < NTracks; ++i ) {
    LFTrack &track = tracks[i];
    
    LFTrackParam par;
    par.Z() = 0;
    par.X()  = (float(rand())/RAND_MAX-0.5)*5;
    par.Tx() = (float(rand())/RAND_MAX-0.5)*0.2;
    
    sim.Simulate( par, track );

  }

    ///  ---- GET FITTED TRACKS -----  
  TStopwatch timer;
  
  LFFitter fit;

  fit.SetSigma( Sigma );
  
#ifdef TIME
  timer.Start(1);
#endif 
  for ( int i = 0; i < NTracks; ++i ) {
    LFTrack &track = tracks[i];
    fit.Fit( track );
  }
#ifdef TIME
  timer.Stop();
#endif
     
      /// ---- SAVE RESULTS ---

#ifdef OUTPUT
  ofstream fout;
  fout.open("output",fstream::out);
  
  for ( int i = 0; i < NTracks; ++i ) {
    LFTrack &track = tracks[i];
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
