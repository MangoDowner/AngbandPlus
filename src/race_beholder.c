#include "angband.h"

static void _birth(void) 
{ 
	object_type	forge;

	p_ptr->current_r_idx = MON_GAZER;
	
	object_prep(&forge, lookup_kind(TV_CROWN, SV_IRON_CROWN));
	forge.name2 = EGO_SEEING;
	forge.pval = 4;
	forge.to_a = 10;
	add_esp_weak(&forge, FALSE);
	add_outfit(&forge);

	object_prep(&forge, lookup_kind(TV_WAND, SV_WAND_ACID_BOLT));
	apply_magic(&forge, 1, AM_AVERAGE);
	add_outfit(&forge);

	object_prep(&forge, lookup_kind(TV_WAND, SV_WAND_FIRE_BOLT));
	apply_magic(&forge, 1, AM_AVERAGE);
	add_outfit(&forge);

	object_prep(&forge, lookup_kind(TV_WAND, SV_WAND_COLD_BOLT));
	apply_magic(&forge, 1, AM_AVERAGE);
	add_outfit(&forge);
}

/**********************************************************************
 * Equipment
 **********************************************************************/
static equip_template_t _gazer_template = 
	{2, { {EQUIP_SLOT_HELMET, "Helmet", 0},
		  {EQUIP_SLOT_LITE, "Light", 0} }
};
static equip_template_t _spectator_template = 
	{4, { {EQUIP_SLOT_HELMET, "Helmet", 0},
		  {EQUIP_SLOT_LITE, "Light", 0},
		  {EQUIP_SLOT_RING, "Eyestalk", 0},
		  {EQUIP_SLOT_RING, "Eyestalk", 0} }
};
static equip_template_t _beholder_template = 
	{8, { {EQUIP_SLOT_HELMET, "Helmet", 0},
		  {EQUIP_SLOT_LITE, "Light", 0},
		  {EQUIP_SLOT_RING, "Eyestalk", 0},
		  {EQUIP_SLOT_RING, "Eyestalk", 0},
		  {EQUIP_SLOT_RING, "Eyestalk", 0},
		  {EQUIP_SLOT_RING, "Eyestalk", 0},
		  {EQUIP_SLOT_RING, "Eyestalk", 0},
		  {EQUIP_SLOT_RING, "Eyestalk", 0} }
};
static equip_template_t _ultimate_template = 
	{10, { {EQUIP_SLOT_HELMET, "Helmet", 0},
		   {EQUIP_SLOT_LITE, "Light", 0},
		   {EQUIP_SLOT_RING, "Eyestalk", 0},
		   {EQUIP_SLOT_RING, "Eyestalk", 0},
		   {EQUIP_SLOT_RING, "Eyestalk", 0},
		   {EQUIP_SLOT_RING, "Eyestalk", 0},
		   {EQUIP_SLOT_RING, "Eyestalk", 0},
		   {EQUIP_SLOT_RING, "Eyestalk", 0},
		   {EQUIP_SLOT_RING, "Eyestalk", 0},
		   {EQUIP_SLOT_RING, "Eyestalk", 0} }
};

/**********************************************************************
 * Innate Attacks
 **********************************************************************/
static void _calc_innate_attacks(void)
{
	if (!p_ptr->blind)
	{
		innate_attack_t	a = {0};
		int l = p_ptr->lev;

		a.dd = 1 + (l + 5)/12;
		a.ds = 6 + l/15;
		a.weight = 250;
		a.to_h = p_ptr->lev/2;

		a.effect[0] = GF_MISSILE;

		a.effect[1] = GF_DRAIN_MANA;
		a.effect_chance[1] = 50+l;

		a.effect[2] = GF_OLD_CONF;
		a.effect_chance[2] = 40+l;

		if (p_ptr->lev >= 45)
		{
			a.effect[3] = GF_STASIS;
			a.effect_chance[3] = 20 + l/2;
		}
		else
		{
			a.effect[3] = GF_OLD_SLEEP;
			a.effect_chance[3] = 30+l;
		}

		if (p_ptr->lev >= 35)
		{
			a.effect[4] = GF_STUN;
			a.effect_chance[4] = 20 + l/2;
		}

		calc_innate_blows(&a, 4);
		a.msg = "You gaze at %s.";
		a.name = "Gaze";

		p_ptr->innate_attacks[p_ptr->innate_attack_ct++] = a;
	}
}

/***********************************************************************************
 *                 10           25          35                 45
 * Beholder: Gazer -> Spectator -> Beholder -> Undead Beholder -> Ultimate Beholder
 ***********************************************************************************/
