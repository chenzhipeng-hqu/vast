#ifndef _TYPES_H
#define _TYPES_H

#include <stddef.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>

#define SIZE_1K         (1024u)
#define SIZE_1M         (1024*1024u)

typedef long            base_t;      /* Nbit CPU related date type */
typedef unsigned long   ubase_t;     /* Nbit unsigned CPU related data type */

typedef ubase_t         sig_t;
//typedef ubase_t         time_t;
typedef int          	err_t;
typedef base_t          off_t;

typedef uint8_t         u8;
typedef uint16_t        u16;
typedef uint32_t        u32;
typedef int8_t          s8;
typedef int16_t         s16;
typedef int32_t         s32;

typedef short int VAST_BOOL;

/**
  * @brief  VAST Lock structures definition
  */
typedef enum
{
	VAST_UNLOCKED 	= 0x00U,
	VAST_LOCKED 	= 0x01U
}lock_t;

/**
  * @brief  HAL Status structures definition
  */
enum
{
	VAST_OK   		= 0x00U,
	VAST_ERROR    	= 0x01U,
	VAST_BUSY     	= 0x02U,
	VAST_TIMEOUT  	= 0x03U
} ;

/**
 * block device geometry structure
 */
struct device_blk_geometry
{
    uint32_t sector_count;                           /**< count of sectors */
    uint32_t bytes_per_sector;                       /**< number of bytes per sector */
    uint32_t block_size;                             /**< number of bytes to erase one block */
};

typedef struct _data_t
{
	size_t 		len;
	uint8_t		buf[0];
}data_t;

#endif  /* end of _TYPES_H */
