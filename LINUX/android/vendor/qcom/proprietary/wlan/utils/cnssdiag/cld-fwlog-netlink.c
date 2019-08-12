/*
 * Copyright (c) 2015-2016 Qualcomm Atheros, Inc.
 *
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 */

/*
 * hostapd / VLAN initialization
 * Copyright 2003, Instant802 Networks, Inc.
 * Copyright 2005-2006, Devicescape Software, Inc.
 * Copyright (c) 2009, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

/*
 * Copyright (c) 2013-2014 The Linux Foundation. All rights reserved.
 *
 * Previously licensed under the ISC license by Qualcomm Atheros, Inc.
 *
 *
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all
 * copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * This file was originally distributed by Qualcomm Atheros, Inc.
 * under proprietary terms before Copyright ownership was assigned
 * to the Linux Foundation.
 */



#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <getopt.h>
#include <limits.h>
#include <asm/types.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/capability.h>
#include <sys/prctl.h>
#include <sys/statvfs.h>
#include <dirent.h>
#include <linux/prctl.h>
#include <pwd.h>
#ifdef ANDROID
#include <private/android_filesystem_config.h>
#endif
#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <net/if.h>
#include "event.h"
#include "msg.h"

#include "diag_lsm.h"
#include "diagcmd.h"
#include "diag.h"
#include "cld-diag-parser.h"

#define CNSS_INTF "wlan0"
#define DELAY_IN_S 3
#define FREE_MEMORY_THRESHOLD 100


const char options[] =
"Options:\n\
-f, --logfile(Currently file path is fixed)\n\
-c, --console (prints the logs in the console)\n\
-s, --silent (No print will come when logging)\n\
-q, --qxdm  (prints the logs in the qxdm)\n\
-d, --debug  (more prints in logcat, check logcat \n\
-s ROME_DEBUG, example to use: -q -d or -c -d)\n\
-b --buffer_size ( example to use : -b 64(in KBs)\n\
-m --cnss_diag_config_file_loc ( example to use : -m /data/misc/cnss_diag.conf)\n\
The options can also be given in the abbreviated form --option=x or -o x. \
The options can be given in any order";

struct sockaddr_nl src_addr, dest_addr;
struct nlmsghdr *nlh = NULL;
struct iovec iov;
static int sock_fd = -1;
struct msghdr msg;

int32_t num_wrapped = 0;
FILE *host_log_fp = NULL;
FILE *fw_log_fp = NULL;
int host_file_index = 0, fw_file_index = 0;
const char *progname;
char *bufferHostData;
char *bufferFwData;
char *hostData;
char *fwData;
int32_t optionflag = 0;
int log_path_flag = WRITE_TO_INTERNAL_SDCARD;
int delayed_count = 0;
int avail_space = 0;
int32_t available_host_buffer_memory = DEFAULT_LOG_BUFFER_LIMIT;
int32_t available_fw_buffer_memory = DEFAULT_LOG_BUFFER_LIMIT;
unsigned int configured_buffer_size = 0;
int max_file_size = MAX_FILE_SIZE;
int max_archives = MAX_FILE_INDEX;
int free_mem_threshold = FREE_MEMORY_THRESHOLD;
char host_file_path[MAX_FILENAME_SIZE] = "/sdcard/wlan_logs/host_driver_logs_current.txt";
char fw_file_path[MAX_FILENAME_SIZE] = "/sdcard/wlan_logs/cnss_fw_logs_current.txt";
char host_file_name[MAX_FILENAME_SIZE];
char fw_file_name[MAX_FILENAME_SIZE];
char host_file_prefix[MAX_FILENAME_SIZE];
char fw_file_prefix[MAX_FILENAME_SIZE];
char wlan_log_dir_path[MAX_FILENAME_SIZE];


char *cnssdiag_config_file = "/data/misc/wifi/cnss_diag.conf";
char log_capture_loc[MAX_SIZE] = "/sdcard/wlan_logs/";



