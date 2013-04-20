/* File: dungeon.c */

/* Purpose: Angband game engine */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"

#define CHAINSWORD_NOISE 100


/*
 * Return a "feeling" (or NULL) about an item.  Method 1 (Heavy).
 */
static cptr value_check_aux1(object_type *o_ptr)
{
	/* Artifacts */
	if (artifact_p(o_ptr) || o_ptr->art_name)
	{
		/* Cursed/Broken */
		if (cursed_p(o_ptr) || broken_p(o_ptr)) return "terrible";

		/* Normal */
		return "special";
	}

	/* Ego-Items */
	if (ego_item_p(o_ptr))
	{
		/* Cursed/Broken */
		if (cursed_p(o_ptr) || broken_p(o_ptr)) return "worthless";

		/* Normal */
		return "excellent";
	}

	/* Cursed items */
	if (cursed_p(o_ptr)) return "cursed";

	/* Broken items */
	if (broken_p(o_ptr)) return "broken";

	/* Good "armor" bonus */
	if (o_ptr->to_a > 0) return "good";

	/* Good "weapon" bonus */
	if (o_ptr->to_h + o_ptr->to_d > 0) return "good";

	/* Default to "average" */
	return "average";
}


/*
 * Return a "feeling" (or NULL) about an item.  Method 2 (Light).
 */
static cptr value_check_aux2(object_type *o_ptr)
{
	/* Cursed items (all of them) */
	if (cursed_p(o_ptr)) return "cursed";

	/* Broken items (all of them) */
	if (broken_p(o_ptr)) return "broken";

	/* Artifacts -- except cursed/broken ones */
	if (artifact_p(o_ptr) || o_ptr->art_name) return "good";

	/* Ego-Items -- except cursed/broken ones */
	if (ego_item_p(o_ptr)) return "good";

	/* Good armor bonus */
	if (o_ptr->to_a > 0) return "good";

	/* Good weapon bonuses */
	if (o_ptr->to_h + o_ptr->to_d > 0) return "good";

	/* No feeling */
	return (NULL);
}


/*
 * Sense the inventory (pseudo-ID)
 */
static void sense_inventory(void)
{
	int		i;
	int		plev = p_ptr->lev;
	bool		heavy = heavy_pseudo;
	cptr		feel;
	object_type	*o_ptr;
	char		o_name[80];

	/*** Check for "sensing" ***/

	/* No sensing when confused */
	if (p_ptr->confused) return;

	/*
	 * Also no sensing when you can't see (can't judge its quality if
	 * you can't take a look at it now and again :), are Berserk, or are
	 * distracted by other conditions... -- Gumby
	 */
	if (p_ptr->shero) return;
	if (p_ptr->blind) return;
	if (p_ptr->stun) return;
	if (p_ptr->paralyzed) return;
	if (p_ptr->afraid) return;
	if (p_ptr->cut) return;
	if (p_ptr->image) return;
	if (p_ptr->poisoned) return;

	/* Analyze the class */
	switch (p_ptr->pclass)
	{
		case CLASS_WARRIOR: case CLASS_WEAPONMASTER:
		case CLASS_ARCHER:
		{
			if (0 != rand_int(10000L / (plev * plev + 40))) return;
			break;
		}

		case CLASS_PRIEST:
		{
			if (0 != rand_int(15000L / (plev * plev + 40))) return;
			break;
		}

		case CLASS_ROGUE: case CLASS_BEASTMASTER: case CLASS_MONK:
		case CLASS_HIGH_MAGE:
		{
			if (0 != rand_int(20000L / (plev * plev + 40))) return;
			break;
		}

		case CLASS_RANGER: case CLASS_PALADIN:
		case CLASS_WARRIOR_MAGE: case CLASS_CHAOS_WARRIOR:
		{
			if (0 != rand_int(50000L / (plev * plev + 40))) return;
			break;
		}

		case CLASS_MINDCRAFTER:
		{
			if (0 != rand_int(40000L / (plev * plev + 40))) return;
			break;
		}

		case CLASS_MAGE:
		{
			if (0 != rand_int(150000L / (plev * plev + 40))) return;
			break;
		}
	}

	/*** Sense everything ***/

	/* Check everything */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		bool okay = FALSE;

		o_ptr = &inventory[i];

		/* Skip empty slots */
		if (!o_ptr->k_idx) continue;

		/* Valid "tval" codes */
		switch (o_ptr->tval)
		{
			case TV_SHOT: case TV_ARROW: case TV_BOLT:
			case TV_BOW: case TV_DIGGING: case TV_HAFTED:
			case TV_POLEARM: case TV_AXE: case TV_SWORD:
			case TV_BOOTS: case TV_GLOVES: case TV_HELM:
			case TV_CROWN: case TV_SHIELD: case TV_CLOAK:
			case TV_SOFT_ARMOR: case TV_HARD_ARMOR:
			case TV_DRAG_ARMOR: case TV_RING: case TV_AMULET:
			{
				okay = TRUE;
				break;
			}
		}

		/* Skip non-sense machines */
		if (!okay) continue;

		/* We know about it already, do not tell us again */
		if (o_ptr->ident & (IDENT_SENSE)) continue;

		/* It is fully known, no information needed */
		if (object_known_p(o_ptr)) continue;

		/* Occasional failure on inventory items */
		if ((i < INVEN_WIELD) && (0 != rand_int(5))) continue;

		/* Check for a feeling */
		feel = (heavy ? value_check_aux1(o_ptr) : value_check_aux2(o_ptr));

		/* Skip non-feelings */
		if (!feel) continue;

		/* Stop everything */
		if (disturb_minor) disturb(0, 0);

		/* Get an object description */
		object_desc(o_name, o_ptr, FALSE, 0);

		/* Message (equipment) */
		if (i >= INVEN_WIELD)
		{
			msg_format("You feel the %s (%c) you are %s %s %s...",
			           o_name, index_to_label(i), describe_use(i),
			           ((o_ptr->number == 1) ? "is" : "are"), feel);
		}

		/* Message (inventory) */
		else
		{
			msg_format("You feel the %s (%c) in your pack %s %s...",
			           o_name, index_to_label(i),
			           ((o_ptr->number == 1) ? "is" : "are"), feel);
		}

		/* We have "felt" it */
		o_ptr->ident |= (IDENT_SENSE);

		/* Inscribe it textually */
		if (!o_ptr->note) o_ptr->note = quark_add(feel);

		/* Combine / Reorder the pack (later) */
		p_ptr->notice |= (PN_COMBINE | PN_REORDER);

		/* Window stuff */
		p_ptr->window |= (PW_INVEN | PW_EQUIP);
	}
}



/*
 * Regenerate hit points				-RAK-
 */
static void regenhp(int percent)
{
	s32b        new_chp, new_chp_frac;
	int                   old_chp;

	/* Save the old hitpoints */
	old_chp = p_ptr->chp;

	/* Extract the new hitpoints */
	new_chp = ((long)p_ptr->mhp) * percent + PY_REGEN_HPBASE;
	p_ptr->chp += new_chp >> 16;   /* div 65536 */

	/* check for overflow */
	if ((p_ptr->chp < 0) && (old_chp > 0)) p_ptr->chp = MAX_SHORT;
	new_chp_frac = (new_chp & 0xFFFF) + p_ptr->chp_frac;	/* mod 65536 */
	if (new_chp_frac >= 0x10000L)
	{
		p_ptr->chp_frac = new_chp_frac - 0x10000L;
		p_ptr->chp++;
	}
	else
	{
		p_ptr->chp_frac = new_chp_frac;
	}

	/* Fully healed */
	if (p_ptr->chp >= p_ptr->mhp)
	{
		p_ptr->chp = p_ptr->mhp;
		p_ptr->chp_frac = 0;
	}

	/* Notice changes */
	if (old_chp != p_ptr->chp)
	{
		/* Redraw */
		p_ptr->redraw |= (PR_HP);

		/* Window stuff */
		p_ptr->window |= (PW_PLAYER);
	}
}


/*
 * Regenerate mana points				-RAK-
 */
static void regenmana(int percent)
{
	s32b        new_mana, new_mana_frac;
	int                   old_csp;

	old_csp = p_ptr->csp;
	new_mana = ((long)p_ptr->msp) * percent + PY_REGEN_MNBASE;
	p_ptr->csp += new_mana >> 16;	/* div 65536 */
	/* check for overflow */
	if ((p_ptr->csp < 0) && (old_csp > 0))
	{
		p_ptr->csp = MAX_SHORT;
	}
	new_mana_frac = (new_mana & 0xFFFF) + p_ptr->csp_frac;	/* mod 65536 */
	if (new_mana_frac >= 0x10000L)
	{
		p_ptr->csp_frac = new_mana_frac - 0x10000L;
		p_ptr->csp++;
	}
	else
	{
		p_ptr->csp_frac = new_mana_frac;
	}

	/* Must set frac to zero even if equal */
	if (p_ptr->csp >= p_ptr->msp)
	{
		p_ptr->csp = p_ptr->msp;
		p_ptr->csp_frac = 0;
	}

	/* Redraw mana */
	if (old_csp != p_ptr->csp)
	{
		/* Redraw */
		p_ptr->redraw |= (PR_MANA);

		/* Window stuff */
		p_ptr->window |= (PW_PLAYER);
		p_ptr->window |= (PW_SPELL);
	}
}






