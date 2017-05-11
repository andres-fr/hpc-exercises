//#define PRESENTATION

const int NStations = 1;

//#define FILE_OUTPUT

#include "TStopwatch.h"
#include "TStyle.h"
#include "TFile.h"
#include "TH1D.h"
#include "TF1.h"
#include "TCanvas.h"
#include <iostream>
#include <iomanip>


void Pulls(TString nSt="") {

  TStopwatch timer;
  timer.Start();

//  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
//  basiclibs();

  TStyle *plain = new TStyle("Plain","Plain Style(no colors/fill areas)");
  //plain->SetCanvasBorderMode(0);
  //plain->SetPadBorderMode(0);
  plain->SetPadColor(0);
  plain->SetCanvasColor(0);
  plain->SetTitleColor(0);
  plain->SetStatColor(0);
#ifdef PRESENTATION
  plain->SetOptStat(0);
#else
  plain->SetOptStat(110010);
#endif
  plain->SetOptFit(0011);
  plain->SetStatW(0.225);
  plain->SetStatH(0.06);
  //plain->SetStatX(0.7);
  plain->SetOptTitle(0);
  plain->SetFrameBorderMode(0);

  plain->SetPadLeftMargin(0.15);
  plain->SetPadRightMargin(0.05);
  plain->SetPadTopMargin(0.08);
  plain->SetPadBottomMargin(0.13);
  
  plain->cd();

  TString name;
  if(!nSt.IsNull()) 
  {
    TString name_temp = ".root";
    name = "track_param_";
    name += nSt;
    name += name_temp;
  }
  else name = "track_param.root";
  cout << name << endl;
  TFile *dir = new TFile(name.Data(),"read");
  //TDirectory *dir = f->GetDirectory("HLTTPCCATrackerPerformance.root:/HLTTPCCATrackerPerformance/TrackFit");

#ifdef FILE_OUTPUT
  FILE *ress, *pulls;
  ress = fopen("resolution.txt","w");
  pulls = fopen("pull.txt","w");
#endif
  
  const int NParameters = 2;
  
  TH1D *hRes[NParameters];
  TH1D *hPull[NParameters];
  TF1  *fRes[NParameters];
  TF1  *fPull[NParameters];

  TString sRes[NParameters];
  TString sPull[NParameters];
  TString sResFit[NParameters];
  TString sPullFit[NParameters];
  
  TString ParNames[NParameters] = {"X","Tx"};
  TString ParNamesRes[NParameters] = {" Residualx"," Residualtx"};
  // TString ParVal[NParameters] = {" [cm]"," [cm]","",""," [c/Gev]"}; // isn't used

  TString AxisNameRes[NParameters];
  TString AxisNamePull[NParameters];

  AxisNameRes[0] = "Residual (x^{reco} - x^{mc}) [#mum]";

  AxisNameRes[1] = "Residual (t_{x}^{reco} - t_{x}^{mc}) [10^{-3}]";


  AxisNamePull[0] = "Pull x";

  AxisNamePull[1] = "Pull tx";


  TString res = "Residial";
  TString pull = "Pull ";

  float sigmaRes[NStations][NParameters];
  float sigmaPull[NStations][NParameters];
  int nRes[NStations][NParameters]; // n Entries
  int nPull[NStations][NParameters];
  
  for (int is = 0; is < NStations; is++) {
    TCanvas *c0 = new TCanvas("c0","c0",0,0,1200,600);

    // TString Sta_name = "Station";
    // Sta_name += is;
    // Sta_name += ".";
    TString Sta_name = "";

    for(int i=0; i<NParameters; i++)
    {
        //    sRes[i]=res+ParNames[i];
      sRes[i]=Sta_name+ParNamesRes[i];
      sPull[i]=Sta_name+pull+ParNames[i];
      sResFit[i]=sRes[i]+"GaussFit";
      sPullFit[i]=sPull[i]+"GaussFit";
    }

    for(int i=0; i<NParameters; i++)
    {
      hRes[i] = (TH1D*) dir->Get(sRes[i]);
      cout <<sRes[i]<<endl;
      cout <<sResFit[i]<<endl;
      fRes[i] = new TF1(sResFit[i],"gaus");
      fRes[i]->SetLineColor(2);
      hRes[i]->Fit(sResFit[i],"","",hRes[i]->GetXaxis()->GetXmin(),hRes[i]->GetXaxis()->GetXmax());
    
      hPull[i] = (TH1D*) dir->Get(sPull[i]);
      fPull[i] = new TF1(sPullFit[i],"gaus");
      fPull[i]->SetLineColor(2);
      hPull[i]->Fit(sPullFit[i],"","",hPull[i]->GetXaxis()->GetXmin(),hPull[i]->GetXaxis()->GetXmax());
    }

#ifdef FILE_OUTPUT
    for(int i=0; i<NParameters; i++)
    {
      fprintf(ress,"%s	%.3g	%.3g\n", ParNames[i].Data(), fRes[i]->GetParameter(1), fRes[i]->GetParameter(2));
      fprintf(pulls,"%s	%.3g	%.3g\n", ParNames[i].Data(), fPull[i]->GetParameter(1), fPull[i]->GetParameter(2));
    }
#endif
    // delete c0;
    TCanvas *c1;
    c1 = new TCanvas((Sta_name+"Canvas").Data(),(Sta_name+"Canvas").Data(),0,0,1200,600);
      //  c1 -> Divide(NParameters-2,2);
    c1->Divide(NParameters,2);
    c1->UseCurrentStyle();

    int NPad=0;

    for(int i=0; i<NParameters; i++)
    {
        //if(i == 1 || i == 3) continue;
      NPad++;
      c1->cd(NPad);
      hRes[i]->GetXaxis()->SetTitleSize(0.06);
      hRes[i]->GetXaxis()->SetLabelSize(0.07);
      hRes[i]->GetXaxis()->SetNdivisions(5,5,0);
      hRes[i]->GetXaxis()->SetTitleOffset(1.05);
      hRes[i]->GetXaxis()->SetTitle(AxisNameRes[i]);
      hRes[i]->GetYaxis()->SetTitleSize(0.06);
      hRes[i]->GetYaxis()->SetLabelSize(0.07);
      hRes[i]->GetYaxis()->SetNdivisions(3,5,0);
      hRes[i]->Draw();
 //     fRes[i]->Draw("same");
    }
    for(int i=0; i<NParameters; i++)
    {
        //if(i == 1 || i == 3) continue;
      NPad++;
      c1->cd(NPad);
      hPull[i]->GetXaxis()->SetTitleSize(0.06);
      hPull[i]->GetXaxis()->SetLabelSize(0.07);
      hPull[i]->GetXaxis()->SetNdivisions(5,5,0);
      hPull[i]->GetXaxis()->SetTitleOffset(1.05);
      hPull[i]->GetXaxis()->SetTitle(AxisNamePull[i]);
      hPull[i]->GetYaxis()->SetTitleSize(0.06);
      hPull[i]->GetYaxis()->SetLabelSize(0.07);
      hPull[i]->GetYaxis()->SetNdivisions(3,5,0);
      hPull[i]->Draw();
   //   fPull[i]->Draw("same");
    }

    for(int i=0; i<NParameters; i++) {
      sigmaRes[is][i] = fRes[i]->GetParameter(2);
      sigmaPull[is][i] = fPull[i]->GetParameter(2);
      nRes[is][i] = hRes[i]->GetEntries();
      nPull[is][i] = hPull[i]->GetEntries();
    }
    
    c1->SaveAs(Sta_name+"TrackPull.pdf");
    c1->Update();
  }

  cout << "                ------------ SUMMARY ----------- "  << endl;
  cout.setf(ios::fixed);
  cout << "       " << "     ";
  for(int i=0; i<NParameters; i++) {
    cout << " SPar" << i << " ";
  }
  cout << " | ";
  for(int i=0; i<NParameters; i++) {
    cout << "SPull" << i << " ";
  }
  cout << " | ";
  for(int i=NParameters-1; i<NParameters; i++) {
    cout << " NPar" << i << " ";
  }
  cout << " | ";
  for(int i=0; i<NParameters; i++) {
    cout << "NPull" << i << " ";
  }
  cout << endl;
  for(int is=0; is<NStations; is++) {
    cout << " ista = " << is << ":  ";
    for(int i=0; i<NParameters; i++) {
      cout << setprecision(2) << setw(6) << sigmaRes[is][i] << " ";
    }
    cout << " | ";
    for(int i=0; i<NParameters; i++) {
      cout << setprecision(2) << setw(6) << sigmaPull[is][i] << " ";
    }
    cout << " | ";
    for(int i=NParameters-1; i<NParameters; i++) {
      cout << setw(6) << nRes[is][i] << " ";
    }
    cout << " | ";
    for(int i=0; i<NParameters; i++) {
      cout << setw(6) << nPull[is][i] << " ";
    }
    cout << endl;
  }


#ifdef FILE_OUTPUT
  fclose(ress);
  fclose(pulls);
#endif
  
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);

}
