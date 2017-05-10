#include "UserCode/B_production_x_sec_13_TeV/interface/functions.h"

//-----------------------------------------------------------------
// Definition of channel #
// channel = 1: B+ -> J/psi K+
// channel = 2: B0 -> J/psi K*
// channel = 3: B0 -> J/psi Ks
// channel = 4: Bs -> J/psi phi
// channel = 5: Jpsi + pipi
// channel = 6: Lambda_b -> Jpsi + Lambda
//-----------------------------------------------------------------

//input example: plot_ratio --ratio fs_fu --bins pt_y --eff 1 --syst 0 --poly 1
int main(int argc, char** argv)
{
  std::string ratio = "fs_fu";
  std::string bins = "pt";
  int poly = 1;
  int eff = 1;
  int syst = 0;

  for(int i=1 ; i<argc ; ++i)
    {
      std::string argument = argv[i];
      std::stringstream convert;

      if(argument == "--ratio")
	{
	  convert << argv[++i];
	  convert >> ratio;
	}
      if(argument == "--bins")
	{
	  convert << argv[++i];
	  convert >> bins;
	}
      if(argument == "--eff")
	{
	  convert << argv[++i];
	  convert >> eff;
	}
      if(argument == "--syst")
	{
	  convert << argv[++i];
	  convert >> syst;
	}
      if(argument == "--poly")
	{
	  convert << argv[++i];
	  convert >> poly;
	}
    }
  
  //to create the directories to save the .png files
  std::vector<std::string> dir_list;  
  dir_list.push_back("ratio");
  create_dir(dir_list);
  
  //set up the vectors
  TString var1_name = "";
  TString var2_name = "";

  int n_var1_bins=1;
  int n_var2_bins=1;

  double* var1_bins = NULL;
  double* var2_bins = NULL;

  TString measure = "ratio";
  
  setup_bins(measure, 0, bins, &var1_name, &n_var1_bins, &var2_name, &n_var2_bins, &var1_bins, &var2_bins);
  
  //initialize arrays for yield, efficiencies, etc 
  double var1_bin_centre[n_var1_bins];
  double var1_bin_centre_lo[n_var1_bins];
  double var1_bin_centre_hi[n_var1_bins];
  
  double total_eff[2][n_var2_bins][n_var1_bins];
  double total_eff_err_lo[2][n_var2_bins][n_var1_bins];
  double total_eff_err_hi[2][n_var2_bins][n_var1_bins];
  
  double ratio_eff[n_var2_bins][n_var1_bins];
  double ratio_eff_err_lo[n_var2_bins][n_var1_bins];
  double ratio_eff_err_hi[n_var2_bins][n_var1_bins];

  double ratio_array[n_var2_bins][n_var1_bins];
  double ratio_err_lo[n_var2_bins][n_var1_bins];
  double ratio_err_hi[n_var2_bins][n_var1_bins];

  double yield_syst[2][n_var2_bins][n_var1_bins];
 
  double ratio_syst_sqrt_lo[n_var2_bins][n_var1_bins];
  double ratio_syst_sqrt_hi[n_var2_bins][n_var1_bins];
  
  double ratio_syst_lo[n_var2_bins][n_var1_bins];
  double ratio_syst_hi[n_var2_bins][n_var1_bins];
    
  double b_fraction[2];
  double b_fraction_err[2];

  TString x_axis_name = "";
  if(var1_name =="pt")
    x_axis_name = "p_{T}(B) [GeV]";
  else
    x_axis_name = "|y|(B)";
  
  //calculate bin centre
  for(int i=0; i<n_var1_bins; i++)
    {
      var1_bin_centre[i] = var1_bins[i] + (var1_bins[i+1]-var1_bins[i])/2;
      var1_bin_centre_lo[i] = var1_bin_centre[i] - var1_bins[i];
      var1_bin_centre_hi[i] = var1_bins[i+1] - var1_bin_centre[i];
    }
  
  TString ratio_str = "";

  if(ratio == "fs_fu")
    ratio_str = "Bs_Bu";
  else
    if(ratio == "fs_fd")
      ratio_str = "Bs_Bd";
    else
      if(ratio == "fd_fu")
            ratio_str = "Bd_Bu";
  
  //read ratios
  TString read_ratio = "";

  if(eff)
    read_ratio = ratio;
  else
    read_ratio = ratio_str;
  
  read_vector(measure, 0, read_ratio, var1_name , var2_name, n_var1_bins, n_var2_bins, var1_bins, var2_bins, ratio_array[0], ratio_err_lo[0], ratio_err_hi[0]);

  for(int ch=0; ch<2; ch++)
    {
      int channel = 1;

      if(ratio == "fs_fu")
        channel = 3*ch+1; //fs_fu: if ch=0 -> channel=1, if ch=1 -> channel=4
      else
        if(ratio == "fs_fd")
          channel = 2*(ch+1); //fs_fd: if ch=0 -> channel=2, if ch=1 -> channel=4
	else
          if(ratio == "fd_fu")
            channel= ch+1; //fd_fu: if ch=0 -> channel=1, if ch=1 -> channel=2
	  else
            {
              printf("ERROR: The ratio you asked for is not deffined. Only fs_fu, fs_fd, fd_fu are deffined. Please check in the code.");
              return 0;
            }
      
      RooRealVar* branch = branching_fraction(channel);
      b_fraction[ch] = branch->getVal();
      b_fraction_err[ch] = branch->getError();

      //read efficiency
      if(eff)
	{
	  read_vector(measure, channel, "totaleff", var1_name , var2_name, n_var1_bins, n_var2_bins, var1_bins, var2_bins, total_eff[ch][0], total_eff_err_lo[ch][0], total_eff_err_hi[ch][0]);
	}
  
      //read syst
      if(syst)
	{
	  //read_vector(measure, channel, "totaleff", var1_name , var2_name, n_var1_bins, n_var2_bins, var1_bins, var2_bins, total_eff[0], total_eff_err_lo[0], total_eff_err_hi[0]);
	}
  
      //add all the systematics
      for(int j=0; j<n_var2_bins; j++)
	{
	  for(int i=0; i<n_var1_bins; i++)
	    {
	      //syst set to zero for now
	      yield_syst[ch][j][i]=0;
	    }
	}
  }//enf of ch cicle
  
  if(eff)
    {
      //calculate and plot ratioeff
      for(int j=0; j<n_var2_bins; j++)
	{
	  for(int i=0; i<n_var1_bins; i++)
	    {
	      ratio_eff[j][i] = total_eff[0][j][i] / total_eff[1][j][i];
	      ratio_eff_err_lo[j][i] = ratio_eff[j][i] * sqrt(pow(total_eff_err_lo[0][j][i]/total_eff[0][j][i],2) + pow(total_eff_err_lo[1][j][i]/total_eff[1][j][i],2));
	      ratio_eff_err_hi[j][i] = ratio_eff[j][i] * sqrt(pow(total_eff_err_hi[0][j][i]/total_eff[0][j][i],2) + pow(total_eff_err_hi[1][j][i]/total_eff[1][j][i],2));
	    }
	  plot_eff(measure, "ratioeff", 0, n_var1_bins, var2_name, var2_bins[j], var2_bins[j+1], x_axis_name, ratio, var1_bin_centre, var1_bin_centre_lo, var1_bin_centre_hi, ratio_eff[j], ratio_eff_err_lo[j], ratio_eff_err_hi[j]);
	}
    }
  
  //to calculate the ratio
  for(int j=0; j<n_var2_bins; j++)
    {
      for(int i=0; i<n_var1_bins; i++)
	{
	  ratio_array[j][i]  *= pow(10,j);
	  ratio_err_lo[j][i] *= pow(10,j);
	  ratio_err_hi[j][i] *= pow(10,j);

	  ratio_syst_sqrt_lo[j][i] = pow(yield_syst[0][j][i],2) + pow(yield_syst[1][j][i],2);
          ratio_syst_sqrt_hi[j][i] = ratio_syst_sqrt_lo[j][i];

          if(eff)
            {
	      ratio_syst_sqrt_lo[j][i]  += pow(ratio_eff_err_lo[j][i]/ratio_eff[j][i],2) + pow(b_fraction_err[0]/b_fraction[0],2) + pow(b_fraction_err[1]/b_fraction[1],2);
              ratio_syst_sqrt_hi[j][i] += pow(ratio_eff_err_hi[j][i]/ratio_eff[j][i],2) + pow(b_fraction_err[0]/b_fraction[0],2) + pow(b_fraction_err[1]/b_fraction[1],2);
            }

          ratio_syst_lo[j][i]  = ratio_array[j][i] * sqrt(ratio_syst_sqrt_lo[j][i]);
          ratio_syst_hi[j][i]  = ratio_array[j][i] * sqrt(ratio_syst_sqrt_hi[j][i]);
	}
    }

  //plot the ratio
  TCanvas cz;
  
  double leg_x2 = 0.98;
  double leg_x1 = leg_x2 - 0.25;
  double leg_y2 = 0.98;
  double leg_y1 = leg_y2 - (0.05 * n_var2_bins);

  TLegend *leg = new TLegend (leg_x1, leg_y1, leg_x2, leg_y2);
  
  TLatex * tex = new TLatex(0.69,0.91,TString::Format("%.2f fb^{-1} (13 TeV)",LUMINOSITY));
  tex->SetNDC(kTRUE);
  tex->SetLineWidth(2);
  tex->SetTextSize(0.04);
  tex->Draw();
  tex = new TLatex(0.11,0.91,"CMS Preliminary");
  tex->SetNDC(kTRUE);
  tex->SetTextFont(42);
  tex->SetTextSize(0.04);
  tex->SetLineWidth(2);
  tex->Draw();
      
  //find ratio max and min
  double ratio_max = ratio_array[n_var2_bins-1][0] + ratio_syst_hi[n_var2_bins-1][0];
  double ratio_min = ratio_array[0][n_var1_bins-1] - ratio_syst_lo[0][n_var1_bins-1];

  for(int i=0; i<n_var1_bins; i++)
    {
      if((ratio_array[n_var2_bins-1][i] + ratio_syst_hi[n_var2_bins-1][i] ) > ratio_max)
	ratio_max = ratio_array[n_var2_bins-1][i] + ratio_syst_hi[n_var2_bins-1][i];
    }

  for(int i=0; i<n_var1_bins; i++)
    {
      if((ratio_array[0][i] - ratio_syst_lo[0][i]) < ratio_min)
	ratio_min = ratio_array[0][i] - ratio_syst_lo[0][i];
    }

  for(int j=0; j<n_var2_bins; j++)
    {
      TGraphAsymmErrors* graph = new TGraphAsymmErrors(n_var1_bins, var1_bin_centre, ratio_array[j], var1_bin_centre_lo, var1_bin_centre_hi, ratio_err_lo[j], ratio_err_hi[j]);

      TString ratio_title = ratio;
      
      if(eff)
        ratio_title += " fragmentation fraction ratio";
      else
        ratio_title += " yield ratio";

      graph->SetTitle(ratio_title);
      
      //draw this for the first var2 bin, or in case there is only one bin.
      if(j==0) 
	{
	  graph->GetXaxis()->SetTitle(x_axis_name);
	  	  
	  //to set the range of the plot, it takes the min and max value of ratio.
	  if(n_var2_bins > 1)
            graph->GetYaxis()->SetRangeUser(0.1*ratio_min, 10*ratio_max);
          else
	    {
	      graph->GetYaxis()->SetRangeUser(0.5*ratio_min, 2*ratio_max);
	      
	      if(poly)
                {
                  //fit the ratio with a polynomial function
		  graph->Fit("pol1","W","");
                  graph->GetFunction("pol1")->SetLineColor(4);
                  gStyle->SetOptStat(1111);
                  gStyle->SetOptFit(11); //show p0 and p1 parameters from the pol1 fit
                  gStyle->SetStatX(0.9);
                  gStyle->SetStatY(0.9);
                }
              cz.Update();
	    }
	  
	  graph->Draw("ap same");
	}
      else //for the rest of the rapidity bins.
	{     
	  graph->Draw("p same");
	}
      
      //print the var2 bin label
      TString label = "";
      
      if(j!=0)
        label += TString::Format("(#times 10^{%d}) ",j);
      
      if(var1_name == "pt")
        label += TString::Format("%.2f",var2_bins[j]) + " < |y| < " + TString::Format("%.2f",var2_bins[j+1]);
      else
        label += TString::Format("%d",(int)var2_bins[j]) + " < pt < " + TString::Format("%d",(int)var2_bins[j+1]);
      
      leg->AddEntry(graph, label, "lp");
      
      //systematic errors
      if(eff)
        {
	  TGraphAsymmErrors* graph_syst = new TGraphAsymmErrors(n_var1_bins, var1_bin_centre, ratio_array[j], var1_bin_centre_lo, var1_bin_centre_hi, ratio_syst_lo[j], ratio_syst_hi[j]);
	  graph_syst->SetFillColor(j+2);
	  graph_syst->SetFillStyle(3001);
	  graph_syst->Draw("2 same");
	}
    }//end of var2 cicle
  
  leg->Draw("same");
  cz.Update();
  
  if(n_var2_bins > 1)
    cz.SetLogy();
  
  TString systematic = "";

  if(syst)
    systematic = "_syst";

  if(eff)
    cz.SaveAs("ratio/" + ratio + "_" + bins + "_bins" + systematic + "_" + TString::Format(VERSION) + ".png");
  else
    cz.SaveAs("ratio/" + ratio + "_yield_ratio_" + bins + "_bins" + systematic + "_" + TString::Format(VERSION) + ".png");
  
  /////////////////////////////////////////////////////////////
  //To show the values and the errors at the end, like a table/
  /////////////////////////////////////////////////////////////

  //ratio eff
  print_table("EFFICIENCY RATIO", n_var1_bins, n_var2_bins, var1_name, var2_name, var1_bins, var2_bins, ratio_eff[0], ratio_eff_err_lo[0], ratio_eff_err_hi[0]);

  //Fragmentation fraction
  print_table("FRAGMENTATION FRACTION RATIO", n_var1_bins, n_var2_bins, var1_name, var2_name, var1_bins, var2_bins, ratio_array[0], ratio_err_lo[0], ratio_err_hi[0], ratio_syst_lo[0], ratio_syst_hi[0]);
  
}//end