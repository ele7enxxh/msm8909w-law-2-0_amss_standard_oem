/*****************************************************************
# Copyright (c) $Date: 2016/12/13 $ Qualcomm Technologies INCORPORATED.
# All Rights Reserved.
# Modified by Qualcomm Technologies INCORPORATED on $Date: 2016/12/13 $
*****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "passfail.h"

#if !defined(__qdsp6__) && !defined(__hexagon__)
#define MAXLINE 512

void do_passone(char *fn, char *file, int line, char *fmt, ...)
{
    int              n;
    char    buf[MAXLINE];
    va_list args;
    va_start(args,fmt);
    snprintf(buf, sizeof(buf), "PASS: %s in %s:%d:",fn,file,line);
    n = strlen(buf);
    vsnprintf(buf+n, sizeof(buf)-n, fmt, args);
    va_end(args);
    fflush(stdout);         /* in case stdout and stderr are the same */
    fputs(buf, stderr);
    fflush(stderr);
}

void do_passnow(char *fn, char *file, int line, char *fmt, ...)
{
    va_list args;
    va_start(args,fmt);
    do_passone(fn, file, line, fmt, args);
    va_end(args);
    exit(0);
}

void do_failone(char *fn, char *file, int line, char *fmt, ...)
{
    int              n;
    char    buf[MAXLINE];
    va_list args;
    va_start(args,fmt);
    snprintf(buf, sizeof(buf), "FAIL: %s in %s:%d:",fn,file,line);
    n = strlen(buf);
    vsnprintf(buf+n, sizeof(buf)-n, fmt, args);
    va_end(args);
    fflush(stdout);         /* in case stdout and stderr are the same */
    fputs(buf, stderr);
    fflush(stderr);
}

void do_failnow(char *fn, char *file, int line, char *fmt, ...)
{
    va_list args;
    va_start(args,fmt);
    do_failone(fn, file, line, fmt, args);
    va_end(args);
    exit(1);
}


#else
void do_passone(char *fn, char *file, int line, char *fmt, ...)
{
        va_list args;
        va_start(args,fmt);
        fprintf(stdout,"PASS: %s in %s:%d:",fn,file,line);
        vfprintf(stdout,fmt,args);
        va_end(args);
        fflush(stdout);
}


void do_passnow(char *fn, char *file, int line, char *fmt, ...)
{
        va_list args;
        va_start(args,fmt);
        fprintf(stdout,"PASS: %s in %s:%d:",fn,file,line);
        vfprintf(stdout,fmt,args);
        va_end(args);
        fflush(stdout);
	exit(0);
}

void do_failone(char *fn, char *file, int line, char *fmt, ...)
{
        va_list args;
        va_start(args,fmt);
        fprintf(stdout,"FAIL: %s in %s:%d:",fn,file,line);
        vfprintf(stdout,fmt,args);
        va_end(args);
        fflush(stdout);
}

void do_failnow(char *fn, char *file, int line, char *fmt, ...)
{
        va_list args;
        va_start(args,fmt);
        fprintf(stdout,"FAIL: %s in %s:%d:",fn,file,line);
        vfprintf(stdout,fmt,args);
        va_end(args);
        fflush(stdout);
	exit(1);
}
#endif
