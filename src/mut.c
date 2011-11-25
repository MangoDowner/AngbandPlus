#include "angband.h"

typedef struct {
	mutation_rating rating;
	u16b type;
	byte stat;
	byte prob;
	spell_info spell;
} mutation_info;

static mutation_info _mutations[MAX_MUTATIONS] = 
{
	{MUT_RATING_GOOD,		MUT_TYPE_ACTIVATION, A_DEX, 8, {9,  9,  30, spit_acid_spell}},
	{MUT_RATING_GOOD,		MUT_TYPE_ACTIVATION, A_CON, 6, {20,  0,  40, breathe_fire_I_spell}},
	{MUT_RATING_AVERAGE,	MUT_TYPE_ACTIVATION, A_CHR, 4, {12, 12,  40, hypnotic_gaze_spell}},
	{MUT_RATING_AVERAGE,	MUT_TYPE_ACTIVATION, A_WIS, 4, {9,  9,  40, telekinesis_spell}},
	{MUT_RATING_GOOD,		MUT_TYPE_ACTIVATION, A_WIS, 6, {7,  7,  30, teleport_spell}},
	{MUT_RATING_GOOD,		MUT_TYPE_ACTIVATION, A_WIS, 4, {5,  3,  30, mind_blast_spell}},
	{MUT_RATING_GOOD,		MUT_TYPE_ACTIVATION, A_CON, 4, {15, 15,  30, radiation_spell}},
	{MUT_RATING_GOOD,		MUT_TYPE_ACTIVATION, A_CON, 4, {2,  1,  30, vampirism_spell}},
	{MUT_RATING_GOOD,		MUT_TYPE_ACTIVATION, A_INT, 6, {3,  2,  30, smell_metal_spell}},
	{MUT_RATING_GOOD,		MUT_TYPE_ACTIVATION, A_INT, 8, {5,  4,  30, smell_monsters_spell}},
	{MUT_RATING_GOOD,		MUT_TYPE_ACTIVATION, A_WIS, 6, {3,  3,  30, phase_door_spell}},
	{MUT_RATING_AVERAGE,	MUT_TYPE_ACTIVATION, A_CON, 4, {8, 12,  40, eat_rock_spell}},
	{MUT_RATING_GOOD,		MUT_TYPE_ACTIVATION, A_DEX, 4, {15, 12,  40, swap_pos_spell}},	
	{MUT_RATING_GOOD,		MUT_TYPE_ACTIVATION, A_CON, 6, {20, 14,  40, shriek_spell}},
	{MUT_RATING_AVERAGE,	MUT_TYPE_ACTIVATION, A_INT, 6, {3,  2,  30, light_area_spell}},
	{MUT_RATING_AVERAGE,	MUT_TYPE_ACTIVATION, A_WIS, 4, {7, 14,  30, detect_curses_spell}},
	{MUT_RATING_GREAT,		MUT_TYPE_ACTIVATION, A_STR, 6, {8,  8,  50, berserk_spell}},
	{MUT_RATING_AVERAGE,	MUT_TYPE_ACTIVATION, A_CON, 2, {18, 20,  50, polymorph_self_spell}},
	{MUT_RATING_AVERAGE,	MUT_TYPE_ACTIVATION, A_INT, 4, {10,  5,  70, alchemy_spell}},
	{MUT_RATING_GOOD,		MUT_TYPE_ACTIVATION, A_CON, 2, {1,  6,  60, grow_mold_spell}},
	{MUT_RATING_GREAT,		MUT_TYPE_ACTIVATION, A_CON, 6, {25, 10,  50, resist_elements_spell}},
	{MUT_RATING_GOOD,		MUT_TYPE_ACTIVATION, A_STR, 6, {12, 12,  50, earthquake_spell}},
	{MUT_RATING_GREAT,		MUT_TYPE_ACTIVATION, A_WIS, 2, {17,  1,  80, eat_magic_spell}},
	{MUT_RATING_AVERAGE,	MUT_TYPE_ACTIVATION, A_INT, 4, {6,  6,  50, weigh_magic_spell}},
	{MUT_RATING_GREAT,		MUT_TYPE_ACTIVATION, A_CHR, 2, {12, 23,  70, sterility_spell}},
	{MUT_RATING_GOOD,		MUT_TYPE_ACTIVATION, A_DEX, 4, {10, 12,  60, panic_hit_spell}},
	{MUT_RATING_GOOD,		MUT_TYPE_ACTIVATION, A_CHR, 6, {7, 15,  60, dazzle_spell}},
	{MUT_RATING_GOOD,		MUT_TYPE_ACTIVATION, A_WIS, 6, {7, 10,  50, laser_eye_spell}},
	{MUT_RATING_GOOD,		MUT_TYPE_ACTIVATION, A_INT, 4, {17, 50,  70, recall_spell}},
	{MUT_RATING_GOOD,		MUT_TYPE_ACTIVATION, A_WIS, 2, {25, 25,  70, banish_evil_spell}},
	{MUT_RATING_AVERAGE,	MUT_TYPE_ACTIVATION, A_CON, 4, {2,  2,  30, cold_touch_spell}},
	{MUT_RATING_AVERAGE,	MUT_TYPE_ACTIVATION, A_STR, 4, {1,  0,  40, power_throw_spell}},
	{MUT_RATING_BAD,		MUT_TYPE_EFFECT,	     0, 2, {0,  0,   0, berserk_rage_mut}},
	{MUT_RATING_BAD,		MUT_TYPE_EFFECT,	     0, 2, {0,  0,   0, cowardice_mut}},
	{MUT_RATING_BAD,		MUT_TYPE_EFFECT,	     0, 2, {0,  0,   0, random_teleport_mut}},
	{MUT_RATING_BAD,		MUT_TYPE_EFFECT,	     0, 2, {0,  0,   0, alcohol_mut}},
	{MUT_RATING_BAD,		MUT_TYPE_EFFECT,	     0, 2, {0,  0,   0, hallucination_mut}},
	{MUT_RATING_AVERAGE,	MUT_TYPE_EFFECT,	     0, 2, {0,  0,   0, flatulence_mut}},
	{MUT_RATING_GOOD,		              0,	     0, 4, {0,  0,   0, scorpion_tail_mut}},
	{MUT_RATING_GOOD,		              0,	     0, 4, {0,  0,   0, horns_mut}},
	{MUT_RATING_GOOD,		              0,	     0, 4, {0,  0,   0, beak_mut}},
	{MUT_RATING_BAD,		MUT_TYPE_EFFECT,	     0, 4, {0,  0,   0, attract_demon_mut}},
	{MUT_RATING_AVERAGE,	MUT_TYPE_EFFECT,	     0, 2, {0,  0,   0, produce_mana_mut}},
	{MUT_RATING_BAD,		MUT_TYPE_EFFECT,	     0, 4, {0,  0,   0, speed_flux_mut}},
	{MUT_RATING_AVERAGE,	MUT_TYPE_EFFECT,	     0, 4, {0,  0,   0, random_banish_mut}},
	{MUT_RATING_AVERAGE,	MUT_TYPE_EFFECT,	     0, 2, {0,  0,   0, eat_light_mut}},
	{MUT_RATING_GOOD,		              0,	     0, 4, {0,  0,   0, trunk_mut}},
	{MUT_RATING_BAD,		MUT_TYPE_EFFECT,	     0, 2, {0,  0,   0, attract_animal_mut}},
	{MUT_RATING_GOOD,		              0,	     0, 2, {0,  0,   0, tentacles_mut}},
	{MUT_RATING_AVERAGE,	MUT_TYPE_EFFECT,	     0, 2, {0,  0,   0, raw_chaos_mut}},
	{MUT_RATING_BAD,		MUT_TYPE_EFFECT,	     0, 6, {0,  0,   0, normality_mut}},
	{MUT_RATING_GOOD,		MUT_TYPE_EFFECT,	     0, 2, {0,  0,   0, wraith_mut}},
	{MUT_RATING_AVERAGE,	MUT_TYPE_EFFECT,	     0, 2, {0,  0,   0, polymorph_wounds_mut}},
	{MUT_RATING_AWFUL,		MUT_TYPE_EFFECT,	     0, 2, {0,  0,   0, wasting_mut}},
	{MUT_RATING_BAD,		MUT_TYPE_EFFECT,	     0, 2, {0,  0,   0, attract_dragon_mut}},
	{MUT_RATING_AVERAGE,	MUT_TYPE_EFFECT,	     0, 4, {0,  0,   0, weird_mind_mut}},
	{MUT_RATING_BAD,		MUT_TYPE_EFFECT,	     0, 2, {0,  0,   0, nausea_mut}},
	{MUT_RATING_GREAT,		              0,	     0, 4, {0,  0,   0, chaos_deity_mut}},
	{MUT_RATING_BAD,		MUT_TYPE_EFFECT,	     0, 2, {0,  0,   0, shadow_walk_mut}},
	{MUT_RATING_AVERAGE,	MUT_TYPE_EFFECT,	     0, 2, {0,  0,   0, warning_mut}},
	{MUT_RATING_GOOD,		MUT_TYPE_EFFECT,	     0, 2, {0,  0,   0, invulnerability_mut}},
	{MUT_RATING_AVERAGE,	MUT_TYPE_EFFECT,	     0, 4, {0,  0,   0, sp_to_hp_spell}},
	{MUT_RATING_AVERAGE,	MUT_TYPE_EFFECT,	     0, 2, {0,  0,   0, hp_to_sp_spell}},
	{MUT_RATING_BAD,		MUT_TYPE_EFFECT,	     0, 2, {0,  0,   0, fumbling_mut}},
	{MUT_RATING_GREAT,		MUT_TYPE_BONUS,			 0, 6, {0,  0,   0, he_man_mut}},
	{MUT_RATING_AWFUL,		MUT_TYPE_BONUS,			 0, 6, {0,  0,   0, puny_mut}},
	{MUT_RATING_GREAT,		MUT_TYPE_BONUS,			 0, 6, {0,  0,   0, einstein_mut}},
	{MUT_RATING_AWFUL,		MUT_TYPE_BONUS,			 0, 6, {0,  0,   0, moron_mut}},
	{MUT_RATING_GREAT,		MUT_TYPE_BONUS,			 0, 4, {0,  0,   0, resilient_mut}},
	{MUT_RATING_AVERAGE,	MUT_TYPE_BONUS,			 0, 4, {0,  0,   0, fat_mut}},
	{MUT_RATING_AWFUL,		MUT_TYPE_BONUS,			 0, 4, {0,  0,   0, albino_mut}},
	{MUT_RATING_AWFUL,		MUT_TYPE_BONUS,			 0, 6, {0,  0,   0, rotting_flesh_mut}},
	{MUT_RATING_BAD,		MUT_TYPE_BONUS,			 0, 4, {0,  0,   0, silly_voice_mut}},
	{MUT_RATING_BAD,		MUT_TYPE_BONUS,			 0, 4, {0,  0,   0, blank_face_mut}},
	{MUT_RATING_AVERAGE,	             0,			 0, 2, {0,  0,   0, illusion_normal_mut}},
	{MUT_RATING_AVERAGE,	MUT_TYPE_BONUS,			 0, 6, {0,  0,   0, extra_eyes_mut}},
	{MUT_RATING_GOOD,		MUT_TYPE_BONUS,			 0, 4, {0,  0,   0, magic_resistance_mut}},
	{MUT_RATING_BAD,		MUT_TYPE_BONUS,			 0, 6, {0,  0,   0, extra_noise_mut}},
	{MUT_RATING_AVERAGE,	MUT_TYPE_BONUS,			 0, 6, {0,  0,   0, infravision_mut}},
	{MUT_RATING_GREAT,		MUT_TYPE_BONUS,			 0, 4, {0,  0,   0, extra_legs_mut}},
	{MUT_RATING_AWFUL,		MUT_TYPE_BONUS,			 0, 4, {0,  0,   0, short_legs_mut}},
	{MUT_RATING_GOOD,		MUT_TYPE_BONUS,			 0, 4, {0,  0,   0, elec_aura_mut}},
	{MUT_RATING_GOOD,		MUT_TYPE_BONUS,			 0, 4, {0,  0,   0, fire_aura_mut}},
	{MUT_RATING_AVERAGE,	MUT_TYPE_BONUS,			 0, 6, {0,  0,   0, warts_mut}},
	{MUT_RATING_GOOD,		MUT_TYPE_BONUS,			 0, 6, {0,  0,   0, scales_mut}},
	{MUT_RATING_GREAT,		MUT_TYPE_BONUS,			 0, 4, {0,  0,   0, steel_skin_mut}},
	{MUT_RATING_AVERAGE,	MUT_TYPE_BONUS,			 0, 4, {0,  0,   0, wings_mut}},
	{MUT_RATING_AVERAGE,	MUT_TYPE_BONUS,			 0, 6, {0,  0,   0, fearless_mut}},
	{MUT_RATING_GOOD,		MUT_TYPE_BONUS,			 0, 4, {0,  0,   0, regeneration_mut}},
	{MUT_RATING_GREAT,		MUT_TYPE_BONUS,			 0, 4, {0,  0,   0, telepathy_mut}},
	{MUT_RATING_GOOD,		MUT_TYPE_BONUS,			 0, 6, {0,  0,   0, limber_mut}},
	{MUT_RATING_BAD,		MUT_TYPE_BONUS,			 0, 6, {0,  0,   0, arthritis_mut}},
	{MUT_RATING_AWFUL,		             0,			 0, 2, {0,  0,   0, bad_luck_mut}},
	{MUT_RATING_AWFUL,		             0,			 0, 2, {0,  0,   0, vulnerability_mut}},
	{MUT_RATING_GOOD,		MUT_TYPE_BONUS,			 0, 6, {0,  0,   0, motion_mut}},
	{MUT_RATING_GREAT,				     0,			 0, 2, {0,  0,   0, good_luck_mut}},
	{MUT_RATING_GREAT,				     0,			 0, 0, {0,  0,   0, fast_learner_mut}},
	{MUT_RATING_GOOD,				     0,			 0, 0, {0,  0,   0, weapon_skills_mut}},
	{MUT_RATING_GOOD,				     0,			 0, 0, {0,  0,   0, subtle_casting_mut}},
	{MUT_RATING_GOOD,				     0,			 0, 0, {0,  0,   0, peerless_sniper_mut}},
	{MUT_RATING_GOOD,				     0,			 0, 0, {0,  0,   0, unyielding_mut}},
	{MUT_RATING_GREAT,				     0,			 0, 0, {0,  0,   0, ambidexterity_mut}},
	{MUT_RATING_GOOD,		MUT_TYPE_BONUS,			 0, 0, {0,  0,   0, untouchable_mut}},
	{MUT_RATING_GOOD,		MUT_TYPE_ACTIVATION, A_INT, 0, {25, 25, 70, loremaster_mut}},
	{MUT_RATING_GREAT,				     0,			 0, 0, {0,  0,   0, arcane_mastery_mut}},
	{MUT_RATING_GREAT,				     0,			 0, 0, {0,  0,   0, evasion_mut}},
	{MUT_RATING_GOOD,				     0,			 0, 0, {0,  0,   0, potion_chugger_mut}},
	{MUT_RATING_GREAT,				     0,			 0, 0, {0,  0,   0, one_with_magic_mut}},
	{MUT_RATING_GREAT,	    MUT_TYPE_ACTIVATION, A_INT, 0, {20, 25, 70, peerless_tracker_mut}},
	{MUT_RATING_GREAT,				     0,			 0, 0, {0,  0,   0, infernal_deal_mut}},
};