/*
 * Regenerate the monsters (once per 100 game turns)
 *
 * XXX XXX XXX Should probably be done during monster turns.
 */
static void regen_monsters(void)
{
	int i, frac;


	/* Regenerate everyone */
	for (i = 1; i < m_max; i++)
	{
		/* Check the i'th monster */
		monster_type *m_ptr = &m_list[i];
		monster_race *r_ptr = &r_info[m_ptr->r_idx];



		/* Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Allow regeneration (if needed) */
		if (m_ptr->hp < m_ptr->maxhp)
		{
			/* Hack -- Base regeneration */
			frac = m_ptr->maxhp / 100;

			/* Hack -- Minimal regeneration rate */
			if (!frac) frac = 1;

			/* Hack -- Some monsters regenerate quickly */
			if (r_ptr->flags2 & (RF2_REGENERATE)) frac *= 2;


			/* Hack -- Regenerate */
			m_ptr->hp += frac;

			/* Do not over-regenerate */
			if (m_ptr->hp > m_ptr->maxhp) m_ptr->hp = m_ptr->maxhp;

			/* Redraw (later) if needed */
			if (health_who == i) p_ptr->redraw |= (PR_HEALTH);
		}
	}
}


/*
 * Forcibly pseudo-identify an object in the inventory
 * (or on the floor)
 * 
 * note: currently this function allows pseudo-id of any object,
 * including silly ones like potions & scrolls, which always
 * get '{average}'. This should be changed, either to stop such
 * items from being pseudo-id'd, or to allow psychometry to
 * detect whether the unidentified potion/scroll/etc is
 * good (Cure Light Wounds, Restore Strength, etc) or
 * bad (Poison, Weakness etc) or 'useless' (Slime Mold Juice, etc).
 */
bool psychometry(void)
{
	int		item;
	object_type	*o_ptr;
	char		o_name[80];
        cptr		feel;

	/* Get an item (from equip or inven or floor) */
	if (!get_item(&item, "Meditate on which item? ", TRUE, TRUE, TRUE))
	{
		if (item == -2) msg_print("You have nothing appropriate.");
		return (FALSE);
	}

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}

	/* It is fully known, no information needed */
	if ((object_known_p(o_ptr)) || (o_ptr->ident & IDENT_SENSE))
	{
		msg_print("You cannot find out anything more about that.");
		return TRUE;
	}

	/* Check for a feeling */
	feel = value_check_aux1(o_ptr);

	/* Get an object description */
	object_desc(o_name, o_ptr, FALSE, 0);
    
	/* Skip non-feelings */
	if (!feel)
	{
		msg_format("You do not perceive anything unusual about the %s.", o_name);
		return TRUE;
	}

	msg_format("You feel that the %s %s %s...",
	    o_name, ((o_ptr->number == 1) ? "is" : "are"), feel);

	/* We have "felt" it */
	o_ptr->ident |= (IDENT_SENSE);

	/* Inscribe it textually */
	if (!o_ptr->note) o_ptr->note = quark_add(feel);

	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);

	/* Something happened */
	return (TRUE);
}


/*
 * Handle certain things once every 10 game turns
 */
