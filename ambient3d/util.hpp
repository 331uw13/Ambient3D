#ifndef AMBIENT3D_UTIL_HPP
#define AMBIENT3D_UTIL_HPP

#include <initializer_list>
#include <string>

namespace AMutil {

    float normalize(float t, float min, float max);
    float lerp(float t, float min, float max);
    float map(float t, float src_min, float src_max, float dst_min, float dst_max);
    int randomi(int min, int max, int* seed);
    float randomf(float min, float max, int* seed);

    std::string combine_constchar(std::initializer_list<const char*> list);
    std::string combine_files(std::initializer_list<const char*> filenames);

};



#endif

