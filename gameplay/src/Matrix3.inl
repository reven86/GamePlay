#include "Matrix3.h"

namespace gameplay
{

inline const Matrix3 Matrix3::operator+(const Matrix3& m) const
{
    Matrix3 result(*this);
    result.add(m);
    return result;
}

inline Matrix3& Matrix3::operator+=(const Matrix3& m)
{
    add(m);
    return *this;
}

inline const Matrix3 Matrix3::operator-(const Matrix3& m) const
{
    Matrix3 result(*this);
    result.subtract(m);
    return result;
}

inline Matrix3& Matrix3::operator-=(const Matrix3& m)
{
    subtract(m);
    return *this;
}

inline const Matrix3 Matrix3::operator-() const
{
    Matrix3 m(*this);
    m.negate();
    return m;
}

inline const Matrix3 Matrix3::operator*(const Matrix3& m) const
{
    Matrix3 result(*this);
    result.multiply(m);
    return result;
}

inline Matrix3& Matrix3::operator*=(const Matrix3& m)
{
    multiply(m);
    return *this;
}

inline Vector2& operator*=(Vector2& v, const Matrix3& m)
{
    m.transformVector(&v);
    return v;
}

inline const Vector2 operator*(const Matrix3& m, const Vector2& v)
{
    Vector2 x;
    m.transformVector(v, &x);
    return x;
}

}
