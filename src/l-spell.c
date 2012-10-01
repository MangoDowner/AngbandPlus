/*
** Lua binding: spell
** Generated automatically by tolua 4.0a - angband.
*/

#include "lua/tolua.h"

/* Exported function */
int tolua_spell_open (lua_State* tolua_S);
void tolua_spell_close (lua_State* tolua_S);

#include "angband.h"

/* function to register type */
static void toluaI_reg_types (lua_State* tolua_S)
{
(void)tolua_S; /* Hack - prevent compiler warnings */
 tolua_usertype(tolua_S,"size_t");
 tolua_usertype(tolua_S,"object_type");
 tolua_usertype(tolua_S,"monster_type");
}

/* error messages */
#define TOLUA_ERR_SELF tolua_error(tolua_S,"invalid 'self'")
#define TOLUA_ERR_ASSIGN tolua_error(tolua_S,"#vinvalid type in variable assignment.")

/* function: poly_r_idx */
static int toluaI_spell_poly_r_idx00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,2)
 )
 goto tolua_lerror;
 else
 {
  int r_idx = ((int)  tolua_getnumber(tolua_S,1,0));
 {
  s16b toluaI_ret = (s16b)  poly_r_idx(r_idx);
 tolua_pushnumber(tolua_S,(long)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'poly_r_idx'.");
 return 0;
}

/* function: teleport_away */
static int toluaI_spell_teleport_away00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,2,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,3)
 )
 goto tolua_lerror;
 else
 {
  int m_idx = ((int)  tolua_getnumber(tolua_S,1,0));
  int dis = ((int)  tolua_getnumber(tolua_S,2,0));
 {
  teleport_away(m_idx,dis);
 }
 }
 return 0;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'teleport_away'.");
 return 0;
}

/* function: teleport_player */
static int toluaI_spell_teleport_player00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,2)
 )
 goto tolua_lerror;
 else
 {
  int dis = ((int)  tolua_getnumber(tolua_S,1,0));
 {
  teleport_player(dis);
 }
 }
 return 0;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'teleport_player'.");
 return 0;
}

/* function: teleport_player_to */
static int toluaI_spell_teleport_player_to00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,2,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,3)
 )
 goto tolua_lerror;
 else
 {
  int ny = ((int)  tolua_getnumber(tolua_S,1,0));
  int nx = ((int)  tolua_getnumber(tolua_S,2,0));
 {
  teleport_player_to(ny,nx);
 }
 }
 return 0;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'teleport_player_to'.");
 return 0;
}

/* function: teleport_player_level */
static int toluaI_spell_teleport_player_level00(lua_State* tolua_S)
{
 if (
 !tolua_isnoobj(tolua_S,1)
 )
 goto tolua_lerror;
 else
 {
 {
  teleport_player_level();
 }
 }
 return 0;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'teleport_player_level'.");
 return 0;
}

/* function: take_hit */
static int toluaI_spell_take_hit00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,2,LUA_TSTRING,0) ||
 !tolua_isnoobj(tolua_S,3)
 )
 goto tolua_lerror;
 else
 {
  int dam = ((int)  tolua_getnumber(tolua_S,1,0));
  cptr kb_str = ((cptr)  tolua_getstring(tolua_S,2,0));
 {
  take_hit(dam,kb_str);
 }
 }
 return 0;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'take_hit'.");
 return 0;
}

/* function: acid_dam */
static int toluaI_spell_acid_dam00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,2,LUA_TSTRING,0) ||
 !tolua_isnoobj(tolua_S,3)
 )
 goto tolua_lerror;
 else
 {
  int dam = ((int)  tolua_getnumber(tolua_S,1,0));
  cptr kb_str = ((cptr)  tolua_getstring(tolua_S,2,0));
 {
  acid_dam(dam,kb_str);
 }
 }
 return 0;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'acid_dam'.");
 return 0;
}

/* function: elec_dam */
static int toluaI_spell_elec_dam00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,2,LUA_TSTRING,0) ||
 !tolua_isnoobj(tolua_S,3)
 )
 goto tolua_lerror;
 else
 {
  int dam = ((int)  tolua_getnumber(tolua_S,1,0));
  cptr kb_str = ((cptr)  tolua_getstring(tolua_S,2,0));
 {
  elec_dam(dam,kb_str);
 }
 }
 return 0;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'elec_dam'.");
 return 0;
}

/* function: fire_dam */
static int toluaI_spell_fire_dam00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,2,LUA_TSTRING,0) ||
 !tolua_isnoobj(tolua_S,3)
 )
 goto tolua_lerror;
 else
 {
  int dam = ((int)  tolua_getnumber(tolua_S,1,0));
  cptr kb_str = ((cptr)  tolua_getstring(tolua_S,2,0));
 {
  fire_dam(dam,kb_str);
 }
 }
 return 0;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'fire_dam'.");
 return 0;
}

/* function: cold_dam */
static int toluaI_spell_cold_dam00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,2,LUA_TSTRING,0) ||
 !tolua_isnoobj(tolua_S,3)
 )
 goto tolua_lerror;
 else
 {
  int dam = ((int)  tolua_getnumber(tolua_S,1,0));
  cptr kb_str = ((cptr)  tolua_getstring(tolua_S,2,0));
 {
  cold_dam(dam,kb_str);
 }
 }
 return 0;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'cold_dam'.");
 return 0;
}

