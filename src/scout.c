#include "angband.h"

static void _cavern_creation_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Cavern Creation");
		break;
	case SPELL_DESC:
		var_set_string(res, "Stone to Mud all surrounding walls.");
		break;
	case SPELL_CAST:
	{
		int dir, x, y, ct = 0;
		int count = 0;
		for (dir = 0; dir < 8; dir++)
		{
			y = py + ddy_ddd[dir];
			x = px + ddx_ddd[dir];
		
			if (!in_bounds(y, x)) continue;
			if (!cave_have_flag_bold(y, x, FF_HURT_ROCK))  continue;
			cave_alter_feat(y, x, FF_HURT_ROCK);
			ct++;
		}
		if (ct) 
			p_ptr->update |= (PU_FLOW | PU_BONUS);

		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

static void _dark_stalker_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Dark Stalker");
		break;
	case SPELL_DESC:
		var_set_string(res, "Temporarily grants enhanced stealth.");
		break;
	case SPELL_CAST:
		set_tim_dark_stalker(50 + randint1(50), FALSE);
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

static void _greater_whirlwind_attack_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Greater Whirlwind Attack");
		break;
	case SPELL_DESC:
		var_set_string(res, "Perform a massive ambush on nearby monsters.");
		break;
	case SPELL_CAST:
	{
		int              i, x, y;
		cave_type       *c_ptr;
		monster_type    *m_ptr;

/*		 cba
		d218l
		e3@7k
		f456j
		 ghi  */

		typedef struct _offset_t { int dx; int dy; } _offset;
		static _offset offsets[] = {
			{ 0, -1},
			{-1, -1},
			{-1,  0},
			{-1,  1},
			{ 0,  1},
			{ 1,  1},
			{ 1,  0},
			{ 1, -1},
			{ 1, -2},
			{ 0, -2},
			{-1, -2},
			{-2, -1},
			{-2,  0},
			{-2,  1},
			{-1,  2},
			{ 0,  2},
			{ 1,  2},
			{ 2,  1},
			{ 2,  0},
			{ 2, -1},
			{ 0,  0}, /* sentinel */
		};

		for (i = 0;; i++)
		{
			_offset offset = offsets[i];
			if (offset.dx == 0 && offset.dy == 0) break;

			y = py + offset.dy;
			x = px + offset.dx;

			if (!in_bounds(y, x)) continue;
			if (!projectable(py, px, y, x)) continue;
			
			c_ptr = &cave[y][x];

			if (!c_ptr->m_idx) continue;
			
			m_ptr = &m_list[c_ptr->m_idx];

			if (m_ptr->ml || cave_have_flag_bold(y, x, FF_PROJECT))
			{
				int msec = delay_factor * delay_factor * delay_factor;

				if (panel_contains(y, x) && player_can_see_bold(y, x))
				{
					char c = object_char(&inventory[INVEN_RARM]);
					byte a = object_attr(&inventory[INVEN_RARM]);

					print_rel(c, a, y, x);
					move_cursor_relative(y, x);
					Term_fresh();
					Term_xtra(TERM_XTRA_DELAY, msec);
					lite_spot(y, x);
					Term_fresh();
				}
				else
					Term_xtra(TERM_XTRA_DELAY, msec);

				py_attack(y, x, 0);
			}
		}
		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}


static void _nimble_dodge_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Nimble Dodge");
		break;
	case SPELL_DESC:
		var_set_string(res, "For a short time, you will have a chance of dodging enemy breath attacks.");
		break;
	case SPELL_CAST:
		set_tim_nimble_dodge(20 + randint1(20), FALSE);
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

static void _stealthy_snipe_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Stealthy Snipe");
		break;
	case SPELL_DESC:
		var_set_string(res, "For a short while, your missile attacks will not anger distant monsters.");
		break;
	case SPELL_CAST:
		set_tim_stealthy_snipe(1 + randint1(6), FALSE);
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

static void _whirlwind_attack_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Whirlwind Attack");
		break;
	case SPELL_DESC:
		var_set_string(res, "Attack all adjacent monsters in a single ambush.");
		break;
	default:
		massacre_spell(cmd, res);
		break;
	}
}

/****************************************************************
 * Spell Table and Exports
 ****************************************************************/

static spell_info _spells[] = 
{
    /*lvl cst fail spell */
    {  5,  2, 30, strafing_spell},
	{  9, 10, 50, detection_spell},
	{ 13,  8, 40, stone_to_mud_spell},
	{ 17, 20, 50, magic_mapping_spell},
	{ 21, 30, 60, _dark_stalker_spell},
	{ 25, 18, 50, _whirlwind_attack_spell},
	{ 29, 25, 60, teleport_spell},
	{ 33, 40, 55, _nimble_dodge_spell},
	{ 37, 24, 45, _cavern_creation_spell},
	{ 41, 70, 50, _stealthy_snipe_spell},
	{ 45, 60, 70, clairvoyance_spell},
	{ 49, 42, 65, _greater_whirlwind_attack_spell},
	{ -1, -1, -1, NULL}
};