static spell_info _beholder_spells[] = {
	{  1,  1, 30, paralyze_spell},
	{  1,  2, 30, confuse_spell},
	{ 10,  4, 30, cause_wounds_II_spell},
	{ 10,  5, 30, slow_spell},
	{ 10,  7, 30, amnesia_spell},
	{ 25,  7, 30, drain_mana_spell},
	{ 25,  4, 30, frost_bolt_spell},
	{ 25,  6, 30, fire_bolt_spell},
	{ 25,  7, 30, acid_bolt_spell},
	{ 25,  7, 30, scare_spell},
	{ 25, 10, 50, mind_blast_spell},
	{ -1, -1, -1, NULL}
};
static spell_info _undead_beholder_spells[] = {
	{  1,  1, 30, paralyze_spell},
	{  1,  2, 30, confuse_spell},
	{ 10,  5, 30, slow_spell},
	{ 10,  7, 30, amnesia_spell},
	{ 25,  7, 30, drain_mana_spell},
	{ 25,  7, 30, scare_spell},
	{ 25, 10, 50, mind_blast_spell},
	{ 35, 10, 30, animate_dead_spell},
	{ 35, 12, 40, cause_wounds_IV_spell},
	{ 35, 15, 50, mana_bolt_I_spell},
	{ 35, 20, 50, brain_smash_spell},
	{ 35, 30, 50, summon_undead_spell},
	{ -1, -1, -1, NULL}
};
static spell_info _ultimate_beholder_spells[] = {
	{  1,  1, 30, paralyze_spell},
	{  1,  2, 30, confuse_spell},
	{ 10,  5, 30, slow_spell},
	{ 10,  7, 30, amnesia_spell},
	{ 25,  7, 30, drain_mana_spell},
	{ 25, 10, 30, frost_ball_spell},
	{ 25, 12, 30, fire_ball_spell},
	{ 25, 14, 30, acid_ball_spell},
	{ 25,  7, 30, scare_spell},
	{ 25, 10, 50, mind_blast_spell},
	{ 35, 12, 30, cause_wounds_IV_spell},
	{ 35, 20, 50, brain_smash_spell},
	{ 45, 30, 50, mana_bolt_II_spell},
	{ 45, 40, 60, summon_kin_spell},
	{ 45, 40, 60, dispel_magic_spell},
	{ 45, 50, 80, darkness_storm_II_spell},
	{ -1, -1, -1, NULL}
};
static int _get_spells(spell_info* spells, int max) {
	if (p_ptr->blind)
	{
		msg_print("You can't see!");
		return 0;
	}

	if (p_ptr->lev >= 45)
		return get_spells_aux(spells, max, _ultimate_beholder_spells);
	else if (p_ptr->lev >= 35)
		return get_spells_aux(spells, max, _undead_beholder_spells);
	else
		return get_spells_aux(spells, max, _beholder_spells);
}
static void _calc_bonuses(void) {
	int l = p_ptr->lev;
	int ac = l/2 + l*l/100;

	p_ptr->to_a += ac;
	p_ptr->dis_to_a += ac;

	p_ptr->levitation = TRUE;
	if (p_ptr->lev >= 45)
	{
		p_ptr->telepathy = TRUE;
		res_add(RES_TELEPORT);
		res_add(RES_POIS);
		res_add(RES_CONF);
		p_ptr->free_act = TRUE;
		p_ptr->pspeed += 6;
	}
	else if (p_ptr->lev >= 35)
	{
		p_ptr->telepathy = TRUE;
		res_add(RES_TELEPORT);
		res_add(RES_ACID);
		res_add(RES_FIRE);
		res_add(RES_COLD);
		res_add(RES_ELEC);
		res_add(RES_POIS);
		res_add(RES_NETHER);
		res_add(RES_CONF);
		p_ptr->free_act = TRUE;
		p_ptr->pspeed += 4;
		p_ptr->hold_life = TRUE;
	}
	else if (p_ptr->lev >= 25)
	{
		res_add(RES_TELEPORT);
		res_add(RES_POIS);
		res_add(RES_CONF);
		p_ptr->free_act = TRUE;
		p_ptr->pspeed += 2;
	}
	else if (p_ptr->lev >= 10)
	{
		res_add(RES_FEAR);
		res_add(RES_CONF);
		p_ptr->free_act = TRUE;
	}
	else
	{
		res_add(RES_POIS);
	}
}
static void _get_flags(u32b flgs[TR_FLAG_SIZE]) {
	add_flag(flgs, TR_LEVITATION);
	if (p_ptr->lev >= 45)
	{
		add_flag(flgs, TR_TELEPATHY);
		add_flag(flgs, TR_RES_POIS);
		add_flag(flgs, TR_RES_CONF);
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_SPEED);
	}
	else if (p_ptr->lev >= 35)
	{
		add_flag(flgs, TR_TELEPATHY);
		add_flag(flgs, TR_RES_ACID);
		add_flag(flgs, TR_RES_COLD);
		add_flag(flgs, TR_RES_FIRE);
		add_flag(flgs, TR_RES_ELEC);
		add_flag(flgs, TR_RES_POIS);
		add_flag(flgs, TR_RES_CONF);
		add_flag(flgs, TR_RES_NETHER);
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_SPEED);
		add_flag(flgs, TR_HOLD_LIFE);
	}
	else if (p_ptr->lev >= 25)
	{
		add_flag(flgs, TR_RES_POIS);
		add_flag(flgs, TR_RES_CONF);
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_SPEED);
	}
	else if (p_ptr->lev >= 10)
	{
		add_flag(flgs, TR_RES_FEAR);
		add_flag(flgs, TR_RES_CONF);
		add_flag(flgs, TR_FREE_ACT);
	}
	else
	{
		add_flag(flgs, TR_RES_POIS);
	}
}
static void _gain_level(int new_level) {
	if (p_ptr->current_r_idx == MON_GAZER && new_level >= 10)
	{
		p_ptr->current_r_idx = MON_SPECTATOR;
		msg_print("You have evolved into a Spectator.");
		equip_on_change_race();
		p_ptr->redraw |= PR_MAP | PR_BASIC;
	}
	if (p_ptr->current_r_idx == MON_SPECTATOR && new_level >= 25)
	{
		p_ptr->current_r_idx = MON_BEHOLDER;
		msg_print("You have evolved into a Beholder.");
		equip_on_change_race();
		p_ptr->redraw |= PR_MAP | PR_BASIC;
	}
	if (p_ptr->current_r_idx == MON_BEHOLDER && new_level >= 35)
	{
		p_ptr->current_r_idx = MON_UNDEAD_BEHOLDER;
		msg_print("You have evolved into an Undead Beholder.");
		equip_on_change_race();
		p_ptr->redraw |= PR_MAP | PR_BASIC;
	}
	if (p_ptr->current_r_idx == MON_UNDEAD_BEHOLDER && new_level >= 45)
	{
		p_ptr->current_r_idx = MON_ULTIMATE_BEHOLDER;
		p_ptr->psex = SEX_FEMALE;
		sp_ptr = &sex_info[p_ptr->psex];
		msg_print("You have evolved into an Ultimate Beholder.");
		equip_on_change_race();
		p_ptr->redraw |= PR_MAP | PR_BASIC;
	}
}
static caster_info * _caster_info(void)
{
	static caster_info me = {0};
	static bool init = FALSE;
	if (!init)
	{
		me.magic_desc = "power";
		me.which_stat = A_INT;
		me.weight = 450;
		me.options = CASTER_ALLOW_DEC_MANA;
		init = TRUE;
	}
	return &me;
}

