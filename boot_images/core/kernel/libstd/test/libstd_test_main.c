/*===============================================================================================
 * FILE:        libstd_test_main.c
 *
 * DESCRIPTION: Implementation of libstd test cases
 *
 *              Copyright (c) 1999 - 2010 Qualcomm Technologies Incorporated.
 *              All Rights Reserved. QUALCOMM Proprietary and Confidential.
 *===============================================================================================*/
 
/*===============================================================================================
 *
 *                            Edit History
 * $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/kernel/libstd/test/libstd_test_main.c#1 $ 
 * $DateTime: 2015/09/01 00:30:35 $
 *
 *===============================================================================================*/
#include <stringl/stringl.h>
#include "libstd_test.h"

static const char libstd_str_null_c[]        = "";
static const wchar_t libstd_str_null_wct[]   = L"";
static const wchar libstd_str_null_wc[]      = {'\0'};
static const char libstd_str_1_9_c[]         = "123456789";
static const wchar_t libstd_str_1_9_wct[]    = L"123456789";
static const wchar libstd_str_1_9_wc[]       = {'1','2','3','4','5','6','7','8','9','\0'};
static const char libstd_str_1_A_c[]         = "123456789A";
static const wchar_t libstd_str_1_A_wct[]    = L"123456789A";
static const wchar libstd_str_1_A_wc[]       = {'1','2','3','4','5','6','7','8','9','A','\0'};
static const char libstd_str_1_F_c[]         = "123456789ABCDEF";
static const wchar_t libstd_str_1_F_wct[]    = L"123456789ABCDEF";
static const wchar libstd_str_1_F_wc[]       = {'1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','\0'};
static const wchar str_1_3_wc[] = {'1','2','3','\0'};
static const wchar str_0_4_wc[] = {'0', '1','2','3','4','\0'};
static const wchar str_4_6_wc[] = {'4','5','6','\0'};
static const wchar str_1_6_wc[] = {'1','2','3','4','5','6','\0'};
static const wchar str_4_0_wc[] = {'4','5','6','7','8','9','0','\0'};
static const wchar std_4_0_4_wc[] = {'4','5','6','7','8','9','0','1','2','3','4','\0'};


void libstd_start_strcasecmp_tests(void);
void libstd_start_strncasecmp_tests(void);
void libstd_start_wstrlen_tests(void);
void libstd_start_strlcpy_tests(void);
void libstd_start_wcslcpy_tests(void);
void libstd_start_wstrlcpy_tests(void);
void libstd_start_strlcat_tests(void);
void libstd_start_wcslcat_tests(void);
void libstd_start_wstrlcat_tests(void);
void libstd_start_std_scanul_tests(void);

static void libstd_perform_tests(void)
{
    libstd_start_strcasecmp_tests();
    libstd_start_strncasecmp_tests();
    libstd_start_wstrlen_tests();
    libstd_start_strlcpy_tests();
    libstd_start_wcslcpy_tests();
    libstd_start_wstrlcpy_tests();
    libstd_start_strlcat_tests();
    libstd_start_wcslcat_tests();
    libstd_start_wstrlcat_tests();
    libstd_start_std_scanul_tests();
}

int libstd_start_tests(void)
{
    int err_code;
    err_code = libstd_test_log_init();
    if (0 == err_code)
    {
        libstd_perform_tests();
        //libstd_test_log_destroy();
    }
    return err_code;
}

void libstd_start_strcasecmp_tests(void)
{
    int tc = 0;
    //LHS is equal to RHS
    LIBSTD_RECORD_TEST(++tc, (0 == strcasecmp("a", "a")));
    LIBSTD_RECORD_TEST(++tc, (0 == strcasecmp("a", "A")));
    LIBSTD_RECORD_TEST(++tc, (0 == strcasecmp("A", "a")));
    LIBSTD_RECORD_TEST(++tc, (0 == strcasecmp("a", "a\0b"))); 
    LIBSTD_RECORD_TEST(++tc, (0 == strcasecmp("a", "A\0b")));

    //LHS is less than RHS
    LIBSTD_RECORD_TEST(++tc, (0 > strcasecmp("a", "b")));
    LIBSTD_RECORD_TEST(++tc, (0 > strcasecmp("A", "b")));
    LIBSTD_RECORD_TEST(++tc, (0 > strcasecmp("a", "aa")));
    LIBSTD_RECORD_TEST(++tc, (0 > strcasecmp("a", "Aa")));

    // LHS is greater than RHS
    LIBSTD_RECORD_TEST(++tc, (0 < strcasecmp("b", "a")));
    LIBSTD_RECORD_TEST(++tc, (0 < strcasecmp("b", "A")));
    LIBSTD_RECORD_TEST(++tc, (0 < strcasecmp("aa", "a")));
    LIBSTD_RECORD_TEST(++tc, (0 < strcasecmp("Aa", "a")));
}

