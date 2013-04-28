
/* File: talents.c */

/*
 * Sangband talents.  Pseudo-probe, dodging, can forge, talent descriptions
 * and effects.
 *
 * Copyright (c) 2002
 * Leon Marrick, Julian Lighton, Michael Gorse, Chris Petit
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

#include "angband.h"




/*
 * If more than 18 talents are available, we print a second column.
 *
 * The actual number of lines used to display talents is three greater than
 * this: one for instructions and two more for spacing.
 *
 * If more than 36 talents become possible, this number will have to
 * increase.  If the description text starts to get squeezed out, your best
 * option will probably be to use 50-line mode.
 */
#define BREAK_NUM        18




/*
 * Warriors will eventually learn to pseudo-probe monsters.  This allows
 * them to better choose between slays and brands.  They select a target,
 * and receive (slightly incomplete) information about racial type,
 * basic resistances, and HPs.  -LM-
 */
void pseudo_probe(void)
{
	char m_name[80];

	/* Acquire the target monster */
	int m_idx = p_ptr->target_who;
	monster_type *m_ptr = &m_list[m_idx];
	monster_race *r_ptr = &r_info[m_ptr->r_idx];
	monster_lore *l_ptr = &l_list[m_ptr->r_idx];

	int approx_hp;


	/* Must not be hallucinating */
	if (p_ptr->image) return;

	/* If no target monster, fail. */
	if (p_ptr->target_who < 1)
	{
		msg_print("You must actually target a monster.");
		return;
	}

	else
	{
		/* Get "the monster" or "something" */
		monster_desc(m_name, m_ptr, 0x04);

		/* Approximate monster HPs */
		approx_hp = rand_spread(m_ptr->hp, m_ptr->hp / 4);

		/* Round the result */
		approx_hp = round_it(approx_hp, 8);

		/* Describe the monster */
		msg_format("%^s has about %d hit points.", m_name, approx_hp);

		/* Learn some flags.  Chance of omissions. */
		if ((r_ptr->flags3 & (RF3_ANIMAL)) && (!one_in_(20)))
			l_ptr->flags3 |= (RF3_ANIMAL);
		if ((r_ptr->flags3 & (RF3_EVIL)) && (!one_in_(10)))
			l_ptr->flags3 |= (RF3_EVIL);
		if ((r_ptr->flags3 & (RF3_UNDEAD)) && (!one_in_(20)))
			l_ptr->flags3 |= (RF3_UNDEAD);
		if ((r_ptr->flags3 & (RF3_DEMON)) && (!one_in_(20)))
			l_ptr->flags3 |= (RF3_DEMON);
		if ((r_ptr->flags3 & (RF3_ORC)) && (!one_in_(20)))
			l_ptr->flags3 |= (RF3_ORC);
		if ((r_ptr->flags3 & (RF3_TROLL)) && (!one_in_(20)))
			l_ptr->flags3 |= (RF3_TROLL);
		if ((r_ptr->flags3 & (RF3_GIANT)) && (!one_in_(10)))
			l_ptr->flags3 |= (RF3_GIANT);
		if ((r_ptr->flags3 & (RF3_DRAGON)) && (!one_in_(20)))
			l_ptr->flags3 |= (RF3_DRAGON);

		if ((r_ptr->flags3 & (RF3_HURT_FIRE)) && (!one_in_(5)))
			l_ptr->flags3 |= (RF3_HURT_FIRE);
		if ((r_ptr->flags3 & (RF3_HURT_COLD)) && (!one_in_(5)))
			l_ptr->flags3 |= (RF3_HURT_COLD);

		if ((r_ptr->flags3 & (RF3_IM_ACID)) && (!one_in_(5)))
			l_ptr->flags3 |= (RF3_IM_ACID);
		if ((r_ptr->flags3 & (RF3_IM_ELEC)) && (!one_in_(5)))
			l_ptr->flags3 |= (RF3_IM_ELEC);
		if ((r_ptr->flags3 & (RF3_IM_FIRE)) && (!one_in_(5)))
			l_ptr->flags3 |= (RF3_IM_FIRE);
		if ((r_ptr->flags3 & (RF3_IM_COLD)) && (!one_in_(5)))
			l_ptr->flags3 |= (RF3_IM_COLD);
		if ((r_ptr->flags3 & (RF3_IM_POIS)) && (!one_in_(5)))
			l_ptr->flags3 |= (RF3_IM_POIS);

		if ((r_ptr->flags3 & (RF3_RES_EDGED)) && (!one_in_(5)))
			l_ptr->flags3 |= (RF3_RES_EDGED);
		if ((r_ptr->flags3 & (RF3_IM_EDGED)) && (!one_in_(10)))
			l_ptr->flags3 |= (RF3_IM_EDGED);
		if ((r_ptr->flags3 & (RF3_RES_BLUNT)) && (!one_in_(5)))
			l_ptr->flags3 |= (RF3_RES_BLUNT);
		if ((r_ptr->flags3 & (RF3_IM_BLUNT)) && (!one_in_(10)))
			l_ptr->flags3 |= (RF3_IM_BLUNT);

		/* Get base name of monster */
		strcpy(m_name, (r_name + r_ptr->name));

		/* Pluralize it (unless unique) */
		if (!(r_ptr->flags1 & (RF1_UNIQUE)))
		{
			plural_aux(m_name);
		}

		/* Note that we learnt some new flags */
		msg_format("You feel you know more about %s.", m_name);

		/* Update monster recall window */
		if (p_ptr->monster_race_idx == m_ptr->r_idx)
		{
			/* Window stuff */
			p_ptr->window |= (PW_MONSTER);
		}
	}
}