boolean isDriverLoaded = FALSE;

char *line_trim(char *);

static void
usage(void)
{
	fprintf(stderr, "Usage:\n%s options\n", progname);
	fprintf(stderr, "%s\n", options);
	exit(-1);
}
/* function to find whether file exists or not */
static  int doesFileExist(const char *filename) {
	struct stat st;
	int result = stat(filename, &st);
	return result == 0;
}

static uint32_t get_le32(const uint8_t *pos)
{
	return pos[0] | (pos[1] << 8) | (pos[2] << 16) | (pos[3] << 24);
}


/* Opens a directory wlan_logs and searches the same for the presence of
 * host and firmware log files. Sets the index of the file which is used
 * to store the logs before the reboot.
 */


void readDir(const char *dirName) {
	struct DIR *fdir;
	struct dirent *dirent;
	int *host_files = NULL, *fw_files = NULL;
	char fwFileName[32], hostFileName[32];
	int i = 0, found = 0;
	host_files = (int *)malloc(sizeof(int) * max_archives);
	if (NULL == host_files)
	{
	    debug_printf("%s: failed to allocate memory to host_files\n", __func__);
	    return;
	}

	fw_files = (int *)malloc(sizeof(int) * max_archives);
	if (NULL == fw_files)
	{
		debug_printf("%s: failed to allocate memory to fw_files\n",
				__func__);
		free(host_files);
		host_files = NULL;
		return;
	}
	memset(host_files, 0, (sizeof(int) * max_archives));
	memset(fw_files, 0, (sizeof(int) * max_archives));
	fdir = opendir(dirName);
	if (NULL == fdir) {
		debug_printf("%s: fdir is NULL\n", __func__);
		free(host_files);
		host_files = NULL;
		free(fw_files);
		fw_files = NULL;
		return;
	}
	chdir(dirName);
	while ((dirent = readdir(fdir)) != NULL)
	{
		found = 0;
		for (i = 0; i < max_archives; i++) {
			snprintf(hostFileName, sizeof(hostFileName),
				"host_driver_logs_" "%03d" ".txt", i);
			snprintf(fwFileName, sizeof(fwFileName),
				"cnss_fw_logs_" "%03d" ".txt", i);

			if ((0 == (strcmp(dirent->d_name, hostFileName))))
			{
				host_files[i] = 1;
				found = 1;
			}
			else if ((0 == (strcmp(dirent->d_name, fwFileName))))
			{
				fw_files[i] = 1;
				found = 1;
			}
			if (found)
				break;
		}
	}
/*
 * value 0 at index 'i' indicates, host log file current.txt will be renamed
 * with the filename at 'i'th index.
  */
	i = 0;
	while (i < max_archives) {
		if (!host_files[i]) {
		host_file_index = i;
		break;
		}
	i++;
	}
/*
 * value 0 at index 'i' indicates, FW log file current.txt will be renamed
 * with the filename at 'i'th index.
 */
	i = 0;
	while (i < max_archives) {
		if (!fw_files[i]){
			fw_file_index = i;
			break;
		}
	i++;
	}
	debug_printf("%s: host_file_index: %d, fw_file_index: %d\n",
		 __func__, host_file_index, fw_file_index);
	free(fw_files);
	free(host_files);
	closedir(fdir);
	fw_files = NULL;
	host_files = NULL;
}

/*
 * rename host/firmware current.txt logfile with the corresponding
 * host/firmware log file with proper index and delete its next
 * index file to identify the last file name used to store the logs
 * after a reboot.
 */


