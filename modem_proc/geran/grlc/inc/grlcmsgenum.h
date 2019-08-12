/*****************************************************************************
***
*** TITLE
***
***  GPRS LAYER 2 RLC MESSAGE ENUMS
***
***
*** DESCRIPTION
***
***  RLC enum definitions for replacing F3s.
***
***
*** Copyright (c) 2001-2016 Qualcomm Technologies, Inc.
*** All Rights Reserved. QUALCOMM Proprietary.
***
*** Export of this technology or software is regulated by the U.S.
*** Government. Diversion contrary to U.S. law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grlc/inc/grlcmsgenum.h#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 04-05-16   RRP     Initial version
*****************************************************************************/

#ifndef INC_GRLCMSGENUM_H
#define INC_GRLCMSGENUM_H

#define INVALID_GAS_ID 0xFF

typedef enum
{
  GRLC_F3_MSG_ENUM_0, // ERLU %u OOR MinMax: 0x%08x Force to %u
  GRLC_F3_MSG_ENUM_1, // ERLD append_pdu blk_len=%d < len=%d
  GRLC_F3_MSG_ENUM_2, // ERLD req sending zero-length PDU
  GRLC_F3_MSG_ENUM_3, // dpdu=0x%x tlli=0x%x FrstLast=%08x
  GRLC_F3_MSG_ENUM_4, // GRLD Info Sent PDU FBI gas_id=%d
  GRLC_F3_MSG_ENUM_5, // GRLD Info Sent PDU FBI=0 no-ext trnctd gas_id=%d
  GRLC_F3_MSG_ENUM_6, // GRLD Asm Error Unexp ME=00 gas_id=%d
  GRLC_F3_MSG_ENUM_7, // GRLD Info Sent PDU no-pdu no-ext (01) li=0 gas_id=%d
  GRLC_F3_MSG_ENUM_8, // GRLD Info Sent PDU p0 e0 01 li=0 trnctd gas_id=%d
  GRLC_F3_MSG_ENUM_9, // GRLD Info Sent PDU p0 e0 01 li=%d gas_id=%d
  GRLC_F3_MSG_ENUM_10, // GRLD Info Sent p1 e1 10 li=%d gas_id=%d
  GRLC_F3_MSG_ENUM_11, // GRLD Sent p1 e0 11 num_octs=%d li=%d plen=%d
  GRLC_F3_MSG_ENUM_12, // GRLD 11 b_l=%d n_octs=%d n_e=%d
  GRLC_F3_MSG_ENUM_13, // GRLD 11 FBI p_len=%d gas_id=%d
  GRLC_F3_MSG_ENUM_14, // GRLD Asm Error Unexp ME=%d gas_id=%d
  GRLC_F3_MSG_ENUM_15, // ERLD Info Sent PDU FBI last-bsn=%u gas_id=%d
  GRLC_F3_MSG_ENUM_16, // ERLD Info Sent PDU FBI=0 no-ext trnctd last-bsn=%u bsn2=%u gas_id=%d
  GRLC_F3_MSG_ENUM_17, // ERLD Asm Error Unexp LI=%u E=%u Eth=%u
  GRLC_F3_MSG_ENUM_18, // ERLD Info Sent PDU FBI last-bsn=%u gas_id=%d
  GRLC_F3_MSG_ENUM_19, // ERLD Info Sent PDU last-bsn=%u gas_id=%d
  GRLC_F3_MSG_ENUM_20, // ERLD Info Sent PDU last-bsn=%u gas_id=%d
  GRLC_F3_MSG_ENUM_21, // ERLD Info Sent PDU FBI last-bsn=%u gas_id=%d
  GRLC_F3_MSG_ENUM_22, // ERLD Asm Free trailing %u octets gas_id=%d
  GRLC_F3_MSG_ENUM_23, // GRLD Asm Error Unexp LI/E %d %d
  GRLC_F3_MSG_ENUM_24, // GRLD SnFaVrVq %08x RBB %08x %08x
  GRLC_F3_MSG_ENUM_25, // ERLD TRSEmWsz=%08x 00vq00vr=%08x ssnPBSN=%08x
  GRLC_F3_MSG_ENUM_26, // ERLD epdan FBEeCSLe=%08x ssnPBSN=%08x DL0vq0vr=%08x
  GRLC_F3_MSG_ENUM_27, // ERLD U epdan FBEeCSLe=%08x ssnPBSN=%08x DL0vq0vr=%08x
  GRLC_F3_MSG_ENUM_28, //            crbb[2]=%08x [1]=%08x [0]=%08x
  GRLC_F3_MSG_ENUM_29, //            crbb[3]=%08x len=%u dec
  GRLC_F3_MSG_ENUM_30, //            urbb[2]=%08x [1]=%08x [0]=%08x
  GRLC_F3_MSG_ENUM_31, //                [4]=%08x [3]=%08x len=%u dec 
  GRLC_F3_MSG_ENUM_32, // ERLD cnt_a=%08x cnt_b=%08x cnt_c=%08x
  GRLC_F3_MSG_ENUM_33, // ERLD enter dump mode on bsn=%d
  GRLC_F3_MSG_ENUM_34, // ERLD dump hdr bsn=%d %08x %08x
  GRLC_F3_MSG_ENUM_35, // ERLD dump 1st pld %08x %08x %08x
  GRLC_F3_MSG_ENUM_36, // ERLD dump 2nd pld %08x %08x %08x
  GRLC_F3_MSG_ENUM_37, // GRLD blk_ptr is NULL
  GRLC_F3_MSG_ENUM_38, // GRLD NeLeBsFE %08x
  GRLC_F3_MSG_ENUM_39, // GRLD NeLeBsFE %08x LiMELiME %08x
  GRLC_F3_MSG_ENUM_40, // GRLD NeLeBsFE %08x LiMELiME %08x %08x
  GRLC_F3_MSG_ENUM_41, // dsm_a_len=%u len=%u cs=%d
  GRLC_F3_MSG_ENUM_42, // GRLD DATA BLK: %08x %08x %08x
  GRLC_F3_MSG_ENUM_43, // GRLD Diag DL ===============================
  GRLC_F3_MSG_ENUM_44, // GRLD Diag State=%d APDUs=%d AOcts=%d
  GRLC_F3_MSG_ENUM_45, // GRLD Diag          UPDUs=%d UOcts=%d
  GRLC_F3_MSG_ENUM_46, // GRLD Diag Filled-PDUs=%d Filled-Octs=%d
  GRLC_F3_MSG_ENUM_47, // GRLD Diag Acknack-Reqs=%d BSN=%d
  GRLC_F3_MSG_ENUM_48, // GRLD Diag VQ=%d VR=%d SSN=%d
  GRLC_F3_MSG_ENUM_49, // GRLD Diag CntA=%d CntB=%d CntC=%d
  GRLC_F3_MSG_ENUM_50, // GRLD Info state=%d Event=%d
  GRLC_F3_MSG_ENUM_51, // GRLD Info VR=%d VQ=%d SSN=%d
  GRLC_F3_MSG_ENUM_52, // ELRU e_pg_calc_init octets = pad_octets(%u)
  GRLC_F3_MSG_ENUM_53, // ELRU e_pg_calc_init octets(%u) > pad_octets(%u)
  GRLC_F3_MSG_ENUM_54, // ELRU e_pg_calc_init octets(%u) < pad_octets(%u)
  GRLC_F3_MSG_ENUM_55, // ERLU e_pg_calc_init clr b_li_o_exists
  GRLC_F3_MSG_ENUM_56, // GRLU pgadj tbc=%u new=%u abs=%u
  GRLC_F3_MSG_ENUM_57, // GRLU pgadj tbc=%u new=%u abs=%u
  GRLC_F3_MSG_ENUM_58, // GRLU pg close %u new grp %u tbc=%u
  GRLC_F3_MSG_ENUM_59, // GRLU ch_grp (1) cv=%u ptx_cv=%u pak_cv=%u
  GRLC_F3_MSG_ENUM_60, // GRLU ch_grp (2) cv=%u ptx_cv=%u pak_cv=%u
  GRLC_F3_MSG_ENUM_61, // GRLU ch_grp (3) cv=%u ptx_cv=%u pak_cv=%u
  GRLC_F3_MSG_ENUM_62, // GRLU ch_grp (1) both CV NOT active ptx_cv=%u pak_cv=%u cv=%u grp
  GRLC_F3_MSG_ENUM_63, // GRLU ch_grp (2) rep, grp ptx_cv=%u pak_cv=%u cv=%u grp
  GRLC_F3_MSG_ENUM_64, // GRLU ch_grp (4) no rep, no grp ptx_cv=%u pak_cv=%u cv=%u !grp
  GRLC_F3_MSG_ENUM_65, // GRLU pg add %u open tbc=%u num_blks=%u
  GRLC_F3_MSG_ENUM_66, // GRLU pg close %u new grp %u tbc=%u
  GRLC_F3_MSG_ENUM_67, // GRLU pgxf ONQ add psn %u len = %u blks=%u
  GRLC_F3_MSG_ENUM_68, // GRLU pgxf ONQ add psn %u len = %u blks=%u
  GRLC_F3_MSG_ENUM_69, // GRLU blk_size=0, cs=%d tlli_cs=%d ti=%d
  GRLC_F3_MSG_ENUM_70, // GRLU blk_size=0, mcs=%d tlli_cs=%d ti=%d
  GRLC_F3_MSG_ENUM_71, // GRLU blk_size=0, cs=%d tlli_cs=%d ti=%d
  GRLC_F3_MSG_ENUM_72, // GRLU blk_size=0, mcs=%d tlli_cs=%d ti=%d
  GRLC_F3_MSG_ENUM_73, // ul.ptx_cv = BS_CV_CONST in pg_clr_tbc_info()
  GRLC_F3_MSG_ENUM_74, // GRLU pgxf 1st psn %u len = %u blks=%u
  GRLC_F3_MSG_ENUM_75, // GRLU pgxf add psn %u len = %u blks=%u
  GRLC_F3_MSG_ENUM_76, // GRLU pgxf ONQ add psn %u len=%u blks=%u
  GRLC_F3_MSG_ENUM_77, // GRLU pgxf tbc=%u new=%u abs=%u
  GRLC_F3_MSG_ENUM_78, // GRLU pgxf 1st psn %u len = %u blks=%u
  GRLC_F3_MSG_ENUM_79, // GRLU pgxf add psn %u len = %u blks=%u
  GRLC_F3_MSG_ENUM_80, // GRLU pgxf ONQ add psn %u len=%u blks=%u
  GRLC_F3_MSG_ENUM_81, // ERLU pgxf tbc=%u new=%u abs=%u
  GRLC_F3_MSG_ENUM_82, // GRLU pg cur pdu NULL (tbc=%u)
  GRLC_F3_MSG_ENUM_83, // GRLU pg INIT psn %u tbc=%u last_p=%u
  GRLC_F3_MSG_ENUM_84, // GRLU pgxf tbc=%u new=%u abs=%u
  GRLC_F3_MSG_ENUM_85, // GRLU pgxf add psn %u len = %u blks=%u
  GRLC_F3_MSG_ENUM_86, // GRLU pg check_tx_cv ret FALSE psn=%u tbc=%u abs=%u
  GRLC_F3_MSG_ENUM_87, // group_pdus calls group_pdus_onq b_check=%u
  GRLC_F3_MSG_ENUM_88, // GRLU PGS close last_psn %u tbc=%u num_blks=%u
  GRLC_F3_MSG_ENUM_89, // GRLU PGS bad state %d
  GRLC_F3_MSG_ENUM_90, // GRLU gp end tbc=%u psn=%u last_psn=%u
  GRLC_F3_MSG_ENUM_91, // GRLU unpg pdu NULL
  GRLC_F3_MSG_ENUM_92, // GRLU PGS ungp neg pg_pdus=%u
  GRLC_F3_MSG_ENUM_93, // GRLU PGS ungp neg pg_octs=%u len=%u
  GRLC_F3_MSG_ENUM_94, // GRLU ungp pg_close psn=%u
  GRLC_F3_MSG_ENUM_95, // GRLU ungp xfer psn(%u) to psn(%u)
  GRLC_F3_MSG_ENUM_96, // GRLU ungp xfer psn(%u) to psn(%u)
  GRLC_F3_MSG_ENUM_97, // GRLU pg cur pdu NULL (tbc=%u)
  GRLC_F3_MSG_ENUM_98, // GRLU pg INIT psn %u tbc=%u last_p=%u
  GRLC_F3_MSG_ENUM_99, // GRLU pgxf add psn %u len = %u blks=%u
  GRLC_F3_MSG_ENUM_100, // GRLU PGS close last_psn %u tbc=%u num_blks=%u
  GRLC_F3_MSG_ENUM_101, // GRLU PGS bad state %d
  GRLC_F3_MSG_ENUM_102, // GRLU gp end tbc=%u psn=%u last_psn=%u
  GRLC_F3_MSG_ENUM_103, // Invalid PL1 sig %d
  GRLC_F3_MSG_ENUM_104, // ERLD EDGE IR operational window size %d
  GRLC_F3_MSG_ENUM_105, // GRLC: Get Static Mes Buf: Lo Water!
  GRLC_F3_MSG_ENUM_106, // ERLU FCtrl-SUSPEND E_HI_WM=%u E_LO_WM=%u llc_octets=%lu temppdu=%u
  GRLC_F3_MSG_ENUM_107, // GRLU FCtrl-SUSPEND G_HI_WM=%u G_LO_WM=%u llc_octets=%lu temppdu=%u
  GRLC_F3_MSG_ENUM_108, // ERLU FCtrl-RESUME  E_HI_WM=%u E_LO_WM=%u llc_octets=%lu temppdu=%u
  GRLC_F3_MSG_ENUM_109, // GRLU FCtrl-RESUME  G_HI_WM=%u G_LO_WM=%u llc_octets=%lu temppdu=%u
  GRLC_F3_MSG_ENUM_110, // GRLU LLC Put: Msg NULL
  GRLC_F3_MSG_ENUM_111, // GRLU LLC Put: Invalid Prim %d
  GRLC_F3_MSG_ENUM_112, // GRLC error CFA LOG unknown message
  GRLC_F3_MSG_ENUM_113, // Invalid MAC sig %d
  GRLC_F3_MSG_ENUM_114, // GRLU re-start await_ass_guard timer. Time in ms %d
  GRLC_F3_MSG_ENUM_115, // GRLU put %u
  GRLC_F3_MSG_ENUM_116, // GRLU put_paq full 0CtrMxCt=%08x in=%d out=%d
  GRLC_F3_MSG_ENUM_117, // GRLU triggering panic reset
  GRLC_F3_MSG_ENUM_118, // GRLU put_paq NULL
  GRLC_F3_MSG_ENUM_119, // GRLU make_cur psn=%u ofs=%u cur_p=%u
  GRLC_F3_MSG_ENUM_120, // GRLU ##make_cur psn=%u len(%u) <= offset(%u)
  GRLC_F3_MSG_ENUM_121, // GRLU ##make_cur dup returns NULL
  GRLC_F3_MSG_ENUM_122, // GRLU ## make_cur len=0. psn=%u
  GRLC_F3_MSG_ENUM_123, // GRLU make_cur unput %u
  GRLC_F3_MSG_ENUM_124, // GRLU ##make_cur psn=%u len(%u) <= offset(%u)
  GRLC_F3_MSG_ENUM_125, // GRLU ##make_cur %u not in PAQ cur_psn=%u
  GRLC_F3_MSG_ENUM_126, // GRLU ##unput_paq dup returns NULL
  GRLC_F3_MSG_ENUM_127, // GRLU unput_paq clrs cur_pdu_ptr as well
  GRLC_F3_MSG_ENUM_128, // GRLU ##unput_paq dup ptr NULL psn=%u dup=%x ptr=%x
  GRLC_F3_MSG_ENUM_129, // GRLU psn %u not in PAQ ctr=%u cur_p=%u
  GRLC_F3_MSG_ENUM_130, // GRLU SEND UL TX COMPLETE IND to LLC
  GRLC_F3_MSG_ENUM_131, // freeing llc pdu with psn=%d
  GRLC_F3_MSG_ENUM_132, // GRLU SEND UL TX COMPLETE IND to LLC
  GRLC_F3_MSG_ENUM_133, // GRLU seek vs=%d vsf=%d p_vsf=%d
  GRLC_F3_MSG_ENUM_134, // ERLU seek_cv0 va=%d ind=%d psn=%u
  GRLC_F3_MSG_ENUM_135, // GRLU seek_cv0 va=%d ind=%d psn=%u
  GRLC_F3_MSG_ENUM_136, // GRLU ##seek ret_psn=%u cur_p=%u
  GRLC_F3_MSG_ENUM_137, // ERLD Rcved Invalid 1st payload with CRC=%d bsn1=%03x [%d]
  GRLC_F3_MSG_ENUM_138, // ERLD Rcved 1st payload with CRC=0 & dsm_ptr=NULL bsn1=%03x [%d]
  GRLC_F3_MSG_ENUM_139, // ERLD Rcved Invalid 2nd payload with CRC=%d & dsm_ptr=0x bsn2=%03x [%d]
  GRLC_F3_MSG_ENUM_140, // ERLD Rcved 2nd payload with CRC=0 & dsm_ptr= NULL bsn2=%03x [%d]
  GRLC_F3_MSG_ENUM_141, // ERLD Rcved MCS=%d with valid 2nd payload [%d]
  GRLC_F3_MSG_ENUM_142, // ERLD U Rcved Invalid 1st payload with CRC=1 & dsm_ptr= NULL [%d]
  GRLC_F3_MSG_ENUM_143, // ERLD U Rcved 1st payload with CRC=0 & dsm_ptr= NULL [%d]
  GRLC_F3_MSG_ENUM_144, // ERLD U Rcved Invalid 2nd payload with CRC=1 & dsm_ptr=NULL [%d]
  GRLC_F3_MSG_ENUM_145, // ERLD U Rcved 2nd payload with CRC=0 & dsm_ptr= NULL [%d]
  GRLC_F3_MSG_ENUM_146, // ERLD U Rcved MCS=%d with valid 2nd payload [%d]
  GRLC_F3_MSG_ENUM_147, // ERLD Rcved both splits for bsn=%d
  GRLC_F3_MSG_ENUM_148, // ERLD Rcved duplicate split bsn=%d, rcved_spb=%d, spb=%d
  GRLC_F3_MSG_ENUM_149, // ERLD partial split block dsm freed bsn=%u
  GRLC_F3_MSG_ENUM_150, // ERLD vr_max has gone out of window,update vr_max %d vq %d
  GRLC_F3_MSG_ENUM_151, // GRLD UNACK discard bsn=%d out of seq, vr=%d, vq=%d
  GRLC_F3_MSG_ENUM_152, // GRLD UNACK re-ass bsn=%d out of seq, vr=%d, vq=%d
  GRLC_F3_MSG_ENUM_153, // ELRD gUrbb Start_bsn=%u Urbb[4]=%08x Urbb[3]=%08x
  GRLC_F3_MSG_ENUM_154, // ELRD gUrbb Urbb[2]=%08x Urbb[1]=%08x Urbb[0]=%08x
  GRLC_F3_MSG_ENUM_155, // ERLD gcrbb bm_len=%u avail_space_for_bm=%u
  GRLC_F3_MSG_ENUM_156, // ERLD gcrbb run=%u len=%u ind=%u
  GRLC_F3_MSG_ENUM_157, // ERLD gcrbb bad encode run=%u len=%u ind=%u
  GRLC_F3_MSG_ENUM_158, // ELRD gcrbb dropped QR ES/P=%d
  GRLC_F3_MSG_ENUM_159, // ELRD gcrbb pos=%u tcode=%x, tcode_len=%u
  GRLC_F3_MSG_ENUM_160, // ELRD gcrbb pos=%u mcode=%x, mcode_len=%u
  GRLC_F3_MSG_ENUM_161, // ELRD gcrbb pos=%u crbb[1]=%08x crbb[0]=%08x
  GRLC_F3_MSG_ENUM_162, // ELRD       pos=%u crbb[3]=%08x crbb[2]=%08x
  GRLC_F3_MSG_ENUM_163, // res_cnt greater than or equal to MAX_CODE_WORD_ARRAY_SIZE oor %u
  GRLC_F3_MSG_ENUM_164, // ERLD gcrbb crbb_len=%u, esn_crbb=%u
  GRLC_F3_MSG_ENUM_165, // GRLD un-exp DL empty
  GRLC_F3_MSG_ENUM_166, // GRLD VN Receive State Err: BSN %d VN %d
  GRLC_F3_MSG_ENUM_167, // ERLD un-exp DL Q empty 
  GRLC_F3_MSG_ENUM_168, // ERLD VN Receive State Err: BSN %d EVN %d
  GRLC_F3_MSG_ENUM_169, // ERLD EPDAN_REQ mask without msg
  GRLC_F3_MSG_ENUM_170, // ERLD EPDAN_REQ without sig in DPQueue
  GRLC_F3_MSG_ENUM_171, // ERLD incoming BSN2 mcs 8 marked TRUE=%u mcs=%u
  GRLC_F3_MSG_ENUM_172, // ERLD Did not respond to EPDAN_REQ 
  GRLC_F3_MSG_ENUM_173, // ERLD EPDAN_REQ mask without msg
  GRLC_F3_MSG_ENUM_174, // GRLD un-exp DL empty
  GRLC_F3_MSG_ENUM_175, // GRLD A BSN=%d SP=%d FBI=%d
  GRLC_F3_MSG_ENUM_176, // ERLD EPDAN_REQ with DL Q empty 
  GRLC_F3_MSG_ENUM_177, // ERLD EPDAN_REQ mask without msg
  GRLC_F3_MSG_ENUM_178, // ERLD EPDAN_REQ without sig in DPQueue
  GRLC_F3_MSG_ENUM_179, // ERLD Did not respond to EPDAN_REQ 
  GRLC_F3_MSG_ENUM_180, // GRLD un-exp DL empty
  GRLC_F3_MSG_ENUM_181, // GRLD U BSN=%d SP=%d FBI=%d
  GRLC_F3_MSG_ENUM_182, // GRLD U free %d vr=%d vq=%d
  GRLC_F3_MSG_ENUM_183, // GRLD RX_U FBI Sent PCAck
  GRLC_F3_MSG_ENUM_184, // ERLD un-exp with DL Q empty 
  GRLC_F3_MSG_ENUM_185, // GRLD RX_U FBI in RX_UNACK Sent PCAck
  GRLC_F3_MSG_ENUM_186, // ERLD EPDAN_REQ mask without msg
  GRLC_F3_MSG_ENUM_187, // ERLD EPDAN_REQ without sig in DPQueue
  GRLC_F3_MSG_ENUM_188, // ERLD Did not respond to EPDAN_REQ 
  GRLC_F3_MSG_ENUM_189, // ERLD U EPDAN_REQ mask without msg
  GRLC_F3_MSG_ENUM_190, // ERLD RX_UNACK_PENDING_RELEASE Sent PCAck
  GRLC_F3_MSG_ENUM_191, // ERLD U EPDAN_REQ mask without msg
  GRLC_F3_MSG_ENUM_192, // GRLD un-exp DL empty
  GRLC_F3_MSG_ENUM_193, // GRLD U BSN=%d SP=%d FBI=%d
  GRLC_F3_MSG_ENUM_194, // GRLD RX_U_P_R FBI Sent PCAck
  GRLC_F3_MSG_ENUM_195, // ERLD U PH_DATA_IND with DL Q empty 
  GRLC_F3_MSG_ENUM_196, // GRLD RX_U FBI in PEND_REL Sent PCAck
  GRLC_F3_MSG_ENUM_197, // ERLD epdan len_no_qrep=%u len_with_qrep=%u
  GRLC_F3_MSG_ENUM_198, // ERLD ms_out_mem RESET vq %d vr_max %d oper_ws %d
  GRLC_F3_MSG_ENUM_199, // ERLD ms_out_mem SET vq %d vr_max %d oper_ws %d
  GRLC_F3_MSG_ENUM_200, // GRLD No avail space for EPDAN desc len_no_qrep %d, len_qrep %d 
  GRLC_F3_MSG_ENUM_201, // GRLD No avail space for EPDAN desc when vq != vr
  GRLC_F3_MSG_ENUM_202, // ELRD urbb dropped QR ES/P=%d
  GRLC_F3_MSG_ENUM_203, // ELRD urbb dropped QR ES/P=%d
  GRLC_F3_MSG_ENUM_204, // ERLD CRBB active
  GRLC_F3_MSG_ENUM_205, // ELRD urbb dropped QR ES/P=%d
  GRLC_F3_MSG_ENUM_206, // ERLD epdan ESN_CRBB(%u) beyond VR-1 (%u)
  GRLC_F3_MSG_ENUM_207, // ERLD urbb dropped QR ES/P=%d
  GRLC_F3_MSG_ENUM_208, // ERLD No URBB/CRBB. Implies V(R)=V(Q)!!
  GRLC_F3_MSG_ENUM_209, // ERLD epdan ESN_CRBB(%u) beyond VR-1 (%u)
  GRLC_F3_MSG_ENUM_210, // ERLD No URBB/CRBB. Implies V(R)=V(Q)!!
  GRLC_F3_MSG_ENUM_211, // GRLD Poll Queue Overflow %d
  GRLC_F3_MSG_ENUM_212, // GRLD clr status %d to %d-1
  GRLC_F3_MSG_ENUM_213, // GRLD clr status bad mode
  GRLC_F3_MSG_ENUM_214, // GRLD PSHO MR_DL_PSHO_COMPLETE_IND in psho_status=%d psho_type=%d reset_flag=%d
  GRLC_F3_MSG_ENUM_215, // GRLD NULL/REL ##tbf mode mismatch for ps handover
  GRLC_F3_MSG_ENUM_216, // GRLD NULL/REL dl_ass_ind_present FALSE for ps handover
  GRLC_F3_MSG_ENUM_217, // GRLD NULL/REL Rcvd DL_ASS mode=%d tlli=0x%x ws=%u
  GRLC_F3_MSG_ENUM_218, // GRLD NULL/REL PSHO Invalid psho_status
  GRLC_F3_MSG_ENUM_219, // GRLD X_FER PSHO MR_DL_PSHO_COMPLETE_IND in psho_status=%d psho_type=%d reset_flag=%d
  GRLC_F3_MSG_ENUM_220, // GRLD X_FER PSHO WTOG successfull
  GRLC_F3_MSG_ENUM_221, // GRLD ##tbf mode mismatch for ps handover
  GRLC_F3_MSG_ENUM_222, // GRLD XFER dl_ass_ind_present FALSE for ps handover
  GRLC_F3_MSG_ENUM_223, // ERLD X_FER PSHO RX_A  WS=%u in DL_ASS_IND curr ws=%u not updated nts %d
  GRLC_F3_MSG_ENUM_224, // GRLD X_FER PSHO GPRS DL_ASS
  GRLC_F3_MSG_ENUM_225, // GRLD X_FER PSHO GPRS DL_ASS
  GRLC_F3_MSG_ENUM_226, // GRLD XFER dl_ass_ind_present FALSE for ps handover
  GRLC_F3_MSG_ENUM_227, // GRLD X_FER PSHO Rcvd DL_ASS mode=%d tlli=0x%x ws=%u
  GRLC_F3_MSG_ENUM_228, // GRLD X_FER PSHO ##invalid psho_type=%d
  GRLC_F3_MSG_ENUM_229, // GRLD X_FER PSHO ##invalid psho_status
  GRLC_F3_MSG_ENUM_230, // GRLD ##Invalid tbf mode
  GRLC_F3_MSG_ENUM_231, // GRLD Blind DL FIFO storage full,ignoring new incoming blocks
  GRLC_F3_MSG_ENUM_232, // GRLD DL FIFO EMPTY
  GRLC_F3_MSG_ENUM_233, // GRLD DL FIFO EMPTY
  GRLC_F3_MSG_ENUM_234, // GRLD Invalid TBF MODE
  GRLC_F3_MSG_ENUM_235, // GRLD PSHO Blind Data Block counter = %u
  GRLC_F3_MSG_ENUM_236, // GRLU calc_cv num_timeslots = 0 Forced to 1
  GRLC_F3_MSG_ENUM_237, // GRLU calc_cv close psn %u last_p=%u
  GRLC_F3_MSG_ENUM_238, // ERLU e_calc_cv num_timeslots = 0 Forced to 1
  GRLC_F3_MSG_ENUM_239, // ERLU e_calc CV=%u tot=%u abs=%u
  GRLC_F3_MSG_ENUM_240, // ERLU e_calc CV top=%u bot=%u seg_ind=%u
  GRLC_F3_MSG_ENUM_241, // ERLU e_calc CV started %u tot=%u abs=%u
  GRLC_F3_MSG_ENUM_242, // ERLU e_calc CV top=%u bot=%u K=%u
  GRLC_F3_MSG_ENUM_243, // ERLU e_calc CV ts=%u
  GRLC_F3_MSG_ENUM_244, // ERLU e_calc_cv close psn %u ptx_cv=%u pak_cv=%u
  GRLC_F3_MSG_ENUM_245, // GRLU sgm 0 p=%u pl=%u last_p=%u
  GRLC_F3_MSG_ENUM_246, // GRLU SGM p=%u l=%u rem=%u
  GRLC_F3_MSG_ENUM_247, // GRLU ##SGM cur=NULL
  GRLC_F3_MSG_ENUM_248, // GRLU ##SGM cur=NULL
  GRLC_F3_MSG_ENUM_249, // GRLU Info SGM rpl=0 #=%d pl=%d r=%d
  GRLC_F3_MSG_ENUM_250, // GRLU ##sgm segment_head returns NULL
  GRLC_F3_MSG_ENUM_251, // GRLU ##sgm non NULL after augment_tail
  GRLC_F3_MSG_ENUM_252, // GRLU ##sgm segment_head returns NULL
  GRLC_F3_MSG_ENUM_253, // GRLU ##sgm non NULL after augment_tail
  GRLC_F3_MSG_ENUM_254, // GRLU SGM Case 2
  GRLC_F3_MSG_ENUM_255, // GRLU ##sgm segment_head returns NULL
  GRLC_F3_MSG_ENUM_256, // GRLU ##sgm non NULL after augment_tail
  GRLC_F3_MSG_ENUM_257, // GRLU ##sgm segment_head returns NULL
  GRLC_F3_MSG_ENUM_258, // GRLU ##sgm non NULL after augment_tail
  GRLC_F3_MSG_ENUM_259, // GRLU ##sgm segment_head returns NULL
  GRLC_F3_MSG_ENUM_260, // GRLU ##sgm non NULL after augment_tail
  GRLC_F3_MSG_ENUM_261, // SGM s_last=TRUE atta bsn=%d CV=%d
  GRLC_F3_MSG_ENUM_262, // GRLU ##sgm cv=%d psn(%u) != last_p(%u)
  GRLC_F3_MSG_ENUM_263, // ERLU esgm 0 p=%u pl=%u last_p=%u
  GRLC_F3_MSG_ENUM_264, // ERLU ESGM p=%u l=%u rem=%u
  GRLC_F3_MSG_ENUM_265, // ERLU ##eSGM cur=NULL
  GRLC_F3_MSG_ENUM_266, // ERLU ##eSGM cur=NULL
  GRLC_F3_MSG_ENUM_267, // ERLU Info eSGM rpl=0 #=%d pl=%d r=%d
  GRLC_F3_MSG_ENUM_268, // ERLU ##esgm segment_head returns NULL
  GRLC_F3_MSG_ENUM_269, // ERLU ##esgm non NULL after augment_tail
  GRLC_F3_MSG_ENUM_270, // ERLU ##esgm segment_head returns NULL
  GRLC_F3_MSG_ENUM_271, // ERLU ##esgm non NULL after augment_tail
  GRLC_F3_MSG_ENUM_272, // ERLU eSGM precise-fit bsn=%u abs_bsn=%u
  GRLC_F3_MSG_ENUM_273, // ERLU ##esgm segment_head returns NULL
  GRLC_F3_MSG_ENUM_274, // ERLU ##esgm non NULL after augment_tail
  GRLC_F3_MSG_ENUM_275, // ESGM s_last=TRUE atta bsn=%d CV=%d
  GRLC_F3_MSG_ENUM_276, // ERLU ##esgm cv=%d psn(%u) != last_p(%u)
  GRLC_F3_MSG_ENUM_277, // GRLU ##SGML %u being cleared not in PAQ
  GRLC_F3_MSG_ENUM_278, // GRLU SGML psn=%u last_psn=%u seg=%d
  GRLC_F3_MSG_ENUM_279, // T4SeekFreeNode Bin Table Full
  GRLC_F3_MSG_ENUM_280, // T4 one run length binary tree
  GRLC_F3_MSG_ENUM_281, //      Index   Left  Right  Runlength
  GRLC_F3_MSG_ENUM_282, // { /* %3u */  %3u,   %3u, 
  GRLC_F3_MSG_ENUM_283, //                            %5u, },
  GRLC_F3_MSG_ENUM_284, // T4 zero run length binary tree
  GRLC_F3_MSG_ENUM_285, //      Index   Left  Right  Runlength
  GRLC_F3_MSG_ENUM_286, // { /* %3u */  %3u,   %3u, 
  GRLC_F3_MSG_ENUM_287, //                            %5u, },
  GRLC_F3_MSG_ENUM_288, // T4DecodeORLTest failed at char %u uLen = %u
  GRLC_F3_MSG_ENUM_289, // T4VerifyORLTable FAILED raw ind = %3u, RL = %3u decoded RL = %3u
  GRLC_F3_MSG_ENUM_290, // T4VerifyORLTable PASS raw ind %3u, RL = %3u decoded RL = %3u
  GRLC_F3_MSG_ENUM_291, // T4DecodeZRLTest failed at char %u uLen = %u
  GRLC_F3_MSG_ENUM_292, // T4VerifyZRLTable FAILED raw ind = %3u, RL = %3u decoded RL = %3u
  GRLC_F3_MSG_ENUM_293, // T4VerifyZRLTable PASS raw ind %3u, RL = %3u decoded RL = %3u
  GRLC_F3_MSG_ENUM_294, // T4DecodeORLTest failed at char %u uLen = %u
  GRLC_F3_MSG_ENUM_295, // T4DecodeORLTest ORL = %u at char %u of uLen = %u
  GRLC_F3_MSG_ENUM_296, // T4DecodeORLTest ORL failed u = %u uLen = %u
  GRLC_F3_MSG_ENUM_297, // T4AFn Pos = %u Ack = %u
  GRLC_F3_MSG_ENUM_298, // ERLU bad CRBB length 0
  GRLC_F3_MSG_ENUM_299, // ERLU t4_dec run length=%u #bits=%u bit=%u
  GRLC_F3_MSG_ENUM_300, // ERLU bad node CurInd=%u bit=%u bit_cnt=%u
  GRLC_F3_MSG_ENUM_301, // ERLU t4_dec search failed at node %u after %u bits
  GRLC_F3_MSG_ENUM_302, // #decoded = %u
  GRLC_F3_MSG_ENUM_303, // ERLD t4enc OOR run=%u run_length=%u
  GRLC_F3_MSG_ENUM_304, // ERLD t4enc run=%u run_length=%u
  GRLC_F3_MSG_ENUM_305, // ERLD       mc=x%03x mclen=%u
  GRLC_F3_MSG_ENUM_306, // ERLD       tc=x%03x tclen=%u
  GRLC_F3_MSG_ENUM_307, // GRLD TEST_MODE_B Rcved out of Win bsn=%d, vq=%d, vr=%d
  GRLC_F3_MSG_ENUM_308, // ERLD TEST_MODE Rcved both splits for bsn=%d
  GRLC_F3_MSG_ENUM_309, // ERLD TEST_MODE Rcved duplicate split bsn=%d, rcved_spb=%d, spb=%d
  GRLC_F3_MSG_ENUM_310, // ERLD TEST_MODE did not update V(R)=%d, dl[gas_id].bsn=%d, dl[gas_id].vq=%d
  GRLC_F3_MSG_ENUM_311, // ERLD TEST_MODE Rcved out of win bsn=%d vq=%d vr=%d
  GRLC_F3_MSG_ENUM_312, // ERLD vr_max has gone out of window,update vr_max %d vq %d
  GRLC_F3_MSG_ENUM_313, // GRLD un-exp DL empty
  GRLC_F3_MSG_ENUM_314, // GRLD Ack DS_TEST_MODE B Rcvd PH_DATA_IND bsn=%d 
  GRLC_F3_MSG_ENUM_315, // GRLD ACK TEST_MODE B 1st DL blk bsn=%d vq=%d vr=%d
  GRLC_F3_MSG_ENUM_316, // GRLD ACK TEST_MODE rcved bsn=%d with vn=%d
  GRLC_F3_MSG_ENUM_317, // GRLD DS_TEST_MODE error in fifo put block
  GRLC_F3_MSG_ENUM_318, // RLD DS_TEST_MODE B ACK sent bsn=%d tn=%d ctr=%d
  GRLC_F3_MSG_ENUM_319, // GRLD DS_TEST_MODE failed to free payload
  GRLC_F3_MSG_ENUM_320, // GRLD Ack DS_TEST_MODE B failed to free payload
  GRLC_F3_MSG_ENUM_321, // ERLD TEST_MODE PH_DATA_IND with DL Q empty 
  GRLC_F3_MSG_ENUM_322, // ERLD TEST_MODE Rcved 1st payload with CRC=1 & dsm_ptr= NULL [%d]
  GRLC_F3_MSG_ENUM_323, // ERLD TEST_MODE Rcved 1st payload with CRC=0 & dsm_ptr= NULL [%d]
  GRLC_F3_MSG_ENUM_324, // ERLD TEST_MODE Rcved 2nd payload with CRC=1 & dsm_ptr=NULL [%d]
  GRLC_F3_MSG_ENUM_325, // ERLD TEST_MODE Rcved 2nd payload with CRC=0 & dsm_ptr= NULL [%d]
  GRLC_F3_MSG_ENUM_326, // ERLD TEST_MODE Rcved MCS=%d with valid 2nd payload [%d]
  GRLC_F3_MSG_ENUM_327, // ERLD ACK TEST_MODE Rcved bsn1=%d and bsn2=%d
  GRLC_F3_MSG_ENUM_328, // ERLD ACK TEST_MODE rcved bsn=%d failed crc
  GRLC_F3_MSG_ENUM_329, // ERLD ACK TEST_MODE rcved bsn=%d with vn=%d
  GRLC_F3_MSG_ENUM_330, // ERLD EPDAN_REQ mask without msg
  GRLC_F3_MSG_ENUM_331, // ERLD EPDAN_REQ without sig in DPQueue
  GRLC_F3_MSG_ENUM_332, // ERLD DS_TEST_MODE error in fifo put block 
  GRLC_F3_MSG_ENUM_333, // ERLD DS_TEST_MODE B ACK sent bsn1=%d bsn2=%d ctr=%d
  GRLC_F3_MSG_ENUM_334, // ERLD DS_TEST_MODE ACK failed to free payload
  GRLC_F3_MSG_ENUM_335, // ERLD DS_TEST_MODE ACK failed to free payload
  GRLC_F3_MSG_ENUM_336, // ERLD Did not respond to EPDAN_REQ 
  GRLC_F3_MSG_ENUM_337, // GRLD DS_TEST_MODE un-exp DL fifo empty
  GRLC_F3_MSG_ENUM_338, // GRLD UnAck DS_TEST_MODE B Rcvd PH_DATA_IND bsn=%d 
  GRLC_F3_MSG_ENUM_339, // GRLD RX_U FBI Sent PCAck
  GRLC_F3_MSG_ENUM_340, // GRLD DS_TEST_MODE error in fifo put block 
  GRLC_F3_MSG_ENUM_341, // RLD DS_TEST_MODE B UNACK sent bsn=%d tn=%d ctr=%d
  GRLC_F3_MSG_ENUM_342, // GRLD DS_TEST_MODE failed to free payload
  GRLC_F3_MSG_ENUM_343, // GRLD Ack DS_TEST_MODE B failed to free payload
  GRLC_F3_MSG_ENUM_344, // GRLD DS_TEST_MODE invalid mode %d
  GRLC_F3_MSG_ENUM_345, // ERLD TEST_MODE PH_DATA_IND with DL Q empty 
  GRLC_F3_MSG_ENUM_346, // ERLD TEST_MODE Rcved 1st payload with CRC=1 & dsm_ptr= NULL [%d]
  GRLC_F3_MSG_ENUM_347, // ERLD TEST_MODE Rcved 1st payload with CRC=0 & dsm_ptr= NULL [%d]
  GRLC_F3_MSG_ENUM_348, // ERLD REST_MODE Rcved 2nd payload with CRC=1 & dsm_ptr=NULL [%d]
  GRLC_F3_MSG_ENUM_349, // ERLD TEST_MODE Rcved 2nd payload with CRC=0 & dsm_ptr= NULL [%d]
  GRLC_F3_MSG_ENUM_350, // ERLD TEST_MODE Rcved MCS=%d with valid 2nd payload [%d]
  GRLC_F3_MSG_ENUM_351, // ERLD RX_U FBI Sent PCAck
  GRLC_F3_MSG_ENUM_352, // ERLD EPDAN_REQ mask without msg
  GRLC_F3_MSG_ENUM_353, // ERLD EPDAN_REQ without sig in DPQueue
  GRLC_F3_MSG_ENUM_354, // ERLD DS_TEST_MODE error in fifo put block 
  GRLC_F3_MSG_ENUM_355, // ERLD DS_TEST_MODE B sent bsn1=%d bsn2=%d tn=%d
  GRLC_F3_MSG_ENUM_356, // ERLD DS_TEST_MODE failed to free payload
  GRLC_F3_MSG_ENUM_357, // ERLD DS_TEST_MODE failed to free payload
  GRLC_F3_MSG_ENUM_358, // GRLU init_mcs %u or cur_res.cs %u is oor
  GRLC_F3_MSG_ENUM_359, // GRLU pdu-txed psn=%u vs=%u seg_ind=%u
  GRLC_F3_MSG_ENUM_360, // GRLU pdu-txed TRUE bsn=%u last_psn=%u
  GRLC_F3_MSG_ENUM_361, // GRLU pdu-txed TRUE cv=0
  GRLC_F3_MSG_ENUM_362, // GRLU pdu-txed FALSE - will fit in next bsn with LI=0
  GRLC_F3_MSG_ENUM_363, // GRLU pdu-txed TRUE psn=%d fits with L1
  GRLC_F3_MSG_ENUM_364, // GRLU pdu-txed TRUE bsn=%u last_psn=%u
  GRLC_F3_MSG_ENUM_365, // GRLU pdu-txed TRUE cv=0 or good fit
  GRLC_F3_MSG_ENUM_366, // GRLU pdu-txed bad tbf_mode
  GRLC_F3_MSG_ENUM_367, // GRLU pdu-txed FALSE 
  GRLC_F3_MSG_ENUM_368, // GRLU A ##Dup Exhaustion %d vb=%d ofs=%u
  GRLC_F3_MSG_ENUM_369, // dsm_ptr=%x a_len=%u dup=%x
  GRLC_F3_MSG_ENUM_370, // GRLU U ##dup not NULL bsn=%d
  GRLC_F3_MSG_ENUM_371, // GRLU U ##Dup Exhaustion %d vb=%d ofs=%u
  GRLC_F3_MSG_ENUM_372, // dsm_ptr=%x a_len=%u dup=%x
  GRLC_F3_MSG_ENUM_373, // GRLU send_b bad put result %d bsn=%d
  GRLC_F3_MSG_ENUM_374, // GRLU A Potential Dup Exhaustion %d vb=%d ofs=%u
  GRLC_F3_MSG_ENUM_375, // dsm_ptr=%x a_len=%u dup=%x
  GRLC_F3_MSG_ENUM_376, // BSN=%u vb=%u data_len=%u
  GRLC_F3_MSG_ENUM_377, //  dsm_ptr=%p dup=%p rem_cnt=%u
  GRLC_F3_MSG_ENUM_378, //  mcs=%u ptx_miss_cnt=%u nack_miss_cnt=%u
  GRLC_F3_MSG_ENUM_379, //  1st_psn=%u last_psn=%u rlc_mode=%u
  GRLC_F3_MSG_ENUM_380, // GRLU U dup not NULL bsn=%d
  GRLC_F3_MSG_ENUM_381, // GRLU U Potential Dup Exhaustion %d vb=%d ofs=%u
  GRLC_F3_MSG_ENUM_382, // dsm_ptr=%x a_len=%u dup=%x
  GRLC_F3_MSG_ENUM_383, // dup free = %u dp=%x gas_id
  GRLC_F3_MSG_ENUM_384, // GRLU send_b bad put result %d bsn=%d
  GRLC_F3_MSG_ENUM_385, // GRLU ##atta_blk[%d] dsm not NULL 0x%x
  GRLC_F3_MSG_ENUM_386, // GRLU ##atta_blk[%d] dup not NULL 0x%x
  GRLC_F3_MSG_ENUM_387, // GRLU atta bsn=%d 1st_psn=%u last_psn=%u
  GRLC_F3_MSG_ENUM_388, // GRLU ##atta_blk[%d] dsm not NULL 0x%x
  GRLC_F3_MSG_ENUM_389, // GRLU ##atta_blk[%d] dup not NULL 0x%x
  GRLC_F3_MSG_ENUM_390, // GRLU atta bsn=%d 1st_psn=%u last_psn=%u
  GRLC_F3_MSG_ENUM_391, // ERLU e_tx num_blks = 0 va=%d vs=%d vsf=%d
  GRLC_F3_MSG_ENUM_392, // GRLU PL1 FIFO full for NACK blks
  GRLC_F3_MSG_ENUM_393, // ERLU init_msc of NACK block bsn %d is UNDEF!!
  GRLC_F3_MSG_ENUM_394, // GRLU PL1 FIFO full for split NACK bsn=%u
  GRLC_F3_MSG_ENUM_395, // GRLU ptx vsf(%d) >= seg_ind(%d)
  GRLC_F3_MSG_ENUM_396, // GRLU Info tx NAK FIFO full ind1=%d num_blks=%d
  GRLC_F3_MSG_ENUM_397, // ERLU e_tx petx = %u
  GRLC_F3_MSG_ENUM_398, // ERLU e_tx ptx/pack w full va=%u vs=%u repeat_sending_p_ack=%u
  GRLC_F3_MSG_ENUM_399, // ERLU e_tx SI=1 va=%d vs=%d seg=%d
  GRLC_F3_MSG_ENUM_400, // ERLU e_tx ptx/pack w ! full va=%u vs=%u p_vsf=%u
  GRLC_F3_MSG_ENUM_401, // GRLU ptx vsf(%d) >= seg_ind(%d)
  GRLC_F3_MSG_ENUM_402, // GRLU PL1 FIFO full for PTX blks
  GRLC_F3_MSG_ENUM_403, // GRLU tx CV=0 bsn=%d
  GRLC_F3_MSG_ENUM_404, // GRLU tx ind2 CV=0 bsn=%d
  GRLC_F3_MSG_ENUM_405, // GRLU Info PTX FIFO full ind1=%d ind2=%d
  GRLC_F3_MSG_ENUM_406, // ERLU e_tx ena-PACKs p=%u vs=%d in=%d
  GRLC_F3_MSG_ENUM_407, // ERLU tx SI=1 va=%d vs=%d seg=%d
  GRLC_F3_MSG_ENUM_408, // ERLU e_tx num_blks=%u WINDOW_FULL=%u segment_state=%u
  GRLC_F3_MSG_ENUM_409, // ERLU Pack FIFO Cleared
  GRLC_F3_MSG_ENUM_410, // GRLU PL1 FIFO full for PACK blks
  GRLC_F3_MSG_ENUM_411, // ERLU tx NACK %d not acked from PL1
  GRLC_F3_MSG_ENUM_412, // ERLU init_msc of PACK block bsn %d is UNDEF!!
  GRLC_F3_MSG_ENUM_413, // GRLU PL1 FIFO full for split PACK bsn=%u
  GRLC_F3_MSG_ENUM_414, // ERLU tx NACK %d not acked from PL1
  GRLC_F3_MSG_ENUM_415, // ERLU e_tx PAK FIFO full ind1=%d ind2=%d
  GRLC_F3_MSG_ENUM_416, // ERLU tx PTX %d not acked from PL1
  GRLC_F3_MSG_ENUM_417, // GRLU PL1 FIFO full for PACK blks
  GRLC_F3_MSG_ENUM_418, // GRLU PL1 FIFO full for split PACK with CV=0 bsn=%u
  GRLC_F3_MSG_ENUM_419, // ELRU e_tx petx=%u sending CV_0 bsn =%u num_blks=%u
  GRLC_F3_MSG_ENUM_420, // ERLU U blks=0 PL1space=%u seg=%u vs=%u
  GRLC_F3_MSG_ENUM_421, // ERLU U pack vsf(%d) >= seg_ind(%d)
  GRLC_F3_MSG_ENUM_422, // ERLU U tx CV=0 bsn=%d
  GRLC_F3_MSG_ENUM_423, // ERLU free prev CV_0 bsn (%u)
  GRLC_F3_MSG_ENUM_424, // ERLU new ind1 e_bsn_of_cv_0_block = %u
  GRLC_F3_MSG_ENUM_425, // GRLU tx CV=0 bsn=%d
  GRLC_F3_MSG_ENUM_426, // ERLU free prev CV_0 bsn (%u)
  GRLC_F3_MSG_ENUM_427, // ERLU new ind2 e_bsn_of_cv_0_block = %u
  GRLC_F3_MSG_ENUM_428, // ERLU Info UNACK PACK FIFO full ind1=%d ind2=%d
  GRLC_F3_MSG_ENUM_429, // ERLU U post free %u NpOctets=%08x 1st_to_free=%u
  GRLC_F3_MSG_ENUM_430, // GRLU tx num_blks = 0 va=%d vs=%d vsf=%d
  GRLC_F3_MSG_ENUM_431, // GRLU tx SI=1 va=%d vs=%d seg=%d
  GRLC_F3_MSG_ENUM_432, // GRLU ptx vsf(%d) >= seg_ind(%d)
  GRLC_F3_MSG_ENUM_433, // GRLU tx ena-PACKs p=%u vs=%d in=%d
  GRLC_F3_MSG_ENUM_434, // GRLU tx SI=1 va=%d vs=%d seg=%d
  GRLC_F3_MSG_ENUM_435, // GRLU P_PACK %d becomes PACK (PL1 mis-ack)
  GRLC_F3_MSG_ENUM_436, // GRLU tx PTX %d becomes PACK (PL1 mis-ack)
  GRLC_F3_MSG_ENUM_437, // GRLU U blks=0 PL1space=%d seg=%d vs=%d
  GRLC_F3_MSG_ENUM_438, // GRLU tx_u pack_vsf < V(S)
  GRLC_F3_MSG_ENUM_439, // GRLU U tx CV=0 bsn=%d
  GRLC_F3_MSG_ENUM_440, // GRLU free prev CV_0 bsn (%u)
  GRLC_F3_MSG_ENUM_441, // GRLU new bsn_of_cv_0_block = %u
  GRLC_F3_MSG_ENUM_442, // GRLU U post free %u NpOctets=%08x 1st_to_free=%u
  GRLC_F3_MSG_ENUM_443, // ERLU EXT_UTBF T3182 started CV=0 or stalled bsn=%u e_bsn_of_cv_0=%u
  GRLC_F3_MSG_ENUM_444, // ERLU XTimer started CV=0 bsn=%u e_bsn_of_cv_0=%u
  GRLC_F3_MSG_ENUM_445, // ERLU XTimer stopped CV not 0 bsn=%u e_bsn_of_cv_0=%u
  GRLC_F3_MSG_ENUM_446, // ERLU A post TLLI IncN3104=%u max=%u vsf=%u
  GRLC_F3_MSG_ENUM_447, // ERLU U post bsn=%u RA_AT_TX_COMPLETE aborted cv=0
  GRLC_F3_MSG_ENUM_448, // ERLU U post bsn=%u RA_AT_TX_COMPLETE
  GRLC_F3_MSG_ENUM_449, // ERLU U T3182 started CV=0 or stalled bsn=%u e_bsn_of_cv_0=%u
  GRLC_F3_MSG_ENUM_450, // GRLU EXT_TBF:T3182 start bsn cv 0 = %d
  GRLC_F3_MSG_ENUM_451, // GRLU XTimer started CV=0 bsn=%u bsn_of_cv_0=%u
  GRLC_F3_MSG_ENUM_452, // GRLU XTimer stopped CV not 0 bsn=%u bsn_of_cv_0=%u
  GRLC_F3_MSG_ENUM_453, // GRLU A post TLLI IncN3104=%u max=%d vsf=%u
  GRLC_F3_MSG_ENUM_454, // GRLU U post bsn=%u RA_AT_TX_COMPLETE aborted cv=0
  GRLC_F3_MSG_ENUM_455, // GRLU U post bsn=%u RA_AT_TX_COMPLETE
  GRLC_F3_MSG_ENUM_456, // GRLU U post IncN3104=%d max=%d vsf=%d
  GRLC_F3_MSG_ENUM_457, // GRLU U T3182 started CV=0 or stalled bsn=%u bsn_of_cv_0=%u
  GRLC_F3_MSG_ENUM_458, // GRLU A_X s=%d return
  GRLC_F3_MSG_ENUM_459, // GRLU A Expy N3104=%d N3104_MAX=%d
  GRLC_F3_MSG_ENUM_460, // A_X calls group_pdus
  GRLC_F3_MSG_ENUM_461, // ERLU A_X s=%d return
  GRLC_F3_MSG_ENUM_462, // GRLU A Expy N3104=%d N3104_MAX=%d
  GRLC_F3_MSG_ENUM_463, // E_A_X calls group_pdus
  GRLC_F3_MSG_ENUM_464, // GRLU U_X s=%d return
  GRLC_F3_MSG_ENUM_465, // U_X calls group_pdus
  GRLC_F3_MSG_ENUM_466, // ERLU U_X s=%d return
  GRLC_F3_MSG_ENUM_467, // E_U_X calls group_pdus
  GRLC_F3_MSG_ENUM_468, // GRLU re-psn head PDU NULL nw psn=%u qcnt=%u
  GRLC_F3_MSG_ENUM_469, // GRLU re-seq-pdus old pg.psn=%u start_psn=%u end_psn=%u
  GRLC_F3_MSG_ENUM_470, // GRLU del_sig_pdus psn=%u cause=%u len=%u
  GRLC_F3_MSG_ENUM_471, // GRLU inform gmm pdu tx failure when ps access = %d
  GRLC_F3_MSG_ENUM_472, // GRLU del_signalling_pdus PDU null when q_cnt=%u
  GRLC_F3_MSG_ENUM_473, // GRLU del_signalling_pdus starting PSN=%u
  GRLC_F3_MSG_ENUM_474, // GRLU tlli_ovl p=%u old_tlli=0x%x new=0x%x
  GRLC_F3_MSG_ENUM_475, // GRLU load check_prr cur_p=0x%x next_p=0x%x
  GRLC_F3_MSG_ENUM_476, // GRLU load_cur p=%u l=%u ol=%u
  GRLC_F3_MSG_ENUM_477, // GRLU unp NULL pdu qcnt=%u
  GRLC_F3_MSG_ENUM_478, // GRLU unp %u qcnt=%u msg_ptr=%x
  GRLC_F3_MSG_ENUM_479, // GRLU     ptr=%x dup=%x
  GRLC_F3_MSG_ENUM_480, // GRLU unp_next qcnt=%u n_p=%u next=%x
  GRLC_F3_MSG_ENUM_481, // GRLU shorten psn=%u cur-ofs=%u new-ofs=%u
  GRLC_F3_MSG_ENUM_482, // GRLU ##shorten dup returns NULL
  GRLC_F3_MSG_ENUM_483, // GRLU ##shorten len=0. psn=%u dup_pdu=0x%x
  GRLC_F3_MSG_ENUM_484, // GRLU shorten %u orig_l=%u len=%u
  GRLC_F3_MSG_ENUM_485, // GRLU Delete higher layer pdu cause=%d  psn=%d
  GRLC_F3_MSG_ENUM_486, // GRLU delink del non-data psn=%u cause=%u transaction_id=%u
  GRLC_F3_MSG_ENUM_487, // Failures_ind reached MAX size=%d
  GRLC_F3_MSG_ENUM_488, // GRLU Delink s=%u qcnt=%u susp=%u
  GRLC_F3_MSG_ENUM_489, // GRLU Relink s=%u qcnt=%u susp=%u
  GRLC_F3_MSG_ENUM_490, // GRLU enq SWAP INS skip cnt=%u
  GRLC_F3_MSG_ENUM_491, // GRLU enq SWAP INS brk cnt=%u
  GRLC_F3_MSG_ENUM_492, // GRLU enq SWAP loop-end qcnt=%u
  GRLC_F3_MSG_ENUM_493, // GRLU enq SWAP ins
  GRLC_F3_MSG_ENUM_494, // GRLU enq SWAP append
  GRLC_F3_MSG_ENUM_495, // GRLU enq SWAP PDU NULL
  GRLC_F3_MSG_ENUM_496, // GRLU PDU reordering for signalling PDU - reorder pdu %d
  GRLC_F3_MSG_ENUM_497, // GRLU grlc_ul_promote_sig_pdus failure next=NULL
  GRLC_F3_MSG_ENUM_498, // GRLU enq delete, pdu len=%u, cause %d
  GRLC_F3_MSG_ENUM_499, // GRLU enq delete ALL signalling pdu under no service , pdu len=%u, cause %d
  GRLC_F3_MSG_ENUM_500, // GRLU enq delete llc pdu len=%u
  GRLC_F3_MSG_ENUM_501, // GRLU enq Q full tbf-mode=%u FCtrl=%u octes=%lu
  GRLC_F3_MSG_ENUM_502, // GRLU ##exceeded max num of MAX_NUM_SIGNALLING_PDU-RLC out of sync
  GRLC_F3_MSG_ENUM_503, // GRLU ##enq dup returns NULL
  GRLC_F3_MSG_ENUM_504, // GRLU enq SWAP cur=%u last=%u
  GRLC_F3_MSG_ENUM_505, // GRLU enq SWAP next psn=%u qcnt=%u
  GRLC_F3_MSG_ENUM_506, // enq group_pdus
  GRLC_F3_MSG_ENUM_507, // enq group_pdus
  GRLC_F3_MSG_ENUM_508, // GRLU susp state data cnt %d, cause %d
  GRLC_F3_MSG_ENUM_509, // GRLU enq LLC PDU PFI=%d, PFC=%d
  GRLC_F3_MSG_ENUM_510, // GRLU NULL pdu ptr received
  GRLC_F3_MSG_ENUM_511, // GRLU free PM msg NULL
  GRLC_F3_MSG_ENUM_512, // GRLU Pfree %u PM NpOctets=%08x small=%u
  GRLC_F3_MSG_ENUM_513, // GRLU bad access_phase %d
  GRLC_F3_MSG_ENUM_514, // ## cur_blk dsm %d not NULL
  GRLC_F3_MSG_ENUM_515, // ## cur_blk dup %d not NULL
  GRLC_F3_MSG_ENUM_516, // ## cur_blk dsm %d not NULL
  GRLC_F3_MSG_ENUM_517, // ## cur_blk dup %d not NULL
  GRLC_F3_MSG_ENUM_518, // ERLU ##cur_res.cs oor %u
  GRLC_F3_MSG_ENUM_519, // GRLU usf info FN = %lu, usfs = %lu, blanked = %lu
  GRLC_F3_MSG_ENUM_520, // GRLU usf info consecutive blanked = %lu, percentage blanking = %lu
  GRLC_F3_MSG_ENUM_521, // GRLU UL free TX workspace
  GRLC_F3_MSG_ENUM_522, // ERLU tx_arr[%u].dup_dsm_ptr not NULL 0x%x
  GRLC_F3_MSG_ENUM_523, // ERLU tx_arr[%u].dsm_ptr not NULL 0x%x
  GRLC_F3_MSG_ENUM_524, // GRLU Sys ext_utbf cv_max t3192_ind X...YYZZ=%08x PAQ 0CtrInOu=%08x PaqMax=%u
  GRLC_F3_MSG_ENUM_525, // ERLU det_realloc PSN realloc'ed earlier %u
  GRLC_F3_MSG_ENUM_526, // GRLU prr cur_p=0x%x next_p=0x%x state=%u
  GRLC_F3_MSG_ENUM_527, // GRLU prr cur_p NULL or not in xfer or bad ral state NULL
  GRLC_F3_MSG_ENUM_528, // GRLU prr ret next_p = 0x%x rals = %u
  GRLC_F3_MSG_ENUM_529, // GRLU prr rals = %u cur-psn=%u next-psn=%u prev_realloc_psn=%u
  GRLC_F3_MSG_ENUM_530, // GRLU prr RA_IMMEDIATE psn=%u
  GRLC_F3_MSG_ENUM_531, // GRLU prr TX_COMPLETED psn=%u
  GRLC_F3_MSG_ENUM_532, // GRLU prr AWAIT_TX_COMPLETE psn=%u
  GRLC_F3_MSG_ENUM_533, // GRLU prr CHANGE_MODE complete psn=%u realloc_psn=%u
  GRLC_F3_MSG_ENUM_534, // GRLU prr CHANGE_MODE 3/4 psn=%u
  GRLC_F3_MSG_ENUM_535, // GRLU prr CHANGE_MODE ack/un-ack not complete psn=%u
  GRLC_F3_MSG_ENUM_536, // GRLU prr CHANGE_MODE non-ext No Realloc psn=%u
  GRLC_F3_MSG_ENUM_537, // GRLU prr bad realloc_state = %u
  GRLC_F3_MSG_ENUM_538, // GRLU prr psn=%u next_psn=%u
  GRLC_F3_MSG_ENUM_539, // ERLU EXT_UTBF process guard timer sub-state = %u timer-state =%u
  GRLC_F3_MSG_ENUM_540, // ERLU XTimer GUARD_OFF .. no actions
  GRLC_F3_MSG_ENUM_541, // ERLU XTimer CV_ZERO va=%u vs=%u vseg=%u
  GRLC_F3_MSG_ENUM_542, // ERLU XTimer started va=vseg CV_ZERO -> GUARD_ON
  GRLC_F3_MSG_ENUM_543, // ERLU XTimer re-started CV_ZERO va=%u old_va=%u
  GRLC_F3_MSG_ENUM_544, // ERLU XTimer GUARD_ON va=%u vs=%u vseg=%u
  GRLC_F3_MSG_ENUM_545, // ERLU XTimer stopped vs > va GUARD_ON > GUARD_OFF
  GRLC_F3_MSG_ENUM_546, // ERLU XTimer bad state = %u
  GRLC_F3_MSG_ENUM_547, // GRLU Info Flushed PDU queue
  GRLC_F3_MSG_ENUM_548, // GRLU clear_cur_pdu_grouping_info %u
  GRLC_F3_MSG_ENUM_549, // GRLU cur_pdu_ptr->psn = %u, pg_last_psn was %d now %u
  GRLC_F3_MSG_ENUM_550, // GRLU Info Flushed PDU queue sapi_id=%d
  GRLC_F3_MSG_ENUM_551, //  GRLU ULTQ: PDU cause =%d, SAPI = %d PDU Type = %d
  GRLC_F3_MSG_ENUM_552, // GRLU next_pdu_ptr is NULL sapi=%d
  GRLC_F3_MSG_ENUM_553, // GRLU next_pdu_ptr not freed psn=%d sapi=%d
  GRLC_F3_MSG_ENUM_554, //  GRLU OldQ: PDU cause =%d, SAPI = %d, PDU Type =%d 
  GRLC_F3_MSG_ENUM_555, //  GRLU ULQ: PDU cause =%d, SAPI = %d, PDU Type =%d 
  GRLC_F3_MSG_ENUM_556, // GRLU cur-psn=%u last_p=%u pgstate=%d
  GRLC_F3_MSG_ENUM_557, // GRLU cur#blks=%u #blks=%u #pdus=%u
  GRLC_F3_MSG_ENUM_558, // GRLU ##error recovery
  GRLC_F3_MSG_ENUM_559, // GRLU triggering panic reset
  GRLC_F3_MSG_ENUM_560, // GRLU ##OOR ssn=%u va=%u vs=%u
  GRLC_F3_MSG_ENUM_561, // GRLU upd ACK %d
  GRLC_F3_MSG_ENUM_562, // GRLU ##update tx-dup not cleared at bsn=%d
  GRLC_F3_MSG_ENUM_563, // ELRU ind=%u 1st=%u last=%u
  GRLC_F3_MSG_ENUM_564, // ELRU upd CV 0 ind %u psn to free %u
  GRLC_F3_MSG_ENUM_565, // ERLU upd CV=%u ind %u psn to free %u
  GRLC_F3_MSG_ENUM_566, // GRLU upd T3182 stopped SI=0 va=%d vs=%d seg=%d
  GRLC_F3_MSG_ENUM_567, // GRLU upd IncN3102=%d max=%d inc=%d
  GRLC_F3_MSG_ENUM_568, // ERLU e_upd ACK %u
  GRLC_F3_MSG_ENUM_569, // ERLU e_upd dup ! clr at BSN %u
  GRLC_F3_MSG_ENUM_570, // GRLU upd NACK %d nack_ctr=%d
  GRLC_F3_MSG_ENUM_571, // ERLU e_ack pos_rel_ssn=%u ack=%u
  GRLC_F3_MSG_ENUM_572, // ERLU e_upd invalid urbb_len %u. Forced to WS %u
  GRLC_F3_MSG_ENUM_573, // ERLU e_upd BOW=1 ACK %u va=%u vs=%u
  GRLC_F3_MSG_ENUM_574, // GRLU e_upd dup ! clr at BSN %u
  GRLC_F3_MSG_ENUM_575, // ERLU e_upd BOW=1 BSN oor va=%u bsn=%u vs=%u
  GRLC_F3_MSG_ENUM_576, // ERLU e_upd BOW=1 NACK SSN-1 global blk cnt=%u [%04x].blk_cnt = %u
  GRLC_F3_MSG_ENUM_577, // ERLU e_upd NACK %u upd nack_vsf (was %u)
  GRLC_F3_MSG_ENUM_578, // ERLU e_upd BOW=1 NACK SSN-1 %d va=%u vs=%u
  GRLC_F3_MSG_ENUM_579, // ERLU e_upd t4_dec rets OK bits_decoded=%u
  GRLC_F3_MSG_ENUM_580, // ERLU e_upd t4_dec rets FAILED bits_decoded=%u
  GRLC_F3_MSG_ENUM_581, // ERLU e_upd t4_dec rets bits_decoded=%u ssn=%u
  GRLC_F3_MSG_ENUM_582, // ERLU e_upd URBB ACK %u va=%u vs=%u
  GRLC_F3_MSG_ENUM_583, // GRLU e_upd URBB dup ! clr at BSN %u
  GRLC_F3_MSG_ENUM_584, // ERLU e_upd URBB NACK SSN-1 global blk cnt=%u [%u].blk_cnt = %u
  GRLC_F3_MSG_ENUM_585, // ERLU e_upd URBB NACK %d upd nack_vsf (was %d)
  GRLC_F3_MSG_ENUM_586, // ERLU e_upd URBB NACK %d nack_vsf=%u
  GRLC_F3_MSG_ENUM_587, // GRLU e_upd BSN %u OOR
  GRLC_F3_MSG_ENUM_588, // ERLU upd EOW=1 NACK %d nack_ctr=%d
  GRLC_F3_MSG_ENUM_589, // ERLU upd EOW=1 can't NACK %d nack_ctr=%d
  GRLC_F3_MSG_ENUM_590, // GRLU upd T3182 stopped SI=0 va=%d vs=%d seg=%d
  GRLC_F3_MSG_ENUM_591, // ELRU ind=%u 1st=%u last=%u
  GRLC_F3_MSG_ENUM_592, // ELRU e_upd CV 0 ind %u psn to free %u
  GRLC_F3_MSG_ENUM_593, // ERLU e_upd CV=%u ind %u psn to free %u
  GRLC_F3_MSG_ENUM_594, // ERLU upd free inc %u va=%d upd free inclusive = %u
  GRLC_F3_MSG_ENUM_595, // ERLU e_upd    va=%u    vs=%u vsf=%u
  GRLC_F3_MSG_ENUM_596, // ERLU       n_vsf=%u p_vsf=%u seg=%u
  GRLC_F3_MSG_ENUM_597, // ERLU upd N3102 IncN3102=%d max=%d inc=%d
  GRLC_F3_MSG_ENUM_598, // GRLU CRes failed ResN3104=%d max=%d
  GRLC_F3_MSG_ENUM_599, // ERLU CRes success ResN3104=%u max=%u
  GRLC_F3_MSG_ENUM_600, // ERLU CRes success stopped T3182
  GRLC_F3_MSG_ENUM_601, // ERLU CRes ul[gas_id].mcs=%u tlli_ind.mcs=%u cur_res.mcs=%u
  GRLC_F3_MSG_ENUM_602, // ERLU CRes FAI mcs=%u tlli-cs=%u p=%u
  GRLC_F3_MSG_ENUM_603, // ERLU CRes FAI XTimer stopped
  GRLC_F3_MSG_ENUM_604, // ERLU seek p=%u seg=%u psn_cv0=%u
  GRLC_F3_MSG_ENUM_605, // ERLU CRes tx-complete
  GRLC_F3_MSG_ENUM_606, // ERLU A CRes cs=%u tlli-cs=%u p=%u
  GRLC_F3_MSG_ENUM_607, // ERLU ##CRes No Reproc A (2) bsn(%u) outside seg(%d)
  GRLC_F3_MSG_ENUM_608, // ERLU CRes PTX (2) bsn=vsf=%u vs=%u seg=%u
  GRLC_F3_MSG_ENUM_609, // ERLU CRes PTX (3) del bsn=%u vs=%u vsf=%u
  GRLC_F3_MSG_ENUM_610, // ERLU CRes PTX (1) bsn=vsf=%u vs=%u seg=%u
  GRLC_F3_MSG_ENUM_611, // ERLU CRes No Reprocess
  GRLC_F3_MSG_ENUM_612, // ERLU U CRes cs=%u tlli-cs=%u p=%u
  GRLC_F3_MSG_ENUM_613, // ERLU CRes UNACK (2) bsn=vsf=%u vs=%u seg=%u
  GRLC_F3_MSG_ENUM_614, // ERLU CRes PAK (3) del bsn=%u vs=%u vsf=%u
  GRLC_F3_MSG_ENUM_615, // ERLU CRes UNACK (1) bsn=vsf=%u vs=%u seg=%u
  GRLC_F3_MSG_ENUM_616, // GRLU CRes success ResN3104=%d max=%d
  GRLC_F3_MSG_ENUM_617, // GRLU CRes success stopped T3182
  GRLC_F3_MSG_ENUM_618, // GRLU CRes FAI cs=%d tlli-cs=%d p=%u
  GRLC_F3_MSG_ENUM_619, // GRLU CRes FAI XTimer stopped
  GRLC_F3_MSG_ENUM_620, // GRLU seek p=%u seg=%d psn_cv0=%u
  GRLC_F3_MSG_ENUM_621, // GRLU CRes tx-complete
  GRLC_F3_MSG_ENUM_622, // GRLU A CRes cs=%d tlli-cs=%d p=%u
  GRLC_F3_MSG_ENUM_623, // GRLU ##CRes No Reproc A (2) bsn(%u) outside seg(%d)
  GRLC_F3_MSG_ENUM_624, // GRLU CRes PTX (2) bsn=vsf=%d vs=%d seg=%d
  GRLC_F3_MSG_ENUM_625, // GRLU CRes PTX (3) del bsn=%d vs=%d vsf=%d
  GRLC_F3_MSG_ENUM_626, // GRLU CRes PTX (1) bsn=vsf=%u vs=%u seg=%u
  GRLC_F3_MSG_ENUM_627, // GRLU CRes No Reprocess
  GRLC_F3_MSG_ENUM_628, // GRLU U CRes cs=%d tlli-cs=%d p=%u
  GRLC_F3_MSG_ENUM_629, // GRLU ChCS cs=%d new-cs=%d p=%u
  GRLC_F3_MSG_ENUM_630, // GRLU Rcvd Susp in US_AWAIT_ASS
  GRLC_F3_MSG_ENUM_631, // GRLU Rcvd SuspInXfer (%d) new state=NULL
  GRLC_F3_MSG_ENUM_632, // GRLU Already Susp s=%d
  GRLC_F3_MSG_ENUM_633, // GRLU psn from sapi %d in PAQ
  GRLC_F3_MSG_ENUM_634, // GRLU psn from sapi %d only in PAQ
  GRLC_F3_MSG_ENUM_635, // GRLU SUSP PURGE susp data s=%d qcnt=%d
  GRLC_F3_MSG_ENUM_636, // GRLU Rcvd Purge not in Suspend(%d)
  GRLC_F3_MSG_ENUM_637, // GRLU PURGE_DATA_REQ: Pre purge s=%d susp_state=%d
  GRLC_F3_MSG_ENUM_638, // GRLU PURGE_DATA_REQ: Pre purge ULQ cnt=%d ULTQ cnt=%d OldQ cnt=%d
  GRLC_F3_MSG_ENUM_639, // GRLU invalid ul state %d
  GRLC_F3_MSG_ENUM_640, // GRLU PURGE_DATA_REQ: Post purge ULQ cnt=%d ULTQ cnt=%d OldQ cnt=%d
  GRLC_F3_MSG_ENUM_641, // GRLU SUSP Resume s=%d qcnt=%u
  GRLC_F3_MSG_ENUM_642, // GRLU RESUME state = %d NoTBFEst
  GRLC_F3_MSG_ENUM_643, // GRLU resume_req not in SUSPEND susp_s=%d s=%d qcnt=%u
  GRLC_F3_MSG_ENUM_644, // GRLU Est tbf req for Test Mode B ts_ofsset=%d
  GRLC_F3_MSG_ENUM_645, // GRLU Est tbf req for Test Mode SRB M_bit = %d ts_ofsset=%d
  GRLC_F3_MSG_ENUM_646, // GRLU INVALID Est tbf req for Test Mode=%d
  GRLC_F3_MSG_ENUM_647, // GRLU acc_res TLLI ResN3104=%d max=%d 1st_psn=%u
  GRLC_F3_MSG_ENUM_648, // GRLU acc_res tbc=%u blocks_req=%u typ=%d
  GRLC_F3_MSG_ENUM_649, // GRLU res_inf Dyn Close Final=%d
  GRLC_F3_MSG_ENUM_650, // GRLU res_inf Dyn Open Final=%d
  GRLC_F3_MSG_ENUM_651, // GRLU res_inf blks_grtd=%d cs=%d retry=%d
  GRLC_F3_MSG_ENUM_652, // GRLU res_inf blks_grtd=%d cs=%d retry=%d
  GRLC_F3_MSG_ENUM_653, // GRLU RA ptr NULL or PSN(%u) freed(%u). State = %u
  GRLC_F3_MSG_ENUM_654, // GRLU suppress REALLOC during PSHO
  GRLC_F3_MSG_ENUM_655, // ERLU XTimer stopped sending realloc req
  GRLC_F3_MSG_ENUM_656, // ERLU XTimer stopped sending realloc req
  GRLC_F3_MSG_ENUM_657, // GRLU RA ra_octs=%d cv-zero=%d
  GRLC_F3_MSG_ENUM_658, // GRLU est_tbf PDU NULL or no-access
  GRLC_F3_MSG_ENUM_659, // est_tbf octets=%u pg_octets=%u qcnt=%u
  GRLC_F3_MSG_ENUM_660, // GRLU tbf_est rlc_mode=%d ptc=%d prio=%d
  GRLC_F3_MSG_ENUM_661, // GRLU         cause=%d phase=%d o/c=%d
  GRLC_F3_MSG_ENUM_662, // GRLU         octs=%d blks=%d psn=%u
  GRLC_F3_MSG_ENUM_663, // GRLU GRR_STATUS_IND failure psn=%d cause=%d transaction_id=%d
  GRLC_F3_MSG_ENUM_664, // GRLU GRR_RESUME_CNF
  GRLC_F3_MSG_ENUM_665, // GRLU GRR_STATUS_IND success psn=%d cause=%d transaction_id=%u
  GRLC_F3_MSG_ENUM_666, // GRLU GRR_STATUS_IND success already indicated for psn_success_ind=%d
  GRLC_F3_MSG_ENUM_667, // GRLU GPRS-TEST-MODE-A Release - Purge all PDUs
  GRLC_F3_MSG_ENUM_668, // GRLU AbRelWoRetry psn=%u cause=%u DEL_ONE
  GRLC_F3_MSG_ENUM_669, // GRLU AbRelAccRetry/CellResel psn=%u cause=%u NO_DEL
  GRLC_F3_MSG_ENUM_670, // GRLU NormRel cur-psn=%u cur-tbc=%u cur-blks=%u
  GRLC_F3_MSG_ENUM_671, // GRLU NormRel PGS_CLOSE p=%u lp=%u tbc=%u
  GRLC_F3_MSG_ENUM_672, // GRLU REL bad release cause %d
  GRLC_F3_MSG_ENUM_673, // GRLU GPRS-TEST-MODE-Release - Purge all PDUs
  GRLC_F3_MSG_ENUM_674, // GRLU reproc cause=%u psn=%u rem_ind=%u
  GRLC_F3_MSG_ENUM_675, // GRLU reproc0 mode=%u ptx_cv=%u pak_cv=%u
  GRLC_F3_MSG_ENUM_676, // GRLU reproc0 psn=%u rem_ind=%u len=%u
  GRLC_F3_MSG_ENUM_677, // GRLU reproc0 seg=%u vs=%u vsf=%u
  GRLC_F3_MSG_ENUM_678, // ERLU Reproc A (2) NoDel vsf(bsn)=%d 1st_psn=%u ofs=%u
  GRLC_F3_MSG_ENUM_679, // ERLU ##No Reproc A (2) bsn(%u) outside seg(%d)
  GRLC_F3_MSG_ENUM_680, // ERLU Reproc A (3) Del bsn=%u 1st_psn=%u ofs=%u
  GRLC_F3_MSG_ENUM_681, // ERLU Reproc A (1) empty TRUE bsn(vs)=%d vsf=%d psn=%u
  GRLC_F3_MSG_ENUM_682, // ERLU Reproc A (1) rem=%u len=%u CV=%d
  GRLC_F3_MSG_ENUM_683, // ERLU          (1) psn=%u last_p=%u
  GRLC_F3_MSG_ENUM_684, // ERLU Reproc A (1) cont' cur=%u new_cur=%u last_p=%u
  GRLC_F3_MSG_ENUM_685, // ERLU Reproc A (1) rem(%u) != len(%u)
  GRLC_F3_MSG_ENUM_686, // GRLU Reproc A (2) NoDel vsf(bsn)=%d 1st_psn=%u ofs=%u
  GRLC_F3_MSG_ENUM_687, // GRLU ##No Reproc A (2) bsn(%d) outside seg(%d)
  GRLC_F3_MSG_ENUM_688, // GRLU Reproc A (3) Del bsn=%d 1st_psn=%u ofs=%u
  GRLC_F3_MSG_ENUM_689, // GRLU Rep A (1) empty TRUE bsn(vs)=%d vsf=%d psn=%u
  GRLC_F3_MSG_ENUM_690, // GRLU Reproc A (1) rem=%u len=%u CV=%d
  GRLC_F3_MSG_ENUM_691, // GRLU          (1) psn=%u last_p=%u
  GRLC_F3_MSG_ENUM_692, // GRLU Reproc A (1) cont' cur=%u new_cur=%u last_p=%u
  GRLC_F3_MSG_ENUM_693, // ERLU Reproc A (1) rem(%u) != len(%u)
  GRLC_F3_MSG_ENUM_694, // ERLU Reproc U (2) NoDel p_vsf=%u p_vsf=bsn=%u seg=%u
  GRLC_F3_MSG_ENUM_695, // ERLU ##No Reproc U (2) bsn(%u) outside seg(%u)
  GRLC_F3_MSG_ENUM_696, // ERLU Reproc U (3) NoDel p_vsf=%u p_vsf=bsn=%u seg=%u
  GRLC_F3_MSG_ENUM_697, // ERLU Reproc U (3) Del bsn=%u vs=%u p_vsf=%u
  GRLC_F3_MSG_ENUM_698, // ERLU Reproc U (1) empty bsn=vs=%u p_vsf=%u seg=%u
  GRLC_F3_MSG_ENUM_699, // ERLU Reproc U (1) rem=%u len=%u CV=%u
  GRLC_F3_MSG_ENUM_700, // ERLU          (1) psn=%u last_p=%u
  GRLC_F3_MSG_ENUM_701, // ERLU Reproc U (1) cont' cur=%u new_cur=%u last_p=%u
  GRLC_F3_MSG_ENUM_702, // ERLU Reproc U (1) rem(%u) != len(%u)
  GRLC_F3_MSG_ENUM_703, // GRLU Reproc U (2) NoDel p_vsf=%d p_vsf=bsn=%d seg=%d
  GRLC_F3_MSG_ENUM_704, // GRLU ##No Reproc U (2) bsn(%d) outside seg(%d)
  GRLC_F3_MSG_ENUM_705, // GRLU Reproc U (3) Del bsn=%d vs=%d p_vsf=%d
  GRLC_F3_MSG_ENUM_706, // GRLU Reproc U (3) Del bsn=%d vs=%d p_vsf=%d
  GRLC_F3_MSG_ENUM_707, // GRLU Reproc U (1) empty bsn=vs=%d p_vsf=%d seg=%d
  GRLC_F3_MSG_ENUM_708, // GRLU Reproc U (1) rem=%u len=%u CV=%d
  GRLC_F3_MSG_ENUM_709, // GRLU          (1) psn=%u last_p=%u
  GRLC_F3_MSG_ENUM_710, // GRLU Reproc U (1) cont' cur=%u new_cur=%u last_p=%u
  GRLC_F3_MSG_ENUM_711, // GRLU Reproc U (1) rem(%u) != len(%u)
  GRLC_F3_MSG_ENUM_712, // GRLU reproc1 psn=%u rem_ind=%u len=%u
  GRLC_F3_MSG_ENUM_713, // GRLU reproc1 seg=%u vs=%u vsf=%u
  GRLC_F3_MSG_ENUM_714, // GRLU reproc1 ptx_cv=%u pak_cv=%u pack_vsf=%u
  GRLC_F3_MSG_ENUM_715, // GRLU reproc1 tbc=%u psn=%u last_psn=%u
  GRLC_F3_MSG_ENUM_716, // ERLU Sent PTX %d CV %d l %d
  GRLC_F3_MSG_ENUM_717, // GRLU Sent PTX %d CV %d l %d
  GRLC_F3_MSG_ENUM_718, // ERLU Sent PAK %d CV %d l %d
  GRLC_F3_MSG_ENUM_719, // GRLU Sent PAK %d CV %d l %d
  GRLC_F3_MSG_ENUM_720, // ERLU Sent ACK %d CV %d l %d
  GRLC_F3_MSG_ENUM_721, // GRLU Sent ACK %d CV %d l %d
  GRLC_F3_MSG_ENUM_722, // ERLU Sent UNA %d CV %d l %d
  GRLC_F3_MSG_ENUM_723, // GRLU Sent UNA %d CV %d l %d
  GRLC_F3_MSG_ENUM_724, // ERLU Sent PPA %d CV %d l %d
  GRLC_F3_MSG_ENUM_725, // GRLU Sent PPA %d CV %d l %d
  GRLC_F3_MSG_ENUM_726, // ERLU Sent NAK %d CV %d l %d
  GRLC_F3_MSG_ENUM_727, // GRLU Sent NAK %d CV %d l %d
  GRLC_F3_MSG_ENUM_728, // GRLU RLC UL state already NULL %d
  GRLC_F3_MSG_ENUM_729, // GRLU show_paq NpOctets=%08x dup free=%u small free=%u
  GRLC_F3_MSG_ENUM_730, // ERLU TsMcTfTp=%08x NewTsMcTfTp=%08x PhMoTcGr=%08x
  GRLC_F3_MSG_ENUM_731, // ERLU NewMCS=%u WS=%u
  GRLC_F3_MSG_ENUM_732, // GRLU TsCsTfTp=%08x NewTsCsTfTp=%08x PhMoTcGr=%08x
  GRLC_F3_MSG_ENUM_733, // GRLU NewCS=%u
  GRLC_F3_MSG_ENUM_734, // ERLU AWAIT_ASS ALLOC_CNF TsMcTfTp=%08x PhMoTcGr=%08x WS=%u
  GRLC_F3_MSG_ENUM_735, // GRLU AWAIT_ASS ALLOC_CNF TsCsTfTp=%08x PhMoTcGr=%08x
  GRLC_F3_MSG_ENUM_736, // GRLU MoPtRpGC=%08x tlli=%08x Tylen_PcPi=%08x 
  GRLC_F3_MSG_ENUM_737, // VaVs %08x VfSg%08x PfNf %08x
  GRLC_F3_MSG_ENUM_738, // GRLU Ptx ctr=%d in=%d out=%d
  GRLC_F3_MSG_ENUM_739, // GRLU Ptx read_ctr=%d read_out=%d validity=%d
  GRLC_F3_MSG_ENUM_740, // GRLU Pak ctr=%d in=%d out=%d
  GRLC_F3_MSG_ENUM_741, // GRLU Pak read_ctr=%d read_out=%d dup free=%u
  GRLC_F3_MSG_ENUM_742, // GRLU Nak ctr=%d in=%d out=%d
  GRLC_F3_MSG_ENUM_743, // GRLU Nak read_ctr=%d read_out=%d
  GRLC_F3_MSG_ENUM_744, // GRLU FA=1 SnOctets=%08x 0x%08x %08x
  GRLC_F3_MSG_ENUM_745, // GRLU FA=0 SnOctets=%08x 0x%08x %08x
  GRLC_F3_MSG_ENUM_746, // ERLU FBECSSsn=%08x crbb_len=%u urbb_len=%u
  GRLC_F3_MSG_ENUM_747, // ERLU crbb[2]=%08x [1]%08x [0]%08x
  GRLC_F3_MSG_ENUM_748, // ERLU crbb[3]=%08x
  GRLC_F3_MSG_ENUM_749, // ERLU urbb[2]=%08x [1]%08x [0]%08x
  GRLC_F3_MSG_ENUM_750, // ERLU urbb[4]=%08x [3]%08x
  GRLC_F3_MSG_ENUM_751, // Temp Queue = %u items
  GRLC_F3_MSG_ENUM_752, // psn=%u orig_len=%u ptr=0x%x
  GRLC_F3_MSG_ENUM_753, // Main Queue = %u items
  GRLC_F3_MSG_ENUM_754, // psn=%u orig_len=%u ptr=0x%x
  GRLC_F3_MSG_ENUM_755, // GRLU Dump Mode
  GRLC_F3_MSG_ENUM_756, // GRLU Diag CntA=%d CntB=%d CntC=%d
  GRLC_F3_MSG_ENUM_757, // GRLU  va=%d  vs=%d vsf=%d
  GRLC_F3_MSG_ENUM_758, // GRLU seg=%d p_f=%d n_f=%d
  GRLC_F3_MSG_ENUM_759, // GRLU tbc=%u abs=%u si=%d
  GRLC_F3_MSG_ENUM_760, // GRLU #p=%u #octs=%lu #grtd=%u
  GRLC_F3_MSG_ENUM_761, // GRLU cur_blk bsn=%d ofs=%d a_bsn=%d
  GRLC_F3_MSG_ENUM_762, // GRLU cur_blk data_len=%d rem_cnt=%d cv=%d
  GRLC_F3_MSG_ENUM_763, // GRLU b_access = %u 1st_psn_to_free %u fctrl=%u
  GRLC_F3_MSG_ENUM_764, // GRLU curp=0x%x nxt=0x%x #q=%d
  GRLC_F3_MSG_ENUM_765, // GRLU p=%u last_p=%u pgstate=%d
  GRLC_F3_MSG_ENUM_766, // GRLU cur#blks=%u next#blks=%u next_s=%d
  GRLC_F3_MSG_ENUM_767, // GRLU len=%u act_len=%u rem=%u
  GRLC_F3_MSG_ENUM_768, // GRLU ori_len=%u ptc=%d rprio=%d
  GRLC_F3_MSG_ENUM_769, // GRLU #blks=%u restore=%u #pdus=%u
  GRLC_F3_MSG_ENUM_770, // GRLU PtxCtr=%d PtxRdCtr=%d NakCtr=%d
  GRLC_F3_MSG_ENUM_771, // GRLU NakRdCtr=%d PakCtr=%d PakRdCtr=%d
  GRLC_F3_MSG_ENUM_772, // GRLU validity=%d dup free = %u
  GRLC_F3_MSG_ENUM_773, // GRLU PL1_NO_GET
  GRLC_F3_MSG_ENUM_774, // GRLU dup free = %u tot = %u
  GRLC_F3_MSG_ENUM_775, // GRLU large free = %u tot = %u
  GRLC_F3_MSG_ENUM_776, // GRLU small free = %u tot = %u
  GRLC_F3_MSG_ENUM_777, // SmCntFree=%08x ,DupCntFree=%08x , SmMinDupMin=%08x
  GRLC_F3_MSG_ENUM_778, // GRLU Info state %d, evt %d, rals %d
  GRLC_F3_MSG_ENUM_779, // GRLU Info VA %d VS %d IN_INDEX %d
  GRLC_F3_MSG_ENUM_780, // GRLU Info [%d].dup_dsm_ptr not NULL
  GRLC_F3_MSG_ENUM_781, // GRLU Info [%d].dsm_ptr not NULL
  GRLC_F3_MSG_ENUM_782, // GRLU EGPRS Info [%d].dup_dsm_ptr not NULL
  GRLC_F3_MSG_ENUM_783, // GRLU EGPRS Info [%d].dsm_ptr not NULL
  GRLC_F3_MSG_ENUM_784, // GRLU Diag UL ===============================
  GRLC_F3_MSG_ENUM_785, // GRLU Diag State=%d SubState=%d
  GRLC_F3_MSG_ENUM_786, // GRLU Diag APDUs=%d AOcts=%d
  GRLC_F3_MSG_ENUM_787, // GRLU Diag UPDUs=%d UOcts=%d CS-Chngs=%d
  GRLC_F3_MSG_ENUM_788, // GRLU Diag VA=%d VS=%d NSEG=%d
  GRLC_F3_MSG_ENUM_789, // GRLU Diag VSF=%d PVSF=%d NVSF=%d
  GRLC_F3_MSG_ENUM_790, // GRLU Diag N3102=%d N3104=%d
  GRLC_F3_MSG_ENUM_791, // GRLU Diag CntA=%d CntB=%d CntC=%d
  GRLC_F3_MSG_ENUM_792, // GRLU Diag Acc-rej=%d
  GRLC_F3_MSG_ENUM_793, // GRLU Diag Retry=%d wo_retry=%d sysinfo=%d
  GRLC_F3_MSG_ENUM_794, // GRLU Diag LLC-Evts=%d MAC-Evts=%d PL1-Evts=%d
  GRLC_F3_MSG_ENUM_795, // GRLU Diag TMR-Evts=%d
  GRLC_F3_MSG_ENUM_796, // GRLU pg psn=%u state=%d last_p%u
  GRLC_F3_MSG_ENUM_797, // GRLU start await_ass_guard timer
  GRLC_F3_MSG_ENUM_798, // GRLU stop await_ass_guard timer
  GRLC_F3_MSG_ENUM_799, // GRLU stale_tmr started
  GRLC_F3_MSG_ENUM_800, // GRLU stale_tmr not started, state: %d, flag: %d
  GRLC_F3_MSG_ENUM_801, // GRLU stale_tmr stopped
  GRLC_F3_MSG_ENUM_802, // GRLU stale_tmr already stopped/expired, flag:%d
  GRLC_F3_MSG_ENUM_803, // GRLU Invalid MCS %d
  GRLC_F3_MSG_ENUM_804, // GRLU Invalid MCS %d or 2nd BSN %d
  GRLC_F3_MSG_ENUM_805, // GRLU Invalid MCS %d
  GRLC_F3_MSG_ENUM_806, // GRLU Invalid MCS %d or 1st/2nd PS %d/%d 
  GRLC_F3_MSG_ENUM_807, // EGRLU cmd_mcs %d, new_mcs %d force PS to %d
  GRLC_F3_MSG_ENUM_808, // GRLU Invalid MCS %d or PS %d
  GRLC_F3_MSG_ENUM_809, // ERLU GPRS to EGPRS
  GRLC_F3_MSG_ENUM_810, // ERLU EGPRS to GPRS
  GRLC_F3_MSG_ENUM_811, // Payload is RLC_PAYLOAD_RESERVED
  GRLC_F3_MSG_ENUM_812, // xlate_ext_mac_hdr: bad payload type
  GRLC_F3_MSG_ENUM_813, // xlate_int_mac_hdr: RLC_PAYLOAD_RESERVED
  GRLC_F3_MSG_ENUM_814, // xlate_int_mac_hdr: bad payload type
  GRLC_F3_MSG_ENUM_815, // xlate_ext_dl_data() Error stripping extension octet
  GRLC_F3_MSG_ENUM_816, // xlate_e_d Failed to strip MAC/RLC headers
  GRLC_F3_MSG_ENUM_817, // xlate_e_d Failed to strip MAC/RLC headers
  GRLC_F3_MSG_ENUM_818, // e_xlate_ext_dl_data Failed to strip payload
  GRLC_F3_MSG_ENUM_819, // ERLD DL_CORRECT_DEPADDING bsn=%u mcs=%u
  GRLC_F3_MSG_ENUM_820, // ERLD DL_CORRECT_DEPADDING MCS_3 CRC bsn=%u mcs=%u
  GRLC_F3_MSG_ENUM_821, // e_xlate_ext_dl_data Failed to strip payload
  GRLC_F3_MSG_ENUM_822, // ERLD DL_CORRECT_DEPADDING bsn=%u mcs=%u
  GRLC_F3_MSG_ENUM_823, // ERLD DL_CORRECT_DEPADDING MCS_6 CRC bsn=%u mcs=%u
  GRLC_F3_MSG_ENUM_824, // ERLD incoming BSN1 mcs 8 marked TRUE bsn=%u mcs=%u
  GRLC_F3_MSG_ENUM_825, // GRLD 2nd BSN %d - 1st BSN %d >= WS %d 
  GRLC_F3_MSG_ENUM_826, // GRLD xlate_ext_dl_data() Error stripping extension octet
  GRLC_F3_MSG_ENUM_827, // ELRD Inconsistent LI and E-bit combination when CRC pass b=%u l1=%u ext=%u
  GRLC_F3_MSG_ENUM_828, // ERLD Invalid LI sum for payload crc pass, nacking the block
  GRLC_F3_MSG_ENUM_829, // ERLD incorrect crc pass block %u fn=%ld mcs=%u
  GRLC_F3_MSG_ENUM_830, // ERLD invalid blck length mcs_len=%d pl1_len=%d
  GRLC_F3_MSG_ENUM_831, // Invalid Coding Scheme %d
  GRLC_F3_MSG_ENUM_832, // GRLD xlate_ext_dl_data() Error stripping extension octet
  GRLC_F3_MSG_ENUM_833, // ELRD Inconsistent LI and E-bit combination when CRC pass b=%u l1=%u ext=%u
  GRLC_F3_MSG_ENUM_834, // ERLD Invalid LI sum for payload crc pass, nacking the block
  GRLC_F3_MSG_ENUM_835, // ERLD incorrect crc pass block %u fn=%ld mcs=%u
  GRLC_F3_MSG_ENUM_836, // ERLD invalid unpadded blck length mcs_len=%d pl1_len=%d
  GRLC_F3_MSG_ENUM_837, // ERLD invalid blck length mcs_len=%d pl1_len=%d
  GRLC_F3_MSG_ENUM_838, // ERLD Conflicting ES/P values RLC %d vs PL1 %d
  GRLC_F3_MSG_ENUM_839, // ERLD Conflicting RRBP values RLC %d vs PL1 %d
  GRLC_F3_MSG_ENUM_840, // ERLD invalid blck length mcs_len=%d pl1_len=%d
  GRLC_F3_MSG_ENUM_841, // Invalid Coding Scheme %d
  GRLC_F3_MSG_ENUM_842, // ERLD invalid unpadded blck length mcs_len=%d pl1_len=%d
  GRLC_F3_MSG_ENUM_843, // ERLD invalid blck length mcs_len=%d pl1_len=%d
  GRLC_F3_MSG_ENUM_844, // xlate_e_d Failed to strip payload
  GRLC_F3_MSG_ENUM_845, // GRLU xlate_int non-null payload post-strip l=%u
  GRLC_F3_MSG_ENUM_846, // ERLU xlate_ext OOR MCS %u
  GRLC_F3_MSG_ENUM_847, // xlate_e_d Failed to strip payload
  GRLC_F3_MSG_ENUM_848, // GRLU xlate_int non-null payload post-strip l=%u
  GRLC_F3_MSG_ENUM_849, // xlate_e_d Failed to strip payload
  GRLC_F3_MSG_ENUM_850, // xlate_e_d Failed to strip payload
  GRLC_F3_MSG_ENUM_851, // GRLU xlate_int non-null payload post-strip l=%u
  GRLC_F3_MSG_ENUM_852, // xlate_e_d Failed to strip payload
  GRLC_F3_MSG_ENUM_853, // GRLU xlate_int non-null payload post-strip l=%u
  GRLC_F3_MSG_ENUM_854, // xlate_e_d Failed to strip payload
  GRLC_F3_MSG_ENUM_855, // GRLU xlate_int non-null payload post-strip l=%u
  GRLC_F3_MSG_ENUM_856, // Invalid Coding Scheme %d
  GRLC_F3_MSG_ENUM_857, // 
  GRLC_F3_MSG_ENUM_858, // GRLU RM_UL_REALLOC_REQ prev_realloc_psn: old=%d, new=%d, pt=%d, rp=%d
  GRLC_F3_MSG_ENUM_859, // ERLD send_pdu pnum=%d plen=%d octs=%d
  GRLC_F3_MSG_ENUM_860, // ERLD send_pdu pnum=%d plen=%d octs=%d
  GRLC_F3_MSG_ENUM_861, // p_rx_arr already initialised
  GRLC_F3_MSG_ENUM_862, // RX array allocated OK
  GRLC_F3_MSG_ENUM_863, // ERLD DL RX Workspace free
  GRLC_F3_MSG_ENUM_864, // GRLD qsh_cb action = %u
  GRLC_F3_MSG_ENUM_865, // GRLD qsh_cb METRIC_CFG action=%u subs_id=%u
  GRLC_F3_MSG_ENUM_866, // GRLD qsh_cb sig already set
  GRLC_F3_MSG_ENUM_867, // GRLD qsh_cb action oor = %u
  GRLC_F3_MSG_ENUM_868, // ERLD t3192 not started state=%u
  GRLC_F3_MSG_ENUM_869, // ERLD block tuneaway %d
  GRLC_F3_MSG_ENUM_870, // ERLD unblock tuneaway %d
  GRLC_F3_MSG_ENUM_871, // GRLD MAC event without msg
  GRLC_F3_MSG_ENUM_872, // GRLD NULL MAC msg null
  GRLC_F3_MSG_ENUM_873, // GRLD DS_NULL ##MR_DL_ASS_IND not expected in PSHO
  GRLC_F3_MSG_ENUM_874, // ERLD NULL WS=%u in DL_ASS_IND
  GRLC_F3_MSG_ENUM_875, // ERLD NULL PDA unxp TBF_MODE %u
  GRLC_F3_MSG_ENUM_876, // GRLD NULL Rcvd DL_ASS mode=%d tlli=0x%x ws=%u
  GRLC_F3_MSG_ENUM_877, // GRLD DS_NULL ##MR_DL_PSHO_START_IND not expected
  GRLC_F3_MSG_ENUM_878, // GRLD DS_NULL PSHO MR_DL_PSHO_START_IND tbf_mode=%d
  GRLC_F3_MSG_ENUM_879, // GRLD DS_NULL PSHO MR_DL_PSHO_COMPLETE_IND
  GRLC_F3_MSG_ENUM_880, // GRLD DS_NULL PSHO ##Invalid MR_DL_PSHO_COMPLETE_IND as b_psho_active is FALSE
  GRLC_F3_MSG_ENUM_881, // GRLD NULL Unexp MAC sig %d
  GRLC_F3_MSG_ENUM_882, // GRLD NULL PL1 msg null
  GRLC_F3_MSG_ENUM_883, // GRLD NULL PL1 EPDAN_REQ without DL_ASS
  GRLC_F3_MSG_ENUM_884, // GRLD NULL PL1 EPDAN_REQ without DL_ASS
  GRLC_F3_MSG_ENUM_885, // GRLD NULL PL1 EPDAN_REQ PSHO ignored
  GRLC_F3_MSG_ENUM_886, // GRLD NULL invalid PL1 msg
  GRLC_F3_MSG_ENUM_887, // GRLD NULL PH_DATA_IND without DL_ASS
  GRLC_F3_MSG_ENUM_888, // GRLD un-exp DL empty
  GRLC_F3_MSG_ENUM_889, // GRLD R_U_P_R discard block
  GRLC_F3_MSG_ENUM_890, // GRLD un-exp DL empty
  GRLC_F3_MSG_ENUM_891, // GRLD R_U_P_R discard block
  GRLC_F3_MSG_ENUM_892, // GRLD TIMER event without msg
  GRLC_F3_MSG_ENUM_893, // GRLD NULL Unexp timer sig %d
  GRLC_F3_MSG_ENUM_894, // NULL Unexp event %d
  GRLC_F3_MSG_ENUM_895, // GRLD RX_A Rcvd PH_DATA_IND
  GRLC_F3_MSG_ENUM_896, // ERLD RX_A PSHO ##EPDAN_SIG ignored
  GRLC_F3_MSG_ENUM_897, // GRLD Rcvd unexpected DE_PL1 event
  GRLC_F3_MSG_ENUM_898, // RX_ACK sig without msg
  GRLC_F3_MSG_ENUM_899, // GRLD R_A Rcvd DL_REL
  GRLC_F3_MSG_ENUM_900, // GRLD DL ACK Rel, Proc DL FIFO - start
  GRLC_F3_MSG_ENUM_901, // GRLD DL ACK Rel, Proc DL FIFO - end
  GRLC_F3_MSG_ENUM_902, // GRLD RX_A Rcvd DL_TIMER_IND(MAC)
  GRLC_F3_MSG_ENUM_903, // GRLD RX_A MR_DL_ASS_IND not expected in PSHO
  GRLC_F3_MSG_ENUM_904, // ERLD RX_A WS=%u in DL_ASS_IND curr ws=%u not updated nts =%d
  GRLC_F3_MSG_ENUM_905, // ERLD RX_A Rcvd invalid DL_ASS_IND tbf_m %d, curr_tbf_m %d, rlc_m %d
  GRLC_F3_MSG_ENUM_906, // GRLD RX_A DL_ASS mode=%d tlli=0x%x ws=%u
  GRLC_F3_MSG_ENUM_907, // GRLD RX_A ##MR_DL_PSHO_START_IND not expected
  GRLC_F3_MSG_ENUM_908, // GRLD RX_A PSHO MR_DL_PSHO_START_IND tbf_mode=%d
  GRLC_F3_MSG_ENUM_909, // GRLD RX_A PSHO MR_DL_PSHO_COMPLETE_IND
  GRLC_F3_MSG_ENUM_910, // GRLD RX_A ##Invalid MR_DL_PSHO_COMPLETE_IND as b_psho_active is FALSE
  GRLC_F3_MSG_ENUM_911, // ERLD RX_U Rcvd invalid Test Mode %d
  GRLC_F3_MSG_ENUM_912, // GRLD RX_A Unexp MAC sig %d
  GRLC_F3_MSG_ENUM_913, // GRLD null timer msg
  GRLC_F3_MSG_ENUM_914, // GRLD RX_A Expy T3190 %u
  GRLC_F3_MSG_ENUM_915, // RX_ACK Unexp timer sig %d
  GRLC_F3_MSG_ENUM_916, // GRLD null MAC msg
  GRLC_F3_MSG_ENUM_917, // GRLD RX_A_P_R Rcvd DL_REL
  GRLC_F3_MSG_ENUM_918, // GRLD RX_A_P_R DL_ASS IGNORED as Releasing DL TBF mode=%d ctrl_ack=0x%x
  GRLC_F3_MSG_ENUM_919, // GRLD RX_A_P_R DL_ASS  -- T3192 NOT RUNNING
  GRLC_F3_MSG_ENUM_920, // ERLD NULL WS=%u in DL_ASS_IND
  GRLC_F3_MSG_ENUM_921, // ERLD NULL PDA unxp TBF_MODE %u
  GRLC_F3_MSG_ENUM_922, // GRLD RX_A_P_R DL_ASS mode=%d tlli=0x%x ws=%u
  GRLC_F3_MSG_ENUM_923, // GRLD REL_ACK ##DS_REL_ACK MR_DL_PSHO_START_IND not expected
  GRLC_F3_MSG_ENUM_924, // GRLD REL_ACK PSHO MR_DL_PSHO_START_IND  tbf_mode=%d
  GRLC_F3_MSG_ENUM_925, // GRLD REL_ACK PSHO MR_DL_PSHO_COMPLETE_IND
  GRLC_F3_MSG_ENUM_926, // GRLD REL_ACK ##Invalid MR_DL_PSHO_COMPLETE_IND as b_psho_active is FALSE
  GRLC_F3_MSG_ENUM_927, // GRLD RX_A_P_REL null timer msg
  GRLC_F3_MSG_ENUM_928, // GRLD RX_A_P_REL Expy T3192 %u
  GRLC_F3_MSG_ENUM_929, // GRLD RX_A Unexp timer sig %d
  GRLC_F3_MSG_ENUM_930, // ERLD REL_ACK PSHO ##EPDAN_SIG ignored
  GRLC_F3_MSG_ENUM_931, // GRLD Rcvd unexpected DE_PL1 event
  GRLC_F3_MSG_ENUM_932, // GRLD RX_A_P_REL Unexp event %d
  GRLC_F3_MSG_ENUM_933, // GRLD RX_U Rcvd PH_DATA_IND
  GRLC_F3_MSG_ENUM_934, // ERLD RX_U PSHO ##EPDAN_SIG ignored by RLC
  GRLC_F3_MSG_ENUM_935, // GRLD U Rcvd unexpected DE_PL1 event
  GRLC_F3_MSG_ENUM_936, // RX_UNACK sig without msg
  GRLC_F3_MSG_ENUM_937, // GRLD RX_U Rcvd DL_REL
  GRLC_F3_MSG_ENUM_938, // GRLD DL UNACK Rel, Proc DL FIFO - start
  GRLC_F3_MSG_ENUM_939, // GRLD DL UNACK Rel, Proc DL FIFO - end
  GRLC_F3_MSG_ENUM_940, // GRLD RX_U ##MR_DL_ASS_IND not expected in PSHO
  GRLC_F3_MSG_ENUM_941, // ERLD RX_U WS=%u in DL_ASS_IND curr ws=%u not updated nts=%d
  GRLC_F3_MSG_ENUM_942, // ERLD RX_A Rcvd invalid DL_ASS_IND tbf_m %d, curr_tbf_m %d, rlc_m %d
  GRLC_F3_MSG_ENUM_943, // GRLD RX_U ##MR_DL_PSHO_START_IND not expected
  GRLC_F3_MSG_ENUM_944, // GRLD RX_U PSHO MR_DL_PSHO_START_IND tbf_mode=%d
  GRLC_F3_MSG_ENUM_945, // GRLD RX_U PSHO MR_DL_PSHO_COMPLETE_IND
  GRLC_F3_MSG_ENUM_946, // GRLD RX_U ##Invalid MR_DL_PSHO_COMPLETE_IND as b_psho_active is FALSE
  GRLC_F3_MSG_ENUM_947, // ERLD RX_U Rcvd invalid Test Mode %d
  GRLC_F3_MSG_ENUM_948, // GRLD RX_U Unexp MAC sig %d
  GRLC_F3_MSG_ENUM_949, // GRLD RX_U null timer msg
  GRLC_F3_MSG_ENUM_950, // GRLD RX_U Expy T3190 %u
  GRLC_F3_MSG_ENUM_951, // GRLD RX_U Unexp event %d
  GRLC_F3_MSG_ENUM_952, // GRLD null MAC msg pointer
  GRLC_F3_MSG_ENUM_953, // GRLD RX_U_P_R Rcvd DL_REL
  GRLC_F3_MSG_ENUM_954, // GRLD RX_U_P_REL DL_ASS IGNORED as Releasing DL TBF mode=%d ctrl_ack=0x%x
  GRLC_F3_MSG_ENUM_955, // GRLD RX_U_P_REL DL_ASS  -- T3192 NOT RUNNING
  GRLC_F3_MSG_ENUM_956, // GRLD RX_U_P_REL DL_ASS mode=%d tlli=0x%x nts=%d
  GRLC_F3_MSG_ENUM_957, // ERLD NULL WS=%u in DL_ASS_IND
  GRLC_F3_MSG_ENUM_958, // ERLD NULL PDA unxp TBF_MODE %u
  GRLC_F3_MSG_ENUM_959, // GRLD REL_UNACK PSHO ##MR_DL_PSHO_START_IND not expected
  GRLC_F3_MSG_ENUM_960, // GRLD REL_UNACK PSHO MR_DL_PSHO_START_IND tbf_mode=%d
  GRLC_F3_MSG_ENUM_961, // GRLD REL_UNACK PSHO MR_DL_PSHO_COMPLETE_IND
  GRLC_F3_MSG_ENUM_962, // GRLD REL_UNACK PSHO ##Invalid MR_DL_PSHO_COMPLETE_IND as b_psho_active is FALSE
  GRLC_F3_MSG_ENUM_963, // GRLD null timer msg pointer
  GRLC_F3_MSG_ENUM_964, // GRLD RX_U_P_REL Expy T3192 %u
  GRLC_F3_MSG_ENUM_965, // GRLD RX_U_P_REL Unexp timer sig %d
  GRLC_F3_MSG_ENUM_966, // ERLD REL_UNACK PSHO ##EPDAN_SIG ignored by RLC
  GRLC_F3_MSG_ENUM_967, // ERLD U PEND_REL rcvd unexpected DE_PL1 event
  GRLC_F3_MSG_ENUM_968, // GRLD RX_U_P_REL Unexp event %d
  GRLC_F3_MSG_ENUM_969, // GRLD RX_U Rcvd PH_DATA_IND in DS_TEST_MODE
  GRLC_F3_MSG_ENUM_970, // GRLD Invalid GPRS TEST MODE Rcvd PH_DATA_IND in SRB
  GRLC_F3_MSG_ENUM_971, // GRLD Rcvd unexpected DE_PL1 event
  GRLC_F3_MSG_ENUM_972, // GRLD TestMode Rcvd DL_REL
  GRLC_F3_MSG_ENUM_973, // GRLD TestMode Rcvd MR_DL_ASS_IND nts %d- No Action
  GRLC_F3_MSG_ENUM_974, // GRLD DS_TEST_MODE RX_U Unexp MAC sig %d
  GRLC_F3_MSG_ENUM_975, // GRLD DS_TEST_MODE Invalid MAC sig ptr %p
  GRLC_F3_MSG_ENUM_976, // GRLD TestMode null timer msg %p
  GRLC_F3_MSG_ENUM_977, // GRLD TestMode Expy T3190 %u
  GRLC_F3_MSG_ENUM_978, // GRLD TestMode B RX_U Invalid timer sig %d
  GRLC_F3_MSG_ENUM_979, // GRLD DS_TEST_MODE Unexp event %d
  GRLC_F3_MSG_ENUM_980, // Invalid downlink state %d
  GRLC_F3_MSG_ENUM_981, // ERLD padding_removal_req() wrong hdr_type %u
  GRLC_F3_MSG_ENUM_982, // GRLD dl_init blk[%u], vq=%d.dsm not NULL
  GRLC_F3_MSG_ENUM_983, // GRLD Partial Split : dl_init blk[%u], vq=%d.dsm not NULL
  GRLC_F3_MSG_ENUM_984, // GRLD dl_init non-NULL dsm and vn=INVALID at rx_arr[%u] ptr=%x
  GRLC_F3_MSG_ENUM_985, // GRLD dl_init non-NULL dsm and vn=ASSEMBLED at rx_arr[%u] ptr=%x
  GRLC_F3_MSG_ENUM_986, // GRLD dl_init bad vn(%u) rx_arr[%u] ptr=%x
  GRLC_F3_MSG_ENUM_987, // GRLD cur_dl_blk[].dsm not NULL
  GRLC_F3_MSG_ENUM_988, // GRLD ##Invalid tbf mode
  GRLC_F3_MSG_ENUM_989, // GRLD Resetting ..
  GRLC_F3_MSG_ENUM_990, // GRLD handle_qsh_sig ACTION_START active addr=0x%x
  GRLC_F3_MSG_ENUM_991, // GRLD handle_qsh_sig b_metrics_active already set
  GRLC_F3_MSG_ENUM_992, // GRLD handle_qsh_sig ACTION_STOP inactive
  GRLC_F3_MSG_ENUM_993, // GRLD unexpected ACTION %d
  GRLC_F3_MSG_ENUM_994, // ERLD qsh metric_dump_ptr NULL on exit
  GRLC_F3_MSG_ENUM_995, // ERLD qsh metric_dump_ptr NULL on entry
  GRLC_F3_MSG_ENUM_996, // GRLD fifo_clr in=%d out=%d ctr=%d
  GRLC_F3_MSG_ENUM_997, // GRLD fifo_clr in=%d out=%d ctr=%d
  GRLC_F3_MSG_ENUM_998, // GRLD FIFO re-init (pre-re-init values) in=%u out=%u ctr=%u
  GRLC_F3_MSG_ENUM_999, // GRLD fifo_clr in=%d out=%d ctr=%d
  GRLC_F3_MSG_ENUM_1000, // GRLD FIFO re-init (pre-re-init values) in=%u out=%u ctr=%u
  GRLC_F3_MSG_ENUM_1001, // GRLD FIFO PL1 put GPRS block whilst in EGPRS mode ctr=%u in=%u out=%u
  GRLC_F3_MSG_ENUM_1002, // GRLD FIFO IN <before write> ctr=%u in_ind=%u, out_ind=%u
  GRLC_F3_MSG_ENUM_1003, // GRLD FIFO OUT <before read> ctr=%u in_ind=%u, out_ind=%u
  GRLC_F3_MSG_ENUM_1004, // ERLD FIFO PL1 put EGPRS block whilst in GPRS mode ctr=%u in=%u out=%u
  GRLC_F3_MSG_ENUM_1005, // GRLD FIFO IN <before write> ctr=%u in_ind=%u, out_ind=%u
  GRLC_F3_MSG_ENUM_1006, // ERLD FIFO put block with status UNREAD ctr=%d, in_ind=%d, out_ind=%d
  GRLC_F3_MSG_ENUM_1007, // ERLD FIFO put block with 1st pld len=0
  GRLC_F3_MSG_ENUM_1008, // ERLD FIFO put block with 1st pld dsm ref_cnt=%d
  GRLC_F3_MSG_ENUM_1009, // ERLD FIFO put block with 2nd pld len=0
  GRLC_F3_MSG_ENUM_1010, // ERLD FIFO put block with 2nd pld dsm ref_cnt=%d
  GRLC_F3_MSG_ENUM_1011, // GRLD FIFO OUT <before read> ctr=%u in_ind=%u, out_ind=%u
  GRLC_F3_MSG_ENUM_1012, // ERLD FIFO get block with status READ ctr=%d, in_ind=%d, out_ind=%d
  GRLC_F3_MSG_ENUM_1013, // ERLD FIFO get block with 1st pld len=0
  GRLC_F3_MSG_ENUM_1014, // ERLD FIFO get block with 1st pld dsm ref_cnt=%d
  GRLC_F3_MSG_ENUM_1015, // ERLD FIFO get block with 2nd pld len=0
  GRLC_F3_MSG_ENUM_1016, // ERLD FIFO get block with 2nd pld dsm ref_cnt=%d
  GRLC_F3_MSG_ENUM_1017, // G/ERLD A/U BSN=%d FBI=%d FIFO 00CtInOu=%08x
  GRLC_F3_MSG_ENUM_1018, // ERLD A blk CFEsCFEe=%08x M0bs1bs2=%08x FIFO 00CtInOu=%08x
  GRLC_F3_MSG_ENUM_1019, // ERLD U blk CFEsCFEe=%08x M0bs1bs2=%08x FIFO 00CtInOu=%08x
  GRLC_F3_MSG_ENUM_1020, // ERLD BSN=%u, len=%u
  GRLC_F3_MSG_ENUM_1021, // ERLD BSN=%u, len=%u
  GRLC_F3_MSG_ENUM_1022, // ERLD DATA BLK: %08x %08x %08x
  GRLC_F3_MSG_ENUM_1023, // ERLU post_tx bsn(%u) != vs(%u) vb[vs]=%u
  GRLC_F3_MSG_ENUM_1024, // ERLU post_tx ind(%d) becomes PACK bsn(%d) (PL1 mis-ack)
  GRLC_F3_MSG_ENUM_1025, // ERLU post bsn=%u RA_AT_TX_COMPLETE aborted cv=0
  GRLC_F3_MSG_ENUM_1026, // ERLU post bsn=%u RA_AT_TX_COMPLETE
  GRLC_F3_MSG_ENUM_1027, // ERLU post bsn=%u RA_AT_TX_COMPLETE
  GRLC_F3_MSG_ENUM_1028, // ERLU post T3182 start CV=0 NoNack va=%u vs=%u seg=%u
  GRLC_F3_MSG_ENUM_1029, // ERLU post WinFull start T3182 va=%u vs=%d SI=%u
  GRLC_F3_MSG_ENUM_1030, // GRLU post_tx bsn(%u) != vs(%u) vb[vs]=%u
  GRLC_F3_MSG_ENUM_1031, // GRLU post_tx ind(%d) becomes PACK bsn(%d) (PL1 mis-ack)
  GRLC_F3_MSG_ENUM_1032, // GRLU post bsn=%u RA_AT_TX_COMPLETE aborted cv=0
  GRLC_F3_MSG_ENUM_1033, // GRLU post bsn=%u RA_AT_TX_COMPLETE
  GRLC_F3_MSG_ENUM_1034, // GRLU post bsn=%u RA_AT_TX_COMPLETE
  GRLC_F3_MSG_ENUM_1035, // GRLU post T3182 start CV=0 NoNack va=%d vs=%d seg=%d
  GRLC_F3_MSG_ENUM_1036, // GRLU post WinFull start T3182 va=%u vs=%u SI=%u
  GRLC_F3_MSG_ENUM_1037, // GRLU one-time p_ta_arr not NULL
  GRLC_F3_MSG_ENUM_1038, // TX_ARR size = %lu
  GRLC_F3_MSG_ENUM_1039, // TX_ARR allocated OK
  GRLC_F3_MSG_ENUM_1040, // GRLU - UL Resumed
  GRLC_F3_MSG_ENUM_1041, // GRLU S=%u action on ps_access=%u BS_CV_MAX=%u
  GRLC_F3_MSG_ENUM_1042, // GRLC UL Alloc cnf Await. Show FIFO then re-init
  GRLC_F3_MSG_ENUM_1043, // ERLU A ALLOC_CNF #TS=%d mcs=%d typ=%d
  GRLC_F3_MSG_ENUM_1044, // ERLU chMCS/TFI/TS ul[gas_id].mcs=%u alloc_cnf.mcs=%u
  GRLC_F3_MSG_ENUM_1045, // GRLU A ALLOC_CNF #TS=%d cs=%d typ=%d
  GRLC_F3_MSG_ENUM_1046, // GRLU chCS/TFI/TS cs=%d new cs=%d
  GRLC_F3_MSG_ENUM_1047, // GRLU U UnsolAss old TS=%d new TS=%d
  GRLC_F3_MSG_ENUM_1048, // ERLU U chMCS/TFI/TS mcs=%u new mcs=%u
  GRLC_F3_MSG_ENUM_1049, // GRLU U ALLOC_CNF #TS=%d cs=%d typ=%d
  GRLC_F3_MSG_ENUM_1050, // GRLU U UnsolAss old TS=%d new TS=%d
  GRLC_F3_MSG_ENUM_1051, // GRLU close-ended converted to open-ended
  GRLC_F3_MSG_ENUM_1052, // GRLU PSHO ##rlc_mode diff in cur_pdu rlc_mode =%u ass_rlc_mode=%u
  GRLC_F3_MSG_ENUM_1053, // GRLU US_NULL event without msg
  GRLC_F3_MSG_ENUM_1054, // GRLU NULL cur_access=%u no-access
  GRLC_F3_MSG_ENUM_1055, // GRLU NULL PSHO ignore access-ind in PSHO
  GRLC_F3_MSG_ENUM_1056, // GRLU NULL cur_access=%u access
  GRLC_F3_MSG_ENUM_1057, // GRLU NULL cur_access=%u access
  GRLC_F3_MSG_ENUM_1058, // GRLU GTTP psn=%d
  GRLC_F3_MSG_ENUM_1059, // GRLU NULL PSHO_START whilst ALREADY active
  GRLC_F3_MSG_ENUM_1060, // GRLU NULL PSHO_START access is now FALSE
  GRLC_F3_MSG_ENUM_1061, // GRLU NULL PSHO_COMPLETE whilst NOT active
  GRLC_F3_MSG_ENUM_1062, // GRLU A PSHO_COMPLETE succ = %u type = %u rlc_mode=%u
  GRLC_F3_MSG_ENUM_1063, // GRLU NULL PSHO_COMPLETE success
  GRLC_F3_MSG_ENUM_1064, // GRLU OTInit SetN3102=max=%d inc=%d dec=%d
  GRLC_F3_MSG_ENUM_1065, // GRLU NULL PSHO_COMPLETE failure
  GRLC_F3_MSG_ENUM_1066, // GRLU NULL Unexp MAC sig %d
  GRLC_F3_MSG_ENUM_1067, // GRLU AWAIT_ASS MAC event without msg
  GRLC_F3_MSG_ENUM_1068, // GRLU Test Mode in US_AWAIT_ASS Rcvd Unexp sig %d , send stop test_mode to LLC
  GRLC_F3_MSG_ENUM_1069, // GRLU close-ended converted to open-ended
  GRLC_F3_MSG_ENUM_1070, // GRLU AWAIT_ASS GTTP ##dup returns NULL
  GRLC_F3_MSG_ENUM_1071, // GRLU AWAIT_ASS send PDU via GRR: PSN %d LEN %d TLLI %08x
  GRLC_F3_MSG_ENUM_1072, // GRLU AWAIT_ASS ##cur_pdu_ptr NULL on entering into GTTP_XFER
  GRLC_F3_MSG_ENUM_1073, // GRLU Est UL tbf for Test Mode
  GRLC_F3_MSG_ENUM_1074, // GRLU Invalid Test Mode %d
  GRLC_F3_MSG_ENUM_1075, // GRLC AWAIT_ASS no-access flag=%u
  GRLC_F3_MSG_ENUM_1076, // GRLU AWAIT_ASS PSHO_START whilst ALREADY active
  GRLC_F3_MSG_ENUM_1077, // GRLC AWAIT_ASS PSHO_START
  GRLC_F3_MSG_ENUM_1078, // GRLU AWAIT_ASS action on ps_access=%u
  GRLC_F3_MSG_ENUM_1079, // GRLU AWAIT_ASS Unexp MAC sig %d
  GRLC_F3_MSG_ENUM_1080, // GRLU A PSHO_COMPLETE type = %u reset_flag=%u rlc_mode=%u
  GRLC_F3_MSG_ENUM_1081, // GRLU OTInit SetN3102=max=%d inc=%d dec=%d
  GRLC_F3_MSG_ENUM_1082, // GRLU A PSHO_COMPLETE success bad type %u
  GRLC_F3_MSG_ENUM_1083, // GRLU A PSHP_COMPLETE bad status %u
  GRLC_F3_MSG_ENUM_1084, // GRLU A MAC event without msg
  GRLC_F3_MSG_ENUM_1085, // GRLU A ResN3104=%d max=%d
  GRLC_F3_MSG_ENUM_1086, // ERLU A EPUAN pre_tx=%u resegment=%u tfi=%u
  GRLC_F3_MSG_ENUM_1087, // ERLU FAI XTimer stopped
  GRLC_F3_MSG_ENUM_1088, // ERLU prr TX_COMPLETED psn=%u
  GRLC_F3_MSG_ENUM_1089, // GRLU FAI XTimer stopped
  GRLC_F3_MSG_ENUM_1090, // GRLU prr TX_COMPLETED psn=%u
  GRLC_F3_MSG_ENUM_1091, // GRLU close-ended converted to open-ended
  GRLC_F3_MSG_ENUM_1092, // ERLU A ##cur_tbf_mode=%u next_tbf_mode=%u
  GRLC_F3_MSG_ENUM_1093, // GRLU XFER no-access
  GRLC_F3_MSG_ENUM_1094, // GRLU A XFER PSHO_START whilst ALREADY active
  GRLC_F3_MSG_ENUM_1095, // GRLU A XFER PSHO_START
  GRLC_F3_MSG_ENUM_1096, // GRLU A XFER PSHO_COMPLETE whilst PSHO not active
  GRLC_F3_MSG_ENUM_1097, // GRLU A XFER PSHO_COMPLETE
  GRLC_F3_MSG_ENUM_1098, // GRLU XFER access (ignore access = %u) PSHO=%u
  GRLC_F3_MSG_ENUM_1099, // GRLU XFER action on ps_access=%u
  GRLC_F3_MSG_ENUM_1100, // GRLU A Rcvd REL_IND cause %d
  GRLC_F3_MSG_ENUM_1101, // ERLU A Rcvd CONTENTION_IND petx=%u reseg=%u
  GRLC_F3_MSG_ENUM_1102, // GRLU A Unexp sig %d
  GRLC_F3_MSG_ENUM_1103, // GRLU A tmr event without msg
  GRLC_F3_MSG_ENUM_1104, // GRLU A Expy Stale PDU tmr %u
  GRLC_F3_MSG_ENUM_1105, // GRLU - UL Suspended
  GRLC_F3_MSG_ENUM_1106, // GRLU A Unexp TMR sig %d
  GRLC_F3_MSG_ENUM_1107, // GRLU A tmr event without msg
  GRLC_F3_MSG_ENUM_1108, // GRLU A Expy T3182 %u NoAcknack
  GRLC_F3_MSG_ENUM_1109, // GRLU A DecN3102=%d inc=%d dec=%d
  GRLC_F3_MSG_ENUM_1110, // GRLU A N3102 Expy (cell-resel) = %d inc=%d dec=%d
  GRLC_F3_MSG_ENUM_1111, // GRLU A Expy MAC_RESP
  GRLC_F3_MSG_ENUM_1112, // ERLU XTimer expiry cnt = %u
  GRLC_F3_MSG_ENUM_1113, // ERLU XTimer expiry: no release cnt = %u
  GRLC_F3_MSG_ENUM_1114, // GRLU A Unexp TMR sig %d
  GRLC_F3_MSG_ENUM_1115, // GRLU GTTP MAC event without msg
  GRLC_F3_MSG_ENUM_1116, // GRLU GTTP psn %u sent OK
  GRLC_F3_MSG_ENUM_1117, // GRLU GTTP RELEASE_IND Unexp cause %u
  GRLC_F3_MSG_ENUM_1118, // GRLU GTTP Failed. To resend psn %u delete_flag=%u
  GRLC_F3_MSG_ENUM_1119, // GRLU GTTP Unexp MAC sig %u
  GRLC_F3_MSG_ENUM_1120, // GRLU U MAC event without msg
  GRLC_F3_MSG_ENUM_1121, // GRLU U T3182 stopped paknak
  GRLC_F3_MSG_ENUM_1122, // ERLU U paknak FA=%d SSN=%d
  GRLC_F3_MSG_ENUM_1123, // GRLU Invalid MCS %d
  GRLC_F3_MSG_ENUM_1124, // GRLU Invalid bsn_cv_0 = %u
  GRLC_F3_MSG_ENUM_1125, // GRLU U paknak FA=%d SSN=%d
  GRLC_F3_MSG_ENUM_1126, // GRLU close-ended converted to open-ended
  GRLC_F3_MSG_ENUM_1127, // ERLU U ##cur_tbf_mode=%u next_tbf_mode=%u
  GRLC_F3_MSG_ENUM_1128, // GRLU U Rcvd REL_IND cause %d
  GRLC_F3_MSG_ENUM_1129, // GRLU U Rcvd CONTENTION_IND petx=%u reseg=%u
  GRLC_F3_MSG_ENUM_1130, // GRLU UN-XFER no-access delete_flag=%u
  GRLC_F3_MSG_ENUM_1131, // GRLU GPRS-TEST-MODE-A Release - Purge all PDUs
  GRLC_F3_MSG_ENUM_1132, // GRLU U XFER PSHO_START whilst ALREADY active
  GRLC_F3_MSG_ENUM_1133, // GRLU U XFER PSHO_START
  GRLC_F3_MSG_ENUM_1134, // GRLU U XFER PSHO_COMPLETE whilst PSHO not active
  GRLC_F3_MSG_ENUM_1135, // GRLU U XFER PSHO_COMPLETE
  GRLC_F3_MSG_ENUM_1136, // GRLU U XFER access (ignore access = %u) PSHO=%u
  GRLC_F3_MSG_ENUM_1137, // GRLU UN-XFER action on ps_access=%u
  GRLC_F3_MSG_ENUM_1138, // GRLU U Unexp MAC sig %d
  GRLC_F3_MSG_ENUM_1139, // GRLU U timer event without msg
  GRLC_F3_MSG_ENUM_1140, // GRLU U Expy T3182 NoAcknack %u
  GRLC_F3_MSG_ENUM_1141, // GRLU U DecN3102=%d inc=%d dec=%d
  GRLC_F3_MSG_ENUM_1142, // GRLU U N3102 Expy = %d inc=%d dec=%d
  GRLC_F3_MSG_ENUM_1143, // GRLU U Expy MAC_RESP
  GRLC_F3_MSG_ENUM_1144, // GRLU Test Mode rcvd MAC event without msg
  GRLC_F3_MSG_ENUM_1145, // GRLU Test Mode Rcvd REL_IND cause %d
  GRLC_F3_MSG_ENUM_1146, // ERLU TEST MODE paknak FA=%d SSN=%d
  GRLC_F3_MSG_ENUM_1147, // GRLU TEST MODE paknak FA=%d SSN=%d
  GRLC_F3_MSG_ENUM_1148, // GRLU Test Mode rcved alloc_cnf
  GRLC_F3_MSG_ENUM_1149, // GRLU Test Mode rcvd ivalid MAC sig %d
  GRLC_F3_MSG_ENUM_1150, // GRLU Info s=%d ll-pdus=%d ll-octs=%lu
  GRLC_F3_MSG_ENUM_1151, // GRLU ##nodata %u ext-utbf=%u pre-emptive-tx=%u
  GRLC_F3_MSG_ENUM_1152, // GRLU US_NULL Unexp PL1 sig
  GRLC_F3_MSG_ENUM_1153, // GRLU NULL Unexp evt %d
  GRLC_F3_MSG_ENUM_1154, // GRLU AWAIT_ASS tmr event without msg
  GRLC_F3_MSG_ENUM_1155, // GRLU AWAIT_ASS Expy AWAIT_ASS_GUARD = %u
  GRLC_F3_MSG_ENUM_1156, // GRLU AWAIT_ASS Expy MAC_RESP
  GRLC_F3_MSG_ENUM_1157, // GRLU AWAIT_ASS Unexp TMR sig %d
  GRLC_F3_MSG_ENUM_1158, // GRLU AWAIT_ASS Unexp PL1 sig
  GRLC_F3_MSG_ENUM_1159, // GRLU AWAIT_ASS Unexp event %d
  GRLC_F3_MSG_ENUM_1160, // GRLU A Ignore PH_READY_TO_SEND in PSHO
  GRLC_F3_MSG_ENUM_1161, // GRLU A PSHO cur_pdu_ptr NULL PH_READY_TO_SEND Ignored
  GRLC_F3_MSG_ENUM_1162, // GRLU A Rcvd PH_READY_TO_SEND
  GRLC_F3_MSG_ENUM_1163, // GRLU A Rcvd PH_READY_TO_SEND
  GRLC_F3_MSG_ENUM_1164, // GRLU XFER UE_LLC
  GRLC_F3_MSG_ENUM_1165, // GRLU GTTP Unexp event %u
  GRLC_F3_MSG_ENUM_1166, // GRLU A Ignore PH_READY_TO_SEND in PSHO
  GRLC_F3_MSG_ENUM_1167, // GRLU U PSHO cur_pdu_ptr NULL PH_READY_TO_SEND Ignored
  GRLC_F3_MSG_ENUM_1168, // GRLU U Rcvd PH_READY_TO_SEND_IND
  GRLC_F3_MSG_ENUM_1169, // GRLU U Rcvd PH_READY_TO_SEND_IND
  GRLC_F3_MSG_ENUM_1170, // ERLU U Rcvd UE_LLC
  GRLC_F3_MSG_ENUM_1171, // GRLU Test Mode rcvd ivalid LLC sig
  GRLC_F3_MSG_ENUM_1172, // GRLU Test Mode rcvd timer event without msg
  GRLC_F3_MSG_ENUM_1173, // GRLU Test Mode rcvd Inval timer sig %d
  GRLC_F3_MSG_ENUM_1174, // GRLU Test Mode rcvd Inval UL EVENT %d
  GRLC_F3_MSG_ENUM_1175, // GRLU AWAIT_RECOVERY MAC sig %d
  GRLC_F3_MSG_ENUM_1176, // GRLU US_NULL event without msg
  GRLC_F3_MSG_ENUM_1177, // GRLU AWAIT_RECOVERY clr timer sig %d
  GRLC_F3_MSG_ENUM_1178, // GRLC invalid state %d
  GRLC_F3_MSG_ENUM_1179, // GRLU Resetting %u
  GRLC_F3_MSG_ENUM_1180, // GRLU OTInit SetN3102=max=%d inc=%d dec=%d
  GRLC_F3_MSG_ENUM_1181, // GRLU CRes failed ResN3104=%d max=%d
  GRLC_F3_MSG_ENUM_1182, // GRLU inform SM no ps access - DEL_ONE psn %u
  GRLC_F3_MSG_ENUM_1183, // GRLU inform gmm pdu tx failure to upper layer
  GRLC_F3_MSG_ENUM_1184, // GRLU handle_qsh_sig ACTION_START setting b_metrics_active addr=0x%x
  GRLC_F3_MSG_ENUM_1185, // GRLU handle_qsh_sig b_metrics_active already set
  GRLC_F3_MSG_ENUM_1186, // GRLU handle_qsh_sig ACTION_STOP inactive
  GRLC_F3_MSG_ENUM_1187, // GRLU unexpected ACTION %d
  GRLC_F3_MSG_ENUM_1188, // ERLU qsh metric_dump_ptr NULL on exit
  GRLC_F3_MSG_ENUM_1189, // ERLU qsh metric_dump_ptr NULL on entry
  GRLC_F3_MSG_ENUM_1190, // GRLU LLC Get: Invalid Prim %d
  GRLC_F3_MSG_ENUM_1191, // GRLC qsh_cb action = %u
  GRLC_F3_MSG_ENUM_1192, // GRLU qsh_cb METRIC_CFG action=%u subs_id=%u
  GRLC_F3_MSG_ENUM_1193, // GRLU qsh_cb sig already set
  GRLC_F3_MSG_ENUM_1194, // GRLC qsh_cb action oor = %u
  GRLC_F3_MSG_ENUM_1195, // Dump %08x %08x %08x
  GRLC_F3_MSG_ENUM_1196, // Dump %08x %08x %08x
  GRLC_F3_MSG_ENUM_1197, // Dump %08x %08x %08x
  GRLC_F3_MSG_ENUM_1198, // Dump %08x %08x %08x
  GRLC_F3_MSG_ENUM_1199, // GRLU UL_FIFO bad ack status %d
  GRLC_F3_MSG_ENUM_1200, // GRLU FIFO_PUT Area %d full
  GRLC_F3_MSG_ENUM_1201, // GRLC UL FIFO PUT bad status %d
  GRLC_F3_MSG_ENUM_1202, // GRLU FIFO_PUT Area %d full
  GRLC_F3_MSG_ENUM_1203, // GRLC UL FIFO PUT bad status %d
  GRLC_F3_MSG_ENUM_1204, // GRLU FIFO busy access_mode=[%d]
  GRLC_F3_MSG_ENUM_1205, // GRLU ptx_rd ptx_cv=%u pak_cv=%u
  GRLC_F3_MSG_ENUM_1206, // GRLU pak_rd pak_cv=%u ptx_cv=%u
  GRLC_F3_MSG_ENUM_1207, // GRLC Read-acc no blocks
  GRLC_F3_MSG_ENUM_1208, // GRLU dec nack_read_ctr at 0
  GRLC_F3_MSG_ENUM_1209, // GRLU Get-acc: prev-rd no nack block
  GRLC_F3_MSG_ENUM_1210, // GRLU dec ptx_read_ctr at 0
  GRLC_F3_MSG_ENUM_1211, // GRLU Get-acc: prev-rd no ptx block
  GRLC_F3_MSG_ENUM_1212, // GRLU pack_read_ctr at 0
  GRLC_F3_MSG_ENUM_1213, // GRLU Get-acc: prev-rd no pak block
  GRLC_F3_MSG_ENUM_1214, // GRLC UL FIFO bad ack state %d
  GRLC_F3_MSG_ENUM_1215, // GRLU spit PACK %d pack_vsf was %d
  GRLC_F3_MSG_ENUM_1216, // GRLU spit PACK %d pack_vsf was %d
  GRLC_F3_MSG_ENUM_1217, // ERLU spit NACK %d nack_vsf was %d
  GRLC_F3_MSG_ENUM_1218, // ERLU BSN %u vb=%u not TXA_PENDING_PACK
  GRLC_F3_MSG_ENUM_1219, // ERLU 2nd BSN %d vb=%u not TXA_PENDING_PACK
  GRLC_F3_MSG_ENUM_1220, // GRLU BSN %d vn=%u not TXA_PENDING_PACK
  GRLC_F3_MSG_ENUM_1221, // ERLU NACK force nack_ctr %d to nack_read_ctr %d
  GRLC_F3_MSG_ENUM_1222, // GRLC UL FIFO bsn(%u) OOR before VS(%u)/VA(%u)
  GRLC_F3_MSG_ENUM_1223, // GRLC UL FIFO corrected bsn(vs=%u) ptx_out_ind=%u = ptx_in-ptx_ctr
  GRLC_F3_MSG_ENUM_1224, // ERLU del-unread-pak bsn=%u pak_ctr=%u pak_read_ctr=%u
  GRLC_F3_MSG_ENUM_1225, // GRLU enq psn=%u pdu_prio=%u NpOctets=%08x MoPtRpCa=%08x TyLenPcPi=%08x
  GRLC_F3_MSG_ENUM_1226, // GRLU enq psn=%u pdu_prio=%u NpOctets=%08x MoPtRpCa=%08x TyLenPcPi=%08x
  GRLC_F3_MSG_ENUM_1227, // GRLU post msg buf(%u) OVF
  GRLC_F3_MSG_ENUM_1228, // GRLU post msg buf(%u) UDF
  GRLC_F3_MSG_ENUM_1229, // ERLU post PTX 0BsnBsLn=%08x 0TbcABsn=%08x CvMcCpSi=%08x
  GRLC_F3_MSG_ENUM_1230, // ERLU post PAK 0BsnBsLn=%08x 0TbcABsn=%08x CvMcCpSi=%08x
  GRLC_F3_MSG_ENUM_1231, // ERLU post ACK 0BsnBsLn=%08x 0TbcABsn=%08x CvMcCpSi=%08x
  GRLC_F3_MSG_ENUM_1232, // ERLU post UNA 0BsnBsLn=%08x 0TbcABsn=%08x CvMcCpSi=%08x
  GRLC_F3_MSG_ENUM_1233, // ERLU post PPA 0BsnBsLn=%08x 0TbcABsn=%08x CvMcCpSi=%08x
  GRLC_F3_MSG_ENUM_1234, // ERLU post NAK 0BsnBsLn=%08x 0TbcABsn=%08x CvMcCpSi=%08x
  GRLC_F3_MSG_ENUM_1235, // ERLU post invalid ack_state %d
  GRLC_F3_MSG_ENUM_1236, // GRLU post PTX 0BsnBsLn=%08x 0TbcABsn=%08x CvCsTsSi=%08x
  GRLC_F3_MSG_ENUM_1237, // GRLU post PAK 0BsnBsLn=%08x 0TbcABsn=%08x CvCsTsSi=%08x
  GRLC_F3_MSG_ENUM_1238, // GRLU post ACK 0BsnBsLn=%08x 0TbcABsn=%08x CvCsTsSi=%08x
  GRLC_F3_MSG_ENUM_1239, // GRLU post UNA 0BsnBsLn=%08x 0TbcABsn=%08x CvCsTsSi=%08x
  GRLC_F3_MSG_ENUM_1240, // GRLU post PPA 0BsnBsLn=%08x 0TbcABsn=%08x CvCsTsSi=%08x
  GRLC_F3_MSG_ENUM_1241, // GRLU post NAK 0BsnBsLn=%08x 0TbcABsn=%08x CvCsTsSi=%08x
  GRLC_F3_MSG_ENUM_1242, // GRLU post invalid ack_state %d
  GRLC_F3_MSG_ENUM_1243, // GRLD Handling QSH signal when metrics action FALSE
  GRLC_F3_MSG_ENUM_1244, // GRLU Handling QSH signal when metrics action FALSE
  GRLC_F3_MSG_ENUM_MAX
} grlc_f3_msg_enum_t;


