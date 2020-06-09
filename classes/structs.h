#ifndef _structs_h_
#define _structs_h_

#include <iostream>

enum level0 {
//  k_e12_tortgt = 0,
//  k_e12_trtgtd,
  k_hp121 = 0,
  k_hptgt,
//  k_mgsmm1,
//  k_mgsmm2,
//  k_mgsmm3,
//  k_mgsmpd104,
//  k_mgsmpd105,
//  k_mgsmpd106,
  k_mm1cor,
//  k_mm1gf,
//  k_mm1gpr,
//  k_mm1hv1,
//  k_mm1hv2,
//  k_mm1hv3,
//  k_mm1prc,
//  k_mm1rtd,
  k_mm1xav,
  k_mm1yav,
  k_mm2cor,
//  k_mm2gf,
//  k_mm2gpr,
//  k_mm2hv1,
//  k_mm2hv2,
//  k_mm2hv3,
//  k_mm2prc,
//  k_mm2rtd,
  k_mm2xav,
  k_mm2yav,
  k_mm3cor,
//  k_mm3gf,
//  k_mm3gpr,
//  k_mm3hv1,
//  k_mm3hv2,
//  k_mm3hv3,
//  k_mm3prc,
//  k_mm3rtd,
  k_mm3xav,
  k_mm3yav,
//  k_mma1ds,
//  k_mma1pd,
//  k_mma2ds,
//  k_mma2pd,
//  k_mma3ds,
//  k_mma3pd,
//  k_nslina,
//  k_nslinb,
//  k_nslinc,
//  k_nslind,
//  k_tgtt1,
//  k_tgtt2,
//  k_tgtt3,
//  k_tgtt4,
//  k_thptbw,
//  k_thptcw,
//  k_thpths,
//  k_thpttw,
  k_vp121,
  k_vptgt,
  k_nLevel0      // Number of files/parameters to parse
};

enum level1 {
//  k_mgsmm1_cal = k_nLevel0,
//  k_mgsmm2_cal,
//  k_mgsmm3_cal,
//  k_mm1cnt_cal,
//  k_mm1cor_cal,
//  k_mm1int_cal,
  k_mm1_sig_calib = k_nLevel0,
//  k_mm2cnt_cal,
//  k_mm2cor_cal,
//  k_mm2int_cal,
  k_mm2_sig_calib,
//  k_mm3cnt_cal,
//  k_mm3cor_cal,
//  k_mm3int_cal,
  k_mm3_sig_calib,
  k_nLevel1 = (k_mm3_sig_calib - k_nLevel0)+1,     // Number of files/parameters to parse
  k_nLevel = k_nLevel0 + k_nLevel1
};

