#ifndef HSU_DPS_DEFS_H
#define HSU_DPS_DEFS_H
/*=============================================================================

            hsu_dps_defs   ---   H E A D E R   F I L E

=============================================================================*/
/**
@file hsu_dps_defs.h

DPS common definitions for high-speed USB.

Structures defined by this file are:
- hsu_dpof_params_t
- hsu_dpof_params_t
- hsu_dps_version_t
- hsu_dps_req_conf_t
- hsu_dps_res_conf_t
- hsu_dps_crop_area_t
- hsu_dps_job_info_t
- hsu_dps_job_config_t
- hsu_dps_device_status_t
- hsu_dps_capability_t
- hsu_dps_cap_array_t
*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/hsu_dps_defs.h#1 $

when     who  what, where, why
-------- ---  ---------------------------------------------------------
06/25/10  lo  (Tech Pubs) Edited/added Doxygen comments and markup.
09/11/07  me  Creation

=============================================================================*/

/*=============================================================================
Copyright (c) 2007-2010 by Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
=============================================================================*/

/*=============================================================================

                PUBLIC DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/

/** @ingroup hsusb_qc_api
    @{
*/

/*=============================================================================
  Constants and Macros
=============================================================================*/
#define HSU_DPS_FILENAME_SIZE   24
#define HSU_DPS_DATE_SIZE       24
#define HSU_DPS_FILEPATH_SIZE   64
#define HSU_MAX_DCF_PATH_SIZE   255
#define HSU_DPS_MAX_VERSIONS    10
#define HSU_DPS_MAX_STRING_SIZE 64
#define HSU_MAX_CAPABILITIES    100

/*=============================================================================
Typedefs
=============================================================================*/

/** Digital Print Order Format (DPOF) parameters.
*/
typedef struct
{
  uint16 prt_pid;
    /**< Print job ID -- The job number defined in the DPOF file. */
  char   file_path[HSU_MAX_DCF_PATH_SIZE+1];
    /**< Path to the currently printing image data file, which is described in
         the DPOF file */
  uint16 copy_id;
    /**< Copy ID -- A decimal page number for the DPOF print job. */
} hsu_dpof_params_t;

/** Print job status. */
typedef struct
{
  int               job_is_dpof;
    /**< Indicates whether this is a DPOF job. */
  hsu_dpof_params_t dpof_params;
    /**< Digital Print Order Format parameters. */
  uint16            pages_done;
    /**< The number of pages already printed for this job. */
  uint16            total_pages;
    /**< The total number of pages to be printed for this job. */
  uint16            images_printed;
    /**< The number of images in the current job that have been printed. */
} hsu_dps_job_status_t;

/** DPS version structure (maj.min format).
*/
typedef struct
{
  uint16 major;      /**< Major DPS version number. */
  uint16 minor;      /**< Minor DPS version number. */
} hsu_dps_version_t; 

/** Print service configuration request.
*/
typedef struct
{
  int               versions_count;
    /**< The number of version information structures in the dps_version
         array. */
  hsu_dps_version_t dps_version[HSU_DPS_MAX_VERSIONS];
    /**< Array of DPS version structures. */
  char              vendor_name[HSU_DPS_MAX_STRING_SIZE+1];
    /**< The device's manufacturer name. */
  hsu_dps_version_t vendor_specific_version;
    /**< The vendor-specific profile version supported by the device. */
  char              product_name[HSU_DPS_MAX_STRING_SIZE+1];
    /**< The device's product name. */
  char              serial_no[HSU_DPS_MAX_STRING_SIZE+1];
    /**< The device's serial number (provided as an ASCII string). */
} hsu_dps_req_conf_t;

/** Print service configuration response.
*/
typedef struct
{
  uint32             service_available;
    /**< Indicates whether the print server is available for processing the
         print client's job. */
  hsu_dps_req_conf_t conf;
    /**< Print service configuration request structure. */
} hsu_dps_res_conf_t;      

/** Printer image cropping area definition.
*/
typedef struct
{
  uint16  left;   /**< Left border of the crop area. */
  uint16  top;    /**< Top border of the crop area. */
  uint16  width;  /**< Width of the crop area. */
  uint16  height; /**< Height of the crop area. */
} hsu_dps_crop_area_t;

