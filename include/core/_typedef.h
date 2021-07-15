#pragma once
namespace omf{

///////////////////////////////////////////////////////
typedef long long int64;
typedef int int32;
typedef short int16;
typedef char int8;
typedef unsigned long long uint64;
typedef unsigned uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;

typedef union {
	uint32 u32;
	uint16 u16[2];
	uint8 u8[4];

	int32 i32;
	int16 i16[2];
	int8 i8[4];

	void* ptr;
}V32;

typedef union {
	uint64 u64;
	uint32 u32[2];
	uint16 u16[4];
	uint8 u8[8];

	int64 i64;
	int32 i32[2];
	int16 i16[4];
	int8 i8[8];

	void* ptr[2];
}V64;

enum class Position:unsigned {
	front = 0,
	current = 1,
	end = 2,
};

enum class Concurrent:unsigned {
	nonsupport = 0,
	safe = 1,
	blocking = 2,
};

enum class Approximate:int {
	prev = -1,
	nearest = 0,
	next = 1,
};
typedef union {
	uint32 u32;
	uint16 u16;
	uint8 u8;

	int32 i32;
	int16 i16;
	int8 i8;

	void* ptr;
}V32x;

typedef union {
	uint64 u64;
	uint32 u32;
	uint16 u16;
	uint8 u8;

	int64 i64;
	int32 i32;
	int16 i16;
	int8 i8;

	void* ptr;
}V64x;

const V64x __zero{0};
const char* const __nullstr{0};
///
typedef struct{
	const char* key;
	const char* val;
}KeyValuePair;
typedef struct{
	void* buf;
	int len;
}BufferSizePair;
typedef struct {
	int offset;
	int len;
}OffsetSizePair;
};

//////////////////////////////////////////////////





