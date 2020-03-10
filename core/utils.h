/**
 ******************************************************************************
 * @file    xxx.h
 * @author  chenzhipeng
 * @version V1.0.0
 * @date    29-Jun-2019
 * @brief
 ******************************************************************************
 * @attention
 *
 ******************************************************************************
 */

/** @addtogroup Project
  * @{
  */

/* Define to prevent recursive inclusion */
#ifndef _UTILS_H_
#define _UTILS_H_

#ifdef __cplusplus
 extern "C" {
#endif

/***********************************************
				 include
***********************************************/
#include <core/types.h>
#include <vast_common.h>

/***********************************************
				 define
***********************************************/
#if 0
#define _U  0x01    /* upper */
#define _L  0x02    /* lower */
#define _D  0x04    /* digit */
#define _C  0x08    /* cntrl */
#define _P  0x10    /* punct */
#define _S  0x20    /* white space (space/lf/tab) */
#define _X  0x40    /* hex digit */
#define _SP 0x80    /* hard space (0x20) */
#endif

#define __VAST_UNUSED(X) 				(void)(X)      /* To avoid gcc/g++ warnings */


#define __VAST_SET_BIT(REG, BIT)     	((REG) |= (1<<(BIT)))
#define __VAST_CLEAR_BIT(REG, BIT)   	((REG) &= ~(1<<(BIT)))

#define __VAST_READ_BIT(REG, BIT)   	((REG) & (BIT))

#define __VAST_CHECK_BIT(x ,BIT) 		(!(!((x) & 1<< (BIT))))  //check whether the BIT of x is 1;
#define __VAST_TST_BIT					__VAST_CHECK_BIT
#define __VAST_IS_BIT_SET    			__VAST_CHECK_BIT
#define __VAST_IS_BIT_CLR     			!__VAST_CHECK_BIT

#define __VAST_GET_BITS(val,x1,x2) 		( ((val)>>(x1)) & ((1<<((x2)-(x1)+1))-1) )

#define __VAST_CLEAR_REG(REG)        	((REG) = (0x0))

#define __VAST_WRITE_REG(REG, VAL)   	((REG) = (VAL))

#define __VAST_READ_REG(REG)         	((REG))

#define __VAST_MODIFY_REG(REG, CLEARMASK, SETMASK)  WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))

#define __VAST_POSITION_VAL(VAL)     	(__CLZ(__RBIT(VAL)))

#define __VAST_ABS(x)   				((x)>0 ? (x) : -(x))

#define __VAST_GET_SIGN(x) 				((x)>=0?(1):0)  //get sign

//#define __VAST_SWAP(x, y)				do{x = x + y; y = x - y; x = x - y;}while(0)
#define __VAST_SWAP(x, y)				do {(x) ^= (y); (y) ^= (x); (x) ^= (y);} while (0)

#define	__VAST_IS2N(x)					((x) != 0 && (((x) & ((x) - 1)) == 0))    	//Is it some power of 2

#define __VAST_LOCK(__HANDLE__)      	do{                                        \
										  if((__HANDLE__)->lock == VAST_LOCKED)   \
										  {                                      \
											 return (int)VAST_BUSY;                    \
										  }                                      \
										  else                                   \
										  {                                      \
											 (__HANDLE__)->lock = (int)VAST_LOCKED;    \
										  }                                      \
										}while (0U)

#define __VAST_UNLOCK(__HANDLE__)     	do{                                       \
											(__HANDLE__)->lock = (int)VAST_UNLOCKED;    \
										}while (0U)

#define	__VAST_LIMIT(x, x_min, x_max)	(((x) > (x_max)) ? ((x) = (x_max)) : (((x) < (x_min)) ? ((x) = (x_min)) : (x)))
#define	__VAST_MIN_LIMIT(x, x_min)		((x)>(x_min) ? (x):(x_min))
#define	__VAST_MAX_LIMIT(x, x_max) 	 	((x)<(x_max) ? (x):(x_max))