void libstd_start_wstrlen_tests(void)
{
   int tc = 0;
   LIBSTD_RECORD_TEST(++tc, (0 == wstrlen(libstd_str_null_wc)));
   LIBSTD_RECORD_TEST(++tc, (9 == wstrlen(libstd_str_1_9_wc)));
}

void libstd_start_strncasecmp_tests(void)
{
    int tc = 0;
    // LHS is equal to RHS
    LIBSTD_RECORD_TEST(++tc, (0 == strncasecmp("a", "a", 1)));
    LIBSTD_RECORD_TEST(++tc, (0 == strncasecmp("a", "A", 1)));
    LIBSTD_RECORD_TEST(++tc, (0 == strncasecmp("A", "a", 1)));
    LIBSTD_RECORD_TEST(++tc, (0 == strncasecmp("a", "ab", 1)));
    LIBSTD_RECORD_TEST(++tc, (0 == strncasecmp("a", "Ab", 1)));
    LIBSTD_RECORD_TEST(++tc, (0 == strncasecmp("a", "a\0b", 1))); 
    LIBSTD_RECORD_TEST(++tc, (0 == strncasecmp("a", "A\0b", 1)));
    LIBSTD_RECORD_TEST(++tc, (0 == strncasecmp("a\0b", "a\0c", 3))); 
    LIBSTD_RECORD_TEST(++tc, (0 == strncasecmp("a\0b", "A\0c", 3)));

    //LHS is less than RHS
    LIBSTD_RECORD_TEST(++tc, (0 > strncasecmp("a", "b", 1)));
    LIBSTD_RECORD_TEST(++tc, (0 > strncasecmp("A", "b", 1)));
    LIBSTD_RECORD_TEST(++tc, (0 > strncasecmp("a", "aa", 2)));
    LIBSTD_RECORD_TEST(++tc, (0 > strncasecmp("a", "Aa", 2)));

    // LHS is greater than RHS
    LIBSTD_RECORD_TEST(++tc, (0 < strncasecmp("b", "a", 1)));
    LIBSTD_RECORD_TEST(++tc, (0 < strncasecmp("b", "A", 1)));
    LIBSTD_RECORD_TEST(++tc, (0 < strncasecmp("aa", "a", 2)));
    LIBSTD_RECORD_TEST(++tc, (0 < strncasecmp("Aa", "a", 2)));
}


void libstd_start_strlcpy_tests(void)
{
    int tc = 0;
    const char *test_str = NULL;
    char buf[10];
    
    test_str = "1";
    LIBSTD_RECORD_TEST(++tc, (strlen(test_str) == strlcpy(buf, test_str, sizeof(buf))));
    LIBSTD_RECORD_TEST(++tc, (0 == strcmp(buf, test_str)));

    test_str = libstd_str_null_c;
    LIBSTD_RECORD_TEST(++tc, (strlen(test_str) == strlcpy(buf, test_str, sizeof(buf))));
    LIBSTD_RECORD_TEST(++tc, (0 == strcmp(buf, test_str)));
    
    test_str = libstd_str_1_9_c;
    LIBSTD_RECORD_TEST(++tc, (strlen(test_str) == strlcpy(buf, test_str, sizeof(buf))));
    LIBSTD_RECORD_TEST(++tc, (0 == strcmp(buf, test_str)));

    test_str = libstd_str_1_A_c;
    LIBSTD_RECORD_TEST(++tc, (strlen(test_str) == strlcpy(buf, test_str, sizeof(buf))));
    LIBSTD_RECORD_TEST(++tc, (0 > strcmp(buf, test_str)));
    LIBSTD_RECORD_TEST(++tc, (0 == strcmp(buf, libstd_str_1_9_c)));

    test_str = libstd_str_1_F_c;
    LIBSTD_RECORD_TEST(++tc, (strlen(test_str) == strlcpy(buf, test_str, sizeof(buf))));
    LIBSTD_RECORD_TEST(++tc, (0 > strcmp(buf, test_str)));
    LIBSTD_RECORD_TEST(++tc, (0 == strcmp(buf, libstd_str_1_9_c)));
}