/* function: inc_stat */
static int toluaI_spell_inc_stat00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,2)
 )
 goto tolua_lerror;
 else
 {
  int stat = ((int)  tolua_getnumber(tolua_S,1,0));
 {
  bool toluaI_ret = (bool)  inc_stat(stat);
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'inc_stat'.");
 return 0;
}

/* function: dec_stat */
static int toluaI_spell_dec_stat00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,2,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,3,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,4)
 )
 goto tolua_lerror;
 else
 {
  int stat = ((int)  tolua_getnumber(tolua_S,1,0));
  int amount = ((int)  tolua_getnumber(tolua_S,2,0));
  int permanent = ((int)  tolua_getnumber(tolua_S,3,0));
 {
  bool toluaI_ret = (bool)  dec_stat(stat,amount,permanent);
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'dec_stat'.");
 return 0;
}

/* function: res_stat */
static int toluaI_spell_res_stat00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,2)
 )
 goto tolua_lerror;
 else
 {
  int stat = ((int)  tolua_getnumber(tolua_S,1,0));
 {
  bool toluaI_ret = (bool)  res_stat(stat);
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'res_stat'.");
 return 0;
}

/* function: apply_disenchant */
static int toluaI_spell_apply_disenchant00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,2)
 )
 goto tolua_lerror;
 else
 {
  int mode = ((int)  tolua_getnumber(tolua_S,1,0));
 {
  bool toluaI_ret = (bool)  apply_disenchant(mode);
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'apply_disenchant'.");
 return 0;
}

/* function: project */
static int toluaI_spell_project00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,2,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,3,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,4,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,5,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,6,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,7,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,8,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,9,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,10,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,11,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,12)
 )
 goto tolua_lerror;
 else
 {
  int who = ((int)  tolua_getnumber(tolua_S,1,0));
  int rad = ((int)  tolua_getnumber(tolua_S,2,0));
  int y0 = ((int)  tolua_getnumber(tolua_S,3,0));
  int x0 = ((int)  tolua_getnumber(tolua_S,4,0));
  int y1 = ((int)  tolua_getnumber(tolua_S,5,0));
  int x1 = ((int)  tolua_getnumber(tolua_S,6,0));
  int dam = ((int)  tolua_getnumber(tolua_S,7,0));
  int typ = ((int)  tolua_getnumber(tolua_S,8,0));
  u32b flg = ((u32b)  tolua_getnumber(tolua_S,9,0));
  int degrees = ((int)  tolua_getnumber(tolua_S,10,0));
  byte source_diameter = ((byte)  tolua_getnumber(tolua_S,11,0));
 {
  bool toluaI_ret = (bool)  project(who,rad,y0,x0,y1,x1,dam,typ,flg,degrees,source_diameter);
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'project'.");
 return 0;
}

/* function: warding_glyph */
static int toluaI_spell_warding_glyph00(lua_State* tolua_S)
{
 if (
 !tolua_isnoobj(tolua_S,1)
 )
 goto tolua_lerror;
 else
 {
 {
  warding_glyph();
 }
 }
 return 0;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'warding_glyph'.");
 return 0;
}

/* function: make_monster_trap */
static int toluaI_spell_make_monster_trap00(lua_State* tolua_S)
{
 if (
 !tolua_isnoobj(tolua_S,1)
 )
 goto tolua_lerror;
 else
 {
 {
  bool toluaI_ret = (bool)  make_monster_trap();
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'make_monster_trap'.");
 return 0;
}

/* function: identify_pack */
static int toluaI_spell_identify_pack00(lua_State* tolua_S)
{
 if (
 !tolua_isnoobj(tolua_S,1)
 )
 goto tolua_lerror;
 else
 {
 {
  identify_pack();
 }
 }
 return 0;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'identify_pack'.");
 return 0;
}

/* function: remove_curse */
static int toluaI_spell_remove_curse00(lua_State* tolua_S)
{
 if (
 !tolua_isnoobj(tolua_S,1)
 )
 goto tolua_lerror;
 else
 {
 {
  bool toluaI_ret = (bool)  remove_curse();
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'remove_curse'.");
 return 0;
}

/* function: remove_all_curse */
static int toluaI_spell_remove_all_curse00(lua_State* tolua_S)
{
 if (
 !tolua_isnoobj(tolua_S,1)
 )
 goto tolua_lerror;
 else
 {
 {
  bool toluaI_ret = (bool)  remove_all_curse();
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'remove_all_curse'.");
 return 0;
}

/* function: self_knowledge */
static int toluaI_spell_self_knowledge00(lua_State* tolua_S)
{
 if (
 !tolua_isnoobj(tolua_S,1)
 )
 goto tolua_lerror;
 else
 {
 {
  self_knowledge();
 }
 }
 return 0;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'self_knowledge'.");
 return 0;
}

/* function: lose_all_info */
static int toluaI_spell_lose_all_info00(lua_State* tolua_S)
{
 if (
 !tolua_isnoobj(tolua_S,1)
 )
 goto tolua_lerror;
 else
 {
 {
  bool toluaI_ret = (bool)  lose_all_info();
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'lose_all_info'.");
 return 0;
}

/* function: set_recall */
static int toluaI_spell_set_recall00(lua_State* tolua_S)
{
 if (
 !tolua_isnoobj(tolua_S,1)
 )
 goto tolua_lerror;
 else
 {
 {
  set_recall();
 }
 }
 return 0;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_recall'.");
 return 0;
}

/* function: detect_traps */
static int toluaI_spell_detect_traps00(lua_State* tolua_S)
{
 if (
 !tolua_isnoobj(tolua_S,1)
 )
 goto tolua_lerror;
 else
 {
 {
  bool toluaI_ret = (bool)  detect_traps();
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'detect_traps'.");
 return 0;
}

/* function: detect_doors */
static int toluaI_spell_detect_doors00(lua_State* tolua_S)
{
 if (
 !tolua_isnoobj(tolua_S,1)
 )
 goto tolua_lerror;
 else
 {
 {
  bool toluaI_ret = (bool)  detect_doors();
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'detect_doors'.");
 return 0;
}

/* function: detect_stairs */
static int toluaI_spell_detect_stairs00(lua_State* tolua_S)
{
 if (
 !tolua_isnoobj(tolua_S,1)
 )
 goto tolua_lerror;
 else
 {
 {
  bool toluaI_ret = (bool)  detect_stairs();
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'detect_stairs'.");
 return 0;
}

/* function: detect_treasure */
static int toluaI_spell_detect_treasure00(lua_State* tolua_S)
{
 if (
 !tolua_isnoobj(tolua_S,1)
 )
 goto tolua_lerror;
 else
 {
 {
  bool toluaI_ret = (bool)  detect_treasure();
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'detect_treasure'.");
 return 0;
}

/* function: detect_objects_gold */
static int toluaI_spell_detect_objects_gold00(lua_State* tolua_S)
{
 if (
 !tolua_isnoobj(tolua_S,1)
 )
 goto tolua_lerror;
 else
 {
 {
  bool toluaI_ret = (bool)  detect_objects_gold();
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'detect_objects_gold'.");
 return 0;
}

/* function: detect_objects_normal */
static int toluaI_spell_detect_objects_normal00(lua_State* tolua_S)
{
 if (
 !tolua_isnoobj(tolua_S,1)
 )
 goto tolua_lerror;
 else
 {
 {
  bool toluaI_ret = (bool)  detect_objects_normal();
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'detect_objects_normal'.");
 return 0;
}

/* function: detect_objects_magic */
static int toluaI_spell_detect_objects_magic00(lua_State* tolua_S)
{
 if (
 !tolua_isnoobj(tolua_S,1)
 )
 goto tolua_lerror;
 else
 {
 {
  bool toluaI_ret = (bool)  detect_objects_magic();
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'detect_objects_magic'.");
 return 0;
}

/* function: detect_monsters_normal */
static int toluaI_spell_detect_monsters_normal00(lua_State* tolua_S)
{
 if (
 !tolua_isnoobj(tolua_S,1)
 )
 goto tolua_lerror;
 else
 {
 {
  bool toluaI_ret = (bool)  detect_monsters_normal();
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'detect_monsters_normal'.");
 return 0;
}

/* function: detect_monsters_invis */
static int toluaI_spell_detect_monsters_invis00(lua_State* tolua_S)
{
 if (
 !tolua_isnoobj(tolua_S,1)
 )
 goto tolua_lerror;
 else
 {
 {
  bool toluaI_ret = (bool)  detect_monsters_invis();
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'detect_monsters_invis'.");
 return 0;
}

/* function: detect_monsters_evil */
static int toluaI_spell_detect_monsters_evil00(lua_State* tolua_S)
{
 if (
 !tolua_isnoobj(tolua_S,1)
 )
 goto tolua_lerror;
 else
 {
 {
  bool toluaI_ret = (bool)  detect_monsters_evil();
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'detect_monsters_evil'.");
 return 0;
}

/* function: detect_all */
static int toluaI_spell_detect_all00(lua_State* tolua_S)
{
 if (
 !tolua_isnoobj(tolua_S,1)
 )
 goto tolua_lerror;
 else
 {
 {
  bool toluaI_ret = (bool)  detect_all();
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'detect_all'.");
 return 0;
}

/* function: stair_creation */
static int toluaI_spell_stair_creation00(lua_State* tolua_S)
{
 if (
 !tolua_isnoobj(tolua_S,1)
 )
 goto tolua_lerror;
 else
 {
 {
  stair_creation();
 }
 }
 return 0;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'stair_creation'.");
 return 0;
}

/* function: enchant */
static int toluaI_spell_enchant00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,tolua_tag(tolua_S,"object_type"),0) ||
 !tolua_istype(tolua_S,2,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,3,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,4)
 )
 goto tolua_lerror;
 else
 {
  object_type* o_ptr = ((object_type*)  tolua_getusertype(tolua_S,1,0));
  int n = ((int)  tolua_getnumber(tolua_S,2,0));
  int eflag = ((int)  tolua_getnumber(tolua_S,3,0));
 {
  bool toluaI_ret = (bool)  enchant(o_ptr,n,eflag);
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'enchant'.");
 return 0;
}

/* function: enchant_spell */
static int toluaI_spell_enchant_spell00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,2,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,3,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,4)
 )
 goto tolua_lerror;
 else
 {
  int num_hit = ((int)  tolua_getnumber(tolua_S,1,0));
  int num_dam = ((int)  tolua_getnumber(tolua_S,2,0));
  int num_ac = ((int)  tolua_getnumber(tolua_S,3,0));
 {
  bool toluaI_ret = (bool)  enchant_spell(num_hit,num_dam,num_ac);
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'enchant_spell'.");
 return 0;
}

/* function: ident_spell */
static int toluaI_spell_ident_spell00(lua_State* tolua_S)
{
 if (
 !tolua_isnoobj(tolua_S,1)
 )
 goto tolua_lerror;
 else
 {
 {
  bool toluaI_ret = (bool)  ident_spell();
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ident_spell'.");
 return 0;
}

/* function: identify_fully */
static int toluaI_spell_identify_fully00(lua_State* tolua_S)
{
 if (
 !tolua_isnoobj(tolua_S,1)
 )
 goto tolua_lerror;
 else
 {
 {
  bool toluaI_ret = (bool)  identify_fully();
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'identify_fully'.");
 return 0;
}

/* function: recharge */
static int toluaI_spell_recharge00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,2)
 )
 goto tolua_lerror;
 else
 {
  int num = ((int)  tolua_getnumber(tolua_S,1,0));
 {
  bool toluaI_ret = (bool)  recharge(num);
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'recharge'.");
 return 0;
}

/* function: speed_monsters */
static int toluaI_spell_speed_monsters00(lua_State* tolua_S)
{
 if (
 !tolua_isnoobj(tolua_S,1)
 )
 goto tolua_lerror;
 else
 {
 {
  bool toluaI_ret = (bool)  speed_monsters();
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'speed_monsters'.");
 return 0;
}

/* function: slow_monsters */
static int toluaI_spell_slow_monsters00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,2)
 )
 goto tolua_lerror;
 else
 {
  int power = ((int)  tolua_getnumber(tolua_S,1,0));
 {
  bool toluaI_ret = (bool)  slow_monsters(power);
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'slow_monsters'.");
 return 0;
}

/* function: sleep_monsters */
static int toluaI_spell_sleep_monsters00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,2)
 )
 goto tolua_lerror;
 else
 {
  int power = ((int)  tolua_getnumber(tolua_S,1,0));
 {
  bool toluaI_ret = (bool)  sleep_monsters(power);
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'sleep_monsters'.");
 return 0;
}

/* function: banish_evil */
static int toluaI_spell_banish_evil00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,2)
 )
 goto tolua_lerror;
 else
 {
  int dist = ((int)  tolua_getnumber(tolua_S,1,0));
 {
  bool toluaI_ret = (bool)  banish_evil(dist);
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'banish_evil'.");
 return 0;
}

/* function: turn_undead */
static int toluaI_spell_turn_undead00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,2)
 )
 goto tolua_lerror;
 else
 {
  int power = ((int)  tolua_getnumber(tolua_S,1,0));
 {
  bool toluaI_ret = (bool)  turn_undead(power);
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'turn_undead'.");
 return 0;
}

/* function: dispel_undead */
static int toluaI_spell_dispel_undead00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,2)
 )
 goto tolua_lerror;
 else
 {
  int dam = ((int)  tolua_getnumber(tolua_S,1,0));
 {
  bool toluaI_ret = (bool)  dispel_undead(dam);
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'dispel_undead'.");
 return 0;
}

/* function: dispel_evil */
static int toluaI_spell_dispel_evil00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,2)
 )
 goto tolua_lerror;
 else
 {
  int dam = ((int)  tolua_getnumber(tolua_S,1,0));
 {
  bool toluaI_ret = (bool)  dispel_evil(dam);
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'dispel_evil'.");
 return 0;
}

/* function: dispel_monsters */
static int toluaI_spell_dispel_monsters00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,2)
 )
 goto tolua_lerror;
 else
 {
  int dam = ((int)  tolua_getnumber(tolua_S,1,0));
 {
  bool toluaI_ret = (bool)  dispel_monsters(dam);
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'dispel_monsters'.");
 return 0;
}

/* function: aggravate_monsters */
static int toluaI_spell_aggravate_monsters00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,2)
 )
 goto tolua_lerror;
 else
 {
  int who = ((int)  tolua_getnumber(tolua_S,1,0));
 {
  aggravate_monsters(who);
 }
 }
 return 0;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'aggravate_monsters'.");
 return 0;
}

/* function: banishment */
static int toluaI_spell_banishment00(lua_State* tolua_S)
{
 if (
 !tolua_isnoobj(tolua_S,1)
 )
 goto tolua_lerror;
 else
 {
 {
  bool toluaI_ret = (bool)  banishment();
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'banishment'.");
 return 0;
}

/* function: mass_banishment */
static int toluaI_spell_mass_banishment00(lua_State* tolua_S)
{
 if (
 !tolua_isnoobj(tolua_S,1)
 )
 goto tolua_lerror;
 else
 {
 {
  bool toluaI_ret = (bool)  mass_banishment();
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'mass_banishment'.");
 return 0;
}

/* function: probing */
static int toluaI_spell_probing00(lua_State* tolua_S)
{
 if (
 !tolua_isnoobj(tolua_S,1)
 )
 goto tolua_lerror;
 else
 {
 {
  bool toluaI_ret = (bool)  probing();
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'probing'.");
 return 0;
}

/* function: make_monster_trap */
static int toluaI_spell_make_monster_trap01(lua_State* tolua_S)
{
 if (
 !tolua_isnoobj(tolua_S,1)
 )
 goto tolua_lerror;
 else
 {
 {
  bool toluaI_ret = (bool)  make_monster_trap();
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 return toluaI_spell_make_monster_trap00(tolua_S);
}

/* function: destroy_area */
static int toluaI_spell_destroy_area00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,2,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,3,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,4,tolua_tag(tolua_S,"bool"),0) ||
 !tolua_isnoobj(tolua_S,5)
 )
 goto tolua_lerror;
 else
 {
  int y1 = ((int)  tolua_getnumber(tolua_S,1,0));
  int x1 = ((int)  tolua_getnumber(tolua_S,2,0));
  int r = ((int)  tolua_getnumber(tolua_S,3,0));
  bool full = ((bool)  tolua_getbool(tolua_S,4,0));
 {
  destroy_area(y1,x1,r,full);
 }
 }
 return 0;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'destroy_area'.");
 return 0;
}

/* function: earthquake */
static int toluaI_spell_earthquake00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,2,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,3,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,4)
 )
 goto tolua_lerror;
 else
 {
  int cy = ((int)  tolua_getnumber(tolua_S,1,0));
  int cx = ((int)  tolua_getnumber(tolua_S,2,0));
  int r = ((int)  tolua_getnumber(tolua_S,3,0));
 {
  earthquake(cy,cx,r);
 }
 }
 return 0;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'earthquake'.");
 return 0;
}

/* function: lite_room */
static int toluaI_spell_lite_room00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,2,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,3)
 )
 goto tolua_lerror;
 else
 {
  int y1 = ((int)  tolua_getnumber(tolua_S,1,0));
  int x1 = ((int)  tolua_getnumber(tolua_S,2,0));
 {
  lite_room(y1,x1);
 }
 }
 return 0;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'lite_room'.");
 return 0;
}

/* function: unlite_room */
static int toluaI_spell_unlite_room00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,2,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,3)
 )
 goto tolua_lerror;
 else
 {
  int y1 = ((int)  tolua_getnumber(tolua_S,1,0));
  int x1 = ((int)  tolua_getnumber(tolua_S,2,0));
 {
  unlite_room(y1,x1);
 }
 }
 return 0;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'unlite_room'.");
 return 0;
}

/* function: lite_area */
static int toluaI_spell_lite_area00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,2,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,3)
 )
 goto tolua_lerror;
 else
 {
  int dam = ((int)  tolua_getnumber(tolua_S,1,0));
  int rad = ((int)  tolua_getnumber(tolua_S,2,0));
 {
  bool toluaI_ret = (bool)  lite_area(dam,rad);
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'lite_area'.");
 return 0;
}

/* function: unlite_area */
static int toluaI_spell_unlite_area00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,2,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,3)
 )
 goto tolua_lerror;
 else
 {
  int dam = ((int)  tolua_getnumber(tolua_S,1,0));
  int rad = ((int)  tolua_getnumber(tolua_S,2,0));
 {
  bool toluaI_ret = (bool)  unlite_area(dam,rad);
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'unlite_area'.");
 return 0;
}

/* function: fire_bolt_or_beam */
static int toluaI_spell_fire_bolt_or_beam00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,2,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,3,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,4,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,5)
 )
 goto tolua_lerror;
 else
 {
  int prob = ((int)  tolua_getnumber(tolua_S,1,0));
  int typ = ((int)  tolua_getnumber(tolua_S,2,0));
  int dir = ((int)  tolua_getnumber(tolua_S,3,0));
  int dam = ((int)  tolua_getnumber(tolua_S,4,0));
 {
  bool toluaI_ret = (bool)  fire_bolt_or_beam(prob,typ,dir,dam);
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'fire_bolt_or_beam'.");
 return 0;
}

/* function: fire_bolt_beam_special */
static int toluaI_spell_fire_bolt_beam_special00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,2,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,3,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,4,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,5,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,6)
 )
 goto tolua_lerror;
 else
 {
  int typ = ((int)  tolua_getnumber(tolua_S,1,0));
  int dir = ((int)  tolua_getnumber(tolua_S,2,0));
  int dam = ((int)  tolua_getnumber(tolua_S,3,0));
  int rad = ((int)  tolua_getnumber(tolua_S,4,0));
  u32b flg = ((u32b)  tolua_getnumber(tolua_S,5,0));
 {
  bool toluaI_ret = (bool)  fire_bolt_beam_special(typ,dir,dam,rad,flg);
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'fire_bolt_beam_special'.");
 return 0;
}

/* function: fire_ball */
static int toluaI_spell_fire_ball00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,2,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,3,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,4,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,5)
 )
 goto tolua_lerror;
 else
 {
  int typ = ((int)  tolua_getnumber(tolua_S,1,0));
  int dir = ((int)  tolua_getnumber(tolua_S,2,0));
  int dam = ((int)  tolua_getnumber(tolua_S,3,0));
  int rad = ((int)  tolua_getnumber(tolua_S,4,0));
 {
  bool toluaI_ret = (bool)  fire_ball(typ,dir,dam,rad);
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'fire_ball'.");
 return 0;
}

/* function: fire_orb */
static int toluaI_spell_fire_orb00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,2,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,3,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,4,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,5)
 )
 goto tolua_lerror;
 else
 {
  int typ = ((int)  tolua_getnumber(tolua_S,1,0));
  int dir = ((int)  tolua_getnumber(tolua_S,2,0));
  int dam = ((int)  tolua_getnumber(tolua_S,3,0));
  int rad = ((int)  tolua_getnumber(tolua_S,4,0));
 {
  bool toluaI_ret = (bool)  fire_orb(typ,dir,dam,rad);
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'fire_orb'.");
 return 0;
}

/* function: fire_ball_special */
static int toluaI_spell_fire_ball_special00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,2,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,3,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,4,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,5,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,6,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,7)
 )
 goto tolua_lerror;
 else
 {
  int typ = ((int)  tolua_getnumber(tolua_S,1,0));
  int dir = ((int)  tolua_getnumber(tolua_S,2,0));
  int dam = ((int)  tolua_getnumber(tolua_S,3,0));
  int rad = ((int)  tolua_getnumber(tolua_S,4,0));
  u32b flg = ((u32b)  tolua_getnumber(tolua_S,5,0));
  int source_diameter = ((int)  tolua_getnumber(tolua_S,6,0));
 {
  bool toluaI_ret = (bool)  fire_ball_special(typ,dir,dam,rad,flg,source_diameter);
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'fire_ball_special'.");
 return 0;
}

/* function: fire_arc */
static int toluaI_spell_fire_arc00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,2,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,3,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,4,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,5,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,6)
 )
 goto tolua_lerror;
 else
 {
  int typ = ((int)  tolua_getnumber(tolua_S,1,0));
  int dir = ((int)  tolua_getnumber(tolua_S,2,0));
  int dam = ((int)  tolua_getnumber(tolua_S,3,0));
  int rad = ((int)  tolua_getnumber(tolua_S,4,0));
  int degrees = ((int)  tolua_getnumber(tolua_S,5,0));
 {
  bool toluaI_ret = (bool)  fire_arc(typ,dir,dam,rad,degrees);
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'fire_arc'.");
 return 0;
}

/* function: fire_swarm */
static int toluaI_spell_fire_swarm00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,2,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,3,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,4,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,5,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,6)
 )
 goto tolua_lerror;
 else
 {
  int num = ((int)  tolua_getnumber(tolua_S,1,0));
  int typ = ((int)  tolua_getnumber(tolua_S,2,0));
  int dir = ((int)  tolua_getnumber(tolua_S,3,0));
  int dam = ((int)  tolua_getnumber(tolua_S,4,0));
  int rad = ((int)  tolua_getnumber(tolua_S,5,0));
 {
  bool toluaI_ret = (bool)  fire_swarm(num,typ,dir,dam,rad);
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'fire_swarm'.");
 return 0;
}

/* function: fire_bolt */
static int toluaI_spell_fire_bolt00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,2,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,3,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,4)
 )
 goto tolua_lerror;
 else
 {
  int typ = ((int)  tolua_getnumber(tolua_S,1,0));
  int dir = ((int)  tolua_getnumber(tolua_S,2,0));
  int dam = ((int)  tolua_getnumber(tolua_S,3,0));
 {
  bool toluaI_ret = (bool)  fire_bolt(typ,dir,dam);
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'fire_bolt'.");
 return 0;
}

/* function: fire_beam */
static int toluaI_spell_fire_beam00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,2,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,3,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,4)
 )
 goto tolua_lerror;
 else
 {
  int typ = ((int)  tolua_getnumber(tolua_S,1,0));
  int dir = ((int)  tolua_getnumber(tolua_S,2,0));
  int dam = ((int)  tolua_getnumber(tolua_S,3,0));
 {
  bool toluaI_ret = (bool)  fire_beam(typ,dir,dam);
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'fire_beam'.");
 return 0;
}

/* function: fire_bolt_or_beam */
static int toluaI_spell_fire_bolt_or_beam01(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,2,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,3,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,4,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,5)
 )
 goto tolua_lerror;
 else
 {
  int prob = ((int)  tolua_getnumber(tolua_S,1,0));
  int typ = ((int)  tolua_getnumber(tolua_S,2,0));
  int dir = ((int)  tolua_getnumber(tolua_S,3,0));
  int dam = ((int)  tolua_getnumber(tolua_S,4,0));
 {
  bool toluaI_ret = (bool)  fire_bolt_or_beam(prob,typ,dir,dam);
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 return toluaI_spell_fire_bolt_or_beam00(tolua_S);
}

/* function: project_arc */
static int toluaI_spell_project_arc00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,2,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,3,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,4,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,5,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,6,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,7,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,8,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,9,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,10,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,11)
 )
 goto tolua_lerror;
 else
 {
  int who = ((int)  tolua_getnumber(tolua_S,1,0));
  int rad = ((int)  tolua_getnumber(tolua_S,2,0));
  int y0 = ((int)  tolua_getnumber(tolua_S,3,0));
  int x0 = ((int)  tolua_getnumber(tolua_S,4,0));
  int y1 = ((int)  tolua_getnumber(tolua_S,5,0));
  int x1 = ((int)  tolua_getnumber(tolua_S,6,0));
  int dam = ((int)  tolua_getnumber(tolua_S,7,0));
  int typ = ((int)  tolua_getnumber(tolua_S,8,0));
  u32b flg = ((u32b)  tolua_getnumber(tolua_S,9,0));
  int degrees = ((int)  tolua_getnumber(tolua_S,10,0));
 {
  bool toluaI_ret = (bool)  project_arc(who,rad,y0,x0,y1,x1,dam,typ,flg,degrees);
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'project_arc'.");
 return 0;
}

/* function: project_star */
static int toluaI_spell_project_star00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,2,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,3,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,4,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,5,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,6,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,7,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,8)
 )
 goto tolua_lerror;
 else
 {
  int who = ((int)  tolua_getnumber(tolua_S,1,0));
  int rad = ((int)  tolua_getnumber(tolua_S,2,0));
  int y0 = ((int)  tolua_getnumber(tolua_S,3,0));
  int x0 = ((int)  tolua_getnumber(tolua_S,4,0));
  int dam = ((int)  tolua_getnumber(tolua_S,5,0));
  int typ = ((int)  tolua_getnumber(tolua_S,6,0));
  u32b flg = ((u32b)  tolua_getnumber(tolua_S,7,0));
 {
  bool toluaI_ret = (bool)  project_star(who,rad,y0,x0,dam,typ,flg);
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'project_star'.");
 return 0;
}

/* function: project_los */
static int toluaI_spell_project_los00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,2,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,3)
 )
 goto tolua_lerror;
 else
 {
  int typ = ((int)  tolua_getnumber(tolua_S,1,0));
  int dam = ((int)  tolua_getnumber(tolua_S,2,0));
 {
  bool toluaI_ret = (bool)  project_los(typ,dam);
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'project_los'.");
 return 0;
}

/* function: explosion */
static int toluaI_spell_explosion00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,2,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,3,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,4,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,5,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,6,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,7)
 )
 goto tolua_lerror;
 else
 {
  int who = ((int)  tolua_getnumber(tolua_S,1,0));
  int rad = ((int)  tolua_getnumber(tolua_S,2,0));
  int y0 = ((int)  tolua_getnumber(tolua_S,3,0));
  int x0 = ((int)  tolua_getnumber(tolua_S,4,0));
  int dam = ((int)  tolua_getnumber(tolua_S,5,0));
  int typ = ((int)  tolua_getnumber(tolua_S,6,0));
 {
  bool toluaI_ret = (bool)  explosion(who,rad,y0,x0,dam,typ);
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'explosion'.");
 return 0;
}

/* function: lite_line */
static int toluaI_spell_lite_line00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,2)
 )
 goto tolua_lerror;
 else
 {
  int dir = ((int)  tolua_getnumber(tolua_S,1,0));
 {
  bool toluaI_ret = (bool)  lite_line(dir);
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'lite_line'.");
 return 0;
}

/* function: strong_lite_line */
static int toluaI_spell_strong_lite_line00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,2)
 )
 goto tolua_lerror;
 else
 {
  int dir = ((int)  tolua_getnumber(tolua_S,1,0));
 {
  bool toluaI_ret = (bool)  strong_lite_line(dir);
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'strong_lite_line'.");
 return 0;
}

/* function: drain_life */
static int toluaI_spell_drain_life00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,2,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,3)
 )
 goto tolua_lerror;
 else
 {
  int dir = ((int)  tolua_getnumber(tolua_S,1,0));
  int dam = ((int)  tolua_getnumber(tolua_S,2,0));
 {
  bool toluaI_ret = (bool)  drain_life(dir,dam);
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'drain_life'.");
 return 0;
}

/* function: wall_to_mud */
static int toluaI_spell_wall_to_mud00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,2,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,3)
 )
 goto tolua_lerror;
 else
 {
  int dir = ((int)  tolua_getnumber(tolua_S,1,0));
  int dam = ((int)  tolua_getnumber(tolua_S,2,0));
 {
  bool toluaI_ret = (bool)  wall_to_mud(dir,dam);
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'wall_to_mud'.");
 return 0;
}

/* function: destroy_door */
static int toluaI_spell_destroy_door00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,2)
 )
 goto tolua_lerror;
 else
 {
  int dir = ((int)  tolua_getnumber(tolua_S,1,0));
 {
  bool toluaI_ret = (bool)  destroy_door(dir);
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'destroy_door'.");
 return 0;
}

/* function: disarm_trap */
static int toluaI_spell_disarm_trap00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,2)
 )
 goto tolua_lerror;
 else
 {
  int dir = ((int)  tolua_getnumber(tolua_S,1,0));
 {
  bool toluaI_ret = (bool)  disarm_trap(dir);
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'disarm_trap'.");
 return 0;
}

/* function: heal_monster */
static int toluaI_spell_heal_monster00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,2,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,3)
 )
 goto tolua_lerror;
 else
 {
  int dir = ((int)  tolua_getnumber(tolua_S,1,0));
  int dam = ((int)  tolua_getnumber(tolua_S,2,0));
 {
  bool toluaI_ret = (bool)  heal_monster(dir,dam);
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'heal_monster'.");
 return 0;
}

/* function: speed_monster */
static int toluaI_spell_speed_monster00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,2)
 )
 goto tolua_lerror;
 else
 {
  int dir = ((int)  tolua_getnumber(tolua_S,1,0));
 {
  bool toluaI_ret = (bool)  speed_monster(dir);
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'speed_monster'.");
 return 0;
}

/* function: slow_monster */
static int toluaI_spell_slow_monster00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,2)
 )
 goto tolua_lerror;
 else
 {
  int dir = ((int)  tolua_getnumber(tolua_S,1,0));
 {
  bool toluaI_ret = (bool)  slow_monster(dir);
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'slow_monster'.");
 return 0;
}

/* function: sleep_monster */
static int toluaI_spell_sleep_monster00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,2)
 )
 goto tolua_lerror;
 else
 {
  int dir = ((int)  tolua_getnumber(tolua_S,1,0));
 {
  bool toluaI_ret = (bool)  sleep_monster(dir);
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'sleep_monster'.");
 return 0;
}

/* function: confuse_monster */
static int toluaI_spell_confuse_monster00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,2,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,3)
 )
 goto tolua_lerror;
 else
 {
  int dir = ((int)  tolua_getnumber(tolua_S,1,0));
  int plev = ((int)  tolua_getnumber(tolua_S,2,0));
 {
  bool toluaI_ret = (bool)  confuse_monster(dir,plev);
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'confuse_monster'.");
 return 0;
}

/* function: poly_monster */
static int toluaI_spell_poly_monster00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,2)
 )
 goto tolua_lerror;
 else
 {
  int dir = ((int)  tolua_getnumber(tolua_S,1,0));
 {
  bool toluaI_ret = (bool)  poly_monster(dir);
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'poly_monster'.");
 return 0;
}

/* function: clone_monster */
static int toluaI_spell_clone_monster00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,2)
 )
 goto tolua_lerror;
 else
 {
  int dir = ((int)  tolua_getnumber(tolua_S,1,0));
 {
  bool toluaI_ret = (bool)  clone_monster(dir);
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'clone_monster'.");
 return 0;
}

/* function: fear_monster */
static int toluaI_spell_fear_monster00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,2,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,3)
 )
 goto tolua_lerror;
 else
 {
  int dir = ((int)  tolua_getnumber(tolua_S,1,0));
  int plev = ((int)  tolua_getnumber(tolua_S,2,0));
 {
  bool toluaI_ret = (bool)  fear_monster(dir,plev);
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'fear_monster'.");
 return 0;
}

/* function: teleport_monster */
static int toluaI_spell_teleport_monster00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,2)
 )
 goto tolua_lerror;
 else
 {
  int dir = ((int)  tolua_getnumber(tolua_S,1,0));
 {
  bool toluaI_ret = (bool)  teleport_monster(dir);
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'teleport_monster'.");
 return 0;
}

/* function: door_creation */
static int toluaI_spell_door_creation00(lua_State* tolua_S)
{
 if (
 !tolua_isnoobj(tolua_S,1)
 )
 goto tolua_lerror;
 else
 {
 {
  bool toluaI_ret = (bool)  door_creation();
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'door_creation'.");
 return 0;
}

/* function: trap_creation */
static int toluaI_spell_trap_creation00(lua_State* tolua_S)
{
 if (
 !tolua_isnoobj(tolua_S,1)
 )
 goto tolua_lerror;
 else
 {
 {
  bool toluaI_ret = (bool)  trap_creation();
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'trap_creation'.");
 return 0;
}

/* function: make_monster_trap */
static int toluaI_spell_make_monster_trap02(lua_State* tolua_S)
{
 if (
 !tolua_isnoobj(tolua_S,1)
 )
 goto tolua_lerror;
 else
 {
 {
  bool toluaI_ret = (bool)  make_monster_trap();
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 return toluaI_spell_make_monster_trap01(tolua_S);
}

/* function: destroy_doors_touch */
static int toluaI_spell_destroy_doors_touch00(lua_State* tolua_S)
{
 if (
 !tolua_isnoobj(tolua_S,1)
 )
 goto tolua_lerror;
 else
 {
 {
  bool toluaI_ret = (bool)  destroy_doors_touch();
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'destroy_doors_touch'.");
 return 0;
}

/* function: sleep_monsters_touch */
static int toluaI_spell_sleep_monsters_touch00(lua_State* tolua_S)
{
 if (
 !tolua_isnoobj(tolua_S,1)
 )
 goto tolua_lerror;
 else
 {
 {
  bool toluaI_ret = (bool)  sleep_monsters_touch();
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'sleep_monsters_touch'.");
 return 0;
}

/* function: curse_armor */
static int toluaI_spell_curse_armor00(lua_State* tolua_S)
{
 if (
 !tolua_isnoobj(tolua_S,1)
 )
 goto tolua_lerror;
 else
 {
 {
  bool toluaI_ret = (bool)  curse_armor();
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'curse_armor'.");
 return 0;
}

/* function: curse_weapon */
static int toluaI_spell_curse_weapon00(lua_State* tolua_S)
{
 if (
 !tolua_isnoobj(tolua_S,1)
 )
 goto tolua_lerror;
 else
 {
 {
  bool toluaI_ret = (bool)  curse_weapon();
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'curse_weapon'.");
 return 0;
}

/* function: brand_object */
static int toluaI_spell_brand_object00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,tolua_tag(tolua_S,"object_type"),0) ||
 !tolua_istype(tolua_S,2,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,3)
 )
 goto tolua_lerror;
 else
 {
  object_type* o_ptr = ((object_type*)  tolua_getusertype(tolua_S,1,0));
  byte brand_type = ((byte)  tolua_getnumber(tolua_S,2,0));
 {
  brand_object(o_ptr,brand_type);
 }
 }
 return 0;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'brand_object'.");
 return 0;
}

/* function: brand_weapon */
static int toluaI_spell_brand_weapon00(lua_State* tolua_S)
{
 if (
 !tolua_isnoobj(tolua_S,1)
 )
 goto tolua_lerror;
 else
 {
 {
  brand_weapon();
 }
 }
 return 0;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'brand_weapon'.");
 return 0;
}

/* function: brand_ammo */
static int toluaI_spell_brand_ammo00(lua_State* tolua_S)
{
 if (
 !tolua_isnoobj(tolua_S,1)
 )
 goto tolua_lerror;
 else
 {
 {
  bool toluaI_ret = (bool)  brand_ammo();
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'brand_ammo'.");
 return 0;
}

/* function: brand_bolts */
static int toluaI_spell_brand_bolts00(lua_State* tolua_S)
{
 if (
 !tolua_isnoobj(tolua_S,1)
 )
 goto tolua_lerror;
 else
 {
 {
  bool toluaI_ret = (bool)  brand_bolts();
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'brand_bolts'.");
 return 0;
}

/* function: ring_of_power */
static int toluaI_spell_ring_of_power00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,2)
 )
 goto tolua_lerror;
 else
 {
  int dir = ((int)  tolua_getnumber(tolua_S,1,0));
 {
  ring_of_power(dir);
 }
 }
 return 0;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ring_of_power'.");
 return 0;
}

/* function: map_area */
static int toluaI_spell_map_area00(lua_State* tolua_S)
{
 if (
 !tolua_isnoobj(tolua_S,1)
 )
 goto tolua_lerror;
 else
 {
 {
  map_area();
 }
 }
 return 0;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'map_area'.");
 return 0;
}

/* function: wiz_lite */
static int toluaI_spell_wiz_lite00(lua_State* tolua_S)
{
 if (
 !tolua_isnoobj(tolua_S,1)
 )
 goto tolua_lerror;
 else
 {
 {
  wiz_lite();
 }
 }
 return 0;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'wiz_lite'.");
 return 0;
}

/* function: wiz_dark */
static int toluaI_spell_wiz_dark00(lua_State* tolua_S)
{
 if (
 !tolua_isnoobj(tolua_S,1)
 )
 goto tolua_lerror;
 else
 {
 {
  wiz_dark();
 }
 }
 return 0;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'wiz_dark'.");
 return 0;
}

/* get function: mon_max */
static int toluaI_get_spell_mon_max(lua_State* tolua_S)
{
 tolua_pushnumber(tolua_S,(long)mon_max);
 return 1;
}

/* set function: mon_max */
static int toluaI_set_spell_mon_max(lua_State* tolua_S)
{
 if (!tolua_istype(tolua_S,1,LUA_TNUMBER,0))
 TOLUA_ERR_ASSIGN;
  mon_max = ((s16b)  tolua_getnumber(tolua_S,1,0));
 return 0;
}

/* get function: mon_cnt */
static int toluaI_get_spell_mon_cnt(lua_State* tolua_S)
{
 tolua_pushnumber(tolua_S,(long)mon_cnt);
 return 1;
}

/* set function: mon_cnt */
static int toluaI_set_spell_mon_cnt(lua_State* tolua_S)
{
 if (!tolua_istype(tolua_S,1,LUA_TNUMBER,0))
 TOLUA_ERR_ASSIGN;
  mon_cnt = ((s16b)  tolua_getnumber(tolua_S,1,0));
 return 0;
}

/* function: roff_top */
static int toluaI_spell_roff_top00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,2)
 )
 goto tolua_lerror;
 else
 {
  int r_idx = ((int)  tolua_getnumber(tolua_S,1,0));
 {
  roff_top(r_idx);
 }
 }
 return 0;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'roff_top'.");
 return 0;
}

/* function: screen_roff */
static int toluaI_spell_screen_roff00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,2)
 )
 goto tolua_lerror;
 else
 {
  int r_idx = ((int)  tolua_getnumber(tolua_S,1,0));
 {
  screen_roff(r_idx);
 }
 }
 return 0;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'screen_roff'.");
 return 0;
}

/* function: display_roff */
static int toluaI_spell_display_roff00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,2)
 )
 goto tolua_lerror;
 else
 {
  int r_idx = ((int)  tolua_getnumber(tolua_S,1,0));
 {
  display_roff(r_idx);
 }
 }
 return 0;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'display_roff'.");
 return 0;
}

/* function: delete_monster_idx */
static int toluaI_spell_delete_monster_idx00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,2)
 )
 goto tolua_lerror;
 else
 {
  int i = ((int)  tolua_getnumber(tolua_S,1,0));
 {
  delete_monster_idx(i);
 }
 }
 return 0;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'delete_monster_idx'.");
 return 0;
}

/* function: delete_monster */
static int toluaI_spell_delete_monster00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,2,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,3)
 )
 goto tolua_lerror;
 else
 {
  int y = ((int)  tolua_getnumber(tolua_S,1,0));
  int x = ((int)  tolua_getnumber(tolua_S,2,0));
 {
  delete_monster(y,x);
 }
 }
 return 0;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'delete_monster'.");
 return 0;
}

