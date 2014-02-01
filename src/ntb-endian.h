#ifndef NTB_ENDIAN_H
#define NTB_ENDIAN_H

#define NTB_SWAP_UINT16(x)                      \
  ((uint16_t)                                   \
   (((uint16_t) (x) >> 8) |                     \
    ((uint16_t) (x) << 8)))
#define NTB_SWAP_UINT32(x)                              \
  ((uint32_t)                                           \
   ((((uint32_t) (x) & UINT32_C(0x000000ff)) << 24) |   \
    (((uint32_t) (x) & UINT32_C(0x0000ff00)) << 8) |    \
    (((uint32_t) (x) & UINT32_C(0x00ff0000)) >> 8) |    \
    (((uint32_t) (x) & UINT32_C(0xff000000)) >> 24)))
#define NTB_SWAP_UINT64(x)                                              \
  ((uint64_t)                                                           \
   ((((uint64_t) (x) & (uint64_t) UINT64_C(0x00000000000000ff)) << 56) | \
    (((uint64_t) (x) & (uint64_t) UINT64_C(0x000000000000ff00)) << 40) | \
    (((uint64_t) (x) & (uint64_t) UINT64_C(0x0000000000ff0000)) << 24) | \
    (((uint64_t) (x) & (uint64_t) UINT64_C(0x00000000ff000000)) << 8) | \
    (((uint64_t) (x) & (uint64_t) UINT64_C(0x000000ff00000000)) >> 8) | \
    (((uint64_t) (x) & (uint64_t) UINT64_C(0x0000ff0000000000)) >> 24) | \
    (((uint64_t) (x) & (uint64_t) UINT64_C(0x00ff000000000000)) >> 40) | \
    (((uint64_t) (x) & (uint64_t) UINT64_C(0xff00000000000000)) >> 56)))

#if defined(HAVE_BIG_ENDIAN)
#define NTB_UINT16_FROM_BE(x) (x)
#define NTB_UINT32_FROM_BE(x) (x)
#define NTB_UINT64_FROM_BE(x) (x)
#define NTB_UINT16_FROM_LE(x) NTB_SWAP_UINT16(x)
#define NTB_UINT32_FROM_LE(x) NTB_SWAP_UINT32(x)
#define NTB_UINT64_FROM_LE(x) NTB_SWAP_UINT64(x)
#elif defined(HAVE_LITTLE_ENDIAN)
#define NTB_UINT16_FROM_LE(x) (x)
#define NTB_UINT32_FROM_LE(x) (x)
#define NTB_UINT64_FROM_LE(x) (x)
#define NTB_UINT16_FROM_BE(x) NTB_SWAP_UINT16(x)
#define NTB_UINT32_FROM_BE(x) NTB_SWAP_UINT32(x)
#define NTB_UINT64_FROM_BE(x) NTB_SWAP_UINT64(x)
#else
#error Platform is neither little-endian nor big-endian
#endif

#define NTB_UINT16_TO_LE(x) NTB_UINT16_FROM_LE(x)
#define NTB_UINT16_TO_BE(x) NTB_UINT16_FROM_BE(x)
#define NTB_UINT32_TO_LE(x) NTB_UINT32_FROM_LE(x)
#define NTB_UINT32_TO_BE(x) NTB_UINT32_FROM_BE(x)
#define NTB_UINT64_TO_LE(x) NTB_UINT64_FROM_LE(x)
#define NTB_UINT64_TO_BE(x) NTB_UINT64_FROM_BE(x)


#endif /* NTB_ENDIAN_H */