int _mut_prob_gain(int i)
{
	int result = _mutations[i].prob;
	const int racial_odds = 50;

	if (p_ptr->pclass == CLASS_BERSERKER && mut_type(i) & MUT_TYPE_ACTIVATION)
		return 0;

	switch (i)
	{
	case MUT_CHAOS_GIFT:
		/* TODO: Birth Chaos Warriors with this mutation */
		if (p_ptr->pclass == CLASS_CHAOS_WARRIOR)
			return 0;
		break;

	case MUT_BAD_LUCK:
		if (mut_locked(MUT_GOOD_LUCK))
			return 0;
		break;

	case MUT_HYPN_GAZE:
		if (p_ptr->prace == RACE_VAMPIRE)
			return racial_odds;
		break;

	case MUT_HORNS:
		if (p_ptr->prace == RACE_IMP)
			return racial_odds;
		break;

	case MUT_SHRIEK:
		if (p_ptr->prace == RACE_YEEK)
			return racial_odds;
		break;

	case MUT_POLYMORPH:
		if (p_ptr->prace == RACE_BEASTMAN)
			return racial_odds;
		break;

	case MUT_TENTACLES:
		if (p_ptr->prace == RACE_MIND_FLAYER)
			return racial_odds;
		break;
	}

	if ( _mutations[i].rating < MUT_RATING_AVERAGE
	  && mut_present(MUT_GOOD_LUCK) )
	{
		result = 1;
	}

	if ( _mutations[i].rating > MUT_RATING_AVERAGE
	  && mut_present(MUT_BAD_LUCK) )
	{
		result = 1;
	}

	return result;
}