#ifdef FEATURE_GRLC_STORE_F3_MSG
extern void grlc_f3_msg_init(void);
extern void grlc_store_f3_msg(
  const grlc_f3_msg_enum_t f3_msg_id,
  const uint32             arg1,
  const uint32             arg2,
  const uint32             arg3,
  const gas_id_t           gas_id
);

#define GRLC_STORE_F3_MSG_0(enum, gas_id)                                         \
    grlc_store_f3_msg(enum, 0, 0, 0, gas_id);

#define GRLC_STORE_F3_MSG_1(enum, arg1, gas_id)                                   \
    grlc_store_f3_msg(enum, (uint32)arg1, 0, 0, gas_id);

#define GRLC_STORE_F3_MSG_2(enum, arg1, arg2, gas_id)                             \
    grlc_store_f3_msg(enum, (uint32)arg1, (uint32)arg2, 0, gas_id);

#define GRLC_STORE_F3_MSG_3(enum, arg1, arg2, arg3, gas_id)                       \
    grlc_store_f3_msg(enum, (uint32)arg1, (uint32)arg2, (uint32)arg3, gas_id);

#define GRLC_STORE_F3_MSG_4(enum, arg1, arg2, arg3, arg4, gas_id)                 \
  {                                                                               \
    grlc_store_f3_msg(enum, (uint32)arg1, (uint32)arg2, (uint32)arg3, gas_id);    \
    grlc_store_f3_msg(enum, (uint32)arg4, 0, 0, gas_id);                          \
  }

