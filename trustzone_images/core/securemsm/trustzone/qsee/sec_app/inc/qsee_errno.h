#ifndef __QSEE_ERRNO_H
#define __QSEE_ERRNO_H

#define QSEE_RESULT_SUCCESS                                    0
#define QSEE_RESULT_INCOMPLETE                                 1
#define QSEE_RESULT_BLOCKED_ON_LISTNER                         2 /* Thread blocked on HLOS non-error scenario */

                                                               /* (-1) */
#define QSEE_RESULT_FAILURE                                    0xFFFFFFFF
#define QSEE_RESULT_FAIL_SET_RSP_NO_THREAD                     0xFFFFFFFE
#define QSEE_RESULT_FAIL_PROTECT_APP_RGN                       0xFFFFFFFD
#define QSEE_RESULT_FAIL_SYSCALL_NOT_ALLOWED                   0xFFFFFFFC
#define QSEE_RESULT_FAIL_REQUEST_NOT_NS                        0xFFFFFFFB
#define QSEE_RESULT_FAIL_RESPONSE_NOT_NS                       0xFFFFFFFA
#define QSEE_RESULT_FAIL_REQRSP_NOT_ALIGNED                    0xFFFFFFF9
#define QSEE_RESULT_FAIL_PROTECT_REQRSP_RGN                    0xFFFFFFF8
#define QSEE_RESULT_FAIL_VALIDATE_SEG_OUTSIDE_APP_RGN          0xFFFFFFF7
                                                               /* (-10) */
#define QSEE_RESULT_FAIL_VALIDATE_SEG_OVERLAP_APP_RGN          0xFFFFFFF6
#define QSEE_RESULT_FAIL_LOAD_APP_EXISTS                       0xFFFFFFF5
#define QSEE_RESULT_FAIL_PIL_INIT                              0xFFFFFFF4
#define QSEE_RESULT_FAIL_PIL_AUTH_RESET                        0xFFFFFFF3
#define QSEE_RESULT_FAIL_APP_START                             0xFFFFFFF2
#define QSEE_RESULT_FAIL_REG_LISTENER_ALIGNMENT                0xFFFFFFF1
#define QSEE_RESULT_FAIL_REG_LISTENER_FULL                     0xFFFFFFF0
#define QSEE_RESULT_FAIL_PROTECT_REQRSP_RGN_OCCUPIED           0xFFFFFFEF
#define QSEE_RESULT_FAIL_PROTECT_REQRSP_RGN_NO_THREAD_FOR_APP  0xFFFFFFEE
#define QSEE_RESULT_FAIL_SEND_CMD_NO_THREAD                    0xFFFFFFED
                                                               /* (-20) */
#define QSEE_RESULT_FAIL_NO_HASH_SEG                           0xFFFFFFEC
#define QSEE_RESULT_FAIL_MALLOC_FAILED_FOR_PIL_BUF             0xFFFFFFEB
#define QSEE_RESULT_FAIL_WORD_OVERFLOW                         0xFFFFFFEA
#define QSEE_ERROR_PRINT_APP_FAULTED                           0xFFFFFFE9
#define QSEE_ERROR_PRINT_SET_APP_FAULTED                       0xFFFFFFE8
#define QSEE_ERROR_PRINT_DEREG_LISTENER                        0xFFFFFFE7
#define QSEE_ERROR_PRINT_TOO_MANY_SB                           0xFFFFFFE6
#define QSEE_ERROR_PRINT_MAP_RGN_FAIL                          0xFFFFFFE5
#define QSEE_ERROR_PRINT_SHUTDOWN_APP_BLOCKED_ON_LISTENER      0xFFFFFFE4
#define QSEE_ERROR_PRINT_NULL_APP_ID                           0xFFFFFFE3
                                                               /* (-30) */
#define QSEE_ERROR_PRINT_NO_THREAD_FOR_APP                     0xFFFFFFE2
#define QSEE_ERROR_PRINT_PROTECT_REQRSP_FAIL                   0xFFFFFFE1
#define QSEE_RESULT_FAIL_IMAGE_ALLOC                           0xFFFFFFE0
#define QSEE_RESULT_FAIL_BAD_APP_ID                            0xFFFFFFDF
#define QSEE_RESULT_FAIL_APP_LOAD_VERSION                      0xFFFFFFDE
#define QSEE_RESULT_FAIL_THREAD_MALLOC                         0xFFFFFFDD
#define QSEE_RESULT_FAIL_NO_FREE_APP_ID                        0xFFFFFFDC
#define QSEE_RESULT_FAIL_THREAD_CREATE                         0xFFFFFFDB
#define QSEE_RESULT_FAIL_APP_ALREADY_LOADED                    0xFFFFFFDA
#define QSEE_RESULT_FAIL_INVALID_APP_REGION                    0xFFFFFFD9
                                                               /* (-40) */
