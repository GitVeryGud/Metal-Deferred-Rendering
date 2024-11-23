

#include <stdlib.h>
#include <simd/simd.h>

// Because these are common methods, allow other libraries to overload their implementation.
#define AAPL_SIMD_OVERLOAD __attribute__((__overloadable__))

// Generates a random float value inside the given range.
inline static float AAPL_SIMD_OVERLOAD  random_float(float min, float max)
{
    return (((double)random()/RAND_MAX) * (max-min)) + min;
}

/// Generate a random three-component vector with values between min and max.
vector_float3 AAPL_SIMD_OVERLOAD generate_random_vector(float min, float max);

/// Fast random seed.
void AAPL_SIMD_OVERLOAD seedRand(uint32_t seed);

/// Fast integer random.
int32_t AAPL_SIMD_OVERLOAD randi(void);

/// Fast floating-point random.
float AAPL_SIMD_OVERLOAD randf(float x);

/// Constructs a matrix_float3x3 from three rows of three columns with float values.
/// Indices are m<column><row>.
matrix_float3x3 AAPL_SIMD_OVERLOAD matrix_make_rows(float m00, float m10, float m20,
                                                    float m01, float m11, float m21,
                                                    float m02, float m12, float m22);

/// Constructs a matrix_float4x4 from four rows of four columns with float values.
/// Indices are m<column><row>.
matrix_float4x4 AAPL_SIMD_OVERLOAD matrix_make_rows(float m00, float m10, float m20, float m30,
                                                    float m01, float m11, float m21, float m31,
                                                    float m02, float m12, float m22, float m32,
                                                    float m03, float m13, float m23, float m33);

/// Constructs a matrix_float3x3 from 3 vector_float3 column vectors.
matrix_float3x3 AAPL_SIMD_OVERLOAD matrix_make_columns(vector_float3 col0,
                                                       vector_float3 col1,
                                                       vector_float3 col2);

/// Constructs a matrix_float4x4 from 4 vector_float4 column vectors.
matrix_float4x4 AAPL_SIMD_OVERLOAD matrix_make_columns(vector_float4 col0,
                                                       vector_float4 col1,
                                                       vector_float4 col2,
                                                       vector_float4 col3);

/// Constructs a rotation matrix from the given angle and axis.
matrix_float3x3 AAPL_SIMD_OVERLOAD matrix3x3_rotation(float radians, vector_float3 axis);

/// Constructs a rotation matrix from the given angle and axis.
matrix_float3x3 AAPL_SIMD_OVERLOAD matrix3x3_rotation(float radians, float x, float y, float z);

/// Constructs a scaling matrix with the specified scaling factors.
matrix_float3x3 AAPL_SIMD_OVERLOAD matrix3x3_scale(float x, float y, float z);

/// Constructs a scaling matrix, using the given vector as an array of scaling factors.
matrix_float3x3 AAPL_SIMD_OVERLOAD matrix3x3_scale(vector_float3 s);

/// Extracts the upper-left 3x3 submatrix of the given 4x4 matrix.
matrix_float3x3 AAPL_SIMD_OVERLOAD matrix3x3_upper_left(matrix_float4x4 m);

/// Returns the inverse of the transpose of the given matrix.
matrix_float3x3 AAPL_SIMD_OVERLOAD matrix_inverse_transpose(matrix_float3x3 m);

/// Constructs a rotation matrix from the provided angle and axis
matrix_float4x4 AAPL_SIMD_OVERLOAD matrix4x4_rotation(float radians, vector_float3 axis);

/// Constructs a rotation matrix from the given angle and axis.
matrix_float4x4 AAPL_SIMD_OVERLOAD matrix4x4_rotation(float radians, float x, float y, float z);

/// Constructs an identity matrix.
matrix_float4x4 AAPL_SIMD_OVERLOAD matrix4x4_identity(void);

/// Constructs a scaling matrix with the given scaling factors.
matrix_float4x4 AAPL_SIMD_OVERLOAD matrix4x4_scale(float sx, float sy, float sz);

/// Constructs a scaling matrix, using the given vector as an array of scaling factors.
matrix_float4x4 AAPL_SIMD_OVERLOAD matrix4x4_scale(vector_float3 s);

/// Constructs a translation matrix that translates by the vector (tx, ty, tz).
matrix_float4x4 AAPL_SIMD_OVERLOAD matrix4x4_translation(float tx, float ty, float tz);

/// Constructs a translation matrix that translates by the vector (t.x, t.y, t.z).
matrix_float4x4 AAPL_SIMD_OVERLOAD matrix4x4_translation(vector_float3 t);

