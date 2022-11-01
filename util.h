

#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define FLAG(i) 1 << i

#define ALLOC(size) malloc(size)
#define CALLOC(num, size) calloc(num, size)
#define REALLOC(ptr, size) realloc(ptr, size)
#define NEW(type) (type*) ALLOC(sizeof(type))
#define FREE(x) free(x)


typedef size_t Bool;
#define False 0
#define True 1



uint32_t getRandom();
int32_t getRandom(int32_t min_value, int32_t range_value);
float getRandomFloat(float min_value, float range_value);
inline double to_radian(double degree);




/*
    #define FOREACH(ELEMENT) \
        ELEMENT(element1) \
        ELEMENT(element2) \
        ELEMENT(element3) \

    typedef enum ELEMENT_ENUM {
        FOREACH_ELEMENT(GENERATE_ENUM)
    } ELEMENT_ENUM;

    static const char* ELEMENT_STRING[] = {
        FOREACH_ELEMENT(GENERATE_STRING)
    }

*/

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,





#ifdef UTIL_H_IMPLOMENTATION 

uint32_t m_w = 0x0011F3;
uint32_t m_z = 0x200FFA;

uint32_t getRandom(){
	m_z = 36969 * (m_z & 65535) + (m_z >> 16);
	m_w = 18000 * (m_w & 65535) + (m_w >> 16);
	return (m_z << 16) + m_w;
}

int32_t getRandom(int32_t min_value, int32_t range_value){
	return (int32_t)(((double)getRandom() / (double)UINT_MAX) * (double)range_value + (double)min_value);
}

double getRandomFloat(double min_value, double range_value){
	return (double)(((double)getRandom() / (double)UINT_MAX) * (double)range_value + (double)min_value);
}

inline double to_radian(double degree){
    return degree * 0.0174532925;
}

#endif //UTIL_H_IMPLOMENTATION


#endif //UTIL_H