/** Print job information.
*/
typedef struct
{
  hsu_dps_crop_area_t cropping_area;
    /**< Image cropping area. */
  uint32              file_id;
    /**< DPS file ID for the image to be printed. */
  char                file_name[HSU_DPS_FILENAME_SIZE+1];
    /**< File name to be printed. */
  char                date[HSU_DPS_DATE_SIZE+1];
    /**< Date to be printed, indicated as an arbitrary string. */
  uint16              copies;
    /**< Number of copies of the image corresponding to the file_id to be
         printed. */
  hsu_dpof_params_t   dpof_restart;
    /**< Determines whether to automatically restart print jobs. */
  char                file_path[HSU_DPS_FILEPATH_SIZE+1];
    /**< Path to the printing image data file. */
} hsu_dps_job_info_t;

/** Print job configuration.
*/
typedef struct
{
  uint32 quality;          /**< Print quality. */
  uint32 paper_type;       /**< Paper type. */
  uint32 paper_size;       /**< Paper size. */
  uint32 file_type;        /**< File type. */
  uint32 date_print;       /**< Determines whether to print dates provided for
                                print jobs. */
  uint32 filename_print;   /**< Determines whether to print file names given
                                to print jobs. */
  uint32 image_optimize;   /**< Image optimizations. */
  uint32 layout;           /**< Print layout. */
  uint32 fixed_size;       /**< Fixed paper size. */
  uint32 cropping;         /**< Determines whether to specify the cropping area
                                to be printed. */
} hsu_dps_job_config_t;


/** Printer device status.
*/
typedef struct
{
  uint32 print_service_status;
    /**< Current print service status. */
  uint32 job_end_reason;
    /**< Indicates whether a job has been terminated and if so, whether it
         terminated normally or abnormally. */
  uint32 error_status;
    /**< Current printer status. */
  uint32 error_reason;
    /**< Reason for the current printer status in the case of an error. */
  uint32 disconnect_enable;
    /**< Indicates whether the digital still camera can disconnect. */
  uint32 capability_changed;
    /**< Indicates whether the printer capabilities have changed since the last
         DPS_GetCapability operation. */
  uint32 new_job_ok;
    /**< Indicates whether the printer can accept a new DPS_StartJob
         request. */
} hsu_dps_device_status_t;

/** Enumeration of printer capability types to be used in a DPS_GetCapability
    request.
*/
typedef enum
{
  HSU_DPS_CAP_QUALITIES,
    /**< -- Requests a list of the quality levels supported by the printer. */
  HSU_DPS_CAP_PAPER_SIZES,
    /**< -- Requests a list of the paper size values supported by the printer.
    */
  HSU_DPS_CAP_PAPER_TYPES,
    /**< -- Requests a list of the paper type values supported by the printer.
    */
  HSU_DPS_CAP_FILE_TYPES,
    /**< -- Requests a list of file type values supported by the printer. */
  HSU_DPS_CAP_DATE_PRINTS,
    /**< -- Requests information as to whether the printer supports printing
            dates adjacent to images. */
  HSU_DPS_CAP_FILENAME_PRINTS,
    /**< -- Requests information as to whether the printer supports printing
            file names adjacent to images. */
  HSU_DPS_CAP_IMAGE_OPTIMIZE,
    /**< -- Requests information as to whether the printer can enhance or
            optimize images. */
  HSU_DPS_CAP_LAYOUTS,
    /**< -- Requests a list of page layouts supported by the printer. */
  HSU_DPS_CAP_FIXED_SIZES,
    /**< -- Requests a list of values supported by the printer for fixed image
            size output. */
  HSU_DPS_CAP_CROPPINGS
    /**< -- Requests an indication as to whether the printer supports image
            cropping. */
} hsu_dps_capability_t;

/** Printer capabilities array.
*/
typedef struct
{
  int32                count;
    /**< The number of printer capability elements in the capability array. */
  hsu_dps_capability_t cap_type;
    /**< Capability type. */
  uint32               capability[HSU_MAX_CAPABILITIES];
    /**< Variable size printer capabilities array */
} hsu_dps_cap_array_t;

/** @} */ /* endgroup hsusb_qc_api */

#endif /* HSU_DPS_DEFS_H */
