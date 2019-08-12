/*****************************************************************
# Copyright (c) $Date: 2016/12/13 $ Qualcomm Technologies INCORPORATED.
# All Rights Reserved.
# Modified by Qualcomm Technologies INCORPORATED on $Date: 2016/12/13 $
*****************************************************************/
#ifndef PASSFAIL_H

#define PASSFAIL_H 1

#define passone(...) do_passone((char *)__FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)
#define passnow(...) do_passnow((char *)__FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)

#define failone(...) do_failone((char *)__FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)
#define failnow(...) do_failnow((char *)__FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)

#define fail failnow
#define pass passone

#define fail_if(cond) do { if (cond) failnow(#cond); } while (0)
#define pass_if(cond) do { if (cond) passone(#cond); } while (0)

void do_passone(char *fn, char *file, int line, char *fmt, ...);
void do_passnow(char *fn, char *file, int line, char *fmt, ...);
void do_failone(char *fn, char *file, int line, char *fmt, ...);
void do_failnow(char *fn, char *file, int line, char *fmt, ...);

#endif