int _mut_prob_lose(int i)
{
	int result = _mutations[i].prob;

	if ( _mutations[i].rating > MUT_RATING_AVERAGE
	  && mut_present(MUT_GOOD_LUCK) )
	{
		result = 1;
	}

	if ( _mutations[i].rating < MUT_RATING_AVERAGE
	  && mut_present(MUT_BAD_LUCK) )
	{
		result = 1;
	}

	return result;
}

void _mut_refresh(void)
{
	mutant_regenerate_mod = mut_regenerate_mod();
	p_ptr->update |= PU_BONUS;
	p_ptr->update |= PU_HP;
	p_ptr->update |= PU_MANA;
	p_ptr->update |= PU_SPELLS;
	handle_stuff();
}

void mut_calc_bonuses(void)
{
	int i;
	variant v;
	var_init(&v);

	for (i = 0; i < MAX_MUTATIONS; i++)
	{
		if ( mut_present(i)
		  && (_mutations[i].type & MUT_TYPE_BONUS) )
		{
			(_mutations[i].spell.fn)(SPELL_CALC_BONUS, &v);
		}
	}

	var_clear(&v);
}

int mut_count(mut_pred pred)
{
	int i;
	int count = 0;
	for (i = 0; i < MAX_MUTATIONS; ++i)
	{
		if (mut_present(i))
		{
			if (pred == NULL || (pred)(i))
				++count;
		}
	}

	return count;
}