/* function: compact_monsters */
static int toluaI_spell_compact_monsters00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,2)
 )
 goto tolua_lerror;
 else
 {
  int size = ((int)  tolua_getnumber(tolua_S,1,0));
 {
  compact_monsters(size);
 }
 }
 return 0;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'compact_monsters'.");
 return 0;
}

/* function: wipe_mon_list */
static int toluaI_spell_wipe_mon_list00(lua_State* tolua_S)
{
 if (
 !tolua_isnoobj(tolua_S,1)
 )
 goto tolua_lerror;
 else
 {
 {
  wipe_mon_list();
 }
 }
 return 0;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'wipe_mon_list'.");
 return 0;
}

/* function: mon_pop */
static int toluaI_spell_mon_pop00(lua_State* tolua_S)
{
 if (
 !tolua_isnoobj(tolua_S,1)
 )
 goto tolua_lerror;
 else
 {
 {
  s16b toluaI_ret = (s16b)  mon_pop();
 tolua_pushnumber(tolua_S,(long)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'mon_pop'.");
 return 0;
}

/* function: get_mon_num_prep */
static int toluaI_spell_get_mon_num_prep00(lua_State* tolua_S)
{
 if (
 !tolua_isnoobj(tolua_S,1)
 )
 goto tolua_lerror;
 else
 {
 {
  errr toluaI_ret = (errr)  get_mon_num_prep();
 tolua_pushnumber(tolua_S,(long)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_mon_num_prep'.");
 return 0;
}

/* function: get_mon_num */
static int toluaI_spell_get_mon_num00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,2)
 )
 goto tolua_lerror;
 else
 {
  int level = ((int)  tolua_getnumber(tolua_S,1,0));
 {
  s16b toluaI_ret = (s16b)  get_mon_num(level);
 tolua_pushnumber(tolua_S,(long)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_mon_num'.");
 return 0;
}

/* function: monster_desc */
static int toluaI_spell_monster_desc00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TSTRING,0) ||
 !tolua_istype(tolua_S,2,tolua_tag(tolua_S,"size_t"),0) ||
 !tolua_istype(tolua_S,3,tolua_tag(tolua_S,"const monster_type"),0) ||
 !tolua_istype(tolua_S,4,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,5)
 )
 goto tolua_lerror;
 else
 {
  char* desc = ((char*)  tolua_getstring(tolua_S,1,0));
  size_t max = *((size_t*)  tolua_getusertype(tolua_S,2,0));
  const monster_type* m_ptr = ((const monster_type*)  tolua_getusertype(tolua_S,3,0));
  int mode = ((int)  tolua_getnumber(tolua_S,4,0));
 {
  monster_desc(desc,max,m_ptr,mode);
 }
 }
 return 0;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'monster_desc'.");
 return 0;
}

/* function: lore_do_probe */
static int toluaI_spell_lore_do_probe00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,2)
 )
 goto tolua_lerror;
 else
 {
  int m_idx = ((int)  tolua_getnumber(tolua_S,1,0));
 {
  lore_do_probe(m_idx);
 }
 }
 return 0;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'lore_do_probe'.");
 return 0;
}