#define GRLC_STORE_F3_MSG_5(enum, arg1, arg2, arg3, arg4, arg5, gas_id)           \
  {                                                                               \
    grlc_store_f3_msg(enum, (uint32)arg1, (uint32)arg2, (uint32)arg3, gas_id);    \
    grlc_store_f3_msg(enum, (uint32)arg4, (uint32)arg5, 0, gas_id);               \
  }
#else /* !FEATURE_GRLC_STORE_F3_MSG */
#define GRLC_STORE_F3_MSG_0(enum, gas_id)
#define GRLC_STORE_F3_MSG_1(enum, arg1, gas_id)
#define GRLC_STORE_F3_MSG_2(enum, arg1, arg2, gas_id)
#define GRLC_STORE_F3_MSG_3(enum, arg1, arg2, arg3, gas_id)
#define GRLC_STORE_F3_MSG_4(enum, arg1, arg2, arg3, arg4, gas_id)
#define GRLC_STORE_F3_MSG_5(enum, arg1, arg2, arg3, arg4, arg5, gas_id)
#endif /* !FEATURE_GRLC_STORE_F3_MSG */

#define MSG_GRLC_0_G(prio, enum, txt)                                       \
  {                                                                         \
    GRLC_STORE_F3_MSG_0(enum, gas_id);                                      \
    MSG_GERAN_##prio##_0_G(txt);                                            \
  }                                                                         
                                                                            