void mut_do_cmd_knowledge(void)
{
	FILE *fff;
	char file_name[1024];

	/* Open a new file */
	fff = my_fopen_temp(file_name, 1024);

	/* Dump the mutations to file */
	if (fff) mut_dump_file(fff);

	/* Close the file */
	my_fclose(fff);

	/* Display the file contents */
	show_file(TRUE, file_name, T("Mutations", "�����Ѱ�"), 0, 0);

	/* Remove the file */
	fd_kill(file_name);
}

void mut_dump_file(FILE* file)
{
	int i;
	variant desc;
	var_init(&desc);
	for (i = 0; i < MAX_MUTATIONS; ++i)
	{
		if (mut_present(i))
		{
			(_mutations[i].spell.fn)(SPELL_MUT_DESC, &desc);
			fprintf(file, "%s\n", var_get_string(&desc));
		}
	}
	var_clear(&desc);
}

bool mut_gain(int mut_idx)
{
	variant v;

	if (mut_idx < 0 || mut_idx >= MAX_MUTATIONS) return FALSE;
	if (mut_present(mut_idx)) return FALSE;
	
	var_init(&v);
	add_flag(p_ptr->muta, mut_idx);
	(_mutations[mut_idx].spell.fn)(SPELL_GAIN_MUT, &v);
	var_clear(&v);

	_mut_refresh();
	return TRUE;
}

