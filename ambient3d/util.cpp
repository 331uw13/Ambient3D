#include <cstdio>
#include <raylib.h>

#include "util.hpp"


static int AMutil__randomgen (int* seed) {
    *seed = 0x343FD * *seed + 0x269EC3;
    return (*seed >> 16) & 0x7FFF;
}


float AMutil::normalize(float t, float min, float max) {
    return (t - min) / (max - min);
}

float AMutil::lerp(float t, float min, float max) {
    return (max - min) * t + min;
}

float AMutil::map(float t, float src_min, float src_max, float dst_min, float dst_max) {
    return (t - src_min) * (dst_max - dst_min) / (src_max - src_min) + dst_min;
}

int AMutil::randomi(int min, int max, int* seed) {
    return AMutil__randomgen(seed) % (max - min) + min;
}

float AMutil::randomf(float min, float max, int* seed) {
    return ((float)AMutil__randomgen(seed) / ((float)0x7FFF / (max - min))) + min;
}

std::string AMutil::combine_constchar(std::initializer_list<const char*> list) {
    std::string res = "";
    for(const char* c : list) {
        res += c;
    }
    return res;
}

std::string AMutil::combine_files(std::initializer_list<const char*> filenames) {
    std::string res = "";
    for(const char* fname : filenames) {
        if(!FileExists(fname)) {
            fprintf(stderr, "ERROR! %s: \"%s\" doesnt exist, or no permission to read.\n", 
                    __func__, fname);
            continue;
        }

        char* content = LoadFileText(fname);
        res += content;
        UnloadFileText(content);
    }
    return res;
}