/*
 * Maximal base dodging.
 */
#define DODGE_MAX    150

/*
 * Calculate the character's ability to dodge blows, missiles, and traps.
 * -LM-
 *
 * Accept maximum value for dodging.
 *
 * Dodging ability mostly depends on dodging skill, but the weight of worn
 * armour and of the backpack, and character DEX is also very important.
 * Characters will need to work on all of these factors in order to dodge
 * effectively.
 *
 * It's hard to dodge with a big, heavy shield.  The weight of a wielded
 * shield is multiplied by at least 3 for these calculations.
 *
 * Special bonuses can make this function return a value higher than "max".
 */
int dodging_ability(int max)
{
	int dexterity_factor, encumbrance;
	int burden = 0, limit;
	int raw_dodging;

	/* Get the dodging skill (between 0 and 100) */
	int skill = get_skill(S_DODGING, 0, 100);

	/* Base dodging */
	int dodging_skill = skill;


	/* Sometimes we cannot dodge at all */
	if ((p_ptr->blind) || (p_ptr->confused) || (p_ptr->paralyzed))
	{
		/* No dodging, unless skill is maximal, and not paralyzed */
		if ((skill < 100) || (p_ptr->paralyzed)) return (0);
	}

	/* Hallucinating characters are not as good at dodging */
	if (p_ptr->image)
	{
		if (skill < 95) dodging_skill /= 2;
	}

	/* Stunned characters are not as good at dodging */
	if (p_ptr->stun)
	{
		if (skill < 98)
		{
			dodging_skill -= p_ptr->stun * dodging_skill / 100;
			if (dodging_skill < 0) dodging_skill = 0;
		}
	}

	/* Get dexterity factor (usually between 0 and 45) */
	dexterity_factor = MAX(0, 3 * (p_ptr->stat_ind[A_DEX] - 3) / 2);

	/* It is hard to dodge with a shield, especially a heavy one. */
	if (TRUE)
	{
		object_type *o_ptr = &inventory[INVEN_ARM];

		/* Wearing a shield */
		if (o_ptr->tval == TV_SHIELD)
		{
			/* (shield weight is also counted in total weight) */
			burden = o_ptr->weight * (7 - (skill / 20));
		}
	}

	/* Add carried weight to shield weight */
	burden += p_ptr->total_weight;

	/* Calculate encumbrance (usu. between 20 and 75) */
	encumbrance = burden / adj_str_wgt[p_ptr->stat_ind[A_STR]];

	/* Increases in skill raise the amount you can carry without penalty */
	limit = 33 + skill / 4;

	/* No penalty at "limit" or below.  Triple "limit" means no dodging. */
	burden = (50 * encumbrance / limit) - limit;

	/* Set bounds */
	if (burden > 100) burden = 100;
	if (burden <   0) burden =   0;


	/* Dodging depends mostly on skill and dex (ranges from 0 to ~145) */
	raw_dodging = dodging_skill + dexterity_factor;

	/* Penalize inattention to dexterity or weight */
	raw_dodging = MIN(dexterity_factor * 4, raw_dodging);
	raw_dodging = ((raw_dodging * (100 - burden)) + 50) / 100;

	/* Set limit on basic dodging ability */
	if (raw_dodging > DODGE_MAX) raw_dodging = DODGE_MAX;


	/* Allow magical enhancements */
	if (p_ptr->evasion)
	{
		raw_dodging += get_skill(S_WIZARDRY, 30, 60);
	}

	/* It is much easier for Burglars to dodge in the dark */
	if ((no_light()) && (get_skill(S_BURGLARY, 0, 100)))
	{
		/* Calculate maximum possible bonus */
		int dark_factor = get_skill(S_BURGLARY, 5, 55) + (raw_dodging / 4);

		/* Need darkness both here and adjacent to get full benefit */
		raw_dodging += dark_factor * darkness_ratio(1) / 100;
	}

	/* It is hard to dodge in water */
	if (cave_feat[p_ptr->py][p_ptr->px] == FEAT_WATER)
	{
		raw_dodging /= 3;
	}

	/* Translate raw dodging into a proportion of the (base) maximum */
	return ((s16b)(raw_dodging * max / DODGE_MAX));
}


/*
 * Determine if a precognition message will be displayed.
 */
bool can_precog(int max_chance, int cutoff)
{
	int skill = get_skill(S_PERCEPTION, 0, 100);
	int chance;

	/* No chance if below the cutoff, or if cutoff is 100 */
	if (skill < cutoff) return (FALSE);
	if (cutoff == 100) return (FALSE);

	/* Otherwise, chance is always at least 20% */
	chance = 20;

	/*
	 * The remaining chance between 20 and max_chance depends on the
	 * skill level above the cutoff.  At a skill of 100, chance is
	 * max_chance.
	 */
	chance += div_round((s32b)((skill - cutoff) * (max_chance - 20)),
	                    (100L - cutoff));

	/* Can get a message */
	if (randint(100) <= chance) return (TRUE);

	/* No message */
	return (FALSE);
}



/*
 * Check to see if conditions are right to create objects.
 */
