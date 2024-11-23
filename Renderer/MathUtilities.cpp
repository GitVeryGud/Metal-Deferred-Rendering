

#include "MathUtilities.h"
#include <assert.h>
#include <stdlib.h>

uint32_t seed_lo, seed_hi;

void AAPL_SIMD_OVERLOAD seedRand(uint32_t seed) {
    seed_lo = seed; seed_hi = ~seed;
}

static vector_float3 AAPL_SIMD_OVERLOAD vector_make(float x, float y, float z) {
    return (vector_float3){ x, y, z };
}

//------------------------------------------------------------------------------
// matrix_make_rows takes input data with rows of elements.
// This way, the calling code matrix data can look like the rows
// of a matrix made for transforming column vectors.

// Indices are m<column><row>
matrix_float3x3 AAPL_SIMD_OVERLOAD matrix_make_rows(
                                   float m00, float m10, float m20,
                                   float m01, float m11, float m21,
                                   float m02, float m12, float m22) {
    return (matrix_float3x3){ {
            { m00, m01, m02 },      // each line here provides column data
            { m10, m11, m12 },
            { m20, m21, m22 } } };
}

matrix_float4x4 AAPL_SIMD_OVERLOAD matrix_make_rows(
                                   float m00, float m10, float m20, float m30,
                                   float m01, float m11, float m21, float m31,
                                   float m02, float m12, float m22, float m32,
                                   float m03, float m13, float m23, float m33) {
    return (matrix_float4x4){ {
        { m00, m01, m02, m03 },     // each line here provides column data
        { m10, m11, m12, m13 },
        { m20, m21, m22, m23 },
        { m30, m31, m32, m33 } } };
}

// each arg is a column vector
matrix_float3x3 AAPL_SIMD_OVERLOAD matrix_make_columns(
                                   vector_float3 col0,
                                   vector_float3 col1,
                                   vector_float3 col2) {
    return (matrix_float3x3){ col0, col1, col2 };
}

// each arg is a column vector
matrix_float4x4 AAPL_SIMD_OVERLOAD matrix_make_columns(
                                   vector_float4 col0,
                                   vector_float4 col1,
                                   vector_float4 col2,
                                   vector_float4 col3) {
    return (matrix_float4x4){ col0, col1, col2, col3 };
}

matrix_float3x3 AAPL_SIMD_OVERLOAD matrix3x3_rotation(float radians, vector_float3 axis) {
    axis = vector_normalize(axis);
    float ct = cosf(radians);
    float st = sinf(radians);
    float ci = 1 - ct;
    float x = axis.x, y = axis.y, z = axis.z;
    return matrix_make_rows(    ct + x * x * ci, x * y * ci - z * st, x * z * ci + y * st,
                            y * x * ci + z * st,     ct + y * y * ci, y * z * ci - x * st,
                            z * x * ci - y * st, z * y * ci + x * st,     ct + z * z * ci );
}

matrix_float3x3 AAPL_SIMD_OVERLOAD matrix3x3_rotation(float radians, float x, float y, float z) {
    return matrix3x3_rotation(radians, vector_make(x, y, z));
}

matrix_float3x3 AAPL_SIMD_OVERLOAD matrix3x3_scale(float sx, float sy, float sz) {
    return matrix_make_rows(sx,  0,  0,
                             0, sy,  0,
                             0,  0, sz);
}

matrix_float3x3 AAPL_SIMD_OVERLOAD matrix3x3_scale(vector_float3 s) {
    return matrix_make_rows(s.x,   0,   0,
                              0, s.y,   0,
                              0,   0, s.z);
}

matrix_float3x3 AAPL_SIMD_OVERLOAD matrix3x3_upper_left(matrix_float4x4 m) {
    vector_float3 x = m.columns[0].xyz;
    vector_float3 y = m.columns[1].xyz;
    vector_float3 z = m.columns[2].xyz;
    return matrix_make_columns(x, y, z);
}

