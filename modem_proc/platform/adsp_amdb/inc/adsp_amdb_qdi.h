#ifndef _ADSP_AMDB_QDI_H_
#define _ADSP_AMDB_QDI_H_


#define ADSP_AMDB_QDI_DRIVER_NAME "ADSP_AMDB_QDI_DRIVER"

#define ADSP_AMDB_ADD_CAPI              ( 1  + QDI_PRIVATE )
#define ADSP_AMDB_ADD_APPI              ( 2  + QDI_PRIVATE )
#define ADSP_AMDB_REMOVE_CAPI           ( 3  + QDI_PRIVATE )
#define ADSP_AMDB_REMOVE_APPI           ( 4  + QDI_PRIVATE )
#define ADSP_AMDB_REMOVE_ALL_CAPI       ( 5  + QDI_PRIVATE )
#define ADSP_AMDB_REMOVE_ALL_APPI       ( 6  + QDI_PRIVATE )
#define ADSP_AMDB_PRINT_ALL_CAPI        ( 7  + QDI_PRIVATE )
#define ADSP_AMDB_PRINT_ALL_APPI        ( 8  + QDI_PRIVATE )
#define ADSP_AMDB_ADD_CAPI_EX           ( 9  + QDI_PRIVATE )
#define ADSP_AMDB_ADD_CAPI_V2           ( 10 + QDI_PRIVATE )
#define ADSP_AMDB_REMOVE_CAPI_V2        ( 11 + QDI_PRIVATE )
#define ADSP_AMDB_REMOVE_CAPI_EX        ( 12 + QDI_PRIVATE )
#define ADSP_AMDB_REMOVE_ALL_CAPI_V2    ( 13 + QDI_PRIVATE )
#define ADSP_AMDB_PRINT_ALL_CAPI_V2     ( 14 + QDI_PRIVATE )

/**********************
 * COmmon structure used for message passing *
 **********************/
struct amdb_api_strings_type{
    const char* filename_str;
    unsigned int filename_str_len;
    const char* getsize_str;
    unsigned int getsize_str_len;
    const char* ctor_str;
    unsigned int ctor_str_len;
};

/**********************
 * Funtion prototypes *
 **********************/
ADSPResult adsp_amdb_add_capi_exp(int id, bool_t preload,
                                  const char* filename_str,
                                  const char* getsize_str,
                                  const char* ctor_str);

ADSPResult adsp_amdb_add_appi_exp(int id, bool_t preload,
                                  const char* filename_str,
                                  const char* getsize_str,
                                  const char* ctor_str);

ADSPResult adsp_amdb_remove_capi_exp(int id, const char* filename_str);

ADSPResult adsp_amdb_remove_appi_exp(int id, const char* filename_str);

ADSPResult adsp_amdb_remove_all_capi_exp(void);
ADSPResult adsp_amdb_remove_all_appi_exp(void);
ADSPResult adsp_amdb_print_all_capi_exp(void);
ADSPResult adsp_amdb_print_all_appi_exp(void);
ADSPResult adsp_amdb_add_capi_ex_exp(int type, int id1, int id2, bool_t preload,
                                     const char* filename_str,
                                     const char* getsize_str,
                                     const char* ctor_str);
ADSPResult adsp_amdb_add_capi_v2_exp(int type, int id1, int id2, bool_t preload,
                                     const char* filename_str,
                                     const char* getsize_str,
                                     const char* ctor_str);
ADSPResult adsp_amdb_remove_capi_v2_exp(int type, int id1, int id2,
                                        const char* filename_str);
ADSPResult adsp_amdb_remove_capi_ex_exp(int type, int id1, int id2,
                                        const char* filename_str);
ADSPResult adsp_amdb_remove_all_capi_v2_exp(void);
ADSPResult adsp_amdb_print_all_capi_v2_exp(void);

#endif /*_ADSP_AMDB_QDI_H_*/
