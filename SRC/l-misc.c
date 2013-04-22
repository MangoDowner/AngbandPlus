/*
** Lua binding: misc
** Generated automatically by tolua 4.0a - angband.
*/

#include "lua/tolua.h"

/* Exported function */
int tolua_misc_open (lua_State* tolua_S);
void tolua_misc_close (lua_State* tolua_S);

#include "angband.h"
#include "script.h"

/* function to register type */
static void toluaI_reg_types (lua_State* tolua_S)
{
(void)tolua_S; /* Hack - prevent compiler warnings */
}

/* error messages */
#define TOLUA_ERR_SELF tolua_error(tolua_S,"invalid 'self'")
#define TOLUA_ERR_ASSIGN tolua_error(tolua_S,"#vinvalid type in variable assignment.")

/* get function: ddd */
static int toluaI_get_misc_ddd(lua_State* tolua_S)
{
 int toluaI_index;
 if (!tolua_istype(tolua_S,2,LUA_TNUMBER,0))
 tolua_error(tolua_S,"invalid type in array indexing.");
 toluaI_index = (int)tolua_getnumber(tolua_S,2,0)-1;
 if (toluaI_index<0 || toluaI_index>=9)
 tolua_error(tolua_S,"array indexing out of range.");
 tolua_pushnumber(tolua_S,(long)ddd[toluaI_index]);
 return 1;
}

/* get function: ddx */
static int toluaI_get_misc_ddx(lua_State* tolua_S)
{
 int toluaI_index;
 if (!tolua_istype(tolua_S,2,LUA_TNUMBER,0))
 tolua_error(tolua_S,"invalid type in array indexing.");
 toluaI_index = (int)tolua_getnumber(tolua_S,2,0)-1;
 if (toluaI_index<0 || toluaI_index>=10)
 tolua_error(tolua_S,"array indexing out of range.");
 tolua_pushnumber(tolua_S,(long)ddx[toluaI_index]);
 return 1;
}

/* get function: ddy */
static int toluaI_get_misc_ddy(lua_State* tolua_S)
{
 int toluaI_index;
 if (!tolua_istype(tolua_S,2,LUA_TNUMBER,0))
 tolua_error(tolua_S,"invalid type in array indexing.");
 toluaI_index = (int)tolua_getnumber(tolua_S,2,0)-1;
 if (toluaI_index<0 || toluaI_index>=10)
 tolua_error(tolua_S,"array indexing out of range.");
 tolua_pushnumber(tolua_S,(long)ddy[toluaI_index]);
 return 1;
}

/* get function: ddx_ddd */
static int toluaI_get_misc_ddx_ddd(lua_State* tolua_S)
{
 int toluaI_index;
 if (!tolua_istype(tolua_S,2,LUA_TNUMBER,0))
 tolua_error(tolua_S,"invalid type in array indexing.");
 toluaI_index = (int)tolua_getnumber(tolua_S,2,0)-1;
 if (toluaI_index<0 || toluaI_index>=9)
 tolua_error(tolua_S,"array indexing out of range.");
 tolua_pushnumber(tolua_S,(long)ddx_ddd[toluaI_index]);
 return 1;
}

/* get function: ddy_ddd */
static int toluaI_get_misc_ddy_ddd(lua_State* tolua_S)
{
 int toluaI_index;
 if (!tolua_istype(tolua_S,2,LUA_TNUMBER,0))
 tolua_error(tolua_S,"invalid type in array indexing.");
 toluaI_index = (int)tolua_getnumber(tolua_S,2,0)-1;
 if (toluaI_index<0 || toluaI_index>=9)
 tolua_error(tolua_S,"array indexing out of range.");
 tolua_pushnumber(tolua_S,(long)ddy_ddd[toluaI_index]);
 return 1;
}

/* function: script_do_file */
static int toluaI_misc_script_do_file00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TSTRING,0) ||
 !tolua_isnoobj(tolua_S,2)
 )
 goto tolua_lerror;
 else
 {
  cptr filename = ((cptr)  tolua_getstring(tolua_S,1,0));
 {
  bool toluaI_ret = (bool)  script_do_file(filename);
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'script_do_file'.");
 return 0;
}

/* Open function */
int tolua_misc_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 toluaI_reg_types(tolua_S);
 tolua_constant(tolua_S,NULL,"TRUE",TRUE);
 tolua_constant(tolua_S,NULL,"FALSE",FALSE);
 tolua_globalarray(tolua_S,"ddd",toluaI_get_misc_ddd,NULL);
 tolua_globalarray(tolua_S,"ddx",toluaI_get_misc_ddx,NULL);
 tolua_globalarray(tolua_S,"ddy",toluaI_get_misc_ddy,NULL);
 tolua_globalarray(tolua_S,"ddx_ddd",toluaI_get_misc_ddx_ddd,NULL);
 tolua_globalarray(tolua_S,"ddy_ddd",toluaI_get_misc_ddy_ddd,NULL);
 tolua_function(tolua_S,NULL,"script_do_file",toluaI_misc_script_do_file00);
 return 1;
}
/* Close function */
void tolua_misc_close (lua_State* tolua_S)
{
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"TRUE");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"FALSE");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"ddd");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"ddx");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"ddy");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"ddx_ddd");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"ddy_ddd");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"script_do_file");
}