void backup_file(enum fileType file_type)
{
	char newFileName[100];
	char delFileName[100];
	int ret =0;
	switch (file_type)
	{
		case HOST_LOG_FILE:
		{
			snprintf(newFileName, sizeof(newFileName), "%s" "%03d" ".txt",
				host_file_prefix, host_file_index);
			errno = 0;
			ret = rename(host_file_name, newFileName);
			host_log_fp = NULL;
			host_file_index++;
			if (max_archives == host_file_index) {
				host_file_index = 0;
			}
			snprintf(delFileName, sizeof(delFileName), "%s" "%03d"".txt",
			host_file_prefix, host_file_index);
			unlink(delFileName);
			break;
		}
		case FW_LOG_FILE:
		{
			snprintf(newFileName, sizeof(newFileName), "%s" "%03d" ".txt",
				fw_file_prefix, fw_file_index);
			rename(fw_file_name, newFileName);
			fw_log_fp = NULL;
			fw_file_index++;
			if (max_archives == fw_file_index) {
				fw_file_index = 0;
			}
			snprintf(delFileName, sizeof(delFileName), "%s" "%03d"".txt",
			fw_file_prefix, (fw_file_index));
			unlink(delFileName);
			break;
		}
	}
	return;
}

static void cleanup(void)
{
	if (sock_fd)
		close(sock_fd);
	if(host_log_fp)
		fclose(host_log_fp);
	if(fw_log_fp)
		fclose(fw_log_fp);
}

static void stop(int32_t signum)
{
	UNUSED(signum);
	if(optionflag & LOGFILE_FLAG){
		printf("Recording stopped\n");
		cleanup();
	}
	exit(0);
}


void process_cnss_log_file(uint8_t *dbgbuf)
{
	uint16_t length = 0;
	uint32_t dropped = 0;
	uint32_t timestamp = 0;
	uint32_t res =0;
	struct dbglog_slot *slot = (struct dbglog_slot *)dbgbuf;
	if (NULL != fw_log_fp)
		fseek(fw_log_fp, ftell(fw_log_fp), SEEK_SET);
	timestamp = get_le32((uint8_t *)&slot->timestamp);
	length = get_le32((uint8_t *)&slot->length);
	dropped = get_le32((uint8_t *)&slot->dropped);
	if (!((optionflag & SILENT_FLAG) == SILENT_FLAG)) {
		/* don't like this have to fix it */
		printf("Read bytes %ld timestamp=%u length=%u fw dropped=%u\n",
		    (fw_log_fp != NULL )? ftell(fw_log_fp) : 0, timestamp, length, dropped);
	}
	if (NULL != fw_log_fp) {
		if ((res = fwrite(dbgbuf, RECLEN, 1, fw_log_fp)) != 1) {
			perror("fwrite");
			return;
		}
		fflush(fw_log_fp);
	}
}

/*
 * This function trims any leading and trailing white spaces
 */
char *line_trim(char *str)
{
	char *ptr;

	if(*str == '\0') return str;

	/* Find the first non white-space */
	for (ptr = str; i_isspace(*ptr); ptr++);
	if (*ptr == '\0')
	    return str;

	/* This is the new start of the string*/
	str = ptr;

	/* Find the last non white-space and null terminate the string */
	ptr += strlen(ptr) - 1;
	for (; ptr != str && i_isspace(*ptr); ptr--);
	ptr[1] = '\0';

	return str;
}

