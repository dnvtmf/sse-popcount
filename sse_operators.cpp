struct sse_vector final {
    union {
        __m128i  v;
        uint8_t  u8[16];
        uint16_t u16[8];
        uint32_t u32[4];
        uint64_t u64[2];
    };

    sse_vector() = delete;
    sse_vector(sse_vector&) = delete;

    explicit sse_vector(const __m128i& vec): v(vec) {}
};


__m128i operator&(sse_vector a, sse_vector b) {

    return _mm_and_si128(a.v, b.v);
}


__m128i operator|(sse_vector a, sse_vector b) {

    return _mm_or_si128(a.v, b.v);
}


__m128i operator^(sse_vector a, sse_vector b) {

    return _mm_xor_si128(a.v, b.v);
}


struct shift16 final {
    const unsigned bits;

    shift16() = delete;
    explicit shift16(unsigned bits) : bits(bits) {};
};


__m128i operator>>(const __m128i a, const shift16 amount) {

    return _mm_srli_epi16(a, amount.bits);
}


uint64_t lower_qword(const __m128i v) {

    return _mm_cvtsi128_si64(v);
}


uint64_t higher_qword(const __m128i v) {

    return lower_qword(_mm_srli_si128(v, 8));
}


uint64_t simd_sum_epu64(const __m128i v) {

    return lower_qword(v) + higher_qword(v);
}


#if defined(HAVE_AVX2_INSTRUCTIONS)
uint64_t simd_sum_epu64(const __m256i v) {

    return static_cast<uint64_t>(_mm256_extract_epi64(v, 0))
         + static_cast<uint64_t>(_mm256_extract_epi64(v, 1))
         + static_cast<uint64_t>(_mm256_extract_epi64(v, 2))
         + static_cast<uint64_t>(_mm256_extract_epi64(v, 3));
}
#endif


#if defined(HAVE_AVX512BW_INSTRUCTIONS)
uint64_t simd_sum_epu64(const __m512i v) {

    const __m256i lo = _mm512_extracti64x4_epi64(v, 0);
    const __m256i hi = _mm512_extracti64x4_epi64(v, 1);

    return simd_sum_epu64(lo) + simd_sum_epu64(hi);
}
#endif
