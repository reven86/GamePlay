#ifndef MATRIX3_H_
#define MATRIX3_H_

#include "Vector2.h"
#include "Vector3.h"

namespace gameplay
{

/**
 * Defines a 3 x 3 floating point matrix representing a 2D transformation.
 *
 * Vectors are treated as columns, resulting in a matrix that is represented as follows,
 * where x and y are the translation components of the matrix:
 *
 * 1  0  x
 * 0  1  y
 * 0  0  1
 *
 * The matrix uses column-major format such that array indices increase down column first.
 * Since matrix multiplication is not commutative, multiplication must be done in the
 * correct order when combining transformations. Suppose we have a translation
 * matrix T and a rotation matrix R. To first rotate an object around the origin
 * and then translate it, you would multiply the two matrices as TR.
 *
 * Likewise, to first translate the object and then rotate it, you would do RT.
 * So generally, matrices must be multiplied in the reverse order in which you
 * want the transformations to take place (this also applies to
 * the scale, rotate, and translate methods below; these methods are convenience
 * methods for post-multiplying by a matrix representing a scale, rotation, or translation).
 *
 */
class Matrix3
{
public:

    /**
     * Stores the columns of this 4x4 matrix.
     * */
    float m[9];

    /**
     * Constructs a matrix initialized to the identity matrix:
     *
     * 1  0  0
     * 0  1  0
     * 0  0  1
     */
    Matrix3();

    /**
     * Constructs a matrix initialized to the specified value.
     *
     * @param m11 The first element of the first row.
     * @param m12 The second element of the first row.
     * @param m13 The third element of the first row.
     * @param m21 The first element of the second row.
     * @param m22 The second element of the second row.
     * @param m23 The third element of the second row.
     * @param m31 The first element of the third row.
     * @param m32 The second element of the third row.
     * @param m33 The third element of the third row.
     */
    Matrix3(float m11, float m12, float m13, float m21, float m22, float m23, float m31,
           float m32, float m33);

    /**
     * Creates a matrix initialized to the specified column-major array.
     *
     * The passed-in array is in column-major order, so the memory layout of the array is as follows:
     *
     *     0  3  6
     *     1  4  7
     *     2  5  8
     *
     * @param m An array containing 9 elements in column-major order.
     */
    Matrix3(const float* m);

    /**
     * Constructs a new matrix by copying the values from the specified matrix.
     *
     * @param copy The matrix to copy.
     */
    Matrix3(const Matrix3& copy);

    /**
     * Destructor.
     */
    ~Matrix3();

    /**
     * Returns the identity matrix:
     *
     * 1  0  0
     * 0  1  0
     * 0  0  1
     *
     * @return The identity matrix.
     */
    static const Matrix3& identity();

    /**
     * Returns the matrix with all zeros.
     *
     * @return The matrix with all zeros.
     */
    static const Matrix3& zero();

    /**
     * Creates a scale matrix.
     *
     * @param scale The amount to scale.
     * @param dst A matrix to store the result in.
     */
    static void createScale(const Vector2& scale, Matrix3* dst);

    /**
     * Creates a scale matrix.
     *
     * @param xScale The amount to scale along the x-axis.
     * @param yScale The amount to scale along the y-axis.
     * @param dst A matrix to store the result in.
     */
    static void createScale(float xScale, float yScale, Matrix3* dst);

    /**
     * Creates a matrix describing a rotation around the z-axis.
     *
     * @param angle The angle of rotation (in radians).
     * @param dst A matrix to store the result in.
     */
    static void createRotation(float angle, Matrix3* dst);

    /**
     * Creates a translation matrix.
     *
     * @param translation The translation.
     * @param dst A matrix to store the result in.
     */
    static void createTranslation(const Vector2& translation, Matrix3* dst);

    /**
     * Creates a translation matrix.
     *
     * @param xTranslation The translation on the x-axis.
     * @param yTranslation The translation on the y-axis.
     * @param dst A matrix to store the result in.
     */
    static void createTranslation(float xTranslation, float yTranslation, Matrix3* dst);

    /**
     * Adds a scalar value to each component of this matrix.
     *
     * @param scalar The scalar to add.
     */
    void add(float scalar);

    /**
     * Adds a scalar value to each component of this matrix and stores the result in dst.
     *
     * @param scalar The scalar value to add.
     * @param dst A matrix to store the result in.
     */
    void add(float scalar, Matrix3* dst);

