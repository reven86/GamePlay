#include "Base.h"
#include "Matrix3.h"
#include "MathUtil.h"

namespace gameplay
{

static const float MATRIX3_IDENTITY[9] =
{
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f,
};

Matrix3::Matrix3()
{
    *this = Matrix3::identity();
}

Matrix3::Matrix3(float m11, float m12, float m13, float m21, float m22, float m23, float m31, float m32, float m33)
{
    set(m11, m12, m13, m21, m22, m23, m31, m32, m33);
}

Matrix3::Matrix3(const float* m)
{
    set(m);
}

Matrix3::Matrix3(const Matrix3& copy)
{
    memcpy(m, copy.m, MATRIX3_SIZE);
}

Matrix3::~Matrix3()
{
}

const Matrix3& Matrix3::identity()
{
    static Matrix3 m(
        1, 0, 0,
        0, 1, 0,
        0, 0, 1 );
    return m;
}

const Matrix3& Matrix3::zero()
{
    static Matrix3 m(
        0, 0, 0,
        0, 0, 0,
        0, 0, 0 );
    return m;
}

void Matrix3::createScale(const Vector2& scale, Matrix3* dst)
{
    GP_ASSERT(dst);

    memcpy(dst, MATRIX3_IDENTITY, MATRIX3_SIZE);

    dst->m[0] = scale.x;
    dst->m[4] = scale.y;
}

void Matrix3::createScale(float xScale, float yScale, Matrix3* dst)
{
    GP_ASSERT(dst);

    memcpy(dst, MATRIX3_IDENTITY, MATRIX3_SIZE);

    dst->m[0] = xScale;
    dst->m[4] = yScale;
}

void Matrix3::createRotation(float angle, Matrix3* dst)
{
    GP_ASSERT(dst);

    memcpy(dst, MATRIX3_IDENTITY, MATRIX3_SIZE);

    float c = cos(angle);
    float s = sin(angle);

    dst->m[0] = c;
    dst->m[1] = s;
    dst->m[3] = -s;
    dst->m[4] = c;
}

void Matrix3::createTranslation(const Vector2& translation, Matrix3* dst)
{
    GP_ASSERT(dst);

    memcpy(dst, MATRIX3_IDENTITY, MATRIX3_SIZE);

    dst->m[6] = translation.x;
    dst->m[7] = translation.y;
}

void Matrix3::createTranslation(float xTranslation, float yTranslation, Matrix3* dst)
{
    GP_ASSERT(dst);

    memcpy(dst, MATRIX3_IDENTITY, MATRIX3_SIZE);

    dst->m[6] = xTranslation;
    dst->m[7] = yTranslation;
}

void Matrix3::add(float scalar)
{
    add(scalar, this);
}

void Matrix3::add(float scalar, Matrix3* dst)
{
    GP_ASSERT(dst);

    MathUtil::addMatrix3(m, scalar, dst->m);
}

void Matrix3::add(const Matrix3& m)
{
    add(*this, m, this);
}

void Matrix3::add(const Matrix3& m1, const Matrix3& m2, Matrix3* dst)
{
    GP_ASSERT(dst);

    MathUtil::addMatrix3(m1.m, m2.m, dst->m);
}

bool Matrix3::decompose(Vector2* scale, float* rotation, Vector2* translation) const
{
    if (translation)
    {
        // Extract the translation.
        translation->x = m[6];
        translation->y = m[7];
    }

    // Nothing left to do.
    if (scale == NULL && rotation == NULL)
        return true;

    // Extract the scale.
    // This is simply the length of each axis (row/column) in the Matrix3.
    Vector2 xaxis(m[0], m[1]);
    float scaleX = xaxis.length();

    Vector2 yaxis(m[3], m[4]);
    float scaleY = yaxis.length();

    // Determine if we have a negative scale (true if determinant is less than zero).
    // In this case, we simply negate a single axis of the scale.
    float det = determinant();
    if (det < 0)
        scaleY = -scaleY;

    if (scale)
    {
        scale->x = scaleX;
        scale->y = scaleY;
    }

    // Nothing left to do.
    if (rotation == NULL)
        return true;

    // Scale too close to zero, can't decompose rotation.
    if (scaleX < MATH_TOLERANCE || fabs(scaleY) < MATH_TOLERANCE)
        return false;

    *rotation = atan2(xaxis.y, xaxis.x);

    return true;
}

float Matrix3::determinant() const
{
    // Calculate the determinant.
    return m[0] * ( m[4] * m[8] - m[5] * m[7] ) - m[1] * ( m[3] * m[8] - m[5] * m[6] ) + m[2] * ( m[3] * m[7] - m[4] * m[6] );
}

void Matrix3::getScale(Vector2* scale) const
{
    decompose(scale, NULL, NULL);
}

bool Matrix3::getRotation(float* rotation) const
{
    return decompose(NULL, rotation, NULL);
}

void Matrix3::getTranslation(Vector2* translation) const
{
    decompose(NULL, NULL, translation);
}

void Matrix3::getUpVector(Vector2* dst) const
{
    GP_ASSERT(dst);

    dst->x = m[3];
    dst->y = m[4];
}

void Matrix3::getDownVector(Vector2* dst) const
{
    GP_ASSERT(dst);
    
    dst->x = -m[3];
    dst->y = -m[4];
}

void Matrix3::getLeftVector(Vector2* dst) const
{
    GP_ASSERT(dst);

    dst->x = -m[0];
    dst->y = -m[1];
}

void Matrix3::getRightVector(Vector2* dst) const
{
    GP_ASSERT(dst);

    dst->x = m[0];
    dst->y = m[1];
}

bool Matrix3::invert()
{
    return invert(this);
}

bool Matrix3::invert(Matrix3* dst) const
{
    // Calculate the determinant.
    float det = determinant( );

    // Close to zero, can't invert.
    if (fabs(det) <= MATH_TOLERANCE)
        return false;

    // Support the case where m == dst.
    Matrix3 inverse;
    inverse.m[0]  = m[4] * m[8] - m[5] * m[7];
    inverse.m[1]  = -m[1] * m[8] + m[2] * m[7];
    inverse.m[2]  = m[1] * m[5] - m[2] * m[4];

    inverse.m[3]  = -m[3] * m[8] + m[5] * m[6];
    inverse.m[4]  = m[0] * m[8] - m[2] * m[6];
    inverse.m[5]  = -m[0] * m[5] + m[2] * m[3];

    inverse.m[6]  = m[3] * m[7] - m[4] * m[6];
    inverse.m[7]  = -m[0] * m[7] + m[1] * m[6];
    inverse.m[8]  = m[0] * m[4] - m[1] * m[3];

    multiply(inverse, 1.0f / det, dst);

    return true;
}

bool Matrix3::isIdentity() const
{
    return (memcmp(m, MATRIX3_IDENTITY, MATRIX3_SIZE) == 0);
}

void Matrix3::multiply(float scalar)
{
    multiply(scalar, this);
}

void Matrix3::multiply(float scalar, Matrix3* dst) const
{
    multiply(*this, scalar, dst);
}

void Matrix3::multiply(const Matrix3& m, float scalar, Matrix3* dst)
{
    GP_ASSERT(dst);

    MathUtil::multiplyMatrix3(m.m, scalar, dst->m);
}

void Matrix3::multiply(const Matrix3& m)
{
    multiply(*this, m, this);
}

void Matrix3::multiply(const Matrix3& m1, const Matrix3& m2, Matrix3* dst)
{
    GP_ASSERT(dst);

    MathUtil::multiplyMatrix3(m1.m, m2.m, dst->m);
}

void Matrix3::negate()
{
    negate(this);
}

void Matrix3::negate(Matrix3* dst) const
{
    GP_ASSERT(dst);

    MathUtil::negateMatrix3(m, dst->m);
}

void Matrix3::rotate(float angle)
{
    rotate(angle, this);
}

void Matrix3::rotate(float angle, Matrix3* dst) const
{
    Matrix3 r;
    createRotation(angle, &r);
    multiply(*this, r, dst);
}

void Matrix3::scale(float value)
{
    scale(value, this);
}

void Matrix3::scale(float value, Matrix3* dst) const
{
    scale(value, value, dst);
}

void Matrix3::scale(float xScale, float yScale)
{
    scale(xScale, yScale, this);
}

void Matrix3::scale(float xScale, float yScale, Matrix3* dst) const
{
    Matrix3 s;
    createScale(xScale, yScale, &s);
    multiply(*this, s, dst);
}

void Matrix3::scale(const Vector2& s)
{
    scale(s.x, s.y, this);
}

void Matrix3::scale(const Vector2& s, Matrix3* dst) const
{
    scale(s.x, s.y, dst);
}

void Matrix3::set(float m11, float m12, float m13, float m21, float m22, float m23, float m31, float m32, float m33)
{
    m[0]  = m11;
    m[1]  = m21;
    m[2]  = m31;
    m[3]  = m12;
    m[4]  = m22;
    m[5]  = m32;
    m[6]  = m13;
    m[7]  = m23;
    m[8]  = m33;
}

void Matrix3::set(const float* m)
{
    GP_ASSERT(m);
    memcpy(this->m, m, MATRIX3_SIZE);
}

void Matrix3::set(const Matrix3& m)
{
    memcpy(this->m, m.m, MATRIX3_SIZE);
}

void Matrix3::setIdentity()
{
    memcpy(m, MATRIX3_IDENTITY, MATRIX3_SIZE);
}

void Matrix3::setZero()
{
    memset(m, 0, MATRIX3_SIZE);
}

void Matrix3::subtract(const Matrix3& m)
{
    subtract(*this, m, this);
}

void Matrix3::subtract(const Matrix3& m1, const Matrix3& m2, Matrix3* dst)
{
    GP_ASSERT(dst);

    MathUtil::subtractMatrix3(m1.m, m2.m, dst->m);
}

void Matrix3::transformPoint(Vector2* point) const
{
    GP_ASSERT(point);
    transformVector(point->x, point->y, 1.0f, point);
}

void Matrix3::transformPoint(const Vector2& point, Vector2* dst) const
{
    transformVector(point.x, point.y, 1.0f, dst);
}

void Matrix3::transformVector(Vector2* vector) const
{
    GP_ASSERT(vector);
    transformVector(vector->x, vector->y, 0.0f, vector);
}

void Matrix3::transformVector(const Vector2& vector, Vector2* dst) const
{
    transformVector(vector.x, vector.y, 0.0f, dst);
}

void Matrix3::transformVector(float x, float y, float z, Vector2* dst) const
{
    GP_ASSERT(dst);

    MathUtil::transformVector3(m, x, y, z, (float*)dst);
}

void Matrix3::transformVector(Vector3* vector) const
{
    GP_ASSERT(vector);
    transformVector(*vector, vector);
}

void Matrix3::transformVector(const Vector3& vector, Vector3* dst) const
{
    GP_ASSERT(dst);

    MathUtil::transformVector3(m, (const float*) &vector, (float*)dst);
}

void Matrix3::translate(float x, float y)
{
    translate(x, y, this);
}

void Matrix3::translate(float x, float y, Matrix3* dst) const
{
    Matrix3 t;
    createTranslation(x, y, &t);
    multiply(*this, t, dst);
}

void Matrix3::translate(const Vector2& t)
{
    translate(t.x, t.y, this);
}

void Matrix3::translate(const Vector2& t, Matrix3* dst) const
{
    translate(t.x, t.y, dst);
}

void Matrix3::transpose()
{
    transpose(this);
}

void Matrix3::transpose(Matrix3* dst) const
{
    GP_ASSERT(dst);

    MathUtil::transposeMatrix3(m, dst->m);
}

}