static void process_world(void)
{
	int		x, y, i, j;
	int		regen_amount;
	bool		cave_no_regen = FALSE;
	cave_type	*c_ptr;
	object_type	*o_ptr;
	u32b		f1 = 0, f2 = 0, f3 = 0;
	char		o_name[80];

	/* Every 10 game turns */
	if (turn % 10) return;

	/* Stop being astral once in the town! -- Gumby */
	if (p_ptr->astral && !dun_level)
	{
		msg_print("Ahhhh! You finally have a body again!");

		p_ptr->astral = FALSE;
		p_ptr->was_astral = TRUE;
		p_ptr->max_dlv = dun_level;

		msg_print("Unfortunately, that means you'll have to walk back down again...");
	}

	/*** Check the Time and Load ***/

	if (!(turn % 1000))
	{
		/* Check time and load */
		if ((0 != check_time()) || (0 != check_load()))
		{
			/* Warning */
			if (closing_flag <= 2)
			{
				/* Disturb */
				disturb(0, 0);

				/* Count warnings */
				closing_flag++;

				/* Message */
				msg_print("The gates to GUMBAND are closing...");
				msg_print("Please finish up and/or save your game.");
			}

			/* Slam the gate */
			else
			{
				/* Message */
				msg_print("The gates to GUMBAND are now closed.");

				/* Stop playing */
				alive = FALSE;
			}
		}
	}

	/*** Attempt timed autosave ***/
	if (autosave_t && autosave_freq)
	{
		if (!(turn % ((s32b) autosave_freq * 10 )))
		{
			is_autosave = TRUE;
			msg_print("Autosaving the game...");
			do_cmd_save_game();
			is_autosave = FALSE;
		}
	}


	/*** Handle the "town" (stores and sunshine) ***/

	/* While in town */
	if (!dun_level)
	{
		/* Hack -- Daybreak/Nighfall in town */
		if (!(turn % ((10L * TOWN_DAWN) / 2)))
		{
			bool dawn;

			/* Check for dawn */
			dawn = (!(turn % (10L * TOWN_DAWN)));

			/* Day breaks */
			if (dawn)
			{
				/* Message */
				msg_print("The fiery green sun ignites.");

				/* Hack -- Scan the town */
				for (y = 0; y < cur_hgt; y++)
				{
					for (x = 0; x < cur_wid; x++)
					{
						/* Get the cave grid */
						c_ptr = &cave[y][x];

						/* Assume lit */
						c_ptr->info |= (CAVE_GLOW);

						/* Hack -- Memorize lit grids if allowed */
						if (view_perma_grids) c_ptr->info |= (CAVE_MARK);

						/* Hack -- Notice spot */
						note_spot(y, x);
					}
				}
			}

			/* Night falls */
			else
			{
				/* Message */
				msg_print("The sun has dimmed to a pale glow.");

				/* Hack -- Scan the town */
				for (y = 0; y < cur_hgt; y++)
				{
					for (x = 0; x < cur_wid; x++)
					{
						/* Get the cave grid */
						c_ptr = &cave[y][x];

						/* Darken "boring" features */
						if (c_ptr->feat <= FEAT_INVIS)
						{
							/* Forget the grid */
							c_ptr->info &= ~(CAVE_GLOW | CAVE_MARK);

							/* Hack -- Notice spot */
							note_spot(y, x);
						}
					}
				}
			}

			/* Update the monsters */
			p_ptr->update |= (PU_MONSTERS);

			/* Redraw map */
			p_ptr->redraw |= (PR_MAP);

			/* Window stuff */
			p_ptr->window |= (PW_OVERHEAD);
		}
	}


	/* While in the dungeon */
	else
	{
		/*** Update the Stores ***/

		/* Update the stores once a day (while in dungeon) */
		if (!(turn % (10L * STORE_TURNS)))
		{
			int n;

			/* Message */
			if (cheat_xtra) msg_print("Updating Shops...");

			/* Maintain each shop */
			for (n = 0; n < MAX_STORES; n++)
			{
				/* Maintain */
				store_maint(n);
			}

			/* Sometimes, shuffle the shop-keepers */
			if (rand_int(STORE_SHUFFLE) == 0)
			{
				/* Message */
				if (cheat_xtra) msg_print("Shuffling a Shopkeeper...");

				/* Shuffle a random shop */
				store_shuffle(rand_int(MAX_STORES));
			}

			/* Message */
			if (cheat_xtra) msg_print("Done.");
		}
	}


	/*** Process the monsters ***/

	/* Check for creature generation */
	if (rand_int(MAX_M_ALLOC_CHANCE) == 0)
	{
		/* Make a new monster */
		(void)alloc_monster(MAX_SIGHT + 5, FALSE);
	}

	/* Hack -- Check for creature regeneration */
	if (!(turn % 100)) regen_monsters();


	/*** Damage over Time ***/

	/* Take damage from poison */
	if ((p_ptr->poisoned) && !(p_ptr->invuln))
	{
		/* Take damage */
		take_hit(1, "poison");
	}

	/* (Vampires) Take damage from sunlight */
	if (p_ptr->prace == RACE_VAMPIRE)
	{
		if ((!dun_level)
		    && (!(p_ptr->resist_lite)) && !(p_ptr->invuln)
		    && (!((turn / ((10L * TOWN_DAWN)/2)) % 2)))
		{
			if (cave[py][px].info & (CAVE_GLOW))
			{
				/* Take damage */
				msg_print("The sun's rays scorch your undead flesh!");
				take_hit(1, "sunlight");
				cave_no_regen = TRUE;
			}
		}

		for (i = INVEN_WIELD; i < INVEN_TOTAL; i++)
		{
			object_type * o_ptr = &inventory[i];

			char o_name [80];
			char ouch [80];

			/* Skip non-objects */
			if (!o_ptr->k_idx) continue;

			/* Get the flags */
			object_flags(o_ptr, &f1, &f2, &f3);

			/* Get an object description */
			object_desc(o_name, o_ptr, FALSE, 0);

			if (((f3 & (TR3_LITE)) || (o_ptr->tval == TV_LITE)) &&
			    !(p_ptr->resist_lite))
			{
				msg_format("The %s scorches your undead flesh!", o_name);

				cave_no_regen = TRUE;

				/* Get an object description */
				object_desc(o_name, o_ptr, TRUE, 0);

				sprintf(ouch, "wielding %s", o_name);
				if (!(p_ptr->invuln)) take_hit(1, ouch);
			}
		}
	}

	/*
	 * Burn you for wielding something that can hurt your race. What can
	 * hurt you is based on monster.txt. Klackons and Gambolts are
	 * considered Animals, despite no monster.txt entry. Note that Evil
	 * isn't included - that would be a nightmare to code - and I've
	 * decided to let those wearing heavy gloves not take the damage. :)
	 *							-- Gumby
	 */
	if (inventory[INVEN_WIELD].k_idx && !(inventory[INVEN_HANDS].tval &&
	    (inventory[INVEN_HANDS].sval > SV_SET_OF_LEATHER_GLOVES)))
	{
		object_type * o_ptr = &inventory[INVEN_WIELD];

		char o_name [80];
		char ouch [80];
		bool burn = FALSE;

		/* Get the flags */
		object_flags(o_ptr, &f1, &f2, &f3);

		/* Get an object description */
		object_desc(o_name, o_ptr, FALSE, 0);

		switch (p_ptr->prace)
		{
			case RACE_HALF_TROLL:
				if (f1 & (TR1_SLAY_TROLL)) burn = TRUE;
				break;
			case RACE_HALF_GIANT:
				if (f1 & (TR1_SLAY_GIANT)) burn = TRUE;
				break;
			case RACE_GAMBOLT: case RACE_KLACKON:
			case RACE_YEEK:
				if (f1 & (TR1_SLAY_ANIMAL)) burn = TRUE;
				break;
			case RACE_DRACONIAN:
				if ((f1 & (TR1_SLAY_DRAGON)) ||
				    (f1 & (TR1_KILL_DRAGON))) burn = TRUE;
				break;
			case RACE_VAMPIRE: case RACE_SPECTRE:
				if (f1 & (TR1_SLAY_UNDEAD)) burn = TRUE;
				break;
			default: /* not hurt by a slay */
				break;
		}

		/*
		 * Slay Humanoid exists only for Erekose's sword, Kanajana,
		 * whose effects come from being radioactive, those with
		 * resist poison don't get harmed.
		 */
		if ((f1 & (TR1_SLAY_HUMANOID)) && !(p_ptr->resist_pois))
			burn = TRUE;

		if ((burn) && (randint(2)==1) && !(p_ptr->invuln))
		{
			msg_format("The %s burns you!", o_name);
			cave_no_regen = TRUE;

			/* Get an object description */
			object_desc(o_name, o_ptr, TRUE, 0);

			sprintf(ouch, "wielding %s", o_name);
			take_hit(1, ouch);
		}
	}

	/* Spectres -- take damage when moving through walls */
	/*
	 * Added: Any non-astral character takes damage if inside through
	 * walls without wraith form -- NOTE: Spectres will never be
	 * reduced below 0 hp by being inside a stone wall; others
	 * WILL BE!
	 */

	if (!cave_floor_bold(py, px))
	{
		if (!(p_ptr->astral)) cave_no_regen = TRUE;

		if (!(p_ptr->invuln) && !(p_ptr->wraith_form) && !(p_ptr->astral) &&
		    ((p_ptr->chp > ((p_ptr->lev)/5)) || (p_ptr->prace != RACE_SPECTRE)))
		{
			cptr dam_desc;

			if ((p_ptr->prace == RACE_SPECTRE) &&
			    !(p_ptr->astral))
			{
				msg_print("Your molecules feel disrupted!");
				dam_desc = "density";
			}
			else
			{
				msg_print("You are being crushed!");
				dam_desc = "solid rock";
			}

			take_hit(1 + ((p_ptr->lev)/10), dam_desc);
		}
	}


	/* Take damage from cuts */
	if ((p_ptr->cut) && !(p_ptr->invuln))
	{
		/* Mortal wound or Deep Gash */
		if (p_ptr->cut > 200)
		{
			i = 3;
		}

		/* Severe cut */
		else if (p_ptr->cut > 100)
		{
			i = 2;
		}

		/* Other cuts */
		else
		{
			i = 1;
		}

		/* Take damage */
		take_hit(i, "a fatal wound");
	}


	/*** Check the Food, and Regenerate ***/

	/* Digest normally */
	if (p_ptr->food < PY_FOOD_MAX)
	{
		/* Every 100 game turns */
		if (!(turn % 100))
		{
			/* Basic digestion rate based on speed */
			i = extract_energy[p_ptr->pspeed] * 2;

			/* Regeneration takes more food (was 30) */
			if (p_ptr->regenerate) i += 20;

			/* Slow digestion takes less food */
			if (p_ptr->slow_digest) i -= 10;

			/* Minimal digestion */
			if (i < 1) i = 1;

			/* Digest some food */
			(void)set_food(p_ptr->food - i);
		}
	}

	/* Digest quickly when gorged */
	else
	{
		/* Digest a lot of food */
		(void)set_food(p_ptr->food - 100);
	}

	/* Starve to death (slowly) */
	if (p_ptr->food < PY_FOOD_STARVE)
	{
		/* Calculate damage */
		i = (PY_FOOD_STARVE - p_ptr->food) / 10;

		/* Take damage */
		if (!(p_ptr->invuln)) take_hit(i, "starvation");
	}

	/* Default regeneration */
	regen_amount = PY_REGEN_NORMAL;

	/* Getting Weak */
	if (p_ptr->food < PY_FOOD_WEAK)
	{
		/* Lower regeneration */
		if (p_ptr->food < PY_FOOD_STARVE)
		{
			regen_amount = 0;
		}
		else if (p_ptr->food < PY_FOOD_FAINT)
		{
			regen_amount = PY_REGEN_FAINT;
		}
		else
		{
			regen_amount = PY_REGEN_WEAK;
		}

		/* Getting Faint */
		if (p_ptr->food < PY_FOOD_FAINT)
		{
			/* Faint occasionally */
			if (!p_ptr->paralyzed && (rand_int(100) < 10))
			{
				/* Message */
				msg_print("You faint from the lack of food.");
				disturb(1, 0);

				/* Hack -- faint (bypass free action) */
				(void)set_paralyzed(p_ptr->paralyzed + 1 + rand_int(5));
			}
		}
	}


	{
		/* Regeneration ability */
		if (p_ptr->regenerate)
		{
			regen_amount = regen_amount * 2;
		}
	}

	/* Searching or Resting */
	if (p_ptr->searching || resting)
	{
		regen_amount = regen_amount * 2;
	}

	/* Regenerate the mana */
	if (p_ptr->csp < p_ptr->msp) regenmana(regen_amount);

	/* Poisoned or cut yields no healing */
	if (p_ptr->poisoned) regen_amount = 0;
	if (p_ptr->cut) regen_amount = 0;

	/* Regenerate Hit Points if needed */
	if (p_ptr->chp < p_ptr->mhp)
	{
		regenhp(regen_amount);
	}


	/*** Timeout Various Things ***/

	/* Hack -- Hallucinating */
	if (p_ptr->image)
	{
		(void)set_image(p_ptr->image - 1);
	}

	/* Blindness */
	if (p_ptr->blind)
	{
		(void)set_blind(p_ptr->blind - 1);
	}

	/* Times see-invisible */
	if (p_ptr->tim_invis)
	{
		(void)set_tim_invis(p_ptr->tim_invis - 1);
	}

	if (multi_rew)
	{
		multi_rew = FALSE;
	}

	/* Timed esp */
	if (p_ptr->tim_esp)
	{
		(void)set_tim_esp(p_ptr->tim_esp - 1);
	}

	/* Timed infra-vision */
	if (p_ptr->tim_infra)
	{
		(void)set_tim_infra(p_ptr->tim_infra - 1);
	}

	/* Paralysis */
	if (p_ptr->paralyzed)
	{
		(void)set_paralyzed(p_ptr->paralyzed - 1);
	}

	/* Confusion */
	if (p_ptr->confused)
	{
		(void)set_confused(p_ptr->confused - 1);
	}

	/* Afraid */
	if (p_ptr->afraid)
	{
		(void)set_afraid(p_ptr->afraid - 1);
	}

	/* Fast */
	if (p_ptr->fast)
	{
		(void)set_fast(p_ptr->fast - 1);
	}

	/* Slow */
	if (p_ptr->slow)
	{
		(void)set_slow(p_ptr->slow - 1);
	}

	/* Protection from evil */
	if (p_ptr->protevil)
	{
		(void)set_protevil(p_ptr->protevil - 1);
	}

	/* Invulnerability */
	if (p_ptr->invuln)
	{
		(void)set_invuln(p_ptr->invuln - 1);
	}

	/* Wraith form */
	if (p_ptr->wraith_form)
	{
		(void)set_shadow(p_ptr->wraith_form - 1);
	}

	/* Heroism */
	if (p_ptr->hero)
	{
		(void)set_hero(p_ptr->hero - 1);
	}

	/* Super Heroism */
	if (p_ptr->shero)
	{
		(void)set_shero(p_ptr->shero - 1);
	}

	/* Blessed */
	if (p_ptr->blessed)
	{
		(void)set_blessed(p_ptr->blessed - 1);
	}

	/* Shield */
	if (p_ptr->shield)
	{
		(void)set_shield(p_ptr->shield - 1);
	}

	/* Oppose Acid */
	if (p_ptr->oppose_acid)
	{
		(void)set_oppose_acid(p_ptr->oppose_acid - 1);
	}

	/* Oppose Lightning */
	if (p_ptr->oppose_elec)
	{
		(void)set_oppose_elec(p_ptr->oppose_elec - 1);
	}

	/* Oppose Fire */
	if (p_ptr->oppose_fire)
	{
		(void)set_oppose_fire(p_ptr->oppose_fire - 1);
	}

	/* Oppose Cold */
	if (p_ptr->oppose_cold)
	{
		(void)set_oppose_cold(p_ptr->oppose_cold - 1);
	}

	/* Oppose Poison */
	if (p_ptr->oppose_pois)
	{
		(void)set_oppose_pois(p_ptr->oppose_pois - 1);
	}


	/*** Poison and Stun and Cut ***/

	/* Poison */
	if (p_ptr->poisoned)
	{
		int adjust = (adj_con_fix[p_ptr->stat_ind[A_CON]] + 1);

		/* Apply some healing */
		(void)set_poisoned(p_ptr->poisoned - adjust);
	}

	/* Stun */
	if (p_ptr->stun)
	{
		int adjust = (adj_con_fix[p_ptr->stat_ind[A_CON]] + 1);

		/* Apply some healing */
		(void)set_stun(p_ptr->stun - adjust);
	}

	/* Cut */
	if (p_ptr->cut)
	{
		int adjust = (adj_con_fix[p_ptr->stat_ind[A_CON]] + 1);

		/* Hack -- Truly "mortal" wound */
		if (p_ptr->cut > 1000) adjust = 0;

		/* Apply some healing */
		(void)set_cut(p_ptr->cut - adjust);
	}



	/*** Process Light ***/

	/* Check for light being wielded */
	o_ptr = &inventory[INVEN_LITE];

	/* Burn some fuel in the current lite */
	if (o_ptr->tval == TV_LITE)
	{
		/* Hack -- Use some fuel (except on artifacts) */
		if (!artifact_p(o_ptr) &&
		    !(o_ptr->sval == SV_LITE_FEANORAN_LAMP) &&
		    (o_ptr->pval > 0))
		{
			/* Decrease life-span */
			o_ptr->pval--;

			/* Hack -- notice interesting fuel steps */
			if ((o_ptr->pval < 100) || (!(o_ptr->pval % 100)))
			{
				/* Window stuff */
				p_ptr->window |= (PW_EQUIP);
			}

			/* Hack -- Special treatment when blind */
			if (p_ptr->blind)
			{
				/* Hack -- save some light for later */
				if (o_ptr->pval == 0) o_ptr->pval++;
			}

			/* The light is now out */
			else if (o_ptr->pval == 0)
			{
				disturb(0, 0);
				msg_print("Your light has gone out!");
			}

			/* The light is getting dim */
			else if ((o_ptr->pval < 100) && (!(o_ptr->pval % 10)))
			{
				if (disturb_minor) disturb(0, 0);
				msg_print("Your light is growing faint.");
			}
		}
	}

	/* Calculate torch radius */
	p_ptr->update |= (PU_TORCH);

	/*
	 * Process the effects of mutations (in mutation.c), if you have any
	 * that might need to be checked -- Gumby
	 */
	if (p_ptr->muta2)
	{
		process_mutations();
	}

	/*** Process Inventory ***/

	/* Handle experience draining */
	if (p_ptr->exp_drain)
	{
		if ((rand_int(100) < 10) && (p_ptr->exp > 0))
		{
			p_ptr->exp--;
			p_ptr->max_exp--;
			check_experience();
			msg_print("You feel your life slowly drain away...");
		}
	}

	/* Process equipment */
	for (j = 0, i = INVEN_WIELD; i < INVEN_TOTAL; i++)
	{
		/* Get the object */
		o_ptr = &inventory[i];

		object_flags(o_ptr, &f1, &f2, &f3);

	 	/*
	 	 * Auto-curse
	 	 * Curse_turn is set in remove_curse_aux()).
	 	 */
	 	if ((f3 & TR3_AUTO_CURSE) && (~o_ptr->ident & IDENT_CURSED) &&
		    (turn > curse_turn))
	 	{
	 		o_ptr->ident |= IDENT_CURSED;
	 		object_desc(o_name, o_ptr, FALSE, 0);
			o_ptr->note = quark_add("cursed");
	 		msg_format("The %s suddenly feels deathly cold!", o_name);
	 	}

		/* Make a chainsword noise */
		if ((o_ptr->name1 == ART_CHAINSWORD) && randint(CHAINSWORD_NOISE) == 1)
		{
			char noise[80];
			get_rnd_line("chainswd.txt", noise);
			msg_print(noise);
			if (disturb_minor) disturb(FALSE, FALSE);
		}

		/*
		 * Hack: Uncursed teleporting items (e.g. Trump Weapons)
		 *can actually be useful!
		 */

		if ((f3 & TR3_TELEPORT) && (rand_int(100)<1))
		{
			if ((o_ptr->ident & IDENT_CURSED) && !(p_ptr->anti_tele))
			{
				disturb(0,0);

				msg_print("You feel unstable...");

				/* Teleport player */
				teleport_player(40);
			}
			else
			{
				if ((!disturb_other) ||
				    (o_ptr->note && (strchr(quark_str(o_ptr->note),'.'))))
				{
					/* Do nothing */
					/* msg_print("Teleport aborted.") */ ;
				}
				else if (get_check("Teleport? "))
				{
					disturb(0,0);
					teleport_player(50);
				}
			}
		}


		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;

		/* Recharge activatable objects */
		if (o_ptr->timeout > 0)
		{
			/* Recharge */
			o_ptr->timeout--;

			/* Notice changes */
			if (!(o_ptr->timeout)) j++;
		}
	}

	/* Notice changes */
	if (j)
	{
		/* Window stuff */
		p_ptr->window |= (PW_EQUIP);
	}

	/* Recharge rods */
	for (j = 0, i = 0; i < INVEN_PACK; i++)
	{
		o_ptr = &inventory[i];

		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;

		/* Examine all charging rods */
		if ((o_ptr->tval == TV_ROD) && (o_ptr->pval))
		{
			/* Charge it */
			o_ptr->pval--;

			/* Notice changes */
			if (!(o_ptr->pval)) j++;
		}
	}

	/* Notice changes */
	if (j)
	{
		/* Combine pack */
		p_ptr->notice |= (PN_COMBINE);

		/* Window stuff */
		p_ptr->window |= (PW_INVEN);
	}

	/* Feel the inventory */
	sense_inventory();


	/*** Process Objects ***/

	/* Process objects */
	for (i = 1; i < o_max; i++)
	{
		/* Access object */
		o_ptr = &o_list[i];

		/* Skip dead objects */
		if (!o_ptr->k_idx) continue;

		/* Recharge rods on the ground */
		if ((o_ptr->tval == TV_ROD) && (o_ptr->pval)) o_ptr->pval--;
	}


	/*** Involuntary Movement ***/

#if 0
	/*
	 * This has been replaced with an object specific check.
	 * Uncursed teleporting objects ask: "Teleport? [y/n]" -- TY
	 */

	/* Mega-Hack -- Random teleportation XXX XXX XXX */
	if ((p_ptr->teleport) && (rand_int(100) < 1))
	{
		/* Teleport player */
		teleport_player(40);
	}
#endif /* 0 */

	/* Delayed Word-of-Recall */
	if (p_ptr->word_recall)
	{
		/*
		 * HACK: Autosave BEFORE resetting the recall counter (rr9)
		 * The player is yanked up/down as soon as
		 * he loads the autosaved game.
		 */
		if (autosave_l && (p_ptr->word_recall == 1))
		{
			is_autosave = TRUE;
			msg_print("Autosaving the game...");
			do_cmd_save_game();
			is_autosave = FALSE;
		}

		/* Count down towards recall */
		p_ptr->word_recall--;

		/* Activate the recall */
		if (!p_ptr->word_recall)
		{
			/* Disturbing! */
			disturb(0, 0);

			/* Determine the level */
			if (dun_level)
			{
				msg_print("You feel yourself yanked upwards!");

				dun_level = 0;
				new_level_flag = TRUE;
			}
			else
			{
				msg_print("You feel yourself yanked downwards!");

				dun_level = p_ptr->max_dlv;
				if (dun_level < 1) dun_level = 1;
				new_level_flag = TRUE;
			}
			
			/* Sound */
			sound(SOUND_TPLEVEL);
		}
	}
}



