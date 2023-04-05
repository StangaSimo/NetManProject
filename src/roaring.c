// !!! DO NOT EDIT - THIS IS AN AUTO-GENERATED FILE !!!
// Created by amalgamation.sh on 2023-04-05T14:55:03Z

/*
 * The CRoaring project is under a dual license (Apache/MIT).
 * Users of the library may choose one or the other license.
 */
/*
 * Copyright 2016-2022 The CRoaring authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/*
 * MIT License
 *
 * Copyright 2016-2022 The CRoaring authors
 *
 * Permission is hereby granted, free of charge, to any
 * person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the
 * Software without restriction, including without
 * limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software
 * is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice
 * shall be included in all copies or substantial portions
 * of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF
 * ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
 * TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT
 * SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 * IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * SPDX-License-Identifier: MIT
 */

#include "roaring.h"

/* used for http://dmalloc.com/ Dmalloc - Debug Malloc Library */
#ifdef DMALLOC
#include "dmalloc.h"
#endif

#include "roaring.h"  /* include public API definitions */
/* begin file include/roaring/isadetection.h */
#ifndef ROARING_ISADETECTION_H
#define ROARING_ISADETECTION_H
#if defined(__x86_64__) || defined(_M_AMD64) // x64




#ifndef CROARING_COMPILER_SUPPORTS_AVX512
#ifdef __has_include
// We want to make sure that the AVX-512 functions are only built on compilers
// fully supporting AVX-512.
#if __has_include(<avx512vbmi2intrin.h>)
#define CROARING_COMPILER_SUPPORTS_AVX512 1
#endif // #if __has_include(<avx512vbmi2intrin.h>)
#endif // #ifdef __has_include

// Visual Studio 2019 and up support AVX-512
#ifdef _MSC_VER
#if _MSC_VER >= 1920
#define CROARING_COMPILER_SUPPORTS_AVX512 1
#endif // #if _MSC_VER >= 1920
#endif // #ifdef _MSC_VER

#ifndef CROARING_COMPILER_SUPPORTS_AVX512
#define CROARING_COMPILER_SUPPORTS_AVX512 0
#endif // #ifndef CROARING_COMPILER_SUPPORTS_AVX512
#endif // #ifndef CROARING_COMPILER_SUPPORTS_AVX512


#ifdef __cplusplus
extern "C" { namespace roaring { namespace internal {
#endif
enum {
  ROARING_SUPPORTS_AVX2 = 1,
  ROARING_SUPPORTS_AVX512 = 2,
};
int croaring_hardware_support();
#ifdef __cplusplus
} } }  // extern "C" { namespace roaring { namespace internal {
#endif
#endif // x64
#endif // ROARING_ISADETECTION_H
/* end file include/roaring/isadetection.h */
/* begin file include/roaring/containers/perfparameters.h */
#ifndef PERFPARAMETERS_H_
#define PERFPARAMETERS_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" { namespace roaring { namespace internal {
#endif

/**
During lazy computations, we can transform array containers into bitset
containers as
long as we can expect them to have  ARRAY_LAZY_LOWERBOUND values.
*/
enum { ARRAY_LAZY_LOWERBOUND = 1024 };

/* default initial size of a run container
   setting it to zero delays the malloc.*/
enum { RUN_DEFAULT_INIT_SIZE = 0 };

/* default initial size of an array container
   setting it to zero delays the malloc */
enum { ARRAY_DEFAULT_INIT_SIZE = 0 };

/* automatic bitset conversion during lazy or */
#ifndef LAZY_OR_BITSET_CONVERSION
#define LAZY_OR_BITSET_CONVERSION true
#endif

/* automatically attempt to convert a bitset to a full run during lazy
 * evaluation */
#ifndef LAZY_OR_BITSET_CONVERSION_TO_FULL
#define LAZY_OR_BITSET_CONVERSION_TO_FULL true
#endif

/* automatically attempt to convert a bitset to a full run */
#ifndef OR_BITSET_CONVERSION_TO_FULL
#define OR_BITSET_CONVERSION_TO_FULL true
#endif

#ifdef __cplusplus
} } }  // extern "C" { namespace roaring { namespace internal {
#endif

#endif
/* end file include/roaring/containers/perfparameters.h */
/* begin file include/roaring/containers/container_defs.h */
/*
 * container_defs.h
 *
 * Unlike containers.h (which is a file aggregating all the container includes,
 * like array.h, bitset.h, and run.h) this is a file included BY those headers
 * to do things like define the container base class `container_t`.
 */

#ifndef INCLUDE_CONTAINERS_CONTAINER_DEFS_H_
#define INCLUDE_CONTAINERS_CONTAINER_DEFS_H_

#ifdef __cplusplus
    #include <type_traits>  // used by casting helper for compile-time check
#endif

// The preferences are a separate file to separate out tweakable parameters

#ifdef __cplusplus
namespace roaring { namespace internal {  // No extern "C" (contains template)
#endif


/*
 * Since roaring_array_t's definition is not opaque, the container type is
 * part of the API.  If it's not going to be `void*` then it needs a name, and
 * expectations are to prefix C library-exported names with `roaring_` etc.
 *
 * Rather than force the whole codebase to use the name `roaring_container_t`,
 * the few API appearances use the macro ROARING_CONTAINER_T.  Those includes
 * are prior to containers.h, so make a short private alias of `container_t`.
 * Then undefine the awkward macro so it's not used any more than it has to be.
 */
typedef ROARING_CONTAINER_T container_t;
#undef ROARING_CONTAINER_T


/*
 * See ROARING_CONTAINER_T for notes on using container_t as a base class.
 * This macro helps make the following pattern look nicer:
 *
 *     #ifdef __cplusplus
 *     struct roaring_array_s : public container_t {
 *     #else
 *     struct roaring_array_s {
 *     #endif
 *         int32_t cardinality;
 *         int32_t capacity;
 *         uint16_t *array;
 *     }
 */
#if defined(__cplusplus)
    #define STRUCT_CONTAINER(name) \
        struct name : public container_t  /* { ... } */
#else
    #define STRUCT_CONTAINER(name) \
        struct name  /* { ... } */
#endif


/**
 * Since container_t* is not void* in C++, "dangerous" casts are not needed to
 * downcast; only a static_cast<> is needed.  Define a macro for static casting
 * which helps make casts more visible, and catches problems at compile-time
 * when building the C sources in C++ mode:
 *
 *     void some_func(container_t **c, ...) {  // double pointer, not single
 *         array_container_t *ac1 = (array_container_t *)(c);  // uncaught!!
 *
 *         array_container_t *ac2 = CAST(array_container_t *, c)  // C++ errors
 *         array_container_t *ac3 = CAST_array(c);  // shorthand for #2, errors
 *     }
 *
 * Trickier to do is a cast from `container**` to `array_container_t**`.  This
 * needs a reinterpret_cast<>, which sacrifices safety...so a template is used
 * leveraging <type_traits> to make sure it's legal in the C++ build.
 */
#ifdef __cplusplus
    #define CAST(type,value)            static_cast<type>(value)
    #define movable_CAST(type,value)    movable_CAST_HELPER<type>(value)

    template<typename PPDerived, typename Base>
    PPDerived movable_CAST_HELPER(Base **ptr_to_ptr) {
        typedef typename std::remove_pointer<PPDerived>::type PDerived;
        typedef typename std::remove_pointer<PDerived>::type Derived;
        static_assert(
            std::is_base_of<Base, Derived>::value,
            "use movable_CAST() for container_t** => xxx_container_t**"
        );
        return reinterpret_cast<Derived**>(ptr_to_ptr);
    }
#else
    #define CAST(type,value)            ((type)value)
    #define movable_CAST(type, value)   ((type)value)
#endif

// Use for converting e.g. an `array_container_t**` to a `container_t**`
//
#define movable_CAST_base(c)   movable_CAST(container_t **, c)


#ifdef __cplusplus
} }  // namespace roaring { namespace internal {
#endif

#endif  /* INCLUDE_CONTAINERS_CONTAINER_DEFS_H_ */
/* end file include/roaring/containers/container_defs.h */
/* begin file include/roaring/array_util.h */
#ifndef ARRAY_UTIL_H
#define ARRAY_UTIL_H

#include <stddef.h>  // for size_t
#include <stdint.h>


#if CROARING_IS_X64
#ifndef CROARING_COMPILER_SUPPORTS_AVX512
#error "CROARING_COMPILER_SUPPORTS_AVX512 needs to be defined."
#endif // CROARING_COMPILER_SUPPORTS_AVX512
#endif

#ifdef __cplusplus
extern "C" { namespace roaring { namespace internal {
#endif

/*
 *  Good old binary search.
 *  Assumes that array is sorted, has logarithmic complexity.
 *  if the result is x, then:
 *     if ( x>0 )  you have array[x] = ikey
 *     if ( x<0 ) then inserting ikey at position -x-1 in array (insuring that array[-x-1]=ikey)
 *                   keys the array sorted.
 */
inline int32_t binarySearch(const uint16_t *array, int32_t lenarray,
                            uint16_t ikey) {
    int32_t low = 0;
    int32_t high = lenarray - 1;
    while (low <= high) {
        int32_t middleIndex = (low + high) >> 1;
        uint16_t middleValue = array[middleIndex];
        if (middleValue < ikey) {
            low = middleIndex + 1;
        } else if (middleValue > ikey) {
            high = middleIndex - 1;
        } else {
            return middleIndex;
        }
    }
    return -(low + 1);
}

/**
 * Galloping search
 * Assumes that array is sorted, has logarithmic complexity.
 * if the result is x, then if x = length, you have that all values in array between pos and length
 *    are smaller than min.
 * otherwise returns the first index x such that array[x] >= min.
 */
static inline int32_t advanceUntil(const uint16_t *array, int32_t pos,
                                   int32_t length, uint16_t min) {
    int32_t lower = pos + 1;

    if ((lower >= length) || (array[lower] >= min)) {
        return lower;
    }

    int32_t spansize = 1;

    while ((lower + spansize < length) && (array[lower + spansize] < min)) {
        spansize <<= 1;
    }
    int32_t upper = (lower + spansize < length) ? lower + spansize : length - 1;

    if (array[upper] == min) {
        return upper;
    }
    if (array[upper] < min) {
        // means
        // array
        // has no
        // item
        // >= min
        // pos = array.length;
        return length;
    }

    // we know that the next-smallest span was too small
    lower += (spansize >> 1);

    int32_t mid = 0;
    while (lower + 1 != upper) {
        mid = (lower + upper) >> 1;
        if (array[mid] == min) {
            return mid;
        } else if (array[mid] < min) {
            lower = mid;
        } else {
            upper = mid;
        }
    }
    return upper;
}

/**
 * Returns number of elements which are less then $ikey.
 * Array elements must be unique and sorted.
 */
static inline int32_t count_less(const uint16_t *array, int32_t lenarray,
                                 uint16_t ikey) {
    if (lenarray == 0) return 0;
    int32_t pos = binarySearch(array, lenarray, ikey);
    return pos >= 0 ? pos : -(pos+1);
}

/**
 * Returns number of elements which are greater then $ikey.
 * Array elements must be unique and sorted.
 */
static inline int32_t count_greater(const uint16_t *array, int32_t lenarray,
                                    uint16_t ikey) {
    if (lenarray == 0) return 0;
    int32_t pos = binarySearch(array, lenarray, ikey);
    if (pos >= 0) {
        return lenarray - (pos+1);
    } else {
        return lenarray - (-pos-1);
    }
}

/**
 * From Schlegel et al., Fast Sorted-Set Intersection using SIMD Instructions
 * Optimized by D. Lemire on May 3rd 2013
 *
 * C should have capacity greater than the minimum of s_1 and s_b + 8
 * where 8 is sizeof(__m128i)/sizeof(uint16_t).
 */
int32_t intersect_vector16(const uint16_t *__restrict__ A, size_t s_a,
                           const uint16_t *__restrict__ B, size_t s_b,
                           uint16_t *C);

int32_t intersect_vector16_inplace(uint16_t *__restrict__ A, size_t s_a,
                           const uint16_t *__restrict__ B, size_t s_b);

/**
 * Take an array container and write it out to a 32-bit array, using base
 * as the offset.
 */
int array_container_to_uint32_array_vector16(void *vout, const uint16_t* array, size_t cardinality,
                                    uint32_t base);
#if CROARING_COMPILER_SUPPORTS_AVX512
int avx512_array_container_to_uint32_array(void *vout, const uint16_t* array, size_t cardinality,
                                    uint32_t base);
#endif
/**
 * Compute the cardinality of the intersection using SSE4 instructions
 */
int32_t intersect_vector16_cardinality(const uint16_t *__restrict__ A,
                                       size_t s_a,
                                       const uint16_t *__restrict__ B,
                                       size_t s_b);

/* Computes the intersection between one small and one large set of uint16_t.
 * Stores the result into buffer and return the number of elements. */
int32_t intersect_skewed_uint16(const uint16_t *smallarray, size_t size_s,
                                const uint16_t *largearray, size_t size_l,
                                uint16_t *buffer);

/* Computes the size of the intersection between one small and one large set of
 * uint16_t. */
int32_t intersect_skewed_uint16_cardinality(const uint16_t *smallarray,
                                            size_t size_s,
                                            const uint16_t *largearray,
                                            size_t size_l);


/* Check whether the size of the intersection between one small and one large set of uint16_t is non-zero. */
bool intersect_skewed_uint16_nonempty(const uint16_t *smallarray, size_t size_s,
                                const uint16_t *largearray, size_t size_l);
/**
 * Generic intersection function.
 */
int32_t intersect_uint16(const uint16_t *A, const size_t lenA,
                         const uint16_t *B, const size_t lenB, uint16_t *out);
/**
 * Compute the size of the intersection (generic).
 */
int32_t intersect_uint16_cardinality(const uint16_t *A, const size_t lenA,
                                     const uint16_t *B, const size_t lenB);

/**
 * Checking whether the size of the intersection  is non-zero.
 */
bool intersect_uint16_nonempty(const uint16_t *A, const size_t lenA,
                         const uint16_t *B, const size_t lenB);
/**
 * Generic union function.
 */
size_t union_uint16(const uint16_t *set_1, size_t size_1, const uint16_t *set_2,
                    size_t size_2, uint16_t *buffer);

/**
 * Generic XOR function.
 */
int32_t xor_uint16(const uint16_t *array_1, int32_t card_1,
                   const uint16_t *array_2, int32_t card_2, uint16_t *out);

/**
 * Generic difference function (ANDNOT).
 */
int difference_uint16(const uint16_t *a1, int length1, const uint16_t *a2,
                      int length2, uint16_t *a_out);

/**
 * Generic intersection function.
 */
size_t intersection_uint32(const uint32_t *A, const size_t lenA,
                           const uint32_t *B, const size_t lenB, uint32_t *out);

/**
 * Generic intersection function, returns just the cardinality.
 */
size_t intersection_uint32_card(const uint32_t *A, const size_t lenA,
                                const uint32_t *B, const size_t lenB);

/**
 * Generic union function.
 */
size_t union_uint32(const uint32_t *set_1, size_t size_1, const uint32_t *set_2,
                    size_t size_2, uint32_t *buffer);

/**
 * A fast SSE-based union function.
 */
uint32_t union_vector16(const uint16_t *__restrict__ set_1, uint32_t size_1,
                        const uint16_t *__restrict__ set_2, uint32_t size_2,
                        uint16_t *__restrict__ buffer);
/**
 * A fast SSE-based XOR function.
 */
uint32_t xor_vector16(const uint16_t *__restrict__ array1, uint32_t length1,
                      const uint16_t *__restrict__ array2, uint32_t length2,
                      uint16_t *__restrict__ output);

/**
 * A fast SSE-based difference function.
 */
int32_t difference_vector16(const uint16_t *__restrict__ A, size_t s_a,
                            const uint16_t *__restrict__ B, size_t s_b,
                            uint16_t *C);

/**
 * Generic union function, returns just the cardinality.
 */
size_t union_uint32_card(const uint32_t *set_1, size_t size_1,
                         const uint32_t *set_2, size_t size_2);

/**
* combines union_uint16 and  union_vector16 optimally
*/
size_t fast_union_uint16(const uint16_t *set_1, size_t size_1, const uint16_t *set_2,
                    size_t size_2, uint16_t *buffer);


bool memequals(const void *s1, const void *s2, size_t n);

#ifdef __cplusplus
} } }  // extern "C" { namespace roaring { namespace internal {
#endif

#endif
/* end file include/roaring/array_util.h */
/* begin file include/roaring/utilasm.h */
/*
 * utilasm.h
 *
 */

#ifndef INCLUDE_UTILASM_H_
#define INCLUDE_UTILASM_H_


#ifdef __cplusplus
extern "C" { namespace roaring {
#endif

#if defined(CROARING_INLINE_ASM)
#define CROARING_ASMBITMANIPOPTIMIZATION  // optimization flag

#define ASM_SHIFT_RIGHT(srcReg, bitsReg, destReg) \
    __asm volatile("shrx %1, %2, %0"              \
                   : "=r"(destReg)                \
                   :             /* write */      \
                   "r"(bitsReg), /* read only */  \
                   "r"(srcReg)   /* read only */  \
                   )

#define ASM_INPLACESHIFT_RIGHT(srcReg, bitsReg)  \
    __asm volatile("shrx %1, %0, %0"             \
                   : "+r"(srcReg)                \
                   :            /* read/write */ \
                   "r"(bitsReg) /* read only */  \
                   )

#define ASM_SHIFT_LEFT(srcReg, bitsReg, destReg) \
    __asm volatile("shlx %1, %2, %0"             \
                   : "=r"(destReg)               \
                   :             /* write */     \
                   "r"(bitsReg), /* read only */ \
                   "r"(srcReg)   /* read only */ \
                   )
// set bit at position testBit within testByte to 1 and
// copy cmovDst to cmovSrc if that bit was previously clear
#define ASM_SET_BIT_INC_WAS_CLEAR(testByte, testBit, count) \
    __asm volatile(                                         \
        "bts %2, %0\n"                                      \
        "sbb $-1, %1\n"                                     \
        : "+r"(testByte), /* read/write */                  \
          "+r"(count)                                       \
        :            /* read/write */                       \
        "r"(testBit) /* read only */                        \
        )

#define ASM_CLEAR_BIT_DEC_WAS_SET(testByte, testBit, count) \
    __asm volatile(                                         \
        "btr %2, %0\n"                                      \
        "sbb $0, %1\n"                                      \
        : "+r"(testByte), /* read/write */                  \
          "+r"(count)                                       \
        :            /* read/write */                       \
        "r"(testBit) /* read only */                        \
        )

#define ASM_BT64(testByte, testBit, count) \
    __asm volatile(                        \
        "bt %2,%1\n"                       \
        "sbb %0,%0" /*could use setb */    \
        : "=r"(count)                      \
        :              /* write */         \
        "r"(testByte), /* read only */     \
        "r"(testBit)   /* read only */     \
        )

#endif

#ifdef __cplusplus
} }  // extern "C" { namespace roaring {
#endif

#endif  /* INCLUDE_UTILASM_H_ */
/* end file include/roaring/utilasm.h */
/* begin file include/roaring/bitset_util.h */
#ifndef BITSET_UTIL_H
#define BITSET_UTIL_H

#include <stdint.h>


#if CROARING_IS_X64
#ifndef CROARING_COMPILER_SUPPORTS_AVX512
#error "CROARING_COMPILER_SUPPORTS_AVX512 needs to be defined."
#endif // CROARING_COMPILER_SUPPORTS_AVX512
#endif