#define MSG_GRLC_1_G(prio, enum, txt, arg1)                                 \
  {                                                                         \
    GRLC_STORE_F3_MSG_1(enum, arg1, gas_id);                                \
    MSG_GERAN_##prio##_1_G(txt, arg1);                                      \
  }                                                                         
                                                                            
#define MSG_GRLC_2_G(prio, enum, txt, arg1, arg2)                           \
  {                                                                         \
    GRLC_STORE_F3_MSG_2(enum, arg1, arg2, gas_id);                          \
    MSG_GERAN_##prio##_2_G(txt, arg1, arg2);                                \
  }                                                                         
                                                                            
#define MSG_GRLC_3_G(prio, enum, txt, arg1, arg2, arg3)                     \
  {                                                                         \
    GRLC_STORE_F3_MSG_3(enum, arg1, arg2, arg3, gas_id);                    \
    MSG_GERAN_##prio##_3_G(txt, arg1, arg2, arg3);                          \
  }

#define MSG_GRLC_4_G(prio, enum, txt, arg1, arg2, arg3, arg4)               \
  {                                                                         \
    GRLC_STORE_F3_MSG_4(enum, arg1, arg2, arg3, arg4, gas_id);              \
    MSG_GERAN_##prio##_4_G(txt, arg1, arg2, arg3, arg4);                    \
  }                                                                         
                                                                            