static cptr _mut_name(menu_choices choices, int which) {
	static char buf[255];
	mut_name(((int*)choices)[which], buf);
	return buf;
}

int mut_gain_choice(mut_pred pred)
{
	int choices[MAX_MUTATIONS];
	int i;
	int ct = 0;
	menu_list_t list = { "Gain which mutation?", "Browse which mutation?", NULL,
						_mut_name, NULL, NULL, 
						choices, 0};

	for (i = 0; i < MAX_MUTATIONS; i++)
	{
		if (!mut_present(i))
		{
			if (pred == NULL || (pred(i)))
				choices[ct++] = i;
		}
	}

	if (ct == 0) return -1;

	list.count = ct;

	for (;;)
	{
		i = menu_choose(&list);
		if (i >= 0)
		{
			char buf[1024];
			char buf2[1024];
			int idx = choices[i];
			mut_name(idx, buf2);
			sprintf(buf, "You will gain %s.  Are you sure?", buf2);
			if (get_check(buf))
			{
				mut_gain(idx);
				return idx;
			}
		}
		msg_print("Please make a choice!");
	}

	return -1;
}

int mut_gain_random_aux(mut_pred pred)
{
	int prob[MAX_MUTATIONS];
	int i;
	int tot = 0;

	/* build probability table */
	for (i = 0; i < MAX_MUTATIONS; i++)
	{
		int cur = 0;
		if (!mut_present(i))
		{
			if (pred == NULL || (pred(i)))
				cur = _mut_prob_gain(i);
		}
		tot += cur;
		prob[i] = tot;
	}

	/* any left? */
	if (tot > 0)
	{
		int j = randint0(tot);
		
		/* scan the probability table for the correct mutation */
		for (i = 0; i < MAX_MUTATIONS; i++)
		{
			if (j < prob[i])
			{
				return i;
			}
		}
	}

	return -1;
}