/* function: lore_treasure */
static int toluaI_spell_lore_treasure00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,2,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,3,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,4)
 )
 goto tolua_lerror;
 else
 {
  int m_idx = ((int)  tolua_getnumber(tolua_S,1,0));
  int num_item = ((int)  tolua_getnumber(tolua_S,2,0));
  int num_gold = ((int)  tolua_getnumber(tolua_S,3,0));
 {
  lore_treasure(m_idx,num_item,num_gold);
 }
 }
 return 0;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'lore_treasure'.");
 return 0;
}

/* function: update_mon */
static int toluaI_spell_update_mon00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,2,tolua_tag(tolua_S,"bool"),0) ||
 !tolua_isnoobj(tolua_S,3)
 )
 goto tolua_lerror;
 else
 {
  int m_idx = ((int)  tolua_getnumber(tolua_S,1,0));
  bool full = ((bool)  tolua_getbool(tolua_S,2,0));
 {
  update_mon(m_idx,full);
 }
 }
 return 0;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'update_mon'.");
 return 0;
}

/* function: update_monsters */
static int toluaI_spell_update_monsters00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,tolua_tag(tolua_S,"bool"),0) ||
 !tolua_isnoobj(tolua_S,2)
 )
 goto tolua_lerror;
 else
 {
  bool full = ((bool)  tolua_getbool(tolua_S,1,0));
 {
  update_monsters(full);
 }
 }
 return 0;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'update_monsters'.");
 return 0;
}

