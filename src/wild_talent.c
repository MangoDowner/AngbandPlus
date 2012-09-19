#include "angband.h"

void detect_evil_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Detect Evil", "�ٰ�¸�ߴ���"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Detects all evil monsters in your vicinity.", "�᤯�μٰ��ʥ�󥹥������Τ��롣"));
		break;
	case SPELL_CAST:
		detect_monsters_evil(DETECT_RAD_DEFAULT);
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void burning_strike_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Burning Strike");
		break;
	case SPELL_DESC:
		var_set_string(res, "Attacks a monster with more damage unless it has resistance to fire.");
		break;
	case SPELL_CAST:
	{
		int y, x, dir = 0;
		var_set_bool(res, FALSE);
		if (!get_rep_dir2(&dir)) return;
		if (dir == 5) return;

		y = py + ddy[dir];
		x = px + ddx[dir];

		if (cave[y][x].m_idx)
		{
			py_attack(y, x, HISSATSU_FIRE);
			var_set_bool(res, TRUE);
		}
		else
		{
			msg_print(T("There is no monster.", "���������ˤϥ�󥹥����Ϥ��ޤ���"));
		}
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void lightning_eagle_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Lightning Eagle", "������޻�"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Attacks a monster with more damage unless it has resistance to electricity.", "�ŷ������Τʤ���󥹥����������礭�����᡼����Ϳ���롣"));
		break;
	case SPELL_CAST:
	{
		int y, x, dir = 0;
		var_set_bool(res, FALSE);
		if (!get_rep_dir2(&dir)) return;
		if (dir == 5) return;

		y = py + ddy[dir];
		x = px + ddx[dir];

		if (cave[y][x].m_idx)
		{
			py_attack(y, x, HISSATSU_ELEC);
			var_set_bool(res, TRUE);
		}
		else
		{
			msg_print(T("There is no monster.", "���������ˤϥ�󥹥����Ϥ��ޤ���"));
		}
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

/*
 * We are using p_ptr->magic_num to remember talents.
 * 0 indicates no talent for this group, so we always subtract
 * 1 before indexing into an array.
 */

typedef struct {
int			stat;
cptr		gain_desc;
spell_info	spell;
} talent_t;

/*
 * Talents are grouped.  Each group of talent will contain abilities
 * of a similar kind, such as offense, defense, detection.
 */
#define _MAX_TALENTS 25
#define _MAX_TALENTS_PER_GROUP 10

static talent_t _talents[_MAX_TALENTS][_MAX_TALENTS_PER_GROUP] = 
{
	/* CL1: Weak offense */
	{
		{ A_INT, "like a Dark Elf", {1, 1, 30, magic_missile_spell}},
		{ A_STR, "like an Android", {1, 1, 30, android_ray_gun_spell}},
		{ A_CON, "like a Vampire", {1, 1, 70, vampirism_spell}},
		{ A_DEX, "like a Novice Ranger", {1, 1, 30, shoot_arrow_spell}},
		{ A_WIS, "like a Novice Paladin", {1, 1, 30, cause_wounds_I_spell}},
		{ -1, NULL, {0, 0, 0, NULL}},
	},
	/* CL3: Weak utility */
	{
		{ A_WIS, "like a Yeek", {3, 5, 30, scare_monster_spell}},
		{ A_WIS, "like a Half Orc", {3, 5, 50, remove_fear_spell}},
		{ A_CHR, "like a Warlock", {3, 5, 60, satisfy_hunger_spell}},
		{ A_CHR, "like a Warlock", {3, 5, 40, light_area_spell}},
		{ A_CHR, "like a Mutant", {3, 12, 40, hypnotic_gaze_spell}},		
		{ A_WIS, "like a Priest", {3, 5, 40, bless_spell}},
		{ -1, NULL, {0, 0, 0, NULL}},
	},
	/* CL5: Middle Utility */
	{
		{ A_INT, "like a Gnome", {3, 5, 50, phase_door_spell}},
		{ A_DEX, "like an Archer", {1, 0, 0, create_ammo_spell}},
		{ A_WIS, "like a Dwarf", {5, 5, 50, detect_doors_stairs_traps_spell}},
		{ A_INT, "like a Mutant", {3, 2, 30, smell_metal_spell}},
		{ A_CHR, "like a Warlock", {5, 5, 50, detect_objects_spell}},
		{ -1, NULL, {0, 0, 0, NULL}},
	},

	/* CL7: Middle Utility */
	{
		{ A_CHR, "like a Warlock", {5, 5, 40, detect_monsters_spell}},
		{ A_STR, "like a Berserker", {7, 5, 40, detect_menace_spell}},
		{ A_INT, "like a Trump Mage", {7, 12, 50, telepathy_spell}},
		{ A_WIS, "like a Priest", {7, 5, 40, detect_evil_spell}},
		{ -1, NULL, {0, 0, 0, NULL}},
	},
	/* CL9: Middle Offense */
	{
		{ A_CHR, "like a Beastmaster", {1, 0, 30, dominate_living_I_spell}},
		{ A_WIS, "like an Imp", {9, 7, 50, imp_fire_spell}},
		{ A_STR, "like an Android", {9, 7, 30, android_blaster_spell}},
		{ A_DEX, "like a Kobold", {9, 8, 50, poison_dart_spell}},
		{ A_WIS, "like a Moon Beast", {9, 7, 40, cause_wounds_II_spell}},
		{ -1, NULL, {0, 0, 0, NULL}},
	},
	/* CL11: Middle Offense */
	{
		{ A_CON, "like a Mutant", {11, 15, 30, radiation_spell}},
		{ A_CON, "like a Mutant", {11, 14, 40, shriek_spell}},
		{ A_WIS, "like a Mutant", {7, 10, 50, laser_eye_spell}},
		{ A_CON, "like a Mutant", {2, 2, 30, cold_touch_spell}},
		{ A_STR, "like a Mutant", {1, 0, 40, power_throw_spell}},
		{ -1, NULL, {0, 0, 0, NULL}},
	},
	/* CL13: Good Utility */
	{
		{ A_DEX, "like a Berserker", {10, 10, 50, recall_spell}},
		{ A_INT, "like a Tengu", {13, 10, 50, teleport_to_spell}},
		{ A_CHR, "like an Ent", {10, 20, 50, summon_tree_spell}},
		{ A_INT, "like a Sprite", {7, 12, 50, sleeping_dust_spell}},
		{ A_CON, "like a Mutant", {7, 12, 40, eat_rock_spell}},
		{ -1, NULL, {0, 0, 0, NULL}},
	},
	/* CL15: Middle Offense */
	{
		{ A_INT, "like a Mindflayer", {15, 12, 50, mind_blast_spell}},
		{ A_CON, "like a Mutant", {15,  0,  40, breathe_fire_I_spell}},
		{ A_CHR, "like a Beastmaster", {15, 0, 50, dominate_living_II_spell}},
		{ A_WIS, "like a Black Knight", {15, 12, 40, cause_wounds_III_spell}},
		{ -1, NULL, {0, 0, 0, NULL}},
	},
	/* CL17: Middle Offense */
	{
		{ A_STR, "like a Cyclops", {17, 15, 50, throw_boulder_spell}},
		{ A_STR, "like an Android", {17, 20, 40, android_bazooka_spell}},
		{ A_CHR, "like a Dark Elven Warlock", {17, 15, 50, mana_bolt_I_spell}},
		{ A_DEX, "like a Samurai", {17, 12, 40, burning_strike_spell}},
		{ -1, NULL, {0, 0, 0, NULL}},
	},
	/* CL19: Good Utility */
	{
		{ A_INT, "like a Half Titan", {15, 10, 40, probing_spell}},
		{ A_STR, "like a Half Giant", {19, 10, 40, stone_to_mud_spell}},
		{ A_CHR, "like a Warlock", {19, 20, 50, identify_spell}},
		{ A_CHR, "like a Warlock", {19, 10, 40, teleport_spell}},
		{ A_INT, "like a Mutant", {15, 12, 40, swap_pos_spell}},
		{ -1, NULL, {0, 0, 0, NULL}},
	},
	/* CL21: Good Buff*/
	{
        { A_STR, "like a Half Troll", {10, 12, 50, berserk_spell}},
		{ A_CON, "like a Golem", {20, 15, 50, stone_skin_spell}},		
		{ A_CHR, "like a Kutar", {20, 15, 40, kutar_expand_spell}},
		{ A_CHR, "like a Warlock", {15, 10, 30, heroism_spell}},
		{ A_CHR, "like a Warlock", {21, 40, 40, protection_from_evil_spell}},
		{ -1, NULL, {0, 0, 0, NULL}},
	},
	/* CL23: Good Recovery */
	{
        { A_WIS, "like a Zombie", {23, 30, 50, restore_life_spell}},
		{ A_WIS, "like a Force-Trainer", {15, 0, 30, clear_mind_spell}},
		{ A_CHR, "like a Warlock", {20, 20, 50, remove_curse_I_spell}},
		{ A_WIS, "like a Priest", {20, 15, 50, cure_wounds_III_spell}},
		{ A_WIS, "like a Ranger", {8, 7, 50, resist_environment_spell}},
		{ -1, NULL, {0, 0, 0, NULL}},
	},
	/* CL25: Good Offense */
	{
		{ A_CON, "like a Draconian", {1, 0, 40, draconian_breath_spell}},
		{ A_DEX, "like a Klackon", {9, 9, 40, spit_acid_spell}},
		{ A_CON, "like a Balrog", {15, 10, 50, demon_breath_spell}},
		{ A_WIS, "like a Blue-Mage", {20, 10,  40, brain_smash_spell}},
		{ A_INT, "like a Chaos Warrior", {5, 4, 30, touch_of_confusion_spell}},
		{ -1, NULL, {0, 0, 0, NULL}},
	},
	/* CL27: Good Utility */
	{
		{ A_DEX, "like a Rogue", {8, 12, 50, panic_hit_spell}},
		{ A_INT, "like a Half Ogre", {25, 35, 40, explosive_rune_spell}},
		{ A_CHR, "like a Warlock", {25, 20, 50, magic_mapping_spell}},
		{ A_CHR, "like a Warlock", {25, 40, 50, recharging_spell}},
		{ A_INT, "like a Mutant", {10, 5, 50, alchemy_spell}},
		{ A_WIS, "like a Mutant", {9, 9, 40, telekinesis_spell}},
		{ -1, NULL, {0, 0, 0, NULL}},
	},
	/* CL29: Good Recovery/Ability */
	{
		{ A_INT, "like a Warrior-Mage", {25, 0, 50, hp_to_sp_spell}},
		{ A_INT, "like a Warrior-Mage", {25, 0, 50, sp_to_hp_spell}},
		{ A_INT, "like a Mage", {25, 1, 70, eat_magic_spell}},
		{ A_DEX, "like a Ninja", {20, 0, 0, quick_walk_spell}},
		{ -1, NULL, {0, 0, 0, NULL}},
	},
	/* CL31: Good Offense */
	{
		{ A_WIS, "like a Good Paladin", {30, 30, 40, holy_lance_spell}},
		{ A_WIS, "like an Evil Paladin", {30, 30, 40, hell_lance_spell}},
		{ A_STR, "like an Android", {30, 30, 50, android_beam_cannon_spell}},
		{ A_INT, "like a Monastic Lich", {30, 30, 40, cause_wounds_IV_spell}},
		{ -1, NULL, {0, 0, 0, NULL}},
	},
	/* CL33: Good Utility */
	{
		{ A_INT, "like an Amberite", {30, 50, 50, shadow_shifting_spell}},
		{ A_INT, "like a Tourist", {25, 20, 30, identify_fully_spell}},
		{ A_CHR, "like a Warlock", {30, 10, 40, earthquake_spell}},
		{ A_CHR, "like a Warlock", {30, 20, 40, teleport_level_spell}},
		{ A_WIS, "like a Priest", {30, 30, 50, healing_I_spell}},
		{ -1, NULL, {0, 0, 0, NULL}},
	},
	/* CL35: Good Offense/Ability */
	{
		{ A_DEX, "like a Samurai", {33, 20, 60, lightning_eagle_spell}},
		{ A_DEX, "like a Monk", {30, 30, 60, monk_double_attack_spell}},
		{ A_INT, "like Jack of Shadows", {35, 30, 50, darkness_storm_I_spell}},
		{ A_INT, "like a Solar", {35, 30, 50, starburst_I_spell}},
		{ A_CON, "like a Greater Balrog", {35, 10, 50, breathe_fire_II_spell}},
		{ -1, NULL, {0, 0, 0, NULL}},
	},
	/* CL37: Great Buff */
	{
		{ A_DEX, "like a Monk", {25, 0, 0, monk_posture_spell}},
		{ A_DEX, "like a Samurai", {25, 0, 0, samurai_posture_spell}},
		{ A_CON, "like a Mutant", {25, 10, 50, resist_elements_spell}},
		{ A_INT, "like a Daemon Mage", {35, 40, 80, polymorph_demon_spell}},
		{ -1, NULL, {0, 0, 0, NULL}},
	},
	/* CL39: Great Utility */
	{
		{ A_CHR, "like a Mutant", {12, 23, 70, sterility_spell}},
		{ A_CHR, "like a Mutant", {7, 15, 60, dazzle_spell}},
		{ A_WIS, "like a Mutant", {25, 25, 70, banish_evil_spell}},
		{ A_CON, "like a Mutant", {1, 6, 60, grow_mold_spell}},
		{ A_DEX, "like a Samurai", {30, 20, 65, rush_attack_spell}},
		{ -1, NULL, {0, 0, 0, NULL}},
	},
	/* CL41: Great Offense */
	{
		{ A_STR, "like an Android", {40, 40, 70, android_rocket_spell}},
		{ A_WIS, "like a Mindcrafter", {40, 40, 70, psycho_spear_spell}},
		{ A_CON, "like The Destroyer", {40, 40, 70, breathe_disintegration_spell}},
		{ A_CHR, "like an Eye Druj", {40, 35, 50, mana_bolt_II_spell}},
		{ A_INT, "like Habu, The Champion of Chaos", {40, 40, 65, mana_storm_I_spell}},
		{ -1, NULL, {0, 0, 0, NULL}},
	},
	/* CL43: Great Utility */
	{
		{ A_WIS, "like an Amberite", {40, 75, 75, pattern_mindwalk_spell}},
		{ A_CHR, "like a Warlock", {35, 70, 60, destruction_spell}},
		{ A_CHR, "like a Warlock", {40, 50, 65, dimension_door_spell}},
		{ A_WIS, "like a Life Priest", {35, 70, 90, clairvoyance_spell}},
		{ -1, NULL, {0, 0, 0, NULL}},
	},
	/* CL45: Great Utility */
	{
		{ A_INT, "like a Chaos Warrior", {40, 50, 60, confusing_lights_spell}},
		{ A_WIS, "like an Evil Priest", {40, 40, 60, evocation_spell}},
		{ A_STR, "like a Berserker", {40, 50, 80, massacre_spell}},
		{ A_DEX, "like a Ninja", {40, 50, 80, hide_in_mud_spell}},
		{ A_WIS, "like a Crusade Paladin", {44, 50, 80, eye_for_an_eye_spell}},
		{ -1, NULL, {0, 0, 0, NULL}},
	},
	/* CL47: General Awesomeness */
	{
		{ A_DEX, "like a Ninja", {40, 50, 50, super_stealth_spell}},
		{ A_WIS, "like a Priest", {40, 50, 80, healing_II_spell}},
		{ A_INT, "like a Hex Mage", {40, 50, 60, building_up_spell}},
		{ A_INT, "like a Craft Mage", {47, 70, 70, polymorph_colossus_spell}},
		{ -1, NULL, {0, 0, 0, NULL}},
	},
	/* CL49: Capstone Offense */
	{
		{ A_INT, "like Atlach-Nacha, the Spider God", {49, 50, 65, darkness_storm_II_spell}},
		{ A_WIS, "like Raphael, the Messenger", {49, 50, 65, starburst_II_spell}},
		{ A_STR, "like Oremorj, the Cyberdemon Lord", {49, 50, 65, rocket_II_spell}},
		{ A_CON, "like Morgoth, Lord of Darkness", {49, 50, 70, mana_storm_II_spell}},
		{ A_INT, "like a Craft Mage", {49, 50, 70, force_branding_spell}},
		{ -1, NULL, {0, 0, 0, NULL}},
	},
};

static int _group_size(int i)
{
	int result = 0;
	if (i >= 0 && i < _MAX_TALENTS)
	{
		int j;
		for (j = 0; j < _MAX_TALENTS_PER_GROUP ; ++j)
		{
			talent_t *talent_ptr = &_talents[i][j];
			if (talent_ptr->stat == -1)
			{
				result = j;
				break;
			}
		}
	}
	return result;
}

static int _get_spells_imp(spell_info* spells, int max, int start, int stop)
{
	int ct = 0, i;
	for (i = start; i <= stop; ++i)
	{
		int idx = p_ptr->magic_num1[i] - 1;	/* Magic Numbers are base 1, Table indices base 0 */
		if (ct >= max) break;
		if (idx >= 0 && idx < _group_size(i))
		{
			talent_t *talent = &_talents[i][idx];
			spell_info *spell = &spells[ct++];
			spell->level = talent->spell.level;
			spell->cost = talent->spell.cost;
			spell->fail = calculate_fail_rate(
				talent->spell.level, 
				talent->spell.fail, 
				p_ptr->stat_ind[talent->stat]
			);
			spell->fn = talent->spell.fn;
		}
	}
	return ct;
}

/*
 * We now group wild talents.  Its hard to pick from a large list of seemingly unsorted
 * choices.  Also, there is wide variety in monitor sizes and resolutions, so attempting
 * to prompt for more than 15 or so choices at a time is a bad idea anyway.
 */
typedef struct {
	cptr name;
	cptr help;
	int min_slot;
	int max_slot;
	int color;
} group_choice;

group_choice _groups[] =  {
	{ "Wild Beginnings", "Your early wild talents.  Perhaps these are not so awe inspiring, but they will allow you to survive early on.", 0, 7, TERM_GREEN},
	{ "Wild Musings", "Your middle powers, more useful and destructive.", 8, 16, TERM_L_UMBER},
	{ "Wild Destructions", "Your most powerful wild talents.  Death!  Destruction!  Devastation!  Monsters tremble in fear before the awesomeness of your power!", 17, _MAX_TALENTS - 1, TERM_RED},
};

static cptr _group_name(menu_choices choices, int which) {
	return _groups[which].name;
}

static cptr _group_help(menu_choices choices, int which) {
	return _groups[which].help;
}

static int _group_color(menu_choices choices, int which) {
	return _groups[which].color;
}

static int _get_spells(spell_info* spells, int max)
{
	int idx = -1;
	int ct = 0;
	menu_list_t list = { "Use which group of talents?", "Browse which group of talents?", NULL,
						_group_name, _group_help, _group_color, 
						_groups, 3};
	
	idx = menu_choose(&list);
	if (idx < 0) return 0;

	/* Hack: Add innate Wonder attack to Wild Beginnings */
	if (idx == 0)
	{
		spell_info* spell = &spells[ct++];
		spell->level = 10;
		spell->cost = 10;
		spell->fail = calculate_fail_rate(10, 30, p_ptr->stat_ind[A_INT]);
		spell->fn = wonder_spell;
	}

	ct += _get_spells_imp(spells + ct, max - ct, _groups[idx].min_slot, _groups[idx].max_slot);
	if (ct == 0)
		msg_print("You don't know any of those talents yet!");
	return ct;
}

void _gain_power(int level)
{
int group_idx = -1;

	if (level % 2 == 1)
		group_idx = level/2;

	if (group_idx >= 0 && group_idx < _MAX_TALENTS && _group_size(group_idx) > 0)
	{
		variant name;
		int idx = randint0(_group_size(group_idx));
		talent_t *talent = &_talents[group_idx][idx];

		if (!talent->spell.fn)
		{
			msg_print("BUG: Where is your talent?!");
			return;
		}

		var_init(&name);
		(talent->spell.fn)(SPELL_NAME, &name);

		msg_format("You gain the power of '%s' %s.", var_get_string(&name), talent->gain_desc);
		p_ptr->magic_num1[group_idx] = idx + 1;

		var_clear(&name);
	}
}

void wild_talent_fix_up(void)
{
	int i;

	if (p_ptr->pclass != CLASS_WILD_TALENT) return;

	for (i = 1; i <= p_ptr->max_plv; ++i)
	{
	int group_idx = -1;

		if (i % 2 == 1)
			group_idx = i/2;

		if (group_idx >= 0 && group_idx < _MAX_TALENTS && _group_size(group_idx) > 0 && p_ptr->magic_num1[group_idx] == 0)
		{
			_gain_power(i);
		}
	}
}



bool _mut_avg_pred(int mut_idx)
{
	if (mut_type(mut_idx) == MUT_TYPE_ACTIVATION) return FALSE;
	if (mut_rating(mut_idx) == MUT_RATING_AVERAGE) return TRUE;
	return FALSE;
}

bool _mut_avg_plus_pred(int mut_idx)
{
	if (mut_type(mut_idx) == MUT_TYPE_ACTIVATION) return FALSE;
	if (mut_rating(mut_idx) >= MUT_RATING_AVERAGE) return TRUE;
	return FALSE;
}

bool _mut_good_pred(int mut_idx)
{
	if (mut_type(mut_idx) == MUT_TYPE_ACTIVATION) return FALSE;
	if (mut_rating(mut_idx) == MUT_RATING_GOOD) return TRUE;
	return FALSE;
}

bool _mut_good_plus_pred(int mut_idx)
{
	if (mut_type(mut_idx) == MUT_TYPE_ACTIVATION) return FALSE;
	if (mut_rating(mut_idx) >= MUT_RATING_GOOD) return TRUE;
	return FALSE;
}

bool _mut_great_pred(int mut_idx)
{
	if (mut_type(mut_idx) == MUT_TYPE_ACTIVATION) return FALSE;
	if (mut_rating(mut_idx) == MUT_RATING_GREAT) return TRUE;
	return FALSE;
}

void _gain_mutation(int level)
{
	switch (level)
	{
	case 10:
		mut_gain_random(_mut_good_pred);
		break;
	case 20:
		mut_gain_random(_mut_good_pred);
		break;
	case 30:
		mut_gain_random(_mut_good_plus_pred);
		break;
	case 40:
		mut_gain_random(_mut_great_pred);
		break;
	case 50:
		mut_gain_random(_mut_great_pred);
		break;
	}
}

static void _gain_level(int new_level)
{
	_gain_power(new_level);
	_gain_mutation(new_level);
}

void wild_talent_scramble(void)
{
	int i;

	if (p_ptr->pclass != CLASS_WILD_TALENT) return;

	/* Forget old talents */
	for (i = 0; i <= _MAX_TALENTS; ++i)
		p_ptr->magic_num1[i] = 0;

	msg_print("You feel wild forces of randomness enter your body!");

	/* Regain new talents */
	for (i = 1; i <= p_ptr->max_plv; ++i)
		_gain_power(i);
}

void wild_talent_new_life(void)
{
	int i;

	if (p_ptr->pclass != CLASS_WILD_TALENT) return;

	/* re-grant powers */
	wild_talent_scramble();

	/* re-grant mutations */
	for (i = 1; i <= p_ptr->max_plv; ++i)
		_gain_mutation(i);
}

static void _calc_bonuses(void)
{
	if (p_ptr->lev >= 30)
		p_ptr->regenerate = TRUE;
}

static void _character_dump(FILE* file)
{
	int i;
	spell_info spells[MAX_SPELLS];
	int ct = _get_spells_imp(spells, MAX_SPELLS, 0, _MAX_TALENTS - 1);

	for (i = 0; i < ct; i++)
	{
		spell_info* current = &spells[i];
		current->cost += get_spell_cost_extra(current->fn);
		current->fail = MAX(current->fail, get_spell_fail_min(current->fn));
	}

	if (ct > 0)
	{
		int i;
		variant name, info;

		var_init(&name);
		var_init(&info);

		fprintf(file, "\n\n  [Wild Talents]\n\n");
		fprintf(file, "%-23.23s Lv Cost Fail Info\n", "");
		for (i = 0; i < ct; ++i)
		{
			spell_info *spell = &spells[i];

			(spell->fn)(SPELL_NAME, &name);
			(spell->fn)(SPELL_INFO, &info);

			fprintf(file, "%-23.23s %2d %4d %3d%% %s\n", 
							var_get_string(&name),
							spell->level,
							spell->cost,
							spell->fail,
							var_get_string(&info));
		}

		var_clear(&name);
		var_clear(&info);
	}
}

static caster_info * _caster_info(void)
{
	static caster_info me = {0};
	static bool init = FALSE;
	if (!init)
	{
		me.magic_desc = "wild spell";
		me.use_sp = TRUE;
		init = TRUE;
	}
	return &me;
}

static void _spoiler_dump(FILE* fff)
{
	int i, j, lv = 1;
	variant vn, vd;
	var_init(&vn);
	var_init(&vd);

	fprintf(fff, "\n== Spells ==\n\n");
	fprintf(fff, "The Wild-Talent gains one spell randomly from each group at the indicated level. Potions of New Life and Polymorph as well as Chaos attacks can scramble the Wild-Talent's powers.\n\n");
	for (i = 0; i < _MAX_TALENTS; i++)
	{
		fprintf(fff, "|| *L%d Power* || *Stat* || *Lvl* || *Mana* || *Fail* || *Description* ||\n", lv);

		for (j = 0; j < _MAX_TALENTS_PER_GROUP; j++)
		{
			talent_t *talent_ptr = &_talents[i][j];

			if (talent_ptr->stat == -1) break;

			talent_ptr->spell.fn(SPELL_SPOIL_NAME, &vn);
			if (var_is_null(&vn)) talent_ptr->spell.fn(SPELL_NAME, &vn);

			talent_ptr->spell.fn(SPELL_SPOIL_DESC, &vd);
			if (var_is_null(&vd)) talent_ptr->spell.fn(SPELL_DESC, &vd);

			fprintf(fff, "||%s||%s||%d||%d||%d||%s||\n", 
				var_get_string(&vn), stat_abbrev_true[talent_ptr->stat], 
				talent_ptr->spell.level, talent_ptr->spell.cost, talent_ptr->spell.fail,
				var_get_string(&vd)
			);
		}
		lv += 2;
	}

	var_clear(&vn);
	var_clear(&vd);
}

class_t *wild_talent_get_class_t(void)
{
	static class_t me = {0};
	static bool init = FALSE;

	/* static info never changes */
	if (!init)
	{           /* dis, dev, sav, stl, srh, fos, thn, thb */
	skills_t bs = { 30,  40,  31,   1,  24,  16,  68,  50 };
	skills_t xs = {  8,  15,  10,   0,   0,   0,  18,  20 };

		me.name = "Wild-Talent";
		me.desc = "The Wild-Talent gains random talents and abilities as they "
		          "level up.  They are good fighters, and decent with magical devices, "
				  "but their true forte is their vast array of potential random "
				  "powers.  Except you never know what those might be!";
		
		me.stats[A_STR] = -1;
		me.stats[A_INT] =  2;
		me.stats[A_WIS] = -1;
		me.stats[A_DEX] =  2;
		me.stats[A_CON] = -2;
		me.stats[A_CHR] = +3;
		
		me.base_skills = bs;
		me.extra_skills = xs;
		
		me.hd = 2;
		me.exp = 110;
		me.pets = 35;
		
		me.calc_bonuses = _calc_bonuses;
		me.get_spells = _get_spells;
		me.caster_info = _caster_info;
		me.gain_level = _gain_level;
		me.character_dump = _character_dump;
		me.spoiler_dump = _spoiler_dump;
		init = TRUE;
	}

	return &me;
}