/// Constructs a translation matrix that scales by the vector (s.x, s.y, s.z)
/// and translates by the vector (t.x, t.y, t.z).
matrix_float4x4 AAPL_SIMD_OVERLOAD matrix4x4_scale_translation(vector_float3 s, vector_float3 t);

/// Starting with left-hand world coordinates, constructs a view matrix that is
/// positioned at (eyeX, eyeY, eyeZ) and looks toward (centerX, centerY, centerZ),
/// with the vector (upX, upY, upZ) pointing up for a left-hand coordinate system.
matrix_float4x4 AAPL_SIMD_OVERLOAD matrix_look_at_left_hand(float eyeX, float eyeY, float eyeZ,
                                                            float centerX, float centerY, float centerZ,
                                                            float upX, float upY, float upZ);

/// Starting with left-hand world coordinates, constructs a view matrix that is
/// positioned at (eye) and looks toward (target), with the vector (up) pointing
/// up for a left-hand coordinate system.
matrix_float4x4 AAPL_SIMD_OVERLOAD matrix_look_at_left_hand(vector_float3 eye,
                                                            vector_float3 target,
                                                            vector_float3 up);

/// Starting with right-hand world coordinates, constructs a view matrix that is
/// positioned at (eyeX, eyeY, eyeZ) and looks toward (centerX, centerY, centerZ),
/// with the vector (upX, upY, upZ) pointing up for a right-hand coordinate system.
matrix_float4x4 AAPL_SIMD_OVERLOAD matrix_look_at_right_hand(float eyeX, float eyeY, float eyeZ,
                                                             float centerX, float centerY, float centerZ,
                                                             float upX, float upY, float upZ);

/// Starting with right-hand world coordinates, constructs a view matrix that is
/// positioned at (eye) and looks toward (target), with the vector (up) pointing
/// up for a right-hand coordinate system.
matrix_float4x4 AAPL_SIMD_OVERLOAD matrix_look_at_right_hand(vector_float3 eye,
                                                             vector_float3 target,
                                                             vector_float3 up);

/// Constructs a symmetric orthographic projection matrix, from left-hand eye
/// coordinates to left-hand clip coordinates.
/// That maps (left, top) to (-1, 1), (right, bottom) to (1, -1), and (nearZ, farZ) to (0, 1).
/// The first four arguments are signed eye coordinates.
/// nearZ and farZ are absolute distances from the eye to the near and far clip planes.
matrix_float4x4 AAPL_SIMD_OVERLOAD matrix_ortho_left_hand(float left, float right, float bottom, float top, float nearZ, float farZ);

/// Constructs a symmetric orthographic projection matrix, from right-hand eye
/// coordinates to right-hand clip coordinates.
/// That maps (left, top) to (-1, 1), (right, bottom) to (1, -1), and (nearZ, farZ) to (0, 1).
/// The first four arguments are signed eye coordinates.
/// nearZ and farZ are absolute distances from the eye to the near and far clip planes.
matrix_float4x4 AAPL_SIMD_OVERLOAD matrix_ortho_right_hand(float left, float right, float bottom, float top, float nearZ, float farZ);

/// Constructs a symmetric perspective projection matrix, from left-hand eye
/// coordinates to left-hand clip coordinates, with a vertical viewing angle of
/// fovyRadians, the given aspect ratio, and the given absolute near and far
/// z distances from the eye.
matrix_float4x4 AAPL_SIMD_OVERLOAD matrix_perspective_left_hand(float fovyRadians, float aspect, float nearZ, float farZ);

/// Constructs a symmetric perspective projection matrix, from right-hand eye
/// coordinates to right-hand clip coordinates, with a vertical viewing angle of
/// fovyRadians, the given aspect ratio, and the given absolute near and far
/// z distances from the eye.
matrix_float4x4  AAPL_SIMD_OVERLOAD matrix_perspective_right_hand(float fovyRadians, float aspect, float nearZ, float farZ);

/// Construct a general frustum projection matrix, from right-hand eye
/// coordinates to left-hand clip coordinates.
/// The bounds left, right, bottom, and top, define the visible frustum at the near clip plane.
/// The first four arguments are signed eye coordinates.
/// nearZ and farZ are absolute distances from the eye to the near and far clip planes.
matrix_float4x4 AAPL_SIMD_OVERLOAD matrix_perspective_frustum_right_hand(float left, float right, float bottom, float top, float nearZ, float farZ);

/// Returns the inverse of the transpose of the given matrix.
matrix_float4x4 AAPL_SIMD_OVERLOAD matrix_inverse_transpose(matrix_float4x4 m);


