
/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

#ifndef __BL_HELPER_H
#define __BL_HELPER_H

/**
 * @addtogroup helper
 * @{
 */

/**************************************************************************//**
 * @file        helper.h
 *
 * @brief       Provides helper API to the bootloader
 * 
 * @author      Matthew Krause
 *
 * @date        2022-09-25
 *****************************************************************************/
#include "config.h"

#define MEMCPY BL_MemCpy
#define MEMSET BL_MemSet

BL_STATIC BL_INLINE void *BL_MemCpy(void *dest,
                                    void *src,
                                    BL_UINT32_T length);
BL_STATIC BL_INLINE void *BL_MemSet(void *dest,
                                    BL_UINT8_T data,
                                    BL_UINT32_T length);

BL_STATIC BL_INLINE void *BL_MemCpy(void *dest,
                                    void *src,
                                    BL_UINT32_T length)
{
    BL_UINT8_T *d = (BL_UINT8_T *) dest;
    BL_UINT8_T *s = (BL_UINT8_T *) src;
    while (length--)
    {
        *d++ = *s++;
    }
    return dest;
}

BL_STATIC BL_INLINE void *BL_MemSet(void *dest,
                                    BL_UINT8_T data,
                                    BL_UINT32_T length)
{
    BL_UINT8_T *p = (BL_UINT8_T *) dest;
    while (length-- > 0)
    {
        *p++ = data;
    }
    return dest;
}


/**@} helper */

#endif //__BL_HELPER_H