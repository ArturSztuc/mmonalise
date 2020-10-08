#include "plot.h"




Plot::Plot(TTree* _treeIn, std::vector< ACNET* > _pars){
  pars = _pars;
  tree = _treeIn;
}

void Plot::SetPlots(){
  // Set the time and 1D histogram plots
  std::cout << "NPARS: " << pars.size() << std::endl;
  for(int par = 0; par < int(pars.size()); ++par){
    histPlots.push_back(new Hist1D(pars[par]));
    timePlots.push_back(new TimePlot(pars[par]));
  }
  histPlots.push_back(new Hist1D(pars[mapKPar(k_mm1cor_cal)], pars[0], 0, 2));
  histPlots.push_back(new Hist1D(pars[mapKPar(k_mm2cor_cal)], pars[0], 0, 10));
  histPlots.push_back(new Hist1D(pars[mapKPar(k_mm3cor_cal)], pars[0], 0.3, 1));

  timePlots.push_back(new TimePlot(pars[mapKPar(k_mm1cor_cal)], pars[0], 0, 2));
  timePlots.push_back(new TimePlot(pars[mapKPar(k_mm2cor_cal)], pars[0], 0, 10));
  timePlots.push_back(new TimePlot(pars[mapKPar(k_mm3cor_cal)], pars[0], 0.3, 1));
}

void Plot::fillRAM(){
  // Get the number of events and number of parameters
  int evs = tree->GetEntries();
  int npars = pars.size();

  //Initialise the data holders
  d_vals = new double*[npars](); 
  d_vals6 = new double**[npars](); 
  d_vals81 = new double**[npars](); 
  d_times = new Long64_t*[npars](); 
  d_time = new Long64_t[evs];
  d_cuts= new bool[evs];
  // Foe every parameter
  for(int par = 0; par < npars; ++par){
    d_vals[par] = new double[evs];
    d_times[par] = new Long64_t[evs];
    d_vals6[par] = new double*[evs]();
    d_vals81[par] = new double*[evs]();
    // And for every event, when more than 1 subevent per event
    for(int ev = 0; ev < evs; ++ev){
      d_vals6[par][ev] = new double[6];
      d_vals81[par][ev] = new double[81];
    }
  }

  // Iterate fill holders with ram
  // Set Branches
  for(int par = 0; par < npars; ++par){
    // Set the branches for both input ttrees and output ttree
    // Notice that e.g. k_vptht variable reads 6 values per bunch, but we only save one (a sum)
    if(pars[par]->leaves== 6){
        tree->SetBranchAddress((pars[par]->name).c_str(), &vals6[par]);
    }
    else if(pars[par]->leaves== 81){
        tree->SetBranchAddress((pars[par]->name).c_str(), &vals81[par]);
    }
    else{
        tree->SetBranchAddress((pars[par]->name).c_str(), &vals[par]);
    }
    // Save the times too, at least for now...
    //tree->Branch((levelX_to_str(par) + "_time").c_str(), &times[par]);
  } 
  tree->SetBranchAddress("time", &time);

  for(int event = 0; event < evs; ++event){
    // Each parameter separately
    tree->GetEntry(event);
    d_time[event] = time;
    d_cuts[event] = true;
    for(int par = 0; par < npars; ++par){
      //inFileVec[par]->cd();

      d_times[par][event] = times[par];

      if(pars[par]->leaves == 6){
        for(int i = 0; i < 6; ++i){
          d_vals6[par][event][i] = vals6[par][i];
        }
      } 
      else if(pars[par]->leaves == 81){
        for(int i = 0; i < 81; ++i){
          d_vals81[par][event][i] = vals81[par][i];
        }
      }
      else{
        d_vals[par][event] = vals[par];
      }
    }
  }
}

void Plot::setTTree(TTree *treeIn){
  //delete tree;
  tree = treeIn;
}

// Clears the RAM
void Plot::clearRAM(){
  int evs = tree->GetEntries();
  int npars = pars.size();
  for(int par = 0; par < npars; ++par){
    for(int ev = 0; ev < evs; ++ev){
      delete d_vals6[par][ev];
      delete d_vals81[par][ev];
    }
    delete d_vals6[par];
    delete d_vals81[par];
    delete d_vals[par];
    delete d_times[par];
  }
  delete d_time;
  delete d_cuts;
}

void Plot::FillPlots(){
  FillCuts();
  for(int plot = 0; plot < int(histPlots.size()); ++plot){
    FillHist1D(histPlots[plot]);
  }
  for(int plot = 0; plot < int(timePlots.size()); ++plot){
    FillTimePlot(timePlots[plot]);
  }

  for(int plot = 0; plot < int(dispPlots.size()); ++plot){
    FillHist2D(dispPlots[plot]);
  }

  for(int plot = 0; plot < int(ratiPlots.size()); ++plot){
    FillHist2D(ratiPlots[plot]);
  }
}

