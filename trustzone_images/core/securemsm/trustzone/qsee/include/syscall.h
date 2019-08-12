#define SYSCALL_export_init_info    0x0
#define SYSCALL_qsee_event_wait     0x1
#define SYSCALL_qsee_wait_hlos      0x2
#define SYSCALL_qsee_request_service  0x3
#define SYSCALL_qsee_reg_sb              0x4
#define SYSCALL_qsee_dereg_sb          0x5
#define SYSCALL_qsee_prep_sb_for_ns_read  0x6
#define SYSCALL_qsee_prep_sb_for_s_read  0x7
#define SYSCALL_qsee_read_register       0x8
#define SYSCALL_qsee_set_global_flag       0x9
#define SYSCALL_qsee_get_global_flag       0xA
#define SYSCALL_qsee_oem_process_cmd       0xB
#define SYSCALL_qsee_app_fatal_error       0xC

/*TZBSP shim syscalls*/
#define SYSCALL_qsee_get_uptime     0xFF
#define SYSCALL_qsee_wdog_stop      0xFE
#define SYSCALL_qsee_wdog_pet       0xFD
#define SYSCALL_qsee_dcache_inval_rgn  0xFC
#define SYSCALL_qsee_dcache_flush_rgn  0xFB
#define SYSCALL_qsee_dcache_clean_rgn  0xFA
#define SYSCALL_qsee_printf  0xF9
#define SYSCALL_qsee_protect_memory 0xF8
#define SYSCALL_qsee_is_sw_fuse_blown 0xF7
#define SYSCALL_qsee_blow_sw_fuse 0xF6
#define SYSCALL_qsee_read_jtag_id 0xF5
#define SYSCALL_qsee_read_serial_num 0xF4
#define SYSCALL_qsee_fuse_write 0xF3
#define SYSCALL_qsee_fuse_read 0xF2
#define SYSCALL_qsee_is_ns_range 0xF0
#define SYSCALL_qsee_is_s_tag_area 0xEF
#define SYSCALL_qsee_hash                0xEE
#define SYSCALL_qsee_hash_init           0xED
#define SYSCALL_qsee_hash_update         0xEC
#define SYSCALL_qsee_hash_final          0xEB
#define SYSCALL_qsee_hash_free_ctx       0xEA
#define SYSCALL_qsee_cipher_init         0xE9
#define SYSCALL_qsee_cipher_free_ctx     0xE8
#define SYSCALL_qsee_cipher_set_param    0xE7
#define SYSCALL_qsee_cipher_get_param    0xE6
#define SYSCALL_qsee_cipher_encrypt      0xE5
#define SYSCALL_qsee_cipher_decrypt      0xE4
#define SYSCALL_qsee_hmac                0xE3
#define SYSCALL_qsee_prng_getdata        0xE2
#define SYSCALL_qsee_crypto_lock_engine  0xE1
#define SYSCALL_qsee_secure_message      0xE0
#define SYSCALL_qsee_authenticate_decrypt_message   0xDF
#define SYSCALL_qsee_ssd_decrypt_img                0xDE
#define SYSCALL_qsee_sd_mgr_device_init             0xDD
#define SYSCALL_qsee_sd_mgr_open_partition          0xDC
#define SYSCALL_qsee_sd_mgr_add_partition           0xDB
#define SYSCALL_qsee_sd_mgr_device_get_info         0xDA
#define SYSCALL_qsee_sd_mgr_client_get_info         0xD9
#define SYSCALL_qsee_sd_mgr_read_sectors            0xD8
#define SYSCALL_qsee_sd_mgr_write_sectors           0xD7
#define SYSCALL_qsee_kdf                            0xD6
#define SYSCALL_qsee_i2c_open                       0xD5
#define SYSCALL_qsee_i2c_read                       0xD4
#define SYSCALL_qsee_i2c_write                      0xD3
#define SYSCALL_qsee_i2c_close                      0xD2
#define SYSCALL_qsee_encapsulate_inter_app_message  0xD1
#define SYSCALL_qsee_decapsulate_inter_app_message  0xD0
#define SYSCALL_qsee_cmac                           0xCF
#define SYSCALL_qsee_spi_open                       0xCD
#define SYSCALL_qsee_spi_read                       0xCC
#define SYSCALL_qsee_spi_write                      0xCB
#define SYSCALL_qsee_spi_close                      0xCA
#define SYSCALL_qsee_spi_full_duplex 		    0xC9
#define SYSCALL_qsee_create_counter                 0xC8
#define SYSCALL_qsee_query_counter                  0xC7
#define SYSCALL_qsee_increment_counter              0xC6
#define SYSCALL_qsee_delete_counter                 0xC5
#define SYSCALL_qsee_get_tz_app_name                0xC4
#define SYSCALL_qsee_hdmi_status_read               0xC3
#define SYSCALL_qsee_hashcipher_encrypt             0xC2 
#define SYSCALL_qsee_hashcipher_decrypt             0xC1
#define SYSCALL_qsee_hash_set_param                 0xC0
#define SYSCALL_qsee_query_rpmb_version             0xA0
#define SYSCALL_qsee_counter_set_folder             0xAD
#define SYSCALL_qsee_get_secure_state               0xB5
#define SYSCALL_qsee_macc_gen_ecc_privkey           0xB4


#define SYSCALL_qsee_cipher_reset                   0xAC
#define SYSCALL_qsee_hash_reset                     0xAB
#define SYSCALL_qsee_hmac_init                      0xAA
#define SYSCALL_qsee_hmac_update                    0xA9
#define SYSCALL_qsee_hmac_final                     0xA8
#define SYSCALL_qsee_hmac_free_ctx                  0xA7
#define SYSCALL_qsee_read_hw_version                0xA1

#define SYSCALL_qsee_macc_sign_service_data         0x9E
#define SYSCALL_qsee_macc_authenticate_device       0x9D
#define SYSCALL_qsee_get_tz_app_id                  0x9F
#define SYSCALL_qsee_macc_provision_service_key     0x9C
#define SYSCALL_qsee_set_bandwidth                  0x98

#define SYSCALL_qsee_query_rpmb_enablement          0x97

#define SYSBASE                     0xffffff00
#define SWIBASE                     0x1400

#define SYSNUM(name)                (SYSBASE + SYSCALL_ ## name)
#define SWINUM(name)                (SWIBASE + SYSCALL_ ## name)