#ifdef __cplusplus
extern "C" { namespace roaring { namespace internal {
#endif

/*
 * Set all bits in indexes [begin,end) to true.
 */
static inline void bitset_set_range(uint64_t *words, uint32_t start,
                                    uint32_t end) {
    if (start == end) return;
    uint32_t firstword = start / 64;
    uint32_t endword = (end - 1) / 64;
    if (firstword == endword) {
        words[firstword] |= ((~UINT64_C(0)) << (start % 64)) &
                             ((~UINT64_C(0)) >> ((~end + 1) % 64));
        return;
    }
    words[firstword] |= (~UINT64_C(0)) << (start % 64);
    for (uint32_t i = firstword + 1; i < endword; i++) {
        words[i] = ~UINT64_C(0);
    }
    words[endword] |= (~UINT64_C(0)) >> ((~end + 1) % 64);
}


/*
 * Find the cardinality of the bitset in [begin,begin+lenminusone]
 */
static inline int bitset_lenrange_cardinality(const uint64_t *words,
                                              uint32_t start,
                                              uint32_t lenminusone) {
    uint32_t firstword = start / 64;
    uint32_t endword = (start + lenminusone) / 64;
    if (firstword == endword) {
        return roaring_hamming(words[firstword] &
                       ((~UINT64_C(0)) >> ((63 - lenminusone) % 64))
                           << (start % 64));
    }
    int answer = roaring_hamming(words[firstword] & ((~UINT64_C(0)) << (start % 64)));
    for (uint32_t i = firstword + 1; i < endword; i++) {
        answer += roaring_hamming(words[i]);
    }
    answer +=
        roaring_hamming(words[endword] &
                (~UINT64_C(0)) >> (((~start + 1) - lenminusone - 1) % 64));
    return answer;
}

/*
 * Check whether the cardinality of the bitset in [begin,begin+lenminusone] is 0
 */
static inline bool bitset_lenrange_empty(const uint64_t *words, uint32_t start,
                                         uint32_t lenminusone) {
    uint32_t firstword = start / 64;
    uint32_t endword = (start + lenminusone) / 64;
    if (firstword == endword) {
        return (words[firstword] & ((~UINT64_C(0)) >> ((63 - lenminusone) % 64))
              << (start % 64)) == 0;
    }
    if (((words[firstword] & ((~UINT64_C(0)) << (start%64)))) != 0) {
        return false;
    }
    for (uint32_t i = firstword + 1; i < endword; i++) {
        if (words[i] != 0) {
            return false;
        }
    }
    if ((words[endword] & (~UINT64_C(0)) >> (((~start + 1) - lenminusone - 1) % 64)) != 0) {
        return false;
    }
    return true;
}


/*
 * Set all bits in indexes [begin,begin+lenminusone] to true.
 */
static inline void bitset_set_lenrange(uint64_t *words, uint32_t start,
                                       uint32_t lenminusone) {
    uint32_t firstword = start / 64;
    uint32_t endword = (start + lenminusone) / 64;
    if (firstword == endword) {
        words[firstword] |= ((~UINT64_C(0)) >> ((63 - lenminusone) % 64))
                             << (start % 64);
        return;
    }
    uint64_t temp = words[endword];
    words[firstword] |= (~UINT64_C(0)) << (start % 64);
    for (uint32_t i = firstword + 1; i < endword; i += 2)
        words[i] = words[i + 1] = ~UINT64_C(0);
    words[endword] =
        temp | (~UINT64_C(0)) >> (((~start + 1) - lenminusone - 1) % 64);
}

/*
 * Flip all the bits in indexes [begin,end).
 */
static inline void bitset_flip_range(uint64_t *words, uint32_t start,
                                     uint32_t end) {
    if (start == end) return;
    uint32_t firstword = start / 64;
    uint32_t endword = (end - 1) / 64;
    words[firstword] ^= ~((~UINT64_C(0)) << (start % 64));
    for (uint32_t i = firstword; i < endword; i++) {
        words[i] = ~words[i];
    }
    words[endword] ^= ((~UINT64_C(0)) >> ((~end + 1) % 64));
}

/*
 * Set all bits in indexes [begin,end) to false.
 */
static inline void bitset_reset_range(uint64_t *words, uint32_t start,
                                      uint32_t end) {
    if (start == end) return;
    uint32_t firstword = start / 64;
    uint32_t endword = (end - 1) / 64;
    if (firstword == endword) {
        words[firstword] &= ~(((~UINT64_C(0)) << (start % 64)) &
                               ((~UINT64_C(0)) >> ((~end + 1) % 64)));
        return;
    }
    words[firstword] &= ~((~UINT64_C(0)) << (start % 64));
    for (uint32_t i = firstword + 1; i < endword; i++) {
        words[i] = UINT64_C(0);
    }
    words[endword] &= ~((~UINT64_C(0)) >> ((~end + 1) % 64));
}

/*
 * Given a bitset containing "length" 64-bit words, write out the position
 * of all the set bits to "out", values start at "base".
 *
 * The "out" pointer should be sufficient to store the actual number of bits
 * set.
 *
 * Returns how many values were actually decoded.
 *
 * This function should only be expected to be faster than
 * bitset_extract_setbits
 * when the density of the bitset is high.
 *
 * This function uses AVX2 decoding.
 */
size_t bitset_extract_setbits_avx2(const uint64_t *words, size_t length,
                                   uint32_t *out, size_t outcapacity,
                                   uint32_t base);

size_t bitset_extract_setbits_avx512(const uint64_t *words, size_t length, 
                                   uint32_t *out, size_t outcapacity, 
                                   uint32_t base);
/*
 * Given a bitset containing "length" 64-bit words, write out the position
 * of all the set bits to "out", values start at "base".
 *
 * The "out" pointer should be sufficient to store the actual number of bits
 *set.
 *
 * Returns how many values were actually decoded.
 */
size_t bitset_extract_setbits(const uint64_t *words, size_t length,
                              uint32_t *out, uint32_t base);

/*
 * Given a bitset containing "length" 64-bit words, write out the position
 * of all the set bits to "out" as 16-bit integers, values start at "base" (can
 *be set to zero)
 *
 * The "out" pointer should be sufficient to store the actual number of bits
 *set.
 *
 * Returns how many values were actually decoded.
 *
 * This function should only be expected to be faster than
 *bitset_extract_setbits_uint16
 * when the density of the bitset is high.
 *
 * This function uses SSE decoding.
 */
size_t bitset_extract_setbits_sse_uint16(const uint64_t *words, size_t length,
                                         uint16_t *out, size_t outcapacity,
                                         uint16_t base);

size_t bitset_extract_setbits_avx512_uint16(const uint64_t *words, size_t length,
                                         uint16_t *out, size_t outcapacity, 
                                         uint16_t base);

/*
 * Given a bitset containing "length" 64-bit words, write out the position
 * of all the set bits to "out",  values start at "base"
 * (can be set to zero)
 *
 * The "out" pointer should be sufficient to store the actual number of bits
 *set.
 *
 * Returns how many values were actually decoded.
 */
size_t bitset_extract_setbits_uint16(const uint64_t *words, size_t length,
                                     uint16_t *out, uint16_t base);

/*
 * Given two bitsets containing "length" 64-bit words, write out the position
 * of all the common set bits to "out", values start at "base"
 * (can be set to zero)
 *
 * The "out" pointer should be sufficient to store the actual number of bits
 * set.
 *
 * Returns how many values were actually decoded.
 */
size_t bitset_extract_intersection_setbits_uint16(const uint64_t * __restrict__ words1,
                                                  const uint64_t * __restrict__ words2,
                                                  size_t length, uint16_t *out,
                                                  uint16_t base);

/*
 * Given a bitset having cardinality card, set all bit values in the list (there
 * are length of them)
 * and return the updated cardinality. This evidently assumes that the bitset
 * already contained data.
 */
uint64_t bitset_set_list_withcard(uint64_t *words, uint64_t card,
                                  const uint16_t *list, uint64_t length);
/*
 * Given a bitset, set all bit values in the list (there
 * are length of them).
 */
void bitset_set_list(uint64_t *words, const uint16_t *list, uint64_t length);

/*
 * Given a bitset having cardinality card, unset all bit values in the list
 * (there are length of them)
 * and return the updated cardinality. This evidently assumes that the bitset
 * already contained data.
 */
uint64_t bitset_clear_list(uint64_t *words, uint64_t card, const uint16_t *list,
                           uint64_t length);

/*
 * Given a bitset having cardinality card, toggle all bit values in the list
 * (there are length of them)
 * and return the updated cardinality. This evidently assumes that the bitset
 * already contained data.
 */

uint64_t bitset_flip_list_withcard(uint64_t *words, uint64_t card,
                                   const uint16_t *list, uint64_t length);

void bitset_flip_list(uint64_t *words, const uint16_t *list, uint64_t length);

#if CROARING_IS_X64
/***
 * BEGIN Harley-Seal popcount functions.
 */
CROARING_TARGET_AVX2
/**
 * Compute the population count of a 256-bit word
 * This is not especially fast, but it is convenient as part of other functions.
 */
static inline __m256i popcount256(__m256i v) {
    const __m256i lookuppos = _mm256_setr_epi8(
        /* 0 */ 4 + 0, /* 1 */ 4 + 1, /* 2 */ 4 + 1, /* 3 */ 4 + 2,
        /* 4 */ 4 + 1, /* 5 */ 4 + 2, /* 6 */ 4 + 2, /* 7 */ 4 + 3,
        /* 8 */ 4 + 1, /* 9 */ 4 + 2, /* a */ 4 + 2, /* b */ 4 + 3,
        /* c */ 4 + 2, /* d */ 4 + 3, /* e */ 4 + 3, /* f */ 4 + 4,

        /* 0 */ 4 + 0, /* 1 */ 4 + 1, /* 2 */ 4 + 1, /* 3 */ 4 + 2,
        /* 4 */ 4 + 1, /* 5 */ 4 + 2, /* 6 */ 4 + 2, /* 7 */ 4 + 3,
        /* 8 */ 4 + 1, /* 9 */ 4 + 2, /* a */ 4 + 2, /* b */ 4 + 3,
        /* c */ 4 + 2, /* d */ 4 + 3, /* e */ 4 + 3, /* f */ 4 + 4);
    const __m256i lookupneg = _mm256_setr_epi8(
        /* 0 */ 4 - 0, /* 1 */ 4 - 1, /* 2 */ 4 - 1, /* 3 */ 4 - 2,
        /* 4 */ 4 - 1, /* 5 */ 4 - 2, /* 6 */ 4 - 2, /* 7 */ 4 - 3,
        /* 8 */ 4 - 1, /* 9 */ 4 - 2, /* a */ 4 - 2, /* b */ 4 - 3,
        /* c */ 4 - 2, /* d */ 4 - 3, /* e */ 4 - 3, /* f */ 4 - 4,

        /* 0 */ 4 - 0, /* 1 */ 4 - 1, /* 2 */ 4 - 1, /* 3 */ 4 - 2,
        /* 4 */ 4 - 1, /* 5 */ 4 - 2, /* 6 */ 4 - 2, /* 7 */ 4 - 3,
        /* 8 */ 4 - 1, /* 9 */ 4 - 2, /* a */ 4 - 2, /* b */ 4 - 3,
        /* c */ 4 - 2, /* d */ 4 - 3, /* e */ 4 - 3, /* f */ 4 - 4);
    const __m256i low_mask = _mm256_set1_epi8(0x0f);

    const __m256i lo = _mm256_and_si256(v, low_mask);
    const __m256i hi = _mm256_and_si256(_mm256_srli_epi16(v, 4), low_mask);
    const __m256i popcnt1 = _mm256_shuffle_epi8(lookuppos, lo);
    const __m256i popcnt2 = _mm256_shuffle_epi8(lookupneg, hi);
    return _mm256_sad_epu8(popcnt1, popcnt2);
}
CROARING_UNTARGET_AVX2

CROARING_TARGET_AVX2
/**
 * Simple CSA over 256 bits
 */
static inline void CSA(__m256i *h, __m256i *l, __m256i a, __m256i b,
                       __m256i c) {
    const __m256i u = _mm256_xor_si256(a, b);
    *h = _mm256_or_si256(_mm256_and_si256(a, b), _mm256_and_si256(u, c));
    *l = _mm256_xor_si256(u, c);
}
CROARING_UNTARGET_AVX2

CROARING_TARGET_AVX2
/**
 * Fast Harley-Seal AVX population count function
 */
inline static uint64_t avx2_harley_seal_popcount256(const __m256i *data,
                                                    const uint64_t size) {
    __m256i total = _mm256_setzero_si256();
    __m256i ones = _mm256_setzero_si256();
    __m256i twos = _mm256_setzero_si256();
    __m256i fours = _mm256_setzero_si256();
    __m256i eights = _mm256_setzero_si256();
    __m256i sixteens = _mm256_setzero_si256();
    __m256i twosA, twosB, foursA, foursB, eightsA, eightsB;

    const uint64_t limit = size - size % 16;
    uint64_t i = 0;

    for (; i < limit; i += 16) {
        CSA(&twosA, &ones, ones, _mm256_lddqu_si256(data + i),
            _mm256_lddqu_si256(data + i + 1));
        CSA(&twosB, &ones, ones, _mm256_lddqu_si256(data + i + 2),
            _mm256_lddqu_si256(data + i + 3));
        CSA(&foursA, &twos, twos, twosA, twosB);
        CSA(&twosA, &ones, ones, _mm256_lddqu_si256(data + i + 4),
            _mm256_lddqu_si256(data + i + 5));
        CSA(&twosB, &ones, ones, _mm256_lddqu_si256(data + i + 6),
            _mm256_lddqu_si256(data + i + 7));
        CSA(&foursB, &twos, twos, twosA, twosB);
        CSA(&eightsA, &fours, fours, foursA, foursB);
        CSA(&twosA, &ones, ones, _mm256_lddqu_si256(data + i + 8),
            _mm256_lddqu_si256(data + i + 9));
        CSA(&twosB, &ones, ones, _mm256_lddqu_si256(data + i + 10),
            _mm256_lddqu_si256(data + i + 11));
        CSA(&foursA, &twos, twos, twosA, twosB);
        CSA(&twosA, &ones, ones, _mm256_lddqu_si256(data + i + 12),
            _mm256_lddqu_si256(data + i + 13));
        CSA(&twosB, &ones, ones, _mm256_lddqu_si256(data + i + 14),
            _mm256_lddqu_si256(data + i + 15));
        CSA(&foursB, &twos, twos, twosA, twosB);
        CSA(&eightsB, &fours, fours, foursA, foursB);
        CSA(&sixteens, &eights, eights, eightsA, eightsB);

        total = _mm256_add_epi64(total, popcount256(sixteens));
    }

    total = _mm256_slli_epi64(total, 4);  // * 16
    total = _mm256_add_epi64(
        total, _mm256_slli_epi64(popcount256(eights), 3));  // += 8 * ...
    total = _mm256_add_epi64(
        total, _mm256_slli_epi64(popcount256(fours), 2));  // += 4 * ...
    total = _mm256_add_epi64(
        total, _mm256_slli_epi64(popcount256(twos), 1));  // += 2 * ...
    total = _mm256_add_epi64(total, popcount256(ones));
    for (; i < size; i++)
        total =
            _mm256_add_epi64(total, popcount256(_mm256_lddqu_si256(data + i)));

    return (uint64_t)(_mm256_extract_epi64(total, 0)) +
           (uint64_t)(_mm256_extract_epi64(total, 1)) +
           (uint64_t)(_mm256_extract_epi64(total, 2)) +
           (uint64_t)(_mm256_extract_epi64(total, 3));
}
CROARING_UNTARGET_AVX2

#define AVXPOPCNTFNC(opname, avx_intrinsic)                                    \
    static inline uint64_t avx2_harley_seal_popcount256_##opname(              \
        const __m256i *data1, const __m256i *data2, const uint64_t size) {     \
        __m256i total = _mm256_setzero_si256();                                \
        __m256i ones = _mm256_setzero_si256();                                 \
        __m256i twos = _mm256_setzero_si256();                                 \
        __m256i fours = _mm256_setzero_si256();                                \
        __m256i eights = _mm256_setzero_si256();                               \
        __m256i sixteens = _mm256_setzero_si256();                             \
        __m256i twosA, twosB, foursA, foursB, eightsA, eightsB;                \
        __m256i A1, A2;                                                        \
        const uint64_t limit = size - size % 16;                               \
        uint64_t i = 0;                                                        \
        for (; i < limit; i += 16) {                                           \
            A1 = avx_intrinsic(_mm256_lddqu_si256(data1 + i),                  \
                               _mm256_lddqu_si256(data2 + i));                 \
            A2 = avx_intrinsic(_mm256_lddqu_si256(data1 + i + 1),              \
                               _mm256_lddqu_si256(data2 + i + 1));             \
            CSA(&twosA, &ones, ones, A1, A2);                                  \
            A1 = avx_intrinsic(_mm256_lddqu_si256(data1 + i + 2),              \
                               _mm256_lddqu_si256(data2 + i + 2));             \
            A2 = avx_intrinsic(_mm256_lddqu_si256(data1 + i + 3),              \
                               _mm256_lddqu_si256(data2 + i + 3));             \
            CSA(&twosB, &ones, ones, A1, A2);                                  \
            CSA(&foursA, &twos, twos, twosA, twosB);                           \
            A1 = avx_intrinsic(_mm256_lddqu_si256(data1 + i + 4),              \
                               _mm256_lddqu_si256(data2 + i + 4));             \
            A2 = avx_intrinsic(_mm256_lddqu_si256(data1 + i + 5),              \
                               _mm256_lddqu_si256(data2 + i + 5));             \
            CSA(&twosA, &ones, ones, A1, A2);                                  \
            A1 = avx_intrinsic(_mm256_lddqu_si256(data1 + i + 6),              \
                               _mm256_lddqu_si256(data2 + i + 6));             \
            A2 = avx_intrinsic(_mm256_lddqu_si256(data1 + i + 7),              \
                               _mm256_lddqu_si256(data2 + i + 7));             \
            CSA(&twosB, &ones, ones, A1, A2);                                  \
            CSA(&foursB, &twos, twos, twosA, twosB);                           \
            CSA(&eightsA, &fours, fours, foursA, foursB);                      \
            A1 = avx_intrinsic(_mm256_lddqu_si256(data1 + i + 8),              \
                               _mm256_lddqu_si256(data2 + i + 8));             \
            A2 = avx_intrinsic(_mm256_lddqu_si256(data1 + i + 9),              \
                               _mm256_lddqu_si256(data2 + i + 9));             \
            CSA(&twosA, &ones, ones, A1, A2);                                  \
            A1 = avx_intrinsic(_mm256_lddqu_si256(data1 + i + 10),             \
                               _mm256_lddqu_si256(data2 + i + 10));            \
            A2 = avx_intrinsic(_mm256_lddqu_si256(data1 + i + 11),             \
                               _mm256_lddqu_si256(data2 + i + 11));            \
            CSA(&twosB, &ones, ones, A1, A2);                                  \
            CSA(&foursA, &twos, twos, twosA, twosB);                           \
            A1 = avx_intrinsic(_mm256_lddqu_si256(data1 + i + 12),             \
                               _mm256_lddqu_si256(data2 + i + 12));            \
            A2 = avx_intrinsic(_mm256_lddqu_si256(data1 + i + 13),             \
                               _mm256_lddqu_si256(data2 + i + 13));            \
            CSA(&twosA, &ones, ones, A1, A2);                                  \
            A1 = avx_intrinsic(_mm256_lddqu_si256(data1 + i + 14),             \
                               _mm256_lddqu_si256(data2 + i + 14));            \
            A2 = avx_intrinsic(_mm256_lddqu_si256(data1 + i + 15),             \
                               _mm256_lddqu_si256(data2 + i + 15));            \
            CSA(&twosB, &ones, ones, A1, A2);                                  \
            CSA(&foursB, &twos, twos, twosA, twosB);                           \
            CSA(&eightsB, &fours, fours, foursA, foursB);                      \
            CSA(&sixteens, &eights, eights, eightsA, eightsB);                 \
            total = _mm256_add_epi64(total, popcount256(sixteens));            \
        }                                                                      \
        total = _mm256_slli_epi64(total, 4);                                   \
        total = _mm256_add_epi64(total,                                        \
                                 _mm256_slli_epi64(popcount256(eights), 3));   \
        total =                                                                \
            _mm256_add_epi64(total, _mm256_slli_epi64(popcount256(fours), 2)); \
        total =                                                                \
            _mm256_add_epi64(total, _mm256_slli_epi64(popcount256(twos), 1));  \
        total = _mm256_add_epi64(total, popcount256(ones));                    \
        for (; i < size; i++) {                                                \
            A1 = avx_intrinsic(_mm256_lddqu_si256(data1 + i),                  \
                               _mm256_lddqu_si256(data2 + i));                 \
            total = _mm256_add_epi64(total, popcount256(A1));                  \
        }                                                                      \
        return (uint64_t)(_mm256_extract_epi64(total, 0)) +                    \
               (uint64_t)(_mm256_extract_epi64(total, 1)) +                    \
               (uint64_t)(_mm256_extract_epi64(total, 2)) +                    \
               (uint64_t)(_mm256_extract_epi64(total, 3));                     \
    }                                                                          \
    static inline uint64_t avx2_harley_seal_popcount256andstore_##opname(      \
        const __m256i *__restrict__ data1, const __m256i *__restrict__ data2,  \
        __m256i *__restrict__ out, const uint64_t size) {                      \
        __m256i total = _mm256_setzero_si256();                                \
        __m256i ones = _mm256_setzero_si256();                                 \
        __m256i twos = _mm256_setzero_si256();                                 \
        __m256i fours = _mm256_setzero_si256();                                \
        __m256i eights = _mm256_setzero_si256();                               \
        __m256i sixteens = _mm256_setzero_si256();                             \
        __m256i twosA, twosB, foursA, foursB, eightsA, eightsB;                \
        __m256i A1, A2;                                                        \
        const uint64_t limit = size - size % 16;                               \
        uint64_t i = 0;                                                        \
        for (; i < limit; i += 16) {                                           \
            A1 = avx_intrinsic(_mm256_lddqu_si256(data1 + i),                  \
                               _mm256_lddqu_si256(data2 + i));                 \
            _mm256_storeu_si256(out + i, A1);                                  \
            A2 = avx_intrinsic(_mm256_lddqu_si256(data1 + i + 1),              \
                               _mm256_lddqu_si256(data2 + i + 1));             \
            _mm256_storeu_si256(out + i + 1, A2);                              \
            CSA(&twosA, &ones, ones, A1, A2);                                  \
            A1 = avx_intrinsic(_mm256_lddqu_si256(data1 + i + 2),              \
                               _mm256_lddqu_si256(data2 + i + 2));             \
            _mm256_storeu_si256(out + i + 2, A1);                              \
            A2 = avx_intrinsic(_mm256_lddqu_si256(data1 + i + 3),              \
                               _mm256_lddqu_si256(data2 + i + 3));             \
            _mm256_storeu_si256(out + i + 3, A2);                              \
            CSA(&twosB, &ones, ones, A1, A2);                                  \
            CSA(&foursA, &twos, twos, twosA, twosB);                           \
            A1 = avx_intrinsic(_mm256_lddqu_si256(data1 + i + 4),              \
                               _mm256_lddqu_si256(data2 + i + 4));             \
            _mm256_storeu_si256(out + i + 4, A1);                              \
            A2 = avx_intrinsic(_mm256_lddqu_si256(data1 + i + 5),              \
                               _mm256_lddqu_si256(data2 + i + 5));             \
            _mm256_storeu_si256(out + i + 5, A2);                              \
            CSA(&twosA, &ones, ones, A1, A2);                                  \
            A1 = avx_intrinsic(_mm256_lddqu_si256(data1 + i + 6),              \
                               _mm256_lddqu_si256(data2 + i + 6));             \
            _mm256_storeu_si256(out + i + 6, A1);                              \
            A2 = avx_intrinsic(_mm256_lddqu_si256(data1 + i + 7),              \
                               _mm256_lddqu_si256(data2 + i + 7));             \
            _mm256_storeu_si256(out + i + 7, A2);                              \
            CSA(&twosB, &ones, ones, A1, A2);                                  \
            CSA(&foursB, &twos, twos, twosA, twosB);                           \
            CSA(&eightsA, &fours, fours, foursA, foursB);                      \
            A1 = avx_intrinsic(_mm256_lddqu_si256(data1 + i + 8),              \
                               _mm256_lddqu_si256(data2 + i + 8));             \
            _mm256_storeu_si256(out + i + 8, A1);                              \
            A2 = avx_intrinsic(_mm256_lddqu_si256(data1 + i + 9),              \
                               _mm256_lddqu_si256(data2 + i + 9));             \
            _mm256_storeu_si256(out + i + 9, A2);                              \
            CSA(&twosA, &ones, ones, A1, A2);                                  \
            A1 = avx_intrinsic(_mm256_lddqu_si256(data1 + i + 10),             \
                               _mm256_lddqu_si256(data2 + i + 10));            \
            _mm256_storeu_si256(out + i + 10, A1);                             \
            A2 = avx_intrinsic(_mm256_lddqu_si256(data1 + i + 11),             \
                               _mm256_lddqu_si256(data2 + i + 11));            \
            _mm256_storeu_si256(out + i + 11, A2);                             \
            CSA(&twosB, &ones, ones, A1, A2);                                  \
            CSA(&foursA, &twos, twos, twosA, twosB);                           \
            A1 = avx_intrinsic(_mm256_lddqu_si256(data1 + i + 12),             \
                               _mm256_lddqu_si256(data2 + i + 12));            \
            _mm256_storeu_si256(out + i + 12, A1);                             \
            A2 = avx_intrinsic(_mm256_lddqu_si256(data1 + i + 13),             \
                               _mm256_lddqu_si256(data2 + i + 13));            \
            _mm256_storeu_si256(out + i + 13, A2);                             \
            CSA(&twosA, &ones, ones, A1, A2);                                  \
            A1 = avx_intrinsic(_mm256_lddqu_si256(data1 + i + 14),             \
                               _mm256_lddqu_si256(data2 + i + 14));            \
            _mm256_storeu_si256(out + i + 14, A1);                             \
            A2 = avx_intrinsic(_mm256_lddqu_si256(data1 + i + 15),             \
                               _mm256_lddqu_si256(data2 + i + 15));            \
            _mm256_storeu_si256(out + i + 15, A2);                             \
            CSA(&twosB, &ones, ones, A1, A2);                                  \
            CSA(&foursB, &twos, twos, twosA, twosB);                           \
            CSA(&eightsB, &fours, fours, foursA, foursB);                      \
            CSA(&sixteens, &eights, eights, eightsA, eightsB);                 \
            total = _mm256_add_epi64(total, popcount256(sixteens));            \
        }                                                                      \
        total = _mm256_slli_epi64(total, 4);                                   \
        total = _mm256_add_epi64(total,                                        \
                                 _mm256_slli_epi64(popcount256(eights), 3));   \
        total =                                                                \
            _mm256_add_epi64(total, _mm256_slli_epi64(popcount256(fours), 2)); \
        total =                                                                \
            _mm256_add_epi64(total, _mm256_slli_epi64(popcount256(twos), 1));  \
        total = _mm256_add_epi64(total, popcount256(ones));                    \
        for (; i < size; i++) {                                                \
            A1 = avx_intrinsic(_mm256_lddqu_si256(data1 + i),                  \
                               _mm256_lddqu_si256(data2 + i));                 \
            _mm256_storeu_si256(out + i, A1);                                  \
            total = _mm256_add_epi64(total, popcount256(A1));                  \
        }                                                                      \
        return (uint64_t)(_mm256_extract_epi64(total, 0)) +                    \
               (uint64_t)(_mm256_extract_epi64(total, 1)) +                    \
               (uint64_t)(_mm256_extract_epi64(total, 2)) +                    \
               (uint64_t)(_mm256_extract_epi64(total, 3));                     \
    }

CROARING_TARGET_AVX2
AVXPOPCNTFNC(or, _mm256_or_si256)
CROARING_UNTARGET_AVX2

CROARING_TARGET_AVX2
AVXPOPCNTFNC(union, _mm256_or_si256)
CROARING_UNTARGET_AVX2

CROARING_TARGET_AVX2
AVXPOPCNTFNC(and, _mm256_and_si256)
CROARING_UNTARGET_AVX2

CROARING_TARGET_AVX2
AVXPOPCNTFNC(intersection, _mm256_and_si256)
CROARING_UNTARGET_AVX2

CROARING_TARGET_AVX2
AVXPOPCNTFNC (xor, _mm256_xor_si256)
CROARING_UNTARGET_AVX2

CROARING_TARGET_AVX2
AVXPOPCNTFNC(andnot, _mm256_andnot_si256)
CROARING_UNTARGET_AVX2


#define VPOPCNT_AND_ADD(ptr, i, accu)   \
    const __m512i v##i = _mm512_loadu_si512((const __m512i*)ptr + i);  \
    const __m512i p##i = _mm512_popcnt_epi64(v##i);    \
    accu = _mm512_add_epi64(accu, p##i);  

#if CROARING_COMPILER_SUPPORTS_AVX512
CROARING_TARGET_AVX512
static inline uint64_t sum_epu64_256(const __m256i v) {

    return (uint64_t)(_mm256_extract_epi64(v, 0))
         + (uint64_t)(_mm256_extract_epi64(v, 1))
         + (uint64_t)(_mm256_extract_epi64(v, 2))
         + (uint64_t)(_mm256_extract_epi64(v, 3));
}


static inline uint64_t simd_sum_epu64(const __m512i v) {

     __m256i lo = _mm512_extracti64x4_epi64(v, 0);
     __m256i hi = _mm512_extracti64x4_epi64(v, 1);

    return sum_epu64_256(lo) + sum_epu64_256(hi);
}

static inline uint64_t avx512_vpopcount(const __m512i* data, const uint64_t size)
{
    const uint64_t limit = size - size % 4;
    __m512i total = _mm512_setzero_si512();
    uint64_t i = 0;

    for (; i < limit; i += 4)
    {    
        VPOPCNT_AND_ADD(data + i, 0, total);
        VPOPCNT_AND_ADD(data + i, 1, total);
        VPOPCNT_AND_ADD(data + i, 2, total);
        VPOPCNT_AND_ADD(data + i, 3, total);
    }
    
    for (; i < size; i++)
    {
        total = _mm512_add_epi64(total, _mm512_popcnt_epi64(_mm512_loadu_si512(data + i)));
    }
        
    return simd_sum_epu64(total);
}
CROARING_UNTARGET_AVX512
#endif

#define AVXPOPCNTFNC512(opname, avx_intrinsic)                                 \
    static inline uint64_t avx512_harley_seal_popcount512_##opname(            \
        const __m512i *data1, const __m512i *data2, const uint64_t size) {     \
        __m512i total = _mm512_setzero_si512();                                \
        const uint64_t limit = size - size % 4;                                \
        uint64_t i = 0;                                                        \
	    for (; i < limit; i += 4) {                                            \
            __m512i a1 = avx_intrinsic(_mm512_loadu_si512(data1 + i),          \
                                       _mm512_loadu_si512(data2 + i));         \
            total = _mm512_add_epi64(total, _mm512_popcnt_epi64(a1));          \
            __m512i a2 = avx_intrinsic(_mm512_loadu_si512(data1 + i + 1),      \
                                       _mm512_loadu_si512(data2 + i + 1));     \
            total = _mm512_add_epi64(total, _mm512_popcnt_epi64(a2));          \
             __m512i a3 = avx_intrinsic(_mm512_loadu_si512(data1 + i + 2),     \
                                       _mm512_loadu_si512(data2 + i + 2));     \
            total = _mm512_add_epi64(total, _mm512_popcnt_epi64(a3));          \
             __m512i a4 = avx_intrinsic(_mm512_loadu_si512(data1 + i + 3),     \
                                       _mm512_loadu_si512(data2 + i + 3));     \
            total = _mm512_add_epi64(total, _mm512_popcnt_epi64(a4));          \
       }                                                                       \
       for(; i < size; i++) {                                                  \
              __m512i a = avx_intrinsic(_mm512_loadu_si512(data1 + i),         \
                       _mm512_loadu_si512(data2 + i));                         \
              total = _mm512_add_epi64(total, _mm512_popcnt_epi64(a));         \
        }                                                                      \
        return simd_sum_epu64(total);                                          \
    }                                                                          \
    static inline uint64_t avx512_harley_seal_popcount512andstore_##opname(    \
        const __m512i *__restrict__ data1, const __m512i *__restrict__ data2,  \
        __m512i *__restrict__ out, const uint64_t size) {                      \
        __m512i total = _mm512_setzero_si512();                                \
        const uint64_t limit = size - size % 4;                                \
        uint64_t i = 0;                                                        \
	    for (; i < limit; i += 4) {                                        \
            __m512i a1 = avx_intrinsic(_mm512_loadu_si512(data1 + i),          \
                                       _mm512_loadu_si512(data2 + i));         \
            _mm512_storeu_si512(out + i, a1);                                  \
            total = _mm512_add_epi64(total, _mm512_popcnt_epi64(a1));          \
            __m512i a2 = avx_intrinsic(_mm512_loadu_si512(data1 + i + 1),      \
                                       _mm512_loadu_si512(data2 + i + 1));     \
            _mm512_storeu_si512(out + i + 1, a2);                              \
            total = _mm512_add_epi64(total, _mm512_popcnt_epi64(a2));          \
             __m512i a3 = avx_intrinsic(_mm512_loadu_si512(data1 + i + 2),     \
                                       _mm512_loadu_si512(data2 + i + 2));     \
            _mm512_storeu_si512(out + i + 2, a3);                              \
            total = _mm512_add_epi64(total, _mm512_popcnt_epi64(a3));          \
            __m512i a4 = avx_intrinsic(_mm512_loadu_si512(data1 + i + 3),      \
                                       _mm512_loadu_si512(data2 + i + 3));     \
            _mm512_storeu_si512(out + i + 3, a4);                              \
            total = _mm512_add_epi64(total, _mm512_popcnt_epi64(a4));          \
       }                                                                       \
       for(; i < size; i++) {                                                  \
              __m512i a = avx_intrinsic(_mm512_loadu_si512(data1 + i),         \
                       _mm512_loadu_si512(data2 + i));                         \
            _mm512_storeu_si512(out + i, a);                                   \
 	       total = _mm512_add_epi64(total, _mm512_popcnt_epi64(a));        \
        }                                                                      \
        return simd_sum_epu64(total);                                          \
    }                                                                          \

#if CROARING_COMPILER_SUPPORTS_AVX512
CROARING_TARGET_AVX512
AVXPOPCNTFNC512(or, _mm512_or_si512)
AVXPOPCNTFNC512(union, _mm512_or_si512)
AVXPOPCNTFNC512(and, _mm512_and_si512)
AVXPOPCNTFNC512(intersection, _mm512_and_si512)
AVXPOPCNTFNC512(xor, _mm512_xor_si512)
AVXPOPCNTFNC512(andnot, _mm512_andnot_si512)
CROARING_UNTARGET_AVX512
#endif
/***
 * END Harley-Seal popcount functions.
 */

#endif  // CROARING_IS_X64

#ifdef __cplusplus
} } }  // extern "C" { namespace roaring { namespace internal
#endif

#endif
/* end file include/roaring/bitset_util.h */
/* begin file include/roaring/containers/array.h */
/*
 * array.h
 *
 */

#ifndef INCLUDE_CONTAINERS_ARRAY_H_
#define INCLUDE_CONTAINERS_ARRAY_H_

#include <string.h>