void Plot::DrawPlots(){
  for(int plot = 0; plot < int(histPlots.size()); ++plot){
    histPlots[plot]->c->cd();
    histPlots[plot]->plot->Draw();
  }
  gStyle->SetOptStat(0);
  for(int plot = 0; plot < int(timePlots.size()); ++plot){
    timePlots[plot]->c->cd();
    timePlots[plot]->plot->Draw("ap");
  }
  for(int plot = 0; plot < int(dispPlots.size()); ++plot){
    dispPlots[plot]->c->cd();
    dispPlots[plot]->plot->Draw("col");
  }
  for(int plot = 0; plot < int(ratiPlots.size()); ++plot){
    ratiPlots[plot]->c->cd();
    ratiPlots[plot]->plot->Draw("col");
  }
}

void Plot::SavePlots(){
  // Save 1D histograms
  for(int plot = 0; plot < int(histPlots.size()); ++plot){
    histPlots[plot]->c->cd();
    std::string norm = "";
    if (histPlots[plot]->norm) norm = (histPlots[plot]->par2->name).c_str();
    std::string name = ("hist1D_" + histPlots[plot]->par->name + norm).c_str();
    histPlots[plot]->c->SaveAs((name + ".png").c_str());
    histPlots[plot]->c->SaveAs((name + ".pdf").c_str());
    histPlots[plot]->c->SaveAs((name + ".root").c_str());
  }

  for(int plot = 0; plot < int(timePlots.size()); ++plot){
    timePlots[plot]->c->cd();
    std::string norm = "";
    if (timePlots[plot]->norm) norm = (timePlots[plot]->par2->name).c_str();
    std::string name = ("time_" + timePlots[plot]->par->name + norm).c_str();
    timePlots[plot]->c->SaveAs((name + ".png").c_str());
    timePlots[plot]->c->SaveAs((name + ".pdf").c_str());
    timePlots[plot]->c->SaveAs((name + ".root").c_str());
  }

  for(int plot = 0; plot < int(dispPlots.size()); ++plot){
    dispPlots[plot]->c->cd();
    std::string normx = "";
    std::string normy = "";
    if (dispPlots[plot]->normx) normx = (dispPlots[plot]->par_norm->name).c_str();
    if (dispPlots[plot]->normy) normy = (dispPlots[plot]->par2_norm->name).c_str();
    std::string name = ("disp_" + dispPlots[plot]->par->name + normx+ "_" + dispPlots[plot]->par2->name + normy).c_str();
    dispPlots[plot]->c->SaveAs((name + ".png").c_str());
    dispPlots[plot]->c->SaveAs((name + ".pdf").c_str());
    dispPlots[plot]->c->SaveAs((name + ".root").c_str());
  }

  for(int plot = 0; plot < int(ratiPlots.size()); ++plot){
    ratiPlots[plot]->c->cd();
    std::string normx = "";
    std::string normy = "";
    if (ratiPlots[plot]->normx) normx = (ratiPlots[plot]->par_norm->name).c_str();
    if (ratiPlots[plot]->normy) normy = (ratiPlots[plot]->par2_norm->name).c_str();
    std::string name = ("ratio_" + ratiPlots[plot]->par->name + normx+ "_" + ratiPlots[plot]->par2->name + normy).c_str();
    ratiPlots[plot]->c->SaveAs((name + ".png").c_str());
    ratiPlots[plot]->c->SaveAs((name + ".pdf").c_str());
    ratiPlots[plot]->c->SaveAs((name + ".root").c_str());
  }
}

void Plot::FillHist2D(Hist2D* plot){
  int evs = tree->GetEntries();
  double ratx = 1;
  double raty = 1;
  double ratx_n = 1;
  double raty_n = 1;
  if(plot->ratio == true){
    ratx = GetEvent(plot->par, 0);
    raty = GetEvent(plot->par2, 0);
    if(plot->normx)
      ratx_n = GetEvent(plot->par_norm, 0);
    if(plot->normy)
      raty_n = GetEvent(plot->par2_norm, 0);
  }

  for(int event = 0; event < evs; ++event){
    if(d_cuts[event] == false)
      continue;
    double norm = 1;
    double norm2 = 1;

    if(plot->normx)
      norm = GetEvent(plot->par_norm, event);
    if(plot->normy)
      norm2 = GetEvent(plot->par2_norm, event);
    plot->plot->Fill((GetEvent(plot->par, event)/ratx)/(norm/ratx_n),
                     (GetEvent(plot->par2, event)/raty)/(norm2/raty_n));
  }
}