bool mut_gain_random(mut_pred pred)
{
	int which = mut_gain_random_aux(pred);
	if (which >= 0 && !mut_present(which))
	{
		chg_virtue(V_CHANCE, 1);
		return mut_gain(which);
	}
	msg_print("You feel normal.");
	return FALSE;
}

int mut_get_powers(spell_info* spells, int max)
{
	int i;
	int ct = 0;
	
	for (i = 0; i < MAX_MUTATIONS; i++)
	{
		if ( mut_present(i)
		  && (_mutations[i].type & MUT_TYPE_ACTIVATION) )
		{
			spell_info *base = &_mutations[i].spell;
			spell_info* current = NULL;
			int stat_idx = p_ptr->stat_ind[_mutations[i].stat];

			if (ct >= max) break;

			current = &spells[ct];
			current->fn = base->fn;
			current->level = base->level;
			current->cost = base->cost;

			current->fail = calculate_fail_rate(base->level, base->fail, stat_idx);			
			ct++;
		}
	}
	return ct;
}

bool mut_good_pred(int mut_idx)
{
	if (mut_rating(mut_idx) < MUT_RATING_GOOD) return FALSE;
	return TRUE;
}

bool mut_human_pred(int mut_idx)
{
	switch (mut_idx)
	{
	case MUT_FAST_LEARNER:
	case MUT_WEAPON_SKILLS:
	case MUT_SUBTLE_CASTING:
	case MUT_PEERLESS_SNIPER:
	case MUT_UNYIELDING:
	case MUT_AMBIDEXTROUS:
	case MUT_UNTOUCHABLE:
	case MUT_LOREMASTER:
	case MUT_ARCANE_MASTERY:
	case MUT_EVASION:
	case MUT_POTION_CHUGGER:
	case MUT_ONE_WITH_MAGIC:
	case MUT_PEERLESS_TRACKER:
	case MUT_INFERNAL_DEAL:
		return TRUE;
	}
	return FALSE;
}

void mut_lock(int mut_idx)
{
	if (mut_idx < 0 || mut_idx >= MAX_MUTATIONS) return;
	if (mut_locked(mut_idx)) return;
	add_flag(p_ptr->muta_lock, mut_idx);
}

bool mut_locked(int mut_idx)
{
	if (mut_idx < 0 || mut_idx >= MAX_MUTATIONS) return FALSE;
	return have_flag(p_ptr->muta_lock, mut_idx);
}