/* function: monster_carry */
static int toluaI_spell_monster_carry00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,2,tolua_tag(tolua_S,"object_type"),0) ||
 !tolua_isnoobj(tolua_S,3)
 )
 goto tolua_lerror;
 else
 {
  int m_idx = ((int)  tolua_getnumber(tolua_S,1,0));
  object_type* j_ptr = ((object_type*)  tolua_getusertype(tolua_S,2,0));
 {
  s16b toluaI_ret = (s16b)  monster_carry(m_idx,j_ptr);
 tolua_pushnumber(tolua_S,(long)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'monster_carry'.");
 return 0;
}

/* function: monster_swap */
static int toluaI_spell_monster_swap00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,2,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,3,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,4,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,5)
 )
 goto tolua_lerror;
 else
 {
  int y1 = ((int)  tolua_getnumber(tolua_S,1,0));
  int x1 = ((int)  tolua_getnumber(tolua_S,2,0));
  int y2 = ((int)  tolua_getnumber(tolua_S,3,0));
  int x2 = ((int)  tolua_getnumber(tolua_S,4,0));
 {
  monster_swap(y1,x1,y2,x2);
 }
 }
 return 0;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'monster_swap'.");
 return 0;
}

/* function: player_place */
static int toluaI_spell_player_place00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,2,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,3)
 )
 goto tolua_lerror;
 else
 {
  int y = ((int)  tolua_getnumber(tolua_S,1,0));
  int x = ((int)  tolua_getnumber(tolua_S,2,0));
 {
  s16b toluaI_ret = (s16b)  player_place(y,x);
 tolua_pushnumber(tolua_S,(long)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'player_place'.");
 return 0;
}

/* function: monster_place */
static int toluaI_spell_monster_place00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,2,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,3,tolua_tag(tolua_S,"monster_type"),0) ||
 !tolua_isnoobj(tolua_S,4)
 )
 goto tolua_lerror;
 else
 {
  int y = ((int)  tolua_getnumber(tolua_S,1,0));
  int x = ((int)  tolua_getnumber(tolua_S,2,0));
  monster_type* n_ptr = ((monster_type*)  tolua_getusertype(tolua_S,3,0));
 {
  s16b toluaI_ret = (s16b)  monster_place(y,x,n_ptr);
 tolua_pushnumber(tolua_S,(long)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'monster_place'.");
 return 0;
}

/* function: place_monster_aux */
static int toluaI_spell_place_monster_aux00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,2,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,3,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,4,tolua_tag(tolua_S,"bool"),0) ||
 !tolua_istype(tolua_S,5,tolua_tag(tolua_S,"bool"),0) ||
 !tolua_isnoobj(tolua_S,6)
 )
 goto tolua_lerror;
 else
 {
  int y = ((int)  tolua_getnumber(tolua_S,1,0));
  int x = ((int)  tolua_getnumber(tolua_S,2,0));
  int r_idx = ((int)  tolua_getnumber(tolua_S,3,0));
  bool slp = ((bool)  tolua_getbool(tolua_S,4,0));
  bool grp = ((bool)  tolua_getbool(tolua_S,5,0));
 {
  bool toluaI_ret = (bool)  place_monster_aux(y,x,r_idx,slp,grp);
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'place_monster_aux'.");
 return 0;
}

/* function: place_monster */
static int toluaI_spell_place_monster00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,2,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,3,tolua_tag(tolua_S,"bool"),0) ||
 !tolua_istype(tolua_S,4,tolua_tag(tolua_S,"bool"),0) ||
 !tolua_isnoobj(tolua_S,5)
 )
 goto tolua_lerror;
 else
 {
  int y = ((int)  tolua_getnumber(tolua_S,1,0));
  int x = ((int)  tolua_getnumber(tolua_S,2,0));
  bool slp = ((bool)  tolua_getbool(tolua_S,3,0));
  bool grp = ((bool)  tolua_getbool(tolua_S,4,0));
 {
  bool toluaI_ret = (bool)  place_monster(y,x,slp,grp);
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'place_monster'.");
 return 0;
}

/* function: alloc_monster */
static int toluaI_spell_alloc_monster00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,2,tolua_tag(tolua_S,"bool"),0) ||
 !tolua_isnoobj(tolua_S,3)
 )
 goto tolua_lerror;
 else
 {
  int dis = ((int)  tolua_getnumber(tolua_S,1,0));
  bool slp = ((bool)  tolua_getbool(tolua_S,2,0));
 {
  bool toluaI_ret = (bool)  alloc_monster(dis,slp);
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'alloc_monster'.");
 return 0;
}

/* function: summon_specific */
static int toluaI_spell_summon_specific00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,2,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,3,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,4,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,5)
 )
 goto tolua_lerror;
 else
 {
  int y1 = ((int)  tolua_getnumber(tolua_S,1,0));
  int x1 = ((int)  tolua_getnumber(tolua_S,2,0));
  int lev = ((int)  tolua_getnumber(tolua_S,3,0));
  int type = ((int)  tolua_getnumber(tolua_S,4,0));
 {
  bool toluaI_ret = (bool)  summon_specific(y1,x1,lev,type);
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'summon_specific'.");
 return 0;
}

/* function: multiply_monster */
static int toluaI_spell_multiply_monster00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,2)
 )
 goto tolua_lerror;
 else
 {
  int m_idx = ((int)  tolua_getnumber(tolua_S,1,0));
 {
  bool toluaI_ret = (bool)  multiply_monster(m_idx);
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'multiply_monster'.");
 return 0;
}

/* function: message_pain */
static int toluaI_spell_message_pain00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,2,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,3)
 )
 goto tolua_lerror;
 else
 {
  int m_idx = ((int)  tolua_getnumber(tolua_S,1,0));
  int dam = ((int)  tolua_getnumber(tolua_S,2,0));
 {
  message_pain(m_idx,dam);
 }
 }
 return 0;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'message_pain'.");
 return 0;
}

