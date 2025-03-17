    .global cross_neon
    .text
    .cpu generic+fp+simd

// void cross_neon(Vector3f* result, const Vector3f* v1, const Vector3f* v2)
cross_neon:
    ld1     {v0.4s}, [x1]        // v0 = {x1, y1, z1, ?}
    ins     v0.s[3], wzr         // v0 = {x1, y1, z1, 0}

    ld1     {v1.4s}, [x2]        // v1 = {x2, y2, z2, ?}
    ins     v1.s[3], wzr         // v1 = {x2, y2, z2, 0}

    ext     v2.16b, v0.16b, v0.16b, #4  // [y1, z1, 0, x1]
    ins     v2.s[2], v0.s[0]            // [y1, z1, x1, x1]
    ins     v2.s[3], wzr                // v2 = {y1, z1, x1, 0}

    ext     v3.16b, v1.16b, v1.16b, #8  // [z2, 0, x2, y2]
    ins     v3.s[1], v1.s[0]            // [z2, x2, x2, y2]
    ins     v3.s[2], v1.s[1]            // [z2, x2, y2, y2]
    ins     v3.s[3], wzr                // v3 = {z2, x2, y2, 0}

    ext     v4.16b, v0.16b, v0.16b, #8  // [z1, 0, x1, y1]
    ins     v4.s[1], v0.s[0]            // [z1, x1, x1, y1]
    ins     v4.s[2], v0.s[1]            // [z1, x1, y1, y1]
    ins     v4.s[3], wzr                // v4 = {z1, x1, y1, 0}

    ext     v5.16b, v1.16b, v1.16b, #4  // [y2, z2, 0, x2]
    ins     v5.s[2], v1.s[0]            // [y2, z2, x2, x2]
    ins     v5.s[3], wzr                // v5 = {y2, z2, x2, 0}

    fmul    v2.4s, v2.4s, v3.4s   // [y1*z2, z1*x2, x1*y2, 0]
    fmul    v4.4s, v4.4s, v5.4s   // [z1*y2, x1*z2, y1*x2, 0]

    fsub    v2.4s, v2.4s, v4.4s   // [y1z2-z1y2, z1x2-x1z2, x1y2-y1x2, 0]

    st1     {v2.s}[0], [x0], #4
    st1     {v2.s}[1], [x0], #4
    st1     {v2.s}[2], [x0]

    ret