    /**
     * Adds the specified matrix to this matrix.
     *
     * @param m The matrix to add.
     */
    void add(const Matrix3& m);

    /**
     * Adds the specified matrices and stores the result in dst.
     *
     * @param m1 The first matrix.
     * @param m2 The second matrix.
     * @param dst The destination matrix to add to.
     */
    static void add(const Matrix3& m1, const Matrix3& m2, Matrix3* dst);

    /**
     * Decomposes the scale, rotation and translation components of this matrix.
     *
     * @param scale The scale.
     * @param rotation The rotation.
     * @param translation The translation.
     */
    bool decompose(Vector2* scale, float* rotation, Vector2* translation) const;

    /**
     * Computes the determinant of this matrix.
     *
     * @return The determinant.
     */
    float determinant() const;

    /**
     * Gets the scalar component of this matrix in the specified vector.
     *
     * If the scalar component of this matrix has negative parts,
     * it is not possible to always extract the exact scalar component;
     * instead, a scale vector that is mathematically equivalent to the
     * original scale vector is extracted and returned.
     *
     * @param scale A vector to receive the scale.
     */
    void getScale(Vector2* scale) const;

    /**
     * Gets the rotational component of this matrix in the specified angle.
     *
     * @param rotation An angle to receive the rotation.
     * 
     * @return true if the rotation is successfully extracted, false otherwise.
     */
    bool getRotation(float* rotation) const;

    /**
     * Gets the translational component of this matrix in the specified vector.
     *
     * @param translation A vector to receive the translation.
     */
    void getTranslation(Vector2* translation) const;

    /**
     * Gets the up vector of this matrix.
     *
     * @param dst The destination vector.
     */
    void getUpVector(Vector2* dst) const;

    /**
     * Gets the down vector of this matrix.
     *
     * @param dst The destination vector.
     */
    void getDownVector(Vector2* dst) const;

    /**
     * Gets the left vector of this matrix.
     *
     * @param dst The destination vector.
     */
    void getLeftVector(Vector2* dst) const;

    /**
     * Gets the right vector of this matrix.
     *
     * @param dst The destination vector.
     */
    void getRightVector(Vector2* dst) const;

    /**
     * Inverts this matrix.
     *
     * @return true if the the matrix can be inverted, false otherwise.
     */
    bool invert();

    /**
     * Stores the inverse of this matrix in the specified matrix.
     *
     * @param dst A matrix to store the invert of this matrix in.
     * 
     * @return true if the the matrix can be inverted, false otherwise.
     */
    bool invert(Matrix3* dst) const;

    /**
     * Determines if this matrix is equal to the identity matrix.
     *
     * @return true if the matrix is an identity matrix, false otherwise.
     */
    bool isIdentity() const;

    /**
     * Multiplies the components of this matrix by the specified scalar.
     *
     * @param scalar The scalar value.
     */
    void multiply(float scalar);

    /**
     * Multiplies the components of this matrix by a scalar and stores the result in dst.
     *
     * @param scalar The scalar value.
     * @param dst A matrix to store the result in.
     */
    void multiply(float scalar, Matrix3* dst) const;

    /**
     * Multiplies the components of the specified matrix by a scalar and stores the result in dst.
     *
     * @param m The matrix.
     * @param scalar The scalar value.
     * @param dst A matrix to store the result in.
     */
    static void multiply(const Matrix3& m, float scalar, Matrix3* dst);

    /**
     * Multiplies this matrix by the specified one.
     *
     * @param m The matrix to multiply.
     */
    void multiply(const Matrix3& m);

    /**
     * Multiplies m1 by m2 and stores the result in dst.
     *
     * @param m1 The first matrix to multiply.
     * @param m2 The second matrix to multiply.
     * @param dst A matrix to store the result in.
     */
    static void multiply(const Matrix3& m1, const Matrix3& m2, Matrix3* dst);

    /**
     * Negates this matrix.
     */
    void negate();

    /**
     * Negates this matrix and stores the result in dst.
     *
     * @param dst A matrix to store the result in.
     */
    void negate(Matrix3* dst) const;

    /**
     * Post-multiplies this matrix by the matrix corresponding to the
     * specified rotation.
     *
     * @param angle The angle of rotation (in radians).
     */
    void rotate(float angle);

