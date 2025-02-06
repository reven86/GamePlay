#include "Ray.h"

namespace gameplay
{

inline Ray& Ray::operator*=(const Matrix& matrix)
{
    transform(matrix);
    return *this;
}

inline const Ray operator*(const Matrix& matrix, const Ray& ray)
{
    Ray r(ray);
    r.transform(matrix);
    return r;
}

inline const Vector3& Ray::getOrigin() const
{
    return _origin;
}

inline const Vector3& Ray::getDirection() const
{
    return _direction;
}

}