/* function: update_smart_learn */
static int toluaI_spell_update_smart_learn00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,2,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,3)
 )
 goto tolua_lerror;
 else
 {
  int m_idx = ((int)  tolua_getnumber(tolua_S,1,0));
  int what = ((int)  tolua_getnumber(tolua_S,2,0));
 {
  update_smart_learn(m_idx,what);
 }
 }
 return 0;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'update_smart_learn'.");
 return 0;
}

/* function: monster_death */
static int toluaI_spell_monster_death00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,2)
 )
 goto tolua_lerror;
 else
 {
  int m_idx = ((int)  tolua_getnumber(tolua_S,1,0));
 {
  monster_death(m_idx);
 }
 }
 return 0;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'monster_death'.");
 return 0;
}

/* function: mon_take_hit */
static int toluaI_spell_mon_take_hit00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,2,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,3,tolua_tag(tolua_S,"bool"),0) ||
 !tolua_istype(tolua_S,4,LUA_TSTRING,0) ||
 !tolua_isnoobj(tolua_S,5)
 )
 goto tolua_lerror;
 else
 {
  int m_idx = ((int)  tolua_getnumber(tolua_S,1,0));
  int dam = ((int)  tolua_getnumber(tolua_S,2,0));
  bool fear = ((bool)  tolua_getbool(tolua_S,3,0));
  cptr note = ((cptr)  tolua_getstring(tolua_S,4,0));
 {
  bool toluaI_ret = (bool)  mon_take_hit(m_idx,dam,&fear,note);
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 tolua_pushbool(tolua_S,(int)fear);
 }
 }
 return 2;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'mon_take_hit'.");
 return 0;
}

