/*
** LuaProfiler
** Created by YuBo at 2018-03-01
*/

/*****************************************************************************
stack.c:
   Lua profiler record stack, implemented by dynamic array
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "profiler_stack.h"

static int s_capacity;
static int s_increase;
static PROF_STACK_RECORD * s_stack_ptr = NULL;
static int s_curr_index = -1;

static int _prof_stack_adjust_size();

int prof_stack_init(int capacity, int increase)
{
    if(s_stack_ptr != NULL)
    {
        if(capacity == s_capacity) {
            s_increase = increase;
            memset(s_stack_ptr, 0, sizeof(PROF_STACK_RECORD) * capacity);
            s_curr_index = -1;
            return 0;
        }else {
            free(s_stack_ptr);
        }
    }
    s_capacity = capacity;
    s_increase = increase;

    s_stack_ptr = (PROF_STACK_RECORD *)malloc(sizeof(PROF_STACK_RECORD) * capacity);
    if(s_stack_ptr == NULL)
    {
        printf("malloc prof stack failed with capacity: %d\n", capacity);
        return 0;
    }
    memset(s_stack_ptr, 0, sizeof(PROF_STACK_RECORD) * capacity);
    s_curr_index = -1;
    return 0;
}

PROF_STACK_RECORD * prof_stack_top()
{
    if(s_curr_index < 0) return NULL;
    return s_stack_ptr + s_curr_index;
}

PROF_STACK_RECORD * prof_stack_push()
{
    if(s_curr_index >= s_capacity - 1)
    {
        printf("ERROR: Profile stack full, Current capacity: %d\n", s_capacity);
        return NULL;
    }

    s_curr_index++;
    return s_stack_ptr + s_curr_index;
}

PROF_STACK_RECORD * prof_stack_pop()
{
    if(s_curr_index < 0)
    {
        return NULL;
    }

    s_curr_index--;
    return s_stack_ptr + s_curr_index + 1;
}

int prof_stack_level()
{
    return s_curr_index;
}

static int _prof_stack_adjust_size()
{
    PROF_STACK_RECORD * new_stack = (PROF_STACK_RECORD *)malloc(sizeof(PROF_STACK_RECORD) * (s_capacity + s_increase));
    if(new_stack == NULL)
    {
        printf("Resize prof stack failed, current capacity: %d, increase: %d", s_capacity, s_increase);
        return -1;
    }
    
    memcpy(new_stack, s_stack_ptr, (s_curr_index + 1) * sizeof(PROF_STACK_RECORD));
    memset(new_stack + s_curr_index + 1, 0, sizeof(PROF_STACK_RECORD) * (s_capacity + s_increase - s_curr_index - 1));

    s_capacity = s_capacity + s_increase;
    free(s_stack_ptr);
    s_stack_ptr = new_stack;
    return 0;
}