void read_config_file(void) {

	FILE *fp = NULL;
	char line_string[256];
	char *line;
	char string[100];
	static int path_flag = 0;
	static int size_flag = 0;
	int archive_flag = 0;
	int memory_threshold_flag = 0;

	int log_storage = 0;

	fp = fopen(cnssdiag_config_file, "a+");
	if (NULL != fp) {
		fseek(fp, 0, SEEK_SET);
		while (!feof(fp)) {
			fgets(line_string, sizeof(line_string), fp);
			line = line_string;
			line = line_trim(line);
			if (*line == '#')
				continue;
			else {
				sscanf(line, "%s", string);
				if (strcmp(string, "LOG_PATH_FLAG") == 0) {
					sscanf((line + strlen("LOG_PATH_FLAG")
						+ FLAG_VALUE_OFFSET),
							"%s", string);
					log_path_flag = atoi(string);
					path_flag = 1;
					debug_printf("file_path=%d\n", log_path_flag);
				}
				else if (strcmp(string, "MAX_LOG_FILE_SIZE") == 0) {
					sscanf((line +	strlen("MAX_LOG_FILE_SIZE") +
						FLAG_VALUE_OFFSET),
							 "%s", string);
					max_file_size = (atoi(string) * (1024) * (1024));
					size_flag = 1;
					debug_printf("max_file_size=%d\n", max_file_size);
				}
				else if (strcmp(string, "MAX_ARCHIVES") == 0) {
					sscanf((line +	strlen("MAX_ARCHIVES") +
						FLAG_VALUE_OFFSET),
							 "%s", string);
					max_archives = atoi(string);
					if (max_archives >= 50)
						max_archives = 50;
					archive_flag = 1;
					debug_printf("max_archives=%d\n", max_archives);
				}
				else if (strcmp(string, "AVAILABLE_MEMORY_THRESHOLD") == 0) {
					sscanf((line +	strlen("AVAILABLE_MEMORY_THRESHOLD") +
						FLAG_VALUE_OFFSET), "%s", string);
					free_mem_threshold = atoi(string);
					memory_threshold_flag = 1;
					debug_printf("free_mem_threshold=%d\n", free_mem_threshold);
				} else if (strcmp(string, "LOG_STORAGE_PATH") == 0) {
					sscanf((line +	strlen("LOG_STORAGE_PATH") +
						FLAG_VALUE_OFFSET), "%s", string);
					if (strlen(string) != 0) {
						strlcpy(log_capture_loc, string, sizeof(log_capture_loc));
						strlcpy(host_file_path, log_capture_loc, sizeof(host_file_path));
						strlcat(host_file_path, "host_driver_logs_current.txt", sizeof(host_file_path));
						strlcpy(fw_file_path, log_capture_loc, sizeof(fw_file_path));
						strlcat(fw_file_path, "cnss_fw_logs_current.txt", sizeof(fw_file_path));
					}
					android_printf("log_capture_location  = %s", log_capture_loc);
					log_storage = 1;

				} else
					continue;
				}
				if ((1 == path_flag) && (1 == size_flag)&& (archive_flag == 1)
						&& (memory_threshold_flag) && log_storage) {
					break;
				}
			}
			if (!path_flag)
				fprintf(fp, "LOG_PATH_FLAG = %d\n", log_path_flag);
			if (!size_flag)
				fprintf(fp, "MAX_LOG_FILE_SIZE = %d\n", MAX_FILE_SIZE /((1024) * (1024)));
			if (!archive_flag)
				fprintf(fp, "MAX_ARCHIVES = %d\n", MAX_FILE_INDEX);
			if (! log_storage)
				fprintf(fp, "LOG_STORAGE_PATH = %s\n", log_capture_loc);
			if (!memory_threshold_flag)
				fprintf(fp, "AVAILABLE_MEMORY_THRESHOLD = %d\n", FREE_MEMORY_THRESHOLD);
	}
	else {
		debug_printf("%s(%s): Configuration file not present "
				"set defualt log file path to internal "
				"sdcard\n", __func__, strerror(errno));
	}
	if (fp)
		fclose(fp);
}