#define MSG_GRLC_5_G(prio, enum, txt, arg1, arg2, arg3, arg4, arg5)         \
  {                                                                         \
    GRLC_STORE_F3_MSG_5(enum, arg1, arg2, arg3, arg4, arg5, gas_id);        \
    MSG_GERAN_##prio##_5_G(txt, arg1, arg2, arg3, arg4, arg5);              \
  }                                                                         
                                                                            
#define MSG_GRLC_0(prio, enum, txt)                                         \
  {                                                                         \
    GRLC_STORE_F3_MSG_0(enum, INVALID_GAS_ID);                              \
    MSG_GERAN_##prio##_0(txt);                                              \
  }                                                                         
                                                                            
#define MSG_GRLC_1(prio, enum, txt, arg1)                                   \
  {                                                                         \
    GRLC_STORE_F3_MSG_1(enum, arg1, INVALID_GAS_ID);                        \
    MSG_GERAN_##prio##_1(txt, arg1);                                        \
  }                                                                         
                                                                            
#define MSG_GRLC_2(prio, enum, txt, arg1, arg2)                             \
  {                                                                         \
    GRLC_STORE_F3_MSG_2(enum, arg1, arg2, INVALID_GAS_ID);                  \
    MSG_GERAN_##prio##_2(txt, arg1, arg2);                                  \
  }                                                                         
                                                                            
