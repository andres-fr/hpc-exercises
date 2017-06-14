void Style()
{
  //////////////////////////////////////////////////////////////////////
  //
  // ROOT style macro for the TDR
  //
  //////////////////////////////////////////////////////////////////////

  gSystem->Load("libPostscript.so");
  gStyle->SetCanvasBorderMode(-1);
  gStyle->SetCanvasBorderSize(1);
  gStyle->SetCanvasColor(10);

  gStyle->SetFrameFillColor(10);
  gStyle->SetFrameBorderSize(1);
  gStyle->SetFrameBorderMode(-1);
  gStyle->SetFrameLineWidth(1.2);
  gStyle->SetFrameLineColor(1);

  gStyle->SetHistFillColor(0);
  gStyle->SetHistLineWidth(2.);
  gStyle->SetHistLineColor(1);

  gStyle->SetPadColor(10);
  gStyle->SetPadBorderSize(1);
  gStyle->SetPadBorderMode(-1);

  gStyle->SetStatColor(10);
  gStyle->SetTitleColor(1);

  gStyle->SetTitleFont(22,"xyz");
  gStyle->SetTitleSize(0.08,"xyz"); // axis titles
  gStyle->SetLabelFont(22,"xyz");
  gStyle->SetLabelFont(22,"xyz");
//   gStyle->SetLabelSize(0.04,"xyz"); 
//   gStyle->SetStatFont(22);
 gStyle->SetLabelOffset(0.015,"xyz");

  gStyle->SetTitleOffset(1.2,"xyz");

  gStyle->SetOptDate(0);
  gStyle->SetOptTitle(0);


//  The parameter mode can be = ksiourmen  (default = 000001111)
//     k = 1;  kurtosis printed
//     k = 2;  kurtosis and kurtosis error printed
//     s = 1;  skewness printed
//     s = 2;  skewness and skewness error printed
//     i = 1;  integral of bins printed
//     o = 1;  number of overflows printed
//     u = 1;  number of underflows printed
//     r = 1;  rms printed
//     r = 2;  rms and rms error printed
//     m = 1;  mean value printed
//     m = 2;  mean and mean error values printed
//     e = 1;  number of entries printed
//     n = 1;  name of histogram is printed
  gStyle->SetOptStat(00000000);

//   The parameter mode can be = pcev  (default = 0111)
//     p = 1;  print Probability
//     c = 1;  print Chisquare/Number of degress of freedom
//     e = 1;  print errors (if e=1, v must be 1)
//     v = 1;  print name/values of parameters
   gStyle->SetOptFit(0011);
//    gStyle->SetStatFontSize(0.03);
	gStyle->SetStatW(0.14);
	gStyle->SetStatH(0.06);

 // Set margins -- I like to shift the plot a little up and to the
// right to make more room for axis labels
// gStyle->SetPadTopMargin(0.08);
// gStyle->SetPadBottomMargin(0.2);
// gStyle->SetPadLeftMargin(0.2);
// gStyle->SetPadRightMargin(0.1);

gStyle->SetNdivisions(505,"xy");


}