#ifdef __cplusplus
extern "C" { namespace roaring {

// Note: in pure C++ code, you should avoid putting `using` in header files
using api::roaring_iterator;
using api::roaring_iterator64;

namespace internal {
#endif

/* Containers with DEFAULT_MAX_SIZE or less integers should be arrays */
enum { DEFAULT_MAX_SIZE = 4096 };

/* struct array_container - sparse representation of a bitmap
 *
 * @cardinality: number of indices in `array` (and the bitmap)
 * @capacity:    allocated size of `array`
 * @array:       sorted list of integers
 */
STRUCT_CONTAINER(array_container_s) {
    int32_t cardinality;
    int32_t capacity;
    uint16_t *array;
};

typedef struct array_container_s array_container_t;

#define CAST_array(c)         CAST(array_container_t *, c)  // safer downcast
#define const_CAST_array(c)   CAST(const array_container_t *, c)
#define movable_CAST_array(c) movable_CAST(array_container_t **, c)

/* Create a new array with default. Return NULL in case of failure. See also
 * array_container_create_given_capacity. */
array_container_t *array_container_create(void);

/* Create a new array with a specified capacity size. Return NULL in case of
 * failure. */
array_container_t *array_container_create_given_capacity(int32_t size);

/* Create a new array containing all values in [min,max). */
array_container_t * array_container_create_range(uint32_t min, uint32_t max);

/*
 * Shrink the capacity to the actual size, return the number of bytes saved.
 */
int array_container_shrink_to_fit(array_container_t *src);

/* Free memory owned by `array'. */
void array_container_free(array_container_t *array);

/* Duplicate container */
array_container_t *array_container_clone(const array_container_t *src);

/* Get the cardinality of `array'. */
ALLOW_UNALIGNED
static inline int array_container_cardinality(const array_container_t *array) {
    return array->cardinality;
}

static inline bool array_container_nonzero_cardinality(
    const array_container_t *array) {
    return array->cardinality > 0;
}

/* Copy one container into another. We assume that they are distinct. */
void array_container_copy(const array_container_t *src, array_container_t *dst);

/*  Add all the values in [min,max) (included) at a distance k*step from min.
    The container must have a size less or equal to DEFAULT_MAX_SIZE after this
   addition. */
void array_container_add_from_range(array_container_t *arr, uint32_t min,
                                    uint32_t max, uint16_t step);


static inline bool array_container_empty(const array_container_t *array) {
    return array->cardinality == 0;
}

/* check whether the cardinality is equal to the capacity (this does not mean
* that it contains 1<<16 elements) */
static inline bool array_container_full(const array_container_t *array) {
    return array->cardinality == array->capacity;
}


/* Compute the union of `src_1' and `src_2' and write the result to `dst'
 * It is assumed that `dst' is distinct from both `src_1' and `src_2'. */
void array_container_union(const array_container_t *src_1,
                           const array_container_t *src_2,
                           array_container_t *dst);

/* symmetric difference, see array_container_union */
void array_container_xor(const array_container_t *array_1,
                         const array_container_t *array_2,
                         array_container_t *out);

/* Computes the intersection of src_1 and src_2 and write the result to
 * dst. It is assumed that dst is distinct from both src_1 and src_2. */
void array_container_intersection(const array_container_t *src_1,
                                  const array_container_t *src_2,
                                  array_container_t *dst);

/* Check whether src_1 and src_2 intersect. */
bool array_container_intersect(const array_container_t *src_1,
                                  const array_container_t *src_2);


/* computers the size of the intersection between two arrays.
 */
int array_container_intersection_cardinality(const array_container_t *src_1,
                                             const array_container_t *src_2);

/* computes the intersection of array1 and array2 and write the result to
 * array1.
 * */
void array_container_intersection_inplace(array_container_t *src_1,
                                          const array_container_t *src_2);

/*
 * Write out the 16-bit integers contained in this container as a list of 32-bit
 * integers using base
 * as the starting value (it might be expected that base has zeros in its 16
 * least significant bits).
 * The function returns the number of values written.
 * The caller is responsible for allocating enough memory in out.
 */
int array_container_to_uint32_array(void *vout, const array_container_t *cont,
                                    uint32_t base);

/* Compute the number of runs */
int32_t array_container_number_of_runs(const array_container_t *ac);

/*
 * Print this container using printf (useful for debugging).
 */
void array_container_printf(const array_container_t *v);

/*
 * Print this container using printf as a comma-separated list of 32-bit
 * integers starting at base.
 */
void array_container_printf_as_uint32_array(const array_container_t *v,
                                            uint32_t base);

/**
 * Return the serialized size in bytes of a container having cardinality "card".
 */
static inline int32_t array_container_serialized_size_in_bytes(int32_t card) {
    return card * 2 + 2;
}

/**
 * Increase capacity to at least min.
 * Whether the existing data needs to be copied over depends on the "preserve"
 * parameter. If preserve is false, then the new content will be uninitialized,
 * otherwise the old content is copied.
 */
void array_container_grow(array_container_t *container, int32_t min,
                          bool preserve);

bool array_container_iterate(const array_container_t *cont, uint32_t base,
                             roaring_iterator iterator, void *ptr);
bool array_container_iterate64(const array_container_t *cont, uint32_t base,
                               roaring_iterator64 iterator, uint64_t high_bits,
                               void *ptr);

/**
 * Writes the underlying array to buf, outputs how many bytes were written.
 * This is meant to be byte-by-byte compatible with the Java and Go versions of
 * Roaring.
 * The number of bytes written should be
 * array_container_size_in_bytes(container).
 *
 */
int32_t array_container_write(const array_container_t *container, char *buf);
/**
 * Reads the instance from buf, outputs how many bytes were read.
 * This is meant to be byte-by-byte compatible with the Java and Go versions of
 * Roaring.
 * The number of bytes read should be array_container_size_in_bytes(container).
 * You need to provide the (known) cardinality.
 */
int32_t array_container_read(int32_t cardinality, array_container_t *container,
                             const char *buf);

/**
 * Return the serialized size in bytes of a container (see
 * bitset_container_write)
 * This is meant to be compatible with the Java and Go versions of Roaring and
 * assumes
 * that the cardinality of the container is already known.
 *
 */
static inline int32_t array_container_size_in_bytes(
    const array_container_t *container) {
    return container->cardinality * sizeof(uint16_t);
}

/**
 * Return true if the two arrays have the same content.
 */
ALLOW_UNALIGNED
static inline bool array_container_equals(
    const array_container_t *container1,
    const array_container_t *container2) {

    if (container1->cardinality != container2->cardinality) {
        return false;
    }
    return memequals(container1->array, container2->array, container1->cardinality*2);
}

/**
 * Return true if container1 is a subset of container2.
 */
bool array_container_is_subset(const array_container_t *container1,
                               const array_container_t *container2);

/**
 * If the element of given rank is in this container, supposing that the first
 * element has rank start_rank, then the function returns true and sets element
 * accordingly.
 * Otherwise, it returns false and update start_rank.
 */
static inline bool array_container_select(const array_container_t *container,
                                          uint32_t *start_rank, uint32_t rank,
                                          uint32_t *element) {
    int card = array_container_cardinality(container);
    if (*start_rank + card <= rank) {
        *start_rank += card;
        return false;
    } else {
        *element = container->array[rank - *start_rank];
        return true;
    }
}

/* Computes the  difference of array1 and array2 and write the result
 * to array out.
 * Array out does not need to be distinct from array_1
 */
void array_container_andnot(const array_container_t *array_1,
                            const array_container_t *array_2,
                            array_container_t *out);

/* Append x to the set. Assumes that the value is larger than any preceding
 * values.  */
static inline void array_container_append(array_container_t *arr,
                                          uint16_t pos) {
    const int32_t capacity = arr->capacity;

    if (array_container_full(arr)) {
        array_container_grow(arr, capacity + 1, true);
    }

    arr->array[arr->cardinality++] = pos;
}

/**
 * Add value to the set if final cardinality doesn't exceed max_cardinality.
 * Return code:
 * 1  -- value was added
 * 0  -- value was already present
 * -1 -- value was not added because cardinality would exceed max_cardinality
 */
static inline int array_container_try_add(array_container_t *arr, uint16_t value,
                                          int32_t max_cardinality) {
    const int32_t cardinality = arr->cardinality;

    // best case, we can append.
    if ((array_container_empty(arr) || arr->array[cardinality - 1] < value) &&
        cardinality < max_cardinality) {
        array_container_append(arr, value);
        return 1;
    }

    const int32_t loc = binarySearch(arr->array, cardinality, value);

    if (loc >= 0) {
        return 0;
    } else if (cardinality < max_cardinality) {
        if (array_container_full(arr)) {
            array_container_grow(arr, arr->capacity + 1, true);
        }
        const int32_t insert_idx = -loc - 1;
        memmove(arr->array + insert_idx + 1, arr->array + insert_idx,
                (cardinality - insert_idx) * sizeof(uint16_t));
        arr->array[insert_idx] = value;
        arr->cardinality++;
        return 1;
    } else {
        return -1;
    }
}

/* Add value to the set. Returns true if x was not already present.  */
static inline bool array_container_add(array_container_t *arr, uint16_t value) {
    return array_container_try_add(arr, value, INT32_MAX) == 1;
}

/* Remove x from the set. Returns true if x was present.  */
static inline bool array_container_remove(array_container_t *arr,
                                          uint16_t pos) {
    const int32_t idx = binarySearch(arr->array, arr->cardinality, pos);
    const bool is_present = idx >= 0;
    if (is_present) {
        memmove(arr->array + idx, arr->array + idx + 1,
                (arr->cardinality - idx - 1) * sizeof(uint16_t));
        arr->cardinality--;
    }

    return is_present;
}

/* Check whether x is present.  */
inline bool array_container_contains(const array_container_t *arr,
                                     uint16_t pos) {
    //    return binarySearch(arr->array, arr->cardinality, pos) >= 0;
    // binary search with fallback to linear search for short ranges
    int32_t low = 0;
    const uint16_t * carr = (const uint16_t *) arr->array;
    int32_t high = arr->cardinality - 1;
    //    while (high - low >= 0) {
    while(high >= low + 16) {
        int32_t middleIndex = (low + high)>>1;
        uint16_t middleValue = carr[middleIndex];
        if (middleValue < pos) {
            low = middleIndex + 1;
        } else if (middleValue > pos) {
            high = middleIndex - 1;
        } else {
            return true;
        }
    }

    for (int i=low; i <= high; i++) {
        uint16_t v = carr[i];
        if (v == pos) {
            return true;
        }
        if ( v > pos ) return false;
    }
    return false;

}

void array_container_offset(const array_container_t *c,
                            container_t **loc, container_t **hic,
                            uint16_t offset);

//* Check whether a range of values from range_start (included) to range_end (excluded) is present. */
static inline bool array_container_contains_range(const array_container_t *arr,
                                                    uint32_t range_start, uint32_t range_end) {
    const int32_t range_count = range_end - range_start;
    const uint16_t rs_included = range_start;
    const uint16_t re_included = range_end - 1;

    // Empty range is always included
    if (range_count <= 0) {
        return true;
    }
    if (range_count > arr->cardinality) {
        return false;
    }

    const int32_t start = binarySearch(arr->array, arr->cardinality, rs_included);
    // If this sorted array contains all items in the range:
    // * the start item must be found
    // * the last item in range range_count must exist, and be the expected end value
    return (start >= 0) && (arr->cardinality >= start + range_count) &&
           (arr->array[start + range_count - 1] == re_included);
}

/* Returns the smallest value (assumes not empty) */
inline uint16_t array_container_minimum(const array_container_t *arr) {
    if (arr->cardinality == 0) return 0;
    return arr->array[0];
}

/* Returns the largest value (assumes not empty) */
inline uint16_t array_container_maximum(const array_container_t *arr) {
    if (arr->cardinality == 0) return 0;
    return arr->array[arr->cardinality - 1];
}

/* Returns the number of values equal or smaller than x */
inline int array_container_rank(const array_container_t *arr, uint16_t x) {
    const int32_t idx = binarySearch(arr->array, arr->cardinality, x);
    const bool is_present = idx >= 0;
    if (is_present) {
        return idx + 1;
    } else {
        return -idx - 1;
    }
}

/* Returns the index of the first value equal or smaller than x, or -1 */
inline int array_container_index_equalorlarger(const array_container_t *arr, uint16_t x) {
    const int32_t idx = binarySearch(arr->array, arr->cardinality, x);
    const bool is_present = idx >= 0;
    if (is_present) {
        return idx;
    } else {
        int32_t candidate = - idx - 1;
        if(candidate < arr->cardinality) return candidate;
        return -1;
    }
}

/*
 * Adds all values in range [min,max] using hint:
 *   nvals_less is the number of array values less than $min
 *   nvals_greater is the number of array values greater than $max
 */
static inline void array_container_add_range_nvals(array_container_t *array,
                                                   uint32_t min, uint32_t max,
                                                   int32_t nvals_less,
                                                   int32_t nvals_greater) {
    int32_t union_cardinality = nvals_less + (max - min + 1) + nvals_greater;
    if (union_cardinality > array->capacity) {
        array_container_grow(array, union_cardinality, true);
    }
    memmove(&(array->array[union_cardinality - nvals_greater]),
            &(array->array[array->cardinality - nvals_greater]),
            nvals_greater * sizeof(uint16_t));
    for (uint32_t i = 0; i <= max - min; i++) {
        array->array[nvals_less + i] = min + i;
    }
    array->cardinality = union_cardinality;
}

/**
 * Adds all values in range [min,max]. This function is currently unused
 * and left as a documentation.
 */
/*static inline void array_container_add_range(array_container_t *array,
                                             uint32_t min, uint32_t max) {
    int32_t nvals_greater = count_greater(array->array, array->cardinality, max);
    int32_t nvals_less = count_less(array->array, array->cardinality - nvals_greater, min);
    array_container_add_range_nvals(array, min, max, nvals_less, nvals_greater);
}*/

/*
 * Removes all elements array[pos] .. array[pos+count-1]
 */
static inline void array_container_remove_range(array_container_t *array,
                                                uint32_t pos, uint32_t count) {
  if (count != 0) {
      memmove(&(array->array[pos]), &(array->array[pos+count]),
              (array->cardinality - pos - count) * sizeof(uint16_t));
      array->cardinality -= count;
  }
}

#ifdef __cplusplus
} } } // extern "C" { namespace roaring { namespace internal {
#endif

#endif /* INCLUDE_CONTAINERS_ARRAY_H_ */
/* end file include/roaring/containers/array.h */
/* begin file include/roaring/containers/bitset.h */
/*
 * bitset.h
 *
 */

#ifndef INCLUDE_CONTAINERS_BITSET_H_
#define INCLUDE_CONTAINERS_BITSET_H_

#include <stdbool.h>
#include <stdint.h>



#ifdef __cplusplus
extern "C" { namespace roaring {

// Note: in pure C++ code, you should avoid putting `using` in header files
using api::roaring_iterator;
using api::roaring_iterator64;

namespace internal {
#endif



enum {
    BITSET_CONTAINER_SIZE_IN_WORDS = (1 << 16) / 64,
    BITSET_UNKNOWN_CARDINALITY = -1
};

STRUCT_CONTAINER(bitset_container_s) {
    int32_t cardinality;
    uint64_t *words;
};

typedef struct bitset_container_s bitset_container_t;

#define CAST_bitset(c)         CAST(bitset_container_t *, c)  // safer downcast
#define const_CAST_bitset(c)   CAST(const bitset_container_t *, c)
#define movable_CAST_bitset(c) movable_CAST(bitset_container_t **, c)

/* Create a new bitset. Return NULL in case of failure. */
bitset_container_t *bitset_container_create(void);

/* Free memory. */
void bitset_container_free(bitset_container_t *bitset);

/* Clear bitset (sets bits to 0). */
void bitset_container_clear(bitset_container_t *bitset);

/* Set all bits to 1. */
void bitset_container_set_all(bitset_container_t *bitset);

/* Duplicate bitset */
bitset_container_t *bitset_container_clone(const bitset_container_t *src);

/* Set the bit in [begin,end). WARNING: as of April 2016, this method is slow
 * and
 * should not be used in performance-sensitive code. Ever.  */
void bitset_container_set_range(bitset_container_t *bitset, uint32_t begin,
                                uint32_t end);

#if defined(CROARING_ASMBITMANIPOPTIMIZATION) && defined(__AVX2__)
/* Set the ith bit.  */
static inline void bitset_container_set(bitset_container_t *bitset,
                                        uint16_t pos) {
    uint64_t shift = 6;
    uint64_t offset;
    uint64_t p = pos;
    ASM_SHIFT_RIGHT(p, shift, offset);
    uint64_t load = bitset->words[offset];
    ASM_SET_BIT_INC_WAS_CLEAR(load, p, bitset->cardinality);
    bitset->words[offset] = load;
}

/* Unset the ith bit. Currently unused. Could be used for optimization. */
/*static inline void bitset_container_unset(bitset_container_t *bitset,
                                          uint16_t pos) {
    uint64_t shift = 6;
    uint64_t offset;
    uint64_t p = pos;
    ASM_SHIFT_RIGHT(p, shift, offset);
    uint64_t load = bitset->words[offset];
    ASM_CLEAR_BIT_DEC_WAS_SET(load, p, bitset->cardinality);
    bitset->words[offset] = load;
}*/

/* Add `pos' to `bitset'. Returns true if `pos' was not present. Might be slower
 * than bitset_container_set.  */
static inline bool bitset_container_add(bitset_container_t *bitset,
                                        uint16_t pos) {
    uint64_t shift = 6;
    uint64_t offset;
    uint64_t p = pos;
    ASM_SHIFT_RIGHT(p, shift, offset);
    uint64_t load = bitset->words[offset];
    // could be possibly slightly further optimized
    const int32_t oldcard = bitset->cardinality;
    ASM_SET_BIT_INC_WAS_CLEAR(load, p, bitset->cardinality);
    bitset->words[offset] = load;
    return bitset->cardinality - oldcard;
}

/* Remove `pos' from `bitset'. Returns true if `pos' was present.  Might be
 * slower than bitset_container_unset.  */
static inline bool bitset_container_remove(bitset_container_t *bitset,
                                           uint16_t pos) {
    uint64_t shift = 6;
    uint64_t offset;
    uint64_t p = pos;
    ASM_SHIFT_RIGHT(p, shift, offset);
    uint64_t load = bitset->words[offset];
    // could be possibly slightly further optimized
    const int32_t oldcard = bitset->cardinality;
    ASM_CLEAR_BIT_DEC_WAS_SET(load, p, bitset->cardinality);
    bitset->words[offset] = load;
    return oldcard - bitset->cardinality;
}

/* Get the value of the ith bit.  */
inline bool bitset_container_get(const bitset_container_t *bitset,
                                 uint16_t pos) {
    uint64_t word = bitset->words[pos >> 6];
    const uint64_t p = pos;
    ASM_INPLACESHIFT_RIGHT(word, p);
    return word & 1;
}

#else

/* Set the ith bit.  */
static inline void bitset_container_set(bitset_container_t *bitset,
                                        uint16_t pos) {
    const uint64_t old_word = bitset->words[pos >> 6];
    const int index = pos & 63;
    const uint64_t new_word = old_word | (UINT64_C(1) << index);
    bitset->cardinality += (uint32_t)((old_word ^ new_word) >> index);
    bitset->words[pos >> 6] = new_word;
}

/* Unset the ith bit. Currently unused.  */
/*static inline void bitset_container_unset(bitset_container_t *bitset,
                                          uint16_t pos) {
    const uint64_t old_word = bitset->words[pos >> 6];
    const int index = pos & 63;
    const uint64_t new_word = old_word & (~(UINT64_C(1) << index));
    bitset->cardinality -= (uint32_t)((old_word ^ new_word) >> index);
    bitset->words[pos >> 6] = new_word;
}*/

/* Add `pos' to `bitset'. Returns true if `pos' was not present. Might be slower
 * than bitset_container_set.  */
static inline bool bitset_container_add(bitset_container_t *bitset,
                                        uint16_t pos) {
    const uint64_t old_word = bitset->words[pos >> 6];
    const int index = pos & 63;
    const uint64_t new_word = old_word | (UINT64_C(1) << index);
    const uint64_t increment = (old_word ^ new_word) >> index;
    bitset->cardinality += (uint32_t)increment;
    bitset->words[pos >> 6] = new_word;
    return increment > 0;
}

/* Remove `pos' from `bitset'. Returns true if `pos' was present.  Might be
 * slower than bitset_container_unset.  */
static inline bool bitset_container_remove(bitset_container_t *bitset,
                                           uint16_t pos) {
    const uint64_t old_word = bitset->words[pos >> 6];
    const int index = pos & 63;
    const uint64_t new_word = old_word & (~(UINT64_C(1) << index));
    const uint64_t increment = (old_word ^ new_word) >> index;
    bitset->cardinality -= (uint32_t)increment;
    bitset->words[pos >> 6] = new_word;
    return increment > 0;
}

/* Get the value of the ith bit.  */
inline bool bitset_container_get(const bitset_container_t *bitset,
                                 uint16_t pos) {
    const uint64_t word = bitset->words[pos >> 6];
    return (word >> (pos & 63)) & 1;
}

#endif

/*
* Check if all bits are set in a range of positions from pos_start (included) to
* pos_end (excluded).
*/
static inline bool bitset_container_get_range(const bitset_container_t *bitset,
                                                uint32_t pos_start, uint32_t pos_end) {

    const uint32_t start = pos_start >> 6;
    const uint32_t end = pos_end >> 6;

    const uint64_t first = ~((1ULL << (pos_start & 0x3F)) - 1);
    const uint64_t last = (1ULL << (pos_end & 0x3F)) - 1;

    if (start == end) return ((bitset->words[end] & first & last) == (first & last));
    if ((bitset->words[start] & first) != first) return false;

    if ((end < BITSET_CONTAINER_SIZE_IN_WORDS) && ((bitset->words[end] & last) != last)){

        return false;
    }

    for (uint16_t i = start + 1; (i < BITSET_CONTAINER_SIZE_IN_WORDS) && (i < end); ++i){

        if (bitset->words[i] != UINT64_C(0xFFFFFFFFFFFFFFFF)) return false;
    }

    return true;
}

/* Check whether `bitset' is present in `array'.  Calls bitset_container_get. */
inline bool bitset_container_contains(const bitset_container_t *bitset,
                                      uint16_t pos) {
    return bitset_container_get(bitset, pos);
}

/*
* Check whether a range of bits from position `pos_start' (included) to `pos_end' (excluded)
* is present in `bitset'.  Calls bitset_container_get_all.
*/
static inline bool bitset_container_contains_range(const bitset_container_t *bitset,
          uint32_t pos_start, uint32_t pos_end) {
    return bitset_container_get_range(bitset, pos_start, pos_end);
}

/* Get the number of bits set */
ALLOW_UNALIGNED
static inline int bitset_container_cardinality(
    const bitset_container_t *bitset) {
    return bitset->cardinality;
}




/* Copy one container into another. We assume that they are distinct. */
void bitset_container_copy(const bitset_container_t *source,
                           bitset_container_t *dest);

/*  Add all the values [min,max) at a distance k*step from min: min,
 * min+step,.... */
void bitset_container_add_from_range(bitset_container_t *bitset, uint32_t min,
                                     uint32_t max, uint16_t step);

/* Get the number of bits set (force computation). This does not modify bitset.
 * To update the cardinality, you should do
 * bitset->cardinality =  bitset_container_compute_cardinality(bitset).*/
int bitset_container_compute_cardinality(const bitset_container_t *bitset);

/* Check whether this bitset is empty,
 *  it never modifies the bitset struct. */
static inline bool bitset_container_empty(
    const bitset_container_t *bitset) {
  if (bitset->cardinality == BITSET_UNKNOWN_CARDINALITY) {
      for (int i = 0; i < BITSET_CONTAINER_SIZE_IN_WORDS; i ++) {
          if((bitset->words[i]) != 0) return false;
      }
      return true;
  }
  return bitset->cardinality == 0;
}


/* Get whether there is at least one bit set  (see bitset_container_empty for the reverse),
   the bitset is never modified */
static inline bool bitset_container_const_nonzero_cardinality(
    const bitset_container_t *bitset) {
    return !bitset_container_empty(bitset);
}

/*
 * Check whether the two bitsets intersect
 */
bool bitset_container_intersect(const bitset_container_t *src_1,
                                  const bitset_container_t *src_2);

/* Computes the union of bitsets `src_1' and `src_2' into `dst'  and return the
 * cardinality. */
int bitset_container_or(const bitset_container_t *src_1,
                        const bitset_container_t *src_2,
                        bitset_container_t *dst);

/* Computes the union of bitsets `src_1' and `src_2' and return the cardinality.
 */
int bitset_container_or_justcard(const bitset_container_t *src_1,
                                 const bitset_container_t *src_2);

/* Computes the union of bitsets `src_1' and `src_2' into `dst' and return the
 * cardinality. Same as bitset_container_or. */
int bitset_container_union(const bitset_container_t *src_1,
                           const bitset_container_t *src_2,
                           bitset_container_t *dst);

/* Computes the union of bitsets `src_1' and `src_2'  and return the
 * cardinality. Same as bitset_container_or_justcard. */
int bitset_container_union_justcard(const bitset_container_t *src_1,
                                    const bitset_container_t *src_2);

/* Computes the union of bitsets `src_1' and `src_2' into `dst', but does not
 * update the cardinality. Provided to optimize chained operations. */
int bitset_container_or_nocard(const bitset_container_t *src_1,
                               const bitset_container_t *src_2,
                               bitset_container_t *dst);

/* Computes the intersection of bitsets `src_1' and `src_2' into `dst' and
 * return the cardinality. */
int bitset_container_and(const bitset_container_t *src_1,
                         const bitset_container_t *src_2,
                         bitset_container_t *dst);

/* Computes the intersection of bitsets `src_1' and `src_2'  and return the
 * cardinality. */
int bitset_container_and_justcard(const bitset_container_t *src_1,
                                  const bitset_container_t *src_2);

/* Computes the intersection of bitsets `src_1' and `src_2' into `dst' and
 * return the cardinality. Same as bitset_container_and. */
int bitset_container_intersection(const bitset_container_t *src_1,
                                  const bitset_container_t *src_2,
                                  bitset_container_t *dst);

/* Computes the intersection of bitsets `src_1' and `src_2' and return the
 * cardinality. Same as bitset_container_and_justcard. */
int bitset_container_intersection_justcard(const bitset_container_t *src_1,
                                           const bitset_container_t *src_2);

/* Computes the intersection of bitsets `src_1' and `src_2' into `dst', but does
 * not update the cardinality. Provided to optimize chained operations. */
int bitset_container_and_nocard(const bitset_container_t *src_1,
                                const bitset_container_t *src_2,
                                bitset_container_t *dst);

/* Computes the exclusive or of bitsets `src_1' and `src_2' into `dst' and
 * return the cardinality. */
int bitset_container_xor(const bitset_container_t *src_1,
                         const bitset_container_t *src_2,
                         bitset_container_t *dst);

/* Computes the exclusive or of bitsets `src_1' and `src_2' and return the
 * cardinality. */
int bitset_container_xor_justcard(const bitset_container_t *src_1,
                                  const bitset_container_t *src_2);

/* Computes the exclusive or of bitsets `src_1' and `src_2' into `dst', but does
 * not update the cardinality. Provided to optimize chained operations. */
int bitset_container_xor_nocard(const bitset_container_t *src_1,
                                const bitset_container_t *src_2,
                                bitset_container_t *dst);

/* Computes the and not of bitsets `src_1' and `src_2' into `dst' and return the
 * cardinality. */
int bitset_container_andnot(const bitset_container_t *src_1,
                            const bitset_container_t *src_2,
                            bitset_container_t *dst);

/* Computes the and not of bitsets `src_1' and `src_2'  and return the
 * cardinality. */
int bitset_container_andnot_justcard(const bitset_container_t *src_1,
                                     const bitset_container_t *src_2);

/* Computes the and not or of bitsets `src_1' and `src_2' into `dst', but does
 * not update the cardinality. Provided to optimize chained operations. */
int bitset_container_andnot_nocard(const bitset_container_t *src_1,
                                   const bitset_container_t *src_2,
                                   bitset_container_t *dst);

void bitset_container_offset(const bitset_container_t *c,
                             container_t **loc, container_t **hic,
                             uint16_t offset);
/*
 * Write out the 16-bit integers contained in this container as a list of 32-bit
 * integers using base
 * as the starting value (it might be expected that base has zeros in its 16
 * least significant bits).
 * The function returns the number of values written.
 * The caller is responsible for allocating enough memory in out.
 * The out pointer should point to enough memory (the cardinality times 32
 * bits).
 */
int bitset_container_to_uint32_array(uint32_t *out,
                                     const bitset_container_t *bc,
                                     uint32_t base);

/*
 * Print this container using printf (useful for debugging).
 */
void bitset_container_printf(const bitset_container_t *v);

/*
 * Print this container using printf as a comma-separated list of 32-bit
 * integers starting at base.
 */
void bitset_container_printf_as_uint32_array(const bitset_container_t *v,
                                             uint32_t base);

/**
 * Return the serialized size in bytes of a container.
 */
static inline int32_t bitset_container_serialized_size_in_bytes(void) {
    return BITSET_CONTAINER_SIZE_IN_WORDS * 8;
}

/**
 * Return the the number of runs.
 */
int bitset_container_number_of_runs(bitset_container_t *bc);

bool bitset_container_iterate(const bitset_container_t *cont, uint32_t base,
                              roaring_iterator iterator, void *ptr);
bool bitset_container_iterate64(const bitset_container_t *cont, uint32_t base,
                                roaring_iterator64 iterator, uint64_t high_bits,
                                void *ptr);

/**
 * Writes the underlying array to buf, outputs how many bytes were written.
 * This is meant to be byte-by-byte compatible with the Java and Go versions of
 * Roaring.
 * The number of bytes written should be
 * bitset_container_size_in_bytes(container).
 */
int32_t bitset_container_write(const bitset_container_t *container, char *buf);

/**
 * Reads the instance from buf, outputs how many bytes were read.
 * This is meant to be byte-by-byte compatible with the Java and Go versions of
 * Roaring.
 * The number of bytes read should be bitset_container_size_in_bytes(container).
 * You need to provide the (known) cardinality.
 */
int32_t bitset_container_read(int32_t cardinality,
                              bitset_container_t *container, const char *buf);
/**
 * Return the serialized size in bytes of a container (see
 * bitset_container_write).
 * This is meant to be compatible with the Java and Go versions of Roaring and
 * assumes
 * that the cardinality of the container is already known or can be computed.
 */
static inline int32_t bitset_container_size_in_bytes(
    const bitset_container_t *container) {
    (void)container;
    return BITSET_CONTAINER_SIZE_IN_WORDS * sizeof(uint64_t);
}

/**
 * Return true if the two containers have the same content.
 */
bool bitset_container_equals(const bitset_container_t *container1,
                             const bitset_container_t *container2);

/**
* Return true if container1 is a subset of container2.
*/
bool bitset_container_is_subset(const bitset_container_t *container1,
                                const bitset_container_t *container2);

/**
 * If the element of given rank is in this container, supposing that the first
 * element has rank start_rank, then the function returns true and sets element
 * accordingly.
 * Otherwise, it returns false and update start_rank.
 */
bool bitset_container_select(const bitset_container_t *container,
                             uint32_t *start_rank, uint32_t rank,
                             uint32_t *element);

/* Returns the smallest value (assumes not empty) */
uint16_t bitset_container_minimum(const bitset_container_t *container);

/* Returns the largest value (assumes not empty) */
uint16_t bitset_container_maximum(const bitset_container_t *container);

/* Returns the number of values equal or smaller than x */
int bitset_container_rank(const bitset_container_t *container, uint16_t x);

/* Returns the index of the first value equal or larger than x, or -1 */
int bitset_container_index_equalorlarger(const bitset_container_t *container, uint16_t x);

#ifdef __cplusplus
} } }  // extern "C" { namespace roaring { namespace internal {
#endif

