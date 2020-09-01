#include "datareader.h"

Datum::Datum(std::string i_folder){
  deadFiles = 0;

  // Time window
  tcut_min = -500;
  tcut_max = 500;
  // input folder
  folder = i_folder;

  isOK = true;
  isOneZombie = false;

  //time reference
  k_ref = k_mm1xav;
  evs = new int[k_nLevel];
}

Datum::Datum(std::string i_folder, std::vector< int> _par, int k_timer){
  deadFiles = 0;

  // Time window
  tcut_min = -500;
  tcut_max = 500;
  // input folder
  folder = i_folder;

  isOK = true;
  isOneZombie = false;

  // Time window reference
  // Vector of parameters
  pars = _par;
  k_ref = k_timer;

  evs = new int[pars.size()];
}

void Datum::PreFill(){
  dataHolder = new TTree("beam_monitors", "Time-matched beam monitor variables");
  parse(0);
  fillRAM();
  matchTimes();
  fillTTree();
}

void Datum::ReduceTTree(){
  dataHolder = new TTree("muon_monitors", "Muon monitor variables");
  parse(0);
  fillDataHolder();
}

void Datum::ReduceThermoc(){
  dataHolder = new TTree("muon_monitors", "Thermocouple wire monitor variables");
  parse(1);
  fillDataHolderThermo();
}

void Datum::ReduceBatchTTree(){
  dataHolder = new TTree("thermo_couple", "Thermocouple wire monitor variables");
  parse(0);
  fillBatchedDataHolder();
}