#define MSG_GRLC_3(prio, enum, txt, arg1, arg2, arg3)                       \
  {                                                                         \
    GRLC_STORE_F3_MSG_3(enum, arg1, arg2, arg3, INVALID_GAS_ID);            \
    MSG_GERAN_##prio##_3(txt, arg1, arg2, arg3);                            \
  }                                                                         
                                                                            
#define MSG_GRLC_4(prio, enum, txt, arg1, arg2, arg3, arg4)                 \
  {                                                                         \
    GRLC_STORE_F3_MSG_4(enum, arg1, arg2, arg3, arg4, INVALID_GAS_ID);      \
    MSG_GERAN_##prio##_4(txt, arg1, arg2, arg3, arg4);                      \
  }

#define MSG_GRLC_5(prio, enum, txt, arg1, arg2, arg3, arg4, arg5)           \
  {                                                                         \
    GRLC_STORE_F3_MSG_5(enum, arg1, arg2, arg3, arg4, arg5, INVALID_GAS_ID);\
    MSG_GERAN_##prio##_5(txt, arg1, arg2, arg3, arg4, arg5);                \
  }


#define MSG_GRLC_HIGH_0(enum, txt)                                     MSG_GRLC_0(HIGH, enum, txt)
#define MSG_GRLC_HIGH_1(enum, txt, arg1)                               MSG_GRLC_1(HIGH, enum, txt, arg1)
#define MSG_GRLC_HIGH_2(enum, txt, arg1, arg2)                         MSG_GRLC_2(HIGH, enum, txt, arg1, arg2)
#define MSG_GRLC_HIGH_3(enum, txt, arg1, arg2, arg3)                   MSG_GRLC_3(HIGH, enum, txt, arg1, arg2, arg3)
#define MSG_GRLC_HIGH_4(enum, txt, arg1, arg2, arg3, arg4)             MSG_GRLC_4(HIGH, enum, txt, arg1, arg2, arg3, arg4)
#define MSG_GRLC_HIGH_5(enum, txt, arg1, arg2, arg3, arg4, arg5)       MSG_GRLC_5(HIGH, enum, txt, arg1, arg2, arg3, arg4, arg5)

