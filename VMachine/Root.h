//	VMachine
//	Root header file, precompiled and included in all source files
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#pragma once

#include <cstdint>
#include <cassert>
#include <deque>
#include <exception>
#include <fstream>
#include <unordered_map>
#include <iomanip>
#include <list>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#include <memory>

#define NOMINMAX
#include <windows.h>
#define HEAP_CREATE_ENABLE_EXECUTE      0x00040000	//Missing from VC++ 2003 Platform SDK

#include <d3d9.h>
#include <dsound.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")

typedef uint8_t  Byte;
typedef uint16_t Word;
typedef uint32_t Dword;
typedef uint64_t Qword;
typedef int8_t   SignedByte;
typedef int16_t  SignedWord;
typedef int32_t  SignedDword;
typedef int64_t  SignedQword;

const double M_PI = 3.1415926535897932384626433832795;

#define VERSION_NUMBER_STRING "0.1.0"
#define COPYRIGHT_YEAR_STRING "2006"

#include "Utility.h"
#include "FramebufferData.h"
#include "SoundBufferData.h"
#include "Timer/Timer.h"

//Define to enable compilation of optional features
//#define DEBUG_OUTPUT
//#define DEBUG_OUTPUT_REGISTERS
//#define INVALID_OPCODES_FATAL
//#define FPS_COUNTER
#define USE_MICROCODE
#define BLOCK_CHAINING
#define JUMP_PATCHING
#define REMOVE_REDUNDANT_FLAGS
#define ZERO_BASE_OPTIMISATION
#define LAZY_EIP
#define PIT_TIMER_READ_RANDOM	//TODO: Remove this
#define USE_FMOPL

#ifndef USE_FMOPL
#define USE_YM3812_CHANNEL
#endif

#ifndef _DEBUG
//#define CONVERT_ASSERTS_TO_EXCEPTIONS
#endif

//Debug output stream
#ifdef DEBUG_OUTPUT
extern std::ofstream dbgOut;
#endif

//Convert asserts to exceptions, for basic debugging in release mode
//FILE_LINE_STRING credit: "Using __FILE__ and __LINE__ to Report Errors" by Curtis Krauskopf
//http://www.decompile.com/cpp/faq/file_and_line_error_string.htm
#ifdef CONVERT_ASSERTS_TO_EXCEPTIONS

//Disable warning C4005: 'assert' : macro redefinition
#pragma warning(push)
#pragma warning(disable: 4005)

#define STRINGIFY(x) #x
#define TO_STRING(x) STRINGIFY(x)
#define FILE_LINE_STRING __FILE__ ":" TO_STRING(__LINE__)
#define assert(x) if(x) ; else throw std::exception(FILE_LINE_STRING)

#pragma warning(pop)

#endif
