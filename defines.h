#pragma once
#include <cstdint>

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef float float32;
typedef double float64;

struct Vertex {
	float32 x;
	float32 y;
	float32 z;

	float32 u;
	float32 v;
	
	float32 r;
	float32 g;
	float32 b;
	float32 a;
};