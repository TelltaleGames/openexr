///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2012, Industrial Light & Magic, a division of Lucas
// Digital Ltd. LLC
// 
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
// *       Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
// *       Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
// *       Neither the name of Industrial Light & Magic nor the names of
// its contributors may be used to endorse or promote products derived
// from this software without specific prior written permission. 
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////

#ifndef INCLUDED_IMF_COMPILER_SPECIFIC_H
#define INCLUDED_IMF_COMPILER_SPECIFIC_H

#include "ImfNamespace.h"
#include "ImfSimd.h"
#include <stdlib.h>
#include "ImfExport.h"
#include "OpenEXRConfig.h"
#include "OpenEXRConfigInternal.h"

OPENEXR_IMF_INTERNAL_NAMESPACE_HEADER_ENTER

//
// Test if we should enable GCC inline asm paths for AVX
//

#if defined(OPENEXR_IMF_HAVE_GCC_INLINE_ASM_AVX) && (defined(_M_X64) || defined(__x86_64__))

    #define IMF_HAVE_GCC_INLINEASM_X86

    #ifdef __LP64__
        #define IMF_HAVE_GCC_INLINEASM_X86_64
    #endif /* __LP64__ */

#endif /* OPENEXR_IMF_HAVE_GCC_INLINE_ASM_AVX */

static unsigned long  systemEndianCheckValue   = 0x12345678;
static unsigned long* systemEndianCheckPointer = &systemEndianCheckValue;

// EXR files are little endian - check processor architecture is too
// (optimisation currently not supported for big endian machines)
static bool GLOBAL_SYSTEM_LITTLE_ENDIAN =
        (*(unsigned char*)systemEndianCheckPointer == 0x78 ? true : false);

inline void*
EXRAllocAligned (size_t size, size_t alignment)
{
    // GNUC is used for things like mingw to (cross-)compile for windows
#ifdef _WIN32
    return _aligned_malloc (size, alignment);
#elif defined(__INTEL_COMPILER) || defined(__ICL) || defined(__ICC) || defined(__ECC)
    return _mm_malloc (size, alignment);
#elif defined(_POSIX_C_SOURCE) && (_POSIX_C_SOURCE >= 200112L)
    void* ptr = 0;
    // With fortify_source on, just doing the (void) cast trick
    // doesn't remove the unused result warning but we expect the
    // other mallocs to return null and to check the return value
    // of this function
    if ( posix_memalign (&ptr, alignment, size) )
        ptr = 0;
    return ptr;
#else
    return malloc(size);
#endif
}

inline void
EXRFreeAligned (void* ptr)
{
#ifdef _WIN32
    _aligned_free (ptr);
#elif defined(__INTEL_COMPILER) || defined(__ICL) || defined(__ICC) ||         \
    defined(__ECC)
    _mm_free (ptr);
#else
    free (ptr);
#endif
}

#if defined(__GNUC__) || defined(__clang__) || defined(_MSC_VER)
// Causes issues on certain gcc versions
//#define EXR_FORCEINLINE inline __attribute__((always_inline))
#define EXR_FORCEINLINE inline
#define EXR_RESTRICT __restrict

#else

// generic compiler
#define EXR_FORCEINLINE inline
#define EXR_RESTRICT

#endif

// 
// Simple CPUID based runtime detection of various capabilities
//
class IMF_EXPORT CpuId
{
    public:
        CpuId();

        bool sse2;
        bool sse3;
        bool ssse3;
        bool sse4_1;
        bool sse4_2;
        bool avx;
        bool f16c;
};


OPENEXR_IMF_INTERNAL_NAMESPACE_HEADER_EXIT


#endif //include guard