void cnss_open_log_file(int max_size_reached)
{

	if (!max_size_reached) {
		host_file_index = 0;
		fw_file_index = 0;
	}

	if (log_path_flag == WRITE_TO_FILE_DISABLED) {
		optionflag &= ~(LOGFILE_FLAG);
		debug_printf("%s: write to file flag is disabled\n", __func__);
	}
	do {

		if (log_path_flag == WRITE_TO_EXTERNAL_SDCARD) {
			snprintf(wlan_log_dir_path, sizeof(wlan_log_dir_path),"%s",
					"/mnt/media_rw/sdcard1/wlan_logs/");
		}
		else if (log_path_flag == WRITE_TO_INTERNAL_SDCARD) {
			snprintf(wlan_log_dir_path, sizeof(wlan_log_dir_path),"%s",
					log_capture_loc);
		}
		snprintf(fw_file_prefix, sizeof(fw_file_prefix), "%s" "cnss_fw_logs_",
				wlan_log_dir_path);
		snprintf(host_file_prefix, sizeof(host_file_prefix), "%s"
				"host_driver_logs_", wlan_log_dir_path);
		snprintf(host_file_name, sizeof(host_file_name), "%s"
				"host_driver_logs_current.txt", wlan_log_dir_path);
		snprintf(fw_file_name, sizeof(fw_file_name), "%s"
				"cnss_fw_logs_current.txt", wlan_log_dir_path);
		mkdir(wlan_log_dir_path, 755);
		readDir(wlan_log_dir_path);
		if (NULL == host_log_fp) {
			if (max_size_reached) {
				host_log_fp = fopen(host_file_name, "w");

			}
			else {
				host_log_fp = fopen(host_file_name, "a+");
				if ((host_log_fp != NULL) && (ftell(host_log_fp) >=
							max_file_size)) {
					if ((avail_space  < free_mem_threshold) &&
					(log_path_flag == WRITE_TO_INTERNAL_SDCARD)) {
						android_printf("Device free memory is insufficient");
						break;
					}
					fflush(host_log_fp);
					backup_file(HOST_LOG_FILE);
					fclose(host_log_fp);
					host_log_fp = fopen(host_file_name, "w");
				}
				else {
					debug_printf("failed to open file a+ mode or file"
							" size %ld is less than max_file_size %d\n",
							(host_log_fp != NULL)? ftell(host_log_fp) : 0,
							max_file_size);
				}
			}
			if (NULL == host_log_fp) {
				debug_printf("Failed to open file %s"
						" : %d\n", host_file_name, errno);
			}
		}
		if (NULL == fw_log_fp) {
			if (max_size_reached) {
				fw_log_fp = fopen(fw_file_name, "w");
			}else {
				fw_log_fp = fopen(fw_file_name, "a+");
				if ((fw_log_fp != NULL) && (ftell(fw_log_fp) >=
							max_file_size)) {
					if ((avail_space  < free_mem_threshold) &&
					(log_path_flag == WRITE_TO_INTERNAL_SDCARD)) {
						android_printf("Device free memory is insufficient");
						break;
					}
					fflush(fw_log_fp);
					backup_file(FW_LOG_FILE);
					fclose(fw_log_fp);
					fw_log_fp = fopen(fw_file_name, "w");
				}
				else {
					debug_printf("failed to open file a+ mode or file"
							" size %ld is less than max_file_size %d\n",
							(fw_log_fp != NULL)? ftell(fw_log_fp) : 0,
							max_file_size);
				}
			}
			if (NULL == fw_log_fp) {
				debug_printf("Failed to open file %s"
						" : %d\n", fw_file_name, errno);
			}
		}

		if ((NULL == host_log_fp) || (NULL == fw_log_fp)) {
			if (MAX_RETRY_COUNT != delayed_count) {
				debug_printf("%s: Sleep and poll again for %s "
						" sdcard\n", __func__,
						(log_path_flag == 1) ? "internal" : "external");
				sleep(DELAY_IN_S);
				delayed_count++;
			}
			else {
				delayed_count = 0;
				if (log_path_flag == WRITE_TO_EXTERNAL_SDCARD) {
					log_path_flag = WRITE_TO_INTERNAL_SDCARD;
					debug_printf("%s: External sdcard not mounted try for"
							" internal sdcard ", __func__);
					continue;
				}
				else {
					debug_printf("%s: Internal sdcard not yet mounted"
						" Disable writing logs to a file\n", __func__);
					log_path_flag = WRITE_TO_FILE_DISABLED;
					break;
				}
			}
		} else
			break;
	} while(1);
	return;
}
/*
 * Process FW debug, FW event and FW log messages
 * Read the payload and process accordingly.
 *
 */