/*
 * Verify use of "wizard" mode
 */
static bool enter_wizard_mode(void)
{
	/* Ask first time */
#if 0
	if (!(noscore & 0x0002))
#else
	if (!noscore)
#endif
	{
		/* Mention effects */
		msg_print("Wizard mode is for debugging and experimenting.");
		msg_print("The game will not be scored if you enter wizard mode.");
		msg_print(NULL);

		/* Verify request */
		if (!get_check("Are you sure you want to enter wizard mode? "))
		{
			return (FALSE);
		}

		/* Mark savefile */
		noscore |= 0x0002;
	}

	/* Success */
	return (TRUE);
}


#ifdef ALLOW_WIZARD

/*
 * Verify use of "debug" commands
 */
static bool enter_debug_mode(void)
{
	/* Ask first time */
#if 0
	if (!(noscore & 0x0008))
#else
	if (!noscore)
#endif
	{
		/* Mention effects */
		msg_print("The debug commands are for debugging and experimenting.");
		msg_print("The game will not be scored if you use debug commands.");
		msg_print(NULL);

		/* Verify request */
		if (!get_check("Are you sure you want to use debug commands? "))
		{
			return (FALSE);
		}

		/* Mark savefile */
		noscore |= 0x0008;
	}

	/* Success */
	return (TRUE);
}