    /**
     * Post-multiplies this matrix by the matrix corresponding to the
     * specified rotation and stores the result in dst.
     *
     * @param angle The angle of rotation (in radians).
     * @param dst A matrix to store the result in.
     */
    void rotate(float angle, Matrix3* dst) const;

    /**
     * Post-multiplies this matrix by the matrix corresponding to the
     * specified scale transformation.
     *
     * @param value The amount to scale along all axes.
     */
    void scale(float value);

    /**
     * Post-multiplies this matrix by the matrix corresponding to the
     * specified scale transformation and stores the result in dst.
     *
     * @param value The amount to scale along all axes.
     * @param dst A matrix to store the result in.
     */
    void scale(float value, Matrix3* dst) const;

    /**
     * Post-multiplies this matrix by the matrix corresponding to the
     * specified scale transformation.
     *
     * @param xScale The amount to scale along the x-axis.
     * @param yScale The amount to scale along the y-axis.
     */
    void scale(float xScale, float yScale);

    /**
     * Post-multiplies this matrix by the matrix corresponding to the
     * specified scale transformation and stores the result in dst.
     *
     * @param xScale The amount to scale along the x-axis.
     * @param yScale The amount to scale along the y-axis.
     * @param dst A matrix to store the result in.
     */
    void scale(float xScale, float yScale, Matrix3* dst) const;

    /**
     * Post-multiplies this matrix by the matrix corresponding to the
     * specified scale transformation.
     *
     * @param s The scale values along the x and y axes.
     */
    void scale(const Vector2& s);

    /**
     * Post-multiplies this matrix by the matrix corresponding to the
     * specified scale transformation and stores the result in dst.
     *
     * @param s The scale values along the x and y axes.
     * @param dst A matrix to store the result in.
     */
    void scale(const Vector2& s, Matrix3* dst) const;

    /**
     * Sets the values of this matrix.
     *
     * @param m11 The first element of the first row.
     * @param m12 The second element of the first row.
     * @param m13 The third element of the first row.
     * @param m21 The first element of the second row.
     * @param m22 The second element of the second row.
     * @param m23 The third element of the second row.
     * @param m31 The first element of the third row.
     * @param m32 The second element of the third row.
     * @param m33 The third element of the third row.
     */
    void set(float m11, float m12, float m13, float m21, float m22, float m23, float m31,
             float m32, float m33);

    /**
     * Sets the values of this matrix to those in the specified column-major array.
     *
     * @param m An array containing 9 elements in column-major format.
     */
    void set(const float* m);

    /**
     * Sets the values of this matrix to those of the specified matrix.
     *
     * @param m The source matrix.
     */
    void set(const Matrix3& m);

    /**
     * Sets this matrix to the identity matrix.
     */
    void setIdentity();

    /**
     * Sets all elements of the current matrix to zero.
     */
    void setZero();

    /**
     * Subtracts the specified matrix from the current matrix.
     *
     * @param m The matrix to subtract.
     */
    void subtract(const Matrix3& m);

    /**
     * Subtracts the specified matrix from the current matrix.
     *
     * @param m1 The first matrix.
     * @param m2 The second matrix.
     * @param dst A matrix to store the result in.
     */
    static void subtract(const Matrix3& m1, const Matrix3& m2, Matrix3* dst);

    /**
     * Transforms the specified point by this matrix.
     *
     * The result of the transformation is stored directly into point.
     *
     * @param point The point to transform and also a vector to hold the result in.
     */
    void transformPoint(Vector2* point) const;

    /**
     * Transforms the specified point by this matrix, and stores
     * the result in dst.
     *
     * @param point The point to transform.
     * @param dst A vector to store the transformed point in.
     */
    void transformPoint(const Vector2& point, Vector2* dst) const;

    /**
     * Transforms the specified vector by this matrix by
     * treating the third (z) coordinate as zero.
     *
     * The result of the transformation is stored directly into vector.
     *
     * @param vector The vector to transform and also a vector to hold the result in.
     */
    void transformVector(Vector2* vector) const;

    /**
     * Transforms the specified vector by this matrix by
     * treating the third (z) coordinate as zero, and stores the
     * result in dst.
     *
     * @param vector The vector to transform.
     * @param dst A vector to store the transformed vector in.
     */
    void transformVector(const Vector2& vector, Vector2* dst) const;