void process_cnss_diag_msg(tAniNlHdr *wnl)
{
	uint8_t *dbgbuf;
	uint8_t *eventbuf = ((uint8_t *)NLMSG_DATA(wnl) + sizeof(wnl->radio));
	uint16_t diag_type = 0;
	uint32_t event_id = 0;
	uint16_t length = 0;
	struct dbglog_slot *slot;
	uint32_t dropped = 0;

	dbgbuf = eventbuf;

	diag_type = *(uint16_t *)eventbuf;
	eventbuf += sizeof(uint16_t);

	length = *(uint16_t *)eventbuf;
	eventbuf += sizeof(uint16_t);

	if (wnl->nlh.nlmsg_type == WLAN_NL_MSG_CNSS_HOST_MSG) {
		if ((wnl->wmsg.type == ANI_NL_MSG_LOG_HOST_MSG_TYPE) ||
			(wnl->wmsg.type == ANI_NL_MSG_LOG_MGMT_MSG_TYPE)) {
			if ((optionflag & LOGFILE_FLAG) && (!doesFileExist(host_file_path))
				&& (log_path_flag == WRITE_TO_INTERNAL_SDCARD)&& host_log_fp) {
				if (fclose(host_log_fp) == EOF)
					perror("Failed to close host file ");
				host_file_index = 0;
				host_log_fp = fopen(host_file_path, "w");
				if (host_log_fp == NULL) {
					debug_printf("Failed to create a new file");
				}
			}
			process_cnss_host_message(wnl, optionflag, host_log_fp);
		}
		else if (wnl->wmsg.type == ANI_NL_MSG_LOG_FW_MSG_TYPE) {
			if ((optionflag & LOGFILE_FLAG) && (!doesFileExist(fw_file_path))
				&&(log_path_flag == WRITE_TO_INTERNAL_SDCARD)&& fw_log_fp) {
				if (fclose(fw_log_fp) == EOF)
					perror("Failed to close fw file ");
				fw_file_index = 0;
				fw_log_fp = fopen(fw_file_path, "w");
				if(fw_log_fp == NULL) {
					debug_printf("Failed to create a new file");
				}
			}
			process_pronto_firmware_logs(wnl, optionflag, fw_log_fp);
		}
	} else if (wnl->nlh.nlmsg_type == WLAN_NL_MSG_CNSS_HOST_EVENT_LOG &&
		   (wnl->wmsg.type == ANI_NL_MSG_LOG_HOST_EVENT_LOG_TYPE)) {
		process_cnss_host_diag_events_log(
		    (char *)((char *)&wnl->wmsg.length +
			      sizeof(wnl->wmsg.length)),
		    optionflag);
	} else {
		if (diag_type == DIAG_TYPE_FW_EVENT) {
			eventbuf += sizeof(uint32_t);
			event_id = *(uint32_t *)eventbuf;
			eventbuf += sizeof(uint32_t);
			if (optionflag & QXDM_FLAG) {
				if (length)
					event_report_payload(event_id, length,
							     eventbuf);
				else
					event_report(event_id);
			}
		} else if (diag_type == DIAG_TYPE_FW_LOG) {
			/* Do nothing for now */
		} else if (diag_type == DIAG_TYPE_FW_DEBUG_MSG) {
			slot =(struct dbglog_slot *)dbgbuf;
			length = get_le32((uint8_t *)&slot->length);
			dropped = get_le32((uint8_t *)&slot->dropped);
			dbglog_parse_debug_logs(&slot->payload[0],
				    length, dropped, wnl->radio);
		} else if (diag_type == DIAG_TYPE_FW_MSG) {
			uint32_t version = 0;
			slot = (struct dbglog_slot *)dbgbuf;
			length = get_32((uint8_t *)&slot->length);
			version = get_le32((uint8_t *)&slot->dropped);
			if ((optionflag & LOGFILE_FLAG) && (!doesFileExist(fw_file_path))
				&&(log_path_flag == WRITE_TO_INTERNAL_SDCARD)&& fw_log_fp) {
				if (fclose(fw_log_fp) == EOF)
					perror("Failed to close fw file ");
				fw_file_index = 0;
				fw_log_fp = fopen(fw_file_path, "w");
				if(fw_log_fp == NULL) {
					debug_printf("Failed to create a new file");
				}
			}
			process_diagfw_msg(&slot->payload[0], length,
			    optionflag, fw_log_fp,
			    version, sock_fd, wnl->radio);
		} else if (diag_type == DIAG_TYPE_HOST_MSG) {
			slot = (struct dbglog_slot *)dbgbuf;
			length = get_32((uint8_t *)&slot->length);
			process_diaghost_msg(slot->payload, length);
		} else {
			/* Do nothing for now */
		}
	}
}

