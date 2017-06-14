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
//  plain->SetOptStat(10);
  plain->SetOptStat(110010);
  plain->SetOptFit(0011);
  plain->SetStatW(0.225);
  plain->SetStatH(0.06);
  //plain->SetStatX(0.7);
  plain->SetOptTitle(0);
  plain->SetFrameBorderMode(0);
  plain->cd();

  TString name;
  if(!nSt.IsNull()) 
  {
    TString name_temp = ".root";
    name = "histo_particle_";
    name += nSt;
    name += name_temp;
  }
  else name = "histo_particle.root";
cout << name << endl;
  TFile *dir = new TFile(name.Data(),"read");
  //TDirectory *dir = f->GetDirectory("HLTTPCCATrackerPerformance.root:/HLTTPCCATrackerPerformance/TrackFit");

  FILE *ress, *pulls;
  ress = fopen("resolution.txt","w");
  pulls = fopen("pull.txt","w");
  
  const int NParameters = 5;
  
  TH1D *hRes[NParameters];
  TH1D *hPull[NParameters];
  TF1  *fRes[NParameters];
  TF1  *fPull[NParameters];

  string sRes[NParameters];
  string sPull[NParameters];
  string sResFit[NParameters];
  string sPullFit[NParameters];
  
  string ParNames[NParameters] = {"X","Y","Tx","Ty","qp"};
  string ParNamesRes[NParameters] = {" Residualx"," Residualy"," Residualtx"," Residualty","p Resolution"};
  string ParVal[NParameters] = {" [cm]"," [cm]","",""," [c/Gev]"};

  string AxisNameRes[NParameters];
  string AxisNamePull[NParameters];

  AxisNameRes[0] = "Residual (x^{reco} - x^{mc}) [#mum]";
  AxisNameRes[1] = "Residual (y^{reco} - y^{mc}) [#mum]";
  AxisNameRes[2] = "Residual (t_{x}^{reco} - t_{x}^{mc}) mrad";
  AxisNameRes[3] = "Residual (t_{y}^{reco} - t_{y}^{mc}) mrad";
  AxisNameRes[4] = "Residual (p^{reco} - p^{mc})/p^{mc}*100%";

  AxisNamePull[0] = "Pull x";
  AxisNamePull[1] = "Pull y";
  AxisNamePull[2] = "Pull tx";
  AxisNamePull[3] = "Pull ty";
  AxisNamePull[4] = "Pull q/p";

  string res = "Residial";
  string pull = "Pull ";
  
  for(int i=0; i<NParameters; i++)
  {
//    sRes[i]=res+ParNames[i];
    sRes[i]=ParNamesRes[i];
    sPull[i]=pull+ParNames[i];
    sResFit[i]=sRes[i]+"GaussFit";
    sPullFit[i]=sPull[i]+"GaussFit";
  }

  for(int i=0; i<NParameters; i++)
  {
    hRes[i] = (TH1D*) dir->Get(sRes[i].data());
cout <<sRes[i].data()<<endl;
cout <<sResFit[i].data()<<endl;
    fRes[i] = new TF1(sResFit[i].data(),"gaus");
    fRes[i]->SetLineColor(2);
    hRes[i]->Fit(sResFit[i].data(),"","",hRes[i]->GetXaxis()->GetXmin(),hRes[i]->GetXaxis()->GetXmax());
    
    hPull[i] = (TH1D*) dir->Get(sPull[i].data());
    fPull[i] = new TF1(sPullFit[i].data(),"gaus");
    fPull[i]->SetLineColor(2);
    hPull[i]->Fit(sPullFit[i].data(),"","",hPull[i]->GetXaxis()->GetXmin(),hPull[i]->GetXaxis()->GetXmax());
  }
  for(int i=0; i<NParameters; i++)
  {
    fprintf(ress,"%s	%.3g	%.3g\n", ParNames[i], fRes[i]->GetParameter(1), fRes[i]->GetParameter(2));
    fprintf(pulls,"%s	%.3g	%.3g\n", ParNames[i], fPull[i]->GetParameter(1), fPull[i]->GetParameter(2));
  }

  fclose(ress);
  fclose(pulls);

  TCanvas *c1;
  c1 = new TCanvas("c1","bx",0,0,1200,800);
//  c1 -> Divide(NParameters-2,2);
  c1 -> Divide(NParameters,2);

  c1->UseCurrentStyle();

  int NPad=0;

  for(int i=0; i<NParameters; i++)
  {
//if(i == 1 || i == 3) continue;
    NPad++;
    c1->cd(NPad);
    hRes[i]->GetXaxis()->SetTitle(AxisNameRes[i].data());
    hRes[i]->GetXaxis()->SetTitleOffset(1);
    hRes[i]->Draw();
    fRes[i]->Draw("same");
  }
  for(int i=0; i<NParameters; i++)
  {
//if(i == 1 || i == 3) continue;
    NPad++;
    c1->cd(NPad);
    hPull[i]->GetXaxis()->SetTitle(AxisNamePull[i].data());
    hPull[i]->GetXaxis()->SetTitleOffset(1);
    hPull[i]->Draw();
    fPull[i]->Draw("same");
  }

  c1->SaveAs("KFTrackPull.pdf");
  c1->SaveAs("KFTrackPull.png");

 
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);

}
