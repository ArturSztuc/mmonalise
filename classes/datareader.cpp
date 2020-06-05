#include "datareader.h"

Datum::Datum(std::string i_folder){
  deadFiles = 0;
  folder = i_folder;
  dataHolder = new TTree("beam_monitors", "Beam monitoring variables");
  parse();
  fillDataHolder();
}

// This will parse the full folder string and gemerate the expeced root file
// names
void Datum::parse(){
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
  std::sprintf(buffer, "%04d-%d-%02dT%02d-%02d-00_%04d-%d-%02dT%02d-%02d-%02d_", 
      year, month, day, hour, minute, year, month, day, hour_to, minute_to, second_to);

  // Buffer into our string
  fileNameBase = buffer;

  // Generate root file names!
  for (int i = 0; i < k_nLevel0 + k_nLevel1; ++i) {
    std::stringstream sstream;
    if(isLvl0(i)) sstream << fileNameBase << level0_to_str(i) << ".root";
    else sstream << fileNameBase << level1_to_str(i) << ".root";
    rootFilesIn.push_back(sstream.str());
  }
  //for (int i = 0; i < k_nLevel0 + k_nLevel1; ++i) {
  //  std::cout << rootFilesIn[i] << std::endl;
  //}
}

// This will iterate through the events in all the input trees and copy them
// into one output TTree
void Datum::fillDataHolder(){
  // Ugly vectors... of files and trees
  std::vector< TFile* > inFileVec;
  std::vector< TTree* > inTreeVec;

  // The variables will be held here. Some have one value per bunch, some have
  // 6, some have more...
  double vals81[k_nLevel0+k_nLevel1][81];
  double vals6[k_nLevel0+k_nLevel1][6];
  double vals[k_nLevel0+k_nLevel1];

  Long64_t time;
  
  dataHolder->SetDirectory(0);

  // Iterate through the file-variables to set the branches
  for(int iFile = 0; iFile < k_nLevel0 + k_nLevel1; ++iFile){

    // Read the files in
    inFileVec.push_back(nullptr);

    if(isLvl0(iFile)) inFileVec[iFile] =  new TFile((folder +"/level0/root/"+ rootFilesIn[iFile]).c_str(), "READ");
    else inFileVec[iFile] =  new TFile((folder +"/level1/root/"+ rootFilesIn[iFile]).c_str(), "READ");

    bBranch[iFile] = true;
    // A cerr if file is not open
    if(inFileVec[iFile]->IsZombie()){
      bBranch[iFile] = false;
      deadFiles += 1;
      std::cout << "File Is Zombie:" << rootFilesIn[iFile] << std::endl;
    }

    // Read the TTrees in (there's only one TTree per file)
    // TODO:
    //  * Sanity check - is the TTree succesfully loaded from the file?
    if(bBranch[iFile] == false){
      inTreeVec.push_back(NULL);
    }
    else{
      if(isLvl0(iFile)) inTreeVec.push_back((TTree*)inFileVec[iFile]->Get(level0_to_str(iFile).c_str()));
      else inTreeVec.push_back((TTree*)inFileVec[iFile]->Get(level1_to_str(iFile).c_str()));
    }
    if(bBranch[iFile] == false)
      continue;
    //if(isLvl0(iFile)) std::cout << "Tree: " << iFile  << " Name: " << level0_to_str(iFile) << " Events: " << inTreeVec[iFile]->GetEntries() << std::endl;
    //else std::cout << "Tree: " << iFile  << " Name: " << level1_to_str(iFile) << " Events: " << inTreeVec[iFile]->GetEntries() << std::endl;

    // Set the branches for both input ttrees and output ttree
    // Notice that e.g. k_vptht variable reads 6 values per bunch, but we only save one (a sum)
    if(iFile == k_vptgt || iFile == k_hptgt || iFile == k_vp121 || iFile == k_hp121){
        inTreeVec[iFile]->SetBranchAddress("val", &vals6[iFile]);
        if(isLvl0(iFile)) dataHolder->Branch(level0_to_str(iFile).c_str(), &vals[iFile]);
        else dataHolder->Branch(level1_to_str(iFile).c_str(), &vals[iFile]);
    }
    else if(iFile == k_mma1ds || iFile == k_mma2ds || iFile == k_mma3ds){
        inTreeVec[iFile]->SetBranchAddress("val", &vals81[iFile]);
        if(isLvl0(iFile)) dataHolder->Branch(level0_to_str(iFile).c_str(), vals81[iFile], (level0_to_str(iFile) + "[81]/D").c_str());
        else dataHolder->Branch(level1_to_str(iFile).c_str(), vals81[iFile], (level1_to_str(iFile) + "[81]/D").c_str());
    }
    else{
        inTreeVec[iFile]->SetBranchAddress("val", &vals[iFile]);
        if(isLvl0(iFile)) dataHolder->Branch(level0_to_str(iFile).c_str(), &vals[iFile]);
        else dataHolder->Branch(level1_to_str(iFile).c_str(), &vals[iFile]);
    }
  } // end iFile < k_nLevel0

  // Set the last branch: time, based on k_mm1xav
  inTreeVec[k_mm1xav]->SetBranchAddress("time", &time);
  dataHolder->Branch("time", &time, "time/l");

  // Now we will iterate through the events and copy over the TTree!
  int nEvents = inTreeVec[0]->GetEntries();
  for (int event = 0; event < nEvents; ++event) {

    // Get entry from each input TTree
    for (int tree = 0; tree < k_nLevel0 + k_nLevel1; ++tree){
      // FIXME XXX TODO : do I need ->cd() here?
        if(bBranch[tree] == false)
          continue;
        inFileVec[tree]->cd();
        inTreeVec[tree]->GetEntry(event);
    }
    // Now we convert the 6-vals-per-bunch into sums
    for (int tree = 0; tree < k_nLevel0 + k_nLevel1; ++tree){
      if(bBranch[tree] == false)
        continue;

      if(tree == k_vptgt || tree == k_hptgt || tree== k_vp121 || tree== k_hp121){
        vals[tree] = vals6_sum(vals6[tree]);
      }
    }

    // Fill all the variables
    dataHolder->Fill();
  }

}

// Save the parsed data into a root file. Save as fileNameBase if the string
// empty
void Datum::saveData(std::string fout_name){
  TFile *fout;
  if(fout_name == "")
    fout = new TFile((fileNameBase + "parsed.root").c_str(), "RECREATE");
  else
    fout = new TFile(fout_name.c_str(), "RECREATE");
  fout->cd();
  dataHolder->Write();
  fout->Close();
}

// A simple array sum...
double Datum::vals6_sum( double vals[6]){
  double sum = 0.0;
  for (int i = 0; i < 6; ++i) sum += vals[i];
  return sum;
}


void Datum::printer(){
  std::cout << "FOLDER    : " << folder << std::endl;
  std::cout << "FILEBASE  : " << fileNameBase << std::endl;
  std::cout << "ZOMBES    : " << deadFiles << std::endl;
}

int main(int argc, char *argv[])
{
  Datum day1("/sn");
  day1.printer();
  return 0;
}