/*
 * Open the socket and bind the socket with src
 * address. Return the socket fd if sucess.
 *
 */
static int32_t create_nl_socket()
{
	int32_t ret;
	int32_t sock_fd;

	sock_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_USERSOCK);
	if (sock_fd < 0) {
		fprintf(stderr, "Socket creation failed sock_fd 0x%x \n",
		        sock_fd);
		return -1;
	}

	memset(&src_addr, 0, sizeof(src_addr));
	src_addr.nl_family = AF_NETLINK;
	src_addr.nl_groups = 0x01;
	src_addr.nl_pid = getpid(); /* self pid */

	ret = bind(sock_fd, (struct sockaddr *)&src_addr, sizeof(src_addr));
	if (ret < 0)
		{
		close(sock_fd);
		return ret;
	}
	return sock_fd;
}

static int initialize()
{
	char *mesg = "Hello";

	memset(&dest_addr, 0, sizeof(dest_addr));
	dest_addr.nl_family = AF_NETLINK;
	dest_addr.nl_pid = 0; /* For Linux Kernel */
	dest_addr.nl_groups = 0; /* unicast */

	if (nlh) {
		free(nlh);
		nlh = NULL;
	}
	nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_MSG_SIZE));
	if (nlh == NULL) {
		android_printf("%s Cannot allocate memory for nlh",
			__func__);
		return -1;
	}
	memset(nlh, 0, NLMSG_SPACE(MAX_MSG_SIZE));
	nlh->nlmsg_len = NLMSG_SPACE(MAX_MSG_SIZE);
	nlh->nlmsg_pid = getpid();
	nlh->nlmsg_type = WLAN_NL_MSG_CNSS_DIAG;
	nlh->nlmsg_flags = NLM_F_REQUEST;

	memcpy(NLMSG_DATA(nlh), mesg, strlen(mesg));

	iov.iov_base = (void *)nlh;
	iov.iov_len = nlh->nlmsg_len;
	msg.msg_name = (void *)&dest_addr;
	msg.msg_namelen = sizeof(dest_addr);
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;

	return 1;
}

static int getAvailableSpace(const char* path) {
	struct statvfs stat;
	if (statvfs(path, &stat) != 0) {
		return -1;
	}
	/* the available size is f_bsize * f_bavail , return in MBs */
	return ((stat.f_bsize * stat.f_bavail) / (1024 * 1024));
}