void libstd_start_wcslcpy_tests(void)
{
    int tc = 0;
    const wchar_t *test_str = NULL;
    wchar_t buf[10];
    
    test_str = L"1";
    LIBSTD_RECORD_TEST(++tc, (wcslen(test_str) == wcslcpy(buf, test_str, 10)));
    LIBSTD_RECORD_TEST(++tc, (0 == wcscmp(buf, test_str)));

    test_str = libstd_str_null_wct;
    LIBSTD_RECORD_TEST(++tc, (wcslen(test_str) == wcslcpy(buf, test_str, 10)));
    LIBSTD_RECORD_TEST(++tc, (0 == wcscmp(buf, test_str)));
    
    test_str = libstd_str_1_9_wct;
    LIBSTD_RECORD_TEST(++tc, (wcslen(test_str) == wcslcpy(buf, test_str, 10)));
    LIBSTD_RECORD_TEST(++tc, (0 == wcscmp(buf, test_str)));

    test_str = libstd_str_1_A_wct;
    LIBSTD_RECORD_TEST(++tc, (wcslen(test_str) == wcslcpy(buf, test_str, 10)));
    LIBSTD_RECORD_TEST(++tc, (0 > wcscmp(buf, test_str)));
    LIBSTD_RECORD_TEST(++tc, (0 == wcscmp(buf, libstd_str_1_9_wct)));

    test_str = libstd_str_1_F_wct;
    LIBSTD_RECORD_TEST(++tc, (wcslen(test_str) == wcslcpy(buf, test_str, 10)));
    LIBSTD_RECORD_TEST(++tc, (0 > wcscmp(buf, test_str)));
    LIBSTD_RECORD_TEST(++tc, (0 == wcscmp(buf, libstd_str_1_9_wct)));
}

void libstd_start_wstrlcpy_tests(void)
{
    int tc = 0;
    const wchar *test_str = NULL;
    wchar buf[10];
    wchar str_1_wc[] = {'1' , '\0'};

    test_str = str_1_wc;
    LIBSTD_RECORD_TEST(++tc, (wstrlen(test_str) == wstrlcpy(buf, test_str, 10)));
    LIBSTD_RECORD_TEST(++tc, (0 == wstrcmp(buf, test_str)));

    test_str = libstd_str_null_wc;
    LIBSTD_RECORD_TEST(++tc, (wstrlen(test_str) == wstrlcpy(buf, test_str, 10)));
    LIBSTD_RECORD_TEST(++tc, (0 == wstrcmp(buf, test_str)));

    test_str = libstd_str_1_9_wc;
    LIBSTD_RECORD_TEST(++tc, (wstrlen(test_str) == wstrlcpy(buf, test_str, 10)));
    LIBSTD_RECORD_TEST(++tc, (0 == wstrcmp(buf, test_str)));

    test_str = libstd_str_1_A_wc;
    LIBSTD_RECORD_TEST(++tc, (wstrlen(test_str) == wstrlcpy(buf, test_str, 10)));
    LIBSTD_RECORD_TEST(++tc, (0 > wstrcmp(buf, test_str)));
    LIBSTD_RECORD_TEST(++tc, (0 == wstrcmp(buf, libstd_str_1_9_wc)));

    test_str = libstd_str_1_F_wc;
    LIBSTD_RECORD_TEST(++tc, (wstrlen(test_str) == wstrlcpy(buf, test_str, 10)));
    LIBSTD_RECORD_TEST(++tc, (0 > wstrcmp(buf, test_str)));
    LIBSTD_RECORD_TEST(++tc, (0 == wstrcmp(buf, libstd_str_1_9_wc)));
}

void libstd_start_strlcat_tests(void)
{
    int tc = 0;
    char buf[10];

    strlcpy(buf, "123", sizeof(buf));
    LIBSTD_RECORD_TEST(++tc, (6 == strlcat(buf, "456", sizeof(buf))));
    LIBSTD_RECORD_TEST(++tc, (0 == strcmp(buf, "123456")));
    
    //strlcpy(buf, "123", sizeof(buf));
    //LIBSTD_RECORD_TEST(++tc, (4 == strlcat(buf, "4\05", sizeof(buf))));
    //LIBSTD_RECORD_TEST(++tc, (0 == strcmp(buf, "1234")));

    strlcpy(buf, "123", sizeof(buf));
    LIBSTD_RECORD_TEST(++tc, (10 == strlcat(buf, "4567890", sizeof(buf))));
    LIBSTD_RECORD_TEST(++tc, (0 == strcmp(buf, libstd_str_1_9_c)));

    strlcpy(buf, libstd_str_1_9_c, sizeof(buf));
    LIBSTD_RECORD_TEST(++tc, (14 == strlcat(buf, "01234", sizeof(buf))));
    LIBSTD_RECORD_TEST(++tc, (0 == strcmp(buf, libstd_str_1_9_c)));

    strlcpy(buf, "123", sizeof(buf));
    LIBSTD_RECORD_TEST(++tc, (14 == strlcat(buf, "45678901234", sizeof(buf))));
    LIBSTD_RECORD_TEST(++tc, (0 == strcmp(buf, libstd_str_1_9_c)));
}