#define QSEE_RESULT_FAIL_APP_REGION_NOT_NS                     0xFFFFFFD8
#define QSEE_RESULT_FAIL_APP_REGION_NOT_SET                    0xFFFFFFD7
#define QSEE_RESULT_FAIL_APP_REGION_ALREADY_SET                0xFFFFFFD6
#define QSEE_RESULT_FAIL_LOG_BUF_NOT_NS                        0xFFFFFFD5
#define QSEE_RESULT_FAIL_RPMB_KEY_PROVISION                    0xFFFFFFD4
#define QSEE_RESULT_FAIL_PIL_SET_MEM_RANGE                     0xFFFFFFD3
#define QSEE_RESULT_FAIL_BAD_APP_NAME                          0xFFFFFFD2
#define QSEE_RESULT_FAIL_IMG_BUF_NOT_NS                        0xFFFFFFD1
#define QSEE_RESULT_FAIL_PROTECT_SB                            0xFFFFFFD0
#define QSEE_RESULT_FAIL_RPMB_ERASE                            0xFFFFFFCF
                                                               /* (-50) */
#define QSEE_RESULT_FAIL_PREPARE_RETURN_NO_THREAD              0xFFFFFFCE
#define QSEE_RESULT_FAIL_QSEE_REQ_NOT_NS                       0xFFFFFFCD
#define QSEE_RESULT_FAIL_QSEE_RSP_NOT_NS                       0xFFFFFFCC
#define QSEE_RESULT_FAIL_REG_SB_INT_OVERFLOW                   0xFFFFFFCB
#define QSEE_RESULT_FAIL_REG_SB_NOT_CP                         0xFFFFFFCA
#define QSEE_RESULT_FAIL_REG_SB_NOT_DDR                        0xFFFFFFC9
#define QSEE_RESULT_FAIL_PRNG_FAIL_IN_CREATE_THREAD            0xFFFFFFC8
#define QSEE_RESULT_FAIL_ALLOC_VIRTUAL_BITMASK                 0xFFFFFFC7
#define QSEE_RESULT_FAIL_TOO_MANY_SEGS                         0xFFFFFFC6
#define QSEE_RESULT_FAIL_NO_LISTENER                           0xFFFFFFC5
#define QSEE_RESULT_FAIL_WORD_OVERFLOW_APP_REQ                 0xFFFFFFC4
#define QSEE_RESULT_FAIL_WORD_OVERFLOW_APP_RSP                 0xFFFFFFC3
#define QSEE_RESULT_FAIL_WORD_UNDERFLOW_REQRSP                 0xFFFFFFC2
#define QSEE_RESULT_FAIL_REG_SB_ALREADY_MAPPED ((int32)(-64))/*0xFFFFFFC0 */

#define QSEE_RESULT_FAIL_CE_PIPE_INVALID     ((int32)(-63)) /* 0xFFFFFFC1 */
#define QSEE_RESULT_FAIL_KEY_ID_EXISTS       ((int32)(-65)) /* 0xFFFFFFBF */
#define QSEE_RESULT_FAIL_MAX_KEYS            ((int32)(-66)) /* 0xFFFFFFBE */
#define QSEE_RESULT_FAIL_SAVE_KS             ((int32)(-67)) /* 0xFFFFFFBD */
#define QSEE_RESULT_FAIL_LOAD_KS             ((int32)(-68)) /* 0xFFFFFFBC */
#define QSEE_RESULT_FAIL_KS_ALREADY_DONE     ((int32)(-69)) /* 0xFFFFFFBB */
#define QSEE_RESULT_FAIL_KEY_ID_DNE          ((int32)(-70)) /* 0xFFFFFFBA */
#define QSEE_RESULT_FAIL_INCORRECT_PSWD      ((int32)(-71)) /* 0xFFFFFFB9 */
#define QSEE_RESULT_FAIL_MAX_ATTEMPT         ((int32)(-72)) /* 0xFFFFFFB8 */
#define QSEE_RESULT_FAIL_KS_OP               ((int32)(-80)) /*0xFFFFFFB0 */   