#if (defined(__GNUC__) || defined(__SUNPRO_CC) || defined (__SUNPRO_C)) && defined(CJSON_API_VISIBILITY)
#define VAST_PUBLIC(type)   __attribute__((visibility("default"))) type
#else
#define VAST_PUBLIC(type) type
#endif

#define __VAST_DIM(array_) (sizeof(array_) / sizeof((array_)[0]))  //dimension

#define vast_malloc             malloc
#define vast_free               free

#define set_bit(x, bit) 		((x) |= 1 << (bit))
#define clr_bit(x, bit) 		((x) &= ~(1 << (bit)))
#define tst_bit(x, bit) 		((x) & (1 << (bit)))
#define get_bits(val,x1,x2) 	(((val)>>(x1))&((1<<((x2)-(x1)+1))-1))

#ifndef min
#define min(a, b) 				((a)<(b) ? (a):(b))
#endif
#ifndef max
#define max(a, b) 				((a)>(b) ? (a):(b))
#endif

#define array_size(array) (sizeof(array)/sizeof(*array))

#define FIELD_SIZEOF(t, f) (sizeof(((t*)0)->f))
#define DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d))

#ifndef offset_of
# define offset_of(TYPE, MEMBER) ((unsigned long) &((TYPE *)0)->MEMBER)
#endif
/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:    the pointer to the member.
 * @type:   the type of the container struct this is embedded in.
 * @member: the name of the member within the struct.
 *
 */
#define container_of(ptr, type, member) \
        ((type *)((char *)(ptr) - (unsigned long)(&((type *)0)->member)))

/*
 * This looks more complex than it should be. But we need to
 * get the type for the ~ right in round_down (it needs to be
 * as wide as the result!), and we want to evaluate the macro
 * arguments just once each.
 */
#define __round_mask(x, y) ((uint32_t)((y)-1))
#define round_up(x, y) ((((x)-1) | __round_mask(x, y))+1)
#define round_down(x, y) ((x) & ~__round_mask(x, y))

#define _swab16(x) ((uint16_t)(             \
    (((uint16_t)(x) & (uint16_t)0x00ffU) << 8) |    \
    (((uint16_t)(x) & (uint16_t)0xff00U) >> 8)))

#define _swab32(x) ((uint32_t)(             \
    (((uint32_t)(x) & (uint32_t)0x000000ffUL) << 24) |\
    (((uint32_t)(x) & (uint32_t)0x0000ff00UL) <<  8) |\
    (((uint32_t)(x) & (uint32_t)0x00ff0000UL) >>  8) |\
    (((uint32_t)(x) & (uint32_t)0xff000000UL) >> 24)))

#ifdef BIG_ENDIAN
#define cpu_to_le32(x) ((uint32_t)_swab32((x)))
#define le32_to_cpu(x) ((uint32_t)_swab32((uint32_t)(x)))
#define cpu_to_le16(x) ((uint16_t)_swab16((x)))
#define le16_to_cpu(x) ((uint16_t)_swab16((uint16_t)(x)))
#define cpu_to_be32(x) ((uint32_t)(x))
#define be32_to_cpu(x) ((uint32_t)(x))
#define cpu_to_be16(x) ((uint16_t)(x))
#define be16_to_cpu(x) ((uint16_t)(x))
#else
#define cpu_to_le32(x) ((uint32_t)(x))
#define le32_to_cpu(x) ((uint32_t)(x))
#define cpu_to_le16(x) ((uint16_t)(x))
#define le16_to_cpu(x) ((uint16_t)(x))
#define cpu_to_be32(x) ((uint32_t)_swab32((x)))
#define be32_to_cpu(x) ((uint32_t)_swab32((uint32_t)(x)))
#define cpu_to_be16(x) ((uint16_t)_swab16((x)))
#define be16_to_cpu(x) ((uint16_t)_swab16((uint16_t)(x)))
#endif