inline std::string level0_to_str(int type){
  std::string ReturnString = "";

  switch(type){
//    case k_e12_tortgt:
//      ReturnString = "E12_TORTGT";
//      break;
//    case k_e12_trtgtd:
//      ReturnString = "E12_TRTGTD";
//      break;
    case k_hp121:
      ReturnString = "HP121";
      break;
    case k_hptgt:
      ReturnString = "HPTGT";
      break;
//    case k_mgsmm1:
//      ReturnString = "MGSMM1";
//      break;
//    case k_mgsmm2:
//      ReturnString = "MGSMM2";
//      break;
//    case k_mgsmm3:
//      ReturnString = "MGSMM3";
//      break;
//    case k_mgsmpd104:
//      ReturnString = "MGSMPD104";
//      break;
//    case k_mgsmpd105:
//      ReturnString = "MGSMPD105";
//      break;
//    case k_mgsmpd106:
//      ReturnString = "MGSMPD106";
//      break;
    case k_mm1cor:
      ReturnString = "MM1COR";
      break;
//    case k_mm1gf:
//      ReturnString = "MM1GF";
//      break;
//    case k_mm1gpr:
//      ReturnString = "MM1GPR";
//      break;
//    case k_mm1hv1:
//      ReturnString = "MM1HV1";
//      break;
//    case k_mm1hv2:
//      ReturnString = "MM1HV2";
//      break;
//    case k_mm1hv3:
//      ReturnString = "MM1HV3";
//      break;
//    case k_mm1prc:
//      ReturnString = "MM1PRC";
//      break;
//    case k_mm1rtd:
//      ReturnString = "MM1RTD";
//      break;
    case k_mm1xav:
      ReturnString = "MM1XAV";
      break;
    case k_mm1yav:
      ReturnString = "MM1YAV";
      break;
    case k_mm2cor:
      ReturnString = "MM2COR";
      break;
//    case k_mm2gf:
//      ReturnString = "MM2GF";
//      break;
//    case k_mm2gpr:
//      ReturnString = "MM2GPR";
//      break;
//    case k_mm2hv1:
//      ReturnString = "MM2HV1";
//      break;
//    case k_mm2hv2:
//      ReturnString = "MM2HV2";
//      break;
//    case k_mm2hv3:
//      ReturnString = "MM2HV3";
//      break;
//    case k_mm2prc:
//      ReturnString = "MM2PRC";
//      break;
//    case k_mm2rtd:
//      ReturnString = "MM2RTD";
//      break;
    case k_mm2xav:
      ReturnString = "MM2XAV";
      break;
    case k_mm2yav:
      ReturnString = "MM2YAV";
      break;
    case k_mm3cor:
      ReturnString = "MM3COR";
      break;
//    case k_mm3gf:
//      ReturnString = "MM3GF";
//      break;
//    case k_mm3gpr:
//      ReturnString = "MM3GPR";
//      break;
//    case k_mm3hv1:
//      ReturnString = "MM3HV1";
//      break;
//    case k_mm3hv2:
//      ReturnString = "MM3HV2";
//      break;
//    case k_mm3hv3:
//      ReturnString = "MM3HV3";
//      break;
//    case k_mm3prc:
//      ReturnString = "MM3PRC";
//      break;
//    case k_mm3rtd:
//      ReturnString = "MM3RTD";
//      break;
    case k_mm3xav:
      ReturnString = "MM3XAV";
      break;
    case k_mm3yav:
      ReturnString = "MM3YAV";
      break;
//    case k_mma1ds:
//      ReturnString = "MMA1DS";
//      break;
//    case k_mma1pd:
//      ReturnString = "MMA1PD";
//      break;
//    case k_mma2ds:
//      ReturnString = "MMA2DS";
//      break;
//    case k_mma2pd:
//      ReturnString = "MMA2PD";
//      break;
//    case k_mma3ds:
//      ReturnString = "MMA3DS";
//      break;
//    case k_mma3pd:
//      ReturnString = "MMA3PD";
//      break;
//    case k_nslina:
//      ReturnString = "NSLINA";
//      break;
//    case k_nslinb:
//      ReturnString = "NSLINB";
//      break;
//    case k_nslinc:
//      ReturnString = "NSLINC";
//      break;
//    case k_nslind:
//      ReturnString = "NSLIND";
//      break;
//    case k_tgtt1:
//      ReturnString = "TGTT1";
//      break;
//    case k_tgtt2:
//      ReturnString = "TGTT2";
//      break;
//    case k_tgtt3:
//      ReturnString = "TGTT3";
//      break;
//    case k_tgtt4:
//      ReturnString = "TGTT4";
//      break;
//    case k_thptbw:
//      ReturnString = "THPTBW";
//      break;
//    case k_thptcw:
//      ReturnString = "THPTCW";
//      break;
//    case k_thpths:
//      ReturnString = "THPTHS";
//      break;
//    case k_thpttw:
//      ReturnString = "THPTTW";
//      break;
    case k_vp121:
      ReturnString = "VP121";
      break;
    case k_vptgt:
      ReturnString = "VPTGT";
      break;
    default:
      std::cerr << "ERROR:  " << __FILE__ << "::" << __LINE__ << std::endl;
      std::cerr << "ERROR:  Did not find to_string for type: " << type << std::endl;
      break;
  }
  return ReturnString;
}


inline std::string level1_to_str(int type){
  std::string ReturnString = "";

  switch(type){
//    case k_mgsmm1_cal:
//      ReturnString = "MGSMM1_CAL";
//      break;
//    case k_mgsmm2_cal:
//      ReturnString = "MGSMM2_CAL";
//      break;
//    case k_mgsmm3_cal:
//      ReturnString = "MGSMM3_CAL";
//      break;
//    case k_mm1cnt_cal:
//      ReturnString = "MM1CNT_CAL";
//      break;
//    case k_mm1cor_cal:
//      ReturnString = "MM1COR_CAL";
//      break;
//    case k_mm1int_cal:
//      ReturnString = "MM1INT_CAL";
//      break;
    case k_mm1_sig_calib:
      ReturnString = "MM1_sig_calib";
      break;
//    case k_mm2cnt_cal:
//      ReturnString = "MM2CNT_CAL";
//      break;
//    case k_mm2cor_cal:
//      ReturnString = "MM2COR_CAL";
//      break;
//    case k_mm2int_cal:
//      ReturnString = "MM2INT_CAL";
//      break;
    case k_mm2_sig_calib:
      ReturnString = "MM2_sig_calib";
      break;
//    case k_mm3cnt_cal:
//      ReturnString = "MM3CNT_CAL";
//      break;
//    case k_mm3cor_cal:
//      ReturnString = "MM3COR_CAL";
//      break;
//    case k_mm3int_cal:
//      ReturnString = "MM3INT_CAL";
//      break;
    case k_mm3_sig_calib:
      ReturnString = "MM3_sig_calib";
      break;
    default:
      std::cerr << "ERROR:  " << __FILE__ << "::" << __LINE__ << std::endl;
      std::cerr << "ERROR:  Did not find to_string for type: " << type << std::endl;
      break;
  }
  return ReturnString;
}

#endif /* ifndef __structs_h__ */