#define QSEE_HANDLE_FAIL_IN_CREATE_BAD_ARGS                    0xFFFFFFB7
#define QSEE_HANDLE_FAIL_IN_CREATE_BAD_CTX                     0xFFFFFFB6
#define QSEE_HANDLE_FAIL_IN_CREATE_TABLE_FULL                  0xFFFFFFB5
#define QSEE_HANDLE_FAIL_IN_DESTROY_BAD_LOOKUP                 0xFFFFFFB4
#define QSEE_HANDLE_FAIL_IN_h2p_BAD_LOOKUP                     0xFFFFFFB3
#define QSEE_HANDLE_INIT_TABLE_FAILED_MALLOC                   0xFFFFFFB2

#define QSEE_RESULT_FAIL_APP_REGION_TOO_LARGE                  0xFFFFFFB1

#define QSEE_RESULT_SD_MGR_ERROR                      0xFFFFFF0F /* Generic Failure */
#define QSEE_RESULT_SD_MGR_ERROR_INVALID_PARAM        0xFFFFFF0E /* Invalid arguments passed to the API */
#define QSEE_RESULT_SD_MGR_ERROR_RPMB_UNKNOWN         0xFFFFFF0D /* Unknown error during RPMB access */
#define QSEE_RESULT_SD_MGR_ERROR_RPMB_MAC             0xFFFFFF0C /* MAC autentication error during RPMB access */
#define QSEE_RESULT_SD_MGR_ERROR_RPMB_WRITE_COUNTER   0xFFFFFF0B /* Write counter mismatch during RPMB access */
#define QSEE_RESULT_SD_MGR_ERROR_RPMB_ADDR            0xFFFFFF0A /* RPMB address failure */
#define QSEE_RESULT_SD_MGR_ERROR_RPMB_WRITE           0xFFFFFF09 /* Failure during RPMB Write */
#define QSEE_RESULT_SD_MGR_ERROR_RPMB_READ            0xFFFFFF08 /* Failure during RPMB Read */
#define QSEE_RESULT_SD_MGR_ERROR_RPMB_NOT_PROVISIONED 0xFFFFFF07 /* RPMB Not provisioned */
#define QSEE_RESULT_SD_MGR_ERROR_RPMB_MAC_GENERATION  0xFFFFFF06 /* Failure during MAC generation */
#define QSEE_RESULT_SD_MGR_ERROR_RPMB_RNG_GENERATION  0xFFFFFF05 /* Failure during RNG generation */
#define QSEE_RESULT_SD_MGR_ERROR_RPMB_NONCE_ERROR     0xFFFFFF04 /* Nonce authentication error */
#define QSEE_RESULT_SD_MGR_ERROR_RPMB_PROVISIONED     0xFFFFFF03 /* RPMB already provisioned */

#define QSEE_RESULT_FAIL_INVALID_SYSCALL              0xFFFFFF02
#define QSEE_RESULT_FAIL_RPMB_OWNER_ID_ALREADY_EXISTS 0xFFFFFF01
#define QSEE_RESULT_FAIL_RSP_LEN_TOO_SMALL            0xFFFFFFAE
#define QSEE_RESULT_FAIL_REQ_LEN_TOO_SMALL            0xFFFFFFAD


#define QSEE_RESULT_FAIL_IMG_LEN_TOO_SMALL            0xFFFFFFAC
#define QSEE_RESULT_FAIL_ELF_IMG_NOT_NS               0xFFFFFFAB
#define QSEE_RESULT_FAIL_MDT_LEN_TOO_SMALL            0xFFFFFFAA
#define QSEE_RESULT_FAIL_MDT_LEN_NOT_NS               0xFFFFFFA9
#define QSEE_RESULT_FAIL_ELF_PH_NOT_NS                0xFFFFFFA7
#define QSEE_RESULT_FAIL_ELF_HASH_NOT_NS              0xFFFFFFA6
#define QSEE_RESULT_FAIL_ELF_SEG_NOT_NS               0xFFFFFFA5
#define QSEE_RESULT_FAIL_INT_UNDERFLOW                0xFFFFFFA4

#define QSEE_RESULT_FAIL_QSEE_BAD_REQ_LEN             0xFFFFFFA3

#define QSEE_RESULT_FAIL_REQ_BUF_INVALID                       0xFFFFFF9F
#define QSEE_RESULT_FAIL_REQ_BUF_SIZE_INVALID                  0xFFFFFF9E

#define QSEE_HANDLE_FAIL_IN_CREATE_NULL_HANDLE        0xFFFFFF9D 
#define QSEE_RESULT_FAIL_LISTENER_BUFFER_IN_USE       0xFFFFFF9C
#define QSEE_RESULT_FAIL_ELF_MEM_FILE_SIZE            0xFFFFFF9B




#endif
