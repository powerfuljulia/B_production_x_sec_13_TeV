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

//input example: create_yield_ratio_vector --ratio fsfu --bins pt
int main(int argc, char** argv)
{
  std::string ratio = "fsfu";
  std::string bins = "pt";

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
  double yield_array[2][n_var2_bins][n_var1_bins];
  double yield_err_lo[2][n_var2_bins][n_var1_bins];
  double yield_err_hi[2][n_var2_bins][n_var1_bins];
  
  double ratio_array[n_var2_bins][n_var1_bins];
  double ratio_err_lo[n_var2_bins][n_var1_bins];
  double ratio_err_hi[n_var2_bins][n_var1_bins];
  
  for(int ch=0; ch<2; ch++)
    {
      int channel = 1;

      if(ratio == "fsfu")
        channel = 3*ch+1; //fs_fu: if ch=0 -> channel=1, if ch=1 -> channel=4
      else
        if(ratio == "fsfd")
          channel = 2*(ch+1); //fs_fd: if ch=0 -> channel=2, if ch=1 -> channel=4
	else
          if(ratio == "fdfu")
            channel= ch+1; //fd_fu: if ch=0 -> channel=1, if ch=1 -> channel=2
	  else
            {
              printf("ERROR: The ratio you asked for is not deffined. Only fsfu, fsfd, fdfu are deffined. Please check in the code.");
              return 0;
            }
      
      //read yields
      read_vector(measure, channel, "yield", var1_name , var2_name, n_var1_bins, n_var2_bins, var1_bins, var2_bins, yield_array[ch][0], yield_err_lo[ch][0], yield_err_hi[ch][0]);
 
    }//enf of ch cicle
  
  //to calculate the yield ratio
  for(int j=0; j<n_var2_bins; j++)
    {
      for(int i=0; i<n_var1_bins; i++)
	{
	  ratio_array[j][i] = yield_array[1][j][i] / yield_array[0][j][i];
          ratio_err_lo[j][i] = ratio_array[j][i] * sqrt( pow(yield_err_lo[0][j][i]/yield_array[0][j][i],2) + pow(yield_err_lo[1][j][i]/yield_array[1][j][i],2) );
          ratio_err_hi[j][i] = ratio_array[j][i] * sqrt( pow(yield_err_hi[0][j][i]/yield_array[0][j][i],2) + pow(yield_err_hi[1][j][i]/yield_array[1][j][i],2) );
	}
    }
  
  TString ratio_str = "";

  if(ratio == "fsfu")
    ratio_str = "BsBu";
  else
    if(ratio == "fsfd")
      ratio_str = "BsBd";
    else
      if(ratio == "fdfu")
	ratio_str = "BdBu";
  
  //to save to file
  for(int j=0; j<n_var2_bins; j++)
    {
      TVectorD out_val(n_var1_bins);
      TVectorD out_err_lo(n_var1_bins);
      TVectorD out_err_hi(n_var1_bins);

      for(int i=0; i<n_var1_bins; i++)
        {
          out_val[i] = ratio_array[j][i];
          out_err_lo[i] = ratio_err_lo[j][i];
          out_err_hi[i] = ratio_err_hi[j][i];
        }

      TString bins_str = "";
      TString out_file_name = "";

      if(var1_name == "pt")
        bins_str = TString::Format("%.2f_to_%.2f", var2_bins[j], var2_bins[j+1]);
      else
        bins_str = TString::Format("%d_to_%d", (int)var2_bins[j], (int)var2_bins[j+1]);

      out_file_name = "ratio/" + ratio_str + "_vector_" + var1_name + "_bins_" + var2_name + "_from_" + bins_str + "_" + TString::Format(VERSION) + ".root";

      if(bins == "full")
        out_file_name = "ratio/" + ratio_str + "_vector_full_bins_" + TString::Format(VERSION) + ".root";

      TFile* fout = new TFile(out_file_name,"recreate");
     
      out_val.Write("val");
      out_err_lo.Write("err_lo");
      out_err_hi.Write("err_hi");

      fout->Close();
      delete fout;
    }

  //print at the end
  print_table("YIELD RATIO", n_var1_bins, n_var2_bins, var1_name, var2_name, var1_bins, var2_bins, ratio_array[0], ratio_err_lo[0], ratio_err_hi[0]);

}//end