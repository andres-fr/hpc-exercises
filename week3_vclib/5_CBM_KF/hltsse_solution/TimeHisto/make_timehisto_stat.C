#include <iostream>
#include <fstream>
#include <cmath>

const int maxNAll = 50;  // max number of threads set to run
const int maxNstat = 100;  // max number of repeats
int Nstat; // number of repeats
// const char* inputFile = "make_tbbtime.dat";
// const char* outputFile = "make_tbbtime.root";
// const char* inputFile = "make_tbbtime_1auto_fit10_re100.dat";
// const char* outputFile = "make_tbbtime_1auto_fit10_re100.root";
// const char* inputFile = "make_tbbtime_new_412_fit10_re100.dat";
// const char* outputFile = "make_tbbtime_new_412_fit10_re100.root";
// const char* inputFile = "make_tbbtime_affi_412_fit10_re100.dat";
// const char* outputFile = "make_tbbtime_affi_412_fit10_re100.root";
// const char* inputFile = "make_tbbtime_affi_23_fit10_re100.dat";
// const char* outputFile = "make_tbbtime_affi_23_fit10_re100.root";
// const char* inputFile = "make_tbbtime_affi_24_fit10_re100.dat";
// const char* outputFile = "make_tbbtime_affi_24_fit10_re100.root";
// const char* inputFile = "make_tbbtime_affi_fixthr_fit10_re100.dat";
// const char* outputFile = "make_tbbtime_affi_fixthr_fit10_re100.root";
// const char* inputFile = "make_tbbtime_new_fixthr_fit10_re100.dat";
// const char* outputFile = "make_tbbtime_new_fixthr_fit10_re100.root";
// const char* inputFile = "make_tbbtime_affi_old_fit10_re100.dat";
// const char* outputFile = "make_tbbtime_affi_old_fit10_re100.root";
// const char* inputFile = "make_tbbtime_affi_fit10_re100.dat";
// const char* outputFile = "make_tbbtime_affi_fit10_re100.root";
// const char* inputFile = "make_tbbtime_new_HT104_fit10_re100.dat";
// const char* outputFile = "make_tbbtime_new_HT104_fit10_re100.root";
// const char* inputFile = "make_tbbtime_new_fixthr_bycores_fit10_re100.dat";
// const char* outputFile = "make_tbbtime_new_fixthr_bycores_fit10_re100.root";
// const char* inputFile = "make_tbbtime_new_HT104_bycores_fit10_re100.dat";
// const char* outputFile = "make_tbbtime_new_HT104_bycores_fit10_re100.root";
// const char* inputFile = "make_tbbtime_new_fixthr_bycoresx10_fit10_re100.dat";
// const char* outputFile = "make_tbbtime_new_fixthr_bycoresx10_fit10_re100.root";
// const char* inputFile = "make_tbbtime_new_fixthr_bycoresx100_fit10_re100.dat";
// const char* outputFile = "make_tbbtime_new_fixthr_bycoresx100_fit10_re100.root";
// const char* inputFile = "make_tbbtime_new_zaderzhka_bycoresx1_1fit_1000re.dat";
// const char* outputFile = "make_tbbtime_new_zaderzhka_bycoresx1_1fit_1000re.root";
// const char* inputFile = "make_tbbtime_new_zaderzhka_bycoresx10_1fit_1000re.dat";
// const char* outputFile = "make_tbbtime_new_zaderzhka_bycoresx10_1fit_1000re.root";
// const char* inputFile = "make_tbbtime_new_zaderzhka_bycoresx100_1fit_1000re.dat";
// const char* outputFile = "make_tbbtime_new_zaderzhka_bycoresx100_1fit_1000re.root";
// const char* inputFile = "make_tbbtime_new_bycoresx1_1fit_1000re.dat";
// const char* outputFile = "make_tbbtime_new_bycoresx1_1fit_1000re.root";
// const char* inputFile = "make_tbbtime_new_bycoresx10_1fit_1000re.dat";
// const char* outputFile = "make_tbbtime_new_bycoresx10_1fit_1000re.root";
// const char* inputFile = "make_tbbtime_new_bycoresx100_1fit_1000re.dat";
// const char* outputFile = "make_tbbtime_new_bycoresx100_1fit_1000re.root";

const char* inputFile = "make_tbbtime_new_zaderzhka_bycoresx1_1fit_1000re_2.dat";
const char* outputFile = "make_tbbtime_new_zaderzhka_bycoresx1_1fit_1000re_2.root";
    
// const char* inputFile = "make_omptime.dat";
// const char* outputFile = "make_omptime.root";
// const char* inputFile = "make_omptime_fit10_re100.dat";
// const char* outputFile = "make_omptime_fit10_re100.root";

struct TStatData{
  float stat[maxNstat];
  int Nstat;
  double mean;
  double error;   // mean-square error

  
  void FCalculate();

  void FConvRes();
};

void TStatData::FCalculate()
{
  mean = 0;
  int Nstat_temp = Nstat;
  for (int i = 0; i < Nstat_temp; i++){
    if (stat[i] > 0)
      mean += stat[i];
    else
      Nstat--;
  }
  mean /= Nstat;

  error = 0;
  for (int i = 0; i < Nstat_temp; i++){
    if (stat[i] > 0) error += (mean - stat[i])*(mean - stat[i]);
  }
  error /= Nstat * (Nstat - 1);
  error = sqrt(error);
}

void TStatData::FConvRes()
{
  mean = 1/mean;
  error = error*mean*mean;
};

struct TOneTbbTest{
  TStatData cpuTime;
  TStatData realTime;

