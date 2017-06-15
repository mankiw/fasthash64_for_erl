/* The MIT License

Copyright (C) 2012 Zilong Tan (eric.zltan@gmail.com)

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use, copy,
modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "fasthash.h"

// Compression function for Merkle-Damgard construction.
// This function is generated using the framework provided.
//#define mix(h) ({					\
//			(h) ^= (h) >> 23;		\
//			(h) *= 0x2127599bf4325c37ULL;	\
//			(h) ^= (h) >> 47; })

#if defined WIN32
unsigned __int64 fasthash64(const void *buf, size_t len, unsigned __int64 seed)
#else
uint64_t fasthash64(const void *buf, size_t len, uint64_t seed)
#endif
{
#if defined WIN32
    const unsigned __int64    m = 0x880355f21e6d1965ULL;
    const unsigned __int64 *pos = (const unsigned __int64 *)buf;
    const unsigned __int64 *end = pos + (len / 8);
    const unsigned char *pos2;
    unsigned __int64 h = seed ^ (len * m);
    unsigned __int64 v;
#else
    const uint64_t    m = 0x880355f21e6d1965ULL;
    const uint64_t *pos = (const uint64_t *)buf;
    const uint64_t *end = pos + (len / 8);
    const unsigned char *pos2;
    uint64_t h = seed ^ (len * m);
    uint64_t v;
#endif

    while (pos != end) {
        v = *pos++;
        v ^= v >> 23;
        v *= 0x2127599bf4325c37ULL;
        v ^= v >> 47;
        h ^= v;
        h *= m;
    }

    pos2 = (const unsigned char*)pos;
    v = 0;

#if defined WIN32
    switch (len & 7)
    {
    case 7: v ^= (unsigned __int64)pos2[6] << 48;
    case 6: v ^= (unsigned __int64)pos2[5] << 40;
    case 5: v ^= (unsigned __int64)pos2[4] << 32;
    case 4: v ^= (unsigned __int64)pos2[3] << 24;
    case 3: v ^= (unsigned __int64)pos2[2] << 16;
    case 2: v ^= (unsigned __int64)pos2[1] << 8;
    case 1: v ^= (unsigned __int64)pos2[0];
#else
    switch (len & 7)
    {
    case 7: v ^= (uint64_t)pos2[6] << 48;
    case 6: v ^= (uint64_t)pos2[5] << 40;
    case 5: v ^= (uint64_t)pos2[4] << 32;
    case 4: v ^= (uint64_t)pos2[3] << 24;
    case 3: v ^= (uint64_t)pos2[2] << 16;
    case 2: v ^= (uint64_t)pos2[1] << 8;
    case 1: v ^= (uint64_t)pos2[0];
#endif

        v ^= v >> 23;
        v *= 0x2127599bf4325c37ULL;
        v ^= v >> 47;
        h ^= v;
        h *= m;
    }

    (h) ^= (h) >> 23;
    (h) *= 0x2127599bf4325c37ULL;
    (h) ^= (h) >> 47;

    return h;
}

#if defined WIN32
unsigned __int32 fasthash32(const void *buf, size_t len, unsigned __int32 seed)
#else
uint32_t fasthash32(const void *buf, size_t len, uint32_t seed)
#endif
{
    // the following trick converts the 64-bit hashcode to Fermat
    // residue, which shall retain information from both the higher
    // and lower parts of hashcode.
#if defined WIN32
    unsigned __int64 h = fasthash64(buf, len, seed);
#else
    uint64_t h = fasthash64(buf, len, seed);
#endif

    return 
#if defined WIN32
        unsigned __int32
#else
        uint32_t
#endif
        (h - (h >> 32));
}



static ERL_NIF_TERM fasthash_nif(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    int ssize;
    enif_get_int(env, argv[0], &ssize);
    char str[1024] = {0};
    enif_get_string(env, argv[1], str, ssize + 1, ERL_NIF_LATIN1);
    uint64_t ret = fasthash64(str, ssize, 0);
    return enif_make_uint64(env, ret);
}
static ErlNifFunc nif_funcs[] =
{
    {"fasthash644erl", 2, fasthash_nif}
};
ERL_NIF_INIT(fasthash,nif_funcs,NULL,NULL,NULL,NULL)