/**********************************************************************
 * Public
 **********************************************************************/
race_t *mon_beholder_get_race_t(void)
{
	static race_t me = {0};
	static bool   init = FALSE;
	static cptr   titles[5] =  {"Gazer", "Spectator", "Beholder", "Undead Beholder", "Ultimate Beholder"};	
	int           rank = 0;

	if (p_ptr->lev >= 10) rank++;
	if (p_ptr->lev >= 25) rank++;
	if (p_ptr->lev >= 35) rank++;
	if (p_ptr->lev >= 45) rank++;

	if (!init)
	{
		me.name = "Beholder";
		me.desc = "Beholders are floating orbs of flesh with a single central eye surrounded by "
					"numerous smaller eyestalks. They attack with their gaze which often confuses or "
					"even paralyzes their foes. They are unable to wield normal weapons or armor, but "
					"may equip a single ring on each of their eyestalks, and the number of eyestalks "
					"increases as the beholder evolves.\n \n"
					"Beholders are monsters so cannot choose a normal class. Instead, they gain powers "
					"as they evolve, including some limited offensive capabilities. But the beholders "
					"primary offense will always be their powerful gaze. Intelligence is the primary "
					"spell stat and beholders are quite intelligent indeed. Their searching and "
					"perception are legendary and they are quite capable with magical devices. "
					"However, they are not very strong and won't be able to stand long against "
					"multiple foes.";

		me.skills.dis = 20;
		me.skills.thb =  0;

		me.infra = 8;
		me.exp = 250;

		me.birth = _birth;
		me.calc_innate_attacks = _calc_innate_attacks;
		me.get_spells = _get_spells;
		me.caster_info = _caster_info;
		me.calc_bonuses = _calc_bonuses;
		me.get_flags = _get_flags;
		me.gain_level = _gain_level;

		me.flags = RACE_IS_MONSTER;
		me.boss_r_idx = MON_OMARAX;
		init = TRUE;
	}

	me.subname = titles[rank];
	me.stats[A_STR] = -3;
	me.stats[A_INT] =  4 + rank;
	me.stats[A_WIS] =  0;
	me.stats[A_DEX] =  1 + rank/2;
	me.stats[A_CON] =  0;
	me.stats[A_CHR] =  0 + rank/2;
	me.skills.srh = 20 + 20*rank;
	me.skills.fos = 20 + 20*rank;
	me.skills.dev = 30 + 20*rank;
	me.skills.sav = 15 + 10*rank;
	me.skills.thn = -20;
	me.skills.stl =  5 + rank;
	me.life = 100;

	if (p_ptr->lev >= 45)
		me.equip_template = &_ultimate_template;
	else if (p_ptr->lev >= 25)
		me.equip_template = &_beholder_template;
	else if (p_ptr->lev >= 10)
		me.equip_template = &_spectator_template;
	else
		me.equip_template = &_gazer_template;

	return &me;
}
