/**************************************************************************//**
 * (c) 2022 Ahriman
 * This code is licensed under MIT license (see LICENSE.txt for details)
 *****************************************************************************/

/**
 * @addtogroup platform
 * @{
 */

/**
 * @file        platform.h
 *
 * @brief       Provides the platform dependent configuration includes, defines
 *              etc.
 *
 * @author      Matthew Krause
 *
 * @date        2022-09-05
 */

/*****************************************************************************/
#ifndef __PLATFORM_H
#define __PLATFOMR_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

#define RAM_FUNC __attribute__ ((section(".ram")))

#endif //__PLATFOMR_H