/*
 * Hack -- Declare the Debug Routines
 */
extern void do_cmd_debug(void);

#endif /* ALLOW_WIZARD */


#ifdef ALLOW_BORG

/*
 * Verify use of "borg" commands
 */
static bool enter_borg_mode(void)
{
	/* Ask first time */
	if (!(noscore & 0x0010))
	{
		/* Mention effects */
		msg_print("The borg commands are for debugging and experimenting.");
		msg_print("The game will not be scored if you use borg commands.");
		msg_print(NULL);

		/* Verify request */
		if (!get_check("Are you sure you want to use borg commands? "))
		{
			return (FALSE);
		}

		/* Mark savefile */
		noscore |= 0x0010;
	}

	/* Success */
	return (TRUE);
}

/*
 * Hack -- Declare the Ben Borg
 */
extern void do_cmd_borg(void);

#endif /* ALLOW_BORG */



/*
 * Parse and execute the current command
 * Give "Warning" on illegal commands.
 *
 * XXX XXX XXX Make some "blocks"
 */
static void process_command(void)
{
	char error_m[80];

#ifdef ALLOW_REPEAT

	/* Handle repeating the last command */
	repeat_check();

#endif /* ALLOW_REPEAT */

	/* Parse the command */
	switch (command_cmd)
	{
		/* Ignore */
		case ESCAPE:
		case ' ':
		{
			break;
		}

		/* Ignore return */
		case '\r':
		{
			break;
		}

		/*** Wizard Commands ***/

		/* Toggle Wizard Mode */
		case KTRL('W'):
		{
			if (wizard)
			{
				wizard = FALSE;
				msg_print("Wizard mode off.");
			}
			else if (enter_wizard_mode())
			{
				wizard = TRUE;
				msg_print("Wizard mode on.");
			}

			/* Update monsters */
			p_ptr->update |= (PU_MONSTERS);

			/* Redraw "title" */
			p_ptr->redraw |= (PR_TITLE);

			break;
		}


#ifdef ALLOW_WIZARD

		/* Special "debug" commands */
		case KTRL('A'):
		{
			/* Enter debug mode */
			if (enter_debug_mode())
			{
				do_cmd_debug();
			}
			break;
		}

#endif /* ALLOW_WIZARD */


#ifdef ALLOW_BORG

		/* Special "borg" commands */
		case KTRL('Z'):
		{
			/* Enter borg mode */
			if (enter_borg_mode())
			{
				do_cmd_borg();
			}

			break;
		}

#endif /* ALLOW_BORG */



		/*** Inventory Commands ***/

		/* Wear/wield equipment */
		case 'w':
		{
			do_cmd_wield();
			break;
		}

		/* Take off equipment */
		case 't':
		{
			do_cmd_takeoff();
			break;
		}

		/* Drop an item */
		case 'd':
		{
			do_cmd_drop();
			break;
		}

		/* Destroy an item */
		case 'k':
		{
			do_cmd_destroy();
			break;
		}

		/* Equipment list */
		case 'e':
		{
			do_cmd_equip();
			break;
		}

		/* Inventory list */
		case 'i':
		{
			do_cmd_inven();
			break;
		}


		/*** Various commands ***/

		/* Identify an object */
		case 'I':
		{
			do_cmd_observe();
			break;
		}

		/* Hack -- toggle windows */
		case KTRL('I'):
		{
			toggle_inven_equip();
			break;
		}


		/*** Standard "Movement" Commands ***/

		/* Alter a grid */
		case '+':
		{
			do_cmd_alter();
			break;
		}

		/* Dig a tunnel */
		case 'T':
		{
			do_cmd_tunnel();
			break;
		}

		/* Move (usually pick up things) */
		case ';':
		{
			do_cmd_walk(always_pickup);
			break;
		}

		/* Move (usually do not pick up) */
		case '-':
		{
			do_cmd_walk(!always_pickup);
			break;
		}


		/*** Running, Resting, Searching, Staying */

		/* Begin Running -- Arg is Max Distance */
		case '.':
		{
			do_cmd_run();
			break;
		}

		/* Stay still (usually pick things up) */
		case ',':
		{
			do_cmd_stay(always_pickup);
			break;
		}

		/* Stay still (usually do not pick up) */
		case 'g':
		{
			do_cmd_stay(!always_pickup);
			break;
		}

		/* Rest -- Arg is time */
		case 'R':
		{
			do_cmd_rest();
			break;
		}

		/* Search for traps/doors */
		case 's':
		{
			do_cmd_search();
			break;
		}

		/* Toggle search mode */
		case 'S':
		{
			do_cmd_toggle_search();
			break;
		}


		/*** Stairs and Doors and Chests and Traps ***/

		/* Enter store */
		case '_':
		{
			do_cmd_store();
			break;
		}

		/* Go up staircase */
		case '<':
		{
			do_cmd_go_up();
			break;
		}

		/* Go down staircase */
		case '>':
		{
			do_cmd_go_down();
			break;
		}

		/* Open a door or chest */
		case 'o':
		{
			do_cmd_open();
			break;
		}

		/* Close a door */
		case 'c':
		{
			do_cmd_close();
			break;
		}

		/* Jam a door with spikes */
		case 'j':
		{
			do_cmd_spike();
			break;
		}

		/* Bash a door */
		case 'B':
		{
			do_cmd_bash();
			break;
		}

		/* Disarm a trap or chest */
		case 'D':
		{
			do_cmd_disarm();
			break;
		}


		/*** Magic and Prayers ***/

		/* Gain new spells/prayers */
		case 'G':
		{
			do_cmd_study();
			break;
		}

		/* Browse a book */
		case 'b':
		{
			do_cmd_browse();
			break;
		}

		/* Cast a spell */
		case 'm':
		{
			if (p_ptr->pclass == CLASS_BEASTMASTER)
			{
				energy_use = 100;
				if ((p_ptr->number_pets <= adj_chr_pet_summon[p_ptr->stat_ind[A_CHR]]) &&
					summon_specific_friendly(py, px, dun_level, SUMMON_NO_UNIQUES, FALSE))
				{
					msg_print("You summon some help.");
					p_ptr->number_pets++;
				}
				else
				{
					msg_print("You called, but no help came.");
				}
			}
			else if (p_ptr->anti_magic)
			{
				cptr which_power = "magic";
				if (p_ptr->pclass == CLASS_MINDCRAFTER)
					which_power = "psionic powers";
				else if (mp_ptr->spell_book == TV_LIFE_BOOK)
					which_power = "prayer";

				msg_format("An anti-magic shell disrupts your %s!", which_power);

				energy_use = 0;
			}
			else
			{
				if (p_ptr->pclass == CLASS_MINDCRAFTER)
				{
					do_cmd_mindcraft();
				}
				else
				{
					do_cmd_cast();
				}
			}
			break;
		}

		/* Issue a pet command */
		case 'p':
		{
			do_cmd_pet();
			break;
		}


		/*** Use various objects ***/

		/* Inscribe an object */
		case '{':
		{
			do_cmd_inscribe();
			break;
		}

		/* Uninscribe an object */
		case '}':
		{
			do_cmd_uninscribe();
			break;
		}

		/* Activate an artifact */
		case 'A':
		{
			do_cmd_activate();
			break;
		}

		/* Eat some food */
		case 'E':
		{
			do_cmd_eat_food();
			break;
		}

		/* Fuel your lantern/torch */
		case 'F':
		{
			do_cmd_refill();
			break;
		}

		/* Fire an item */
		case 'f':
		{
			do_cmd_fire();
			break;
		}

		/* Throw an item */
		case 'v':
		{
			do_cmd_throw();
			break;
		}

		/* Aim a wand */
		case 'a':
		{
			do_cmd_aim_wand();
			break;
		}

		/* Zap a rod */
		case 'z':
		{
			do_cmd_zap_rod();
			break;
		}

		/* Quaff a potion */
		case 'q':
		{
			do_cmd_quaff_potion();
			break;
		}

		/* Read a scroll */
		case 'r':
		{
			do_cmd_read_scroll();
			break;
		}

		/* Use a staff */
		case 'u':
		{
			do_cmd_use_staff();
			break;
		}


		case 'U':
		{
			do_cmd_racial_power();
			break;
		}


		/*** Looking at Things (nearby or on map) ***/

		/* Full dungeon map */
		case 'M':
		{
			do_cmd_view_map();
			break;
		}

		/* Locate player on map */
		case 'L':
		{
			do_cmd_locate();
			break;
		}

		/* Look around */
		case 'l':
		{
			do_cmd_look();
			break;
		}

		/* Target monster or location */
		case '*':
		{
			do_cmd_target();
			break;
		}



		/*** Help and Such ***/

		/* Help */
		case '?':
		{
			do_cmd_help("help.hlp");
			break;
		}

		/* Identify symbol */
		case '/':
		{
			do_cmd_query_symbol();
			break;
		}

		/* Character description */
		case 'C':
		{
			do_cmd_change_name();
			break;
		}


		/*** System Commands ***/

		/* Hack -- User interface */
		case '!':
		{
			(void)Term_user(0);
			break;
		}

		/* Single line from a pref file */
		case '"':
		{
			do_cmd_pref();
			break;
		}

		/* Interact with macros */
		case '@':
		{
			do_cmd_macros();
			break;
		}

		/* Interact with visuals */
		case '%':
		{
			do_cmd_visuals();
			break;
		}

		/* Interact with colors */
		case '&':
		{
			do_cmd_colors();
			break;
		}

		/* Interact with options */
		case '=':
		{
			do_cmd_options();
			p_ptr->redraw |= (PR_EQUIPPY);
			break;
		}


		/*** Misc Commands ***/

#ifdef ALLOW_SPOILERS
		/* Generate spoilers (moved from wiz-mode by Gumby) */
		case '$':
		{
			do_cmd_spoilers();
			break;
		}
#endif /* ALLOW_SPOILERS */

		/* Take notes */
		case ':':
		{
			do_cmd_note();
			break;
		}

		/* Version info */
		case 'V':
		{
			do_cmd_version();
			break;
		}

		/* Repeat level feeling */
		case KTRL('F'):
		{
			do_cmd_feeling();
			break;
		}

		/* Show previous message */
		case KTRL('O'):
		{
			do_cmd_message_one();
			break;
		}

		/* Show previous messages */
		case KTRL('P'):
		{
			do_cmd_messages();
			break;
		}

		/* Redraw the screen */
		case KTRL('R'):
		{
			do_cmd_redraw();
			break;
		}

#ifndef VERIFY_SAVEFILE

		/* Hack -- Save and don't quit */
		case KTRL('S'):
		{
			is_autosave = FALSE;
			do_cmd_save_game();
			break;
		}

#endif /* VERIFY_SAVEFILE */

		case KTRL('T'):
		{
			/* dummy is % thru day/night */
			int dummy = ((turn % ((10L * TOWN_DAWN)/2) * 100) / ((10L * TOWN_DAWN)/2));
			int minute = ((turn % ((10L * TOWN_DAWN)/2) * 720) / ((10L * TOWN_DAWN)/2)) % 60;
			int hour = ((12 * dummy) / 100) - 6;    /* -6 to +6 */
			int hour12 = 0;
			bool morning = FALSE;
			int day = 0;

			if (turn <= (10L * TOWN_DAWN)/4)
			{
				day = 1;
			}
			else
			{
				day = (turn - (10L * TOWN_DAWN / 4)) / (10L * TOWN_DAWN) + 1;
			}

			/* night: 6pm -- 6am */
			if ((turn / ((10L * TOWN_DAWN)/2)) % 2)
			{
				if (hour <= 0)
				{
					hour12 = 12 - (hour * -1);
				}
				else
				{
					hour12 = hour;
				}

				if (hour >= 0) morning = TRUE;
				else morning = FALSE;

				msg_format("%d:%02d %s, day %d.", hour12, minute, (morning? "AM" : "PM"),
					 turn / (10L * TOWN_DAWN) + 1);

				if (dummy < 5)
					msg_print("The sun has grown dark.");
				else if (dummy == 50)
					msg_print("It is midnight.");
				else if ((dummy > 94) && (dummy < 101))
					msg_print("The sun is flickering hopefully.");
				else if ((dummy > 75) && (dummy < 95))
					msg_print("The sun is close to waking.");
				else if (dummy > 100)
					msg_format("What a funny night-time! (%d)", dummy);
				else
					msg_format("It is night.");
			}
			else
			/* day */
			{
				if (hour <= 0)
				{
					hour12 = 12 - (hour * -1);
				}
				else
				{
					hour12 = hour;
				}

				if (hour >= 0) morning = FALSE;
				else morning = TRUE;

				msg_format("%d:%02d %s, day %d.", hour12, minute, (morning? "AM" : "PM"),
					 turn / (10L * TOWN_DAWN) + 1);

				if (dummy < 5)
					msg_print("The sun has reignited.");
				else if (dummy < 25)
					msg_print("It is early morning.");
				else if (dummy < 50)
					msg_print("It is late morning.");
				else if (dummy == 50)
					msg_print("It is noon.");
				else if (dummy < 65)
					msg_print("It is early afternoon.");
				else if (dummy < 85)
					msg_print("It is late afternoon.");
				else if (dummy < 95)
					msg_print("It is early evening.");
				else if (dummy < 101)
					msg_print("The sun is dimming.");
				else
					msg_format("What a strange daytime! (%d)", dummy);
			}
		}
		break;

		/*
		 * Show quest status -- Heino Vander Sanden
		 * Next quest level added -- Gumby
		 */
		case KTRL('Q'):
		{
			int k = 0;

			k = next_quest_level();

			if (is_quest(dun_level, FALSE))
			{
				print_quest_message();
			}
			else
			{
				msg_print("No current quest.");
				if (k != 127)
				{
					if (depth_in_feet)
						msg_format(" Next quest at %d feet.", k * 50);
					else
						msg_format(" Next quest on level %d.", k);
				}
			}
			break;
		}

		/* Save and quit */
		case KTRL('X'):
		{
			alive = FALSE;
			break;
		}

		/* Quit (commit suicide) */
		case 'Q':
		{
			do_cmd_suicide();
			break;
		}

		/* Check artifacts, uniques, objects */
		case '~':
		case '|':
		{
			do_cmd_knowledge();
			break;
		}

		/* Load "screen dump" */
		case '(':
		{
			do_cmd_load_screen();
			break;
		}

		/* Save "screen dump" */
		case ')':
		{
			do_cmd_save_screen();
			break;
		}

		/* Hack -- Unknown command */
		default:
		{
			if (randint(2)==1)
			{
				get_rnd_line("error.txt", error_m);
				msg_print(error_m);
			}
			else
				prt("Type '?' for help.", 0, 0);
			break;
		}
	}
}