#endif /* INCLUDE_CONTAINERS_BITSET_H_ */
/* end file include/roaring/containers/bitset.h */
/* begin file include/roaring/containers/run.h */
/*
 * run.h
 *
 */

#ifndef INCLUDE_CONTAINERS_RUN_H_
#define INCLUDE_CONTAINERS_RUN_H_

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>



#ifdef __cplusplus
extern "C" { namespace roaring {

// Note: in pure C++ code, you should avoid putting `using` in header files
using api::roaring_iterator;
using api::roaring_iterator64;

namespace internal {
#endif

/* struct rle16_s - run length pair
 *
 * @value:  start position of the run
 * @length: length of the run is `length + 1`
 *
 * An RLE pair {v, l} would represent the integers between the interval
 * [v, v+l+1], e.g. {3, 2} = [3, 4, 5].
 */
struct rle16_s {
    uint16_t value;
    uint16_t length;
};

typedef struct rle16_s rle16_t;

#ifdef __cplusplus
    #define MAKE_RLE16(val,len) \
        {(uint16_t)(val), (uint16_t)(len)}  // no tagged structs until c++20
#else
    #define MAKE_RLE16(val,len) \
        (rle16_t){.value = (uint16_t)(val), .length = (uint16_t)(len)}
#endif

/* struct run_container_s - run container bitmap
 *
 * @n_runs:   number of rle_t pairs in `runs`.
 * @capacity: capacity in rle_t pairs `runs` can hold.
 * @runs:     pairs of rle_t.
 */
STRUCT_CONTAINER(run_container_s) {
    int32_t n_runs;
    int32_t capacity;
    rle16_t *runs;
};

typedef struct run_container_s run_container_t;

#define CAST_run(c)         CAST(run_container_t *, c)  // safer downcast
#define const_CAST_run(c)   CAST(const run_container_t *, c)
#define movable_CAST_run(c) movable_CAST(run_container_t **, c)

/* Create a new run container. Return NULL in case of failure. */
run_container_t *run_container_create(void);

/* Create a new run container with given capacity. Return NULL in case of
 * failure. */
run_container_t *run_container_create_given_capacity(int32_t size);

/*
 * Shrink the capacity to the actual size, return the number of bytes saved.
 */
int run_container_shrink_to_fit(run_container_t *src);

/* Free memory owned by `run'. */
void run_container_free(run_container_t *run);

/* Duplicate container */
run_container_t *run_container_clone(const run_container_t *src);

/*
 * Effectively deletes the value at index index, repacking data.
 */
static inline void recoverRoomAtIndex(run_container_t *run, uint16_t index) {
    memmove(run->runs + index, run->runs + (1 + index),
            (run->n_runs - index - 1) * sizeof(rle16_t));
    run->n_runs--;
}

/**
 * Good old binary search through rle data
 */
inline int32_t interleavedBinarySearch(const rle16_t *array, int32_t lenarray,
                                       uint16_t ikey) {
    int32_t low = 0;
    int32_t high = lenarray - 1;
    while (low <= high) {
        int32_t middleIndex = (low + high) >> 1;
        uint16_t middleValue = array[middleIndex].value;
        if (middleValue < ikey) {
            low = middleIndex + 1;
        } else if (middleValue > ikey) {
            high = middleIndex - 1;
        } else {
            return middleIndex;
        }
    }
    return -(low + 1);
}

/*
 * Returns index of the run which contains $ikey
 */
static inline int32_t rle16_find_run(const rle16_t *array, int32_t lenarray,
                                     uint16_t ikey) {
    int32_t low = 0;
    int32_t high = lenarray - 1;
    while (low <= high) {
        int32_t middleIndex = (low + high) >> 1;
        uint16_t min = array[middleIndex].value;
        uint16_t max = array[middleIndex].value + array[middleIndex].length;
        if (ikey > max) {
            low = middleIndex + 1;
        } else if (ikey < min) {
            high = middleIndex - 1;
        } else {
            return middleIndex;
        }
    }
    return -(low + 1);
}


/**
 * Returns number of runs which can'be be merged with the key because they
 * are less than the key.
 * Note that [5,6,7,8] can be merged with the key 9 and won't be counted.
 */
static inline int32_t rle16_count_less(const rle16_t* array, int32_t lenarray,
                                       uint16_t key) {
    if (lenarray == 0) return 0;
    int32_t low = 0;
    int32_t high = lenarray - 1;
    while (low <= high) {
        int32_t middleIndex = (low + high) >> 1;
        uint16_t min_value = array[middleIndex].value;
        uint16_t max_value = array[middleIndex].value + array[middleIndex].length;
        if (max_value + UINT32_C(1) < key) { // uint32 arithmetic
            low = middleIndex + 1;
        } else if (key < min_value) {
            high = middleIndex - 1;
        } else {
            return middleIndex;
        }
    }
    return low;
}

static inline int32_t rle16_count_greater(const rle16_t* array, int32_t lenarray,
                                          uint16_t key) {
    if (lenarray == 0) return 0;
    int32_t low = 0;
    int32_t high = lenarray - 1;
    while (low <= high) {
        int32_t middleIndex = (low + high) >> 1;
        uint16_t min_value = array[middleIndex].value;
        uint16_t max_value = array[middleIndex].value + array[middleIndex].length;
        if (max_value < key) {
            low = middleIndex + 1;
        } else if (key + UINT32_C(1) < min_value) { // uint32 arithmetic
            high = middleIndex - 1;
        } else {
            return lenarray - (middleIndex + 1);
        }
    }
    return lenarray - low;
}

/**
 * increase capacity to at least min. Whether the
 * existing data needs to be copied over depends on copy. If "copy" is false,
 * then the new content will be uninitialized, otherwise a copy is made.
 */
void run_container_grow(run_container_t *run, int32_t min, bool copy);

/**
 * Moves the data so that we can write data at index
 */
static inline void makeRoomAtIndex(run_container_t *run, uint16_t index) {
    /* This function calls realloc + memmove sequentially to move by one index.
     * Potentially copying twice the array.
     */
    if (run->n_runs + 1 > run->capacity)
        run_container_grow(run, run->n_runs + 1, true);
    memmove(run->runs + 1 + index, run->runs + index,
            (run->n_runs - index) * sizeof(rle16_t));
    run->n_runs++;
}

/* Add `pos' to `run'. Returns true if `pos' was not present. */
bool run_container_add(run_container_t *run, uint16_t pos);

/* Remove `pos' from `run'. Returns true if `pos' was present. */
static inline bool run_container_remove(run_container_t *run, uint16_t pos) {
    int32_t index = interleavedBinarySearch(run->runs, run->n_runs, pos);
    if (index >= 0) {
        int32_t le = run->runs[index].length;
        if (le == 0) {
            recoverRoomAtIndex(run, (uint16_t)index);
        } else {
            run->runs[index].value++;
            run->runs[index].length--;
        }
        return true;
    }
    index = -index - 2;  // points to preceding value, possibly -1
    if (index >= 0) {    // possible match
        int32_t offset = pos - run->runs[index].value;
        int32_t le = run->runs[index].length;
        if (offset < le) {
            // need to break in two
            run->runs[index].length = (uint16_t)(offset - 1);
            // need to insert
            uint16_t newvalue = pos + 1;
            int32_t newlength = le - offset - 1;
            makeRoomAtIndex(run, (uint16_t)(index + 1));
            run->runs[index + 1].value = newvalue;
            run->runs[index + 1].length = (uint16_t)newlength;
            return true;

        } else if (offset == le) {
            run->runs[index].length--;
            return true;
        }
    }
    // no match
    return false;
}

/* Check whether `pos' is present in `run'.  */
inline bool run_container_contains(const run_container_t *run, uint16_t pos) {
    int32_t index = interleavedBinarySearch(run->runs, run->n_runs, pos);
    if (index >= 0) return true;
    index = -index - 2;  // points to preceding value, possibly -1
    if (index != -1) {   // possible match
        int32_t offset = pos - run->runs[index].value;
        int32_t le = run->runs[index].length;
        if (offset <= le) return true;
    }
    return false;
}

/*
* Check whether all positions in a range of positions from pos_start (included)
* to pos_end (excluded) is present in `run'.
*/
static inline bool run_container_contains_range(const run_container_t *run,
                                                uint32_t pos_start, uint32_t pos_end) {
    uint32_t count = 0;
    int32_t index = interleavedBinarySearch(run->runs, run->n_runs, pos_start);
    if (index < 0) {
        index = -index - 2;
        if ((index == -1) || ((pos_start - run->runs[index].value) > run->runs[index].length)){
            return false;
        }
    }
    for (int32_t i = index; i < run->n_runs; ++i) {
        const uint32_t stop = run->runs[i].value + run->runs[i].length;
        if (run->runs[i].value >= pos_end) break;
        if (stop >= pos_end) {
            count += (((pos_end - run->runs[i].value) > 0) ? (pos_end - run->runs[i].value) : 0);
            break;
        }
        const uint32_t min = (stop - pos_start) > 0 ? (stop - pos_start) : 0;
        count += (min < run->runs[i].length) ? min : run->runs[i].length;
    }
    return count >= (pos_end - pos_start - 1);
}

/* Get the cardinality of `run'. Requires an actual computation. */
int run_container_cardinality(const run_container_t *run);

/* Card > 0?, see run_container_empty for the reverse */
static inline bool run_container_nonzero_cardinality(
    const run_container_t *run) {
    return run->n_runs > 0;  // runs never empty
}

/* Card == 0?, see run_container_nonzero_cardinality for the reverse */
static inline bool run_container_empty(
    const run_container_t *run) {
    return run->n_runs == 0;  // runs never empty
}



/* Copy one container into another. We assume that they are distinct. */
void run_container_copy(const run_container_t *src, run_container_t *dst);

/**
 * Append run described by vl to the run container, possibly merging.
 * It is assumed that the run would be inserted at the end of the container, no
 * check is made.
 * It is assumed that the run container has the necessary capacity: caller is
 * responsible for checking memory capacity.
 *
 *
 * This is not a safe function, it is meant for performance: use with care.
 */
static inline void run_container_append(run_container_t *run, rle16_t vl,
                                        rle16_t *previousrl) {
    const uint32_t previousend = previousrl->value + previousrl->length;
    if (vl.value > previousend + 1) {  // we add a new one
        run->runs[run->n_runs] = vl;
        run->n_runs++;
        *previousrl = vl;
    } else {
        uint32_t newend = vl.value + vl.length + UINT32_C(1);
        if (newend > previousend) {  // we merge
            previousrl->length = (uint16_t)(newend - 1 - previousrl->value);
            run->runs[run->n_runs - 1] = *previousrl;
        }
    }
}

/**
 * Like run_container_append but it is assumed that the content of run is empty.
 */
static inline rle16_t run_container_append_first(run_container_t *run,
                                                 rle16_t vl) {
    run->runs[run->n_runs] = vl;
    run->n_runs++;
    return vl;
}

/**
 * append a single value  given by val to the run container, possibly merging.
 * It is assumed that the value would be inserted at the end of the container,
 * no check is made.
 * It is assumed that the run container has the necessary capacity: caller is
 * responsible for checking memory capacity.
 *
 * This is not a safe function, it is meant for performance: use with care.
 */
static inline void run_container_append_value(run_container_t *run,
                                              uint16_t val,
                                              rle16_t *previousrl) {
    const uint32_t previousend = previousrl->value + previousrl->length;
    if (val > previousend + 1) {  // we add a new one
        *previousrl = MAKE_RLE16(val, 0);
        run->runs[run->n_runs] = *previousrl;
        run->n_runs++;
    } else if (val == previousend + 1) {  // we merge
        previousrl->length++;
        run->runs[run->n_runs - 1] = *previousrl;
    }
}

/**
 * Like run_container_append_value but it is assumed that the content of run is
 * empty.
 */
static inline rle16_t run_container_append_value_first(run_container_t *run,
                                                       uint16_t val) {
    rle16_t newrle = MAKE_RLE16(val, 0);
    run->runs[run->n_runs] = newrle;
    run->n_runs++;
    return newrle;
}

/* Check whether the container spans the whole chunk (cardinality = 1<<16).
 * This check can be done in constant time (inexpensive). */
static inline bool run_container_is_full(const run_container_t *run) {
    rle16_t vl = run->runs[0];
    return (run->n_runs == 1) && (vl.value == 0) && (vl.length == 0xFFFF);
}

/* Compute the union of `src_1' and `src_2' and write the result to `dst'
 * It is assumed that `dst' is distinct from both `src_1' and `src_2'. */
void run_container_union(const run_container_t *src_1,
                         const run_container_t *src_2, run_container_t *dst);

/* Compute the union of `src_1' and `src_2' and write the result to `src_1' */
void run_container_union_inplace(run_container_t *src_1,
                                 const run_container_t *src_2);

/* Compute the intersection of src_1 and src_2 and write the result to
 * dst. It is assumed that dst is distinct from both src_1 and src_2. */
void run_container_intersection(const run_container_t *src_1,
                                const run_container_t *src_2,
                                run_container_t *dst);

/* Compute the size of the intersection of src_1 and src_2 . */
int run_container_intersection_cardinality(const run_container_t *src_1,
                                           const run_container_t *src_2);

/* Check whether src_1 and src_2 intersect. */
bool run_container_intersect(const run_container_t *src_1,
                                const run_container_t *src_2);

/* Compute the symmetric difference of `src_1' and `src_2' and write the result
 * to `dst'
 * It is assumed that `dst' is distinct from both `src_1' and `src_2'. */
void run_container_xor(const run_container_t *src_1,
                       const run_container_t *src_2, run_container_t *dst);

/*
 * Write out the 16-bit integers contained in this container as a list of 32-bit
 * integers using base
 * as the starting value (it might be expected that base has zeros in its 16
 * least significant bits).
 * The function returns the number of values written.
 * The caller is responsible for allocating enough memory in out.
 */
int run_container_to_uint32_array(void *vout, const run_container_t *cont,
                                  uint32_t base);

/*
 * Print this container using printf (useful for debugging).
 */
void run_container_printf(const run_container_t *v);

/*
 * Print this container using printf as a comma-separated list of 32-bit
 * integers starting at base.
 */
void run_container_printf_as_uint32_array(const run_container_t *v,
                                          uint32_t base);

/**
 * Return the serialized size in bytes of a container having "num_runs" runs.
 */
static inline int32_t run_container_serialized_size_in_bytes(int32_t num_runs) {
    return sizeof(uint16_t) +
           sizeof(rle16_t) * num_runs;  // each run requires 2 2-byte entries.
}

bool run_container_iterate(const run_container_t *cont, uint32_t base,
                           roaring_iterator iterator, void *ptr);
bool run_container_iterate64(const run_container_t *cont, uint32_t base,
                             roaring_iterator64 iterator, uint64_t high_bits,
                             void *ptr);

/**
 * Writes the underlying array to buf, outputs how many bytes were written.
 * This is meant to be byte-by-byte compatible with the Java and Go versions of
 * Roaring.
 * The number of bytes written should be run_container_size_in_bytes(container).
 */
int32_t run_container_write(const run_container_t *container, char *buf);

/**
 * Reads the instance from buf, outputs how many bytes were read.
 * This is meant to be byte-by-byte compatible with the Java and Go versions of
 * Roaring.
 * The number of bytes read should be bitset_container_size_in_bytes(container).
 * The cardinality parameter is provided for consistency with other containers,
 * but
 * it might be effectively ignored..
 */
int32_t run_container_read(int32_t cardinality, run_container_t *container,
                           const char *buf);

/**
 * Return the serialized size in bytes of a container (see run_container_write).
 * This is meant to be compatible with the Java and Go versions of Roaring.
 */
static inline int32_t run_container_size_in_bytes(
    const run_container_t *container) {
    return run_container_serialized_size_in_bytes(container->n_runs);
}

/**
 * Return true if the two containers have the same content.
 */
ALLOW_UNALIGNED
static inline bool run_container_equals(const run_container_t *container1,
                          const run_container_t *container2) {
    if (container1->n_runs != container2->n_runs) {
        return false;
    }
    return memequals(container1->runs, container2->runs,
                     container1->n_runs * sizeof(rle16_t));
}

/**
* Return true if container1 is a subset of container2.
*/
bool run_container_is_subset(const run_container_t *container1,
                             const run_container_t *container2);

/**
 * Used in a start-finish scan that appends segments, for XOR and NOT
 */

void run_container_smart_append_exclusive(run_container_t *src,
                                          const uint16_t start,
                                          const uint16_t length);

/**
* The new container consists of a single run [start,stop).
* It is required that stop>start, the caller is responsability for this check.
* It is required that stop <= (1<<16), the caller is responsability for this check.
* The cardinality of the created container is stop - start.
* Returns NULL on failure
*/
static inline run_container_t *run_container_create_range(uint32_t start,
                                                          uint32_t stop) {
    run_container_t *rc = run_container_create_given_capacity(1);
    if (rc) {
        rle16_t r;
        r.value = (uint16_t)start;
        r.length = (uint16_t)(stop - start - 1);
        run_container_append_first(rc, r);
    }
    return rc;
}

/**
 * If the element of given rank is in this container, supposing that the first
 * element has rank start_rank, then the function returns true and sets element
 * accordingly.
 * Otherwise, it returns false and update start_rank.
 */
bool run_container_select(const run_container_t *container,
                          uint32_t *start_rank, uint32_t rank,
                          uint32_t *element);

/* Compute the difference of src_1 and src_2 and write the result to
 * dst. It is assumed that dst is distinct from both src_1 and src_2. */

void run_container_andnot(const run_container_t *src_1,
                          const run_container_t *src_2, run_container_t *dst);

void run_container_offset(const run_container_t *c,
                         container_t **loc, container_t **hic,
                         uint16_t offset);

/* Returns the smallest value (assumes not empty) */
inline uint16_t run_container_minimum(const run_container_t *run) {
    if (run->n_runs == 0) return 0;
    return run->runs[0].value;
}

/* Returns the largest value (assumes not empty) */
inline uint16_t run_container_maximum(const run_container_t *run) {
    if (run->n_runs == 0) return 0;
    return run->runs[run->n_runs - 1].value + run->runs[run->n_runs - 1].length;
}

/* Returns the number of values equal or smaller than x */
int run_container_rank(const run_container_t *arr, uint16_t x);

/* Returns the index of the first run containing a value at least as large as x, or -1 */
inline int run_container_index_equalorlarger(const run_container_t *arr, uint16_t x) {
    int32_t index = interleavedBinarySearch(arr->runs, arr->n_runs, x);
    if (index >= 0) return index;
    index = -index - 2;  // points to preceding run, possibly -1
    if (index != -1) {   // possible match
        int32_t offset = x - arr->runs[index].value;
        int32_t le = arr->runs[index].length;
        if (offset <= le) return index;
    }
    index += 1;
    if(index  < arr->n_runs) {
      return index;
    }
    return -1;
}

/*
 * Add all values in range [min, max] using hint.
 */
static inline void run_container_add_range_nruns(run_container_t* run,
                                                 uint32_t min, uint32_t max,
                                                 int32_t nruns_less,
                                                 int32_t nruns_greater) {
    int32_t nruns_common = run->n_runs - nruns_less - nruns_greater;
    if (nruns_common == 0) {
        makeRoomAtIndex(run, nruns_less);
        run->runs[nruns_less].value = min;
        run->runs[nruns_less].length = max - min;
    } else {
        uint32_t common_min = run->runs[nruns_less].value;
        uint32_t common_max = run->runs[nruns_less + nruns_common - 1].value +
                              run->runs[nruns_less + nruns_common - 1].length;
        uint32_t result_min = (common_min < min) ? common_min : min;
        uint32_t result_max = (common_max > max) ? common_max : max;

        run->runs[nruns_less].value = result_min;
        run->runs[nruns_less].length = result_max - result_min;

        memmove(&(run->runs[nruns_less + 1]),
                &(run->runs[run->n_runs - nruns_greater]),
                nruns_greater*sizeof(rle16_t));
        run->n_runs = nruns_less + 1 + nruns_greater;
    }
}

/**
 * Add all values in range [min, max]. This function is currently unused
 * and left as documentation.
 */
/*static inline void run_container_add_range(run_container_t* run,
                                           uint32_t min, uint32_t max) {
    int32_t nruns_greater = rle16_count_greater(run->runs, run->n_runs, max);
    int32_t nruns_less = rle16_count_less(run->runs, run->n_runs - nruns_greater, min);
    run_container_add_range_nruns(run, min, max, nruns_less, nruns_greater);
}*/

/**
 * Shifts last $count elements either left (distance < 0) or right (distance > 0)
 */
static inline void run_container_shift_tail(run_container_t* run,
                                            int32_t count, int32_t distance) {
    if (distance > 0) {
        if (run->capacity < count+distance) {
            run_container_grow(run, count+distance, true);
        }
    }
    int32_t srcpos = run->n_runs - count;
    int32_t dstpos = srcpos + distance;
    memmove(&(run->runs[dstpos]), &(run->runs[srcpos]), sizeof(rle16_t) * count);
    run->n_runs += distance;
}

/**
 * Remove all elements in range [min, max]
 */
static inline void run_container_remove_range(run_container_t *run, uint32_t min, uint32_t max) {
    int32_t first = rle16_find_run(run->runs, run->n_runs, min);
    int32_t last = rle16_find_run(run->runs, run->n_runs, max);

    if (first >= 0 && min > run->runs[first].value &&
        max < ((uint32_t)run->runs[first].value + (uint32_t)run->runs[first].length)) {
        // split this run into two adjacent runs

        // right subinterval
        makeRoomAtIndex(run, first+1);
        run->runs[first+1].value = max + 1;
        run->runs[first+1].length = (run->runs[first].value + run->runs[first].length) - (max + 1);

        // left subinterval
        run->runs[first].length = (min - 1) - run->runs[first].value;

        return;
    }

    // update left-most partial run
    if (first >= 0) {
        if (min > run->runs[first].value) {
            run->runs[first].length = (min - 1) - run->runs[first].value;
            first++;
        }
    } else {
        first = -first-1;
    }

    // update right-most run
    if (last >= 0) {
        uint16_t run_max = run->runs[last].value + run->runs[last].length;
        if (run_max > max) {
            run->runs[last].value = max + 1;
            run->runs[last].length = run_max - (max + 1);
            last--;
        }
    } else {
        last = (-last-1) - 1;
    }

    // remove intermediate runs
    if (first <= last) {
        run_container_shift_tail(run, run->n_runs - (last+1), -(last-first+1));
    }
}

#ifdef __cplusplus
} } }  // extern "C" { namespace roaring { namespace internal {
#endif

#endif /* INCLUDE_CONTAINERS_RUN_H_ */
/* end file include/roaring/containers/run.h */
/* begin file include/roaring/containers/convert.h */
/*
 * convert.h
 *
 */

#ifndef INCLUDE_CONTAINERS_CONVERT_H_
#define INCLUDE_CONTAINERS_CONVERT_H_


#ifdef __cplusplus
extern "C" { namespace roaring { namespace internal {
#endif

/* Convert an array into a bitset. The input container is not freed or modified.
 */
bitset_container_t *bitset_container_from_array(const array_container_t *arr);

/* Convert a run into a bitset. The input container is not freed or modified. */
bitset_container_t *bitset_container_from_run(const run_container_t *arr);

/* Convert a run into an array. The input container is not freed or modified. */
array_container_t *array_container_from_run(const run_container_t *arr);

/* Convert a bitset into an array. The input container is not freed or modified.
 */
array_container_t *array_container_from_bitset(const bitset_container_t *bits);

/* Convert an array into a run. The input container is not freed or modified.
 */
run_container_t *run_container_from_array(const array_container_t *c);

/* convert a run into either an array or a bitset
 * might free the container. This does not free the input run container. */
container_t *convert_to_bitset_or_array_container(
        run_container_t *rc, int32_t card,
        uint8_t *resulttype);

/* convert containers to and from runcontainers, as is most space efficient.
 * The container might be freed. */
container_t *convert_run_optimize(
        container_t *c, uint8_t typecode_original,
        uint8_t *typecode_after);

/* converts a run container to either an array or a bitset, IF it saves space.
 */
/* If a conversion occurs, the caller is responsible to free the original
 * container and
 * he becomes reponsible to free the new one. */
container_t *convert_run_to_efficient_container(
        run_container_t *c, uint8_t *typecode_after);

// like convert_run_to_efficient_container but frees the old result if needed
container_t *convert_run_to_efficient_container_and_free(
        run_container_t *c, uint8_t *typecode_after);

/**
 * Create new container which is a union of run container and
 * range [min, max]. Caller is responsible for freeing run container.
 */
container_t *container_from_run_range(
        const run_container_t *run,
        uint32_t min, uint32_t max,
        uint8_t *typecode_after);

#ifdef __cplusplus
} } }  // extern "C" { namespace roaring { namespace internal {
#endif

#endif /* INCLUDE_CONTAINERS_CONVERT_H_ */
/* end file include/roaring/containers/convert.h */
/* begin file include/roaring/containers/mixed_equal.h */
/*
 * mixed_equal.h
 *
 */

#ifndef CONTAINERS_MIXED_EQUAL_H_
#define CONTAINERS_MIXED_EQUAL_H_


#ifdef __cplusplus
extern "C" { namespace roaring { namespace internal {
#endif

/**
 * Return true if the two containers have the same content.
 */
bool array_container_equal_bitset(const array_container_t* container1,
                                  const bitset_container_t* container2);

/**
 * Return true if the two containers have the same content.
 */
bool run_container_equals_array(const run_container_t* container1,
                                const array_container_t* container2);
/**
 * Return true if the two containers have the same content.
 */
bool run_container_equals_bitset(const run_container_t* container1,
                                 const bitset_container_t* container2);

#ifdef __cplusplus
} } }  // extern "C" { namespace roaring { namespace internal {
#endif

#endif /* CONTAINERS_MIXED_EQUAL_H_ */
/* end file include/roaring/containers/mixed_equal.h */
/* begin file include/roaring/containers/mixed_subset.h */
/*
 * mixed_subset.h
 *
 */

#ifndef CONTAINERS_MIXED_SUBSET_H_
#define CONTAINERS_MIXED_SUBSET_H_


#ifdef __cplusplus
extern "C" { namespace roaring { namespace internal {
#endif

/**
 * Return true if container1 is a subset of container2.
 */
bool array_container_is_subset_bitset(const array_container_t* container1,
                                      const bitset_container_t* container2);

/**
* Return true if container1 is a subset of container2.
 */
bool run_container_is_subset_array(const run_container_t* container1,
                                   const array_container_t* container2);

/**
* Return true if container1 is a subset of container2.
 */
bool array_container_is_subset_run(const array_container_t* container1,
                                   const run_container_t* container2);

/**
* Return true if container1 is a subset of container2.
 */
bool run_container_is_subset_bitset(const run_container_t* container1,
                                    const bitset_container_t* container2);

/**
* Return true if container1 is a subset of container2.
*/
bool bitset_container_is_subset_run(const bitset_container_t* container1,
                                    const run_container_t* container2);

#ifdef __cplusplus
} } }  // extern "C" { namespace roaring { namespace internal {
#endif

#endif /* CONTAINERS_MIXED_SUBSET_H_ */
/* end file include/roaring/containers/mixed_subset.h */
/* begin file include/roaring/containers/mixed_andnot.h */
/*
 * mixed_andnot.h
 */
#ifndef INCLUDE_CONTAINERS_MIXED_ANDNOT_H_
#define INCLUDE_CONTAINERS_MIXED_ANDNOT_H_