static int _get_spells(spell_info* spells, int max)
{
	int ct = 0;

	if (heavy_armor())
	{
		msg_print("Your talents are disrupted!");
		return 0;
	}	
	ct = get_spells_aux(spells, max, _spells, p_ptr->stat_ind[A_WIS]);
	if (ct == 0)
		msg_print("You have no powers yet! Why not go kill stuff?");

	return ct;
}

static bool _cave_is_open(int y, int x)
{
	if (cave_have_flag_bold(y, x, FF_HURT_ROCK)) return FALSE;
	if (cave[y][x].feat == feat_permanent) return FALSE;
	if (cave[y][x].feat == feat_permanent_glass_wall) return FALSE;
	if (cave[y][x].feat == feat_mountain) return FALSE;
	return TRUE;
}

static int _count_open_terrain(void)
{
	int dir, x, y;
	int count = 0;
	for (dir = 0; dir < 8; dir++)
	{
		y = py + ddy_ddd[dir];
		x = px + ddx_ddd[dir];
		
		if (!in_bounds(y, x))
		{
			/* Count the edge of wilderness maps as open.
			   Count the edge of dungeon maps as permanent walls. */
			if (dun_level == 0)
				count++;

			continue;
		}

		if (_cave_is_open(y, x))
			count++;
	}
	return count;
}

static void _calc_bonuses(void)
{
	int ct = _count_open_terrain();
	bool disrupt = heavy_armor();

	/* Hack: Heavy Armor negates advantages of being in the open, and
	   actually incurs penalties for being entrenched! */
	if (disrupt)
		ct = 0;

	p_ptr->open_terrain_ct = ct; /* Nimble Dodge needs this information! */

	/* Unfettered Body */
	if (p_ptr->lev >= 1)
	{
		int amt = (ct + 1) * (ct + 1) - 41;
		p_ptr->to_a += amt;
		p_ptr->dis_to_a += amt;
	}

	/* Unfettered Mind */
	if (p_ptr->lev >= 1)
	{
		int amt = (ct + 1) * (ct + 1) / 2 - 20;
		p_ptr->skill_sav += amt;
	}

	if (!disrupt && p_ptr->lev >= 20)
		p_ptr->ambush = TRUE;

	if (!disrupt && p_ptr->lev >= 35)
		p_ptr->telepathy = TRUE;

	if (!disrupt && p_ptr->lev >= 50)
		p_ptr->peerless_stealth = TRUE;

	if (p_ptr->tim_dark_stalker)
		p_ptr->skill_stl += 2 + p_ptr->lev/10;
}

static caster_info * _caster_info(void)
{
	static caster_info me = {0};
	static bool init = FALSE;
	if (!init)
	{
		me.magic_desc = "technique";
		me.use_sp = TRUE;
		init = TRUE;
	}
	return &me;
}

static void _move_player(void)
{
	p_ptr->update |= PU_BONUS;
}

static void _spoiler_dump(FILE* fff)
{
	spoil_spells_aux(fff, _spells);

	fprintf(fff, "\n== Abilities ==\n");
	fprintf(fff, "|| *Lvl* || *Ability* || *Description* ||\n");
	fprintf(fff, "|| 1 || Unfettered Body || `+((X+1)*(X+1)-41) AC` || \n");
	fprintf(fff, "|| 15 || Unfettered Mind || `+((X+1)*(X+1)/2-20) saving throws` ||\n");
	fprintf(fff, "|| 30 || Ambush || First strike against sleeping opponent does triple damage if it hits. ||\n");
	fprintf(fff, "|| 35 || Telepathy ||  ||\n");
	fprintf(fff, "|| 50 || Peerless Stealth || The Scout never aggravates, even if his equipment normally would cause him to aggravate (Stealth is instead halved, just like a Shadow Fairy). Even Sexy personality characters can have their aggravation canceled in this way. ||\n");
	fprintf(fff, "\n_Where X is the number of open squares adjacent to the player._\n");
}

class_t *scout_get_class_t(void)
{
	static class_t me = {0};
	static bool init = FALSE;

	/* static info never changes */
	if (!init)
	{           /* dis, dev, sav, stl, srh, fos, thn, thb */
	skills_t bs = { 30,  33,  34,   6,  50,  24,  50,  50 };
	skills_t xs = { 15,  11,  10,   0,   0,   0,  20,  20 };

		me.name = "Scout";
		me.desc = "The scout is the vanguard of any attack, and excels at stealth and observation "
					"skills. The scout is not the best at one-on-one combat, but is unparalleled at "
					"ambush techniques to destroy groups of weak sentries. The scout is lightly "
					"armored, and heavy armors disrupt their abilities. Furthermore, the scout "
					"can only effectively dodge in open areas, being confined severely hampers "
					"the scout's defensive abilities.";
		me.stats[A_STR] =  1;
		me.stats[A_INT] = -1;
		me.stats[A_WIS] =  2;
		me.stats[A_DEX] =  3;
		me.stats[A_CON] =  0;
		me.stats[A_CHR] =  0;
		me.base_skills = bs;
		me.extra_skills = xs;
		me.calc_bonuses = _calc_bonuses;
		me.caster_info = _caster_info;
		me.get_spells = _get_spells;
		me.move_player = _move_player;
		me.spoiler_dump = _spoiler_dump;
		init = TRUE;
	}

	return &me;
}
