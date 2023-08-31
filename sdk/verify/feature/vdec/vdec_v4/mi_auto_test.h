/* Copyright (c) 2018-2020 Sigmastar Technology Corp.
 All rights reserved.

  Unless otherwise stipulated in writing, any and all information contained
 herein regardless in any format shall remain the sole proprietary of
 Sigmastar Technology Corp. and be kept in strict confidence
 (��Sigmastar Confidential Information��) by the recipient.
 Any unauthorized act including without limitation unauthorized disclosure,
 copying, use, reproduction, sale, distribution, modification, disassembling,
 reverse engineering and compiling of the contents of Sigmastar Confidential
 Information is unlawful and strictly prohibited. Sigmastar hereby reserves the
 rights to any and all damages, losses, costs and expenses resulting therefrom.
*/
#ifndef _MI_AUTO_TEST_H_
#define _MI_AUTO_TEST_H_
#define MI_TEST_FAILE 1
#define MI_TEST_TIMEOUT 2
#define mi_auto_test_print(mi_module, type, test_case, test_result)                                    \
    ({                                                                                                 \
        do                                                                                             \
        {                                                                                              \
            fflush(stdout);                                                                            \
            printf(ASCII_COLOR_RED "[%s][%s][%s][%s]\n", #mi_module, #type, #test_case, #test_result); \
            printf(ASCII_COLOR_END);                                                                   \
        } while (0);                                                                                   \
    })

#define mi_auto_test_print_pass(mi_module, type, test_case, test_result)                                 \
    ({                                                                                                   \
        do                                                                                               \
        {                                                                                                \
            fflush(stdout);                                                                              \
            printf(ASCII_COLOR_GREEN "[%s][%s][%s][%s]\n", #mi_module, #type, test_case, #test_result); \
            printf(ASCII_COLOR_END);                                                                     \
        } while (0);                                                                                     \
    })

#define mi_unit_testing_result(module_name, type, test_case, result)         \
    ({                                                                       \
        do                                                                   \
        {                                                                    \
            if (result == MI_SUCCESS)                                        \
            {                                                                \
                mi_auto_test_print_pass(module_name, type, test_case, Pass); \
            }                                                                \
            else if (result == MI_TEST_TIMEOUT)                              \
            {                                                                \
                mi_auto_test_print(module_name, type, test_case, Timeout);   \
            }                                                                \
            else if (result == MI_TEST_FAILE)                                \
            {                                                                \
                mi_auto_test_print(module_name, type, test_case, Failed);    \
            }                                                                \
        } while (0);                                                         \
    })
#endif