#ifdef __cplusplus
extern "C" { namespace roaring { namespace internal {
#endif

/* Compute the andnot of src_1 and src_2 and write the result to
 * dst, a valid array container that could be the same as dst.*/
void array_bitset_container_andnot(const array_container_t *src_1,
                                   const bitset_container_t *src_2,
                                   array_container_t *dst);

/* Compute the andnot of src_1 and src_2 and write the result to
 * src_1 */

void array_bitset_container_iandnot(array_container_t *src_1,
                                    const bitset_container_t *src_2);

/* Compute the andnot of src_1 and src_2 and write the result to
 * dst, which does not initially have a valid container.
 * Return true for a bitset result; false for array
 */

bool bitset_array_container_andnot(
        const bitset_container_t *src_1, const array_container_t *src_2,
        container_t **dst);

/* Compute the andnot of src_1 and src_2 and write the result to
 * dst (which has no container initially).  It will modify src_1
 * to be dst if the result is a bitset.  Otherwise, it will
 * free src_1 and dst will be a new array container.  In both
 * cases, the caller is responsible for deallocating dst.
 * Returns true iff dst is a bitset  */

bool bitset_array_container_iandnot(
        bitset_container_t *src_1, const array_container_t *src_2,
        container_t **dst);

/* Compute the andnot of src_1 and src_2 and write the result to
 * dst. Result may be either a bitset or an array container
 * (returns "result is bitset"). dst does not initially have
 * any container, but becomes either a bitset container (return
 * result true) or an array container.
 */

bool run_bitset_container_andnot(
        const run_container_t *src_1, const bitset_container_t *src_2,
        container_t **dst);

/* Compute the andnot of src_1 and src_2 and write the result to
 * dst. Result may be either a bitset or an array container
 * (returns "result is bitset"). dst does not initially have
 * any container, but becomes either a bitset container (return
 * result true) or an array container.
 */

bool run_bitset_container_iandnot(
        run_container_t *src_1, const bitset_container_t *src_2,
        container_t **dst);

/* Compute the andnot of src_1 and src_2 and write the result to
 * dst. Result may be either a bitset or an array container
 * (returns "result is bitset").  dst does not initially have
 * any container, but becomes either a bitset container (return
 * result true) or an array container.
 */

bool bitset_run_container_andnot(
        const bitset_container_t *src_1, const run_container_t *src_2,
        container_t **dst);

/* Compute the andnot of src_1 and src_2 and write the result to
 * dst (which has no container initially).  It will modify src_1
 * to be dst if the result is a bitset.  Otherwise, it will
 * free src_1 and dst will be a new array container.  In both
 * cases, the caller is responsible for deallocating dst.
 * Returns true iff dst is a bitset  */

bool bitset_run_container_iandnot(
        bitset_container_t *src_1, const run_container_t *src_2,
        container_t **dst);

/* dst does not indicate a valid container initially.  Eventually it
 * can become any type of container.
 */

int run_array_container_andnot(
        const run_container_t *src_1, const array_container_t *src_2,
        container_t **dst);

/* Compute the andnot of src_1 and src_2 and write the result to
 * dst (which has no container initially).  It will modify src_1
 * to be dst if the result is a bitset.  Otherwise, it will
 * free src_1 and dst will be a new array container.  In both
 * cases, the caller is responsible for deallocating dst.
 * Returns true iff dst is a bitset  */

int run_array_container_iandnot(
        run_container_t *src_1, const array_container_t *src_2,
        container_t **dst);

/* dst must be a valid array container, allowed to be src_1 */

void array_run_container_andnot(const array_container_t *src_1,
                                const run_container_t *src_2,
                                array_container_t *dst);

/* dst does not indicate a valid container initially.  Eventually it
 * can become any kind of container.
 */

void array_run_container_iandnot(array_container_t *src_1,
                                 const run_container_t *src_2);

/* dst does not indicate a valid container initially.  Eventually it
 * can become any kind of container.
 */

int run_run_container_andnot(
        const run_container_t *src_1, const run_container_t *src_2,
        container_t **dst);

/* Compute the andnot of src_1 and src_2 and write the result to
 * dst (which has no container initially).  It will modify src_1
 * to be dst if the result is a bitset.  Otherwise, it will
 * free src_1 and dst will be a new array container.  In both
 * cases, the caller is responsible for deallocating dst.
 * Returns true iff dst is a bitset  */

int run_run_container_iandnot(
        run_container_t *src_1, const run_container_t *src_2,
        container_t **dst);

/*
 * dst is a valid array container and may be the same as src_1
 */

void array_array_container_andnot(const array_container_t *src_1,
                                  const array_container_t *src_2,
                                  array_container_t *dst);

/* inplace array-array andnot will always be able to reuse the space of
 * src_1 */
void array_array_container_iandnot(array_container_t *src_1,
                                   const array_container_t *src_2);

/* Compute the andnot of src_1 and src_2 and write the result to
 * dst (which has no container initially). Return value is
 * "dst is a bitset"
 */

bool bitset_bitset_container_andnot(
        const bitset_container_t *src_1, const bitset_container_t *src_2,
        container_t **dst);

/* Compute the andnot of src_1 and src_2 and write the result to
 * dst (which has no container initially).  It will modify src_1
 * to be dst if the result is a bitset.  Otherwise, it will
 * free src_1 and dst will be a new array container.  In both
 * cases, the caller is responsible for deallocating dst.
 * Returns true iff dst is a bitset  */

bool bitset_bitset_container_iandnot(
        bitset_container_t *src_1, const bitset_container_t *src_2,
        container_t **dst);

#ifdef __cplusplus
} } }  // extern "C" { namespace roaring { namespace internal {
#endif

#endif
/* end file include/roaring/containers/mixed_andnot.h */
/* begin file include/roaring/containers/mixed_intersection.h */
/*
 * mixed_intersection.h
 *
 */

#ifndef INCLUDE_CONTAINERS_MIXED_INTERSECTION_H_
#define INCLUDE_CONTAINERS_MIXED_INTERSECTION_H_

/* These functions appear to exclude cases where the
 * inputs have the same type and the output is guaranteed
 * to have the same type as the inputs.  Eg, array intersection
 */


#ifdef __cplusplus
extern "C" { namespace roaring { namespace internal {
#endif

/* Compute the intersection of src_1 and src_2 and write the result to
 * dst. It is allowed for dst to be equal to src_1. We assume that dst is a
 * valid container. */
void array_bitset_container_intersection(const array_container_t *src_1,
                                         const bitset_container_t *src_2,
                                         array_container_t *dst);

/* Compute the size of the intersection of src_1 and src_2. */
int array_bitset_container_intersection_cardinality(
    const array_container_t *src_1, const bitset_container_t *src_2);



/* Checking whether src_1 and src_2 intersect. */
bool array_bitset_container_intersect(const array_container_t *src_1,
                                         const bitset_container_t *src_2);

/*
 * Compute the intersection between src_1 and src_2 and write the result
 * to *dst. If the return function is true, the result is a bitset_container_t
 * otherwise is a array_container_t. We assume that dst is not pre-allocated. In
 * case of failure, *dst will be NULL.
 */
bool bitset_bitset_container_intersection(const bitset_container_t *src_1,
                                          const bitset_container_t *src_2,
                                          container_t **dst);

/* Compute the intersection between src_1 and src_2 and write the result to
 * dst. It is allowed for dst to be equal to src_1. We assume that dst is a
 * valid container. */
void array_run_container_intersection(const array_container_t *src_1,
                                      const run_container_t *src_2,
                                      array_container_t *dst);

/* Compute the intersection between src_1 and src_2 and write the result to
 * *dst. If the result is true then the result is a bitset_container_t
 * otherwise is a array_container_t.
 * If *dst == src_2, then an in-place intersection is attempted
 **/
bool run_bitset_container_intersection(const run_container_t *src_1,
                                       const bitset_container_t *src_2,
                                       container_t **dst);

/* Compute the size of the intersection between src_1 and src_2 . */
int array_run_container_intersection_cardinality(const array_container_t *src_1,
                                                 const run_container_t *src_2);

/* Compute the size of the intersection  between src_1 and src_2
 **/
int run_bitset_container_intersection_cardinality(const run_container_t *src_1,
                                       const bitset_container_t *src_2);


/* Check that src_1 and src_2 intersect. */
bool array_run_container_intersect(const array_container_t *src_1,
                                      const run_container_t *src_2);

/* Check that src_1 and src_2 intersect.
 **/
bool run_bitset_container_intersect(const run_container_t *src_1,
                                       const bitset_container_t *src_2);

/*
 * Same as bitset_bitset_container_intersection except that if the output is to
 * be a
 * bitset_container_t, then src_1 is modified and no allocation is made.
 * If the output is to be an array_container_t, then caller is responsible
 * to free the container.
 * In all cases, the result is in *dst.
 */
bool bitset_bitset_container_intersection_inplace(
    bitset_container_t *src_1, const bitset_container_t *src_2,
    container_t **dst);

#ifdef __cplusplus
} } }  // extern "C" { namespace roaring { namespace internal {
#endif

#endif /* INCLUDE_CONTAINERS_MIXED_INTERSECTION_H_ */
/* end file include/roaring/containers/mixed_intersection.h */
/* begin file include/roaring/containers/mixed_negation.h */
/*
 * mixed_negation.h
 *
 */

#ifndef INCLUDE_CONTAINERS_MIXED_NEGATION_H_
#define INCLUDE_CONTAINERS_MIXED_NEGATION_H_


#ifdef __cplusplus
extern "C" { namespace roaring { namespace internal {
#endif

/* Negation across the entire range of the container.
 * Compute the  negation of src  and write the result
 * to *dst. The complement of a
 * sufficiently sparse set will always be dense and a hence a bitmap
 * We assume that dst is pre-allocated and a valid bitset container
 * There can be no in-place version.
 */
void array_container_negation(const array_container_t *src,
                              bitset_container_t *dst);

/* Negation across the entire range of the container
 * Compute the  negation of src  and write the result
 * to *dst.  A true return value indicates a bitset result,
 * otherwise the result is an array container.
 *  We assume that dst is not pre-allocated. In
 * case of failure, *dst will be NULL.
 */
bool bitset_container_negation(
        const bitset_container_t *src,
        container_t **dst);

/* inplace version */
/*
 * Same as bitset_container_negation except that if the output is to
 * be a
 * bitset_container_t, then src is modified and no allocation is made.
 * If the output is to be an array_container_t, then caller is responsible
 * to free the container.
 * In all cases, the result is in *dst.
 */
bool bitset_container_negation_inplace(
        bitset_container_t *src,
        container_t **dst);

/* Negation across the entire range of container
 * Compute the  negation of src  and write the result
 * to *dst.
 * Return values are the *_TYPECODES as defined * in containers.h
 *  We assume that dst is not pre-allocated. In
 * case of failure, *dst will be NULL.
 */
int run_container_negation(const run_container_t *src, container_t **dst);

/*
 * Same as run_container_negation except that if the output is to
 * be a
 * run_container_t, and has the capacity to hold the result,
 * then src is modified and no allocation is made.
 * In all cases, the result is in *dst.
 */
int run_container_negation_inplace(run_container_t *src, container_t **dst);

/* Negation across a range of the container.
 * Compute the  negation of src  and write the result
 * to *dst. Returns true if the result is a bitset container
 * and false for an array container.  *dst is not preallocated.
 */
bool array_container_negation_range(
        const array_container_t *src,
        const int range_start, const int range_end,
        container_t **dst);

/* Even when the result would fit, it is unclear how to make an
 * inplace version without inefficient copying.  Thus this routine
 * may be a wrapper for the non-in-place version
 */
bool array_container_negation_range_inplace(
        array_container_t *src,
        const int range_start, const int range_end,
        container_t **dst);

/* Negation across a range of the container
 * Compute the  negation of src  and write the result
 * to *dst.  A true return value indicates a bitset result,
 * otherwise the result is an array container.
 *  We assume that dst is not pre-allocated. In
 * case of failure, *dst will be NULL.
 */
bool bitset_container_negation_range(
        const bitset_container_t *src,
        const int range_start, const int range_end,
        container_t **dst);

/* inplace version */
/*
 * Same as bitset_container_negation except that if the output is to
 * be a
 * bitset_container_t, then src is modified and no allocation is made.
 * If the output is to be an array_container_t, then caller is responsible
 * to free the container.
 * In all cases, the result is in *dst.
 */
bool bitset_container_negation_range_inplace(
        bitset_container_t *src,
        const int range_start, const int range_end,
        container_t **dst);

/* Negation across a range of container
 * Compute the  negation of src  and write the result
 * to *dst.  Return values are the *_TYPECODES as defined * in containers.h
 *  We assume that dst is not pre-allocated. In
 * case of failure, *dst will be NULL.
 */
int run_container_negation_range(
        const run_container_t *src,
        const int range_start, const int range_end,
        container_t **dst);

/*
 * Same as run_container_negation except that if the output is to
 * be a
 * run_container_t, and has the capacity to hold the result,
 * then src is modified and no allocation is made.
 * In all cases, the result is in *dst.
 */
int run_container_negation_range_inplace(
        run_container_t *src,
        const int range_start, const int range_end,
        container_t **dst);

#ifdef __cplusplus
} } }  // extern "C" { namespace roaring { namespace internal {
#endif

#endif /* INCLUDE_CONTAINERS_MIXED_NEGATION_H_ */
/* end file include/roaring/containers/mixed_negation.h */
/* begin file include/roaring/containers/mixed_union.h */
/*
 * mixed_intersection.h
 *
 */

#ifndef INCLUDE_CONTAINERS_MIXED_UNION_H_
#define INCLUDE_CONTAINERS_MIXED_UNION_H_

/* These functions appear to exclude cases where the
 * inputs have the same type and the output is guaranteed
 * to have the same type as the inputs.  Eg, bitset unions
 */


#ifdef __cplusplus
extern "C" { namespace roaring { namespace internal {
#endif

/* Compute the union of src_1 and src_2 and write the result to
 * dst. It is allowed for src_2 to be dst.   */
void array_bitset_container_union(const array_container_t *src_1,
                                  const bitset_container_t *src_2,
                                  bitset_container_t *dst);

/* Compute the union of src_1 and src_2 and write the result to
 * dst. It is allowed for src_2 to be dst.  This version does not
 * update the cardinality of dst (it is set to BITSET_UNKNOWN_CARDINALITY). */
void array_bitset_container_lazy_union(const array_container_t *src_1,
                                       const bitset_container_t *src_2,
                                       bitset_container_t *dst);

/*
 * Compute the union between src_1 and src_2 and write the result
 * to *dst. If the return function is true, the result is a bitset_container_t
 * otherwise is a array_container_t. We assume that dst is not pre-allocated. In
 * case of failure, *dst will be NULL.
 */
bool array_array_container_union(
        const array_container_t *src_1, const array_container_t *src_2,
        container_t **dst);

/*
 * Compute the union between src_1 and src_2 and write the result
 * to *dst if it cannot be written to src_1. If the return function is true,
 * the result is a bitset_container_t
 * otherwise is a array_container_t. When the result is an array_container_t, it
 * it either written to src_1 (if *dst is null) or to *dst.
 * If the result is a bitset_container_t and *dst is null, then there was a failure.
 */
bool array_array_container_inplace_union(
        array_container_t *src_1, const array_container_t *src_2,
        container_t **dst);

/*
 * Same as array_array_container_union except that it will more eagerly produce
 * a bitset.
 */
bool array_array_container_lazy_union(
        const array_container_t *src_1, const array_container_t *src_2,
        container_t **dst);

/*
 * Same as array_array_container_inplace_union except that it will more eagerly produce
 * a bitset.
 */
bool array_array_container_lazy_inplace_union(
        array_container_t *src_1, const array_container_t *src_2,
        container_t **dst);

/* Compute the union of src_1 and src_2 and write the result to
 * dst. We assume that dst is a
 * valid container. The result might need to be further converted to array or
 * bitset container,
 * the caller is responsible for the eventual conversion. */
void array_run_container_union(const array_container_t *src_1,
                               const run_container_t *src_2,
                               run_container_t *dst);

/* Compute the union of src_1 and src_2 and write the result to
 * src2. The result might need to be further converted to array or
 * bitset container,
 * the caller is responsible for the eventual conversion. */
void array_run_container_inplace_union(const array_container_t *src_1,
                                       run_container_t *src_2);

/* Compute the union of src_1 and src_2 and write the result to
 * dst. It is allowed for dst to be src_2.
 * If run_container_is_full(src_1) is true, you must not be calling this
 *function.
 **/
void run_bitset_container_union(const run_container_t *src_1,
                                const bitset_container_t *src_2,
                                bitset_container_t *dst);

/* Compute the union of src_1 and src_2 and write the result to
 * dst. It is allowed for dst to be src_2.  This version does not
 * update the cardinality of dst (it is set to BITSET_UNKNOWN_CARDINALITY).
 * If run_container_is_full(src_1) is true, you must not be calling this
 * function.
 * */
void run_bitset_container_lazy_union(const run_container_t *src_1,
                                     const bitset_container_t *src_2,
                                     bitset_container_t *dst);

#ifdef __cplusplus
} } }  // extern "C" { namespace roaring { namespace internal {
#endif

#endif /* INCLUDE_CONTAINERS_MIXED_UNION_H_ */
/* end file include/roaring/containers/mixed_union.h */
/* begin file include/roaring/containers/mixed_xor.h */
/*
 * mixed_xor.h
 *
 */

#ifndef INCLUDE_CONTAINERS_MIXED_XOR_H_
#define INCLUDE_CONTAINERS_MIXED_XOR_H_

/* These functions appear to exclude cases where the
 * inputs have the same type and the output is guaranteed
 * to have the same type as the inputs.  Eg, bitset unions
 */

/*
 * Java implementation (as of May 2016) for array_run, run_run
 * and  bitset_run don't do anything different for inplace.
 * (They are not truly in place.)
 */



#ifdef __cplusplus
extern "C" { namespace roaring { namespace internal {
#endif

/* Compute the xor of src_1 and src_2 and write the result to
 * dst (which has no container initially).
 * Result is true iff dst is a bitset  */
bool array_bitset_container_xor(
        const array_container_t *src_1, const bitset_container_t *src_2,
        container_t **dst);

/* Compute the xor of src_1 and src_2 and write the result to
 * dst. It is allowed for src_2 to be dst.  This version does not
 * update the cardinality of dst (it is set to BITSET_UNKNOWN_CARDINALITY).
 */

void array_bitset_container_lazy_xor(const array_container_t *src_1,
                                     const bitset_container_t *src_2,
                                     bitset_container_t *dst);
/* Compute the xor of src_1 and src_2 and write the result to
 * dst (which has no container initially). Return value is
 * "dst is a bitset"
 */

bool bitset_bitset_container_xor(
        const bitset_container_t *src_1, const bitset_container_t *src_2,
        container_t **dst);

/* Compute the xor of src_1 and src_2 and write the result to
 * dst. Result may be either a bitset or an array container
 * (returns "result is bitset"). dst does not initially have
 * any container, but becomes either a bitset container (return
 * result true) or an array container.
 */

bool run_bitset_container_xor(
        const run_container_t *src_1, const bitset_container_t *src_2,
        container_t **dst);

/* lazy xor.  Dst is initialized and may be equal to src_2.
 *  Result is left as a bitset container, even if actual
 *  cardinality would dictate an array container.
 */

void run_bitset_container_lazy_xor(const run_container_t *src_1,
                                   const bitset_container_t *src_2,
                                   bitset_container_t *dst);

/* dst does not indicate a valid container initially.  Eventually it
 * can become any kind of container.
 */

int array_run_container_xor(
        const array_container_t *src_1, const run_container_t *src_2,
        container_t **dst);

/* dst does not initially have a valid container.  Creates either
 * an array or a bitset container, indicated by return code
 */

bool array_array_container_xor(
        const array_container_t *src_1, const array_container_t *src_2,
        container_t **dst);

/* dst does not initially have a valid container.  Creates either
 * an array or a bitset container, indicated by return code.
 * A bitset container will not have a valid cardinality and the
 * container type might not be correct for the actual cardinality
 */

bool array_array_container_lazy_xor(
        const array_container_t *src_1, const array_container_t *src_2,
        container_t **dst);

/* Dst is a valid run container. (Can it be src_2? Let's say not.)
 * Leaves result as run container, even if other options are
 * smaller.
 */

void array_run_container_lazy_xor(const array_container_t *src_1,
                                  const run_container_t *src_2,
                                  run_container_t *dst);

/* dst does not indicate a valid container initially.  Eventually it
 * can become any kind of container.
 */

int run_run_container_xor(
        const run_container_t *src_1, const run_container_t *src_2,
        container_t **dst);

/* INPLACE versions (initial implementation may not exploit all inplace
 * opportunities (if any...)
 */

/* Compute the xor of src_1 and src_2 and write the result to
 * dst (which has no container initially).  It will modify src_1
 * to be dst if the result is a bitset.  Otherwise, it will
 * free src_1 and dst will be a new array container.  In both
 * cases, the caller is responsible for deallocating dst.
 * Returns true iff dst is a bitset  */

bool bitset_array_container_ixor(
        bitset_container_t *src_1, const array_container_t *src_2,
        container_t **dst);

bool bitset_bitset_container_ixor(
        bitset_container_t *src_1, const bitset_container_t *src_2,
        container_t **dst);

bool array_bitset_container_ixor(
        array_container_t *src_1, const bitset_container_t *src_2,
        container_t **dst);

/* Compute the xor of src_1 and src_2 and write the result to
 * dst. Result may be either a bitset or an array container
 * (returns "result is bitset"). dst does not initially have
 * any container, but becomes either a bitset container (return
 * result true) or an array container.
 */

bool run_bitset_container_ixor(
        run_container_t *src_1, const bitset_container_t *src_2,
        container_t **dst);

bool bitset_run_container_ixor(
        bitset_container_t *src_1, const run_container_t *src_2,
        container_t **dst);

/* dst does not indicate a valid container initially.  Eventually it
 * can become any kind of container.
 */

int array_run_container_ixor(
        array_container_t *src_1, const run_container_t *src_2,
        container_t **dst);

int run_array_container_ixor(
        run_container_t *src_1, const array_container_t *src_2,
        container_t **dst);

bool array_array_container_ixor(
        array_container_t *src_1, const array_container_t *src_2,
        container_t **dst);

int run_run_container_ixor(
        run_container_t *src_1, const run_container_t *src_2,
        container_t **dst);

#ifdef __cplusplus
} } }  // extern "C" { namespace roaring { namespace internal {
#endif

#endif
/* end file include/roaring/containers/mixed_xor.h */
/* begin file include/roaring/containers/containers.h */
#ifndef CONTAINERS_CONTAINERS_H
#define CONTAINERS_CONTAINERS_H

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>