#define bcd2bin(val) (((val) & 0x0f) + ((val) >> 4) * 10)
#define bin2bcd(val) ((((val) / 10) << 4) + (val) % 10)

#define _abs(a) ((a) < 0 ? -(a) : (a))
#define swap(a, b) do {a ^= b; b ^= a; a ^= b;} while (0)

#define memset    os_memset
#define memcpy    os_memcpy

#define strncmp   os_strncmp
#define strcmp    os_strcmp
#define strlen    os_strlen
#define strdup    os_strdup

#define strchr    os_strchr
#define strstr    os_strstr
//#define sscanf    os_sscanf
#define strncpy   os_strncpy
#define strcpy    os_strcpy
#define memmove   os_memmove
#define memcmp    os_memcmp
 //#define strcasecmp  os_strcasecmp

#ifndef stricmp
	#define stricmp  os_strcasecmp
#endif

/***********************************************
				 typedef
***********************************************/

/***********************************************
			function prototypes
***********************************************/
uint8_t checksum(const void *data, int len);

/* byte order */
uint32_t get_le_val(const uint8_t *p, int bytes);
uint32_t get_be_val(const uint8_t *p, int bytes);
void put_le_val(uint32_t val, uint8_t *p, int bytes);
void put_be_val(uint32_t val, uint8_t *p, int bytes);

int split(char *str, char **arr, const char *del);
int is_all_xx(const void *s1, uint8_t val, int n);

const char *_arr2str(const void *arr, int len, void *dest, int maxlen, int del);
const char *arr2str(const void *arr, int len, void *dest, int maxlen);
int str2arr(const char *str, void *out, int maxlen);
char *i2str(uint8_t val, char *destStr);

void reverse(void *buff, size_t len);

unsigned int mtd_crc32(unsigned int crc, const char *buf, int len);
uint16_t crc16(uint16_t crc, const void *buf, int size);
int count_bit_in_long(unsigned long x);

void *memmem(const void *buf1, int len1, const void *buf2, int len2);
void memadd(const void *data, int add, size_t n);

bool is_all_bcd(const uint8_t *data, size_t n);
void hex2bcd(uint32_t value, uint8_t *bcd, uint8_t bytes);
uint32_t bcd2hex(uint8_t *bcd, uint8_t bytes);

void float_to_bcd(float f, ubase_t bytes, ubase_t frabits, u8 *bcd);
float bcd_to_float(u8 *bcd, ubase_t bytes, ubase_t frabits, bool withsign);

bool is_bcd_time_valid(const uint8_t *bcdtime);
uint8_t week(uint8_t y, uint8_t m, uint8_t d);

unsigned int jiffies_to_msecs(const unsigned long j);

void *os_memset(void *src, int c, size_t n);
void *os_memcpy(void *dest, const void *src, size_t n);

int32_t os_strncmp(const char *cs, const char *ct, size_t count);
int32_t os_strcmp(const char *cs, const char *ct);
size_t  os_strlen(const char *src);
char   *os_strdup(const char *s);

char   *os_strchr(char *s, char c);
char   *os_strstr(const char *str1, const char *str2);
int32_t os_sscanf(const char *buf, const char *fmt, ...);
char   *os_strncpy(char *dest, const char *src, size_t n);
char   *os_strcpy(char *des, const char *src);
void   *os_memmove(void *dest, const void *src, size_t n);
int32_t os_memcmp(const void *cs, const void *ct, size_t count);
uint32_t os_strcasecmp(const char *a, const char *b);

long strtol(const char *str, char **endptr, int base);
long long strtoll(const char *str, char **endptr, int base);
unsigned long strtoul(const char *str, char **endptr, int base);

/***********************************************
	      		    inline
***********************************************/

/***********************************************
                   variable
***********************************************/


#ifdef __cplusplus
 }
#endif

#endif /*_UTILS_H_*/
/**
  * @}
  */

/*************** (C) COPYRIGHT chenzhipeng *******END OF FILE******/