#define MSG_GRLC_HIGH_0_G(enum, txt)                                   MSG_GRLC_0_G(HIGH, enum, txt)
#define MSG_GRLC_HIGH_1_G(enum, txt, arg1)                             MSG_GRLC_1_G(HIGH, enum, txt, arg1)
#define MSG_GRLC_HIGH_2_G(enum, txt, arg1, arg2)                       MSG_GRLC_2_G(HIGH, enum, txt, arg1, arg2)
#define MSG_GRLC_HIGH_3_G(enum, txt, arg1, arg2, arg3)                 MSG_GRLC_3_G(HIGH, enum, txt, arg1, arg2, arg3)
#define MSG_GRLC_HIGH_4_G(enum, txt, arg1, arg2, arg3, arg4)           MSG_GRLC_4_G(HIGH, enum, txt, arg1, arg2, arg3, arg4)
#define MSG_GRLC_HIGH_5_G(enum, txt, arg1, arg2, arg3, arg4, arg5)     MSG_GRLC_5_G(HIGH, enum, txt, arg1, arg2, arg3, arg4, arg5)

#define MSG_GRLC_MED_0_G(enum, txt)                                    MSG_GERAN_MED_0_G(txt)
#define MSG_GRLC_MED_1_G(enum, txt, arg1)                              MSG_GERAN_MED_1_G(txt, arg1)
#define MSG_GRLC_MED_2_G(enum, txt, arg1, arg2)                        MSG_GERAN_MED_2_G(txt, arg1, arg2)
#define MSG_GRLC_MED_3_G(enum, txt, arg1, arg2, arg3)                  MSG_GERAN_MED_3_G(txt, arg1, arg2, arg3)
#define MSG_GRLC_MED_4_G(enum, txt, arg1, arg2, arg3, arg4)            MSG_GERAN_MED_4_G(txt, arg1, arg2, arg3, arg4)
#define MSG_GRLC_MED_5_G(enum, txt, arg1, arg2, arg3, arg4, arg5)      MSG_GERAN_MED_5_G(txt, arg1, arg2, arg3, arg4, arg5)

