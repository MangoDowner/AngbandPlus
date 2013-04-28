#include "angband.h"

static cptr _mon_name(int r_idx)
{
	if (r_idx)
		return r_name + r_info[r_idx].name;
	return ""; /* Birth Menu */
}

/******************************************************************************
 * Troll Equipment (Ettins have two heads)
 ******************************************************************************/
static equip_template_t _ettin_template = 
	{14, { {EQUIP_SLOT_WEAPON_SHIELD, "Right Hand", 0},
		   {EQUIP_SLOT_WEAPON_SHIELD, "Left Hand", 1},
		   {EQUIP_SLOT_BOW, "Shooting", 0},
		   {EQUIP_SLOT_RING, "Right Ring", 0},
		   {EQUIP_SLOT_RING, "Left Ring", 1},
		   {EQUIP_SLOT_AMULET, "Right Neck", 0},
		   {EQUIP_SLOT_AMULET, "Left Neck", 0},
		   {EQUIP_SLOT_LITE, "Light", 0},
		   {EQUIP_SLOT_BODY_ARMOR, "Body", 0},
		   {EQUIP_SLOT_CLOAK, "Cloak", 0},
		   {EQUIP_SLOT_HELMET, "Right Head", 0},
		   {EQUIP_SLOT_HELMET, "Left Head", 0},
		   {EQUIP_SLOT_GLOVES, "Hands", 0},
		   {EQUIP_SLOT_BOOTS, "Feet", 0} }
};

/******************************************************************************
 * Troll Bite
 ******************************************************************************/
static void _calc_innate_attacks(void) 
{
	innate_attack_t	a = {0};

	a.dd = 1 + p_ptr->lev / 13;
	a.ds = 5 + p_ptr->lev / 10;
	a.weight = 250 + p_ptr->lev * 2;
	a.to_h = p_ptr->lev/2;

	switch (p_ptr->current_r_idx)
	{
	case MON_ICE_TROLL:
		a.effect[0] = GF_COLD;
		break;
	case MON_FIRE_TROLL:
		a.effect[0] = GF_FIRE;
		break;
	case MON_ALGROTH:
	case MON_AKLASH:
		a.effect[0] = GF_POIS;
		break;
	case MON_STORM_TROLL:
		a.effect[0] = GF_MISSILE;
		a.effect[1] = GF_ELEC;
		break;
	default:
		a.effect[0] = GF_MISSILE;
	}
	a.blows = 1;

	a.msg = "You bite %s.";
	a.name = "Bite";

	p_ptr->innate_attacks[p_ptr->innate_attack_ct++] = a;
}

/******************************************************************************
 * Troll Powers
 ******************************************************************************/
static void _aklash_breathe_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Breathe Gas");
		break;
	case SPELL_DESC:
		var_set_string(res, "Breathes poison at your opponent.");
		break;
	case SPELL_INFO:
		var_set_string(res, info_damage(0, 0, p_ptr->chp / 4));
		break;
	case SPELL_CAST:
	{
		int dir = 0;
		var_set_bool(res, FALSE);
		if (get_aim_dir(&dir))
		{
			msg_print("You breathe gas.");
			fire_ball(GF_POIS, dir, p_ptr->chp / 4, -2);
			var_set_bool(res, TRUE);
		}
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}


static power_info _aklash_powers[] = 
{
	{ A_CON, {  25, 10, 30, _aklash_breathe_spell} },
	{    -1, { -1, -1, -1, NULL}}
};

static power_info _storm_troll_powers[] = 
{
	{ A_STR, {  40, 10, 30, lightning_ball_spell} },
	{ A_STR, {  40, 10, 35, frost_ball_spell} },
	{ A_STR, {  40, 15, 50, ice_bolt_spell} },
	{ A_STR, {  40, 20, 60, water_bolt_spell} },
	{    -1, { -1, -1, -1, NULL}}
};

static power_info _troll_king_powers[] = 
{
	{ A_DEX, {  40, 2, 30, phase_door_spell} },
	{    -1, { -1, -1, -1, NULL}}
};

static int _get_powers(spell_info* spells, int max) 
{
	switch (p_ptr->current_r_idx)
	{
	case MON_AKLASH: return get_powers_aux(spells, max, _aklash_powers);
	case MON_STORM_TROLL: return get_powers_aux(spells, max, _storm_troll_powers);
	case MON_TROLL_KING: return get_powers_aux(spells, max, _troll_king_powers);
	}

	return 0;
}

/******************************************************************************
 * Troll Evolution
 ******************************************************************************/
static void _birth(void) 
{ 
	object_type	forge;

	p_ptr->current_r_idx = MON_FOREST_TROLL;
	
	object_prep(&forge, lookup_kind(TV_HARD_ARMOR, SV_CHAIN_MAIL));
	add_outfit(&forge);

	object_prep(&forge, lookup_kind(TV_RING, SV_RING_DAMAGE));
	forge.to_d = 7;
	add_outfit(&forge);

	object_prep(&forge, lookup_kind(TV_HAFTED, SV_CLUB));
	forge.weight = 100;
	forge.dd = 3;
	forge.ds = 4;
	forge.to_d = 7;
	add_outfit(&forge);
}