int32_t main(int32_t argc, char *argv[])
{
	int res =0;
	int32_t c;
	boolean fetch_free_mem = TRUE;

	progname = argv[0];
	int32_t option_index = 0;
	static struct option long_options[] = {
		{"logfile", 0, NULL, 'f'},
		{"console", 0, NULL, 'c'},
		{"qxdm", 0, NULL, 'q'},
		{"silent", 0, NULL, 's'},
		{"debug", 0, NULL, 'd'},
		{"buffer_size",required_argument, NULL, 'b'},
		{"config_file",required_argument, NULL, 'm'},
		{ 0, 0, 0, 0}
	};


	while (1) {
		c = getopt_long (argc, argv, "fscqdb:m:", long_options,
				 &option_index);
		if (c == -1) break;

		switch (c) {
		case 'f':
			optionflag |= LOGFILE_FLAG;
			break;
		case 'b':
			optionflag |= BUFFER_SIZE_FLAG;
			if (optarg != NULL) {
				configured_buffer_size = atoi(optarg) * 1024;
				available_host_buffer_memory = configured_buffer_size;
				available_fw_buffer_memory = configured_buffer_size;
			}
			break;

		case 'c':
			optionflag |= CONSOLE_FLAG;
			break;

		case 'q':
			optionflag |= QXDM_FLAG;
			break;

		case 's':
			optionflag |= SILENT_FLAG;
			break;

		case 'd':
			optionflag |= DEBUG_FLAG;
			break;
		case 'm':
			if (optarg != NULL)
				cnssdiag_config_file = optarg;
			break;

		default:
			usage();
		}
	}

	if (!(optionflag & (LOGFILE_FLAG | CONSOLE_FLAG |
			    QXDM_FLAG | SILENT_FLAG | DEBUG_FLAG | BUFFER_SIZE_FLAG))) {
		usage();
		return -1;
	}

	if (optionflag & QXDM_FLAG) {
		/* Intialize the fd required for diag APIs */
		if (TRUE != Diag_LSM_Init(NULL))
			{
			perror("Failed on Diag_LSM_Init\n");
			return -1;
		}
		/* Register CALLABACK for QXDM input data */
		DIAGPKT_DISPATCH_TABLE_REGISTER(DIAG_SUBSYS_WLAN,
		    cnss_wlan_tbl);
	}

	sock_fd = create_nl_socket();
	if (sock_fd < 0) {
		fprintf(stderr, "Socket creation failed sock_fd 0x%x \n",
			sock_fd);
		return -1;
	}
	if (initialize() < 0)
		return -1;
	read_config_file();
	if (optionflag & LOGFILE_FLAG) {
		if (!(optionflag & BUFFER_SIZE_FLAG)) {
			configured_buffer_size = DEFAULT_LOG_BUFFER_LIMIT;
		}
		bufferHostData = (char*) malloc( configured_buffer_size * sizeof(char));
		if (bufferHostData == NULL) {
			android_printf("malloc failed for Host case");
			return -1;
		}
		memset(bufferHostData, 0x00, (configured_buffer_size * sizeof(char)));
		hostData = bufferHostData;
		bufferFwData = (char*) malloc(configured_buffer_size * sizeof(char));
		if (bufferFwData == NULL) {
			android_printf("malloc failed for FW logs");
			return -1;
		}
		memset(bufferFwData, 0x00, (configured_buffer_size * sizeof(char)));
		fwData = bufferFwData;
		cnss_open_log_file(FALSE);
	}
	signal(SIGINT, stop);
	signal(SIGTERM, stop);

	parser_init();

	while ( 1 )  {
		if ((res = recvmsg(sock_fd, &msg, 0)) < 0)
			continue;
		if ((res >= (int)sizeof(struct dbglog_slot)) ||
		    (nlh->nlmsg_type == WLAN_NL_MSG_CNSS_HOST_EVENT_LOG)) {
			if (fetch_free_mem && (optionflag & LOGFILE_FLAG)) {
				avail_space = getAvailableSpace("/sdcard");
				if (avail_space != -1)
					fetch_free_mem = FALSE;
			}
			process_cnss_diag_msg((tAniNlHdr *)nlh);
			memset(nlh,0,NLMSG_SPACE(MAX_MSG_SIZE));
		} else {
			/* Ignore other messages that might be broadcast */
			continue;
		}
	}
	/* Release the handle to Diag*/
	Diag_LSM_DeInit();
	if (optionflag & LOGFILE_FLAG)
		cleanup();
	close(sock_fd);
	free(nlh);
	return 0;
}