  void FCalcAll() { cpuTime.FCalculate(); realTime.FCalculate(); };
  void FConvAllRes() { cpuTime.FConvRes(); realTime.FConvRes(); };
};

void make_timehisto_stat_complex()
{

    // read data
  ifstream iff(inputFile);
  int Nall; // number of threads sets to run
  iff >> Nall >> Nstat;
  cout << "Begin read " << Nall << " threads sets data. With static: " << Nstat << endl;
  float nTread,prepTime,trCpuTime,trRealTime,allCpuTime,allRealTime;
  TString sTemp,sTemp2;
    // skip single and singleVc
  {  
    for (int k = 0; k < 3+8; k++){
      iff >> sTemp;
    }
    iff >> prepTime >> trCpuTime >> trRealTime >> allCpuTime >> allRealTime;
    for (int k = 0; k < 3+8; k++){
      iff >> sTemp;
    }
    iff >> prepTime >> trCpuTime >> trRealTime >> allCpuTime >> allRealTime;
    cout << " singleVc = " << prepTime << " " << trCpuTime << " " << trRealTime << " " << allCpuTime << " " << allRealTime << endl;
  }
  for (int k = 0; k < 3; k++){
    iff >> sTemp;
  }

  TOneTbbTest timeData[maxNAll];
  for (int i=0; i < maxNAll; i++){
    for (int j=0; j<Nstat;  j++){
      timeData[i].cpuTime.stat[j] = 0;
      timeData[i].realTime.stat[j] = 0;
    }
    timeData[i].realTime.Nstat = Nstat;
    timeData[i].cpuTime.Nstat = Nstat;
  }
  bool flag = 1;
  for (int i=0; (i < Nall) && (!iff.eof()); i++){
    for (int j=0; j<Nstat;  j++){
      if (flag){
        iff >> nTread;
        if (nTread != i+1){
          cout << "Wrong data file" << endl;
          break;
        }
      }
      
      if (flag) iff >> sTemp;
      iff >> sTemp2;
      cout << sTemp << sTemp2 << " -- " << endl;
      if ( ( nTread == atoi(sTemp2) ) || ( nTread+1 == atoi(sTemp2) ) ){ // void point
        flag = 0;
        cout << nTread << " " << j  << ":" << endl;
        nTread = atoi(sTemp2);
        continue; 
      }
      
      for (int k = 0; k < 1+8-2; k++){
        iff >> sTemp;
      }

      
      iff >> prepTime >> timeData[i].cpuTime.stat[j] >> timeData[i].realTime.stat[j] >> allCpuTime >> allRealTime;
      cout << nTread << " " << j  << ":"<< timeData[i].cpuTime.stat[j] << " " << timeData[i].realTime.stat[j] << endl;
      flag = 1;
    }
    if (flag && (nTread != i+1)){
      break;
    }
    timeData[i].FCalcAll();
    timeData[i].realTime.FConvRes();
  }


//  trRealTime = 1/timeData[0].realTime.mean; // norm on 1-thread
  trRealTime = trRealTime; // norm on single
  TH1F *hr = new TH1F("Speed up","Speed up",Nall+1,-0.5,Nall+0.5);
  hr->SetBinContent(1,0);
  for (int i=0+1; i<Nall; i+=2){  // only 2,4,6, .. end
//   for (int i=0; i<Nall; i++){  // all 1 .. end
    hr->SetBinContent(i+2,timeData[i].realTime.mean*trRealTime);
    hr->SetBinError(i+2, timeData[i].realTime.error*trRealTime);
  }
  hr->GetXaxis()->SetTitle("Number of threads");
//   hr->GetYaxis()->SetTitle("Time / track  [us]");
  hr->GetYaxis()->SetTitle("Speed up");
  hr->GetYaxis()->SetRangeUser(0,10);
  hr->SetMarkerColor(2);
  hr->SetLineColor(2);
  hr->SetLineColor(2);
  hr->SetLineWidth(2);
  TH1F *hc = new TH1F("CPU Time","CPU Time",Nall+1,-0.5,Nall+0.5);
  hc->SetBinContent(1,0);
  for (int i=0; i<Nall; i++){
    hc->SetBinContent(i+2,timeData[i].cpuTime.mean/trCpuTime);
    hc->SetBinError(i+2, timeData[i].cpuTime.error/trCpuTime);
  }
  hc->GetXaxis()->SetTitle("Number of threads");
  hc->GetYaxis()->SetTitle("Cpu time / Single cpu time ");
  hc->SetMarkerColor(2);
  hc->SetLineColor(2);
  hc->SetLineWidth(2);
//   hc->GetYaxis()->SetTitle("track / us");

//  TH1F *hr = new TH1F("Real time","Real time",Nall+2,-1,Nall+1);
//  for (int i=0; i<Nall; i++)
//    hr->SetBinContent(i,arTrRealTime[i]);
//  TH1F *hc = new TH1F("CPU time","CPU time",Nall+2,-1,Nall+1);
//  for (int i=0; i<Nall; i++)
//    hc->SetBinContent(i,arTrCpuTime[i]);

  gStyle->SetCanvasColor(10);
  gStyle->SetFrameFillColor(10);
  gStyle->SetTitleColor(0);
  gStyle->SetHistFillColor(0);
  gStyle->SetOptStat(00000000);
  gStyle->SetPadGridX(1);
  gStyle->SetPadGridY(1);

  TCanvas *c1 = new TCanvas("ITBB", "ITBB", 700, 700);
//   c1->Divide(2);
  c1->Draw();
//   c1->cd(1);
//   hc->Draw();
//   c1->cd(2);
  hr->Draw();
  TFile file(outputFile,"RECREATE");
//  c1->Write();
  hr->Write();
  hc->Write();
}