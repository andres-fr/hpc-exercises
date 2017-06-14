
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

  //#define Debug

using namespace std;

const int Ntr = 20000; // number of tracks
int NgoodTr = Ntr;     // number of tracks with normal parameters

const char* fres_name = "../data/fit.dat";

float chi2[Ntr];

float Par[Ntr][5][3];		// [0] - value;  [1] - error; [2] - MC
TString Par_name[5]={"X","Y","Tx","Ty","qp"};
float Par_max[5][2];  // [0] - max value;  [1] - max (value - MCvalue)

float xboundary = 200.;
float txboundary = 5.;
const int nbins = 100;

void histo_particle(TString nSt=""){

#ifdef Debug 
  cout << nSt<<endl;
#endif
  if(!nSt.IsNull()) 
  {
    TString nSt_temp = nSt;
    nSt = "_";
    nSt+=nSt_temp;
  }

    //        fres_name += nSt.Data();
#ifdef Debug 
  cout << fres_name <<"111111111"<< endl;
#endif
    // 	Style2m();
  { // read results
    FILE * fpres; 
    fpres = fopen(fres_name, "r");
		
    int nskip = 0; // number of skiped particle, cause have some nan value
    int n = -1; // number of particle
#ifdef Debug 
    cout << "sds !!!!!!"<< feof(fpres)<<endl;
#endif
    for (int i = 0; (i < NgoodTr) && !feof(fpres); i++){
#ifdef Debug 
      cout << i << endl;
#endif
      float Tmc[7],T[6],C[15];
      fscanf (fpres, "%d", &(n));
      if (n!=i+Ntr-NgoodTr) cout << "Error read file" << endl;			
      for(int k = 0; k<7; k++) fscanf (fpres, "%f", &(Tmc[k]));
      for(int k = 0; k<6; k++) fscanf (fpres, "%f", &(T[k]));
      for(int k = 0; k<15; k++) fscanf (fpres, "%f", &(C[k]));
        // there Tmc:  x,y,z,px,py,pz,q
      Par[i][0][2] = Tmc[0];
      Par[i][1][2] = Tmc[1];
      if(Tmc[5] ==0 ) continue;
      Par[i][2][2] = Tmc[3]/Tmc[5];
      Par[i][3][2] = Tmc[4]/Tmc[5];
      Par[i][4][2] = Tmc[6]/sqrt(Tmc[3]*Tmc[3]+Tmc[4]*Tmc[4]+Tmc[5]*Tmc[5]);

      for(int k = 0; k<5; k++){
        if (Par_max[k][0] < abs(T[k])) Par_max[k][0] = abs(T[k]);
        if (Par_max[k][1] < abs(Par[i][k][2]-T[k])) Par_max[k][0] = abs(Par[i][k][2]-T[k]);
      }

      for(int k = 0,kk=0; k<5; k++,kk+=k+1){
        Par[i][k][0] = T[k];
          ///C[kk] = fabs(C[kk]);
        if ( C[kk] > 0 ){
          Par[i][k][1] = sqrt(C[kk]);
        }
        else{
          cout << "Error: In event " << i+Ntr-NgoodTr << " er^2 < 0. Event have been skiped." << endl;
          i--;
          NgoodTr--;
          break;
        }
      };

    }
#ifdef Debug 
    cout << "sds !!!!!!"<< feof(fpres)<<endl;
#endif
    if (feof(fpres)) cout << " Data from file " << fres_name << " read succesfully " << endl; 
    fclose(fpres);
  } // read results
	

    // Make histo

    //     TCanvas *c = new TCanvas("asdf","asdf",0,0,400,500);
    //     c ->Divide(2,3);
#ifdef Debug 
  cout << "tut" << endl;
  cout << fres_name <<"111111111"<< endl;
#endif
  TH1D *Pull[5],*X[5],*dX[5];

  TString FRootName;
  if(!nSt.IsNull()) 
  {
    TString name_temp = ".root";
    FRootName = "histo_particle";
    FRootName += nSt;
    FRootName += name_temp;
  }
  else FRootName = "histo_particle.root";
  TFile f(FRootName.Data(),"RECREATE");
#ifdef Debug 
  cout << FRootName << endl;
#endif
  for (int k = 0; k < 5; k++){

      // ----- Pulls
    Pull[k] = new TH1D("Pull "+Par_name[k],"Pull "+Par_name[k],nbins,-6.,6.);
    for (int i = 0; i < NgoodTr; i++){
      if( Par[i][k][1]!= 0 )Pull[k]->Fill( (Par[i][k][0] - Par[i][k][2]) / Par[i][k][1] );
    }

    TF1  *pullfit = new TF1("gausFitPull "+Par_name[k],"gaus");
    pullfit->SetLineColor(2);
    Pull[k]->Fit("gausFitPull "+Par_name[k],"","",Pull[k]->GetXaxis()->GetXmin(),Pull[k]->GetXaxis()->GetXmax());

    Pull[k]->GetXaxis()->SetTitle("Pull "+Par_name[k]);
    Pull[k]->GetXaxis()->SetTitleSize(0.04);

      // ----- Values
    X[k] = new TH1D(Par_name[k],Par_name[k],nbins,-Par_max[k][0],Par_max[k][0]);
    for (int i = 0; i < NgoodTr; i++){
      X[k]->Fill( Par[i][k][0] );
    }

      // ----- Residual
      // 		dX[k] = new TH1D(Par_name[k]+" Residual",Par_name[k]+" Residual",nbins,-Par_max[k][1]/5,Par_max[k][1]/5);
      // 		for (int i = 0; i < NgoodTr; i++){
      // 			dX[k]->Fill( Par[i][k][0] - Par[i][k][2] );
      // 		}
      // 
      //   	TF1  *dXfit = new TF1("Residual "+ Par_name[k],"gaus");
      // 		dXfit->SetLineColor(2);
      //   	dX[k]->Fit("Residual "+Par_name[k],"","",dX[k]->GetXaxis()->GetXmin(),dX[k]->GetXaxis()->GetXmax());
      // 
      // 		dX[k]->GetXaxis()->SetTitle("Residual "+Par_name[k]);
      // 		dX[k]->GetXaxis()->SetTitleSize(0.04);

      //     c->cd(k);
      // 		Pull[k]->Draw("colz");
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
    dx->Fill( (Par[i][0][0] - Par[i][0][2])*10000. ); // from cm to um
  }

  TF1  *dxfit = new TF1(" Residualx","gaus");
  dxfit->SetLineColor(2);
  dx->Fit(" Residualx","","",dx->GetXaxis()->GetXmin(),dx->GetXaxis()->GetXmax());

  dx->GetXaxis()->SetTitle("Residual (x^{reco}-x^{mc}) [um]");
  dx->GetXaxis()->SetTitleSize(0.04);

    // ----- Residual tx
  TH1D *dtx = new TH1D(" Residualtx"," Residualtx",nbins,-txboundary,txboundary);
  for (int i = 0; i < NgoodTr; i++){
    dtx->Fill( (Par[i][2][0] - Par[i][2][2])*1000. );
  }

  TF1  *dtxfit = new TF1(" Residualtx","gaus");
  dtxfit->SetLineColor(2);
  dtx->Fit(" Residualtx","","",dtx->GetXaxis()->GetXmin(),dtx->GetXaxis()->GetXmax());

  dtx->GetXaxis()->SetTitle("Residual (t_{x}^{reco}-t_{x}^{mc}) [mrad]");
  dtx->GetXaxis()->SetTitleSize(0.04);

    // ----- Residual y
  TH1D *dy = new TH1D(" Residualy"," Residualy",nbins,-xboundary,xboundary);
    //  TH1D *dy = new TH1D(Sss,Sss,nbins,-200,200);
  for (int i = 0; i < NgoodTr; i++){
    dy->Fill( (Par[i][1][0] - Par[i][1][2])*10000. ); // from cm to um
  }

  TF1  *dyfit = new TF1(" Residualy","gaus");
  dyfit->SetLineColor(2);
  dy->Fit(" Residualy","","",dy->GetXaxis()->GetXmin(),dy->GetXaxis()->GetXmax());

  dy->GetXaxis()->SetTitle("Residual (y^{reco}-y^{mc}) [um]");
  dy->GetXaxis()->SetTitleSize(0.04);

    // ----- Residual ty
  TH1D *dty = new TH1D(" Residualty"," Residualty",nbins,-txboundary,txboundary);
  for (int i = 0; i < NgoodTr; i++){
    dty->Fill( (Par[i][3][0] - Par[i][3][2])*1000. );
  }

  TF1  *dtyfit = new TF1(" Residualty","gaus");
  dtyfit->SetLineColor(2);
  dty->Fit(" Residualty","","",dty->GetXaxis()->GetXmin(),dty->GetXaxis()->GetXmax());

  dty->GetXaxis()->SetTitle("Residual (t_{y}^{reco}-t_{y}^{mc}) [mrad]");
  dty->GetXaxis()->SetTitleSize(0.04);

  
    // ---- Resolution
  TH1D *resp = new TH1D("p Resolution","p Resolution",nbins,-10.,10.);
  for (int i = 0; i < NgoodTr; i++){
    if (Par[i][4][2] != 0.) resp->Fill( (fabs(Par[i][4][0])/fabs(Par[i][4][2]) - 1.0)*100 );
      // 		cout << ( Par[i][4][0]/Par[i][4][2] - 1.0 ) << endl;
  }


  TF1  *resfit = new TF1("p Resolution","gaus");
  resfit->SetLineColor(2);
  resp->Fit("p Resolution","","",resp->GetXaxis()->GetXmin(),resp->GetXaxis()->GetXmax());

  resp->GetXaxis()->SetTitle("Resolution (p^{reco}-p^{mc})/p^{mc}");
  resp->GetXaxis()->SetTitleSize(0.04);


    // ---- Resolution dependence of p
  double p[Ntr],rez[Ntr]; 
  for (int i = 0; i < NgoodTr; i++){
    if (Par[i][4][2] != 0.){
      rez[i] = fabs(Par[i][4][0] / Par[i][4][2] - 1.0) ;
      p[i] = fabs(Par[i][4][2]);
    }
  }
    //   TGraph *rezGr = new TGraph(NgoodTr,p,rez);
    //   rezGr->GetXaxis()->SetTitle("p\^\{mc\} [GeV/c]");
    //   rezGr->GetYaxis()->SetTitle("Resolution (p\^\{reco\}-p\^\{mc\})/p\^\{mc\}");
    //   rezGr->SetMarkerColor(2);
    //   rezGr->Draw();

  gStyle->SetOptStat(00000000);
  TProfile *rezGr = new TProfile("Momentum resolution","Momentum resolution",100, 0.0, 2.5,0,3.0);
  rezGr->SetBinContent(1,0);
  for (int i=0; i<NgoodTr; i++){
    rezGr->Fill(p[i],rez[i]*100);
      //     cout << p[i] << " " << rez[i] << endl;
  }
  rezGr->GetXaxis()->SetTitle("p^{mc} [GeV/c]");
  rezGr->GetYaxis()->SetTitle("Resolution (p^{reco}-p^{mc})/p^{mc},  %");
  rezGr->GetYaxis()->SetTitleOffset(1.3);
  rezGr->SetMarkerColor(2);
  rezGr->SetLineColor(2);
  rezGr->SetLineWidth(1);


  rezGr->Write();
  resp->Write();
  dtx->Write();
  dx->Write();
  dty->Write();
  dy->Write();
  
}