    /**
     * Transforms the specified vector by this matrix.
     *
     * @param x The vector x-coordinate to transform by.
     * @param y The vector y-coordinate to transform by.
     * @param z The vector z-coordinate to transform by.
     * @param dst A vector to store the transformed point in.
     */
    void transformVector(float x, float y, float z, Vector2* dst) const;

    /**
     * Transforms the specified vector by this matrix.
     *
     * The result of the transformation is stored directly into vector.
     *
     * @param vector The vector to transform.
     */
    void transformVector(Vector3* vector) const;

    /**
     * Transforms the specified vector by this matrix.
     *
     * @param vector The vector to transform.
     * @param dst A vector to store the transformed point in.
     */
    void transformVector(const Vector3& vector, Vector3* dst) const;

    /**
     * Post-multiplies this matrix by the matrix corresponding to the
     * specified translation.
     *
     * @param x The amount to translate along the x-axis.
     * @param y The amount to translate along the y-axis.
     */
    void translate(float x, float y);

    /**
     * Post-multiplies this matrix by the matrix corresponding to the
     * specified translation and stores the result in dst.
     *
     * @param x The amount to translate along the x-axis.
     * @param y The amount to translate along the y-axis.
     * @param dst A matrix to store the result in.
     */
    void translate(float x, float y, Matrix3* dst) const;

    /**
     * Post-multiplies this matrix by the matrix corresponding to the
     * specified translation.
     *
     * @param t The translation values along the x and y axes.
     */
    void translate(const Vector2& t);

    /**
     * Post-multiplies this matrix by the matrix corresponding to the
     * specified translation and stores the result in dst.
     *
     * @param t The translation values along the x and y axes.
     * @param dst A matrix to store the result in.
     */
    void translate(const Vector2& t, Matrix3* dst) const;

    /**
     * Transposes this matrix.
     */
    void transpose();

    /**
     * Transposes this matrix and stores the result in dst.
     *
     * @param dst A matrix to store the result in.
     */
    void transpose(Matrix3* dst) const;

    /**
     * Calculates the sum of this matrix with the given matrix.
     * 
     * Note: this does not modify this matrix.
     * 
     * @param m The matrix to add.
     * @return The matrix sum.
     */
    inline const Matrix3 operator+(const Matrix3& m) const;
    
    /**
     * Adds the given matrix to this matrix.
     * 
     * @param m The matrix to add.
     * @return This matrix, after the addition occurs.
     */
    inline Matrix3& operator+=(const Matrix3& m);

    /**
     * Calculates the difference of this matrix with the given matrix.
     * 
     * Note: this does not modify this matrix.
     * 
     * @param m The matrix to subtract.
     * @return The matrix difference.
     */
    inline const Matrix3 operator-(const Matrix3& m) const;

    /**
     * Subtracts the given matrix from this matrix.
     * 
     * @param m The matrix to subtract.
     * @return This matrix, after the subtraction occurs.
     */
    inline Matrix3& operator-=(const Matrix3& m);

    /**
     * Calculates the negation of this matrix.
     * 
     * Note: this does not modify this matrix.
     * 
     * @return The negation of this matrix.
     */
    inline const Matrix3 operator-() const;

    /**
     * Calculates the matrix product of this matrix with the given matrix.
     * 
     * Note: this does not modify this matrix.
     * 
     * @param m The matrix to multiply by.
     * @return The matrix product.
     */
    inline const Matrix3 operator*(const Matrix3& m) const;

    /**
     * Right-multiplies this matrix by the given matrix.
     * 
     * @param m The matrix to multiply by.
     * @return This matrix, after the multiplication occurs.
     */
    inline Matrix3& operator*=(const Matrix3& m);
};

/**
 * Transforms the given vector by the given matrix.
 * 
 * Note: this treats the given vector as a vector and not as a point.
 * 
 * @param v The vector to transform.
 * @param m The matrix to transform by.
 * @return This vector, after the transformation occurs.
 */
inline Vector2& operator*=(Vector2& v, const Matrix3& m);

/**
 * Transforms the given vector by the given matrix.
 * 
 * Note: this treats the given vector as a vector and not as a point.
 * 
 * @param m The matrix to transform by.
 * @param v The vector to transform.
 * @return The resulting transformed vector.
 */
inline const Vector2 operator*(const Matrix3& m, const Vector2& v);

}

#include "Matrix3.inl"

#endif