#define MSG_GRLC_MED_0(enum, txt)                                      MSG_GERAN_MED_0(txt)
#define MSG_GRLC_MED_1(enum, txt, arg1)                                MSG_GERAN_MED_1(txt, arg1)
#define MSG_GRLC_MED_2(enum, txt, arg1, arg2)                          MSG_GERAN_MED_2(txt, arg1, arg2)
#define MSG_GRLC_MED_3(enum, txt, arg1, arg2, arg3)                    MSG_GERAN_MED_3(txt, arg1, arg2, arg3)
#define MSG_GRLC_MED_4(enum, txt, arg1, arg2, arg3, arg4)              MSG_GERAN_MED_4(txt, arg1, arg2, arg3, arg4)
#define MSG_GRLC_MED_5(enum, txt, arg1, arg2, arg3, arg4, arg5)        MSG_GERAN_MED_5(txt, arg1, arg2, arg3, arg4, arg5)

#define MSG_GRLC_LOW_0_G(enum, txt)                                    MSG_GERAN_LOW_0_G(txt)
#define MSG_GRLC_LOW_1_G(enum, txt, arg1)                              MSG_GERAN_LOW_1_G(txt, arg1)
#define MSG_GRLC_LOW_2_G(enum, txt, arg1, arg2)                        MSG_GERAN_LOW_2_G(txt, arg1, arg2)
#define MSG_GRLC_LOW_3_G(enum, txt, arg1, arg2, arg3)                  MSG_GERAN_LOW_3_G(txt, arg1, arg2, arg3)
#define MSG_GRLC_LOW_4_G(enum, txt, arg1, arg2, arg3, arg4)            MSG_GERAN_LOW_4_G(txt, arg1, arg2, arg3, arg4)
#define MSG_GRLC_LOW_5_G(enum, txt, arg1, arg2, arg3, arg4, arg5)      MSG_GERAN_LOW_5_G(txt, arg1, arg2, arg3, arg4, arg5)

#define MSG_GRLC_LOW_0(enum, txt)                                      MSG_GERAN_LOW_0(txt)
#define MSG_GRLC_LOW_1(enum, txt, arg1)                                MSG_GERAN_LOW_1(txt, arg1)
#define MSG_GRLC_LOW_2(enum, txt, arg1, arg2)                          MSG_GERAN_LOW_2(txt, arg1, arg2)
#define MSG_GRLC_LOW_3(enum, txt, arg1, arg2, arg3)                    MSG_GERAN_LOW_3(txt, arg1, arg2, arg3)
#define MSG_GRLC_LOW_4(enum, txt, arg1, arg2, arg3, arg4)              MSG_GERAN_LOW_4(txt, arg1, arg2, arg3, arg4)
#define MSG_GRLC_LOW_5(enum, txt, arg1, arg2, arg3, arg4, arg5)        MSG_GERAN_LOW_5(txt, arg1, arg2, arg3, arg4, arg5)

#define MSG_GRLC_ERROR_0(enum, txt)                                    MSG_GRLC_0(ERROR, enum, txt)
#define MSG_GRLC_ERROR_1(enum, txt, arg1)                              MSG_GRLC_1(ERROR, enum, txt, arg1)
#define MSG_GRLC_ERROR_2(enum, txt, arg1, arg2)                        MSG_GRLC_2(ERROR, enum, txt, arg1, arg2)
#define MSG_GRLC_ERROR_3(enum, txt, arg1, arg2, arg3)                  MSG_GRLC_3(ERROR, enum, txt, arg1, arg2, arg3)
#define MSG_GRLC_ERROR_4(enum, txt, arg1, arg2, arg3, arg4)            MSG_GRLC_4(ERROR, enum, txt, arg1, arg2, arg3, arg4)
#define MSG_GRLC_ERROR_5(enum, txt, arg1, arg2, arg3, arg4, arg5)      MSG_GRLC_5(ERROR, enum, txt, arg1, arg2, arg3, arg4, arg5)

#define MSG_GRLC_ERROR_0_G(enum, txt)                                  MSG_GRLC_0_G(ERROR, enum, txt)
#define MSG_GRLC_ERROR_1_G(enum, txt, arg1)                            MSG_GRLC_1_G(ERROR, enum, txt, arg1)
#define MSG_GRLC_ERROR_2_G(enum, txt, arg1, arg2)                      MSG_GRLC_2_G(ERROR, enum, txt, arg1, arg2)
#define MSG_GRLC_ERROR_3_G(enum, txt, arg1, arg2, arg3)                MSG_GRLC_3_G(ERROR, enum, txt, arg1, arg2, arg3)
#define MSG_GRLC_ERROR_4_G(enum, txt, arg1, arg2, arg3, arg4)          MSG_GRLC_4_G(ERROR, enum, txt, arg1, arg2, arg3, arg4)
#define MSG_GRLC_ERROR_5_G(enum, txt, arg1, arg2, arg3, arg4, arg5)    MSG_GRLC_5_G(ERROR, enum, txt, arg1, arg2, arg3, arg4, arg5)

#define MSG_GRLC_EXT_HIGH_0_G(enum, txt)                               GRLC_STORE_F3_MSG_0(enum, gas_id)
#define MSG_GRLC_EXT_HIGH_1_G(enum, txt, arg1)                         GRLC_STORE_F3_MSG_1(enum, arg1, gas_id)
#define MSG_GRLC_EXT_HIGH_2_G(enum, txt, arg1, arg2)                   GRLC_STORE_F3_MSG_2(enum, arg1, arg2, gas_id)
#define MSG_GRLC_EXT_HIGH_3_G(enum, txt, arg1, arg2, arg3)             GRLC_STORE_F3_MSG_3(enum, arg1, arg2, arg3, gas_id)
#define MSG_GRLC_EXT_HIGH_4_G(enum, txt, arg1, arg2, arg3, arg4)       GRLC_STORE_F3_MSG_4(enum, arg1, arg2, arg3, arg4, gas_id)
#define MSG_GRLC_EXT_HIGH_5_G(enum, txt, arg1, arg2, arg3, arg4, arg5) GRLC_STORE_F3_MSG_5(enum, arg1, arg2, arg3, arg4, arg5, gas_id)

#define MSG_GRLC_EXT_HIGH_0(enum, txt)                                 GRLC_STORE_F3_MSG_0(enum, INVALID_GAS_ID)
#define MSG_GRLC_EXT_HIGH_1(enum, txt, arg1)                           GRLC_STORE_F3_MSG_1(enum, arg1, INVALID_GAS_ID)
#define MSG_GRLC_EXT_HIGH_2(enum, txt, arg1, arg2)                     GRLC_STORE_F3_MSG_2(enum, arg1, arg2, INVALID_GAS_ID)
#define MSG_GRLC_EXT_HIGH_3(enum, txt, arg1, arg2, arg3)               GRLC_STORE_F3_MSG_3(enum, arg1, arg2, arg3, INVALID_GAS_ID)
#define MSG_GRLC_EXT_HIGH_4(enum, txt, arg1, arg2, arg3, arg4)         GRLC_STORE_F3_MSG_4(enum, arg1, arg2, arg3, arg4, INVALID_GAS_ID)
#define MSG_GRLC_EXT_HIGH_5(enum, txt, arg1, arg2, arg3, arg4, arg5)   GRLC_STORE_F3_MSG_5(enum, arg1, arg2, arg3, arg4, arg5, INVALID_GAS_ID)

#define MSG_GRLC_EXT_MED_0_G(enum, txt)                                GRLC_STORE_F3_MSG_0(enum, gas_id)
#define MSG_GRLC_EXT_MED_1_G(enum, txt, arg1)                          GRLC_STORE_F3_MSG_1(enum, arg1, gas_id)
#define MSG_GRLC_EXT_MED_2_G(enum, txt, arg1, arg2)                    GRLC_STORE_F3_MSG_2(enum, arg1, arg2, gas_id)
#define MSG_GRLC_EXT_MED_3_G(enum, txt, arg1, arg2, arg3)              GRLC_STORE_F3_MSG_3(enum, arg1, arg2, arg3, gas_id)
#define MSG_GRLC_EXT_MED_4_G(enum, txt, arg1, arg2, arg3, arg4)        GRLC_STORE_F3_MSG_4(enum, arg1, arg2, arg3, arg4, gas_id)
#define MSG_GRLC_EXT_MED_5_G(enum, txt, arg1, arg2, arg3, arg4, arg5)  GRLC_STORE_F3_MSG_5(enum, arg1, arg2, arg3, arg4, arg5, gas_id)
                                                                       
#define MSG_GRLC_EXT_MED_0(enum, txt)                                  GRLC_STORE_F3_MSG_0(enum, INVALID_GAS_ID)
#define MSG_GRLC_EXT_MED_1(enum, txt, arg1)                            GRLC_STORE_F3_MSG_1(enum, arg1, INVALID_GAS_ID)
#define MSG_GRLC_EXT_MED_2(enum, txt, arg1, arg2)                      GRLC_STORE_F3_MSG_2(enum, arg1, arg2, INVALID_GAS_ID)
#define MSG_GRLC_EXT_MED_3(enum, txt, arg1, arg2, arg3)                GRLC_STORE_F3_MSG_3(enum, arg1, arg2, arg3, INVALID_GAS_ID)
#define MSG_GRLC_EXT_MED_4(enum, txt, arg1, arg2, arg3, arg4)          GRLC_STORE_F3_MSG_4(enum, arg1, arg2, arg3, arg4, INVALID_GAS_ID)
#define MSG_GRLC_EXT_MED_5(enum, txt, arg1, arg2, arg3, arg4, arg5)    GRLC_STORE_F3_MSG_5(enum, arg1, arg2, arg3, arg4, arg5, INVALID_GAS_ID)

#endif /* INC_GRLCMSGENUM_H */
