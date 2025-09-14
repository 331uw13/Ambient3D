#ifndef AMBIENT3D_VECTOR3_HPP
#define AMBIENT3D_VECTOR3_HPP




namespace AM {

    struct Vec3 {
        float x { 0 };
        float y { 0 };
        float z { 0 };

        Vec3() {}
        Vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

        float distance(const Vec3& b) const;
    };


};




#endif