static void _gain_level(int new_level) 
{
	if (p_ptr->current_r_idx == MON_FOREST_TROLL && new_level >= 10)
	{
		p_ptr->current_r_idx = MON_STONE_TROLL;
		msg_print("You have evolved into a Stone Troll.");
		p_ptr->redraw |= PR_MAP;
	}
	if (p_ptr->current_r_idx == MON_STONE_TROLL && new_level >= 20)
	{
		switch (randint0(3))
		{
		case 0:
			p_ptr->current_r_idx = MON_ICE_TROLL;
			msg_print("You have evolved into an Ice Troll.");
			break;
		case 1:
			p_ptr->current_r_idx = MON_FIRE_TROLL;
			msg_print("You have evolved into a Fire Troll.");
			break;
		case 2:
			p_ptr->current_r_idx = MON_ALGROTH;
			msg_print("You have evolved into an Algroth.");
			break;
		}
		p_ptr->redraw |= PR_MAP;
	}
	if (p_ptr->current_r_idx == MON_ALGROTH && new_level >= 25)
	{
		p_ptr->current_r_idx = MON_AKLASH;
		msg_print("You have evolved into an Aklash.");
		p_ptr->redraw |= PR_MAP;
	}
	if ( ( p_ptr->current_r_idx == MON_FIRE_TROLL 
	    || p_ptr->current_r_idx == MON_ICE_TROLL 
		|| p_ptr->current_r_idx == MON_AKLASH )
	  && new_level >= 30 )
	{
		p_ptr->current_r_idx = MON_OLOG;
		msg_print("You have evolved into an Olog.");
		p_ptr->redraw |= PR_MAP;
	}
	if (p_ptr->current_r_idx == MON_OLOG && new_level >= 40)
	{
		switch (p_ptr->psubrace)
		{
		case TROLL_ETTIN:
			p_ptr->current_r_idx = MON_ETTIN;
			msg_print("You have evolved into an Ettin.");
			equip_on_change_race(); /* Two heads */
			break;
		case TROLL_STORM:
			p_ptr->current_r_idx = MON_STORM_TROLL;
			msg_print("You have evolved into a Storm Troll.");
			break;
		case TROLL_SPIRIT:
			p_ptr->current_r_idx = MON_SPIRIT_TROLL;
			msg_print("You have evolved into a Spirit Troll.");
			break;
		case TROLL_KING:
			p_ptr->current_r_idx = MON_TROLL_KING;
			msg_print("You have evolved into a Troll King.");
			break;
		}
		p_ptr->redraw |= PR_MAP;
	}
}

/******************************************************************************
 * Troll Bonuses
 ******************************************************************************/
static void _calc_bonuses(void) 
{
	int l = p_ptr->lev;
	int to_a = l/10 + l*l/250 + l*l*l/12500;

	p_ptr->to_a += to_a;
	p_ptr->dis_to_a += to_a;

	p_ptr->regenerate = TRUE; /* cf process_world_aux_hp_and_sp() in dungeon.c for the troll's special regeneration */
	switch (p_ptr->current_r_idx)
	{
	case MON_FOREST_TROLL:
		res_add_vuln(RES_LITE);
		break;
	case MON_STONE_TROLL:
		res_add_vuln(RES_LITE);
		break;
	case MON_ICE_TROLL:
		res_add_vuln(RES_LITE);
		res_add_vuln(RES_FIRE);
		res_add(RES_COLD);
		res_add(RES_COLD);
		break;
	case MON_FIRE_TROLL:
		res_add_vuln(RES_LITE);
		res_add_vuln(RES_COLD);
		res_add(RES_FIRE);
		res_add(RES_FIRE);
		break;
	case MON_ALGROTH:
		break;
	case MON_AKLASH:
		res_add(RES_POIS);
		res_add(RES_ACID);
		break;
	case MON_OLOG:
		res_add(RES_POIS);
		break;
	case MON_ETTIN:
		p_ptr->free_act = TRUE;
		res_add(RES_POIS);
		res_add(RES_CONF);
		break;
	case MON_SPIRIT_TROLL:
		p_ptr->pass_wall = TRUE;
		p_ptr->no_passwall_dam = TRUE;
		p_ptr->free_act = TRUE;
		p_ptr->see_inv = TRUE;
		p_ptr->levitation = TRUE;
		res_add(RES_COLD);
		res_add(RES_ELEC);
		res_add(RES_POIS);
		res_add(RES_CONF);
		break;
	case MON_STORM_TROLL:
		p_ptr->pspeed += 5;
		res_add(RES_COLD);
		res_add(RES_ELEC);
		res_add(RES_ACID);
		break;
	case MON_TROLL_KING:
		p_ptr->pspeed += 7;
		p_ptr->free_act = TRUE;
		res_add(RES_POIS);
		res_add(RES_CONF);
		break;
	}
}