void libstd_start_wcslcat_tests(void)
{
    int tc = 0;
    wchar_t buf[10];

    wcslcpy(buf, L"123", 10);
    LIBSTD_RECORD_TEST(++tc, (6 == wcslcat(buf, L"456", 10)));
    LIBSTD_RECORD_TEST(++tc, (0 == wcscmp(buf, L"123456")));
    
    //wcslcpy(buf, L"123", 10);
    //LIBSTD_RECORD_TEST(++tc, (4 == wcslcat(buf, L"4\05", 10)));
    //LIBSTD_RECORD_TEST(++tc, (0 == wcscmp(buf, L"1234")));

    wcslcpy(buf, L"123", 10);
    LIBSTD_RECORD_TEST(++tc, (10 == wcslcat(buf, L"4567890", 10)));
    LIBSTD_RECORD_TEST(++tc, (0 == wcscmp(buf, libstd_str_1_9_wct)));

    wcslcpy(buf, libstd_str_1_9_wct, 10);
    LIBSTD_RECORD_TEST(++tc, (14 == wcslcat(buf, L"01234", 10)));
    LIBSTD_RECORD_TEST(++tc, (0 == wcscmp(buf, libstd_str_1_9_wct)));

    wcslcpy(buf, L"123", 10);
    LIBSTD_RECORD_TEST(++tc, (14 == wcslcat(buf, L"45678901234", 10)));
    LIBSTD_RECORD_TEST(++tc, (0 == wcscmp(buf, libstd_str_1_9_wct)));
}

void libstd_start_wstrlcat_tests(void)
{
    int tc = 0;
    wchar buf[10];

    wstrlcpy(buf, str_1_3_wc, 10);
    LIBSTD_RECORD_TEST(++tc, (6 == wstrlcat(buf, str_4_6_wc, 10)));
    LIBSTD_RECORD_TEST(++tc, (0 == wstrcmp(buf, str_1_6_wc)));

    wstrlcpy(buf, str_1_3_wc, 10);
    LIBSTD_RECORD_TEST(++tc, (10 == wstrlcat(buf, str_4_0_wc, 10)));
    LIBSTD_RECORD_TEST(++tc, (0 == wstrcmp(buf, libstd_str_1_9_wc)));

    wstrlcpy(buf, libstd_str_1_9_wc, 10);
    LIBSTD_RECORD_TEST(++tc, (14 == wstrlcat(buf, str_0_4_wc, 10)));
    LIBSTD_RECORD_TEST(++tc, (0 == wstrcmp(buf, libstd_str_1_9_wc)));

    wstrlcpy(buf, str_1_3_wc, 10);
    LIBSTD_RECORD_TEST(++tc, (14 == wstrlcat(buf, std_4_0_4_wc, 10)));
    LIBSTD_RECORD_TEST(++tc, (0 == wstrcmp(buf, libstd_str_1_9_wc)));
}