/*
 * Process the player
 *
 * Notice the annoying code to handle "pack overflow", which
 * must come first just in case somebody manages to corrupt
 * the savefiles by clever use of menu commands or something.
 */
static void process_player(void)
{
	int i;

	/*** Apply energy ***/

	if (hack_mutation)
	{
		msg_print("You feel different!");
		(void)gain_random_mutation(0);
		hack_mutation = FALSE;
	}

	/* Give the player some energy */
	p_ptr->energy += extract_energy[p_ptr->pspeed];

	/* No turn yet */
	if (p_ptr->energy < 100) return;


	/*** Check for interupts ***/

	/* Complete resting */
	if (resting < 0)
	{
		/* Basic resting */
		if (resting == -1)
		{
			/* Stop resting */
			if ((p_ptr->chp == p_ptr->mhp) &&
                (p_ptr->csp >= p_ptr->msp))
			{
				disturb(0, 0);
			}
		}

		/* Complete resting */
		else if (resting == -2)
		{
			/* Stop resting */
			if ((p_ptr->chp == p_ptr->mhp) &&
			    (p_ptr->csp == p_ptr->msp) &&
			    !p_ptr->blind && !p_ptr->confused &&
			    !p_ptr->poisoned && !p_ptr->afraid &&
			    !p_ptr->stun && !p_ptr->cut &&
			    !p_ptr->slow && !p_ptr->paralyzed &&
			    !p_ptr->image && !p_ptr->word_recall)
			{
				disturb(0, 0);
			}
		}
	}

	/* Handle "abort" */
	if (!avoid_abort)
	{
		/* Check for "player abort" (semi-efficiently for resting) */
		if (running || command_rep || (resting && !(resting & 0x0F)))
		{
			/* Do not wait */
			inkey_scan = TRUE;

			/* Check for a key */
			if (inkey())
			{
				/* Flush input */
				flush();

				/* Disturb */
				disturb(0, 0);

				/* Hack -- Show a Message */
				msg_print("Cancelled.");
			}
		}
	}


	/*** Handle actual user input ***/

	/* Repeat until out of energy */
	while (p_ptr->energy >= 100)
	{
		/* Notice stuff (if needed) */
		if (p_ptr->notice) notice_stuff();

		/* Update stuff (if needed) */
		if (p_ptr->update) update_stuff();

		/* Redraw stuff (if needed) */
		if (p_ptr->redraw) redraw_stuff();

		/* Redraw stuff (if needed) */
		if (p_ptr->window) window_stuff();


		/* Place the cursor on the player */
		move_cursor_relative(py, px);

		/* Refresh (optional) */
		if (fresh_before) Term_fresh();


		/* Hack -- Pack Overflow */
		if (inventory[INVEN_PACK].k_idx)
		{
			int item = INVEN_PACK;

			char o_name[80];

			object_type *o_ptr;

			/* Access the slot to be dropped */
			o_ptr = &inventory[item];

			/* Disturbing */
			disturb(0, 0);

			/* Warning */
			msg_print("Your pack overflows!");

			/* Describe */
			object_desc(o_name, o_ptr, TRUE, 3);

			/* Message */
			msg_format("You drop %s (%c).", o_name, index_to_label(item));

			/* Drop it (carefully) near the player */
			drop_near(o_ptr, 0, py, px);

			/* Modify, Describe, Optimize */
			inven_item_increase(item, -255);
			inven_item_describe(item);
			inven_item_optimize(item);
				
			/* Notice stuff (if needed) */
			if (p_ptr->notice) notice_stuff();

			/* Update stuff (if needed) */
			if (p_ptr->update) update_stuff();

			/* Redraw stuff (if needed) */
			if (p_ptr->redraw) redraw_stuff();

			/* Redraw stuff (if needed) */
			if (p_ptr->window) window_stuff();
		}


		/* Hack -- cancel "lurking browse mode" */
		if (!command_new) command_see = FALSE;


		/* Assume free turn */
		energy_use = 0;


		/* Paralyzed or Knocked Out */
		if ((p_ptr->paralyzed) || (p_ptr->stun >= 100))
		{
			/* Take a turn */
			energy_use = 100;
		}

		/* Resting */
		else if (resting)
		{
			/* Timed rest */
			if (resting > 0)
			{
				/* Reduce rest count */
				resting--;

				/* Redraw the state */
				p_ptr->redraw |= (PR_STATE);
			}

			/* Take a turn */
			energy_use = 100;
		}

		/* Running */
		else if (running)
		{
			/* Take a step */
			run_step(0);
		}

		/* Repeated command */
		else if (command_rep)
		{
			/* Count this execution */
			command_rep--;

			/* Redraw the state */
			p_ptr->redraw |= (PR_STATE);

			/* Redraw stuff */
			redraw_stuff();

			/* Hack -- Assume messages were seen */
			msg_flag = FALSE;

			/* Clear the top line */
			prt("", 0, 0);

			/* Process the command */
			process_command();
		}

		/* Normal command */
		else
		{
			/* Place the cursor on the player */
			move_cursor_relative(py, px);

			/* Get a command (normal) */
			request_command(FALSE);

			/* Process the command */
			process_command();
		}


		/*** Clean up ***/

		/* Significant */
		if (energy_use)
		{
			/* Use some energy */
			p_ptr->energy -= energy_use;


			/* Hack -- constant hallucination */
			if (p_ptr->image) p_ptr->redraw |= (PR_MAP);


			/* Shimmer monsters if needed */
			if (!avoid_other && shimmer_monsters)
			{
				/* Clear the flag */
				shimmer_monsters = FALSE;

				/* Shimmer multi-hued monsters */
				for (i = 1; i < m_max; i++)
				{
					monster_type *m_ptr;
					monster_race *r_ptr;

					/* Access monster */
					m_ptr = &m_list[i];

					/* Skip dead monsters */
					if (!m_ptr->r_idx) continue;

					/* Access the monster race */
					r_ptr = &r_info[m_ptr->r_idx];

					/* Skip non-multi-hued monsters */
					if (!(r_ptr->flags1 & (RF1_ATTR_MULTI))) continue;

					/* Reset the flag */
					shimmer_monsters = TRUE;

					/* Redraw regardless */
					lite_spot(m_ptr->fy, m_ptr->fx);
				}
			}


			/* Handle monster detection */
			if (repair_monsters)
			{
				/* Reset the flag */
				repair_monsters = FALSE;

				/* Rotate detection flags */
				for (i = 1; i < m_max; i++)
				{
					monster_type *m_ptr;

					/* Access monster */
					m_ptr = &m_list[i];

					/* Skip dead monsters */
					if (!m_ptr->r_idx) continue;

					/* Nice monsters get mean */
					if (m_ptr->mflag & (MFLAG_NICE))
					{
						/* Nice monsters get mean */
						m_ptr->mflag &= ~(MFLAG_NICE);
					}

					/* Handle memorized monsters */
					if (m_ptr->mflag & (MFLAG_MARK))
					{
						/* Maintain detection */
						if (m_ptr->mflag & (MFLAG_SHOW))
						{
							/* Forget flag */
							m_ptr->mflag &= ~(MFLAG_SHOW);
							
							/* Still need repairs */
							repair_monsters = TRUE;
						}

						/* Remove detection */
						else
						{
							/* Forget flag */
							m_ptr->mflag &= ~(MFLAG_MARK);

							/* Assume invisible */
							m_ptr->ml = FALSE;

							/* Update the monster */
							update_mon(i, FALSE);

							/* Redraw regardless */
							lite_spot(m_ptr->fy, m_ptr->fx);
						}
					}
				}
			}
		}


		/* Hack -- notice death or departure */
		if (!alive || death || new_level_flag) break;
	}
}


