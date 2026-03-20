/*
File Name    : yc_log.h
Author       : Yichip
Version      : V1.0
Date         : 2023/11/25
Description  : yc log interface define.
*/

#ifndef __YC_LOG_H__
#define __YC_LOG_H__

#include "stdio.h"
#include "board_config.h"
#include "yc_uart.h"

/*
日志系统说明:
yc系列日志函数自带函数名、行号及末尾换行
YC_LOG_SWITCH:日志总开关
yc_log:通用打印函数,一般用于通用SDK库函数及demo日志输出

对于复杂模块可参考BT模块在此文件中增加模块专用日志输出函数,以方便单独开关日志
YC_BT_LOG_SWITCH:bt模块常规日志开关,对应日志函数为yc_bt_log
YC_BT_WARNING_LOG_SWITCH:bt模块警告性日志开关,对应日志函数为yc_bt_warning_log
YC_BT_ERROR_LOG_SWITCH:bt模块错误日志开关,对应日志函数为yc_bt_error_log
*/

/*log switch control*/
#ifdef __cplusplus
extern "C" {
#endif

#define YC_LOG_SWITCH

/*bt log switch control*/
#ifdef YC_LOG_SWITCH
#define YC_BT_LOG_SWITCH
#define YC_BT_WARNING_LOG_SWITCH
#define YC_BT_ERROR_LOG_SWITCH
#define YC_USB_LOG_SWITCH
#endif

#ifdef YC_LOG_SWITCH

#define yc_log(fmt, ...)                                                        \
    do                                                                          \
    {                                                                           \
        printf(fmt "\r\n", ##__VA_ARGS__);                                      \
    } while (0)

#define yc_log_buf(str,buf,len) printv(str,buf,len)
#else
#define yc_log(fmt, ...)
#define yc_log_buf(str,buf,len)
#endif

#ifdef YC_BT_LOG_SWITCH
#define yc_bt_log(fmt, ...)                                                     \
    do                                                                          \
    {                                                                           \
        printf(fmt "\r\n", ##__VA_ARGS__);                                      \
    } while (0)

#define yc_bt_log_buf(str,buf,len) yc_log_buf(str, buf, len)
#else
#define yc_bt_log(fmt, ...)
#endif

#ifdef YC_BT_WARNING_LOG_SWITCH
#define yc_bt_warning_log(fmt, ...)                                             \
    do                                                                          \
    {                                                                           \
        printf(fmt "\r\n", ##__VA_ARGS__);                                      \
    } while (0)
#else
#define yc_bt_warning_log(fmt, ...)
#endif

#ifdef YC_BT_ERROR_LOG_SWITCH
#define yc_bt_error_log(fmt, ...)                                               \
    do                                                                          \
    {                                                                           \
        printf(fmt "\r\n", ##__VA_ARGS__);                                      \
    } while (0)
#else
#define yc_bt_error_log(fmt, ...)
#endif

#ifdef YC_USB_LOG_SWITCH
#define yc_usb_log(fmt, ...)                                                    \
    do                                                                          \
    {                                                                           \
        printf(fmt "\r\n", ##__VA_ARGS__);                                      \
    } while (0)
#else
#define yc_usb_log(fmt, ...)
#endif

void printport_init(void);

#ifdef YC_LOG_SWITCH
void _assert_handler(const char *file, int line, const char *func);
#define _ASSERT(x)                                         \
    if (!(x))                                              \
    {                                                      \
        __disable_irq();                                   \
        _assert_handler(__FILE__, __LINE__, __FUNCTION__); \
    }
#else
void _assert_handler(void);
#define _ASSERT(x)         \
    if (!(x))              \
    {                      \
        __disable_irq();   \
        _assert_handler(); \
    }
#endif

#ifndef YC_LOG_SWITCH
#define printf(fmt, ...)
#define printv(str, pbuf, len)
#else
void printv(char *s, uint8_t *buf, uint32_t len);
#endif

#ifdef __cplusplus
}
#endif
#endif /*__YC_LOG_H__*/