#ifdef __cplusplus
extern "C" { namespace roaring { namespace internal {
#endif

// would enum be possible or better?

/**
 * The switch case statements follow
 * BITSET_CONTAINER_TYPE -- ARRAY_CONTAINER_TYPE -- RUN_CONTAINER_TYPE
 * so it makes more sense to number them 1, 2, 3 (in the vague hope that the
 * compiler might exploit this ordering).
 */

#define BITSET_CONTAINER_TYPE 1
#define ARRAY_CONTAINER_TYPE 2
#define RUN_CONTAINER_TYPE 3
#define SHARED_CONTAINER_TYPE 4

/**
 * Macros for pairing container type codes, suitable for switch statements.
 * Use PAIR_CONTAINER_TYPES() for the switch, CONTAINER_PAIR() for the cases:
 *
 *     switch (PAIR_CONTAINER_TYPES(type1, type2)) {
 *        case CONTAINER_PAIR(BITSET,ARRAY):
 *        ...
 *     }
 */
#define PAIR_CONTAINER_TYPES(type1,type2) \
    (4 * (type1) + (type2))

#define CONTAINER_PAIR(name1,name2) \
    (4 * (name1##_CONTAINER_TYPE) + (name2##_CONTAINER_TYPE))

/**
 * A shared container is a wrapper around a container
 * with reference counting.
 */

STRUCT_CONTAINER(shared_container_s) {
    container_t *container;
    uint8_t typecode;
    uint32_t counter;  // to be managed atomically
};

typedef struct shared_container_s shared_container_t;

#define CAST_shared(c)         CAST(shared_container_t *, c)  // safer downcast
#define const_CAST_shared(c)   CAST(const shared_container_t *, c)
#define movable_CAST_shared(c) movable_CAST(shared_container_t **, c)

/*
 * With copy_on_write = true
 *  Create a new shared container if the typecode is not SHARED_CONTAINER_TYPE,
 * otherwise, increase the count
 * If copy_on_write = false, then clone.
 * Return NULL in case of failure.
 **/
container_t *get_copy_of_container(container_t *container, uint8_t *typecode,
                                   bool copy_on_write);

/* Frees a shared container (actually decrement its counter and only frees when
 * the counter falls to zero). */
void shared_container_free(shared_container_t *container);

/* extract a copy from the shared container, freeing the shared container if
there is just one instance left,
clone instances when the counter is higher than one
*/
container_t *shared_container_extract_copy(shared_container_t *container,
                                           uint8_t *typecode);

/* access to container underneath */
static inline const container_t *container_unwrap_shared(
    const container_t *candidate_shared_container, uint8_t *type
){
    if (*type == SHARED_CONTAINER_TYPE) {
        *type = const_CAST_shared(candidate_shared_container)->typecode;
        assert(*type != SHARED_CONTAINER_TYPE);
        return const_CAST_shared(candidate_shared_container)->container;
    } else {
        return candidate_shared_container;
    }
}


/* access to container underneath */
static inline container_t *container_mutable_unwrap_shared(
    container_t *c, uint8_t *type
) {
    if (*type == SHARED_CONTAINER_TYPE) {  // the passed in container is shared
        *type = CAST_shared(c)->typecode;
        assert(*type != SHARED_CONTAINER_TYPE);
        return CAST_shared(c)->container;  // return the enclosed container
    } else {
        return c;  // wasn't shared, so return as-is
    }
}

/* access to container underneath and queries its type */
static inline uint8_t get_container_type(
    const container_t *c, uint8_t type
){
    if (type == SHARED_CONTAINER_TYPE) {
        return const_CAST_shared(c)->typecode;
    } else {
        return type;
    }
}

/**
 * Copies a container, requires a typecode. This allocates new memory, caller
 * is responsible for deallocation. If the container is not shared, then it is
 * physically cloned. Sharable containers are not cloneable.
 */
container_t *container_clone(const container_t *container, uint8_t typecode);

/* access to container underneath, cloning it if needed */
static inline container_t *get_writable_copy_if_shared(
    container_t *c, uint8_t *type
){
    if (*type == SHARED_CONTAINER_TYPE) {  // shared, return enclosed container
        return shared_container_extract_copy(CAST_shared(c), type);
    } else {
        return c;  // not shared, so return as-is
    }
}

/**
 * End of shared container code
 */

static const char *container_names[] = {"bitset", "array", "run", "shared"};
static const char *shared_container_names[] = {
    "bitset (shared)", "array (shared)", "run (shared)"};

// no matter what the initial container was, convert it to a bitset
// if a new container is produced, caller responsible for freeing the previous
// one
// container should not be a shared container
static inline bitset_container_t *container_to_bitset(
    container_t *c, uint8_t typecode
){
    bitset_container_t *result = NULL;
    switch (typecode) {
        case BITSET_CONTAINER_TYPE:
            return CAST_bitset(c);  // nothing to do
        case ARRAY_CONTAINER_TYPE:
            result = bitset_container_from_array(CAST_array(c));
            return result;
        case RUN_CONTAINER_TYPE:
            result = bitset_container_from_run(CAST_run(c));
            return result;
        case SHARED_CONTAINER_TYPE:
            assert(false);
            roaring_unreachable;
    }
    assert(false);
    roaring_unreachable;
    return 0;  // unreached
}

/**
 * Get the container name from the typecode
 * (unused at time of writing)
 */
/*static inline const char *get_container_name(uint8_t typecode) {
    switch (typecode) {
        case BITSET_CONTAINER_TYPE:
            return container_names[0];
        case ARRAY_CONTAINER_TYPE:
            return container_names[1];
        case RUN_CONTAINER_TYPE:
            return container_names[2];
        case SHARED_CONTAINER_TYPE:
            return container_names[3];
        default:
            assert(false);
            roaring_unreachable;
            return "unknown";
    }
}*/

static inline const char *get_full_container_name(
    const container_t *c, uint8_t typecode
){
    switch (typecode) {
        case BITSET_CONTAINER_TYPE:
            return container_names[0];
        case ARRAY_CONTAINER_TYPE:
            return container_names[1];
        case RUN_CONTAINER_TYPE:
            return container_names[2];
        case SHARED_CONTAINER_TYPE:
            switch (const_CAST_shared(c)->typecode) {
                case BITSET_CONTAINER_TYPE:
                    return shared_container_names[0];
                case ARRAY_CONTAINER_TYPE:
                    return shared_container_names[1];
                case RUN_CONTAINER_TYPE:
                    return shared_container_names[2];
                default:
                    assert(false);
                    roaring_unreachable;
                    return "unknown";
            }
            break;
        default:
            assert(false);
            roaring_unreachable;
            return "unknown";
    }
    roaring_unreachable;
    return NULL;
}

/**
 * Get the container cardinality (number of elements), requires a  typecode
 */
static inline int container_get_cardinality(
    const container_t *c, uint8_t typecode
){
    c = container_unwrap_shared(c, &typecode);
    switch (typecode) {
        case BITSET_CONTAINER_TYPE:
            return bitset_container_cardinality(const_CAST_bitset(c));
        case ARRAY_CONTAINER_TYPE:
            return array_container_cardinality(const_CAST_array(c));
        case RUN_CONTAINER_TYPE:
            return run_container_cardinality(const_CAST_run(c));
    }
    assert(false);
    roaring_unreachable;
    return 0;  // unreached
}



// returns true if a container is known to be full. Note that a lazy bitset
// container
// might be full without us knowing
static inline bool container_is_full(const container_t *c, uint8_t typecode) {
    c = container_unwrap_shared(c, &typecode);
    switch (typecode) {
        case BITSET_CONTAINER_TYPE:
            return bitset_container_cardinality(
                       const_CAST_bitset(c)) == (1 << 16);
        case ARRAY_CONTAINER_TYPE:
            return array_container_cardinality(
                       const_CAST_array(c)) == (1 << 16);
        case RUN_CONTAINER_TYPE:
            return run_container_is_full(const_CAST_run(c));
    }
    assert(false);
    roaring_unreachable;
    return 0;  // unreached
}

static inline int container_shrink_to_fit(
    container_t *c, uint8_t type
){
    c = container_mutable_unwrap_shared(c, &type);
    switch (type) {
        case BITSET_CONTAINER_TYPE:
            return 0;  // no shrinking possible
        case ARRAY_CONTAINER_TYPE:
            return array_container_shrink_to_fit(CAST_array(c));
        case RUN_CONTAINER_TYPE:
            return run_container_shrink_to_fit(CAST_run(c));
    }
    assert(false);
    roaring_unreachable;
    return 0;  // unreached
}


/**
 * make a container with a run of ones
 */
/* initially always use a run container, even if an array might be
 * marginally
 * smaller */
static inline container_t *container_range_of_ones(
    uint32_t range_start, uint32_t range_end,
    uint8_t *result_type
){
    assert(range_end >= range_start);
    uint64_t cardinality =  range_end - range_start + 1;
    if(cardinality <= 2) {
      *result_type = ARRAY_CONTAINER_TYPE;
      return array_container_create_range(range_start, range_end);
    } else {
      *result_type = RUN_CONTAINER_TYPE;
      return run_container_create_range(range_start, range_end);
    }
}


/*  Create a container with all the values between in [min,max) at a
    distance k*step from min. */
static inline container_t *container_from_range(
    uint8_t *type, uint32_t min,
    uint32_t max, uint16_t step
){
    if (step == 0) return NULL;  // being paranoid
    if (step == 1) {
        return container_range_of_ones(min,max,type);
        // Note: the result is not always a run (need to check the cardinality)
        //*type = RUN_CONTAINER_TYPE;
        //return run_container_create_range(min, max);
    }
    int size = (max - min + step - 1) / step;
    if (size <= DEFAULT_MAX_SIZE) {  // array container
        *type = ARRAY_CONTAINER_TYPE;
        array_container_t *array = array_container_create_given_capacity(size);
        array_container_add_from_range(array, min, max, step);
        assert(array->cardinality == size);
        return array;
    } else {  // bitset container
        *type = BITSET_CONTAINER_TYPE;
        bitset_container_t *bitset = bitset_container_create();
        bitset_container_add_from_range(bitset, min, max, step);
        assert(bitset->cardinality == size);
        return bitset;
    }
}

/**
 * "repair" the container after lazy operations.
 */
static inline container_t *container_repair_after_lazy(
    container_t *c, uint8_t *type
){
    c = get_writable_copy_if_shared(c, type);  // !!! unnecessary cloning
    container_t *result = NULL;
    switch (*type) {
        case BITSET_CONTAINER_TYPE: {
            bitset_container_t *bc = CAST_bitset(c);
            bc->cardinality = bitset_container_compute_cardinality(bc);
            if (bc->cardinality <= DEFAULT_MAX_SIZE) {
                result = array_container_from_bitset(bc);
                bitset_container_free(bc);
                *type = ARRAY_CONTAINER_TYPE;
                return result;
            }
            return c; }
        case ARRAY_CONTAINER_TYPE:
            return c;  // nothing to do
        case RUN_CONTAINER_TYPE:
            return convert_run_to_efficient_container_and_free(
                            CAST_run(c), type);
        case SHARED_CONTAINER_TYPE:
            assert(false);
    }
    assert(false);
    roaring_unreachable;
    return 0;  // unreached
}

/**
 * Writes the underlying array to buf, outputs how many bytes were written.
 * This is meant to be byte-by-byte compatible with the Java and Go versions of
 * Roaring.
 * The number of bytes written should be
 * container_write(container, buf).
 *
 */
static inline int32_t container_write(
    const container_t *c, uint8_t typecode,
    char *buf
){
    c = container_unwrap_shared(c, &typecode);
    switch (typecode) {
        case BITSET_CONTAINER_TYPE:
            return bitset_container_write(const_CAST_bitset(c), buf);
        case ARRAY_CONTAINER_TYPE:
            return array_container_write(const_CAST_array(c), buf);
        case RUN_CONTAINER_TYPE:
            return run_container_write(const_CAST_run(c), buf);
    }
    assert(false);
    roaring_unreachable;
    return 0;  // unreached
}

/**
 * Get the container size in bytes under portable serialization (see
 * container_write), requires a
 * typecode
 */
static inline int32_t container_size_in_bytes(
    const container_t *c, uint8_t typecode
){
    c = container_unwrap_shared(c, &typecode);
    switch (typecode) {
        case BITSET_CONTAINER_TYPE:
            return bitset_container_size_in_bytes(const_CAST_bitset(c));
        case ARRAY_CONTAINER_TYPE:
            return array_container_size_in_bytes(const_CAST_array(c));
        case RUN_CONTAINER_TYPE:
            return run_container_size_in_bytes(const_CAST_run(c));
    }
    assert(false);
    roaring_unreachable;
    return 0;  // unreached
}

/**
 * print the container (useful for debugging), requires a  typecode
 */
void container_printf(const container_t *container, uint8_t typecode);

/**
 * print the content of the container as a comma-separated list of 32-bit values
 * starting at base, requires a  typecode
 */
void container_printf_as_uint32_array(const container_t *container,
                                      uint8_t typecode, uint32_t base);

/**
 * Checks whether a container is not empty, requires a  typecode
 */
static inline bool container_nonzero_cardinality(
    const container_t *c, uint8_t typecode
){
    c = container_unwrap_shared(c, &typecode);
    switch (typecode) {
        case BITSET_CONTAINER_TYPE:
            return bitset_container_const_nonzero_cardinality(
                            const_CAST_bitset(c));
        case ARRAY_CONTAINER_TYPE:
            return array_container_nonzero_cardinality(const_CAST_array(c));
        case RUN_CONTAINER_TYPE:
            return run_container_nonzero_cardinality(const_CAST_run(c));
    }
    assert(false);
    roaring_unreachable;
    return 0;  // unreached
}

/**
 * Recover memory from a container, requires a  typecode
 */
void container_free(container_t *container, uint8_t typecode);

/**
 * Convert a container to an array of values, requires a  typecode as well as a
 * "base" (most significant values)
 * Returns number of ints added.
 */
static inline int container_to_uint32_array(
    uint32_t *output,
    const container_t *c, uint8_t typecode,
    uint32_t base
){
    c = container_unwrap_shared(c, &typecode);
    switch (typecode) {
        case BITSET_CONTAINER_TYPE:
            return bitset_container_to_uint32_array(
                            output, const_CAST_bitset(c), base);
        case ARRAY_CONTAINER_TYPE:
            return array_container_to_uint32_array(
                            output, const_CAST_array(c), base);
        case RUN_CONTAINER_TYPE:
            return run_container_to_uint32_array(
                            output, const_CAST_run(c), base);
    }
    assert(false);
    roaring_unreachable;
    return 0;  // unreached
}

/**
 * Add a value to a container, requires a  typecode, fills in new_typecode and
 * return (possibly different) container.
 * This function may allocate a new container, and caller is responsible for
 * memory deallocation
 */
static inline container_t *container_add(
    container_t *c, uint16_t val,
    uint8_t typecode,  // !!! should be second argument?
    uint8_t *new_typecode
){
    c = get_writable_copy_if_shared(c, &typecode);
    switch (typecode) {
        case BITSET_CONTAINER_TYPE:
            bitset_container_set(CAST_bitset(c), val);
            *new_typecode = BITSET_CONTAINER_TYPE;
            return c;
        case ARRAY_CONTAINER_TYPE: {
            array_container_t *ac = CAST_array(c);
            if (array_container_try_add(ac, val, DEFAULT_MAX_SIZE) != -1) {
                *new_typecode = ARRAY_CONTAINER_TYPE;
                return ac;
            } else {
                bitset_container_t* bitset = bitset_container_from_array(ac);
                bitset_container_add(bitset, val);
                *new_typecode = BITSET_CONTAINER_TYPE;
                return bitset;
            }
        } break;
        case RUN_CONTAINER_TYPE:
            // per Java, no container type adjustments are done (revisit?)
            run_container_add(CAST_run(c), val);
            *new_typecode = RUN_CONTAINER_TYPE;
            return c;
        default:
            assert(false);
            roaring_unreachable;
            return NULL;
    }
}

/**
 * Remove a value from a container, requires a  typecode, fills in new_typecode
 * and
 * return (possibly different) container.
 * This function may allocate a new container, and caller is responsible for
 * memory deallocation
 */
static inline container_t *container_remove(
    container_t *c, uint16_t val,
    uint8_t typecode,  // !!! should be second argument?
    uint8_t *new_typecode
){
    c = get_writable_copy_if_shared(c, &typecode);
    switch (typecode) {
        case BITSET_CONTAINER_TYPE:
            if (bitset_container_remove(CAST_bitset(c), val)) {
                int card = bitset_container_cardinality(CAST_bitset(c));
                if (card <= DEFAULT_MAX_SIZE) {
                    *new_typecode = ARRAY_CONTAINER_TYPE;
                    return array_container_from_bitset(CAST_bitset(c));
                }
            }
            *new_typecode = typecode;
            return c;
        case ARRAY_CONTAINER_TYPE:
            *new_typecode = typecode;
            array_container_remove(CAST_array(c), val);
            return c;
        case RUN_CONTAINER_TYPE:
            // per Java, no container type adjustments are done (revisit?)
            run_container_remove(CAST_run(c), val);
            *new_typecode = RUN_CONTAINER_TYPE;
            return c;
        default:
            assert(false);
            roaring_unreachable;
            return NULL;
    }
}

/**
 * Check whether a value is in a container, requires a  typecode
 */
static inline bool container_contains(
    const container_t *c,
    uint16_t val,
    uint8_t typecode  // !!! should be second argument?
){
    c = container_unwrap_shared(c, &typecode);
    switch (typecode) {
        case BITSET_CONTAINER_TYPE:
            return bitset_container_get(const_CAST_bitset(c), val);
        case ARRAY_CONTAINER_TYPE:
            return array_container_contains(const_CAST_array(c), val);
        case RUN_CONTAINER_TYPE:
            return run_container_contains(const_CAST_run(c), val);
        default:
            assert(false);
            roaring_unreachable;
            return false;
    }
}

/**
 * Check whether a range of values from range_start (included) to range_end (excluded)
 * is in a container, requires a typecode
 */
static inline bool container_contains_range(
    const container_t *c,
    uint32_t range_start, uint32_t range_end,
    uint8_t typecode  // !!! should be second argument?
){
    c = container_unwrap_shared(c, &typecode);
    switch (typecode) {
        case BITSET_CONTAINER_TYPE:
            return bitset_container_get_range(const_CAST_bitset(c),
                                                range_start, range_end);
        case ARRAY_CONTAINER_TYPE:
            return array_container_contains_range(const_CAST_array(c),
                                                    range_start, range_end);
        case RUN_CONTAINER_TYPE:
            return run_container_contains_range(const_CAST_run(c),
                                                    range_start, range_end);
        default:
            assert(false);
            roaring_unreachable;
            return false;
    }
}

/**
 * Returns true if the two containers have the same content. Note that
 * two containers having different types can be "equal" in this sense.
 */
static inline bool container_equals(
    const container_t *c1, uint8_t type1,
    const container_t *c2, uint8_t type2
){
    c1 = container_unwrap_shared(c1, &type1);
    c2 = container_unwrap_shared(c2, &type2);
    switch (PAIR_CONTAINER_TYPES(type1, type2)) {
        case CONTAINER_PAIR(BITSET,BITSET):
            return bitset_container_equals(const_CAST_bitset(c1),
                                           const_CAST_bitset(c2));

        case CONTAINER_PAIR(BITSET,RUN):
            return run_container_equals_bitset(const_CAST_run(c2),
                                               const_CAST_bitset(c1));

        case CONTAINER_PAIR(RUN,BITSET):
            return run_container_equals_bitset(const_CAST_run(c1),
                                               const_CAST_bitset(c2));

        case CONTAINER_PAIR(BITSET,ARRAY):
            // java would always return false?
            return array_container_equal_bitset(const_CAST_array(c2),
                                                const_CAST_bitset(c1));

        case CONTAINER_PAIR(ARRAY,BITSET):
            // java would always return false?
            return array_container_equal_bitset(const_CAST_array(c1),
                                                const_CAST_bitset(c2));

        case CONTAINER_PAIR(ARRAY,RUN):
            return run_container_equals_array(const_CAST_run(c2),
                                              const_CAST_array(c1));

        case CONTAINER_PAIR(RUN,ARRAY):
            return run_container_equals_array(const_CAST_run(c1),
                                              const_CAST_array(c2));

        case CONTAINER_PAIR(ARRAY,ARRAY):
            return array_container_equals(const_CAST_array(c1),
                                          const_CAST_array(c2));

        case CONTAINER_PAIR(RUN,RUN):
            return run_container_equals(const_CAST_run(c1),
                                        const_CAST_run(c2));

        default:
            assert(false);
            roaring_unreachable;
            return false;
    }
}

/**
 * Returns true if the container c1 is a subset of the container c2. Note that
 * c1 can be a subset of c2 even if they have a different type.
 */
static inline bool container_is_subset(
    const container_t *c1, uint8_t type1,
    const container_t *c2, uint8_t type2
){
    c1 = container_unwrap_shared(c1, &type1);
    c2 = container_unwrap_shared(c2, &type2);
    switch (PAIR_CONTAINER_TYPES(type1, type2)) {
        case CONTAINER_PAIR(BITSET,BITSET):
            return bitset_container_is_subset(const_CAST_bitset(c1),
                                              const_CAST_bitset(c2));

        case CONTAINER_PAIR(BITSET,RUN):
            return bitset_container_is_subset_run(const_CAST_bitset(c1),
                                                  const_CAST_run(c2));

        case CONTAINER_PAIR(RUN,BITSET):
            return run_container_is_subset_bitset(const_CAST_run(c1),
                                                  const_CAST_bitset(c2));

        case CONTAINER_PAIR(BITSET,ARRAY):
            return false;  // by construction, size(c1) > size(c2)

        case CONTAINER_PAIR(ARRAY,BITSET):
            return array_container_is_subset_bitset(const_CAST_array(c1),
                                                    const_CAST_bitset(c2));

        case CONTAINER_PAIR(ARRAY,RUN):
            return array_container_is_subset_run(const_CAST_array(c1),
                                                 const_CAST_run(c2));

        case CONTAINER_PAIR(RUN,ARRAY):
            return run_container_is_subset_array(const_CAST_run(c1),
                                                 const_CAST_array(c2));

        case CONTAINER_PAIR(ARRAY,ARRAY):
            return array_container_is_subset(const_CAST_array(c1),
                                             const_CAST_array(c2));

        case CONTAINER_PAIR(RUN,RUN):
            return run_container_is_subset(const_CAST_run(c1),
                                           const_CAST_run(c2));

        default:
            assert(false);
            roaring_unreachable;
            return false;
    }
}

// macro-izations possibilities for generic non-inplace binary-op dispatch

/**
 * Compute intersection between two containers, generate a new container (having
 * type result_type), requires a typecode. This allocates new memory, caller
 * is responsible for deallocation.
 */
static inline container_t *container_and(
    const container_t *c1, uint8_t type1,
    const container_t *c2, uint8_t type2,
    uint8_t *result_type
){
    c1 = container_unwrap_shared(c1, &type1);
    c2 = container_unwrap_shared(c2, &type2);
    container_t *result = NULL;
    switch (PAIR_CONTAINER_TYPES(type1, type2)) {
        case CONTAINER_PAIR(BITSET,BITSET):
            *result_type = bitset_bitset_container_intersection(
                                const_CAST_bitset(c1),
                                const_CAST_bitset(c2), &result)
                                    ? BITSET_CONTAINER_TYPE
                                    : ARRAY_CONTAINER_TYPE;
            return result;

        case CONTAINER_PAIR(ARRAY,ARRAY):
            result = array_container_create();
            array_container_intersection(const_CAST_array(c1),
                                         const_CAST_array(c2),
                                         CAST_array(result));
            *result_type = ARRAY_CONTAINER_TYPE;  // never bitset
            return result;

        case CONTAINER_PAIR(RUN,RUN):
            result = run_container_create();
            run_container_intersection(const_CAST_run(c1),
                                       const_CAST_run(c2),
                                       CAST_run(result));
            return convert_run_to_efficient_container_and_free(
                        CAST_run(result), result_type);

        case CONTAINER_PAIR(BITSET,ARRAY):
            result = array_container_create();
            array_bitset_container_intersection(const_CAST_array(c2),
                                                const_CAST_bitset(c1),
                                                CAST_array(result));
            *result_type = ARRAY_CONTAINER_TYPE;  // never bitset
            return result;

        case CONTAINER_PAIR(ARRAY,BITSET):
            result = array_container_create();
            *result_type = ARRAY_CONTAINER_TYPE;  // never bitset
            array_bitset_container_intersection(const_CAST_array(c1),
                                                const_CAST_bitset(c2),
                                                CAST_array(result));
            return result;

        case CONTAINER_PAIR(BITSET,RUN):
            *result_type = run_bitset_container_intersection(
                                const_CAST_run(c2),
                                const_CAST_bitset(c1), &result)
                                    ? BITSET_CONTAINER_TYPE
                                    : ARRAY_CONTAINER_TYPE;
            return result;

        case CONTAINER_PAIR(RUN,BITSET):
            *result_type = run_bitset_container_intersection(
                                const_CAST_run(c1),
                                const_CAST_bitset(c2), &result)
                                    ? BITSET_CONTAINER_TYPE
                                    : ARRAY_CONTAINER_TYPE;
            return result;

        case CONTAINER_PAIR(ARRAY,RUN):
            result = array_container_create();
            *result_type = ARRAY_CONTAINER_TYPE;  // never bitset
            array_run_container_intersection(const_CAST_array(c1),
                                             const_CAST_run(c2),
                                             CAST_array(result));
            return result;

        case CONTAINER_PAIR(RUN,ARRAY):
            result = array_container_create();
            *result_type = ARRAY_CONTAINER_TYPE;  // never bitset
            array_run_container_intersection(const_CAST_array(c2),
                                             const_CAST_run(c1),
                                             CAST_array(result));
            return result;

        default:
            assert(false);
            roaring_unreachable;
            return NULL;
    }
}

/**
 * Compute the size of the intersection between two containers.
 */
static inline int container_and_cardinality(
    const container_t *c1, uint8_t type1,
    const container_t *c2, uint8_t type2
){
    c1 = container_unwrap_shared(c1, &type1);
    c2 = container_unwrap_shared(c2, &type2);
    switch (PAIR_CONTAINER_TYPES(type1, type2)) {
        case CONTAINER_PAIR(BITSET,BITSET):
            return bitset_container_and_justcard(
                const_CAST_bitset(c1), const_CAST_bitset(c2));

        case CONTAINER_PAIR(ARRAY,ARRAY):
            return array_container_intersection_cardinality(
                const_CAST_array(c1), const_CAST_array(c2));

        case CONTAINER_PAIR(RUN,RUN):
            return run_container_intersection_cardinality(
                const_CAST_run(c1), const_CAST_run(c2));

        case CONTAINER_PAIR(BITSET,ARRAY):
            return array_bitset_container_intersection_cardinality(
                const_CAST_array(c2), const_CAST_bitset(c1));

        case CONTAINER_PAIR(ARRAY,BITSET):
            return array_bitset_container_intersection_cardinality(
                const_CAST_array(c1), const_CAST_bitset(c2));

        case CONTAINER_PAIR(BITSET,RUN):
            return run_bitset_container_intersection_cardinality(
                const_CAST_run(c2), const_CAST_bitset(c1));

        case CONTAINER_PAIR(RUN,BITSET):
            return run_bitset_container_intersection_cardinality(
                const_CAST_run(c1), const_CAST_bitset(c2));

        case CONTAINER_PAIR(ARRAY,RUN):
            return array_run_container_intersection_cardinality(
                const_CAST_array(c1), const_CAST_run(c2));

        case CONTAINER_PAIR(RUN,ARRAY):
            return array_run_container_intersection_cardinality(
                const_CAST_array(c2), const_CAST_run(c1));

        default:
            assert(false);
            roaring_unreachable;
            return 0;
    }
}

/**
 * Check whether two containers intersect.
 */
static inline bool container_intersect(
    const container_t *c1, uint8_t type1,
    const container_t *c2, uint8_t type2
){
    c1 = container_unwrap_shared(c1, &type1);
    c2 = container_unwrap_shared(c2, &type2);
    switch (PAIR_CONTAINER_TYPES(type1, type2)) {
        case CONTAINER_PAIR(BITSET,BITSET):
            return bitset_container_intersect(const_CAST_bitset(c1),
                                              const_CAST_bitset(c2));

        case CONTAINER_PAIR(ARRAY,ARRAY):
            return array_container_intersect(const_CAST_array(c1),
                                             const_CAST_array(c2));

        case CONTAINER_PAIR(RUN,RUN):
            return run_container_intersect(const_CAST_run(c1),
                                           const_CAST_run(c2));

        case CONTAINER_PAIR(BITSET,ARRAY):
            return array_bitset_container_intersect(const_CAST_array(c2),
                                                    const_CAST_bitset(c1));

        case CONTAINER_PAIR(ARRAY,BITSET):
            return array_bitset_container_intersect(const_CAST_array(c1),
                                                    const_CAST_bitset(c2));

        case CONTAINER_PAIR(BITSET,RUN):
            return run_bitset_container_intersect(const_CAST_run(c2),
                                                  const_CAST_bitset(c1));

        case CONTAINER_PAIR(RUN,BITSET):
            return run_bitset_container_intersect(const_CAST_run(c1),
                                                  const_CAST_bitset(c2));

        case CONTAINER_PAIR(ARRAY,RUN):
            return array_run_container_intersect(const_CAST_array(c1),
                                                 const_CAST_run(c2));

        case CONTAINER_PAIR(RUN,ARRAY):
            return array_run_container_intersect(const_CAST_array(c2),
                                                 const_CAST_run(c1));

        default:
            assert(false);
            roaring_unreachable;
            return 0;
    }
}

/**
 * Compute intersection between two containers, with result in the first
 container if possible. If the returned pointer is identical to c1,
 then the container has been modified. If the returned pointer is different
 from c1, then a new container has been created and the caller is responsible
 for freeing it.
 The type of the first container may change. Returns the modified
 (and possibly new) container.
*/
static inline container_t *container_iand(
    container_t *c1, uint8_t type1,
    const container_t *c2, uint8_t type2,
    uint8_t *result_type
){
    c1 = get_writable_copy_if_shared(c1, &type1);
    c2 = container_unwrap_shared(c2, &type2);
    container_t *result = NULL;
    switch (PAIR_CONTAINER_TYPES(type1, type2)) {
        case CONTAINER_PAIR(BITSET,BITSET):
            *result_type =
                bitset_bitset_container_intersection_inplace(
                    CAST_bitset(c1), const_CAST_bitset(c2), &result)
                        ? BITSET_CONTAINER_TYPE
                        : ARRAY_CONTAINER_TYPE;
            return result;

        case CONTAINER_PAIR(ARRAY,ARRAY):
            array_container_intersection_inplace(CAST_array(c1),
                                                 const_CAST_array(c2));
            *result_type = ARRAY_CONTAINER_TYPE;
            return c1;

        case CONTAINER_PAIR(RUN,RUN):
            result = run_container_create();
            run_container_intersection(const_CAST_run(c1),
                                       const_CAST_run(c2),
                                       CAST_run(result));
            // as of January 2016, Java code used non-in-place intersection for
            // two runcontainers
            return convert_run_to_efficient_container_and_free(
                            CAST_run(result), result_type);

        case CONTAINER_PAIR(BITSET,ARRAY):
            // c1 is a bitmap so no inplace possible
            result = array_container_create();
            array_bitset_container_intersection(const_CAST_array(c2),
                                                const_CAST_bitset(c1),
                                                CAST_array(result));
            *result_type = ARRAY_CONTAINER_TYPE;  // never bitset
            return result;

        case CONTAINER_PAIR(ARRAY,BITSET):
            *result_type = ARRAY_CONTAINER_TYPE;  // never bitset
            array_bitset_container_intersection(
                    const_CAST_array(c1), const_CAST_bitset(c2),
                    CAST_array(c1));  // result is allowed to be same as c1
            return c1;

        case CONTAINER_PAIR(BITSET,RUN):
            // will attempt in-place computation
            *result_type = run_bitset_container_intersection(
                                const_CAST_run(c2),
                                const_CAST_bitset(c1), &c1)
                                    ? BITSET_CONTAINER_TYPE
                                    : ARRAY_CONTAINER_TYPE;
            return c1;

        case CONTAINER_PAIR(RUN,BITSET):
            *result_type = run_bitset_container_intersection(
                                const_CAST_run(c1),
                                const_CAST_bitset(c2), &result)
                                    ? BITSET_CONTAINER_TYPE
                                    : ARRAY_CONTAINER_TYPE;
            return result;

        case CONTAINER_PAIR(ARRAY,RUN):
            result = array_container_create();
            *result_type = ARRAY_CONTAINER_TYPE;  // never bitset
            array_run_container_intersection(const_CAST_array(c1),
                                             const_CAST_run(c2),
                                             CAST_array(result));
            return result;

        case CONTAINER_PAIR(RUN,ARRAY):
            result = array_container_create();
            *result_type = ARRAY_CONTAINER_TYPE;  // never bitset
            array_run_container_intersection(const_CAST_array(c2),
                                             const_CAST_run(c1),
                                             CAST_array(result));
            return result;

        default:
            assert(false);
            roaring_unreachable;
            return NULL;
    }
}

/**
 * Compute union between two containers, generate a new container (having type
 * result_type), requires a typecode. This allocates new memory, caller
 * is responsible for deallocation.
 */
static inline container_t *container_or(
    const container_t *c1, uint8_t type1,
    const container_t *c2, uint8_t type2,
    uint8_t *result_type
){
    c1 = container_unwrap_shared(c1, &type1);
    c2 = container_unwrap_shared(c2, &type2);
    container_t *result = NULL;
    switch (PAIR_CONTAINER_TYPES(type1, type2)) {
        case CONTAINER_PAIR(BITSET,BITSET):
            result = bitset_container_create();
            bitset_container_or(const_CAST_bitset(c1),
                                const_CAST_bitset(c2),
                                CAST_bitset(result));
            *result_type = BITSET_CONTAINER_TYPE;
            return result;

        case CONTAINER_PAIR(ARRAY,ARRAY):
            *result_type = array_array_container_union(
                                const_CAST_array(c1),
                                const_CAST_array(c2), &result)
                                    ? BITSET_CONTAINER_TYPE
                                    : ARRAY_CONTAINER_TYPE;
            return result;

        case CONTAINER_PAIR(RUN,RUN):
            result = run_container_create();
            run_container_union(const_CAST_run(c1),
                                const_CAST_run(c2),
                                CAST_run(result));
            *result_type = RUN_CONTAINER_TYPE;
            // todo: could be optimized since will never convert to array
            result = convert_run_to_efficient_container_and_free(
                            CAST_run(result), result_type);
            return result;

        case CONTAINER_PAIR(BITSET,ARRAY):
            result = bitset_container_create();
            array_bitset_container_union(const_CAST_array(c2),
                                         const_CAST_bitset(c1),
                                         CAST_bitset(result));
            *result_type = BITSET_CONTAINER_TYPE;
            return result;

        case CONTAINER_PAIR(ARRAY,BITSET):
            result = bitset_container_create();
            array_bitset_container_union(const_CAST_array(c1),
                                         const_CAST_bitset(c2),
                                         CAST_bitset(result));
            *result_type = BITSET_CONTAINER_TYPE;
            return result;

        case CONTAINER_PAIR(BITSET,RUN):
            if (run_container_is_full(const_CAST_run(c2))) {
                result = run_container_create();
                *result_type = RUN_CONTAINER_TYPE;
                run_container_copy(const_CAST_run(c2),
                                   CAST_run(result));
                return result;
            }
            result = bitset_container_create();
            run_bitset_container_union(const_CAST_run(c2),
                                       const_CAST_bitset(c1),
                                       CAST_bitset(result));
            *result_type = BITSET_CONTAINER_TYPE;
            return result;

        case CONTAINER_PAIR(RUN,BITSET):
            if (run_container_is_full(const_CAST_run(c1))) {
                result = run_container_create();
                *result_type = RUN_CONTAINER_TYPE;
                run_container_copy(const_CAST_run(c1),
                                   CAST_run(result));
                return result;
            }
            result = bitset_container_create();
            run_bitset_container_union(const_CAST_run(c1),
                                       const_CAST_bitset(c2),
                                       CAST_bitset(result));
            *result_type = BITSET_CONTAINER_TYPE;
            return result;

        case CONTAINER_PAIR(ARRAY,RUN):
            result = run_container_create();
            array_run_container_union(const_CAST_array(c1),
                                      const_CAST_run(c2),
                                      CAST_run(result));
            result = convert_run_to_efficient_container_and_free(
                            CAST_run(result), result_type);
            return result;

        case CONTAINER_PAIR(RUN,ARRAY):
            result = run_container_create();
            array_run_container_union(const_CAST_array(c2),
                                      const_CAST_run(c1),
                                      CAST_run(result));
            result = convert_run_to_efficient_container_and_free(
                            CAST_run(result), result_type);
            return result;

        default:
            assert(false);
            roaring_unreachable;
            return NULL;  // unreached
    }
}

/**
 * Compute union between two containers, generate a new container (having type
 * result_type), requires a typecode. This allocates new memory, caller
 * is responsible for deallocation.
 *
 * This lazy version delays some operations such as the maintenance of the
 * cardinality. It requires repair later on the generated containers.
 */
static inline container_t *container_lazy_or(
    const container_t *c1, uint8_t type1,
    const container_t *c2, uint8_t type2,
    uint8_t *result_type
){
    c1 = container_unwrap_shared(c1, &type1);
    c2 = container_unwrap_shared(c2, &type2);
    container_t *result = NULL;
    switch (PAIR_CONTAINER_TYPES(type1, type2)) {
        case CONTAINER_PAIR(BITSET,BITSET):
            result = bitset_container_create();
            bitset_container_or_nocard(
                    const_CAST_bitset(c1), const_CAST_bitset(c2),
                    CAST_bitset(result));  // is lazy
            *result_type = BITSET_CONTAINER_TYPE;
            return result;

        case CONTAINER_PAIR(ARRAY,ARRAY):
            *result_type = array_array_container_lazy_union(
                                const_CAST_array(c1),
                                const_CAST_array(c2), &result)
                                    ? BITSET_CONTAINER_TYPE
                                    : ARRAY_CONTAINER_TYPE;
            return result;

        case CONTAINER_PAIR(RUN,RUN):
            result = run_container_create();
            run_container_union(const_CAST_run(c1),
                                const_CAST_run(c2),
                                CAST_run(result));
            *result_type = RUN_CONTAINER_TYPE;
            // we are being lazy
            result = convert_run_to_efficient_container_and_free(
                CAST_run(result), result_type);
            return result;

        case CONTAINER_PAIR(BITSET,ARRAY):
            result = bitset_container_create();
            array_bitset_container_lazy_union(
                    const_CAST_array(c2), const_CAST_bitset(c1),
                    CAST_bitset(result));  // is lazy
            *result_type = BITSET_CONTAINER_TYPE;
            return result;

        case CONTAINER_PAIR(ARRAY,BITSET):
            result = bitset_container_create();
            array_bitset_container_lazy_union(
                    const_CAST_array(c1), const_CAST_bitset(c2),
                    CAST_bitset(result));  // is lazy
            *result_type = BITSET_CONTAINER_TYPE;
            return result;

        case CONTAINER_PAIR(BITSET,RUN):
            if (run_container_is_full(const_CAST_run(c2))) {
                result = run_container_create();
                *result_type = RUN_CONTAINER_TYPE;
                run_container_copy(const_CAST_run(c2), CAST_run(result));
                return result;
            }
            result = bitset_container_create();
            run_bitset_container_lazy_union(
                const_CAST_run(c2), const_CAST_bitset(c1),
                CAST_bitset(result));  // is lazy
            *result_type = BITSET_CONTAINER_TYPE;
            return result;

        case CONTAINER_PAIR(RUN,BITSET):
            if (run_container_is_full(const_CAST_run(c1))) {
                result = run_container_create();
                *result_type = RUN_CONTAINER_TYPE;
                run_container_copy(const_CAST_run(c1), CAST_run(result));
                return result;
            }
            result = bitset_container_create();
            run_bitset_container_lazy_union(
                const_CAST_run(c1), const_CAST_bitset(c2),
                CAST_bitset(result));  // is lazy
            *result_type = BITSET_CONTAINER_TYPE;
            return result;

        case CONTAINER_PAIR(ARRAY,RUN):
            result = run_container_create();
            array_run_container_union(const_CAST_array(c1),
                                      const_CAST_run(c2),
                                      CAST_run(result));
            *result_type = RUN_CONTAINER_TYPE;
            // next line skipped since we are lazy
            // result = convert_run_to_efficient_container(result, result_type);
            return result;

        case CONTAINER_PAIR(RUN,ARRAY):
            result = run_container_create();
            array_run_container_union(
                const_CAST_array(c2), const_CAST_run(c1),
                CAST_run(result));  // TODO make lazy
            *result_type = RUN_CONTAINER_TYPE;
            // next line skipped since we are lazy
            // result = convert_run_to_efficient_container(result, result_type);
            return result;

        default:
            assert(false);
            roaring_unreachable;
            return NULL;  // unreached
    }
}

/**
 * Compute the union between two containers, with result in the first container.
 * If the returned pointer is identical to c1, then the container has been
 * modified.
 * If the returned pointer is different from c1, then a new container has been
 * created and the caller is responsible for freeing it.
 * The type of the first container may change. Returns the modified
 * (and possibly new) container
*/
static inline container_t *container_ior(
    container_t *c1, uint8_t type1,
    const container_t *c2, uint8_t type2,
    uint8_t *result_type
){
    c1 = get_writable_copy_if_shared(c1, &type1);
    c2 = container_unwrap_shared(c2, &type2);
    container_t *result = NULL;
    switch (PAIR_CONTAINER_TYPES(type1, type2)) {
        case CONTAINER_PAIR(BITSET,BITSET):
            bitset_container_or(const_CAST_bitset(c1),
                                const_CAST_bitset(c2),
                                CAST_bitset(c1));
#ifdef OR_BITSET_CONVERSION_TO_FULL
            if (CAST_bitset(c1)->cardinality == (1 << 16)) {  // we convert
                result = run_container_create_range(0, (1 << 16));
                *result_type = RUN_CONTAINER_TYPE;
                return result;
            }
#endif
            *result_type = BITSET_CONTAINER_TYPE;
            return c1;

        case CONTAINER_PAIR(ARRAY,ARRAY):
            *result_type = array_array_container_inplace_union(
                                CAST_array(c1), const_CAST_array(c2), &result)
                                    ? BITSET_CONTAINER_TYPE
                                    : ARRAY_CONTAINER_TYPE;
            if((result == NULL)
               && (*result_type == ARRAY_CONTAINER_TYPE)) {
                 return c1; // the computation was done in-place!
            }
            return result;

        case CONTAINER_PAIR(RUN,RUN):
            run_container_union_inplace(CAST_run(c1), const_CAST_run(c2));
            return convert_run_to_efficient_container(CAST_run(c1),
                                                      result_type);

        case CONTAINER_PAIR(BITSET,ARRAY):
            array_bitset_container_union(const_CAST_array(c2),
                                         const_CAST_bitset(c1),
                                         CAST_bitset(c1));
            *result_type = BITSET_CONTAINER_TYPE;  // never array
            return c1;

        case CONTAINER_PAIR(ARRAY,BITSET):
            // c1 is an array, so no in-place possible
            result = bitset_container_create();
            *result_type = BITSET_CONTAINER_TYPE;
            array_bitset_container_union(const_CAST_array(c1),
                                         const_CAST_bitset(c2),
                                         CAST_bitset(result));
            return result;

        case CONTAINER_PAIR(BITSET,RUN):
            if (run_container_is_full(const_CAST_run(c2))) {
                result = run_container_create();
                *result_type = RUN_CONTAINER_TYPE;
                run_container_copy(const_CAST_run(c2), CAST_run(result));
                return result;
            }
            run_bitset_container_union(const_CAST_run(c2),
                                       const_CAST_bitset(c1),
                                       CAST_bitset(c1));  // allowed
            *result_type = BITSET_CONTAINER_TYPE;
            return c1;

        case CONTAINER_PAIR(RUN,BITSET):
            if (run_container_is_full(const_CAST_run(c1))) {
                *result_type = RUN_CONTAINER_TYPE;
                return c1;
            }
            result = bitset_container_create();
            run_bitset_container_union(const_CAST_run(c1),
                                       const_CAST_bitset(c2),
                                       CAST_bitset(result));
            *result_type = BITSET_CONTAINER_TYPE;
            return result;

        case CONTAINER_PAIR(ARRAY,RUN):
            result = run_container_create();
            array_run_container_union(const_CAST_array(c1),
                                      const_CAST_run(c2),
                                      CAST_run(result));
            result = convert_run_to_efficient_container_and_free(
                            CAST_run(result), result_type);
            return result;

        case CONTAINER_PAIR(RUN,ARRAY):
            array_run_container_inplace_union(const_CAST_array(c2),
                                              CAST_run(c1));
            c1 = convert_run_to_efficient_container(CAST_run(c1),
                                                    result_type);
            return c1;

        default:
            assert(false);
            roaring_unreachable;
            return NULL;
    }
}

/**
 * Compute the union between two containers, with result in the first container.
 * If the returned pointer is identical to c1, then the container has been
 * modified.
 * If the returned pointer is different from c1, then a new container has been
 * created and the caller is responsible for freeing it.
 * The type of the first container may change. Returns the modified
 * (and possibly new) container
 *
 * This lazy version delays some operations such as the maintenance of the
 * cardinality. It requires repair later on the generated containers.
*/
static inline container_t *container_lazy_ior(
    container_t *c1, uint8_t type1,
    const container_t *c2, uint8_t type2,
    uint8_t *result_type
){
    assert(type1 != SHARED_CONTAINER_TYPE);
    // c1 = get_writable_copy_if_shared(c1,&type1);
    c2 = container_unwrap_shared(c2, &type2);
    container_t *result = NULL;
    switch (PAIR_CONTAINER_TYPES(type1, type2)) {
        case CONTAINER_PAIR(BITSET,BITSET):
#ifdef LAZY_OR_BITSET_CONVERSION_TO_FULL
            // if we have two bitsets, we might as well compute the cardinality
            bitset_container_or(const_CAST_bitset(c1),
                                const_CAST_bitset(c2),
                                CAST_bitset(c1));
            // it is possible that two bitsets can lead to a full container
            if (CAST_bitset(c1)->cardinality == (1 << 16)) {  // we convert
                result = run_container_create_range(0, (1 << 16));
                *result_type = RUN_CONTAINER_TYPE;
                return result;
            }
#else
            bitset_container_or_nocard(const_CAST_bitset(c1),
                                       const_CAST_bitset(c2),
                                       CAST_bitset(c1));

#endif
            *result_type = BITSET_CONTAINER_TYPE;
            return c1;

        case CONTAINER_PAIR(ARRAY,ARRAY):
            *result_type = array_array_container_lazy_inplace_union(
                                CAST_array(c1),
                                const_CAST_array(c2), &result)
                                    ? BITSET_CONTAINER_TYPE
                                    : ARRAY_CONTAINER_TYPE;
            if((result == NULL)
               && (*result_type == ARRAY_CONTAINER_TYPE)) {
                 return c1; // the computation was done in-place!
            }
            return result;

        case CONTAINER_PAIR(RUN,RUN):
            run_container_union_inplace(CAST_run(c1),
                                        const_CAST_run(c2));
            *result_type = RUN_CONTAINER_TYPE;
            return convert_run_to_efficient_container(CAST_run(c1),
                                                      result_type);

        case CONTAINER_PAIR(BITSET,ARRAY):
            array_bitset_container_lazy_union(
                    const_CAST_array(c2), const_CAST_bitset(c1),
                    CAST_bitset(c1));              // is lazy
            *result_type = BITSET_CONTAINER_TYPE;  // never array
            return c1;

        case CONTAINER_PAIR(ARRAY,BITSET):
            // c1 is an array, so no in-place possible
            result = bitset_container_create();
            *result_type = BITSET_CONTAINER_TYPE;
            array_bitset_container_lazy_union(
                    const_CAST_array(c1), const_CAST_bitset(c2),
                    CAST_bitset(result));  // is lazy
            return result;

        case CONTAINER_PAIR(BITSET,RUN):
            if (run_container_is_full(const_CAST_run(c2))) {
                result = run_container_create();
                *result_type = RUN_CONTAINER_TYPE;
                run_container_copy(const_CAST_run(c2),
                                   CAST_run(result));
                return result;
            }
            run_bitset_container_lazy_union(
                const_CAST_run(c2), const_CAST_bitset(c1),
                CAST_bitset(c1));  // allowed //  lazy
            *result_type = BITSET_CONTAINER_TYPE;
            return c1;

        case CONTAINER_PAIR(RUN,BITSET):
            if (run_container_is_full(const_CAST_run(c1))) {
                *result_type = RUN_CONTAINER_TYPE;
                return c1;
            }
            result = bitset_container_create();
            run_bitset_container_lazy_union(
                const_CAST_run(c1), const_CAST_bitset(c2),
                CAST_bitset(result));  //  lazy
            *result_type = BITSET_CONTAINER_TYPE;
            return result;

        case CONTAINER_PAIR(ARRAY,RUN):
            result = run_container_create();
            array_run_container_union(const_CAST_array(c1),
                                      const_CAST_run(c2),
                                      CAST_run(result));
            *result_type = RUN_CONTAINER_TYPE;
            // next line skipped since we are lazy
            // result = convert_run_to_efficient_container_and_free(result,
            // result_type);
            return result;

        case CONTAINER_PAIR(RUN,ARRAY):
            array_run_container_inplace_union(const_CAST_array(c2),
                                              CAST_run(c1));
            *result_type = RUN_CONTAINER_TYPE;
            // next line skipped since we are lazy
            // result = convert_run_to_efficient_container_and_free(result,
            // result_type);
            return c1;

        default:
            assert(false);
            roaring_unreachable;
            return NULL;
    }
}

/**
 * Compute symmetric difference (xor) between two containers, generate a new
 * container (having type result_type), requires a typecode. This allocates new
 * memory, caller is responsible for deallocation.
 */
static inline container_t* container_xor(
    const container_t *c1, uint8_t type1,
    const container_t *c2, uint8_t type2,
    uint8_t *result_type
){
    c1 = container_unwrap_shared(c1, &type1);
    c2 = container_unwrap_shared(c2, &type2);
    container_t *result = NULL;
    switch (PAIR_CONTAINER_TYPES(type1, type2)) {
        case CONTAINER_PAIR(BITSET,BITSET):
            *result_type = bitset_bitset_container_xor(
                                const_CAST_bitset(c1),
                                const_CAST_bitset(c2), &result)
                                    ? BITSET_CONTAINER_TYPE
                                    : ARRAY_CONTAINER_TYPE;
            return result;

        case CONTAINER_PAIR(ARRAY,ARRAY):
            *result_type = array_array_container_xor(
                                const_CAST_array(c1),
                                const_CAST_array(c2), &result)
                                    ? BITSET_CONTAINER_TYPE
                                    : ARRAY_CONTAINER_TYPE;
            return result;

        case CONTAINER_PAIR(RUN,RUN):
            *result_type =
                run_run_container_xor(const_CAST_run(c1),
                                      const_CAST_run(c2), &result);
            return result;

        case CONTAINER_PAIR(BITSET,ARRAY):
            *result_type = array_bitset_container_xor(
                                const_CAST_array(c2),
                                const_CAST_bitset(c1), &result)
                                    ? BITSET_CONTAINER_TYPE
                                    : ARRAY_CONTAINER_TYPE;
            return result;

        case CONTAINER_PAIR(ARRAY,BITSET):
            *result_type = array_bitset_container_xor(
                                const_CAST_array(c1),
                                const_CAST_bitset(c2), &result)
                                    ? BITSET_CONTAINER_TYPE
                                    : ARRAY_CONTAINER_TYPE;
            return result;

        case CONTAINER_PAIR(BITSET,RUN):
            *result_type = run_bitset_container_xor(
                                const_CAST_run(c2),
                                const_CAST_bitset(c1), &result)
                                    ? BITSET_CONTAINER_TYPE
                                    : ARRAY_CONTAINER_TYPE;
            return result;

        case CONTAINER_PAIR(RUN,BITSET):
            *result_type = run_bitset_container_xor(
                                const_CAST_run(c1),
                                const_CAST_bitset(c2), &result)
                                    ? BITSET_CONTAINER_TYPE
                                    : ARRAY_CONTAINER_TYPE;
            return result;

        case CONTAINER_PAIR(ARRAY,RUN):
            *result_type =
                array_run_container_xor(const_CAST_array(c1),
                                        const_CAST_run(c2), &result);
            return result;

        case CONTAINER_PAIR(RUN,ARRAY):
            *result_type =
                array_run_container_xor(const_CAST_array(c2),
                                        const_CAST_run(c1), &result);
            return result;

        default:
            assert(false);
            roaring_unreachable;
            return NULL;  // unreached
    }
}

/* Applies an offset to the non-empty container 'c'.
 * The results are stored in new containers returned via 'lo' and 'hi', for the
 * low and high halves of the result (where the low half matches the original key
 * and the high one corresponds to values for the following key).
 * Either one of 'lo' and 'hi' are allowed to be 'NULL', but not both.
 * Whenever one of them is not 'NULL', it should point to a 'NULL' container.
 * Whenever one of them is 'NULL' the shifted elements for that part will not be
 * computed.
 * If either of the resulting containers turns out to be empty, the pointed
 * container will remain 'NULL'.
 */
static inline void container_add_offset(const container_t *c, uint8_t type,
                                        container_t **lo, container_t **hi,
                                        uint16_t offset) {
    assert(offset != 0);
    assert(container_nonzero_cardinality(c, type));
    assert(lo != NULL || hi != NULL);
    assert(lo == NULL || *lo == NULL);
    assert(hi == NULL || *hi == NULL);

    switch (type) {
    case BITSET_CONTAINER_TYPE:
        bitset_container_offset(const_CAST_bitset(c), lo, hi, offset);
        break;
    case ARRAY_CONTAINER_TYPE:
        array_container_offset(const_CAST_array(c), lo, hi, offset);
        break;
    case RUN_CONTAINER_TYPE:
        run_container_offset(const_CAST_run(c), lo, hi, offset);
        break;
    default:
        assert(false);
        roaring_unreachable;
        break;
    }
}

/**
 * Compute xor between two containers, generate a new container (having type
 * result_type), requires a typecode. This allocates new memory, caller
 * is responsible for deallocation.
 *
 * This lazy version delays some operations such as the maintenance of the
 * cardinality. It requires repair later on the generated containers.
 */
static inline container_t *container_lazy_xor(
    const container_t *c1, uint8_t type1,
    const container_t *c2, uint8_t type2,
    uint8_t *result_type
){
    c1 = container_unwrap_shared(c1, &type1);
    c2 = container_unwrap_shared(c2, &type2);
    container_t *result = NULL;
    switch (PAIR_CONTAINER_TYPES(type1, type2)) {
        case CONTAINER_PAIR(BITSET,BITSET):
            result = bitset_container_create();
            bitset_container_xor_nocard(
                const_CAST_bitset(c1), const_CAST_bitset(c2),
                CAST_bitset(result));  // is lazy
            *result_type = BITSET_CONTAINER_TYPE;
            return result;

        case CONTAINER_PAIR(ARRAY,ARRAY):
            *result_type = array_array_container_lazy_xor(
                                const_CAST_array(c1),
                                const_CAST_array(c2), &result)
                                    ? BITSET_CONTAINER_TYPE
                                    : ARRAY_CONTAINER_TYPE;
            return result;

        case CONTAINER_PAIR(RUN,RUN):
            // nothing special done yet.
            *result_type =
                run_run_container_xor(const_CAST_run(c1),
                                      const_CAST_run(c2), &result);
            return result;

        case CONTAINER_PAIR(BITSET,ARRAY):
            result = bitset_container_create();
            *result_type = BITSET_CONTAINER_TYPE;
            array_bitset_container_lazy_xor(const_CAST_array(c2),
                                            const_CAST_bitset(c1),
                                            CAST_bitset(result));
            return result;

        case CONTAINER_PAIR(ARRAY,BITSET):
            result = bitset_container_create();
            *result_type = BITSET_CONTAINER_TYPE;
            array_bitset_container_lazy_xor(const_CAST_array(c1),
                                            const_CAST_bitset(c2),
                                            CAST_bitset(result));
            return result;

        case CONTAINER_PAIR(BITSET,RUN):
            result = bitset_container_create();
            run_bitset_container_lazy_xor(const_CAST_run(c2),
                                          const_CAST_bitset(c1),
                                          CAST_bitset(result));
            *result_type = BITSET_CONTAINER_TYPE;
            return result;

        case CONTAINER_PAIR(RUN,BITSET):
            result = bitset_container_create();
            run_bitset_container_lazy_xor(const_CAST_run(c1),
                                          const_CAST_bitset(c2),
                                          CAST_bitset(result));
            *result_type = BITSET_CONTAINER_TYPE;
            return result;

        case CONTAINER_PAIR(ARRAY,RUN):
            result = run_container_create();
            array_run_container_lazy_xor(const_CAST_array(c1),
                                         const_CAST_run(c2),
                                         CAST_run(result));
            *result_type = RUN_CONTAINER_TYPE;
            // next line skipped since we are lazy
            // result = convert_run_to_efficient_container(result, result_type);
            return result;

        case CONTAINER_PAIR(RUN,ARRAY):
            result = run_container_create();
            array_run_container_lazy_xor(const_CAST_array(c2),
                                         const_CAST_run(c1),
                                         CAST_run(result));
            *result_type = RUN_CONTAINER_TYPE;
            // next line skipped since we are lazy
            // result = convert_run_to_efficient_container(result, result_type);
            return result;

        default:
            assert(false);
            roaring_unreachable;
            return NULL;  // unreached
    }
}

/**
 * Compute the xor between two containers, with result in the first container.
 * If the returned pointer is identical to c1, then the container has been
 * modified.
 * If the returned pointer is different from c1, then a new container has been
 * created and the caller is responsible for freeing it.
 * The type of the first container may change. Returns the modified
 * (and possibly new) container
*/
static inline container_t *container_ixor(
    container_t *c1, uint8_t type1,
    const container_t *c2, uint8_t type2,
    uint8_t *result_type
){
    c1 = get_writable_copy_if_shared(c1, &type1);
    c2 = container_unwrap_shared(c2, &type2);
    container_t *result = NULL;
    switch (PAIR_CONTAINER_TYPES(type1, type2)) {
        case CONTAINER_PAIR(BITSET,BITSET):
            *result_type = bitset_bitset_container_ixor(
                                CAST_bitset(c1), const_CAST_bitset(c2), &result)
                                    ? BITSET_CONTAINER_TYPE
                                    : ARRAY_CONTAINER_TYPE;
            return result;

        case CONTAINER_PAIR(ARRAY,ARRAY):
            *result_type = array_array_container_ixor(
                                CAST_array(c1), const_CAST_array(c2), &result)
                                    ? BITSET_CONTAINER_TYPE
                                    : ARRAY_CONTAINER_TYPE;
            return result;

        case CONTAINER_PAIR(RUN,RUN):
            *result_type = run_run_container_ixor(
                CAST_run(c1), const_CAST_run(c2), &result);
            return result;

        case CONTAINER_PAIR(BITSET,ARRAY):
            *result_type = bitset_array_container_ixor(
                                CAST_bitset(c1), const_CAST_array(c2), &result)
                                    ? BITSET_CONTAINER_TYPE
                                    : ARRAY_CONTAINER_TYPE;
            return result;

        case CONTAINER_PAIR(ARRAY,BITSET):
            *result_type = array_bitset_container_ixor(
                                CAST_array(c1), const_CAST_bitset(c2), &result)
                                    ? BITSET_CONTAINER_TYPE
                                    : ARRAY_CONTAINER_TYPE;
            return result;

        case CONTAINER_PAIR(BITSET,RUN):
            *result_type =
                bitset_run_container_ixor(
                    CAST_bitset(c1), const_CAST_run(c2), &result)
                        ? BITSET_CONTAINER_TYPE
                        : ARRAY_CONTAINER_TYPE;

            return result;

        case CONTAINER_PAIR(RUN,BITSET):
            *result_type = run_bitset_container_ixor(
                                CAST_run(c1), const_CAST_bitset(c2), &result)
                                    ? BITSET_CONTAINER_TYPE
                                    : ARRAY_CONTAINER_TYPE;
            return result;

        case CONTAINER_PAIR(ARRAY,RUN):
            *result_type = array_run_container_ixor(
                                CAST_array(c1), const_CAST_run(c2), &result);
            return result;

        case CONTAINER_PAIR(RUN,ARRAY):
            *result_type = run_array_container_ixor(
                                CAST_run(c1), const_CAST_array(c2), &result);
            return result;

        default:
            assert(false);
            roaring_unreachable;
            return NULL;
    }
}

/**
 * Compute the xor between two containers, with result in the first container.
 * If the returned pointer is identical to c1, then the container has been
 * modified.
 * If the returned pointer is different from c1, then a new container has been
 * created and the caller is responsible for freeing it.
 * The type of the first container may change. Returns the modified
 * (and possibly new) container
 *
 * This lazy version delays some operations such as the maintenance of the
 * cardinality. It requires repair later on the generated containers.
*/
static inline container_t *container_lazy_ixor(
    container_t *c1, uint8_t type1,
    const container_t *c2, uint8_t type2,
    uint8_t *result_type
){
    assert(type1 != SHARED_CONTAINER_TYPE);
    // c1 = get_writable_copy_if_shared(c1,&type1);
    c2 = container_unwrap_shared(c2, &type2);
    switch (PAIR_CONTAINER_TYPES(type1, type2)) {
        case CONTAINER_PAIR(BITSET,BITSET):
            bitset_container_xor_nocard(CAST_bitset(c1),
                                        const_CAST_bitset(c2),
                                        CAST_bitset(c1));  // is lazy
            *result_type = BITSET_CONTAINER_TYPE;
            return c1;

        // TODO: other cases being lazy, esp. when we know inplace not likely
        // could see the corresponding code for union
        default:
            // we may have a dirty bitset (without a precomputed cardinality)
            // and calling container_ixor on it might be unsafe.
            if (type1 == BITSET_CONTAINER_TYPE) {
                bitset_container_t *bc = CAST_bitset(c1);
                if (bc->cardinality == BITSET_UNKNOWN_CARDINALITY) {
                    bc->cardinality = bitset_container_compute_cardinality(bc);
                }
            }
            return container_ixor(c1, type1, c2, type2, result_type);
    }
}

/**
 * Compute difference (andnot) between two containers, generate a new
 * container (having type result_type), requires a typecode. This allocates new
 * memory, caller is responsible for deallocation.
 */
static inline container_t *container_andnot(
    const container_t *c1, uint8_t type1,
    const container_t *c2, uint8_t type2,
    uint8_t *result_type
){
    c1 = container_unwrap_shared(c1, &type1);
    c2 = container_unwrap_shared(c2, &type2);
    container_t *result = NULL;
    switch (PAIR_CONTAINER_TYPES(type1, type2)) {
        case CONTAINER_PAIR(BITSET,BITSET):
            *result_type = bitset_bitset_container_andnot(
                                const_CAST_bitset(c1),
                                const_CAST_bitset(c2), &result)
                                    ? BITSET_CONTAINER_TYPE
                                    : ARRAY_CONTAINER_TYPE;
            return result;

        case CONTAINER_PAIR(ARRAY,ARRAY):
            result = array_container_create();
            array_array_container_andnot(const_CAST_array(c1),
                                         const_CAST_array(c2),
                                         CAST_array(result));
            *result_type = ARRAY_CONTAINER_TYPE;
            return result;

        case CONTAINER_PAIR(RUN,RUN):
            if (run_container_is_full(const_CAST_run(c2))) {
                result = array_container_create();
                *result_type = ARRAY_CONTAINER_TYPE;
                return result;
            }
            *result_type =
                run_run_container_andnot(const_CAST_run(c1),
                                         const_CAST_run(c2), &result);
            return result;

        case CONTAINER_PAIR(BITSET,ARRAY):
            *result_type = bitset_array_container_andnot(
                                const_CAST_bitset(c1),
                                const_CAST_array(c2), &result)
                                    ? BITSET_CONTAINER_TYPE
                                    : ARRAY_CONTAINER_TYPE;
            return result;

        case CONTAINER_PAIR(ARRAY,BITSET):
            result = array_container_create();
            array_bitset_container_andnot(const_CAST_array(c1),
                                          const_CAST_bitset(c2),
                                          CAST_array(result));
            *result_type = ARRAY_CONTAINER_TYPE;
            return result;

        case CONTAINER_PAIR(BITSET,RUN):
            if (run_container_is_full(const_CAST_run(c2))) {
                result = array_container_create();
                *result_type = ARRAY_CONTAINER_TYPE;
                return result;
            }
            *result_type = bitset_run_container_andnot(
                                const_CAST_bitset(c1),
                                const_CAST_run(c2), &result)
                                    ? BITSET_CONTAINER_TYPE
                                    : ARRAY_CONTAINER_TYPE;
            return result;

        case CONTAINER_PAIR(RUN,BITSET):
            *result_type = run_bitset_container_andnot(
                                const_CAST_run(c1),
                                const_CAST_bitset(c2), &result)
                                    ? BITSET_CONTAINER_TYPE
                                    : ARRAY_CONTAINER_TYPE;
            return result;

        case CONTAINER_PAIR(ARRAY,RUN):
            if (run_container_is_full(const_CAST_run(c2))) {
                result = array_container_create();
                *result_type = ARRAY_CONTAINER_TYPE;
                return result;
            }
            result = array_container_create();
            array_run_container_andnot(const_CAST_array(c1),
                                       const_CAST_run(c2),
                                       CAST_array(result));
            *result_type = ARRAY_CONTAINER_TYPE;
            return result;

        case CONTAINER_PAIR(RUN,ARRAY):
            *result_type = run_array_container_andnot(
                const_CAST_run(c1), const_CAST_array(c2),
                &result);
            return result;

        default:
            assert(false);
            roaring_unreachable;
            return NULL;  // unreached
    }
}

/**
 * Compute the andnot between two containers, with result in the first
 * container.
 * If the returned pointer is identical to c1, then the container has been
 * modified.
 * If the returned pointer is different from c1, then a new container has been
 * created and the caller is responsible for freeing it.
 * The type of the first container may change. Returns the modified
 * (and possibly new) container
*/
static inline container_t *container_iandnot(
    container_t *c1, uint8_t type1,
    const container_t *c2, uint8_t type2,
    uint8_t *result_type
){
    c1 = get_writable_copy_if_shared(c1, &type1);
    c2 = container_unwrap_shared(c2, &type2);
    container_t *result = NULL;
    switch (PAIR_CONTAINER_TYPES(type1, type2)) {
        case CONTAINER_PAIR(BITSET,BITSET):
            *result_type = bitset_bitset_container_iandnot(
                                CAST_bitset(c1),
                                const_CAST_bitset(c2), &result)
                                    ? BITSET_CONTAINER_TYPE
                                    : ARRAY_CONTAINER_TYPE;
            return result;

        case CONTAINER_PAIR(ARRAY,ARRAY):
            array_array_container_iandnot(CAST_array(c1),
                                          const_CAST_array(c2));
            *result_type = ARRAY_CONTAINER_TYPE;
            return c1;

        case CONTAINER_PAIR(RUN,RUN):
            *result_type = run_run_container_iandnot(
                CAST_run(c1), const_CAST_run(c2), &result);
            return result;

        case CONTAINER_PAIR(BITSET,ARRAY):
            *result_type = bitset_array_container_iandnot(
                                CAST_bitset(c1),
                                const_CAST_array(c2), &result)
                                    ? BITSET_CONTAINER_TYPE
                                    : ARRAY_CONTAINER_TYPE;
            return result;

        case CONTAINER_PAIR(ARRAY,BITSET):
            *result_type = ARRAY_CONTAINER_TYPE;
            array_bitset_container_iandnot(CAST_array(c1),
                                           const_CAST_bitset(c2));
            return c1;

        case CONTAINER_PAIR(BITSET,RUN):
            *result_type = bitset_run_container_iandnot(
                                CAST_bitset(c1),
                                const_CAST_run(c2), &result)
                                    ? BITSET_CONTAINER_TYPE
                                    : ARRAY_CONTAINER_TYPE;
            return result;

        case CONTAINER_PAIR(RUN,BITSET):
            *result_type = run_bitset_container_iandnot(
                                CAST_run(c1),
                                const_CAST_bitset(c2), &result)
                                    ? BITSET_CONTAINER_TYPE
                                    : ARRAY_CONTAINER_TYPE;
            return result;

        case CONTAINER_PAIR(ARRAY,RUN):
            *result_type = ARRAY_CONTAINER_TYPE;
            array_run_container_iandnot(CAST_array(c1),
                                        const_CAST_run(c2));
            return c1;

        case CONTAINER_PAIR(RUN,ARRAY):
            *result_type = run_array_container_iandnot(
                CAST_run(c1), const_CAST_array(c2), &result);
            return result;

        default:
            assert(false);
            roaring_unreachable;
            return NULL;
    }
}

/**
 * Visit all values x of the container once, passing (base+x,ptr)
 * to iterator. You need to specify a container and its type.
 * Returns true if the iteration should continue.
 */
static inline bool container_iterate(
    const container_t *c, uint8_t type,
    uint32_t base,
    roaring_iterator iterator, void *ptr
){
    c = container_unwrap_shared(c, &type);
    switch (type) {
        case BITSET_CONTAINER_TYPE:
            return bitset_container_iterate(const_CAST_bitset(c),
                                            base, iterator, ptr);
        case ARRAY_CONTAINER_TYPE:
            return array_container_iterate(const_CAST_array(c),
                                           base, iterator, ptr);
        case RUN_CONTAINER_TYPE:
            return run_container_iterate(const_CAST_run(c),
                                         base, iterator, ptr);
        default:
            assert(false);
            roaring_unreachable;
    }
    assert(false);
    roaring_unreachable;
    return false;
}

static inline bool container_iterate64(
    const container_t *c, uint8_t type,
    uint32_t base,
    roaring_iterator64 iterator,
    uint64_t high_bits, void *ptr
){
    c = container_unwrap_shared(c, &type);
    switch (type) {
        case BITSET_CONTAINER_TYPE:
            return bitset_container_iterate64(const_CAST_bitset(c), base,
                                              iterator, high_bits, ptr);
        case ARRAY_CONTAINER_TYPE:
            return array_container_iterate64(const_CAST_array(c), base,
                                             iterator, high_bits, ptr);
        case RUN_CONTAINER_TYPE:
            return run_container_iterate64(const_CAST_run(c), base,
                                           iterator, high_bits, ptr);
        default:
            assert(false);
            roaring_unreachable;
    }
    assert(false);
    roaring_unreachable;
    return false;
}

static inline container_t *container_not(
    const container_t *c, uint8_t type,
    uint8_t *result_type
){
    c = container_unwrap_shared(c, &type);
    container_t *result = NULL;
    switch (type) {
        case BITSET_CONTAINER_TYPE:
            *result_type = bitset_container_negation(
                                const_CAST_bitset(c), &result)
                                    ? BITSET_CONTAINER_TYPE
                                    : ARRAY_CONTAINER_TYPE;
            return result;
        case ARRAY_CONTAINER_TYPE:
            result = bitset_container_create();
            *result_type = BITSET_CONTAINER_TYPE;
            array_container_negation(const_CAST_array(c),
                                     CAST_bitset(result));
            return result;
        case RUN_CONTAINER_TYPE:
            *result_type =
                run_container_negation(const_CAST_run(c), &result);
            return result;

        default:
            assert(false);
            roaring_unreachable;
    }
    assert(false);
    roaring_unreachable;
    return NULL;
}

static inline container_t *container_not_range(
    const container_t *c, uint8_t type,
    uint32_t range_start, uint32_t range_end,
    uint8_t *result_type
){
    c = container_unwrap_shared(c, &type);
    container_t *result = NULL;
    switch (type) {
        case BITSET_CONTAINER_TYPE:
            *result_type =
                bitset_container_negation_range(
                        const_CAST_bitset(c), range_start, range_end, &result)
                            ? BITSET_CONTAINER_TYPE
                            : ARRAY_CONTAINER_TYPE;
            return result;
        case ARRAY_CONTAINER_TYPE:
            *result_type =
                array_container_negation_range(
                    const_CAST_array(c), range_start, range_end, &result)
                        ? BITSET_CONTAINER_TYPE
                        : ARRAY_CONTAINER_TYPE;
            return result;
        case RUN_CONTAINER_TYPE:
            *result_type = run_container_negation_range(
                            const_CAST_run(c), range_start, range_end, &result);
            return result;

        default:
            assert(false);
            roaring_unreachable;
    }
    assert(false);
    roaring_unreachable;
    return NULL;
}

static inline container_t *container_inot(
    container_t *c, uint8_t type,
    uint8_t *result_type
){
    c = get_writable_copy_if_shared(c, &type);
    container_t *result = NULL;
    switch (type) {
        case BITSET_CONTAINER_TYPE:
            *result_type = bitset_container_negation_inplace(
                                CAST_bitset(c), &result)
                                    ? BITSET_CONTAINER_TYPE
                                    : ARRAY_CONTAINER_TYPE;
            return result;
        case ARRAY_CONTAINER_TYPE:
            // will never be inplace
            result = bitset_container_create();
            *result_type = BITSET_CONTAINER_TYPE;
            array_container_negation(CAST_array(c),
                                     CAST_bitset(result));
            array_container_free(CAST_array(c));
            return result;
        case RUN_CONTAINER_TYPE:
            *result_type =
                run_container_negation_inplace(CAST_run(c), &result);
            return result;

        default:
            assert(false);
            roaring_unreachable;
    }
    assert(false);
    roaring_unreachable;
    return NULL;
}

static inline container_t *container_inot_range(
    container_t *c, uint8_t type,
    uint32_t range_start, uint32_t range_end,
    uint8_t *result_type
){
    c = get_writable_copy_if_shared(c, &type);
    container_t *result = NULL;
    switch (type) {
        case BITSET_CONTAINER_TYPE:
            *result_type =
                bitset_container_negation_range_inplace(
                    CAST_bitset(c), range_start, range_end, &result)
                        ? BITSET_CONTAINER_TYPE
                        : ARRAY_CONTAINER_TYPE;
            return result;
        case ARRAY_CONTAINER_TYPE:
            *result_type =
                array_container_negation_range_inplace(
                    CAST_array(c), range_start, range_end, &result)
                        ? BITSET_CONTAINER_TYPE
                        : ARRAY_CONTAINER_TYPE;
            return result;
        case RUN_CONTAINER_TYPE:
            *result_type = run_container_negation_range_inplace(
                                CAST_run(c), range_start, range_end, &result);
            return result;

        default:
            assert(false);
            roaring_unreachable;
    }
    assert(false);
    roaring_unreachable;
    return NULL;
}

/**
 * If the element of given rank is in this container, supposing that
 * the first
 * element has rank start_rank, then the function returns true and
 * sets element
 * accordingly.
 * Otherwise, it returns false and update start_rank.
 */
static inline bool container_select(
    const container_t *c, uint8_t type,
    uint32_t *start_rank, uint32_t rank,
    uint32_t *element
){
    c = container_unwrap_shared(c, &type);
    switch (type) {
        case BITSET_CONTAINER_TYPE:
            return bitset_container_select(const_CAST_bitset(c),
                                           start_rank, rank, element);
        case ARRAY_CONTAINER_TYPE:
            return array_container_select(const_CAST_array(c),
                                          start_rank, rank, element);
        case RUN_CONTAINER_TYPE:
            return run_container_select(const_CAST_run(c),
                                        start_rank, rank, element);
        default:
            assert(false);
            roaring_unreachable;
    }
    assert(false);
    roaring_unreachable;
    return false;
}

static inline uint16_t container_maximum(
    const container_t *c, uint8_t type
){
    c = container_unwrap_shared(c, &type);
    switch (type) {
        case BITSET_CONTAINER_TYPE:
            return bitset_container_maximum(const_CAST_bitset(c));
        case ARRAY_CONTAINER_TYPE:
            return array_container_maximum(const_CAST_array(c));
        case RUN_CONTAINER_TYPE:
            return run_container_maximum(const_CAST_run(c));
        default:
            assert(false);
            roaring_unreachable;
    }
    assert(false);
    roaring_unreachable;
    return false;
}

static inline uint16_t container_minimum(
    const container_t *c, uint8_t type
){
    c = container_unwrap_shared(c, &type);
    switch (type) {
        case BITSET_CONTAINER_TYPE:
            return bitset_container_minimum(const_CAST_bitset(c));
        case ARRAY_CONTAINER_TYPE:
            return array_container_minimum(const_CAST_array(c));
        case RUN_CONTAINER_TYPE:
            return run_container_minimum(const_CAST_run(c));
        default:
            assert(false);
            roaring_unreachable;
    }
    assert(false);
    roaring_unreachable;
    return false;
}

// number of values smaller or equal to x
static inline int container_rank(
    const container_t *c, uint8_t type,
    uint16_t x
){
    c = container_unwrap_shared(c, &type);
    switch (type) {
        case BITSET_CONTAINER_TYPE:
            return bitset_container_rank(const_CAST_bitset(c), x);
        case ARRAY_CONTAINER_TYPE:
            return array_container_rank(const_CAST_array(c), x);
        case RUN_CONTAINER_TYPE:
            return run_container_rank(const_CAST_run(c), x);
        default:
            assert(false);
            roaring_unreachable;
    }
    assert(false);
    roaring_unreachable;
    return false;
}

/**
 * Add all values in range [min, max] to a given container.
 *
 * If the returned pointer is different from $container, then a new container
 * has been created and the caller is responsible for freeing it.
 * The type of the first container may change. Returns the modified
 * (and possibly new) container.
 */
static inline container_t *container_add_range(
    container_t *c, uint8_t type,
    uint32_t min, uint32_t max,
    uint8_t *result_type
){
    // NB: when selecting new container type, we perform only inexpensive checks
    switch (type) {
        case BITSET_CONTAINER_TYPE: {
            bitset_container_t *bitset = CAST_bitset(c);

            int32_t union_cardinality = 0;
            union_cardinality += bitset->cardinality;
            union_cardinality += max - min + 1;
            union_cardinality -= bitset_lenrange_cardinality(bitset->words,
                                                             min, max-min);

            if (union_cardinality == INT32_C(0x10000)) {
                *result_type = RUN_CONTAINER_TYPE;
                return run_container_create_range(0, INT32_C(0x10000));
            } else {
                *result_type = BITSET_CONTAINER_TYPE;
                bitset_set_lenrange(bitset->words, min, max - min);
                bitset->cardinality = union_cardinality;
                return bitset;
            }
        }
        case ARRAY_CONTAINER_TYPE: {
            array_container_t *array = CAST_array(c);

            int32_t nvals_greater = count_greater(array->array, array->cardinality, max);
            int32_t nvals_less = count_less(array->array, array->cardinality - nvals_greater, min);
            int32_t union_cardinality = nvals_less + (max - min + 1) + nvals_greater;

            if (union_cardinality == INT32_C(0x10000)) {
                *result_type = RUN_CONTAINER_TYPE;
                return run_container_create_range(0, INT32_C(0x10000));
            } else if (union_cardinality <= DEFAULT_MAX_SIZE) {
                *result_type = ARRAY_CONTAINER_TYPE;
                array_container_add_range_nvals(array, min, max, nvals_less, nvals_greater);
                return array;
            } else {
                *result_type = BITSET_CONTAINER_TYPE;
                bitset_container_t *bitset = bitset_container_from_array(array);
                bitset_set_lenrange(bitset->words, min, max - min);
                bitset->cardinality = union_cardinality;
                return bitset;
            }
        }
        case RUN_CONTAINER_TYPE: {
            run_container_t *run = CAST_run(c);

            int32_t nruns_greater = rle16_count_greater(run->runs, run->n_runs, max);
            int32_t nruns_less = rle16_count_less(run->runs, run->n_runs - nruns_greater, min);

            int32_t run_size_bytes = (nruns_less + 1 + nruns_greater) * sizeof(rle16_t);
            int32_t bitset_size_bytes = BITSET_CONTAINER_SIZE_IN_WORDS * sizeof(uint64_t);

            if (run_size_bytes <= bitset_size_bytes) {
                run_container_add_range_nruns(run, min, max, nruns_less, nruns_greater);
                *result_type = RUN_CONTAINER_TYPE;
                return run;
            } else {
                return container_from_run_range(run, min, max, result_type);
            }
        }
        default:
            roaring_unreachable;
    }
}

/*
 * Removes all elements in range [min, max].
 * Returns one of:
 *   - NULL if no elements left
 *   - pointer to the original container
 *   - pointer to a newly-allocated container (if it is more efficient)
 *
 * If the returned pointer is different from $container, then a new container
 * has been created and the caller is responsible for freeing the original container.
 */
static inline container_t *container_remove_range(
    container_t *c, uint8_t type,
    uint32_t min, uint32_t max,
    uint8_t *result_type
){
     switch (type) {
        case BITSET_CONTAINER_TYPE: {
            bitset_container_t *bitset = CAST_bitset(c);

            int32_t result_cardinality = bitset->cardinality -
                bitset_lenrange_cardinality(bitset->words, min, max-min);

            if (result_cardinality == 0) {
                return NULL;
            } else if (result_cardinality <= DEFAULT_MAX_SIZE) {
                *result_type = ARRAY_CONTAINER_TYPE;
                bitset_reset_range(bitset->words, min, max+1);
                bitset->cardinality = result_cardinality;
                return array_container_from_bitset(bitset);
            } else {
                *result_type = BITSET_CONTAINER_TYPE;
                bitset_reset_range(bitset->words, min, max+1);
                bitset->cardinality = result_cardinality;
                return bitset;
            }
        }
        case ARRAY_CONTAINER_TYPE: {
            array_container_t *array = CAST_array(c);

            int32_t nvals_greater = count_greater(array->array, array->cardinality, max);
            int32_t nvals_less = count_less(array->array, array->cardinality - nvals_greater, min);
            int32_t result_cardinality = nvals_less + nvals_greater;

            if (result_cardinality == 0) {
                return NULL;
            } else {
                *result_type = ARRAY_CONTAINER_TYPE;
                array_container_remove_range(array, nvals_less,
                    array->cardinality - result_cardinality);
                return array;
            }
        }
        case RUN_CONTAINER_TYPE: {
            run_container_t *run = CAST_run(c);

            if (run->n_runs == 0) {
                return NULL;
            }
            if (min <= run_container_minimum(run) && max >= run_container_maximum(run)) {
                return NULL;
            }

            run_container_remove_range(run, min, max);
            return convert_run_to_efficient_container(run, result_type);
        }
        default:
            roaring_unreachable;
     }
}

#ifdef __cplusplus
} } }  // extern "C" { namespace roaring { namespace internal {
#endif

#endif
/* end file include/roaring/containers/containers.h */
/* begin file include/roaring/roaring_array.h */
#ifndef INCLUDE_ROARING_ARRAY_H
#define INCLUDE_ROARING_ARRAY_H

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" { namespace roaring {

// Note: in pure C++ code, you should avoid putting `using` in header files
using api::roaring_array_t;

namespace internal {
#endif

enum {
    SERIAL_COOKIE_NO_RUNCONTAINER = 12346,
    SERIAL_COOKIE = 12347,
    FROZEN_COOKIE = 13766,
    NO_OFFSET_THRESHOLD = 4
};

/**
 * Create a new roaring array
 */
roaring_array_t *ra_create(void);

/**
 * Initialize an existing roaring array with the specified capacity (in number
 * of containers)
 */
bool ra_init_with_capacity(roaring_array_t *new_ra, uint32_t cap);

/**
 * Initialize with zero capacity
 */
void ra_init(roaring_array_t *t);

/**
 * Copies this roaring array, we assume that dest is not initialized
 */
bool ra_copy(const roaring_array_t *source, roaring_array_t *dest,
             bool copy_on_write);

/*
 * Shrinks the capacity, returns the number of bytes saved.
 */
int ra_shrink_to_fit(roaring_array_t *ra);

/**
 * Copies this roaring array, we assume that dest is initialized
 */
bool ra_overwrite(const roaring_array_t *source, roaring_array_t *dest,
                  bool copy_on_write);

/**
 * Frees the memory used by a roaring array
 */
void ra_clear(roaring_array_t *r);

/**
 * Frees the memory used by a roaring array, but does not free the containers
 */
void ra_clear_without_containers(roaring_array_t *r);

/**
 * Frees just the containers
 */
void ra_clear_containers(roaring_array_t *ra);

/**
 * Get the index corresponding to a 16-bit key
 */
inline int32_t ra_get_index(const roaring_array_t *ra, uint16_t x) {
    if ((ra->size == 0) || ra->keys[ra->size - 1] == x) return ra->size - 1;
    return binarySearch(ra->keys, (int32_t)ra->size, x);
}

/**
 * Retrieves the container at index i, filling in the typecode
 */
inline container_t *ra_get_container_at_index(
    const roaring_array_t *ra, uint16_t i, uint8_t *typecode
){
    *typecode = ra->typecodes[i];
    return ra->containers[i];
}

/**
 * Retrieves the key at index i
 */
inline uint16_t ra_get_key_at_index(const roaring_array_t *ra, uint16_t i) {
    return ra->keys[i];
}

/**
 * Add a new key-value pair at index i
 */
void ra_insert_new_key_value_at(
        roaring_array_t *ra, int32_t i, uint16_t key,
        container_t *c, uint8_t typecode);

/**
 * Append a new key-value pair
 */
void ra_append(
        roaring_array_t *ra, uint16_t key,
        container_t *c, uint8_t typecode);

/**
 * Append a new key-value pair to ra, cloning (in COW sense) a value from sa
 * at index index
 */
void ra_append_copy(roaring_array_t *ra, const roaring_array_t *sa,
                    uint16_t index, bool copy_on_write);

/**
 * Append new key-value pairs to ra, cloning (in COW sense)  values from sa
 * at indexes
 * [start_index, end_index)
 */
void ra_append_copy_range(roaring_array_t *ra, const roaring_array_t *sa,
                          int32_t start_index, int32_t end_index,
                          bool copy_on_write);

/** appends from sa to ra, ending with the greatest key that is
 * is less or equal stopping_key
 */
void ra_append_copies_until(roaring_array_t *ra, const roaring_array_t *sa,
                            uint16_t stopping_key, bool copy_on_write);

/** appends from sa to ra, starting with the smallest key that is
 * is strictly greater than before_start
 */

void ra_append_copies_after(roaring_array_t *ra, const roaring_array_t *sa,
                            uint16_t before_start, bool copy_on_write);

/**
 * Move the key-value pairs to ra from sa at indexes
 * [start_index, end_index), old array should not be freed
 * (use ra_clear_without_containers)
 **/
void ra_append_move_range(roaring_array_t *ra, roaring_array_t *sa,
                          int32_t start_index, int32_t end_index);
/**
 * Append new key-value pairs to ra,  from sa at indexes
 * [start_index, end_index)
 */
void ra_append_range(roaring_array_t *ra, roaring_array_t *sa,
                     int32_t start_index, int32_t end_index,
                     bool copy_on_write);

/**
 * Set the container at the corresponding index using the specified
 * typecode.
 */
inline void ra_set_container_at_index(
    const roaring_array_t *ra, int32_t i,
    container_t *c, uint8_t typecode
){
    assert(i < ra->size);
    ra->containers[i] = c;
    ra->typecodes[i] = typecode;
}

/**
 * If needed, increase the capacity of the array so that it can fit k values
 * (at
 * least);
 */
bool extend_array(roaring_array_t *ra, int32_t k);

inline int32_t ra_get_size(const roaring_array_t *ra) { return ra->size; }

static inline int32_t ra_advance_until(const roaring_array_t *ra, uint16_t x,
                                       int32_t pos) {
    return advanceUntil(ra->keys, pos, ra->size, x);
}

int32_t ra_advance_until_freeing(roaring_array_t *ra, uint16_t x, int32_t pos);

void ra_downsize(roaring_array_t *ra, int32_t new_length);

inline void ra_replace_key_and_container_at_index(
    roaring_array_t *ra, int32_t i, uint16_t key,
    container_t *c, uint8_t typecode
){
    assert(i < ra->size);

    ra->keys[i] = key;
    ra->containers[i] = c;
    ra->typecodes[i] = typecode;
}

// write set bits to an array
void ra_to_uint32_array(const roaring_array_t *ra, uint32_t *ans);

bool ra_range_uint32_array(const roaring_array_t *ra, size_t offset, size_t limit, uint32_t *ans);

/**
 * write a bitmap to a buffer. This is meant to be compatible with
 * the
 * Java and Go versions. Return the size in bytes of the serialized
 * output (which should be ra_portable_size_in_bytes(ra)).
 */
size_t ra_portable_serialize(const roaring_array_t *ra, char *buf);

/**
 * read a bitmap from a serialized version. This is meant to be compatible
 * with the Java and Go versions.
 * maxbytes  indicates how many bytes available from buf.
 * When the function returns true, roaring_array_t is populated with the data
 * and *readbytes indicates how many bytes were read. In all cases, if the function
 * returns true, then maxbytes >= *readbytes.
 */
bool ra_portable_deserialize(roaring_array_t *ra, const char *buf, const size_t maxbytes, size_t * readbytes);

/**
 * Quickly checks whether there is a serialized bitmap at the pointer,
 * not exceeding size "maxbytes" in bytes. This function does not allocate
 * memory dynamically.
 *
 * This function returns 0 if and only if no valid bitmap is found.
 * Otherwise, it returns how many bytes are occupied by the bitmap data.
 */
size_t ra_portable_deserialize_size(const char *buf, const size_t maxbytes);

/**
 * How many bytes are required to serialize this bitmap (meant to be
 * compatible
 * with Java and Go versions)
 */
size_t ra_portable_size_in_bytes(const roaring_array_t *ra);

/**
 * return true if it contains at least one run container.
 */
bool ra_has_run_container(const roaring_array_t *ra);

/**
 * Size of the header when serializing (meant to be compatible
 * with Java and Go versions)
 */
uint32_t ra_portable_header_size(const roaring_array_t *ra);

/**
 * If the container at the index i is share, unshare it (creating a local
 * copy if needed).
 */
static inline void ra_unshare_container_at_index(roaring_array_t *ra,
                                                 uint16_t i) {
    assert(i < ra->size);
    ra->containers[i] = get_writable_copy_if_shared(ra->containers[i],
                                                    &ra->typecodes[i]);
}

/**
 * remove at index i, sliding over all entries after i
 */
void ra_remove_at_index(roaring_array_t *ra, int32_t i);


/**
* clears all containers, sets the size at 0 and shrinks the memory usage.
*/
void ra_reset(roaring_array_t *ra);

/**
 * remove at index i, sliding over all entries after i. Free removed container.
 */
void ra_remove_at_index_and_free(roaring_array_t *ra, int32_t i);

/**
 * remove a chunk of indices, sliding over entries after it
 */
// void ra_remove_index_range(roaring_array_t *ra, int32_t begin, int32_t end);

// used in inplace andNot only, to slide left the containers from
// the mutated RoaringBitmap that are after the largest container of
// the argument RoaringBitmap.  It is followed by a call to resize.
//
void ra_copy_range(roaring_array_t *ra, uint32_t begin, uint32_t end,
                   uint32_t new_begin);

/**
 * Shifts rightmost $count containers to the left (distance < 0) or
 * to the right (distance > 0).
 * Allocates memory if necessary.
 * This function doesn't free or create new containers.
 * Caller is responsible for that.
 */
void ra_shift_tail(roaring_array_t *ra, int32_t count, int32_t distance);

#ifdef __cplusplus
}  // namespace internal
} }  // extern "C" { namespace roaring {
#endif

#endif
/* end file include/roaring/roaring_array.h */