matrix_float3x3 AAPL_SIMD_OVERLOAD matrix_inverse_transpose(matrix_float3x3 m) {
    return matrix_invert(matrix_transpose(m));
}

matrix_float4x4 AAPL_SIMD_OVERLOAD matrix4x4_rotation(float radians, vector_float3 axis) {
    axis = vector_normalize(axis);
    float ct = cosf(radians);
    float st = sinf(radians);
    float ci = 1 - ct;
    float x = axis.x, y = axis.y, z = axis.z;
    return matrix_make_rows(
                        ct + x * x * ci, x * y * ci - z * st, x * z * ci + y * st, 0,
                    y * x * ci + z * st,     ct + y * y * ci, y * z * ci - x * st, 0,
                    z * x * ci - y * st, z * y * ci + x * st,     ct + z * z * ci, 0,
                                      0,                   0,                   0, 1);
}

matrix_float4x4 AAPL_SIMD_OVERLOAD matrix4x4_rotation(float radians, float x, float y, float z) {
    return matrix4x4_rotation(radians, vector_make(x, y, z));
}

matrix_float4x4 AAPL_SIMD_OVERLOAD matrix4x4_identity(void) {
    return matrix_make_rows(1, 0, 0, 0,
                            0, 1, 0, 0,
                            0, 0, 1, 0,
                            0, 0, 0, 1 );
}

matrix_float4x4 AAPL_SIMD_OVERLOAD matrix4x4_scale(float sx, float sy, float sz) {
    return matrix_make_rows(sx,  0,  0, 0,
                             0, sy,  0, 0,
                             0,  0, sz, 0,
                             0,  0,  0, 1 );
}

matrix_float4x4 AAPL_SIMD_OVERLOAD matrix4x4_scale(vector_float3 s) {
    return matrix_make_rows(s.x,   0,   0, 0,
                              0, s.y,   0, 0,
                              0,   0, s.z, 0,
                              0,   0,   0, 1 );
}

matrix_float4x4 AAPL_SIMD_OVERLOAD matrix4x4_translation(float tx, float ty, float tz) {
    return matrix_make_rows(1, 0, 0, tx,
                            0, 1, 0, ty,
                            0, 0, 1, tz,
                            0, 0, 0,  1 );
}

matrix_float4x4 AAPL_SIMD_OVERLOAD matrix4x4_translation(vector_float3 t) {
    return matrix_make_rows(1, 0, 0, t.x,
                            0, 1, 0, t.y,
                            0, 0, 1, t.z,
                            0, 0, 0,   1 );
}

matrix_float4x4 AAPL_SIMD_OVERLOAD matrix4x4_scale_translation(vector_float3 s, vector_float3 t) {
    return matrix_make_rows(s.x,   0,   0, t.x,
                              0, s.y,   0, t.y,
                              0,   0, s.z, t.z,
                              0,   0,   0,   1 );
}


matrix_float4x4 AAPL_SIMD_OVERLOAD matrix_look_at_left_hand(vector_float3 eye,
                                                            vector_float3 target,
                                                            vector_float3 up) {
    vector_float3 z = vector_normalize(target - eye);
    vector_float3 x = vector_normalize(vector_cross(up, z));
    vector_float3 y = vector_cross(z, x);
    vector_float3 t = vector_make(-vector_dot(x, eye), -vector_dot(y, eye), -vector_dot(z, eye));

    return matrix_make_rows(x.x, x.y, x.z, t.x,
                            y.x, y.y, y.z, t.y,
                            z.x, z.y, z.z, t.z,
                              0,   0,   0,   1 );
}

matrix_float4x4 AAPL_SIMD_OVERLOAD matrix_look_at_left_hand(float eyeX, float eyeY, float eyeZ,
                                                            float centerX, float centerY, float centerZ,
                                                            float upX, float upY, float upZ) {
    vector_float3 eye = vector_make(eyeX, eyeY, eyeZ);
    vector_float3 center = vector_make(centerX, centerY, centerZ);
    vector_float3 up = vector_make(upX, upY, upZ);

    return matrix_look_at_left_hand(eye, center, up);
}

