/* date = October 5th 2020 9:15 pm */

#ifndef IMGVU_H
#define IMGVU_H

// TODO(bumbread): let the platform layer decide whether 
// it needs to be linked to CRT.
#include<stdint.h>
int8_t typedef i8;
uint8_t typedef u8;
int16_t typedef i16;
uint16_t typedef u16;
int32_t typedef i32;
uint32_t typedef u32;
int64_t typedef i64;
uint64_t typedef u64;

enum {false, true} typedef bool;

float typedef r32;
double typedef r64;

u8 typedef byte;
u16 typedef word;
wchar_t typedef char16;

#define internal static
#define global static
#define persist static
#define loop for(;;)

#define assert(x) do { if(!(x)) { __debugbreak(); } } while(0)
#define array_length(a) (sizeof(a) / sizeof((a)[0]))

#include"string.c"
#include"input.h"

struct {
  
  int _stub;
  
} typedef t_program_state;

internal bool update_app(t_button* keyboard, t_program_state* programState, r32 dt);
internal void draw_app(t_program_state* programState);

#endif //IMGVU_H
