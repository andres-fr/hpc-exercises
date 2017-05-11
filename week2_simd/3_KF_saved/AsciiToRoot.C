
#include <fstream>
#include <iostream>
#include <TString.h>
#include <TH1D.h>
#include <TH1F.h>
#include <TF1.h>
#include <TFile.h>
#include <TLatex.h>
#include <TStyle.h>
#include <TProfile.h>

#include <stdio.h>
#include <cmath>

// #define Debug
// #define ERROR_OUT

using namespace std;

const int MaxNtr = 20000;

const char* fres_name = "output";

float chi2[MaxNtr];

const int NPar = 3;
const int NCov = 3;

float Par[MaxNtr][NPar][3];		// [0] - value;  [1] - error; [2] - MC
TString Par_name[NPar]={"z", "X", "Tx"};
float Par_max[NPar][2];  // [0] - max value;  [1] - max (value - MCvalue)

float xboundary = 2.0;
float txboundary = 0.02;

const int nbins = 100;

void AsciiToRoot(){

  int Ntr = 0; // number of tracks
  int NgoodTr = 0;     // number of tracks with normal parameters

    
    // --------------------- Read results -----------------------------
  { 
    FILE * fpres; 
    fpres = fopen(fres_name, "r");

    int n = -1; 
#ifdef Debug 
    cout << "sds !!!!!!"<< feof(fpres)<<endl;
#endif
    for (int i = 0; (i < MaxNtr) && !feof(fpres); i++, NgoodTr++){
#ifdef Debug 
        // cout << i << endl;
#endif
      float Tmc[NPar],T[NPar],C[NCov];
      fscanf (fpres, "%d", &(n)); // iTrack
      if (feof(fpres)) break;
      if ( n != i ) {
        cout << " Corrupted file: " << n << " != " << i << endl;
        return;
      }
      for(int k = 0; k<NPar; k++) fscanf (fpres, "%f", &(Tmc[k]));
      for(int k = 0; k<NPar; k++) fscanf (fpres, "%f", &(T[k]));
      for(int k = 0; k<NCov; k++) fscanf (fpres, "%f", &(C[k]));
#ifdef Debug
      for(int k = 0; k<NPar; k++) cout << Tmc[k] << " ";
      cout << endl;
      for(int k = 0; k<NPar; k++) cout << T[k] << " ";
      cout << endl;
      for(int k = 0; k<NCov; k++) cout << C[k] << " ";
      cout << endl;
#endif
      Par[i][0][2] = Tmc[0];
      Par[i][1][2] = Tmc[1];
      Par[i][2][2] = Tmc[2];

      for(int k = 0; k<NPar; k++){
        if (Par_max[k][0] < abs(T[k])) Par_max[k][0] = abs(T[k]);
        if (Par_max[k][1] < abs(Par[i][k][2]-T[k])) Par_max[k][0] = abs(Par[i][k][2]-T[k]);
      }

      for(int k = 1,kk=0; k<NPar; k++,kk+=k){
        Par[i][k][0] = T[k];
        if ( C[kk] > 0 ){
          Par[i][k][1] = sqrt(C[kk]);
        }
        else{
#ifdef ERROR_OUT
          cout << "Error: In track " << i+Ntr-NgoodTr << " er^2 = " << C[kk] << " <= 0. Entry will be skiped." << endl;
#endif // ERROR_OUT
          Par[i][k][1] = -sqrt(-C[kk]); // mark as bad
            // i--;
            // NgoodTr--;
            // break;
        }
      }
      Par[i][0][0] = T[0];
      Par[i][0][1] = 1;//error

        // float tmp;
        // for(int k = 0; k<7+6+15; k++) fscanf (fpres, "%f", &tmp);
    }

#ifdef Debug 
    cout << "sds !!!!!!"<< feof(fpres)<<endl;
#endif
    if (feof(fpres)) cout << " Data from file " << fres_name << " read succesfully " << endl; 
    fclose(fpres);
  } // read results
	

    // -------------- Fill histo ---------------------------

    //     TCanvas *c = new TCanvas("asdf","asdf",0,0,400,500);
    //     c ->Divide(2,3);
#ifdef Debug 
  cout << "tut" << endl;
  cout << fres_name <<"Fill histo"<< endl;
  cout << "N Read Tracks = " << NgoodTr << endl;
#endif
  TH1D *Pull[NPar],*X[NPar];//,*dX[5];

  TString FRootName;
FRootName = "track_param.root";
  TFile f(FRootName.Data(),"RECREATE");
#ifdef Debug 
  cout << FRootName << endl;
#endif
    
    gStyle->SetOptStat(110010);
    for (int i = 0; i < NgoodTr; i++)
        // cout << Par[i][5][0] - Par[i][5][2] << endl;
      if ( (Par[i][0][0] - Par[i][0][2]) / Par[i][0][1] > 1e-2 ) {
        cout << " ERROR: Track: " << i << " DIFERENT Z OF RECO AND MC!! dz > 1e-2" << endl;
        cout << " Reco: " << Par[i][0][0] << " MC: " << Par[i][0][2] << endl;
        continue;
      }
  
    for (int k = 0; k < NPar; k++){

        // ----- Pulls
      Pull[k] = new TH1D("Pull "+Par_name[k],"Pull "+Par_name[k],nbins,-6.,6.);
      for (int i = 0; i < NgoodTr; i++){
        if( Par[i][k][1] > 0 ) Pull[k]->Fill( (Par[i][k][0] - Par[i][k][2]) / Par[i][k][1] );
      }

      TF1  *pullfit = new TF1("gaussFitPull "+Par_name[k],"gauss");
      pullfit->SetLineColor(2);
      Pull[k]->Fit("gaussFitPull "+Par_name[k],"","",Pull[k]->GetXaxis()->GetXmin(),Pull[k]->GetXaxis()->GetXmax());

      Pull[k]->GetXaxis()->SetTitle("Pull "+Par_name[k]);
      Pull[k]->GetXaxis()->SetTitleSize(0.04);

        // ----- Values
      X[k] = new TH1D(Par_name[k],Par_name[k],nbins,-Par_max[k][0],Par_max[k][0]);
      for (int i = 0; i < NgoodTr; i++){
        // if ( Par[i][5][0] > 1. )
        X[k]->Fill( Par[i][k][0] );
      }

        // Save histo
      Pull[k]->Write();
      X[k]->Write();
        //  		dX[k]->Write();
    }

    TLatex Sss(1,2," Residual1");

      // ----- Residual x
    TH1D *dx = new TH1D(" Residualx"," Residualx",nbins,-xboundary,xboundary);
      // 	TH1D *dx = new TH1D(Sss,Sss,nbins,-200,200);
    for (int i = 0; i < NgoodTr; i++){
      dx->Fill( (Par[i][1][0] - Par[i][1][2]) );
    }

    TF1  *dxfit = new TF1(" Residualx","gaus");
    dxfit->SetLineColor(2);
    dx->Fit(" Residualx","","",dx->GetXaxis()->GetXmin(),dx->GetXaxis()->GetXmax());

    dx->GetXaxis()->SetTitle("Residual (x^{reco}-x^{mc}) [um]");
    dx->GetXaxis()->SetTitleSize(0.04);

      // ----- Residual tx
    TH1D *dtx = new TH1D(" Residualtx"," Residualtx",nbins,-txboundary,txboundary);
    for (int i = 0; i < NgoodTr; i++){
      dtx->Fill( (Par[i][2][0] - Par[i][2][2]) );
    }

    TF1  *dtxfit = new TF1(" Residualtx","gaus");
    dtxfit->SetLineColor(2);
    dtx->Fit(" Residualtx","","",dtx->GetXaxis()->GetXmin(),dtx->GetXaxis()->GetXmax());

    dtx->GetXaxis()->SetTitle("Residual (t_{x}^{reco}-t_{x}^{mc}) [mrad]");
    dtx->GetXaxis()->SetTitleSize(0.04);


    gStyle->SetOptStat(00000000);

    dtx->Write();
    dx->Write();

}

  