static bool good_work_cond(bool msg, bool must_be_in_town)
{
	if ((must_be_in_town) && (p_ptr->depth != 0) && (p_ptr->character_type != PCHAR_IRONMAN))
	{
		if (msg) msg_print("You may only create items in the town.");
		return (FALSE);
	}
	if (p_ptr->confused > 0)
	{
		if (msg) msg_print("You are too confused.");
		return (FALSE);
	}
	if (p_ptr->image > 0)
	{
		if (msg) msg_print("You are hallucinating!");
		return (FALSE);
	}
	if (p_ptr->berserk > 0)
	{
		if (msg) msg_print("You are in a berserk rage.");
		return (FALSE);
	}
	if (p_ptr->berserk > 0)
	{
		if (msg) msg_print("You are in a black rage.");
		return (FALSE);
	}
	if (!player_can_see_bold(p_ptr->py, p_ptr->px))
	{
		if (msg) msg_print("You have no light to work by.");
		return (FALSE);
	}
	if (p_ptr->blind)
	{
		if (msg) msg_print("You can't see.");
		return (FALSE);
	}

	return (TRUE);
}

/*
 * Hook for "get_item()".  Determine if something has charges.
 */
static bool item_tester_unknown_charges(const object_type *o_ptr)
{
	/* Object must be unknown */
	if (object_known_p(o_ptr)) return (FALSE);

	/* Recharge staffs */
	if (o_ptr->tval == TV_STAFF) return (TRUE);

	/* Recharge wands */
	if (o_ptr->tval == TV_WAND) return (TRUE);

	/* Nope */
	return (FALSE);
}


/*
 * Use, get information about, provide a description of, or determine
 * whether the character can use a talent.
 *
 * Return "B" if a talent can be browsed, and "Y" if it can also be used.
 */