/* Open function */
int tolua_spell_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 toluaI_reg_types(tolua_S);
 tolua_constant(tolua_S,NULL,"SUMMON_ANT",SUMMON_ANT);
 tolua_constant(tolua_S,NULL,"SUMMON_SPIDER",SUMMON_SPIDER);
 tolua_constant(tolua_S,NULL,"SUMMON_HOUND",SUMMON_HOUND);
 tolua_constant(tolua_S,NULL,"SUMMON_HYDRA",SUMMON_HYDRA);
 tolua_constant(tolua_S,NULL,"SUMMON_AINU",SUMMON_AINU);
 tolua_constant(tolua_S,NULL,"SUMMON_DEMON",SUMMON_DEMON);
 tolua_constant(tolua_S,NULL,"SUMMON_UNDEAD",SUMMON_UNDEAD);
 tolua_constant(tolua_S,NULL,"SUMMON_DRAGON",SUMMON_DRAGON);
 tolua_constant(tolua_S,NULL,"SUMMON_HI_DEMON",SUMMON_HI_DEMON);
 tolua_constant(tolua_S,NULL,"SUMMON_HI_UNDEAD",SUMMON_HI_UNDEAD);
 tolua_constant(tolua_S,NULL,"SUMMON_HI_DRAGON",SUMMON_HI_DRAGON);
 tolua_constant(tolua_S,NULL,"SUMMON_WRAITH",SUMMON_WRAITH);
 tolua_constant(tolua_S,NULL,"SUMMON_UNIQUE",SUMMON_UNIQUE);
 tolua_constant(tolua_S,NULL,"SUMMON_KIN",SUMMON_KIN);
 tolua_constant(tolua_S,NULL,"SUMMON_ANIMAL",SUMMON_ANIMAL);
 tolua_constant(tolua_S,NULL,"SUMMON_BERTBILLTOM",SUMMON_BERTBILLTOM);
 tolua_constant(tolua_S,NULL,"SUMMON_THIEF",SUMMON_THIEF);
 tolua_constant(tolua_S,NULL,"SUMMON_INDEX",SUMMON_INDEX);
 tolua_constant(tolua_S,NULL,"GF_XXX1",GF_XXX1);
 tolua_constant(tolua_S,NULL,"GF_ARROW",GF_ARROW);
 tolua_constant(tolua_S,NULL,"GF_MISSILE",GF_MISSILE);
 tolua_constant(tolua_S,NULL,"GF_MANA",GF_MANA);
 tolua_constant(tolua_S,NULL,"GF_HOLY_ORB",GF_HOLY_ORB);
 tolua_constant(tolua_S,NULL,"GF_LITE_WEAK",GF_LITE_WEAK);
 tolua_constant(tolua_S,NULL,"GF_DARK_WEAK",GF_DARK_WEAK);
 tolua_constant(tolua_S,NULL,"GF_WATER",GF_WATER);
 tolua_constant(tolua_S,NULL,"GF_PLASMA",GF_PLASMA);
 tolua_constant(tolua_S,NULL,"GF_METEOR",GF_METEOR);
 tolua_constant(tolua_S,NULL,"GF_ICE",GF_ICE);
 tolua_constant(tolua_S,NULL,"GF_GRAVITY",GF_GRAVITY);
 tolua_constant(tolua_S,NULL,"GF_INERTIA",GF_INERTIA);
 tolua_constant(tolua_S,NULL,"GF_FORCE",GF_FORCE);
 tolua_constant(tolua_S,NULL,"GF_TIME",GF_TIME);
 tolua_constant(tolua_S,NULL,"GF_ACID",GF_ACID);
 tolua_constant(tolua_S,NULL,"GF_ELEC",GF_ELEC);
 tolua_constant(tolua_S,NULL,"GF_FIRE",GF_FIRE);
 tolua_constant(tolua_S,NULL,"GF_COLD",GF_COLD);
 tolua_constant(tolua_S,NULL,"GF_POIS",GF_POIS);
 tolua_constant(tolua_S,NULL,"GF_XXX2",GF_XXX2);
 tolua_constant(tolua_S,NULL,"GF_LITE",GF_LITE);
 tolua_constant(tolua_S,NULL,"GF_DARK",GF_DARK);
 tolua_constant(tolua_S,NULL,"GF_XXX3",GF_XXX3);
 tolua_constant(tolua_S,NULL,"GF_CONFUSION",GF_CONFUSION);
 tolua_constant(tolua_S,NULL,"GF_SOUND",GF_SOUND);
 tolua_constant(tolua_S,NULL,"GF_SHARD",GF_SHARD);
 tolua_constant(tolua_S,NULL,"GF_NEXUS",GF_NEXUS);
 tolua_constant(tolua_S,NULL,"GF_NETHER",GF_NETHER);
 tolua_constant(tolua_S,NULL,"GF_CHAOS",GF_CHAOS);
 tolua_constant(tolua_S,NULL,"GF_DISENCHANT",GF_DISENCHANT);
 tolua_constant(tolua_S,NULL,"GF_WIND",GF_WIND);
 tolua_constant(tolua_S,NULL,"GF_KILL_WALL",GF_KILL_WALL);
 tolua_constant(tolua_S,NULL,"GF_KILL_DOOR",GF_KILL_DOOR);
 tolua_constant(tolua_S,NULL,"GF_KILL_TRAP",GF_KILL_TRAP);
 tolua_constant(tolua_S,NULL,"GF_MAKE_WALL",GF_MAKE_WALL);
 tolua_constant(tolua_S,NULL,"GF_MAKE_DOOR",GF_MAKE_DOOR);
 tolua_constant(tolua_S,NULL,"GF_MAKE_TRAP",GF_MAKE_TRAP);
 tolua_constant(tolua_S,NULL,"GF_XXX5",GF_XXX5);
 tolua_constant(tolua_S,NULL,"GF_XXX6",GF_XXX6);
 tolua_constant(tolua_S,NULL,"GF_AWAY_UNDEAD",GF_AWAY_UNDEAD);
 tolua_constant(tolua_S,NULL,"GF_AWAY_EVIL",GF_AWAY_EVIL);
 tolua_constant(tolua_S,NULL,"GF_AWAY_ALL",GF_AWAY_ALL);
 tolua_constant(tolua_S,NULL,"GF_TURN_UNDEAD",GF_TURN_UNDEAD);
 tolua_constant(tolua_S,NULL,"GF_TURN_EVIL",GF_TURN_EVIL);
 tolua_constant(tolua_S,NULL,"GF_TURN_ALL",GF_TURN_ALL);
 tolua_constant(tolua_S,NULL,"GF_DISP_UNDEAD",GF_DISP_UNDEAD);
 tolua_constant(tolua_S,NULL,"GF_DISP_EVIL",GF_DISP_EVIL);
 tolua_constant(tolua_S,NULL,"GF_DISP_ALL",GF_DISP_ALL);
 tolua_constant(tolua_S,NULL,"GF_MAKE_WARY",GF_MAKE_WARY);
 tolua_constant(tolua_S,NULL,"GF_OLD_CLONE",GF_OLD_CLONE);
 tolua_constant(tolua_S,NULL,"GF_OLD_POLY",GF_OLD_POLY);
 tolua_constant(tolua_S,NULL,"GF_OLD_HEAL",GF_OLD_HEAL);
 tolua_constant(tolua_S,NULL,"GF_OLD_SPEED",GF_OLD_SPEED);
 tolua_constant(tolua_S,NULL,"GF_OLD_SLOW",GF_OLD_SLOW);
 tolua_constant(tolua_S,NULL,"GF_OLD_CONF",GF_OLD_CONF);
 tolua_constant(tolua_S,NULL,"GF_OLD_SLEEP",GF_OLD_SLEEP);
 tolua_constant(tolua_S,NULL,"GF_OLD_DRAIN",GF_OLD_DRAIN);
 tolua_constant(tolua_S,NULL,"GF_SPORE",GF_SPORE);
 tolua_function(tolua_S,NULL,"poly_r_idx",toluaI_spell_poly_r_idx00);
 tolua_function(tolua_S,NULL,"teleport_away",toluaI_spell_teleport_away00);
 tolua_function(tolua_S,NULL,"teleport_player",toluaI_spell_teleport_player00);
 tolua_function(tolua_S,NULL,"teleport_player_to",toluaI_spell_teleport_player_to00);
 tolua_function(tolua_S,NULL,"teleport_player_level",toluaI_spell_teleport_player_level00);
 tolua_function(tolua_S,NULL,"take_hit",toluaI_spell_take_hit00);
 tolua_function(tolua_S,NULL,"acid_dam",toluaI_spell_acid_dam00);
 tolua_function(tolua_S,NULL,"elec_dam",toluaI_spell_elec_dam00);
 tolua_function(tolua_S,NULL,"fire_dam",toluaI_spell_fire_dam00);
 tolua_function(tolua_S,NULL,"cold_dam",toluaI_spell_cold_dam00);
 tolua_function(tolua_S,NULL,"inc_stat",toluaI_spell_inc_stat00);
 tolua_function(tolua_S,NULL,"dec_stat",toluaI_spell_dec_stat00);
 tolua_function(tolua_S,NULL,"res_stat",toluaI_spell_res_stat00);
 tolua_function(tolua_S,NULL,"apply_disenchant",toluaI_spell_apply_disenchant00);
 tolua_function(tolua_S,NULL,"project",toluaI_spell_project00);
 tolua_function(tolua_S,NULL,"warding_glyph",toluaI_spell_warding_glyph00);
 tolua_function(tolua_S,NULL,"make_monster_trap",toluaI_spell_make_monster_trap00);
 tolua_function(tolua_S,NULL,"identify_pack",toluaI_spell_identify_pack00);
 tolua_function(tolua_S,NULL,"remove_curse",toluaI_spell_remove_curse00);
 tolua_function(tolua_S,NULL,"remove_all_curse",toluaI_spell_remove_all_curse00);
 tolua_function(tolua_S,NULL,"self_knowledge",toluaI_spell_self_knowledge00);
 tolua_function(tolua_S,NULL,"lose_all_info",toluaI_spell_lose_all_info00);
 tolua_function(tolua_S,NULL,"set_recall",toluaI_spell_set_recall00);
 tolua_function(tolua_S,NULL,"detect_traps",toluaI_spell_detect_traps00);
 tolua_function(tolua_S,NULL,"detect_doors",toluaI_spell_detect_doors00);
 tolua_function(tolua_S,NULL,"detect_stairs",toluaI_spell_detect_stairs00);
 tolua_function(tolua_S,NULL,"detect_treasure",toluaI_spell_detect_treasure00);
 tolua_function(tolua_S,NULL,"detect_objects_gold",toluaI_spell_detect_objects_gold00);
 tolua_function(tolua_S,NULL,"detect_objects_normal",toluaI_spell_detect_objects_normal00);
 tolua_function(tolua_S,NULL,"detect_objects_magic",toluaI_spell_detect_objects_magic00);
 tolua_function(tolua_S,NULL,"detect_monsters_normal",toluaI_spell_detect_monsters_normal00);
 tolua_function(tolua_S,NULL,"detect_monsters_invis",toluaI_spell_detect_monsters_invis00);
 tolua_function(tolua_S,NULL,"detect_monsters_evil",toluaI_spell_detect_monsters_evil00);
 tolua_function(tolua_S,NULL,"detect_all",toluaI_spell_detect_all00);
 tolua_function(tolua_S,NULL,"stair_creation",toluaI_spell_stair_creation00);
 tolua_function(tolua_S,NULL,"enchant",toluaI_spell_enchant00);
 tolua_function(tolua_S,NULL,"enchant_spell",toluaI_spell_enchant_spell00);
 tolua_function(tolua_S,NULL,"ident_spell",toluaI_spell_ident_spell00);
 tolua_function(tolua_S,NULL,"identify_fully",toluaI_spell_identify_fully00);
 tolua_function(tolua_S,NULL,"recharge",toluaI_spell_recharge00);
 tolua_function(tolua_S,NULL,"speed_monsters",toluaI_spell_speed_monsters00);
 tolua_function(tolua_S,NULL,"slow_monsters",toluaI_spell_slow_monsters00);
 tolua_function(tolua_S,NULL,"sleep_monsters",toluaI_spell_sleep_monsters00);
 tolua_function(tolua_S,NULL,"banish_evil",toluaI_spell_banish_evil00);
 tolua_function(tolua_S,NULL,"turn_undead",toluaI_spell_turn_undead00);
 tolua_function(tolua_S,NULL,"dispel_undead",toluaI_spell_dispel_undead00);
 tolua_function(tolua_S,NULL,"dispel_evil",toluaI_spell_dispel_evil00);
 tolua_function(tolua_S,NULL,"dispel_monsters",toluaI_spell_dispel_monsters00);
 tolua_function(tolua_S,NULL,"aggravate_monsters",toluaI_spell_aggravate_monsters00);
 tolua_function(tolua_S,NULL,"banishment",toluaI_spell_banishment00);
 tolua_function(tolua_S,NULL,"mass_banishment",toluaI_spell_mass_banishment00);
 tolua_function(tolua_S,NULL,"probing",toluaI_spell_probing00);
 tolua_function(tolua_S,NULL,"make_monster_trap",toluaI_spell_make_monster_trap01);
 tolua_function(tolua_S,NULL,"destroy_area",toluaI_spell_destroy_area00);
 tolua_function(tolua_S,NULL,"earthquake",toluaI_spell_earthquake00);
 tolua_function(tolua_S,NULL,"lite_room",toluaI_spell_lite_room00);
 tolua_function(tolua_S,NULL,"unlite_room",toluaI_spell_unlite_room00);
 tolua_function(tolua_S,NULL,"lite_area",toluaI_spell_lite_area00);
 tolua_function(tolua_S,NULL,"unlite_area",toluaI_spell_unlite_area00);
 tolua_function(tolua_S,NULL,"fire_bolt_or_beam",toluaI_spell_fire_bolt_or_beam00);
 tolua_function(tolua_S,NULL,"fire_bolt_beam_special",toluaI_spell_fire_bolt_beam_special00);
 tolua_function(tolua_S,NULL,"fire_ball",toluaI_spell_fire_ball00);
 tolua_function(tolua_S,NULL,"fire_orb",toluaI_spell_fire_orb00);
 tolua_function(tolua_S,NULL,"fire_ball_special",toluaI_spell_fire_ball_special00);
 tolua_function(tolua_S,NULL,"fire_arc",toluaI_spell_fire_arc00);
 tolua_function(tolua_S,NULL,"fire_swarm",toluaI_spell_fire_swarm00);
 tolua_function(tolua_S,NULL,"fire_bolt",toluaI_spell_fire_bolt00);
 tolua_function(tolua_S,NULL,"fire_beam",toluaI_spell_fire_beam00);
 tolua_function(tolua_S,NULL,"fire_bolt_or_beam",toluaI_spell_fire_bolt_or_beam01);
 tolua_function(tolua_S,NULL,"project_arc",toluaI_spell_project_arc00);
 tolua_function(tolua_S,NULL,"project_star",toluaI_spell_project_star00);
 tolua_function(tolua_S,NULL,"project_los",toluaI_spell_project_los00);
 tolua_function(tolua_S,NULL,"explosion",toluaI_spell_explosion00);
 tolua_function(tolua_S,NULL,"lite_line",toluaI_spell_lite_line00);
 tolua_function(tolua_S,NULL,"strong_lite_line",toluaI_spell_strong_lite_line00);
 tolua_function(tolua_S,NULL,"drain_life",toluaI_spell_drain_life00);
 tolua_function(tolua_S,NULL,"wall_to_mud",toluaI_spell_wall_to_mud00);
 tolua_function(tolua_S,NULL,"destroy_door",toluaI_spell_destroy_door00);
 tolua_function(tolua_S,NULL,"disarm_trap",toluaI_spell_disarm_trap00);
 tolua_function(tolua_S,NULL,"heal_monster",toluaI_spell_heal_monster00);
 tolua_function(tolua_S,NULL,"speed_monster",toluaI_spell_speed_monster00);
 tolua_function(tolua_S,NULL,"slow_monster",toluaI_spell_slow_monster00);
 tolua_function(tolua_S,NULL,"sleep_monster",toluaI_spell_sleep_monster00);
 tolua_function(tolua_S,NULL,"confuse_monster",toluaI_spell_confuse_monster00);
 tolua_function(tolua_S,NULL,"poly_monster",toluaI_spell_poly_monster00);
 tolua_function(tolua_S,NULL,"clone_monster",toluaI_spell_clone_monster00);
 tolua_function(tolua_S,NULL,"fear_monster",toluaI_spell_fear_monster00);
 tolua_function(tolua_S,NULL,"teleport_monster",toluaI_spell_teleport_monster00);
 tolua_function(tolua_S,NULL,"door_creation",toluaI_spell_door_creation00);
 tolua_function(tolua_S,NULL,"trap_creation",toluaI_spell_trap_creation00);
 tolua_function(tolua_S,NULL,"make_monster_trap",toluaI_spell_make_monster_trap02);
 tolua_function(tolua_S,NULL,"destroy_doors_touch",toluaI_spell_destroy_doors_touch00);
 tolua_function(tolua_S,NULL,"sleep_monsters_touch",toluaI_spell_sleep_monsters_touch00);
 tolua_function(tolua_S,NULL,"curse_armor",toluaI_spell_curse_armor00);
 tolua_function(tolua_S,NULL,"curse_weapon",toluaI_spell_curse_weapon00);
 tolua_function(tolua_S,NULL,"brand_object",toluaI_spell_brand_object00);
 tolua_function(tolua_S,NULL,"brand_weapon",toluaI_spell_brand_weapon00);
 tolua_function(tolua_S,NULL,"brand_ammo",toluaI_spell_brand_ammo00);
 tolua_function(tolua_S,NULL,"brand_bolts",toluaI_spell_brand_bolts00);
 tolua_function(tolua_S,NULL,"ring_of_power",toluaI_spell_ring_of_power00);
 tolua_function(tolua_S,NULL,"map_area",toluaI_spell_map_area00);
 tolua_function(tolua_S,NULL,"wiz_lite",toluaI_spell_wiz_lite00);
 tolua_function(tolua_S,NULL,"wiz_dark",toluaI_spell_wiz_dark00);
 tolua_globalvar(tolua_S,"mon_max",toluaI_get_spell_mon_max,toluaI_set_spell_mon_max);
 tolua_globalvar(tolua_S,"mon_cnt",toluaI_get_spell_mon_cnt,toluaI_set_spell_mon_cnt);
 tolua_function(tolua_S,NULL,"roff_top",toluaI_spell_roff_top00);
 tolua_function(tolua_S,NULL,"screen_roff",toluaI_spell_screen_roff00);
 tolua_function(tolua_S,NULL,"display_roff",toluaI_spell_display_roff00);
 tolua_function(tolua_S,NULL,"delete_monster_idx",toluaI_spell_delete_monster_idx00);
 tolua_function(tolua_S,NULL,"delete_monster",toluaI_spell_delete_monster00);
 tolua_function(tolua_S,NULL,"compact_monsters",toluaI_spell_compact_monsters00);
 tolua_function(tolua_S,NULL,"wipe_mon_list",toluaI_spell_wipe_mon_list00);
 tolua_function(tolua_S,NULL,"mon_pop",toluaI_spell_mon_pop00);
 tolua_function(tolua_S,NULL,"get_mon_num_prep",toluaI_spell_get_mon_num_prep00);
 tolua_function(tolua_S,NULL,"get_mon_num",toluaI_spell_get_mon_num00);
 tolua_function(tolua_S,NULL,"monster_desc",toluaI_spell_monster_desc00);
 tolua_function(tolua_S,NULL,"lore_do_probe",toluaI_spell_lore_do_probe00);
 tolua_function(tolua_S,NULL,"lore_treasure",toluaI_spell_lore_treasure00);
 tolua_function(tolua_S,NULL,"update_mon",toluaI_spell_update_mon00);
 tolua_function(tolua_S,NULL,"update_monsters",toluaI_spell_update_monsters00);
 tolua_function(tolua_S,NULL,"monster_carry",toluaI_spell_monster_carry00);
 tolua_function(tolua_S,NULL,"monster_swap",toluaI_spell_monster_swap00);
 tolua_function(tolua_S,NULL,"player_place",toluaI_spell_player_place00);
 tolua_function(tolua_S,NULL,"monster_place",toluaI_spell_monster_place00);
 tolua_function(tolua_S,NULL,"place_monster_aux",toluaI_spell_place_monster_aux00);
 tolua_function(tolua_S,NULL,"place_monster",toluaI_spell_place_monster00);
 tolua_function(tolua_S,NULL,"alloc_monster",toluaI_spell_alloc_monster00);
 tolua_function(tolua_S,NULL,"summon_specific",toluaI_spell_summon_specific00);
 tolua_function(tolua_S,NULL,"multiply_monster",toluaI_spell_multiply_monster00);
 tolua_function(tolua_S,NULL,"message_pain",toluaI_spell_message_pain00);
 tolua_function(tolua_S,NULL,"update_smart_learn",toluaI_spell_update_smart_learn00);
 tolua_function(tolua_S,NULL,"monster_death",toluaI_spell_monster_death00);
 tolua_function(tolua_S,NULL,"mon_take_hit",toluaI_spell_mon_take_hit00);
 return 1;
}
/* Close function */
void tolua_spell_close (lua_State* tolua_S)
{
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"SUMMON_ANT");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"SUMMON_SPIDER");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"SUMMON_HOUND");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"SUMMON_HYDRA");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"SUMMON_AINU");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"SUMMON_DEMON");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"SUMMON_UNDEAD");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"SUMMON_DRAGON");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"SUMMON_HI_DEMON");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"SUMMON_HI_UNDEAD");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"SUMMON_HI_DRAGON");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"SUMMON_WRAITH");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"SUMMON_UNIQUE");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"SUMMON_KIN");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"SUMMON_ANIMAL");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"SUMMON_BERTBILLTOM");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"SUMMON_THIEF");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"SUMMON_INDEX");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_XXX1");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_ARROW");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_MISSILE");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_MANA");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_HOLY_ORB");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_LITE_WEAK");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_DARK_WEAK");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_WATER");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_PLASMA");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_METEOR");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_ICE");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_GRAVITY");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_INERTIA");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_FORCE");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_TIME");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_ACID");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_ELEC");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_FIRE");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_COLD");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_POIS");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_XXX2");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_LITE");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_DARK");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_XXX3");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_CONFUSION");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_SOUND");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_SHARD");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_NEXUS");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_NETHER");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_CHAOS");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_DISENCHANT");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_WIND");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_KILL_WALL");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_KILL_DOOR");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_KILL_TRAP");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_MAKE_WALL");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_MAKE_DOOR");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_MAKE_TRAP");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_XXX5");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_XXX6");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_AWAY_UNDEAD");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_AWAY_EVIL");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_AWAY_ALL");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_TURN_UNDEAD");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_TURN_EVIL");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_TURN_ALL");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_DISP_UNDEAD");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_DISP_EVIL");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_DISP_ALL");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_MAKE_WARY");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_OLD_CLONE");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_OLD_POLY");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_OLD_HEAL");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_OLD_SPEED");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_OLD_SLOW");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_OLD_CONF");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_OLD_SLEEP");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_OLD_DRAIN");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GF_SPORE");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"poly_r_idx");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"teleport_away");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"teleport_player");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"teleport_player_to");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"teleport_player_level");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"take_hit");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"acid_dam");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"elec_dam");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"fire_dam");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"cold_dam");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"inc_stat");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"dec_stat");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"res_stat");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"apply_disenchant");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"project");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"warding_glyph");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"make_monster_trap");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"identify_pack");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"remove_curse");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"remove_all_curse");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"self_knowledge");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"lose_all_info");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"set_recall");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"detect_traps");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"detect_doors");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"detect_stairs");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"detect_treasure");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"detect_objects_gold");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"detect_objects_normal");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"detect_objects_magic");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"detect_monsters_normal");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"detect_monsters_invis");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"detect_monsters_evil");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"detect_all");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"stair_creation");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"enchant");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"enchant_spell");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"ident_spell");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"identify_fully");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"recharge");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"speed_monsters");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"slow_monsters");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"sleep_monsters");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"banish_evil");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"turn_undead");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"dispel_undead");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"dispel_evil");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"dispel_monsters");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"aggravate_monsters");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"banishment");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"mass_banishment");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"probing");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"make_monster_trap");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"destroy_area");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"earthquake");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"lite_room");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"unlite_room");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"lite_area");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"unlite_area");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"fire_bolt_or_beam");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"fire_bolt_beam_special");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"fire_ball");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"fire_orb");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"fire_ball_special");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"fire_arc");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"fire_swarm");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"fire_bolt");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"fire_beam");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"fire_bolt_or_beam");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"project_arc");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"project_star");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"project_los");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"explosion");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"lite_line");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"strong_lite_line");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"drain_life");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"wall_to_mud");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"destroy_door");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"disarm_trap");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"heal_monster");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"speed_monster");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"slow_monster");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"sleep_monster");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"confuse_monster");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"poly_monster");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"clone_monster");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"fear_monster");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"teleport_monster");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"door_creation");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"trap_creation");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"make_monster_trap");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"destroy_doors_touch");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"sleep_monsters_touch");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"curse_armor");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"curse_weapon");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"brand_object");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"brand_weapon");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"brand_ammo");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"brand_bolts");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"ring_of_power");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"map_area");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"wiz_lite");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"wiz_dark");
 lua_getglobals(tolua_S);
 lua_pushstring(tolua_S,"mon_max"); lua_pushnil(tolua_S); lua_rawset(tolua_S,-3);
 lua_pop(tolua_S,1);
 lua_getglobals(tolua_S);
 lua_pushstring(tolua_S,"mon_cnt"); lua_pushnil(tolua_S); lua_rawset(tolua_S,-3);
 lua_pop(tolua_S,1);
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"roff_top");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"screen_roff");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"display_roff");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"delete_monster_idx");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"delete_monster");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"compact_monsters");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"wipe_mon_list");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"mon_pop");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"get_mon_num_prep");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"get_mon_num");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"monster_desc");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"lore_do_probe");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"lore_treasure");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"update_mon");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"update_monsters");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"monster_carry");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"monster_swap");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"player_place");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"monster_place");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"place_monster_aux");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"place_monster");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"alloc_monster");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"summon_specific");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"multiply_monster");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"message_pain");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"update_smart_learn");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"monster_death");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"mon_take_hit");
}