// Fills RAM with all the times/variables, aborts if something's wrong
void Datum::fillRAM(){

  // First initialization if d_vals/times, with the number of parameters we have
  int npars = pars.size();
  d_vals = new double*[npars](); 
  d_vals6 = new double**[npars](); 
  d_vals81 = new double**[npars](); 
  d_times = new Long64_t*[npars](); 

  dataHolder->SetDirectory(0);

  // Iterate through the file-variables to set the branches
  for(int par = 0; par < npars ; ++par){

    // Read the files in
    inFileVec.push_back(nullptr);

    if(isLvl0(pars[par])) inFileVec[par] =  new TFile((folder +"/level0/root/"+ rootFilesIn[par]).c_str(), "READ");
    else inFileVec[par] =  new TFile((folder +"/level1/root/"+ rootFilesIn[par]).c_str(), "READ");

    bBranch.push_back(true);

    // A cerr if file is not open
    if(inFileVec[par]->IsZombie()){
      bBranch[par] = false;
      deadFiles += 1;
      std::cout << "File Is Zombie:" << rootFilesIn[par] << std::endl;
      inTreeVec.push_back(nullptr);
      continue;
    }
    else{
      inTreeVec.push_back((TTree*)inFileVec[par]->Get(levelX_to_str(pars[par]).c_str()));
    }
    // We will abort anyway, might as well speed things up
    if(deadFiles > 0) continue;

#if OUTPUT
    std::cout << levelX_to_str(pars[par]) << "\t" << " Events: " <<  "\t" << inTreeVec[par]->GetEntries() << std::endl;
#endif
    evs[par] = inTreeVec[par]->GetEntries();

    // Finish the initialization
    d_vals[par]   = new double[evs[par]];
    d_times[par]  = new Long64_t[evs[par]];

    if(is6(pars[par]) == true){
      d_vals6[par]  = new double*[evs[par]]();
      for(int i = 0; i < evs[par]; ++i){
        d_vals6[par][i] = new double[6];
      }
    }
    if(is81(pars[par]) == true){
      d_vals81[par] = new double*[evs[par]]();
      for(int i = 0; i < evs[par]; ++i){
        d_vals81[par][i] = new double[81];
      }
    }

    // Set the branches for both input ttrees and output ttree
    // Notice that e.g. k_vptht variable reads 6 values per bunch, but we only save one (a sum)
    if(is6(pars[par]) == true){
        inTreeVec[par]->SetBranchAddress("val", &vals6[par]);
        dataHolder->Branch(levelX_to_str(pars[par]).c_str(), &vals6[par], (levelX_to_str(pars[par]) + "[6]/D").c_str());
    }
    else if(is81(pars[par]) == true){
        inTreeVec[par]->SetBranchAddress("val", &vals81[par]);
        dataHolder->Branch(levelX_to_str(pars[par]).c_str(), vals81[par], (levelX_to_str(pars[par]) + "[81]/D").c_str());
    }
    else{
        inTreeVec[par]->SetBranchAddress("val", &vals[par]);
        dataHolder->Branch(levelX_to_str(pars[par]).c_str(), &vals[par]);
    }
    inTreeVec[par]->SetBranchAddress("time", &times[par]);
    // Save the times too, at least for now...
    //dataHolder->Branch((levelX_to_str(par) + "_time").c_str(), &times[par]);
  } 

  // Set the last branch: time, based on k_mm1xav
  dataHolder->Branch("time", &times[k_ref], "time/L");

  // Check if at least one file is non-zombie
  if(deadFiles > 0) abort();

  // Each parameter separately
  for(int par = 0; par < npars; ++par){
    //inFileVec[par]->cd();

    for(int event = 0; event < evs[par]; ++event){
      inTreeVec[par]->GetEntry(event);
      d_times[par][event] = times[par];

      if(is6(pars[par]) == true){
        for(int i = 0; i < 6; ++i){
          d_vals6[par][event][i] = vals6[par][i];
        }
      } 
      else if(is81(pars[par]) == true){
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


void Datum::DiagnoseData(){
  std::cout << "DIAGNOSING THE DATA" << std::endl;
  int npars = pars.size();

  for(int par = 0; par < npars; ++par){

    double mean = 0;
    Long64_t mean_tdiff = 0;
    double sd = 0;

    // Get the means first
    for(int event = 0; event < evs[par]; ++event){
      if(is6(pars[par]) == true)
        mean += d_vals6[par][event][1];
      else if(is81(pars[par]) == true)
        mean += d_vals81[par][event][1];
      else
        mean += d_vals[par][event];
      mean_tdiff += d_times[par][event] - d_times[k_ref][event];
    }
    mean    /= evs[par];
    mean_tdiff  /= evs[par];

    // Then the standard deviations
    for(int event = 0; event < evs[par]; ++event){
      if(is6(pars[par]))
        sd += (d_vals6[par][event][0] - mean) * (d_vals6[par][event][0] - mean);
      else if(is81(pars[par]))
        sd += (d_vals81[par][event][0] - mean) * (d_vals81[par][event][0] - mean);
      else
        sd += (d_vals[par][event] - mean) * (d_vals[par][event] - mean);
    }
    sd = sqrt(sd/evs[par]);

    std::cout << levelX_to_str(pars[par]) << "::  " << evs[par]<< std::endl;
    std::cout << " * Mean   : " << mean << "  +/-  " << sd/2 << std::endl;
    std::cout << " * TDIFF  : " << mean_tdiff << std::endl;
  }
  std::cout << "################################################################" << std::endl;
}

bool Datum::passCutZero(int par, int k_par, int ev){
  bool pass = true;
  switch(k_par){
    case k_e12_trtgtd:
      (d_vals[par][ev] < 1.0) ? pass = false : pass = true;
      break;
    case k_mm1cor_cal:
      (d_vals[par][ev] < 0) ? pass = false : pass = true;
    case k_mm2cor_cal:
      (d_vals[par][ev] < 0) ? pass = false : pass = true;
    case k_mm3cor_cal:
      (d_vals[par][ev] < 0) ? pass = false : pass = true;
    default:
      pass = true;
  }
  return pass;
}

void Datum::matchTimes(){

  int npars = pars.size();
  for(int par = 0; par < npars; ++par){
    std::cout << levelX_to_str(pars[par]) << "::  " << evs[par] << std::endl;
  }

  // Iterate through mm1cor_cal, since that's what we will be matching to
  for(int event = 0; event < evs[k_ref]; ++event){
    Long64_t index[npars];
    index[k_ref] = event;
    bool isMatchingGlobal = true;

    // Iterate through each parameter
    for(int par = 0; par < npars; ++par){
      if(par == k_ref) continue;
      // Iterate through all the parameter values
      bool doAll = false;

      // "Radially" out from the current event
      bool doRadial = false;

      // Is the parameter in time window?
      bool inTime = true;

      // Iterate through all events if parameters missmatch
      if(evs[par] < event)
        doAll = true;

      // Check if the parameter value is in time to start with
      if(doAll == false){
        inTime = isInTimeWindow(d_times[k_ref][event], d_times[par][event]);
        if(passCutZero(par, pars[par], event) == false)
          inTime = false;
      }

      // if not, we will check parameters starting from the current nevent.
      if(inTime == false) doRadial = true;

      // Save the indices
      if(doAll == false && inTime == true){
        index[par] = event;
      }

      // Iterate through all the parameters
      if(doAll == true){
        inTime = false;
        for(int i = 0; i < evs[par]; ++i){
          inTime = isInTimeWindow(d_times[k_ref][event], d_times[par][i]);
          if(passCutZero(par, pars[par], i) == false)
            inTime = false;
          if(inTime == true){
            index[par] = i;
            break;
          }
        }
        if(inTime == false) isMatchingGlobal = false;
      }


      // Match radially
      if(doAll == false && doRadial == true){
        inTime = false;
        // Limits for matching radially 
        bool limHigh = false;
        bool limLow = false;
        for(int i = 1; i < evs[par]; ++i){
          // Two iterators, one increasing one decreasing
          int h_i = event + i;
          int l_i = event - i;
          // Did we reached either limit?
          if(h_i >= evs[par]) limHigh = true;
          if(l_i < 0) limLow = true;

          // Look at low first
          if(limLow == false){
            inTime = isInTimeWindow(d_times[k_ref][event], d_times[par][l_i]);
            if(passCutZero(par, pars[par], l_i) == false)
              inTime = false;
            if(inTime == true){
              index[par] = l_i;
              break;
            }
          }
          // Look at high
          if(limHigh == false){
            inTime = isInTimeWindow(d_times[k_ref][event], d_times[par][h_i]);
            if(passCutZero(par, pars[par],  h_i) == false)
              inTime = false;
            if(inTime == true){
              index[par] = h_i;
              break;
            }
          }
        }
        if(inTime == false) isMatchingGlobal = false;
      }
    }
    // Don't do anything if failed matching
    if( isMatchingGlobal == false ) continue;

    std::vector< int > temp;
    for(int par = 0; par < npars; ++par){
      temp.push_back( index[par] );
    }
    time_indices.push_back(temp);
  }
  if(time_indices.size() == 0) isOK = false;

  std::cout << "Matched entries: " << time_indices.size() <<"/" << evs[k_ref] << std::endl;
}

// Fills a TTree with using time-matched indices vector
void Datum::fillTTree(){
  int npars = pars.size();
  for(int event = 0; event < int(time_indices.size()); ++event){
    for(int par = 0; par < npars; ++par){
      times[par] = d_times[par][time_indices[event][par]];
      if(is6(pars[par])){
        for(int i = 0; i < 6; ++i)
          vals6[par][i] = d_vals6[par][time_indices[event][par]][i];
      }
      else if(is81(pars[par])){
        for(int i = 0; i < 81; ++i)
          vals81[par][i] = d_vals81[par][time_indices[event][par]][i];
      }
      else {
          vals[par] = d_vals[par][time_indices[event][par]];
      }
    }
    dataHolder->Fill();
  }
}

bool Datum::isInTimeWindow(Long64_t val1, Long64_t val2){
  Long64_t val;
  val = val1 - val2;
  if(val > tcut_max)
    return false;
  if(val < tcut_min)
    return false;
  return true;
}


void Datum::fillBatchedDataHolder(){
  // Ugly vectors... of files and trees

  dataHolder->SetDirectory(0);

  // Iterate through the file-variables to set the branches
  for(int iFile = 0; iFile < k_nLevel; ++iFile){

    // Read the files in
    inFileVec.push_back(nullptr);

    if(isLvl0(iFile)) inFileVec[iFile] =  new TFile((folder +"/level0/root/"+ rootFilesIn[iFile]).c_str(), "READ");
    else inFileVec[iFile] =  new TFile((folder +"/level1/root/"+ rootFilesIn[iFile]).c_str(), "READ");

    bBranch.push_back(true);
    //bBranch[iFile] = true;
    isOK = true;
    // A cerr if file is not open
    if(inFileVec[iFile]->IsZombie()){
      bBranch[iFile] = false;
      deadFiles += 1;
      std::cout << "File Is Zombie:" << rootFilesIn[iFile] << std::endl;
    }

    // Read the TTrees in (there's only one TTree per file)
    if(bBranch[iFile] == false){
      inTreeVec.push_back(nullptr);
    }
    else{
      inTreeVec.push_back((TTree*)inFileVec[iFile]->Get(levelX_to_str(iFile).c_str()));
    }
    if(bBranch[iFile] == false)
      continue;
#if OUTPUT
    std::cout << levelX_to_str(iFile) << "\t" << " Events: " <<  "\t" << inTreeVec[iFile]->GetEntries() << std::endl;
#endif
    evs[iFile] = inTreeVec[iFile]->GetEntries();

    // Set the branches for both input ttrees and output ttree
    // Notice that e.g. k_vptht variable reads 6 values per bunch, but we only save one (a sum)
    if(is6(iFile) == true){
        inTreeVec[iFile]->SetBranchAddress("val", &vals6[iFile]);
        dataHolder->Branch(levelX_to_str(iFile).c_str(), &m_vals6[iFile], (levelX_to_str(iFile) + "[6]/D").c_str());
        dataHolder->Branch((levelX_to_str(iFile) + "_SD").c_str(), &sd_vals6[iFile], (levelX_to_str(iFile) + "_SD[6]/D").c_str());

    }
    else if(is81(iFile) == true){
        inTreeVec[iFile]->SetBranchAddress("val", &vals81[iFile]);
        dataHolder->Branch(levelX_to_str(iFile).c_str(), m_vals81[iFile], (levelX_to_str(iFile) + "[81]/D").c_str());
        dataHolder->Branch((levelX_to_str(iFile) + "_SD").c_str(), sd_vals81[iFile], (levelX_to_str(iFile) + "_SD[81]/D").c_str());
    }
    else{
        inTreeVec[iFile]->SetBranchAddress("val", &vals[iFile]);
        dataHolder->Branch(levelX_to_str(iFile).c_str(), &m_vals[iFile]);
        dataHolder->Branch((levelX_to_str(iFile) + "_SD").c_str(), &sd_vals[iFile]);
    }
    inTreeVec[iFile]->SetBranchAddress("time", &times[iFile]);
    // Save the times too, at least for now...
    //dataHolder->Branch((levelX_to_str(iFile) + "_time").c_str(), &times[iFile]);
  } // end iFile < k_nLevel0

  double m_mm1cor_trtgtd, m_mm2cor_trtgtd, m_mm3cor_trtgtd;
  double sd_mm1cor_trtgtd, sd_mm2cor_trtgtd, sd_mm3cor_trtgtd;

  // Sort out the mm#cor_cal/trtgtd
  std::string temp_name = levelX_to_str(k_ref) + "_" + levelX_to_str(k_e12_trtgtd);
  dataHolder->Branch(temp_name.c_str(), &m_mm1cor_trtgtd);
  dataHolder->Branch((temp_name + "_SD").c_str(), &sd_mm1cor_trtgtd);

  //temp_name = levelX_to_str(k_mm2cor_cal) + "_" + levelX_to_str(k_e12_trtgtd);
  temp_name = levelX_to_str(k_mm2cor) + "_" + levelX_to_str(k_e12_trtgtd);
  dataHolder->Branch(temp_name.c_str(), &m_mm2cor_trtgtd);
  dataHolder->Branch((temp_name + "_SD").c_str(), &sd_mm2cor_trtgtd);

  //temp_name = levelX_to_str(k_mm3cor_cal) + "_" + levelX_to_str(k_e12_trtgtd);
//  temp_name = levelX_to_str(k_mm3cor) + "_" + levelX_to_str(k_e12_trtgtd);
//  dataHolder->Branch(temp_name.c_str(), &m_mm3cor_trtgtd);
//  dataHolder->Branch((temp_name + "_SD").c_str(), &sd_mm3cor_trtgtd);

  // Set the last branch: time, based on k_mm1xav
  Long64_t ttime;
  dataHolder->Branch("time", &ttime, "time/L");

  // Check if at least one file is non-zombie
  for (int tree = 0; tree < k_nLevel0 + k_nLevel1; ++tree){
    if(bBranch[tree] == false)
      isOneZombie = true;
  }
  if(isOneZombie == true){
    std::cout << "ERROR: At least one file is missing" << std::endl;
    abort();
  }
  // Set the last branch: time, based on k_mm1xav

  // Now we will iterate through the events and copy over the TTree!
  int nEvents = inTreeVec[k_ref]->GetEntries();
  nE = nEvents;
  for (int tree = 0; tree < k_nLevel; ++tree){
    if (nE != inTreeVec[tree]->GetEntries()){
      isOK = false;
      std::cout << "ERROR: Events don't match, aborting" << std::endl;
      std::cout << "Events: per par:" << std::endl;
      for(int ev = 0; ev < k_nLevel; ++ev){
        if(bBranch[tree] == false) continue;
        std::cout << levelX_to_str(ev).c_str() << ": " << evs[ev] << std::endl;
      }
      abort();
    }
  }
#if OUTPUT
  if(isOK == false){
    std::cout << "Folder: " << fileNameBase << "  has branches with different nevents." << std::endl;
  }
#endif

  // Initialize means and sds
  for(int i = 0; i < k_nLevel; ++i){
    m_vals[i] = 0;
    sd_vals[i] = 0;
    for(int j = 0; j < 6; ++j){
      m_vals6[i][j] = 0;
      sd_vals6[i][j] = 0;
    }
    for(int j = 0; j < 81; ++j){
      m_vals81[i][j] = 0;
      sd_vals81[i][j] = 0;
    }
  }
  m_mm1cor_trtgtd = 0;
  m_mm2cor_trtgtd = 0;
  m_mm3cor_trtgtd = 0;
  sd_mm1cor_trtgtd = 0;
  sd_mm2cor_trtgtd = 0;
  sd_mm3cor_trtgtd = 0;

  std::vector< int > ind;
  int events = 0;

  bool isFine;
  for(int ev = 0; ev < inTreeVec[k_ref]->GetEntries(); ++ev){
    isFine = true;
    inTreeVec[k_e12_trtgtd]->GetEntry(ev);
    //inTreeVec[k_mm1_sig_calib]->GetEntry(ev);
    //inTreeVec[k_mm2_sig_calib]->GetEntry(ev);
    //inTreeVec[k_mm3_sig_calib]->GetEntry(ev);

    inTreeVec[k_mma1ds]->GetEntry(ev);
    inTreeVec[k_mma2ds]->GetEntry(ev);
    inTreeVec[k_mma2ds]->GetEntry(ev);

    // Hard cuts for some of the parameters. Maybe the whole k_parameter should
    // be a vector of structs, with each struct a having name, index, type
    // (level 1 level 2), cuts, number of entries etc.??
    if(vals[k_e12_trtgtd] < 1.0)
      isFine = false;
    if(vals[k_mm1_sig_calib] < 0)
      isFine = false;
    if(vals[k_mm2_sig_calib] < 0)
      isFine = false;
    if(vals[k_mm1_sig_calib] < 0)
      isFine = false;
    if(vals[k_mm1cor_cal] < 0)
      isFine = false;
    if(vals[k_mm2cor_cal] < 0)
      isFine = false;
    if(vals[k_mm3cor_cal] < 0)
      isFine = false;

    // Save if passes the cut
    if(isFine == true){
      ind.push_back(ev);
      events++;
    }
  }
  std::cout << "Good events: " << events << std::endl;
  if(events == 0){
    std::cerr << "No good events, aborting!" << std::endl;
    abort();
  }

  // Iterate through the parameters
  for(int par = 0; par < k_nLevel; ++par){

    // Enter parameter's ttree
    inFileVec[par]->cd();

    //int events = inTreeVec[par]->GetEntries();
    // Iterate through parameter's events
    for (int event = 0; event < events; ++event) {
      inTreeVec[par]->GetEntry(ind[event]);
      if((par == k_mm1_sig_calib) && (event == 0)){
        ttime = times[k_mm1_sig_calib];
      }
      //if((par == k_mma1ds) && (event == 0)){
      //  ttime = times[k_mma1ds];
      //}

      // Add them to the medium
      m_vals[par] += vals[par];
      if(is6(par)){
        for(int j = 0; j < 6; j++){
          m_vals6[par][j] += vals6[par][j];
        }
      }
      if(is81(par)){
        for(int j = 0; j < 81; j++){
          m_vals81[par][j] += vals81[par][j];
        }
      }
    }

    // Divide parameters by their nevents to get the medium!
    m_vals[par] /= events;
    if(is6(par)){
      for(int j = 0; j < 6; j++){
        m_vals6[par][j] /= events;
      }
    }
    if(is81(par)){
      for(int j = 0; j < 81; j++){
        m_vals81[par][j] /= events;
      }
    }

    // Now let's get the variance
    for (int event = 0; event < events; ++event) {
      inTreeVec[par]->GetEntry(ind[event]);

      sd_vals[par] += (vals[par] - m_vals[par]) * (vals[par] - m_vals[par]);

      if(is6(par)){
        for(int j = 0; j < 6; j++){
          sd_vals6[par][j] += (vals6[par][j] - m_vals6[par][j]) * (vals6[par][j] - m_vals6[par][j]);
        }
      }
      if(is81(par)){
        for(int j = 0; j < 81; j++){
          sd_vals81[par][j] += (vals81[par][j] - m_vals81[par][j]) * (vals81[par][j] - m_vals81[par][j]);
        }
      }
    }

    // Divide by the number of entries and sqrt!
    sd_vals[par] = sqrt(sd_vals[par]/events);
    if(is6(par)){
      for(int j = 0; j < 6; j++){
        sd_vals6[par][j] = sqrt(sd_vals6[par][j]/events);
      }
    }
    if(is81(par)){
      for(int j = 0; j < 81; j++){
        sd_vals81[par][j] = sqrt(sd_vals81[par][j]/events);
      }
    }
  }

  double d_mm1cor_trtgtd[events];
  double d_mm2cor_trtgtd[events];
//  double d_mm3cor_trtgtd[events];

  //double mm1trt_max = -99999;
  //double mm1trt_min = 99999;

  //double mm2trt_max = -99999;
  //double mm2trt_min = 99999;

  //double mm3trt_max = -99999;
  //double mm3trt_min = 99999;

  // Now sum up the mm#cor/trtgtd...
  for(int i = 0; i < events; ++i){
    inTreeVec[k_e12_trtgtd]->GetEntry(ind[i]);
    //inTreeVec[k_mm1cor_cal]->GetEntry(ind[i]);
    //inTreeVec[k_mm2cor_cal]->GetEntry(ind[i]);
    //inTreeVec[k_mm3cor_cal]->GetEntry(ind[i]);
    //
    //m_mm1cor_trtgtd += vals[k_mm1cor_cal]/vals[k_e12_trtgtd];
    //m_mm2cor_trtgtd += vals[k_mm2cor_cal]/vals[k_e12_trtgtd];
    //m_mm3cor_trtgtd += vals[k_mm3cor_cal]/vals[k_e12_trtgtd];
    //
    //d_mm1cor_trtgtd[i] = vals[k_mm1cor_cal]/vals[k_e12_trtgtd];
    //d_mm2cor_trtgtd[i] = vals[k_mm2cor_cal]/vals[k_e12_trtgtd];
    //d_mm3cor_trtgtd[i] = vals[k_mm3cor_cal]/vals[k_e12_trtgtd];

    inTreeVec[k_mm1cor_cal]->GetEntry(ind[i]);
    inTreeVec[k_mm2cor_cal]->GetEntry(ind[i]);
    inTreeVec[k_mm2cor_cal]->GetEntry(ind[i]);

    m_mm1cor_trtgtd += vals[k_mm1cor_cal]/vals[k_e12_trtgtd];
    m_mm2cor_trtgtd += vals[k_mm2cor_cal]/vals[k_e12_trtgtd];
//    m_mm3cor_trtgtd += vals[k_mm3cor]/vals[k_e12_trtgtd];

    d_mm1cor_trtgtd[i] = vals[k_mm1cor]/vals[k_e12_trtgtd];
    d_mm2cor_trtgtd[i] = vals[k_mm2cor]/vals[k_e12_trtgtd];
//    d_mm3cor_trtgtd[i] = vals[k_mm3cor]/vals[k_e12_trtgtd];


    // Find the minimum and maximum
    //if(mm1trt_max < d_mm1cor_trtgtd[i])
    //  mm1trt_max = d_mm1cor_trtgtd[i];
    //if(mm1trt_min > d_mm1cor_trtgtd[i])
    //  mm1trt_min = d_mm1cor_trtgtd[i];

    //if(mm2trt_max < d_mm2cor_trtgtd[i])
    //  mm2trt_max = d_mm2cor_trtgtd[i];
    //if(mm2trt_min > d_mm2cor_trtgtd[i])
    //  mm2trt_min = d_mm2cor_trtgtd[i];

    //if(mm3trt_max < d_mm3cor_trtgtd[i])
    //  mm3trt_max = d_mm3cor_trtgtd[i];
    //if(mm3trt_min > d_mm3cor_trtgtd[i])
    //  mm3trt_min = d_mm3cor_trtgtd[i];

  }

  // Divide by nentries to get a mean
  m_mm1cor_trtgtd /= events;
  m_mm2cor_trtgtd /= events;
  m_mm3cor_trtgtd /= events;

  //TH1D *mm1 = new TH1D("mm1", "mm1", 30, mm1trt_min, mm1trt_max);
  //TH1D *mm2 = new TH1D("mm2", "mm2", 30, mm2trt_min, mm2trt_max);
  //TH1D *mm3 = new TH1D("mm3", "mm3", 30, mm3trt_min, mm3trt_max);

  // Sum up the variances
  for(int i = 0; i < events; ++i){

    sd_mm1cor_trtgtd += (m_mm1cor_trtgtd - d_mm1cor_trtgtd[i]) * (m_mm1cor_trtgtd - d_mm1cor_trtgtd[i]);
    sd_mm2cor_trtgtd += (m_mm2cor_trtgtd - d_mm2cor_trtgtd[i]) * (m_mm2cor_trtgtd - d_mm2cor_trtgtd[i]);
//    sd_mm3cor_trtgtd += (m_mm3cor_trtgtd - d_mm3cor_trtgtd[i]) * (m_mm3cor_trtgtd - d_mm3cor_trtgtd[i]);

    //sd_mm1cor_trtgtd += (d_mm1cor_trtgtd[i]) * (d_mm1cor_trtgtd[i]);
    //sd_mm2cor_trtgtd += (d_mm2cor_trtgtd[i]) * (d_mm2cor_trtgtd[i]);
    //sd_mm3cor_trtgtd += (d_mm3cor_trtgtd[i]) * (d_mm3cor_trtgtd[i]);

    // Fill the histos
    //mm1->Fill(d_mm1cor_trtgtd[i]);
    //mm2->Fill(d_mm2cor_trtgtd[i]);
    //mm3->Fill(d_mm3cor_trtgtd[i]);
  }
  sd_mm1cor_trtgtd = sqrt(sd_mm1cor_trtgtd/(events));
  sd_mm2cor_trtgtd = sqrt(sd_mm2cor_trtgtd/(events));
  sd_mm3cor_trtgtd = sqrt(sd_mm3cor_trtgtd/(events));


  // Get the mean and RMS
  //double mean1 = mm1->GetMean();
  //double mean2 = mm2->GetMean();
  //double mean3 = mm3->GetMean();

  //double sd1 = mm1->GetRMS();
  //double sd2 = mm2->GetRMS();
  //double sd3 = mm3->GetRMS();

  //TF1 *gauss1 = new TF1("gauss1","[0]/sqrt(2.0*3.14159)/[2]*TMath::Exp(-0.5*pow(x-[1],2)/[2]/[2])",mm1trt_min,mm1trt_max);
  //TF1 *gauss2 = new TF1("gauss2","[0]/sqrt(2.0*3.14159)/[2]*TMath::Exp(-0.5*pow(x-[1],2)/[2]/[2])",mm2trt_min,mm2trt_max);
  //TF1 *gauss3 = new TF1("gauss3","[0]/sqrt(2.0*3.14159)/[2]*TMath::Exp(-0.5*pow(x-[1],2)/[2]/[2])",mm3trt_min,mm3trt_max);

  //double peakval1 = mm1->GetBinCenter(mm1->GetMaximumBin());
  //double peakval2 = mm2->GetBinCenter(mm2->GetMaximumBin());
  //double peakval3 = mm3->GetBinCenter(mm3->GetMaximumBin());

  //gauss1->SetParameters(mm1->GetMaximum()*sd1*sqrt(2*3.14), peakval1, sd1);
  //gauss2->SetParameters(mm2->GetMaximum()*sd2*sqrt(2*3.14), peakval2, sd2);
  //gauss3->SetParameters(mm3->GetMaximum()*sd3*sqrt(2*3.14), peakval3, sd3);

  //mm1->Fit("gauss1", "Rq");
  //mm2->Fit("gauss2", "Rq");
  //mm3->Fit("gauss3", "Rq");

  //std::cout << "MEANS and SDs from NUMERICAL" << std::endl;
  //std::cout << "MEANS: " << m_mm1cor_trtgtd << " " << m_mm2cor_trtgtd << " " << m_mm3cor_trtgtd << std::endl;
  //std::cout << "SDs: " << sd_mm1cor_trtgtd<< " " << sd_mm2cor_trtgtd << " " << sd_mm3cor_trtgtd << std::endl;
  //std::cout << std::endl;

  //std::cout << "MEANS and SDs from TH1D" << std::endl;
  //std::cout << "MEANS: " << mean1 << " " << mean2 << " " << mean3 << std::endl;
  //std::cout << "SDs: " << sd1 << " " << sd2 << " " << sd3 << std::endl;
  //std::cout << std::endl;

  //std::cout << "MEANS and SDs from TF1 FIT" << std::endl;
  //std::cout << "MEANS: " << gauss1->GetParameter(1) << " " << gauss2->GetParameter(1) << " " << gauss3->GetParameter(1)<< std::endl;
  //std::cout << "SDs: " << gauss1->GetParameter(2) << " " << gauss2->GetParameter(2) << " " << gauss3->GetParameter(2) << std::endl;
  //std::cout << std::endl;

  //TCanvas *cm1 = new TCanvas("cm1", "cm1", 1200, 1200);
  //mm1->Draw();
  ////gauss1->Draw("SAME");
  //cm1->SaveAs("mm1.png");
  //TCanvas *cm2 = new TCanvas("cm2", "cm2", 1200, 1200);
  //mm2->Draw();
  ////gauss2->Draw("SAME");
  //cm2->SaveAs("mm2.png");
  //TCanvas *cm3 = new TCanvas("cm3", "cm3", 1200, 1200);
  //mm3->Draw();
  ////gauss3->Draw("SAME");
  //cm3->SaveAs("mm3.png");

  dataHolder->Fill();
}

// This will parse the full folder string and gemerate the expeced root file
// names
void Datum::parse(int mode){
  std::cout << "####################################################################" << std::endl;

  // Split the string into tokens first
  char *token;
  // Checks if the date/time were parsed
  int parsed_date = -1;
  int parsed_time = -1;
  // Get the year, month, day, hour, minute. Seconds are always _00
  int year, month, day, hour, minute, second;

  // Split string into tokens
  token = strtok(strdup(folder.c_str()), "/");

  // Iterate through the tokens and try to extract date/time from them
  while (token!= NULL){
    if(parsed_date != 3)
      parsed_date = sscanf(token, "%4d-%2d-%2d",  &year, &month, &day);
    if(parsed_time != 2)
      parsed_time = sscanf(token, "%2d_%2d_%2d",  &hour, &minute, &second);
    token = strtok(NULL, "/");
  }
  free(token);

  // TODO:
  //  * Need to make error warnings if the date/time were not extracted correctly

  // Construct the root file's name bases, with format: 
  // 2020-1-20T08-00-00_2020-1-20T08-30-00_NSLINA.root
  char buffer[50];

  // Add the time (30 second window)
  int hour_to = hour;
  int minute_to = minute;
  int second_to = second;
  if(minute  >= 30){
    if(hour == 23){
      minute_to = 59;
      second_to = 54;
    } else{
      hour_to = hour + 1;
      minute_to = 0;
    }
  }else
    minute_to += 30;

  // Parse the format into buffer
  std::sprintf(buffer, "%04d-%d-%dT%02d-%02d-00_%04d-%d-%dT%02d-%02d-%02d_", 
      year, month, day, hour, minute, year, month, day, hour_to, minute_to, second_to);

  // Buffer into our string
  fileNameBase = buffer;
  int npars = 0;
  (mode == 1) ? npars = getNPars(mode) : npars = pars.size();

  // Generate root file names!
  for (int i = 0; i < npars; ++i) {
    std::string na;
    (mode != 1) ? na = getString(pars[i], mode) : na = getString(i, mode);
    std::stringstream sstream;
    sstream << fileNameBase << na << ".root";
    rootFilesIn.push_back(sstream.str());
  }
#if OUTPUT
  for (int i = 0; i < npars; ++i) {
    std::cout << rootFilesIn[i] << std::endl;
  }
#endif
}


void Datum::fillDataHolderThermo(){
  dataHolder->SetDirectory(0);

  double t_vals[t_thermo];
  double t_vals6[t_thermo][6];

  double t_med_vals[t_thermo];
  double t_med_vals6[t_thermo][6];

  // Initialize t_med
  for(int i = 0; i < t_thermo; ++i){
    t_med_vals[i] = 0;
    for(int j = 0; j < 6; ++j){
      t_med_vals6[i][j] = 0;
    }
  }

  Long64_t t_time;
  Long64_t t_first_time;


  for(int par = 0; par < t_thermo; ++par){

    // Read the files in
    inFileVec.push_back(nullptr);

    inFileVec[par] = new TFile((folder + "/level0/root/" + rootFilesIn[par]).c_str(), "READ");

    bBranch.push_back(true);
    isOK = true;

    // A cerr if the file is not open
    if(inFileVec[par]->IsZombie()){
      bBranch[par] = false;
      deadFiles += 1;
      std::cerr << "File Is ZOMBIE: " << rootFilesIn[par] << std::endl;
    }

    // Read the TTrees in
    if(bBranch[par] == false){
      inTreeVec.push_back(nullptr);
    }
    else{
      inTreeVec.push_back((TTree*)inFileVec[par]->Get(thermoc_to_str(par).c_str()));
    }

    // Skip if not loaded
    if(bBranch[par] == false)
      continue;

#if OUTPUT
    std::cout << thermoc_to_str(par) << "\t" << " Events: \t" << inTreeVec[par]->GetEntries() << std::endl;
#endif

    if(is6(par, 1) == true){
      inTreeVec[par]->SetBranchAddress("val", &t_vals6[par]);
      dataHolder->Branch(getString(par, 1).c_str(), &t_med_vals6[par], (getString(par, 1) + "[6]/D").c_str());
    }
    else{
      inTreeVec[par]->SetBranchAddress("val", &t_vals[par]);
      dataHolder->Branch(getString(par, 1).c_str(), &t_med_vals[par]);
    }

    if(par == t_e12_trtgtd){
      dataHolder->Branch("time", &t_time, "time/L");
      inTreeVec[t_e12_trtgtd]->SetBranchAddress("time", &t_first_time);
    }
  }


  // Check if at least one file is non-zombie
  for (int tree = 0; tree < t_thermo; ++tree){
    if(bBranch[tree] == false){
      std::cout << "ERROR: At least one file is missing" << std::endl;
      abort();
    }
  }

  for(int par = 0; par < t_thermo; ++par){
    inFileVec[par]->cd();
    int nEvents = inTreeVec[par]->GetEntries();


    // Sum over the events
    for (int event = 0; event < nEvents; ++event){
      inTreeVec[par]->GetEntry(event);

      if((par == t_e12_trtgtd)&&(event == 0)){
        t_first_time = t_time;
      }

      if(is6(par, 1) == true){
        for(int i = 0; i < 6; ++i){
          t_med_vals6[par][i] += t_vals6[par][i];
        }
      } else{
        t_med_vals[par] += t_vals[par];
      }
    }

    // Get med
    if(is6(par, 1) == true){
      for(int i = 0; i < 6; ++i){
        t_med_vals6[par][i] /= nEvents;
      }
    }else{
        t_med_vals[par] /= nEvents;
    }
  }
  dataHolder->Fill();

}

// This will iterate through the events in all the input trees and copy them
// into one output TTree
void Datum::fillDataHolder(){
  // Ugly vectors... of files and trees

  dataHolder->SetDirectory(0);

  // Iterate through the file-variables to set the branches
  for(int iFile = 0; iFile < k_nLevel0 + k_nLevel1; ++iFile){

    // Read the files in
    inFileVec.push_back(nullptr);

    if(isLvl0(iFile)) inFileVec[iFile] =  new TFile((folder +"/level0/root/"+ rootFilesIn[iFile]).c_str(), "READ");
    else inFileVec[iFile] =  new TFile((folder +"/level1/root/"+ rootFilesIn[iFile]).c_str(), "READ");

    bBranch.push_back(true);
    isOK = true;
    // A cerr if file is not open
    if(inFileVec[iFile]->IsZombie()){
      bBranch[iFile] = false;
      deadFiles += 1;
      std::cout << "File Is Zombie:" << rootFilesIn[iFile] << std::endl;
    }

    // Read the TTrees in (there's only one TTree per file)
    if(bBranch[iFile] == false){
      inTreeVec.push_back(nullptr);
    }
    else{
      inTreeVec.push_back((TTree*)inFileVec[iFile]->Get(levelX_to_str(iFile).c_str()));
    }
    if(bBranch[iFile] == false)
      continue;
#if OUTPUT
    std::cout << levelX_to_str(iFile) << "\t" << " Events: " <<  "\t" << inTreeVec[iFile]->GetEntries() << std::endl;
#endif

    // Set the branches for both input ttrees and output ttree
    // Notice that e.g. k_vptht variable reads 6 values per bunch, but we only save one (a sum)
    if(is6(iFile) == true){
        inTreeVec[iFile]->SetBranchAddress("val", &vals6[iFile]);
        dataHolder->Branch(levelX_to_str(iFile).c_str(), &vals6[iFile], (levelX_to_str(iFile) + "[6]/D").c_str());
    }
    else if(is81(iFile) == true){
        inTreeVec[iFile]->SetBranchAddress("val", &vals81[iFile]);
        dataHolder->Branch(levelX_to_str(iFile).c_str(), vals81[iFile], (levelX_to_str(iFile) + "[81]/D").c_str());
    }
    else{
        inTreeVec[iFile]->SetBranchAddress("val", &vals[iFile]);
        dataHolder->Branch(levelX_to_str(iFile).c_str(), &vals[iFile]);
    }
    inTreeVec[iFile]->SetBranchAddress("time", &times[iFile]);
    dataHolder->Branch((levelX_to_str(iFile) + "_time").c_str(), &times[iFile]);
  } // end iFile < k_nLevel0

  // Set the last branch: time, based on k_mm1xav
  //inTreeVec[k_mm1xav]->SetBranchAddress("time", &times);
  dataHolder->Branch("time", &times[k_ref], "time/L");

  // Check if at least one file is non-zombie
  if(isOneZombie == true){
    std::cout << "ERROR: At least one file is missing" << std::endl;
    abort();
  }
  // Set the last branch: time, based on k_mm1xav
  //inTreeVec[k_mm1xav]->SetBranchAddress("time", &times);


  //ReadBranchInfo();

  // Now we will iterate through the events and copy over the TTree!
  int nEvents = inTreeVec[k_ref]->GetEntries();
  nE = nEvents;
  for (int tree = 0; tree < k_nLevel0 + k_nLevel1; ++tree){
    if(bBranch[tree] == false)
      continue;
    if (nE != inTreeVec[tree]->GetEntries()){
      isOK = false;
      std::cout << "ERROR: Events don't match, aborting" << std::endl;
      abort();
    }
  }
#if OUTPUT
  if(isOK == false){
    std::cout << "Folder: " << fileNameBase << "  has branches with different nevents." << std::endl;
  }
#endif

  //bool missmatch;
  //int missmatches = 0;
  //Long64_t timediff[2];
  //timediff[0] = 0;
  //timediff[1] = 0;
  for (int event = 0; event < nEvents; ++event) {

    //bool missmatch = false;
    // Get entry from each input TTree
    for (int tree = 0; tree < k_nLevel0 + k_nLevel1; ++tree){
      // FIXME XXX TODO : do I need ->cd() here?
        if(bBranch[tree] == false)
          continue;
        inFileVec[tree]->cd();
        inTreeVec[tree]->GetEntry(event);
        //if(times[tree] != times[k_mm1xav]){
        //  if(missmatches <= 10){
        //  std::cout << "Times missmatch between: " << levelX_to_str(k_mm1xav) << " << " << levelX_to_str(tree) << std::endl;
        //  std::cout << "Missmatch : " << times[k_mm1xav] << " << " << times[tree] << std::endl;
        //  }
        //  missmatch = true;

        //}
    }


    // Now we convert the 6-vals-per-bunch into sums
    for (int tree = 0; tree < k_nLevel0 + k_nLevel1; ++tree){
      if(bBranch[tree] == false)
        continue;

    }

    // Fill all the variables
    dataHolder->Fill();
  }
}

// Save the parsed data into a root file. Save as fileNameBase if the string
// empty
void Datum::saveData(std::string fout_name){
  if(isOneZombie){
    std::cout << "ERROR: Not saving, At least one file is missing" << std::endl;
    abort();
  }
  if(isOK == false){
      std::cout << "ERROR: Events don't match, not saving" << std::endl;
      abort();
  }
  TFile *fout;
  if(fout_name == "")
    fout = new TFile((fileNameBase + "reduced.root").c_str(), "RECREATE");
  else
    fout = new TFile(fout_name.c_str(), "RECREATE");
  fout->cd();
  dataHolder->Write();
  fout->Close();
}

// A simple array sum for parameters with 6 values per TTree entry
double Datum::vals6_sum( double vals[6]){
  double sum = 0.0;
  for (int i = 0; i < 6; ++i) sum += vals[i];
  return sum;
}

// Prints some usefull info
void Datum::printer(){
  std::cout << "FOLDER    : " << folder << std::endl;
  std::cout << "FILEBASE  : " << fileNameBase << std::endl;
//  std::cout << "ZOMBIES    : " << deadFiles << std::endl;
//  std::cout << "IS OK     : " << isOK << std::endl;
}

// Returns TTree name for any parameter (whether from lvl0 or lvl1 folder)
std::string Datum::levelX_to_str(int lev){
  if(isLvl0(lev) == true)
    return level0_to_str(lev);
  else
    return level1_to_str(lev);
}

// Checks whether the parameter has 6 values per TTree entry
bool Datum::is6(int i, int mode){

  switch(mode){
    case 0:
      if(i == k_vptgt || i == k_hptgt || i == k_vp121 || i == k_hp121){
        return true;
      }
      else
        return false;
    case 1:
//      if(i == t_vptgt || i == t_hptgt || i == t_vp121 || i == t_hp121){
//        return true;
//      }
//      else
        return false;
    default:
      std::cout << "Mode " << mode << " not supported!" << std::endl;
      abort();
  }
}

// Checks whether the parameter has 81 values per TTree entry
bool Datum::is81(int i){
    if(i == k_mm1_sig_calib || i == k_mm2_sig_calib || i == k_mm3_sig_calib){
      return true;
    }
    //if(i == k_mma1ds || i == k_mma2ds || i == k_mma3ds
    //    || i == k_mma1pd || i == k_mma2pd || i == k_mma3pd){
    //  return true;
    //}
    //if(i == k_mma2ds || i == k_mma2ds || i == k_mma3ds 
    //    || i == k_mma1pd || i == k_mma2pd || i == k_mma3pd 
    //    || i == k_mm1_sig_calib || i == k_mm2_sig_calib || i == k_mm3_sig_calib){
    //  return true;
    //}
    else
      return false;
}

// Reads the number of values per TTree entry for all the parameters to be
// loaded
void Datum::ReadBranchInfo(){
  for (int i = 0; i < k_nLevel0 + k_nLevel1; ++i) {
    TBranch *br = (TBranch*)inTreeVec[i]->GetListOfBranches()->At(1);
    TLeaf* leaf = (TLeaf*)br->GetListOfLeaves()->UncheckedAt(0);
    std::cout << i << " " << std::setw(3) << levelX_to_str(i) << "::" << " " << std::setw(10) << leaf->GetLenStatic() << " " << std::endl;
  }
}

std::string Datum::getString(int i, int mode){
  switch(mode){
    case 0:
     return levelX_to_str(i);
    case 1:
     return thermoc_to_str(i);
    default:
      std::cout << "Mode " << mode << " not supported!" << std::endl;
      abort();
  }
}

int Datum::getNPars(int mode){
  switch(mode){
    case 0:
      return k_nLevel;
    case 1:
      return t_thermo;
    default:
      std::cout << "Mode " << mode << " not supported!" << std::endl;
      abort();
  }
}