static cptr do_talent(int talent, int mode)
{
	bool can_use = TRUE;

	/* Function modes */
	bool use   = (mode == TALENT_USE);
	bool info  = (mode == TALENT_INFO);
	bool desc  = (mode == TALENT_DESC);
	bool check = (mode == TALENT_CHECK);

	/* Usage variables */
	int skill;
	int reliability = 0;

	/* Talent-specific variables */
	int dam1, dam2;
	int dur1, dur2;
	int pow, pow1, pow2;
	int dir;

	talent_type *t_ptr;


	/* Paranoia -- require a talent */
	if (talent < 0) return ("N");

	/* Point to this talent */
	t_ptr = &talent_info[talent];

	/* Error-checking -- require a name */
	if (!t_ptr->name) return ("N");


	/* Talent is still timed out */
	if (((use) || (check)) && (p_ptr->ptalents[talent].count))
	{
		if (use) return ("");
		else can_use = FALSE;
	}

	/* Get skill (note that there may not always be one) */
	skill = get_skill(t_ptr->skill, 0, 100);


	/* Most talents depend on one skill */
	if (t_ptr->skill < NUM_SKILLS)
	{
		reliability = skill - t_ptr->min_level;

		/* Note that skill is not great enough to use this talent */
		if (((use) || (check)) && (reliability < 0)) return ("N");
	}


	/* Handle talents */
	switch (talent)
	{
		case TALENT_STONESKIN:
		{
			dur1 = 5 + reliability / 2;     dur2 = 15 + reliability / 2;

			if (info) return (format("dur %d-%d", dur1, dur2));
			if (desc) return ("Turn your skin to stone.");
			if (check)
			{
				/* Require the Oath of Iron */
				if (!(p_ptr->oath & (OATH_OF_IRON))) return ("N");

				/* Must not be confused */
				else if ((p_ptr->confused) || (p_ptr->image)) return ("B");
			}
			if (use)
			{
				(void)set_steelskin(p_ptr->steelskin + rand_range(dur1, dur2),
					"Your skin turns to stone!");
			}
			break;
		}
		case TALENT_BERSERK:
		{
			dur1 = 10 + BERSERK_WEAKNESS_LENGTH + reliability;
			dur2 = 20 + BERSERK_WEAKNESS_LENGTH + reliability;

			if (info) return (format("dur %d-%d", dur1, dur2));
			if (desc) return ("Drive yourself into a berserk rage.");
			if (check)
			{
				/* Require the Oath of Iron */
				if (!(p_ptr->oath & (OATH_OF_IRON))) return ("N");

				/* Must not be confused */
				else if ((p_ptr->confused) || (p_ptr->image)) return ("B");
			}
			if (use)
			{
				(void)set_berserk(p_ptr->berserk + rand_range(dur1, dur2));
			}
			break;
		}
		case TALENT_RESIST_DAM:
		{
			dur1 = 5 + reliability;     dur2 = 10 + reliability;

			if (info) return (format("dur %d-%d", dur1, dur2));
			if (desc) return ("Resist raw damage.");
			if (check)
			{
				/* Require the Oath of Iron */
				if (!(p_ptr->oath & (OATH_OF_IRON))) return ("N");

				/* Must not be confused */
				else if ((p_ptr->confused) || (p_ptr->image)) return ("B");
			}
			if (use)
			{
				(void)set_res_dam(p_ptr->res_dam + rand_range(dur1, dur2));
			}
			break;
		}
		case TALENT_PROBING:
		{
			if (info) return ("");
			if (desc) return ("Reveal a targeted monster's race, approximate HPs, and basic resistances.  Be warned:  the information given is not always complete...");
			if (check)
			{
				/* Require the Oath of Iron */
				if (!(p_ptr->oath & (OATH_OF_IRON))) return ("N");

				/* Must not be confused */
				else if ((p_ptr->confused) || (p_ptr->image)) return ("B");
			}
			if (use)
			{
				/* Get a target */
				msg_print("Target a monster to probe.");
				if (!get_aim_dir(&dir)) return ("");

				/* Low-level probe spell, or cancel */
				if (dir == 5) pseudo_probe();
				else
				{
					msg_print("You must actually target a monster.");
					use = FALSE;
				}
			}
			break;
		}

		case TALENT_DET_MAGIC:
		{
			if (info) return ("");
			if (desc) return ("Detect nearby magical items.");
			if (check)
			{
				/* Wizards have a spell */
				if (p_ptr->realm == MAGE) return ("N");

				/* Must not be confused or berserk */
				if ((p_ptr->confused) || (p_ptr->image) || (p_ptr->berserk))
					return ("B");
			}
			if (use)
			{
				(void)detect_objects_magic(FALSE);
			}
			break;
		}
		case TALENT_PHASE_WARP:
		{
			pow1 = 9 + (reliability / 5);
			pow2 = 9 - (reliability / 5);

			if (info) return (format("rng %d var %d", pow1, pow2));
			if (desc) return ("Semi-controlled teleportation.");
			if (check)
			{
				/* Wizards have a spell */
				if (p_ptr->realm == MAGE) return ("N");

				/* Must not be confused or berserk */
				if ((p_ptr->confused) || (p_ptr->image) || (p_ptr->berserk))
					return ("B");
			}
			if (use)
			{
				if (!phase_warp(pow1, pow2, FALSE)) use = FALSE;
			}
			break;
		}
		case TAP_ENERGY:
		{
			if (info) return ("");
			if (desc) return ("Turn rod, wand, or staff energy into mana.  The higher-level the item, and the more charges it has, the more magical energy it provides.  Rods have little usable energy, and staffs quite a bit.");
			if (check)
			{
				/* Wizards have a spell */
				if (p_ptr->realm == MAGE) return ("N");

				/* Must not be confused or berserk */
				if ((p_ptr->confused) || (p_ptr->image) || (p_ptr->berserk))
					return ("B");
			}
			if (use)
			{
				if (!tap_magical_energy()) use = FALSE;
			}
			break;
		}

		case TALENT_DET_EVIL:
		{
			if (info) return ("");
			if (desc) return ("Detect all nearby evil monsters, even invisible ones.");
			if (check)
			{
				/* Priests have a prayer */
				if (p_ptr->realm == PRIEST) return ("N");

				/* Must not be confused or berserk */
				if ((p_ptr->confused) || (p_ptr->image) || (p_ptr->berserk))
					return ("B");
			}
			if (use)
			{
				/* Never get extended range */
				(void)detect_evil(FALSE, TRUE);
			}
			break;
		}
		case TALENT_RESTORATION:
		{
			if (info) return ("");
			if (desc) return ("Restore all stats.");
			if (check)
			{
				/* Priests have a prayer */
				if (p_ptr->realm == PRIEST) return ("N");

				/* Must not be confused or berserk */
				if ((p_ptr->confused) || (p_ptr->image) || (p_ptr->berserk))
					return ("B");
			}
			if (use)
			{
				(void)restore_stats();
			}
			break;
		}
		case TALENT_DET_ANIMAL:
		{
			if (info) return ("");
			if (desc) return ("Detect all nearby natural creatures.  With high enough skill, the range increases.");
			if (check)
			{
				/* Druids have a technique */
				if (p_ptr->realm == DRUID) return ("N");

				/* Must not be confused or berserk */
				if ((p_ptr->confused) || (p_ptr->image) || (p_ptr->berserk))
					return ("B");
			}
			if (use)
			{
				/* Extended range with reliability >= 45 */
				(void)detect_animals(reliability >= 45, TRUE);
			}
			break;
		}
		case TALENT_MEND_WOUNDS:
		{
			pow = (skill > 85) ? (3 * (skill - 85) / 2) : 0;
			pow1 = skill / 8;
			pow2 = skill / 4;

			if (info)
			{
				if (pow) return (format("heal %d, cure", pow));
				else     return ("");
			}
			if (desc) return ("Mend wounds, cure stunning, heal blindness, reduce poison and disease.  Serious ailments will require more than one use of this talent.  If you are very skilled, you can also heal yourself rapidly.");
			if (check)
			{
				/* Druids have a technique */
				if (p_ptr->realm == DRUID) return ("N");

				/* Must not be confused or berserk */
				if ((p_ptr->confused) || (p_ptr->image) || (p_ptr->berserk))
					return ("B");
			}
			if (use)
			{
				hp_player(pow);

				(void)set_cut(p_ptr->cut - pow2);
				(void)set_stun(p_ptr->stun - pow1);
				(void)set_blind(p_ptr->blind - pow1, NULL);
				(void)set_poisoned(p_ptr->poisoned - pow2);
				(void)set_diseased(p_ptr->diseased - 5, NULL);
			}
			break;
		}

		case TALENT_DET_UNDEAD:
		{
			if (info) return ("");
			if (desc) return ("Detect all nearby undead monsters, visible or invisible.  With high enough skill, the range increases.");
			if (check)
			{
				/* Necromancers have a ritual */
				if (p_ptr->realm == NECRO) return ("N");

				/* Must not be confused or berserk */
				if ((p_ptr->confused) || (p_ptr->image) || (p_ptr->berserk))
					return ("B");
			}
			if (use)
			{
				/* Extended range with reliability >= 45 */
				(void)detect_undead(reliability >= 45, TRUE);
			}
			break;
		}
		case TALENT_REMEMBRANCE:
		{
			if (info) return ("");
			if (desc) return ("Restore experience level.");
			if (check)
			{
				/* Must not be confused or berserk */
				if ((p_ptr->confused) || (p_ptr->image) || (p_ptr->berserk))
					return ("B");
			}
			if (use)
			{
				(void)restore_level();
			}
			break;
		}

		case TALENT_ID_CHARGES:
		{
			if (info) return ("");
			if (desc) return ("Determine the charges on a wand or staff.");
			if (check)
			{
				/* Must not be confused or berserk */
				if ((p_ptr->confused) || (p_ptr->image) || (p_ptr->berserk))
					return ("B");
			}
			if (use)
			{
				item_tester_hook = item_tester_unknown_charges;
				if (!ident_spell()) return (NULL);
			}
			break;
		}

		case TALENT_D_OR_NAB_OBJECT:
		{
			pow = get_skill(t_ptr->skill, 0, 300);

			if (info) return ("");   /* Deliberate -- no weight data */
			if (desc) return ("Detect objects and gold (either in the current room or in line of sight), or Nab an object (must be in line of sight and not too heavy).");
			if (check)
			{
				/* Must not be confused, blind, or berserk */
				if ((p_ptr->confused) || (p_ptr->image) || (p_ptr->blind) ||
				    (p_ptr->berserk))
				{
					return ("B");
				}
			}
			if (use)
			{
				char c;

				/* Repeat until something happens, or user cancels */
				while (TRUE)
				{
					/* Get a character, or abort */
					if (!get_com("Detect objects in current room, or Nab an object? (d, n):", &c)) return (NULL);

					/* Detect */
					if ((c == 'D') || (c == 'd'))
					{
						(void)detect_objects_in_room(p_ptr->py, p_ptr->px);
						break;
					}

					/* Nab */
					else if ((c == 'N') || (c == 'n'))
					{
						if (!get_aim_dir(&dir)) return (NULL);
						(void)fetch_obj(dir, pow);
						break;
					}
				}
			}
			break;
		}
		case TALENT_POISON_AMMO:
		{
			pow1 = get_skill(t_ptr->skill, 0, 4);
			pow2 = get_skill(t_ptr->skill, 0, 12);

			if (info) return (format("poison %d-%d", pow1, pow2));
			if (desc) return ("Poison some ammo.  Mushroom of Poison or (especially) of Envenomation are the best poisoners, but some other kinds of mushrooms and Potions of Poison also work.  Missiles must be ordinary -- no slays or brands.");
			if (check)
			{
				/* Must not be confused, blind, or berserk */
				if ((p_ptr->confused) || (p_ptr->image) || (p_ptr->blind) ||
				    (p_ptr->berserk))
				{
					return ("B");
				}
			}
			if (use)
			{
				poison_ammo(pow2);
			}
			break;
		}
		case TALENT_HIT_AND_RUN:
		{
			if (info) return ("");
			if (desc) return ("The next time you hit or steal from a monster, you will execute a phase door.");
			if (check)
			{
				/* Must not be confused, blind, or berserk */
				if ((p_ptr->confused) || (p_ptr->image) || (p_ptr->blind) ||
				    (p_ptr->berserk))
				{
					return ("B");
				}
			}
			if (use)
			{
				msg_print("You prepare to hit and run.");
				p_ptr->special_attack |= (ATTACK_FLEE);
			}
			break;
		}
		case TALENT_PRED_WEATH:
		{
			/* Accuracy depends on skill and depth */
			int accur = get_skill(S_NATURE, 40, 200) - MIN(100, p_ptr->depth);
			if (accur > 100) accur = 100;

			if (info) return (format("accuracy: %d%%", accur));
			if (desc) return ("Predict the weather.  Becomes more reliable as Nature Lore skill improves, and less reliable the deeper you descend.");
			if (check)
			{
				/* Druids are the only ones interested in weather */
				if (p_ptr->realm != DRUID) return ("N");

				/* Must not be confused or berserk */
				if ((p_ptr->confused) || (p_ptr->image) || (p_ptr->berserk))
					return ("B");
			}
			if (use)
			{
				predict_weather(accur);
			}
			break;
		}
		case TALENT_SNEAKING:
		{
			if (info) return ("");
			if (desc) return (format("Sneak up on monsters (or stop sneaking).  Sneaking slows you down, but makes you a lot more stealthy and allows you to get in deadly sneak attacks.  You may also use the '%c' command to activate this talent.", rogue_like_commands ? '#' : 'S'));
			if (check)
			{
				/* Must not be confused, aggravating, or berserk */
				if ((p_ptr->confused) || (p_ptr->image) || (p_ptr->aggravate) ||
				    (p_ptr->berserk) || (p_ptr->necro_rage))
				{
					return ("B");
				}
			}
			if (use)
			{
				do_cmd_sneaking();
			}
			break;
		}
		case TALENT_SENSE_AREA:
		{
			if (info) return ("");
			if (desc) return ("Map the local area.  Extended range with a skill of 75.");
			if (check)
			{
				/* Must not be confused or berserk */
				if ((p_ptr->confused) || (p_ptr->image) || (p_ptr->berserk) ||
				    (p_ptr->necro_rage))
				{
					return ("B");
				}
			}
			if (use)
			{
				(void)map_area(0, 0, (reliability >= 20));
			}
			break;
		}
		case TALENT_SUPERSTEALTH:
		{
			dur1 = (p_ptr->tim_invis ?  1 : 30);
			dur2 = (p_ptr->tim_invis ? 25 : 50);

			if (info) return (format("dur %d-%d", dur1, dur2));
			if (desc) return ("Become partially invisible.");
			if (check)
			{
				/* Must not be confused, aggravating, or berserk */
				if ((p_ptr->confused) || (p_ptr->image) || (p_ptr->aggravate) ||
				    (p_ptr->berserk))
				{
					return ("B");
				}
			}
			if (use)
			{
				(void)set_invis(p_ptr->tim_invis + rand_range(dur1, dur2),
				                get_skill(S_STEALTH, 20, 40));
			}
			break;
		}
		case TALENT_RECHARGING:
		{
			if (info) return ("");     /* Deliberate lack of information */
			if (desc) return ("Recharge magical devices using essences.  Becomes much safer and more powerful with further increases in Infusion.");
			if (check)
			{
				/* Recharging talent requires decent device skill */
				if ((talent == TALENT_RECHARGING) &&
					(get_skill(S_DEVICE, 0, 100) < LEV_REQ_RECHARGE))
				{
					return ("N");
				}

				/* Must not be confused or berserk */
				if ((p_ptr->confused) || (p_ptr->image) || (p_ptr->berserk))
					return ("B");
			}
			if (use)
			{
				/* Essence-based recharging */
				use = recharge(get_skill(S_INFUSION, 80, 230), TRUE);
			}

			break;
		}
		case TALENT_WEAPON_SMITH:
		{
			if (info) return ("");
			if (desc) return (format("Create hand weapons.  %sYou need components (hunks of metal) and knowledge of the kind of weapon being created.  Essences are very handy if you want to add specific magical powers.  Increasing the weaponsmithing skill allows you to forge better items, and increasing the infusion skill allows you to add more powers using essences.", (p_ptr->character_type == PCHAR_IRONMAN) ? "" : "Only works in town.  "));
			if (check)
			{
				/* Must satisfy a number of conditions */
				if (!good_work_cond(FALSE, TRUE)) return ("B");
			}
			if (use)
			{
				/* Make a melee weapon */
				if (good_work_cond(TRUE, TRUE))
				{
					p_ptr->get_help_index = HELP_FORGING;

					use = make_melee_weapon();
				}
			}
			break;
		}
		case TALENT_ARMOUR_SMITH:
		{
			if (info) return ("");
			if (desc) return (format("Create armour.  %sYou need components (hunks of metal) and knowledge of the kind of armour being created.  Essences are very handy if you want to add specific magical powers.  Increasing the armour forging skill allows you to forge better items, and increasing the infusion skill allows you to add more powers using essences.", (p_ptr->character_type == PCHAR_IRONMAN) ? "" : "Only works in town.  "));
			if (check)
			{
				/* Must satisfy a number of conditions */
				if (!good_work_cond(FALSE, TRUE)) return ("B");
			}
			if (use)
			{
				/* Make armour */
				if (good_work_cond(TRUE, TRUE))
				{
					p_ptr->get_help_index = HELP_FORGING;

					use = make_armour();
				}
			}
			break;
		}
		case TALENT_MISSILE_SMITH:
		{
			if (info) return ("");
			if (desc) return (format("Create missile weapons and ammunition.  %sYou need components (hunks of metal) and knowledge of the kind of item being created.  Essences are very handy if you want to add specific magical powers.  Increasing the bowmaking skill allows you to forge better items in greater quantity.  Increasing the infusion skill lets you add more powers using essences, to larger numbers of objects.", (p_ptr->character_type == PCHAR_IRONMAN) ? "" : "Only works in town.  "));
			if (check)
			{
				/* Must satisfy a number of conditions */
				if (!good_work_cond(FALSE, TRUE)) return ("B");
			}
			if (use)
			{
				/* Make a missile launcher or some ammunition */
				if (good_work_cond(TRUE, TRUE))
				{
					p_ptr->get_help_index = HELP_FORGING;

					use = make_launcher_or_ammo();
				}
			}
			break;
		}
		case TALENT_ALCHEMY:
		{
			if (info) return ("");
			if (desc) return ("Create potions, scrolls, and (with higher skill) rings and amulets.  Works in town or in the dungeon.  You need empty bottles or parchments, knowledge of the item being created (for scrolls and potions), and essences.");
			if (check)
			{
				/* Must satisfy a number of conditions */
				if (!good_work_cond(FALSE, FALSE)) return ("B");
			}
			if (use)
			{
				/* Perform alchemy */
				if (good_work_cond(TRUE, FALSE))
				{
					p_ptr->get_help_index = HELP_FORGING;

					use = do_alchemy();
				}
			}
			break;
		}
		case TALENT_SAVE_BOTTLE:
		{
			if (info) return ("");
			if (desc) return ("Save or stop saving empty bottles and blank parchments.  Useful when your inventory space runs low, or you need to start accumulating more supplies.");
			if (use)
			{
				if (p_ptr->suppress_bottle)
				{
					msg_print("You are now saving parchments and bottles.");
					p_ptr->suppress_bottle = FALSE;
				}
				else
				{
					msg_print("You are no longer saving parchments and bottles.");
					p_ptr->suppress_bottle = TRUE;
				}

				/* Use no time */
				use = FALSE;
			}
			break;
		}

		case TALENT_DRAGON_BREATHING:
		{
			dam1 = (3 * (p_ptr->power - 10)) - 25;
			dam2 = (3 * (p_ptr->power - 10)) + 25;

			if (info) return (format("dam %d-%d", dam1, dam2));
			if (desc) return ("When in dragonform, you can use essences to fuel powerful breaths.  You get bonuses to damage for the basic elements and poison, and penalties for less resistible breaths.");
			if (check)
			{
				/* Must have enough innate power */
				if (p_ptr->power < 30) return ("N");

				/* Must be in dragonform */
				if (p_ptr->schange != SHAPE_DRAGON) return ("N");

				/* Must not be berserk or raging */
				if ((p_ptr->berserk) || (p_ptr->necro_rage)) return ("B");
			}
			if (use)
			{
				int typ, spread;
				int adjust;
				int sval = -1;
				long tmp, dam;

				int k_idx;
				object_kind *k_ptr;
				cptr str;


				/* Get an essence to use, calculate breath type and strength */
				typ = essence_to_magic(&adjust, &sval);

				/* Cancelled */
				if (typ == -1) return ("");

				if (!get_aim_dir(&dir)) return ("");

				/* Look up the index number of this essence */
				k_idx = lookup_kind(TV_ESSENCE, sval);

				/* Point to the essence kind */
				k_ptr = &k_info[k_idx];

				/* Refuse to breathe a nameless essence */
				if (!k_name) return ("");

				/* Get base name */
				str = (k_name + k_ptr->name);

				/* Eat the essence */
				p_ptr->essence[sval]--;

				/* Message */
				msg_format("You breathe %s.", str);

				/* Determine how quickly the breath spreads out */
				if      (typ == GF_POIS)  spread = 90;
				else if (typ == GF_SOUND) spread = 75;
				else if (typ == GF_FORCE) spread = 60;
				else                      spread = 45;

				/* Determine damage */
				dam = rand_range(dam1, dam2);

				/* Apply adjustment to damage */
				tmp = dam * adjust / 100;
				dam = (s16b)tmp;

				/* Breathe. */
				fire_arc(typ, dir, dam, 3 + p_ptr->power / 10, spread);
			}
			break;
		}

		default:
		{
			if (info) return ("???");
			if (desc) return ("This talent has not yet been defined.");
			if (check) return ("N");
			if (use)
			{
				msg_print("This talent has not yet been defined.");
			}
			break;
		}
	}



	/* Used a talent */
	if (use)
	{
		/* Time out the talent */
		p_ptr->ptalents[talent].count = t_ptr->timeout;

		/* Use a turn */
		p_ptr->energy_use = 100;
	}

	/* When checking, report if talent is timed out or not */
	return (can_use ? "Y" : "B");
}