/*
 * Interact with the current dungeon level.
 *
 * This function will not exit until the level is completed,
 * the user dies, or the game is terminated.
 */
static void dungeon(void)
{
	/* Reset various flags */
	new_level_flag = FALSE;
	hack_mind = FALSE;

	/* Reset the "command" vars */
	command_cmd = 0;
	command_new = 0;
	command_rep = 0;
	command_arg = 0;
	command_dir = 0;

	/* Cancel the target */
	target_who = 0;

	/* Cancel the health bar */
	health_track(0);

	/* Check visual effects */
	shimmer_monsters = TRUE;
	shimmer_objects = TRUE;
	repair_monsters = TRUE;
	repair_objects = TRUE;

	/* Disturb */
	disturb(1, 0);

	/* Track maximum player level */
	if (p_ptr->max_plv < p_ptr->lev)
	{
		p_ptr->max_plv = p_ptr->lev;
	}

	/* Track maximum dungeon level */
	if (p_ptr->max_dlv < dun_level)
	{
		p_ptr->max_dlv = dun_level;
	}

	/* Paranoia -- No stairs down from Quest (unless ghostly - G) */
	if (is_quest(dun_level, FALSE) && !p_ptr->astral)
		create_down_stair = FALSE;

	/* Paranoia -- No stairs down from level 97 if ghostly -- Gumby */
	if (p_ptr->astral && (dun_level == 97))
		create_down_stair = FALSE;

	/* Paranoia -- no stairs from town */
	if (!dun_level) create_down_stair = create_up_stair = FALSE;

	/* Option -- no connected stairs */
	if (!dungeon_stair) create_down_stair = create_up_stair = FALSE;

	/* Make a stairway. */
	if (create_up_stair || create_down_stair)
	{
		/* Place a stairway */
		if (cave_valid_bold(py, px))
		{
			/* XXX XXX XXX */
			delete_object(py, px);

			/* Make stairs */
			if (create_down_stair)
			{
				cave_set_feat(py, px, FEAT_MORE);
			}
			else
			{
				cave_set_feat(py, px, FEAT_LESS);
			}
		}

		/* Cancel the stair request */
		create_down_stair = create_up_stair = FALSE;
	}

	/* Choose a panel row */
	panel_row = ((py - SCREEN_HGT / 4) / (SCREEN_HGT / 2));
	if (panel_row > max_panel_rows) panel_row = max_panel_rows;
	else if (panel_row < 0) panel_row = 0;

	/* Choose a panel col */
	panel_col = ((px - SCREEN_WID / 4) / (SCREEN_WID / 2));
	if (panel_col > max_panel_cols) panel_col = max_panel_cols;
	else if (panel_col < 0) panel_col = 0;

	/* Recalculate the boundaries */
	panel_bounds();

	/* Flush messages */
	msg_print(NULL);

	/* Enter "xtra" mode */
	character_xtra = TRUE;

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_SPELL | PW_PLAYER);

	/* Window stuff */
	p_ptr->window |= (PW_MONSTER | PW_VISIBLE);

	/* Redraw dungeon */
	p_ptr->redraw |= (PR_WIPE | PR_BASIC | PR_EXTRA | PR_EQUIPPY);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD);

	/* Update stuff */
	p_ptr->update |= (PU_BONUS | PU_HP | PU_MANA | PU_SPELLS);

	/* Calculate torch radius */
	p_ptr->update |= (PU_TORCH);

	/* Update stuff */
	update_stuff();

	/* Redraw stuff */
	redraw_stuff();

	/* Redraw stuff */
	window_stuff();

	/* Update stuff */
	p_ptr->update |= (PU_VIEW | PU_LITE | PU_FLOW | PU_DISTANCE);

	/* Update stuff */
	update_stuff();

	/* Redraw stuff */
	redraw_stuff();

	/* Leave "xtra" mode */
	character_xtra = FALSE;

	/* Update stuff */
	p_ptr->update |= (PU_BONUS | PU_HP | PU_MANA | PU_SPELLS);

	/* Combine / Reorder the pack */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Notice stuff */
	notice_stuff();

	/* Update stuff */
	update_stuff();

	/* Redraw stuff */
	redraw_stuff();

	/* Window stuff */
	window_stuff();

	/* Refresh */
	Term_fresh();

	/* Announce (or repeat) the feeling */
	if (dun_level) do_cmd_feeling();

	/* Hack -- notice death or departure */
	if (!alive || death || new_level_flag) return;

	/* Print quest message if appropriate */
	if (is_quest(dun_level, FALSE)) quest_discovery();

	/*** Process this dungeon level ***/

	/* Reset the monster generation level */
	monster_level = dun_level;

	/* Reset the object generation level */
	object_level = dun_level;

	hack_mind = TRUE;

	/* Main loop */
	while (TRUE)
	{
		/* Hack -- Compact the monster list occasionally */
		if (m_cnt + 32 > MAX_M_IDX) compact_monsters(64);

		/* Hack -- Compress the monster list occasionally */
		if (m_cnt + 32 < m_max) compact_monsters(0);

		/* Hack -- Compact the object list occasionally */
		if (o_cnt + 32 > MAX_O_IDX) compact_objects(64);

		/* Hack -- Compress the object list occasionally */
		if (o_cnt + 32 < o_max) compact_objects(0);

		/* Process the player */
		process_player();

		/* Notice stuff */
		if (p_ptr->notice) notice_stuff();

		/* Update stuff */
		if (p_ptr->update) update_stuff();

		/* Redraw stuff */
		if (p_ptr->redraw) redraw_stuff();

		/* Redraw stuff */
		if (p_ptr->window) window_stuff();

		/* Hack -- Hilite the player */
		move_cursor_relative(py, px);

		/* Optional fresh */
		if (fresh_after) Term_fresh();

		/* Hack -- Notice death or departure */
		if (!alive || death || new_level_flag) break;

		/* Process all of the monsters */
		process_monsters();

		/* Notice stuff */
		if (p_ptr->notice) notice_stuff();

		/* Update stuff */
		if (p_ptr->update) update_stuff();

		/* Redraw stuff */
		if (p_ptr->redraw) redraw_stuff();

		/* Redraw stuff */
		if (p_ptr->window) window_stuff();

		/* Hack -- Hilite the player */
		move_cursor_relative(py, px);

		/* Optional fresh */
		if (fresh_after) Term_fresh();

		/* Hack -- Notice death or departure */
		if (!alive || death || new_level_flag) break;


		/* Process the world */
		process_world();

		/* Notice stuff */
		if (p_ptr->notice) notice_stuff();

		/* Update stuff */
		if (p_ptr->update) update_stuff();

		/* Redraw stuff */
		if (p_ptr->redraw) redraw_stuff();

		/* Window stuff */
		if (p_ptr->window) window_stuff();

		/* Hack -- Hilite the player */
		move_cursor_relative(py, px);

		/* Optional fresh */
		if (fresh_after) Term_fresh();

		/* Hack -- Notice death or departure */
		if (!alive || death || new_level_flag) break;


		/* Count game turns */
		turn++;
	}
}


