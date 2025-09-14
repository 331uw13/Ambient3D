#include <cmath>

#include "vec3.hpp"



float AM::Vec3::distance(const Vec3& b) const {
    const float dx = this->x - b.x;
    const float dy = this->y - b.y;
    const float dz = this->z - b.z;

    return sqrtf(dx*dx + dy*dy + dz*dz);
}
