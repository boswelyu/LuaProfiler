/*
** LuaProfiler
** Created by YuBo at 2018-03-01
*/

/*****************************************************************************
stack.c:
   Lua profiler record stack, implemented by dynamic array
*****************************************************************************/

#ifndef _PROFILER_STACK_H_
#define _PROFILER_STACK_H_

#include <time.h>

#define MAX_FUNCTION_NAME 128

typedef struct lprofS_sSTACK_RECORD {
	clock_t time_counter;
	const char * function_name;
	const char * source_name;
}PROF_STACK_RECORD;


int prof_stack_init(int capacity, int increase);
PROF_STACK_RECORD * prof_stack_top();
PROF_STACK_RECORD * prof_stack_push();
PROF_STACK_RECORD * prof_stack_pop();
int prof_stack_level();

#endif