/*
 * Load some "user pref files"
 *
 * Modified by Arcum Dagsson to support
 * separate macro files for different realms.
 */
static void load_all_pref_files(void)
{
#ifdef SET_UID
	char *homedir;
#endif
	char buf[1024];

	/* Access the "race" pref file */
	sprintf(buf, "%s.prf", rp_ptr->title);

	/* Process that file */
	process_pref_file(buf);

	/* Access the "class" pref file */
	sprintf(buf, "%s.prf", cp_ptr->title);

	/* Process that file */
	process_pref_file(buf);

	/* Access the "character" pref file */
	sprintf(buf, "%s.prf", player_base);

	/* Process that file */
	process_pref_file(buf);

	/* Access the "realm 1" pref file */
	if (p_ptr->realm1 != REALM_NONE)
	{
		sprintf(buf, "%s.prf", realm_names[p_ptr->realm1]);

		/* Process that file */
		process_pref_file(buf);
	}

	/* Access the "realm 2" pref file */
	if (p_ptr->realm2 != REALM_NONE)
	{
		sprintf(buf, "%s.prf", realm_names[p_ptr->realm2]);

		/* Process that file */
		process_pref_file(buf);
	}

#ifdef SET_UID
	/* Allow players on UNIX systems to keep a pref file in their
		home directory. */

	if ((homedir = getenv("HOME")))
	{
		path_build(buf, 1024, homedir, ".angband.prf");
		(void)process_pref_file(buf);
	}
#endif /* SET_UID */
}

/*
 * Actually play a game
 *
 * If the "new_game" parameter is true, then, after loading the
 * savefile, we will commit suicide, if necessary, to allow the
 * player to start a new game.
 */
void play_game(bool new_game)
{
	int i;
	hack_mutation = FALSE;

	/* Hack -- Character is "icky" */
	character_icky = TRUE;

	/* Hack -- turn off the cursor */
	(void)Term_set_cursor(0);

	/* Attempt to load */
	if (!load_player())
	{
		/* Oops */
		quit("broken savefile");
	}

	/* Nothing loaded */
	if (!character_loaded)
	{
		/* Make new player */
		new_game = TRUE;

		/* The dungeon is not ready */
		character_dungeon = FALSE;
	}

	/* Process old character */
	if (!new_game)
	{
		/* Process the player name */
		process_player_name(FALSE);
	}

	/* Init the RNG */
	if (Rand_quick)
	{
		u32b seed;

		/* Basic seed */
		seed = (time(NULL));

#ifdef SET_UID

		/* Mutate the seed on Unix machines */
		seed = ((seed >> 3) * (getpid() << 1));

#endif

		/* Use the complex RNG */
		Rand_quick = FALSE;

		/* Seed the "complex" RNG */
		Rand_state_init(seed);
	}

	/* Extract the options */
	for (i = 0; option_info[i].o_desc; i++)
	{
		int os = option_info[i].o_set;
		int ob = option_info[i].o_bit;

		/* Set the "default" options */
		if (option_info[i].o_var)
		{
			/* Set */
			if (option_flag[os] & (1L << ob))
			{
				/* Set */
				(*option_info[i].o_var) = TRUE;
			}
			
			/* Clear */
			else
			{
				/* Clear */
				(*option_info[i].o_var) = FALSE;
			}
		}
	}

	/* Roll new character */
	if (new_game)
	{
		/* The dungeon is not ready */
		character_dungeon = FALSE;

		/* Start in town */
		dun_level = 0;

		/* Hack -- seed for flavors */
		seed_flavor = rand_int(0x10000000);

		/* Hack -- seed for town layout */
		seed_town = rand_int(0x10000000);

		/* Roll up a new character */
		player_birth();

		if (quick_start) p_ptr->max_dlv = 5;

		/* Astral beings start in the dungeon -- Gumby */
		if (p_ptr->astral) dun_level = 96;

		/* Hack -- enter the world
		 * Hack -- Vampires start at night. - Gumby
		 */
		if (p_ptr->prace == RACE_VAMPIRE)
		{
			turn = (10L * TOWN_DAWN) / 2;
		}
		else
		{
			turn = 1;
		}
	}

	/* Flash a message */
	prt("Please wait...", 0, 0);

	/* Flush the message */
	Term_fresh();

	/* Hack -- Enter wizard mode */
	if (arg_wizard && enter_wizard_mode()) wizard = TRUE;

	/* Flavor the objects */
	flavor_init();

	/* Reset the visual mappings */
	reset_visuals();

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_SPELL | PW_PLAYER);

	/* Window stuff */
	p_ptr->window |= (PW_MONSTER | PW_VISIBLE);

	/* Window stuff */
	window_stuff();

	/* Load the "pref" files */
	load_all_pref_files();

	/* Set or clear "rogue_like_commands" if requested */
	if (arg_force_original) rogue_like_commands = FALSE;
	if (arg_force_roguelike) rogue_like_commands = TRUE;

	/* React to changes */
	Term_xtra(TERM_XTRA_REACT, 0);

	/* Generate a dungeon level if needed */
	if (!character_dungeon) generate_cave();

	/* Character is now "complete" */
	character_generated = TRUE;

	/* Hack -- Character is no longer "icky" */
	character_icky = FALSE;

	/* Start game */
	alive = TRUE;

	/* Hack -- Enforce "delayed death" */
	if (p_ptr->chp < 0) death = TRUE;

	/* Process */
	while (TRUE)
	{
		/* Process the level */
		dungeon();

		/* Notice stuff */
		if (p_ptr->notice) notice_stuff();

		/* Update stuff */
		if (p_ptr->update) update_stuff();

		/* Redraw stuff */
		if (p_ptr->redraw) redraw_stuff();

		/* Window stuff */
		if (p_ptr->window) window_stuff();

		/* Cancel the target */
		target_who = 0;

		/* Cancel the health bar */
		health_track(0);

		/* Forget the lite */
		forget_lite();

		/* Forget the view */
		forget_view();

		/* Handle "quit and save" */
		if (!alive && !death) break;

		/* Erase the old cave */
		wipe_o_list();
		wipe_m_list();

		msg_print(NULL);

		/* Accidental Death */
		if (alive && death)
		{
			/* Mega-Hack -- Allow player to cheat death */
			if ((wizard || cheat_live) && !get_check("Die? "))
			{
				/* Mark social class, reset age, if needed */
				if (p_ptr->sc) p_ptr->sc = p_ptr->age = 0;

				/* Increase age */
				p_ptr->age++;

				/* Mark savefile */
				noscore |= 0x0001;

				/* Message */
				msg_print("You invoke wizard mode and cheat death.");
				msg_print(NULL);

				/* Restore hit points */
				p_ptr->chp = p_ptr->mhp;
				p_ptr->chp_frac = 0;

				/* Restore spell points */
				p_ptr->csp = p_ptr->msp;
				p_ptr->csp_frac = 0;

				/* Hack -- Healing */
				(void)set_blind(0);
				(void)set_confused(0);
				(void)set_poisoned(0);
				(void)set_afraid(0);
				(void)set_paralyzed(0);
				(void)set_image(0);
				(void)set_stun(0);
				(void)set_cut(0);

				/* Hack -- Prevent starvation */
				(void)set_food(PY_FOOD_MAX - 1);

				/* Hack -- cancel recall */
				if (p_ptr->word_recall)
				{
					/* Message */
					msg_print("A tension leaves the air around you...");
					msg_print(NULL);

					/* Hack -- Prevent recall */
					p_ptr->word_recall = 0;
				}

				/* Note cause of death XXX XXX XXX */
				(void)strcpy(died_from, "Cheating death");

				/* Teleport to town */
				new_level_flag = TRUE;

				/* Go to town */
				if (p_ptr->astral)
					dun_level = 96;
				else
					dun_level = 0;

				/* Do not die */
				death = FALSE;
			}
		}

		/* Handle "death" */
		if (death) break;

		/* Make a new level */
		generate_cave();
	}

	/* Close stuff */
	close_game();

	/* Quit */
	quit(NULL);
}
