#include "mergettree.h"

MergeT::MergeT(std::string i_folder){
  folder = i_folder;
  dataHolder = new TTree("beam_monitors", "Beam monitoring variables");
  parse();
  fillDataHolder();
}

// This will parse the full folder string and gemerate the expeced root file
// names
void MergeT::parse(){

  // Split the string into tokens first
  char *token;
  // Checks if the date/time were parsed
  int parsed_date = -1;
  int parsed_time = -1;
  // Get the year, month, day, hour, minute. Seconds are always _00
  int year, month, day, hour, minute;

  // Split string into tokens
  token = strtok(strdup(folder.c_str()), "/");

  // Iterate through the tokens and try to extract date/time from them
  while (token!= NULL){
    if(parsed_date != 3)
      parsed_date = sscanf(token, "%4d-%2d-%2d",  &year, &month, &day);
    if(parsed_time != 2)
      parsed_time = sscanf(token, "%2d_%2d_00",  &hour, &minute);
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
  if(minute  >= 30){
    hour_to = hour + 1;
    minute_to = 60 - minute;
  }else
    minute_to += 30;

  // Parse the format into buffer
  std::sprintf(buffer, "%04d-%d-%02dT%02d-%02d-00_%04d-%d-%02dT%02d-%02d-00_", 
      year, month, day, hour, minute, year, month, day, hour_to, minute_to);

  // Buffer into our string
  fileNameBase = buffer;

  // Generate root file names!
  for (int i = 0; i < k_nLevel0; ++i) {
    std::stringstream sstream;
    sstream << fileNameBase << level0_to_str(i) << ".root";
    rootFilesIn.push_back(sstream.str());
  }
  for (int i = 0; i < k_nLevel0; ++i) {
    std::cout << rootFilesIn[i] << std::endl;
  }
}

// This will iterate through the events in all the input trees and copy them
// into one output TTree
void MergeT::fillDataHolder(){
  // Ugly vectors... of files and trees
  std::vector< TFile* > inFileVec;
  std::vector< TTree* > inTreeVec;

  // The variables will be held here. Some have one value per bunch, some have
  // 6, some have more...
  double vals81[k_nLevel0][81];
  double vals6[k_nLevel0][6];
  double vals[k_nLevel0];

  Long64_t time;
  
  dataHolder->SetDirectory(0);

  // Iterate through the file-variables to set the branches
  for(int iFile = 0; iFile < k_nLevel0; ++iFile){

    // Read the files in
    inFileVec.push_back(nullptr);
    inFileVec[iFile] =  new TFile((folder +"/level0/"+ rootFilesIn[iFile]).c_str(), "READ");

    // A cerr if file is not open
    if(!inFileVec[iFile]->IsOpen()){
      std::cerr << "ERROR: File not open - " << 
        (folder + "/level0" + rootFilesIn[iFile]).c_str() 
        << std::endl;
    }

    // Read the TTrees in (there's only one TTree per file)
    // TODO:
    //  * Sanity check - is the TTree succesfully loaded from the file?
    inTreeVec.push_back((TTree*)inFileVec[iFile]->Get(level0_to_str(iFile).c_str()));
    std::cout << "Tree: " << iFile  << " Name: " << level0_to_str(iFile) << " Events: " << inTreeVec[iFile]->GetEntries() << std::endl;

    // Set the branches for both input ttrees and output ttree
    // Notice that e.g. k_vptht variable reads 6 values per bunch, but we only save one (a sum)
    if(iFile == k_vptgt || iFile == k_hptgt || iFile == k_vp121 || iFile == k_hp121){
        inTreeVec[iFile]->SetBranchAddress("val", &vals6[iFile]);
        dataHolder->Branch(level0_to_str(iFile).c_str(), &vals[iFile]);
    }
    else if(iFile == k_mma1ds || iFile == k_mma2ds || iFile == k_mma3ds){
        inTreeVec[iFile]->SetBranchAddress("val", &vals81[iFile]);
        dataHolder->Branch(level0_to_str(iFile).c_str(), vals81[iFile], (level0_to_str(iFile) + "[81]/D").c_str());
    }
    else{
        inTreeVec[iFile]->SetBranchAddress("val", &vals[iFile]);
        dataHolder->Branch(level0_to_str(iFile).c_str(), &vals[iFile]);
    }
  } // end iFile < k_nLevel0

  // Set the last branch: time, based on k_mm1xav
  inTreeVec[k_mm1xav]->SetBranchAddress("time", &time);
  dataHolder->Branch("time", &time, "time/l");

  // Now we will iterate through the events and copy over the TTree!
  int nEvents = inTreeVec[0]->GetEntries();
  for (int event = 0; event < nEvents; ++event) {

    // Get entry from each input TTree
    for (int tree = 0; tree < k_nLevel0; ++tree){
      // FIXME XXX TODO : do I need ->cd() here?
        inFileVec[tree]->cd();
        inTreeVec[tree]->GetEntry(event);
    }
    // Now we convert the 6-vals-per-bunch into sums
    for (int tree = 0; tree < k_nLevel0; ++tree){
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
void MergeT::saveData(std::string fout_name){
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
double MergeT::vals6_sum( double vals[6]){
  double sum = 0.0;
  for (int i = 0; i < 6; ++i) sum += vals[i];
  return sum;
}


void MergeT::printer(){
  std::cout << std::endl;
  std::cout << "FOLDER    : " << folder << std::endl;
  std::cout << "FILEBASE  : " << fileNameBase << std::endl;
}

int main(int argc, char *argv[])
{
  MergeT day1("/sn");
  day1.printer();
  return 0;
}