static void _get_flags(u32b flgs[TR_FLAG_SIZE]) 
{
	add_flag(flgs, TR_REGEN);
	switch (p_ptr->current_r_idx)
	{
	case MON_FOREST_TROLL:
		break;
	case MON_STONE_TROLL:
		break;
	case MON_ICE_TROLL:
		add_flag(flgs, TR_RES_COLD);
		add_flag(flgs, TR_BRAND_COLD);
		break;
	case MON_FIRE_TROLL:
		add_flag(flgs, TR_RES_FIRE);
		add_flag(flgs, TR_BRAND_FIRE);
		break;
	case MON_ALGROTH:
		add_flag(flgs, TR_BRAND_POIS);
		break;
	case MON_AKLASH:
		add_flag(flgs, TR_RES_POIS);
		add_flag(flgs, TR_RES_ACID);
		break;
	case MON_OLOG:
		add_flag(flgs, TR_RES_POIS);
		break;
	case MON_ETTIN:
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_RES_POIS);
		add_flag(flgs, TR_RES_CONF);
		break;
	case MON_SPIRIT_TROLL:
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_SEE_INVIS);
		add_flag(flgs, TR_LEVITATION);
		add_flag(flgs, TR_RES_COLD);
		add_flag(flgs, TR_RES_ELEC);
		add_flag(flgs, TR_RES_POIS);
		add_flag(flgs, TR_RES_CONF);
		break;
	case MON_STORM_TROLL:
		add_flag(flgs, TR_SPEED);
		add_flag(flgs, TR_RES_COLD);
		add_flag(flgs, TR_RES_ELEC);
		add_flag(flgs, TR_RES_ACID);
		add_flag(flgs, TR_BRAND_ELEC);
		break;
	case MON_TROLL_KING:
		add_flag(flgs, TR_SPEED);
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_RES_POIS);
		add_flag(flgs, TR_RES_CONF);
		break;
	}
}

static void _get_vulnerabilities(u32b flgs[TR_FLAG_SIZE]) 
{
	switch (p_ptr->current_r_idx)
	{
	case MON_FOREST_TROLL:
		add_flag(flgs, TR_RES_LITE);
		break;
	case MON_STONE_TROLL:
		add_flag(flgs, TR_RES_LITE);
		break;
	case MON_ICE_TROLL:
		add_flag(flgs, TR_RES_LITE);
		add_flag(flgs, TR_RES_FIRE);
		break;
	case MON_FIRE_TROLL:
		add_flag(flgs, TR_RES_LITE);
		add_flag(flgs, TR_RES_COLD);
		break;
	}	
}

static void _calc_weapon_bonuses(object_type *o_ptr, weapon_info_t *info_ptr)
{
	switch (p_ptr->current_r_idx)
	{
	case MON_ICE_TROLL:
		add_flag(info_ptr->flags, TR_BRAND_COLD);
		break;
	case MON_FIRE_TROLL:
		add_flag(info_ptr->flags, TR_BRAND_FIRE);
		break;
	case MON_ALGROTH:
		add_flag(info_ptr->flags, TR_BRAND_POIS);
		break;
	case MON_STORM_TROLL:
		add_flag(info_ptr->flags, TR_BRAND_ELEC);
		break;
	case MON_TROLL_KING:
		info_ptr->to_d += 10;
		info_ptr->dis_to_d += 10;
		break;
	}
}

/******************************************************************************
 * Troll Public API
 ******************************************************************************/
race_t *mon_troll_get_race_t(void)
{
	static race_t me = {0};
	static bool   init = FALSE;

	if (!init)
	{           /* dis, dev, sav, stl, srh, fos, thn, thb */
	skills_t bs = { 25,  18,  30,  -1,  13,   7,  65,  30 };
	skills_t xs = {  7,   7,  10,   0,   0,   0,  28,  10 };

		me.skills = bs;
		me.extra_skills = xs;

		me.name = "Troll";
		me.desc = 	
			"Trolls are disgusting creatures: Big, strong and stupid. They make excellent warriors "
			"but are hopeless with magical devices. Trolls have incredible powers of regeneration.";

		me.infra = 5;
		me.exp = 150;

		me.calc_bonuses = _calc_bonuses;
		me.calc_weapon_bonuses = _calc_weapon_bonuses;
		me.calc_innate_attacks = _calc_innate_attacks;
		me.get_powers = _get_powers;
		me.get_flags = _get_flags;
		me.get_vulnerabilities = _get_vulnerabilities;
		me.gain_level = _gain_level;
		me.birth = _birth;

		me.flags = RACE_IS_MONSTER;
		me.boss_r_idx = MON_ULIK;

		init = TRUE;
	}

	me.subname = _mon_name(p_ptr->current_r_idx);
	me.stats[A_STR] =  3 + p_ptr->lev/12;
	me.stats[A_INT] = -5;
	me.stats[A_WIS] = -5;
	me.stats[A_DEX] = -2 + p_ptr->lev/20;
	me.stats[A_CON] =  3 + p_ptr->lev/13;
	me.stats[A_CHR] =  0;
	me.life = 100 + (p_ptr->lev/10)*4;

	if (p_ptr->current_r_idx == MON_ETTIN)
		me.equip_template = &_ettin_template;
	else
		me.equip_template = NULL;

	return &me;
}