void Plot::FillHist1D(Hist1D* plot){
  int evs = tree->GetEntries();
  for(int event = 0; event < evs; ++event){
    if(d_cuts[event] == false)
      continue;
    double norm = 1;
    if(plot->norm)
      norm = GetEvent(plot->par2, event);
    double fpar = GetEvent(plot->par, event);
    plot->plot->Fill(fpar/norm);
  }
}

//void Plot::FillHist1D(Hist1D* plot){
//  int evs = tree->GetEntries();
//  int id1 = mapKPar(plot->par->idx);
//  int id2 = -1;
//  if(plot->norm)
//    id2 = mapKPar(plot->par2->idx);
//
//  for(int event = 0; event < evs; ++event){
//    double norm = 1;
//    if(plot->norm)
//      norm = d_vals[id2][event];
//    if(plot->par->leaves == 1)
//      plot->plot->Fill(d_vals[id1][event]/norm);
//  }
//}
//
void Plot::FillTimePlot(TimePlot* plot){
  int evs = tree->GetEntries();

  for(int event = 0; event < evs; ++event){
    if(d_cuts[event] == false)
      continue;
    TDatime time_cor;
    Long64_t time_;
    time_cor.Set(d_time[event]/1000);
    time_ = time_cor.Convert();

    if(plot->plot->GetN() <= plot->timeCounter -1)
      plot->plot->Expand(plot->plot->GetN() + 3000);

    double norm = 1;
    if(plot->norm)
      norm = GetEvent(plot->par2, event);
    plot->plot->SetPoint(plot->timeCounter, time_, GetEvent(plot->par, event)/norm);
    plot->timeCounter++;
  }
}

//void Plot::FillTimePlot(TimePlot* plot){
//  int evs = tree->GetEntries();
//  int id1 = mapKPar(plot->par->idx);
//  int id2 = -1;
//  if(plot->norm)
//    id2 = mapKPar(plot->par2->idx);
//
//  for(int event = 0; event < evs; ++event){
//    TDatime time_cor;
//    Long64_t time_;
//    time_cor.Set(d_time[event]/1000);
//    time_ = time_cor.Convert();
//
//    if(plot->plot->GetN() <= plot->timeCounter -1)
//      plot->plot->Expand(plot->plot->GetN() + 3000);
//
//    double norm = 1;
//    if(plot->norm)
//      norm = d_vals[id2][event];
//    plot->plot->SetPoint(plot->timeCounter, time_, d_vals[id1][event]/norm);
//    plot->timeCounter++;
//  }
//}

int Plot::mapKPar(int k_par){
  int npars = pars.size();
  int parr = -999;
  for(int par = 0; par < npars; ++par){
    if(pars[par]->idx == k_par){
      parr = par;
    }
  }
  return parr;
}

double Plot::GetEvent(ACNET* par, int event){
  int leaves = par->leaves;
  double val = 0;
  int idx = 0;

  std::vector<ACNET*>::iterator itr = std::find(pars.begin(), pars.end(), par);
  if( itr!= pars.cend())
    idx = std::distance(pars.begin(), itr);

  switch(leaves){
    case 1:
      val = d_vals[idx][event];
      break;
    case 6:
      for (int i = 0; i < 6; ++i) {
        val += d_vals6[idx][event][i];
      }
      val /= 6.0;
      break;
    default:
      val = 0;
      break;
  }
  return val;
}

void Plot::FillCuts(){
  int evs = tree->GetEntries();
  for(int cut = 0; cut < int(cuts.size()); ++cut){
    int idx = -1;
    std::vector<ACNET*>::iterator itr = std::find(pars.begin(), pars.end(), cuts[cut]->par);
    if( itr!= pars.cend())
      idx = std::distance(pars.begin(), itr);

    for(int event = 0; event < evs; ++event){
      bool pass = true;
      pass = cuts[cut]->pass(GetEvent(pars[idx], event));
      if(!pass)
        d_cuts[event] = pass;
    }
  }
}

void Plot::GetRMS(double &_mean, double &_rms, ACNET* par, ACNET* par_norm = NULL){
  int evs = tree->GetEntries();
  double mean = 0;
  double var = 0;
  double norm = 1;

  // Get the mean first
  for(int i = 0; i < evs; ++i){
    if(par_norm)
      norm = GetEvent(par_norm, i);
    mean += GetEvent(par, i)/norm;
  }
  mean /= evs;
  _mean = mean;


  // Get the variance:
  for(int i = 0; i < evs; ++i){
    if(par_norm)
      norm = GetEvent(par_norm, i);
    var += (GetEvent(par, i)/norm - mean)*(GetEvent(par, i)/norm - mean);
  }
  var /= evs;

  if(var == 0)
    std::cout << "warning, 0 RMS!" <<  std::endl;
  _rms = std::sqrt(var);
}