bool mut_lose(int mut_idx)
{
	variant v;
	if (mut_idx < 0 || mut_idx >= MAX_MUTATIONS) return FALSE;
	if (!mut_present(mut_idx)) return FALSE;
	if (mut_locked(mut_idx)) return FALSE;
	
	var_init(&v);
	remove_flag(p_ptr->muta, mut_idx);
	(_mutations[mut_idx].spell.fn)(SPELL_LOSE_MUT, &v);
	var_clear(&v);

	_mut_refresh();
	return TRUE;
}

void mut_lose_all(void)
{
	if (mut_count(mut_unlocked_pred))
	{
		int i;
		chg_virtue(V_CHANCE, -5);
		msg_print(T("You are cured of all mutations.", "���Ƥ������Ѱۤ����ä���"));

		for (i = 0; i < MUT_FLAG_SIZE; ++i)
			p_ptr->muta[i] = p_ptr->muta_lock[i];

		_mut_refresh();
	}
}

bool mut_lose_random(mut_pred pred)
{
	int prob[MAX_MUTATIONS];
	int i;
	int tot = 0;

	/* build probability table */
	for (i = 0; i < MAX_MUTATIONS; i++)
	{
		int cur = 0;
		if (mut_present(i) && !mut_locked(i))
		{
			if (pred == NULL || (pred(i)))
				cur = _mut_prob_lose(i);
		}
		tot += cur;
		prob[i] = tot;
	}

	/* any left? */
	if (tot > 0)
	{
		int j = randint0(tot);
		int which = -1;
		
		/* scan the probability table for the correct mutation */
		for (i = 0; i < MAX_MUTATIONS; i++)
		{
			if (j < prob[i])
			{
				which = i;
				break;
			}
		}

		if (which >= 0 && mut_present(which)) /* paranoid checks ... should always pass */
		{
			return mut_lose(which);
		}
	}

	msg_print("You feel strange.");
	return FALSE;
}

void mut_name(int i, char* buf)
{
	variant v;
	var_init(&v);

	if (i >= 0 && i < MAX_MUTATIONS)
		(_mutations[i].spell.fn)(SPELL_NAME, &v);
	else
		var_set_string(&v, "None");

	sprintf(buf, "%s", var_get_string(&v));
	var_clear(&v);
}

bool mut_present(int mut_idx)
{
	if (mut_idx < 0 || mut_idx >= MAX_MUTATIONS) return FALSE;
	return have_flag(p_ptr->muta, mut_idx);
}

void mut_process(void)
{
	int i;
	variant v;

	/* No effect on monster arena */
	if (p_ptr->inside_battle) return;

	/* No effect on the global map */
	if (p_ptr->wild_mode) return;

	var_init(&v);

	for (i = 0; i < MAX_MUTATIONS; i++)
	{
		if ( mut_present(i)
		  && (_mutations[i].type & MUT_TYPE_EFFECT) )
		{
			(_mutations[i].spell.fn)(SPELL_PROCESS, &v);
		}
	}

	var_clear(&v);
}

int mut_rating(int mut_idx)
{
	if (mut_idx < 0 || mut_idx >= MAX_MUTATIONS) return MUT_RATING_AWFUL;
	return _mutations[mut_idx].rating;
}

int mut_regenerate_mod(void)
{
	int regen;
	int mod = 10;
	int count = mut_count(mut_unlocked_pred);

	/*
	 * Beastman get 10 "free" mutations and
	 * only 5% decrease per additional mutation
	 */

	if (p_ptr->prace == RACE_BEASTMAN)
	{
		count -= 10;
		mod = 5;
	}

	/* No negative modifier */
	if (count <= 0) return 100;

	regen = 100 - count * mod;

	/* Max. 90% decrease in regeneration speed */
	if (regen < 10) regen = 10;

	return regen;
}

int mut_type(int mut_idx)
{
	if (mut_idx < 0 || mut_idx >= MAX_MUTATIONS) return 0;
	return _mutations[mut_idx].type;
}

void mut_unlock(int mut_idx)
{
	if (mut_idx < 0 || mut_idx >= MAX_MUTATIONS) return;
	if (!mut_locked(mut_idx)) return;
	remove_flag(p_ptr->muta_lock, mut_idx);
}

bool mut_unlocked_pred(int mut_idx)
{
	return !mut_locked(mut_idx);
}