void libstd_start_std_scanul_tests(void)
{
    int tc = 0, ret_val, err_code;
    char *test_str = NULL;
    const char *buf = NULL;
    
    test_str = "1234";
    ret_val = std_scanul(test_str, 10, &buf, &err_code);
    LIBSTD_RECORD_TEST(++tc, (1234 == ret_val));

    test_str = "+1234";
    ret_val = std_scanul(test_str, 10, &buf, &err_code);
    LIBSTD_RECORD_TEST(++tc, (1234 == ret_val));
    
    test_str = " \t\t\t 1234";
    ret_val = std_scanul(test_str, 10, &buf, &err_code);
    LIBSTD_RECORD_TEST(++tc, (1234 == ret_val));

    test_str = "\t\t\t\t\t\t+1234";
    ret_val = std_scanul(test_str, 10, &buf, &err_code);
    LIBSTD_RECORD_TEST(++tc, (1234 == ret_val));

    test_str = "01234";
    ret_val = std_scanul(test_str, 8, &buf, &err_code);
    LIBSTD_RECORD_TEST(++tc, (668 == ret_val));

    test_str = "0x1234";
    ret_val = std_scanul(test_str, 16, &buf, &err_code);
    LIBSTD_RECORD_TEST(++tc, (0x1234 == ret_val));

    test_str = "0xFFFFFFFF";
    ret_val = std_scanul(test_str, 16, &buf, &err_code);
    LIBSTD_RECORD_TEST(++tc, (0xFFFFFFFF == ret_val));

    /* Auto radix detection */
    test_str = "1234";
    ret_val = std_scanul(test_str, 0, &buf, &err_code);
    LIBSTD_RECORD_TEST(++tc, (1234 == ret_val));

    test_str = "01234";
    ret_val = std_scanul(test_str, 0, &buf, &err_code);
    LIBSTD_RECORD_TEST(++tc, (668 == ret_val));
    
    test_str = "0x1234";
    ret_val = std_scanul(test_str, 0, &buf, &err_code);
    LIBSTD_RECORD_TEST(++tc, (0x1234 == ret_val));
    
    test_str = "0X1234";
    ret_val = std_scanul(test_str, 0, &buf, &err_code);
    LIBSTD_RECORD_TEST(++tc, (0x1234 == ret_val));

    /* Bad radix specified */
    test_str = "1234";
    ret_val = std_scanul(test_str, -1, &buf, &err_code);
    LIBSTD_RECORD_TEST(++tc, (0 == ret_val));
    LIBSTD_RECORD_TEST(++tc, (STD_BADPARAM == err_code));
    LIBSTD_RECORD_TEST(++tc, (buf == test_str));
    
    test_str = "1234";
    ret_val = std_scanul(test_str, 1, &buf, &err_code);
    LIBSTD_RECORD_TEST(++tc, (0 == ret_val));
    LIBSTD_RECORD_TEST(++tc, (STD_BADPARAM == err_code));
    LIBSTD_RECORD_TEST(++tc, (buf == test_str));
    
    test_str = "1234";
    ret_val = std_scanul(test_str, 37, &buf, &err_code);
    LIBSTD_RECORD_TEST(++tc, (0 == ret_val));
    LIBSTD_RECORD_TEST(++tc, (STD_BADPARAM == err_code));
    LIBSTD_RECORD_TEST(++tc, (buf == test_str));

    /* No digits specified */
    test_str = "";
    ret_val = std_scanul(test_str, 10, &buf, &err_code);
    LIBSTD_RECORD_TEST(++tc, (0 == ret_val));
    LIBSTD_RECORD_TEST(++tc, (STD_NODIGITS == err_code));

    test_str = "         ";
    ret_val = std_scanul(test_str, 10, &buf, &err_code);
    LIBSTD_RECORD_TEST(++tc, (0 == ret_val));
    LIBSTD_RECORD_TEST(++tc, (STD_NODIGITS == err_code));

    test_str = "     #$%^&";
    ret_val = std_scanul(test_str, 10, &buf, &err_code);
    LIBSTD_RECORD_TEST(++tc, (0 == ret_val));
    LIBSTD_RECORD_TEST(++tc, (STD_NODIGITS == err_code));

    /* Partially valid string */
    test_str = "0x1234";
    ret_val = std_scanul(test_str, 10, &buf, &err_code);
    LIBSTD_RECORD_TEST(++tc, (0 == ret_val));
    LIBSTD_RECORD_TEST(++tc, (0 == strcmp(buf, "x1234")));

    test_str = "1234ABC";
    ret_val = std_scanul(test_str, 10, &buf, &err_code);
    LIBSTD_RECORD_TEST(++tc, (1234 == ret_val));
    LIBSTD_RECORD_TEST(++tc, (0 == strcmp(buf, "ABC")));
    
    /* Negative input specified */
    test_str = "   -1234";
    ret_val = std_scanul(test_str, 10, &buf, &err_code);
    LIBSTD_RECORD_TEST(++tc, (1234 == (-1*(signed int)ret_val)));
    LIBSTD_RECORD_TEST(++tc, (STD_NEGATIVE == err_code));
    
    /* Overflow cases */
    test_str = "0x100000000"; //0xFFFFFFFF + 1
    ret_val = std_scanul(test_str, 16, &buf, &err_code);
    LIBSTD_RECORD_TEST(++tc, (0xFFFFFFFF == ret_val));    
    LIBSTD_RECORD_TEST(++tc, (STD_OVERFLOW == err_code));
    //This is what the AEE documentation implies
    //LIBSTD_RECORD_TEST(++tc, (buf == test_str));
    //This is what the implementation does, check with CS team if accurate
    //LIBSTD_RECORD_TEST(++tc, ((buf - test_str) == strlen(test_str)));
}