/*
 * Return whether we can use or browse a talent now.
 */
int can_use_talent(int talent)
{
	cptr okay = do_talent(talent, TALENT_CHECK);

	if (strstr(okay, "B")) return (0);
	if (strstr(okay, "Y")) return (1);

	return (-1);
}


/*
 * Print a list of talents.
 */
static void print_talents(const s16b *talents)
{
	int i, tmp;
	int attr;

	const talent_type *t_ptr;

	char out_val[160];

	/* Start at row 2, column 0 */
	int row = 2;
	int col = 0;

	/* Dump the talents */
	for (tmp = 0, i = 0; i < NUM_TALENTS; i++)
	{
		/* Get the talent */
		t_ptr = &talent_info[i];

		/* Talent is not available */
		if (talents[i] < 0) continue;

		/* New talent */
		tmp++;

		/* Shift to next column */
		if (tmp == BREAK_NUM)
		{
			row = 2;
			col = 40;
		}

		/* Print the talent index */
		c_put_str((p_ptr->ptalents[i].marked ? TERM_YELLOW : TERM_WHITE),
			format("%c) ", t_ptr->index), row, col);

		/* Build the talent index, name, and information */
		sprintf(out_val, "%-22s %-12s", t_ptr->name,
			do_talent(i, TALENT_INFO));

		/* Talents that cannot be used are printed in gray */
		if (talents[i] > 0) attr = TERM_WHITE;
		else                attr = TERM_SLATE;

		/* Print the talent */
		c_prt(attr, out_val, row, col + 3);

		/* Go to next row */
		row++;
	}
}


