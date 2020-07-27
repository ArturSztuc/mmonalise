// Datareader: you provide a folder with the beam monitor files in, and it will
// merge the root files with different variables into one. To be used as a
// library in your executible or in different class. 
#ifndef _datareader_h_
#define _datareader_h_

// 0 = No diagnostic output
// 1 = Diagnostic output
#define OUTPUT 0 

#include <iostream>
#include <iomanip>
#include <vector>
#include "structs.h"
#include <string.h>

#include "TStyle.h"
#include "TH2F.h"
#include "TH2D.h"
#include "TH1D.h"
#include "TF1.h"


#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TLeaf.h"
#include "TCanvas.h"
#include "TDatime.h"
#include "structs.h"

class Datum{
  public:
    // Constructor
    Datum(std::string i_folder);

    /*  Getters  */
    // Returns the processed TTree
    TTree* getTTree(){ return dataHolder; };

    // Returns the "reconstructed" file basename
    std::string getFileNameBase(){ return fileNameBase; };

    // Saves the merged/parsed data into a .root file.
    // Will save it as fileNameBase_parsed.root if empty
    void saveData(std::string fout_name = "" );

    void ReduceTTree();
    void ReduceBatchTTree();
    void ReduceThermoc();

    void PreFill();

    void DiagnoseData();

    // Printer
    void printer();

  private:
    // Initialize the data
    void init(int mode = 0);

    // Parses the input folder string to "reconstruct" the expected input root
    // filenames. The expected variables-ttres are defined in structs.h
    void parse(int mode = 0);

    // Loads the variables from TTrees onto memory double (times into Long64_t)
    void fillRAM();

    // Fills and array of time-matched indices 
    void matchTimes();

    // This will fill the TTree
    void fillTTree();

    bool isInTimeWindow(Long64_t val1, Long64_t val2);

    // Opens the root files (with names generated in parse()) and fills an
    // output root file
    void fillDataHolder();
    void fillBatchedDataHolder();

    void fillDataHolderThermo();

    // A helper function to look into what's inside a branch
    void ReadBranchInfo();

    std::string getString(int i, int mode = 0);
    int getNPars(int mode = 0);

    // Does the variable iterator belong to lvl0?
    bool isLvl0(int lev){ return lev < k_nLevel0; };
    bool is81(int i);
    bool is6(int i, int mode = 0);

    bool passCutZero(int par, int ev);

    std::string levelX_to_str(int lev);

    // Sums over an array
    double vals6_sum( double vals[]);

    // Time-matching window;
    int tcut_min;
    int tcut_max;


    // Holds all the variables
    double vals81[k_nLevel][81];
    double vals6[k_nLevel][6];
    double vals[k_nLevel];

    // Variable means
    double m_vals81[k_nLevel][81];
    double m_vals6[k_nLevel][6];
    double m_vals[k_nLevel];

    // Variable SD
    double sd_vals81[k_nLevel][81];
    double sd_vals6[k_nLevel][6];
    double sd_vals[k_nLevel];


    // We will keep the variables in the memory... :(
    double *d_vals[k_nLevel];
    double *d_vals6[k_nLevel][6];
    double *d_vals81[k_nLevel][81];
    Long64_t *d_times[k_nLevel];

    // Time-matched indices
    std::vector< std::vector< int > > time_indices;

    int evs[k_nLevel];

    // Holds all the times
    Long64_t times[k_nLevel];

    TDatime th[2];


    std::vector< TFile* > inFileVec;
    std::vector< TTree* > inTreeVec;
    //std::vector< TTree* > inTreeTimeVec;
    
    int             deadFiles;    // Number of dead files
    int             nE;           // Number of events for comparison
    bool            isOK;         // Is the branch OK to continue?
    bool            isOneZombie;  // Is any TFile Zombie?
    std::string     folder;       // Folder containing the root files
    std::string     fileNameBase; // Basename of the root files
    std::vector< std::string >  rootFilesIn;  // An array with all the root file names
    TTree*          dataHolder;   // TTree output holding all the converted data
    TFile*          dataOut;      // TFile containing all the data, if we actually want it

    // Holds the isFileZombie
    //bool bBranch[k_nLevel0 + k_nLevel1];
    std::vector< bool > bBranch;
};

#endif /* ifndef __datareader_h__ */

