/*
** LuaProfiler
** Copyright boswelyu 2018 (https://github.com/boswelyu/LuaProfiler)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>


#include "lua.h"
#include "lauxlib.h"
#include "profiler_stack.h"

#ifdef __ANDROID__
#include <android/log.h>

#define LOG_TAG "LuaProfiler"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

#define INFO_TAG "LuaProfilerInfo"
#define LOGI(...) __android_log_print(ANDROID_LOG_DEBUG, INFO_TAG, __VA_ARGS__);

#endif
#define PDEBUG_LOG 0

#define OUT_FILENAME "lua_cpu_profiler.out"
#define DEBUG_LOG_FILE "lua_profiler.log"

extern void profiler_call_hook_in(lua_Debug *ar);
extern void profiler_call_hook_ret(lua_Debug *ar);
extern void profiler_finish();

void profiler_debug(const char * format, ...);
void profiler_output(const char * format, ...);

FILE * g_outf;

#if PDEBUG_LOG
FILE * g_dlogf;
#endif

/* called by Lua or C (via the callhook mechanism) */
static void hook_func(lua_State *L, lua_Debug *ar) {
        
  lua_getinfo(L, "nS", ar);
  // Skip C functions
  if(strcmp(ar->what, "C") == 0 || ar->name == NULL || strcmp(ar->name, "__index") == 0 
    || strcmp(ar->name, "(for generator)") == 0 || strcmp(ar->namewhat, "local") == 0)
  {
    return;
  }

  switch(ar->event)
  {
    case LUA_HOOKCALL:
      /* entering a function */
      profiler_call_hook_in(ar);
      break;
    case LUA_HOOKRET:
      profiler_call_hook_ret(ar);
      break;
    default:
      profiler_debug("Unexcepted event: %d from function: %s", ar->event, ar->name);
      break;
  }
}


static int profiler_init(lua_State *L)
{
  char buffer[256];
#if PDEBUG_LOG
  // Create Debug log file,
#ifndef __ANDROID__
  strncpy(buffer, DEBUG_LOG_FILE, sizeof(buffer));
  g_dlogf = fopen(buffer, "w");
#endif
#endif

#ifndef __ANDROID__
	strncpy(buffer, OUT_FILENAME, sizeof(buffer));
  g_outf = fopen(buffer, "w");
  if ( g_outf == NULL ) 
  {
    profiler_debug("Create output file [%s] failed, error: %s", buffer, strerror(errno));
    return -1;
  }
#endif

  profiler_output("stack_level\tfunction_name\tcost_time\tsource_name\n");

  // Init Profile Record Stack
  if(prof_stack_init(512, 128) != 0)
  {
    profiler_debug("LuaProfiler Record Stack init failed");
    if(g_outf != NULL) fclose(g_outf);
    return -1;
  }
  
  profiler_debug("LuaProfielr Inited Successful");
  return 0;
}

static int profiler_start(lua_State *L) {
  
  if(profiler_init(L) != 0)
  {
    profiler_debug("LuaProfiler init failed");
    lua_pushboolean(L, 0);
    return 1;
  }

  lua_sethook(L, (lua_Hook)hook_func, LUA_MASKCALL | LUA_MASKRET, 0);

  lua_pushboolean(L, 1);
  return 1;
}

static int profiler_stop(lua_State *L) {
  
  lua_sethook(L, NULL, 0, 0);
  
  profiler_finish();

  lua_pushboolean(L, 1);
  return 1;
}

void profiler_debug(const char * format, ...)
{
#if PDEBUG_LOG
#ifndef __ANDROID__
  if(g_dlogf == NULL) return;
  va_list ap;
  va_start(ap, format);
  vfprintf(g_dlogf, format, ap);
  va_end(ap);
  fflush(g_dlogf);
#else
  char buffer[256];
  va_list ap;
  va_start(ap, format);
  vsnprintf(buffer, sizeof(buffer), format, ap);
  va_end(ap);
  LOGD("%s", buffer);
#endif
#endif
}

void profiler_output(const char * format, ...)
{
  char buffer[256];
#ifndef __ANDROID__
  if(g_outf == NULL) return;
  va_list ap;
  va_start(ap, format);
  vfprintf(g_outf, format, ap);
  va_end(ap);
  fflush(g_outf);
#else
  va_list ap;
  va_start(ap, format);
  vsnprintf(buffer, sizeof(buffer), format, ap);
  va_end(ap);
  LOGI("%s", buffer);
#endif
}

static const luaL_Reg prof_funcs[] = {
  { "start", profiler_start },
  { "stop", profiler_stop },
  { NULL, NULL }
};

int luaopen_profiler(lua_State *L) {
  luaL_openlib(L, "profiler", prof_funcs, 0);
  return 1;
}
