/*
** LuaProfiler
** Copyright boswelyu  2018 (https://github.com/boswelyu/LuaProfiler)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "profiler_stack.h"
#include "lua.h"
#include "lauxlib.h"

extern void profiler_debug(const char * format, ...);
extern void profiler_output(const char * format, ...);

    /* for faster execution (??) */
extern FILE * g_outf;

/* do not allow a string with '\n' and '|' (log file format reserved chars) */
/* - replace them by ' '                                                    */
static void formats(char *s) {
  if (!s) return;
  for (int i = strlen(s); i >= 0; i--) {
    if ((s[i] == '|') || (s[i] == '\n'))
      s[i] = ' ';
  }
}

/* 
 * Increase the record stack, record current time, and save function info into record 
 */
void profiler_call_hook_in(lua_Debug * ld) {	
  
  profiler_debug("Function IN: %s, ssrc: %s, what: %s, namewhat: %s\n", ld->name, ld->short_src, ld->what, ld->namewhat);
  PROF_STACK_RECORD * record = prof_stack_push();
  if(record == NULL)  
  {
    profiler_debug("Push Stack got NULL, source: %s\n", ld->source);
    return;
  }

  record->time_counter = clock();
  record->function_name = ld->name;
  record->source_name = ld->source;
}


/*
 * Calculate the running time of current function traced in stack, print on line to log file 
 */
void profiler_call_hook_ret(lua_Debug * ld) {

  profiler_debug("Function RET: %s, ssrc: %s, what: %s, namewhat: %s\n", ld->name, ld->short_src, ld->what, ld->namewhat);

  int stack_level = prof_stack_level();
  if(stack_level < 0) 
  {
    profiler_debug("Stack level already zero!\n");
    return;
  }

  PROF_STACK_RECORD * record = prof_stack_top();
  if(record == NULL)
  {
    profiler_debug("Internal Error: stack level > 0, but stack top retuned NULL");
    return;
  }

  if(strcmp(ld->name, record->function_name) != 0)
  {
    profiler_debug("[INFO] lua debug info (%s) not match record info (%s)\n", ld->name, record->function_name);
    return;
  }

  clock_t cost_time = clock() - record->time_counter;
  profiler_output("%d\t%s\t%d\t%s\n", stack_level, record->function_name, (unsigned int)cost_time, record->source_name);
  prof_stack_pop();
}

void profiler_finish() {
  if(g_outf) 
  {
    fflush(g_outf);
    fclose(g_outf);
    g_outf = NULL;
  }
}