matrix_float4x4 AAPL_SIMD_OVERLOAD matrix_look_at_right_hand(vector_float3 eye,
                                                             vector_float3 target,
                                                             vector_float3 up) {
    vector_float3 z = vector_normalize(eye - target);
    vector_float3 x = vector_normalize(vector_cross(up, z));
    vector_float3 y = vector_cross(z, x);
    vector_float3 t = vector_make(-vector_dot(x, eye), -vector_dot(y, eye), -vector_dot(z, eye));

    return matrix_make_rows(x.x, x.y, x.z, t.x,
                            y.x, y.y, y.z, t.y,
                            z.x, z.y, z.z, t.z,
                              0,   0,   0,   1 );
}

matrix_float4x4 AAPL_SIMD_OVERLOAD matrix_look_at_right_hand(float eyeX, float eyeY, float eyeZ,
                                                             float centerX, float centerY, float centerZ,
                                                             float upX, float upY, float upZ) {
    vector_float3 eye = vector_make(eyeX, eyeY, eyeZ);
    vector_float3 center = vector_make(centerX, centerY, centerZ);
    vector_float3 up = vector_make(upX, upY, upZ);

    return matrix_look_at_right_hand(eye, center, up);
}

matrix_float4x4 AAPL_SIMD_OVERLOAD matrix_ortho_left_hand(float left, float right, float bottom, float top, float nearZ, float farZ) {
    return matrix_make_rows(
        2 / (right - left),                  0,                  0, (left + right) / (left - right),
                         0, 2 / (top - bottom),                  0, (top + bottom) / (bottom - top),
                         0,                  0, 1 / (farZ - nearZ),          nearZ / (nearZ - farZ),
                         0,                  0,                  0,                               1 );
}

matrix_float4x4 AAPL_SIMD_OVERLOAD matrix_ortho_right_hand(float left, float right, float bottom, float top, float nearZ, float farZ) {
    return matrix_make_rows(
        2 / (right - left),                  0,                   0, (left + right) / (left - right),
        0,                  2 / (top - bottom),                   0, (top + bottom) / (bottom - top),
        0,                                   0, -1 / (farZ - nearZ),          nearZ / (nearZ - farZ),
        0,                                   0,                   0,                               1 );
}

matrix_float4x4 AAPL_SIMD_OVERLOAD matrix_perspective_left_hand(float fovyRadians, float aspect, float nearZ, float farZ) {
    float ys = 1 / tanf(fovyRadians * 0.5);
    float xs = ys / aspect;
    float zs = farZ / (farZ - nearZ);
    return matrix_make_rows(xs,  0,  0,           0,
                             0, ys,  0,           0,
                             0,  0, zs, -nearZ * zs,
                             0,  0,  1,           0 );
}

matrix_float4x4 AAPL_SIMD_OVERLOAD matrix_perspective_right_hand(float fovyRadians, float aspect, float nearZ, float farZ) {
    float ys = 1 / tanf(fovyRadians * 0.5);
    float xs = ys / aspect;
    float zs = farZ / (nearZ - farZ);
    return matrix_make_rows(xs,  0,  0,          0,
                             0, ys,  0,          0,
                             0,  0, zs, nearZ * zs,
                             0,  0, -1,          0 );
}

matrix_float4x4 AAPL_SIMD_OVERLOAD matrix_perspective_frustum_right_hand(float l, float r, float b, float t, float n, float f) {
    return matrix_make_rows(
        2 * n / (r - l),                0, (r + l) / (r - l),                 0,
                     0,   2 * n / (t - b), (t + b) / (t - b),                 0,
                     0,                 0,      -f / (f - n), -f * n  / (f - n),
                     0,                 0,                -1,                 0 );
}

matrix_float4x4 AAPL_SIMD_OVERLOAD matrix_inverse_transpose(matrix_float4x4 m) {
    return matrix_invert(matrix_transpose(m));
}
