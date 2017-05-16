// histo for compare diff dependence of number of threads

#include <iostream>
#include <fstream>
#include <cmath>

const int maxNAll = 50;  // max number of threads set to run
const int maxNstat = 100;  // max number of repeats
int Nstat; // number of repeats
// const char* inputFile = "make_tbbtime.dat";
// const char* outputFile = "make_tbbtime.root";
const int nFiles = 3;
TString inputFile[nFiles];
TString outputFile;
// TString outputFile[nFiles];
TString FileNameTemplate = "make_tbbtime_new_bycores_Ntrthrpart_1fit_1000re";
    
void FileNamesInit(){
  for (int i = 0; i < nFiles; i++){
    inputFile[i] = FileNameTemplate + "_v";
    inputFile[i] += i;
    inputFile[i] += ".dat";
//     outputFile[i] = FileNameTemplate + "_v";
//     outputFile[i] += i;
//     outputFile[i] += ".dat";
  }
  outputFile = FileNameTemplate + "_";
  outputFile += nFiles;
  outputFile += "histo.root";
}

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
  TH1F *hrAll[nFiles];
  FileNamesInit();
  for ( int iFile = 0; iFile < nFiles; iFile++){
//     iFile=1;
      // read data
    cout << " Read data from file " << inputFile[iFile] << endl;
    ifstream iff(inputFile[iFile]);
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
  
  
  // 	trRealTime = 1/timeData[0].realTime.mean; // norm on 1-thread
  //   trRealTime = trRealTime; // norm on single

//     hrAll[iFile] = new TH1F("Real Time","Real Time",Nall+1,-0.5,Nall+0.5);  // all 1 .. end
    hrAll[iFile] = new TH1F("Real Time","Real Time",Nall/2+1,-0.5,Nall/2+0.5);  // only 2,4,6, .. end
    TH1F *hr = hrAll[iFile];
    hr->SetBinContent(1,0);
// //   for (int i=0; i<Nall; i++){  // all 1 .. end
//     hr->SetBinContent(i/2+2,timeData[i].realTime.mean);
//     hr->SetBinError(i/2+2, timeData[i].realTime.error);
//   }
    for (int i=0; i<Nall/2; i++){  // only 2,4,6, .. end
      hr->SetBinContent(i+2,timeData[2*i+1].realTime.mean);
      hr->SetBinError(i+2, timeData[2*i+1].realTime.error);
    }

//     TH1F *hc = new TH1F("CPU Time","CPU Time",Nall+1,-0.5,Nall+0.5);
//     hc->SetBinContent(1,0);
//     for (int i=0; i<Nall; i++){
//       hc->SetBinContent(i+2,timeData[i].cpuTime.mean/trCpuTime);
//       hc->SetBinError(i+2, timeData[i].cpuTime.error/trCpuTime);
//     }
//     hc->GetXaxis()->SetTitle("Number of threads");
//     hc->GetYaxis()->SetTitle("Cpu time / Single cpu time ");
//     hc->SetMarkerColor(2);
//     hc->SetLineColor(2);
//     hc->SetLineWidth(2);
    
  //   hc->GetYaxis()->SetTitle("track / us");
  
    gStyle->SetCanvasColor(10);
    gStyle->SetFrameFillColor(10);
    gStyle->SetTitleColor(0);
    gStyle->SetHistFillColor(0);
    gStyle->SetOptStat(00000000);
    gStyle->SetPadGridX(1);
    gStyle->SetPadGridY(1);
    gStyle->SetOptTitle(0);
    gStyle->SetTextFont(22);
    
  } // iFile

  TCanvas *c1 = new TCanvas("ITBB", "ITBB", 700, 700);
  TLegend *leg1 = new TLegend(0.2-0.03,.62+0.05,0.55-0.03,.75+0.05);
  leg1->SetFillColor(0);
  leg1->SetLineWidth(0);
  leg1->SetBorderSize(0);
  leg1->SetTextFont(22);
//   leg1->SetTextSize(4);

  for (int i = 0; i < nFiles; i++){
    TH1F *hr = hrAll[i];
    
    hr->SetLineColor(0);
    hr->SetLineWidth(0);
//     hr->SetMarkerStyle(22);
//     hr->SetMarkerSize(1.5 +.5*i);
    switch(i){
      case 0: hr->SetMarkerStyle(21); break;
      case 1: hr->SetMarkerStyle(23); break;
      case 2: hr->SetMarkerStyle(22); break;
      default:   hr->SetMarkerStyle(20);
    }
    hr->SetMarkerSize(1.8);
    hr->SetMarkerColor(TColor::GetColorDark(2+i));

    hr->GetYaxis()->SetTitleFont(22);
    hr->GetXaxis()->SetTitleFont(22);
    hr->GetYaxis()->SetLabelFont(22);
    hr->GetXaxis()->SetLabelFont(22);
    hr->GetXaxis()->SetTitle("Number of cores");
    hr->GetYaxis()->SetTitle(Form("Tracks / \\mus"));
    hr->GetYaxis()->SetRangeUser(0,20); // tr/us
    
    if (i) hr->Draw("same"); else hr->Draw();

    leg1->AddEntry( hrAll[nFiles-1-i],Form("10^{%i} tracks/thread",nFiles-1-i+1) );
   }

   leg1->Draw();

// TFile file(outputFile,"RECREATE");
// c1->Write();

}