/*
 * Given an index, get the first legal talent that uses it.
 */
static int get_talent_from_index(char ch)
{
	int i;

	for (i = 0; i < NUM_TALENTS; i++)
	{
		if (can_use_talent(i) < 0) continue;

		if (talent_info[i].index == ch) return (i);
	}

	/* No legal talent */
	return (-1);
}



/*
 * Use or browse talents.
 *
 * Allow instant selection of talents, selection from a menu, and browsing.
 * Interface is a mix of the skills and the spellcasting interfaces.  I
 * admit that the code is a tad clunky.
 *
 * We must be careful to control access to talents:  some can be used,
 * others browsed but not used, and others neither browsed nor used.
 */
void do_cmd_talents(void)
{
	int i;
	int talent = -2;

	int old_rows = screen_rows;

	int selected_talent = -1;

	bool show_list = always_show_list;

	/* In use mode, no redraw */
	byte mode = 0;
	bool redraw = FALSE;

	/* Not using a talent yet */
	bool use_talent = FALSE;

	/* List of available talents */
	s16b talent_avail[NUM_TALENTS];

	talent_type *t_ptr;

	char first_index = '\0';
	char last_index  = '\0';

	char out_val[160];
	char p1[80];

	int num_browse = 0;
	int num_use = 0;
	char choice;


	/* Determine whether talents can be browsed and/or used  */
	for (i = 0; i < NUM_TALENTS; i++)
	{
		/* Get this talent */
		t_ptr = &talent_info[i];

		/* Note unused talents */
		if (!t_ptr->name) talent_avail[i] = -1;

		/* Check whether we can browse or use this talent */
		talent_avail[i] = can_use_talent(i);

		/* Note a talent that can be at least browsed */
		if (talent_avail[i] >= 0)
		{
			if (selected_talent < 0) selected_talent = i;
			num_browse++;
		}

		/* Note a talent that can be used */
		if (talent_avail[i] >= 1)
		{
			/* Get indexes */
			if (first_index == '\0') first_index = t_ptr->index;
			last_index = t_ptr->index;

			num_use++;
		}
	}

	/* No talents at all */
	if (!num_browse)
	{
		msg_print("You have no talents yet.");
		return;
	}

	/* Cannot use any talents now -- go into browse mode */
	if (!num_use)
	{
		mode = 1;
	}

	/* Fill in basic prompt */
	strcpy(p1, "'?' for help, ESC to exit)?");


	/* Get a talent from the user */
	while (TRUE)
	{
		/* In "use" mode */
		if (mode == 0)
		{
			/* Build a prompt */
			(void)strnfmt(out_val, 78, "Use (Talents %c-%c, ! to mark, * to browse, %s",
				first_index, last_index, p1);
		}

		/* In "browse" mode */
		else if (mode == 1)
		{
			/* Build a prompt */
			(void)strnfmt(out_val, 78, "Browse (Talents %c-%c, ! to mark, * to use, %s",
				first_index, last_index, p1);
		}

		/* In "mark" mode */
		else
		{
			/* Build a prompt */
			(void)strnfmt(out_val, 78, "Mark (Talents %c-%c, * to browse, %s",
				first_index, last_index, p1);
		}

		/* Hack -- force the display of a list */
		if (show_list)
		{
			choice = ' ';
			show_list = FALSE;
		}

		/* Get a command, allow cancel */
		else
		{
			if (!get_com(out_val, &choice)) break;
		}

		/* Clear the "message" line (only when a list is being shown) */
		if (redraw) clear_row(1);


		/* Show a menu */
		if (choice == ' ')
		{
			/* Only show the menu, don't hide it  XXX XXX */
			if (!redraw)
			{
				/* Show list */
				redraw = TRUE;

				/* Save screen */
				screen_save();

				/* Clear screen */
				(void)Term_clear();

				/* Set to 25 screen rows */
				(void)Term_rows(FALSE);

				/* List the talents */
				print_talents(talent_avail);
			}
		}

		/* Help me. */
		else if (choice == '?')
		{
			p_ptr->get_help_index = HELP_TALENTS;
			do_cmd_help();

			/* Redraw the talents */
			(void)Term_clear();
			print_talents(talent_avail);
		}


		/* Go into mark mode */
		else if (choice == '!')
		{
			if (redraw) prt("A marked talent will let you know when you can use it again.", 1, 10);

			/* Hack -- require a list */
			else show_list = TRUE;
			mode = 2;

			/* No talent selected */
			selected_talent = -1;
			continue;
		}

		/* Toggle browse and use modes */
		else if (choice == '*')
		{
			if (mode == 1) mode = 0;
			else           mode = 1;

			/* Hack -- browse mode requires a list */
			if ((mode == 1) && (!redraw)) show_list = TRUE;
		}

		/* (Try to) Get talent using index */
		else talent = get_talent_from_index(choice);

		/* Talent is legal */
		if ((talent >= 0) && (talent_avail[talent] >= 0))
		{
			/* In mark mode -- mark or unmark the talent */
			if (mode == 2)
			{
				if (p_ptr->ptalents[talent].marked)
				{
					p_ptr->ptalents[talent].marked = FALSE;

					center_string(out_val, sizeof(out_val), format("You have unmarked \"%s\".",
						talent_info[talent].name), 78);

					prt(out_val, 1, 1);

					print_talents(talent_avail);
				}
				else
				{
					p_ptr->ptalents[talent].marked = TRUE;

					center_string(out_val, sizeof(out_val), format("You have marked \"%s\".",
						talent_info[talent].name), 80);

					prt(out_val, 1, 0);

					print_talents(talent_avail);
				}
			}

			/* In browse mode, or talent is only available for browsing */
			else if ((mode == 1) || (talent_avail[talent] == 0))
			{
				/* We are looking at the list */
				if (redraw)
				{
					/* Clear some space */
					clear_from(MIN(num_browse, BREAK_NUM) + 2);

					/* Move cursor */
					move_cursor(MIN(num_browse, BREAK_NUM) + 2, 7);

					/* Print talent description */
					c_roff(TERM_L_BLUE, format("%s",
						do_talent(talent, TALENT_DESC)), 2, 78);
				}

				/* We are at the basic prompt */
				else
				{
					bell("This talent is still timed out.");
				}
			}

			/* Not in browse mode -- Use the talent */
			else
			{
				use_talent = TRUE;
				break;
			}
		}

		/* Error message */
		else if (talent != -2)
		{
			if (redraw) c_prt(TERM_YELLOW, "Illegal talent choice.", 1, 30);
			else bell("Illegal talent choice.");
		}

		/* Hack -- hide the cursor  XXX XXX */
		(void)Term_gotoxy(0, 26);
	}

	/* Restore the screen */
	if (redraw)
	{
		/* Erase the screen */
		clear_from(0);

		/* Set to 50 screen rows, if we were showing 50 before */
		if (old_rows == 50)
		{
			p_ptr->redraw |= (PR_MAP | PR_BASIC | PR_EXTRA);
			(void)Term_rows(TRUE);
		}

		/* Restore main screen */
		screen_load();
	}

	/* Use a talent */
	if (use_talent)
	{
		(void)do_talent(talent, TALENT_USE);
	}
}

