/*
 * File: spells2.c
 * Purpose: Various assorted spell effects
 *
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This work is free software; you can redistribute it and/or modify it
 * under the terms of either:
 *
 * a) the GNU General Public License as published by the Free Software
 *    Foundation, version 2, or
 *
 * b) the "Angband licence":
 *    This software may be copied and distributed for educational, research,
 *    and not for profit purposes provided that this copyright and statement
 *    are included in all such copies.  Other copyrights may also apply.
 */
#include "angband.h"
#include "object/tvalsval.h"


/*
 * Increase players hit points, notice effects
 */
bool hp_player(int num)
{
	/* Healing needed */
	if (p_ptr->chp < p_ptr->mhp)
	{
		/* Gain hitpoints */
		p_ptr->chp += INT2S16B(num);

		/* Enforce maximum */
		if (p_ptr->chp >= p_ptr->mhp)
		{
			p_ptr->chp = p_ptr->mhp;
			p_ptr->chp_frac = 0;
		}

		/* Redraw */
		p_ptr->redraw |= (PR_HP);

		/* Print a nice message */
		if (num < 5)
			msg_print("You feel a little better.");
		else if (num < 15)
			msg_print("You feel better.");
		else if (num < 35)
			msg_print("You feel much better.");
		else
			msg_print("You feel very good.");

		/* Notice */
		return (TRUE);
	}

	/* Ignore */
	return (FALSE);
}


/*
 * Heal the player by a given percentage of his wounds, or a minimum
 * amount, whichever is larger.
 *
 * Copied wholesale from EyAngband.
 */
bool heal_player(int perc, int min)
{
	int i;

	/* Paranoia */
	if ((perc <= 0) && (min <= 0)) return (FALSE);


	/* No healing needed */
	if (p_ptr->chp >= p_ptr->mhp) return (FALSE);

	/* Figure healing level */
	i = ((p_ptr->mhp - p_ptr->chp) * perc) / 100;

	/* Enforce minimums */
	if (i < min) i = min;

	/* Actual healing */
	return hp_player(i);
}





/*
 * Leave a "glyph of warding" which prevents monster movement
 */
void warding_glyph(void)
{
	s16b py = p_ptr->py;
	s16b px = p_ptr->px;

	if (cave_feat[py][px] != FEAT_FLOOR)
	{
		msg_print("There is no clear floor on which to cast the spell.");
		return;
	}

	/* Create a glyph */
	cave_set_feat(py, px, FEAT_GLYPH);
}




/*
 * Array of stat "descriptions"
 */
static cptr desc_stat_pos[] =
{
	"strong",
	"smart",
	"wise",
	"dextrous",
	"healthy",
	"cute"
};


/*
 * Array of stat "descriptions"
 */
static cptr desc_stat_neg[] =
{
	"weak",
	"stupid",
	"naive",
	"clumsy",
	"sickly",
	"ugly"
};


/*
 * Lose a "point"
 */
bool do_dec_stat(s16b stat, bool perma)
{
	bool sust = FALSE;

	/* Get the "sustain" */
	switch (stat)
	{
		case A_STR: if (p_ptr->state.sustain_str) sust = TRUE; break;
		case A_INT: if (p_ptr->state.sustain_int) sust = TRUE; break;
		case A_WIS: if (p_ptr->state.sustain_wis) sust = TRUE; break;
		case A_DEX: if (p_ptr->state.sustain_dex) sust = TRUE; break;
		case A_CON: if (p_ptr->state.sustain_con) sust = TRUE; break;
		case A_CHR: if (p_ptr->state.sustain_chr) sust = TRUE; break;
	}

	/* Sustain */
	if (sust && !perma)
	{
		/* Message */
		msg_format("You feel very %s for a moment, but the feeling passes.",
		           desc_stat_neg[stat]);

		assert(TR2_SUST_STR == (1<<A_STR)); 
			object_notice_flag(2, 1<<stat);

		/* Notice effect */
		return (TRUE);
	}

	/* Attempt to reduce the stat */
	if (dec_stat(stat, perma))
	{
		/* Message */
		message_format(MSG_DRAIN_STAT, stat, "You feel very %s.", desc_stat_neg[stat]);

		/* Notice effect */
		return (TRUE);
	}

	/* Nothing obvious */
	return (FALSE);
}


/*
 * Restore lost "points" in a stat
 */
bool do_res_stat(int stat)
{
	/* Attempt to increase */
	if (res_stat(stat))
	{
		/* Message */
		msg_format("You feel less %s.", desc_stat_neg[stat]);

		/* Notice */
		return (TRUE);
	}

	/* Nothing obvious */
	return (FALSE);
}


/*
 * Gain a "point" in a stat
 */
bool do_inc_stat(int stat)
{
	bool res;

	/* Restore strength */
	res = res_stat(stat);

	/* Attempt to increase */
	if (inc_stat(stat))
	{
		/* Message */
		msg_format("You feel very %s!", desc_stat_pos[stat]);

		/* Notice */
		return (TRUE);
	}

	/* Restoration worked */
	if (res)
	{
		/* Message */
		msg_format("You feel less %s.", desc_stat_neg[stat]);

		/* Notice */
		return (TRUE);
	}

	/* Nothing obvious */
	return (FALSE);
}



/*
 * Identify everything being carried.
 * Done by a potion of "self knowledge".
 */
void identify_pack(void)
{
	int i;

	/* Simply identify and know every item */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		object_type *o_ptr = &inventory[i];

		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;

		/* Aware and Known */
		if (object_known_p(o_ptr)) continue;

		/* Identify it */
		do_ident_item(i, o_ptr);

		/* repeat with same slot */
		i--;
	}
}






/*
 * Used by the "enchant" function (chance of failure)
 */
static const int enchant_table[16] =
{
	0, 10,  50, 100, 200,
	300, 400, 500, 700, 950,
	990, 992, 995, 997, 999,
	1000
};


/*
 * Hack -- Removes curse from an object.
 */
static void uncurse_object(object_type *o_ptr)
{
	/* Uncurse it */
	o_ptr->flags[3] &= ~(TR3_CURSE_MASK);

	/* Mark as uncursed */
	o_ptr->known_flags[3] &= ~(TR3_CURSE_MASK);
}


/*
 * Removes curses from items in inventory.
 *
 * \param heavy removes heavy curses if true
 * \returns number of items uncursed
 */
static int remove_curse_aux(bool heavy)
{
	int i, cnt = 0;

	/* Attempt to uncurse items being worn */
	for (i = INVEN_WIELD; i < INVEN_TOTAL; i++)
	{
		object_type *o_ptr = &inventory[i];

		if (!o_ptr->k_idx) continue;

		if (!cursed_p(o_ptr)) continue;

		/* Heavily cursed items need a special spell */ 
		if ((o_ptr->flags[3] & TR3_HEAVY_CURSE) && !heavy) continue; 
 	
		/* Perma-cursed items can never be removed */ 
		if (o_ptr->flags[3] & TR3_PERMA_CURSE) continue; 

		/* Uncurse, and update things */
		uncurse_object(o_ptr);

		p_ptr->update |= (PU_BONUS);

		p_ptr->redraw |= (PR_EQUIP);

		/* Count the uncursings */
		cnt++;
	}

	/* Return "something uncursed" */
	return (cnt);
}


/*
 * Remove most curses
 */
bool remove_curse(void)
{
	return ((bool) remove_curse_aux(FALSE)); /* TODO Check why returns int */
}

/*
 * Remove all curses
 */
bool remove_all_curse(void)
{
	return ((bool) remove_curse_aux(TRUE));
}



/*
 * Restores any drained experience
 */
bool restore_level(void)
{
	/* Restore experience */
	if (p_ptr->exp < p_ptr->max_exp)
	{
		/* Message */
		msg_print("You feel your life energies returning.");

		/* Restore the experience */
		p_ptr->exp = p_ptr->max_exp;

		/* Check the experience */
		check_experience();

		/* Did something */
		return (TRUE);
	}

	/* No effect */
	return (FALSE);
}


/*
 * Hack -- acquire self knowledge
 *
 * List various information about the player and/or his current equipment.
 *
 * This tests the flags of the equipment being carried and the innate player
 * flags, so any changes made in calc_bonuses need to be shadowed here.
 *
 * Use the "roff()" routines, perhaps.  XXX XXX XXX
 *
 * Use the "show_file()" method, perhaps.  XXX XXX XXX
 *
 * This function cannot display more than 20 lines.  XXX XXX XXX
 */
void self_knowledge(bool spoil)
{
	int i = 0, j;
	s16b k;
	int max_x = Term->hgt - 2;

	u32b t[OBJ_FLAG_N];

	u32b f[OBJ_FLAG_N];

	object_type *o_ptr;

	cptr info[128];

	f[0] = f[1] = f[2] = f[3] = 0L;


	/* Get item flags from equipment */
	for (k = INVEN_WIELD; k < INVEN_TOTAL; k++)
	{
		o_ptr = &inventory[k];

		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;

		/* Extract the flags */
		if (spoil)
			object_flags(o_ptr, t);
		else 
			object_flags_known(o_ptr, t);

		/* Extract flags */
		f[0] |= t[0];
		f[1] |= t[1];
		f[2] |= t[2];
		f[3] |= t[3];
	}

	/* And flags from the player */
	player_flags(&t[0], &t[1], &t[2], &t[3]);

	/* Extract flags */
	f[0] |= t[0];
	f[1] |= t[1];
	f[2] |= t[2];
	f[3] |= t[3];


	if (p_ptr->timed[TMD_BLIND])
		info[i++] = "You cannot see.";

	if (p_ptr->timed[TMD_CONFUSED])
		info[i++] = "You are confused.";

	if (p_ptr->timed[TMD_AFRAID])
		info[i++] = "You are terrified.";

	if (p_ptr->timed[TMD_TERROR])
		info[i++] = "You are terrified and feel the need to run away.";

	if (p_ptr->timed[TMD_CUT])
		info[i++] = "You are bleeding.";

	if (p_ptr->timed[TMD_STUN])
		info[i++] = "You are stunned.";

	if (p_ptr->timed[TMD_POISONED])
		info[i++] = "You are poisoned.";

	if (p_ptr->timed[TMD_IMAGE])
		info[i++] = "You are hallucinating.";

	if (f[3] & TR3_AGGRAVATE)
		info[i++] = "You aggravate monsters.";

	if (f[3] & TR3_TELEPORT)
		info[i++] = "Your position is very uncertain.";

	if (p_ptr->timed[TMD_BLESSED])
		info[i++] = "You feel righteous.";

	if (p_ptr->timed[TMD_HERO])
		info[i++] = "You feel heroic.";

	if (p_ptr->timed[TMD_SHERO])
		info[i++] = "You are in a battle rage.";

	if (p_ptr->timed[TMD_PROTEVIL])
		info[i++] = "You are protected from evil.";

	if (p_ptr->timed[TMD_SHIELD])
		info[i++] = "You are protected by a mystic shield.";

	if (p_ptr->timed[TMD_REFLECT])
		info[i++] = "You reflect most physical attacks.";

	if (p_ptr->timed[TMD_FIRE]) /* Check wording */
		info[i++] = "Fire flares from your hands.";

	if (p_ptr->timed[TMD_COLD])
		info[i++] = "Moisture freezes from the air around you.";

	if (p_ptr->timed[TMD_ACID])
		info[i++] = "Acide drips fom your hands.";

	if (p_ptr->timed[TMD_ELEC])
		info[i++] = "Sparks fly from your fingers.";

	if (p_ptr->timed[TMD_INVULN])
		info[i++] = "You are temporarily invulnerable.";

	if (p_ptr->confusing)
		info[i++] = "Your hands are glowing dull red.";

	if (p_ptr->searching)
		info[i++] = "You are looking around very carefully.";

	if (p_ptr->new_spells)
		info[i++] = "You can learn some spells/prayers."; /* TODO : Say which */

	if (p_ptr->word_recall)
		info[i++] = "You will soon be recalled.";

	if (rp_ptr->infra || f[0] & TR0_INFRA1 || f[0] & TR0_INFRA2)
		info[i++] = "Your eyes are sensitive to infrared light.";

	if (f[3] & TR3_SLOW_DIGEST)
		info[i++] = "Your appetite is small.";

	if (f[3] & TR3_FEATHER)
		info[i++] = "You land gently.";

	if ((f[3] & TR3_REGEN) || ((f[1] & TR1_REGEN25) && (p_ptr->lev >= 25)))
		info[i++] = "You regenerate quickly.";

	if (f[3] & TR3_TELEPATHY)
		info[i++] = "You have ESP.";

	if (f[3] & TR3_SEE_INVIS)
		info[i++] = "You can see invisible creatures.";

	if (f[3] & TR3_FREE_ACT)
		info[i++] = "You have free action.";

	if (f[2] & TR2_HOLD_LIFE)
		info[i++] = "You have a firm hold on your life force.";

	if (f[2] & TR2_IM_ACID)
	{
		info[i++] = "You are completely immune to acid.";
	}
	else if ((f[2] & TR2_RES_ACID) && (p_ptr->timed[TMD_OPP_ACID]))
	{
		info[i++] = "You resist acid exceptionally well.";
	}
	else if ((f[2] & TR2_RES_ACID) || (p_ptr->timed[TMD_OPP_ACID]))
	{
		info[i++] = "You are resistant to acid.";
	}

	if (f[2] & TR2_IM_ELEC)
	{
		info[i++] = "You are completely immune to lightning.";
	}
	else if ((f[2] & TR2_RES_ELEC) && (p_ptr->timed[TMD_OPP_ELEC]))
	{
		info[i++] = "You resist lightning exceptionally well.";
	}
	else if ((f[2] & TR2_RES_ELEC) || (p_ptr->timed[TMD_OPP_ELEC]))
	{
		info[i++] = "You are resistant to lightning.";
	}

	if (f[2] & TR2_IM_FIRE)
	{
		info[i++] = "You are completely immune to fire.";
	}
	else if ((f[2] & TR2_RES_FIRE) && (p_ptr->timed[TMD_OPP_FIRE]))
	{
		info[i++] = "You resist fire exceptionally well.";
	}
	else if ((f[2] & TR2_RES_FIRE) || (p_ptr->timed[TMD_OPP_FIRE]))
	{
		info[i++] = "You are resistant to fire.";
	}

	if (f[2] & TR2_IM_COLD)
	{
		info[i++] = "You are completely immune to cold.";
	}
	else if ((f[2] & TR2_RES_COLD) && (p_ptr->timed[TMD_OPP_COLD]))
	{
		info[i++] = "You resist cold exceptionally well.";
	}
	else if ((f[2] & TR2_RES_COLD) || (p_ptr->timed[TMD_OPP_COLD]))
	{
		info[i++] = "You are resistant to cold.";
	}

	if ((f[2] & TR2_RES_POIS) && (p_ptr->timed[TMD_OPP_POIS]))
	{
		info[i++] = "You resist poison exceptionally well.";
	}
	else if ((f[2] & TR2_RES_POIS) || (p_ptr->timed[TMD_OPP_POIS]))
	{
		info[i++] = "You are resistant to poison.";
	}

	if (f[2] & TR2_RES_FEAR)
		info[i++] = "You are completely fearless.";

	if (f[2] & TR2_RES_LITE)
		info[i++] = "You are resistant to bright light.";

	if (f[2] & TR2_RES_DARK)
		info[i++] = "You are resistant to darkness.";

	if (f[2] & TR2_RES_BLIND)
		info[i++] = "Your eyes are resistant to blindness.";

	if (f[2] & TR2_RES_CONFU)
		info[i++] = "You are resistant to confusion.";

	if (f[2] & TR2_RES_SOUND)
		info[i++] = "You are resistant to sonic attacks.";

	if (f[2] & TR2_RES_SHARD)
		info[i++] = "You are resistant to blasts of shards.";

	if (f[2] & TR2_RES_NEXUS)
		info[i++] = "You are resistant to nexus attacks.";

	if (f[2] & TR2_RES_NETHR)
		info[i++] = "You are resistant to nether forces.";

	if (f[2] & TR2_RES_CHAOS)
		info[i++] = "You are resistant to chaos.";

	if (f[2] & TR2_RES_DISEN)
		info[i++] = "You are resistant to disenchantment.";

	if (f[2] & TR2_SUST_STR)
		info[i++] = "Your strength is sustained.";

	if (f[2] & TR2_SUST_INT)
		info[i++] = "Your intelligence is sustained.";

	if (f[2] & TR2_SUST_WIS)
		info[i++] = "Your wisdom is sustained.";

	if (f[2] & TR2_SUST_DEX)
		info[i++] = "Your dexterity is sustained.";

	if (f[2] & TR2_SUST_CON)
		info[i++] = "Your constitution is sustained.";

	if (f[2] & TR2_SUST_CHR)
		info[i++] = "Your charisma is sustained.";

	if ((f[0] & TR0_STR1) || (f[0] & TR0_STR2)) 
		info[i++] = "Your strength is affected by your equipment.";

	if ((f[0] & TR0_INT1) || (f[0] & TR0_INT2))
		info[i++] = "Your intelligence is affected by your equipment.";

	if ((f[0] & TR0_WIS1) || (f[0] & TR0_WIS2))
		info[i++] = "Your wisdom is affected by your equipment.";

	if ((f[0] & TR0_DEX1) || (f[0] & TR0_DEX2))
		info[i++] = "Your dexterity is affected by your equipment.";

	if ((f[0] & TR0_CON1) || (f[0] & TR0_CON2))
		info[i++] = "Your constitution is affected by your equipment.";

	if ((f[0] & TR0_CHR1) || (f[0] & TR0_CHR2))
		info[i++] = "Your charisma is affected by your equipment.";

	if ((f[0] & TR0_STEALTH1) || (f[0] & TR0_STEALTH2))
		info[i++] = "Your stealth is affected by your equipment.";

	if ((f[0] & TR0_SEARCH1) || (f[0] & TR0_SEARCH2))
		info[i++] = "Your searching ability is affected by your equipment.";

	if ((f[0] & TR0_INFRA1) || (f[0] & TR0_INFRA2))
		info[i++] = "Your infravision is affected by your equipment.";

	if ((f[0] & TR0_TUNNEL1) || (f[0] & TR0_TUNNEL2))
		info[i++] = "Your digging ability is affected by your equipment.";

	if (f[1] & TR1_SLOW5)
		info[i++] = "You are naturally slow.";

	if (f[1] & TR1_GOOD_SAVE)
		info[i++] = "You seem to be lucky in resisting spells and attacks.";

	if ((f[0] & TR0_SPEED1) || (f[0] & TR0_SPEED2))
		info[i++] = "Your speed is affected by your equipment.";

	if ((f[0] & TR0_BLOWS1) || (f[0] & TR0_BLOWS2))
		info[i++] = "Your attack speed is affected by your equipment.";

	if ((f[0] & TR0_SHOTS1) || (f[0] & TR0_SHOTS2))
		info[i++] = "Your shooting speed is affected by your equipment.";

	if (f[0] & TR0_MIGHT1) /* HACK There is no MIGHT2 */
		info[i++] = "Your shooting might is affected by your equipment.";

	/* TODO Don't forget to check for new flags (e.g. VAMPIRE) */

	/* Get the current weapon */
	o_ptr = &inventory[INVEN_WIELD];

	/* Analyze the weapon */
	if (o_ptr->k_idx)
	{
		/* Special "Attack Bonuses" */
		if (f[1] & TR1_BRAND_ACID)
			info[i++] = "Your weapon melts your foes.";
		if (f[1] & TR1_BRAND_ELEC)
			info[i++] = "Your weapon shocks your foes.";
		if (f[1] & TR1_BRAND_FIRE)
			info[i++] = "Your weapon burns your foes.";
		if (f[1] & TR1_BRAND_COLD)
			info[i++] = "Your weapon freezes your foes.";

		/* Special "slay" flags */
		if (f[1] & TR1_SLAY_ANIMAL)
			info[i++] = "Your weapon strikes at animals with extra force.";
		if (f[1] & TR1_SLAY_EVIL)
			info[i++] = "Your weapon strikes at evil with extra force.";
		if (f[1] & TR1_SLAY_UNDEAD)
			info[i++] = "Your weapon strikes at undead with holy wrath.";
		if (f[1] & TR1_SLAY_DEMON)
			info[i++] = "Your weapon strikes at demons with holy wrath.";
		if (f[1] & TR1_SLAY_ORC)
			info[i++] = "Your weapon is especially deadly against orcs.";
		if (f[1] & TR1_SLAY_TROLL)
			info[i++] = "Your weapon is especially deadly against trolls.";
		if (f[1] & TR1_SLAY_GIANT)
			info[i++] = "Your weapon is especially deadly against giants.";
		if (f[1] & TR1_SLAY_DRAGON)
			info[i++] = "Your weapon is especially deadly against dragons.";

		/* Special "kill" flags */
		if (f[1] & TR1_KILL_DRAGON) /* TODO Don't forget to add the new flags */
			info[i++] = "Your weapon is a great bane of dragons.";
		if (f[1] & TR1_KILL_DEMON)
			info[i++] = "Your weapon is a great bane of demons.";
		if (f[1] & TR1_KILL_UNDEAD)
			info[i++] = "Your weapon is a great bane of undead.";
		if (f[1] & (TR1_KILL_DEMON))
			info[i++] = "Your weapon is a great bane of demons.";
		if (f[1] & (TR1_KILL_ORC))
			info[i++] = "Your weapon is a great bane of orcs.";
		if (f[1] & (TR1_KILL_TROLL))
			info[i++] = "Your weapon is a great bane of trolls.";
		if (f[1] & (TR1_KILL_GIANT))
			info[i++] = "Your weapon is a great bane of giants.";
		if (f[1] & (TR1_KILL_DRAGON))
			info[i++] = "Your weapon is a great bane of dragons.";

		/* Indicate Blessing */
		if (f[3] & TR3_BLESSED)
			info[i++] = "Your weapon has been blessed by the gods.";

		/* Hack */ /* Why is this 'hackish' ? */
		if (f[3] & TR3_IMPACT)
			info[i++] = "Your weapon can induce earthquakes.";

	}


	/* Save screen */
	screen_save();


	/* Clear the screen */
	Term_clear();

	/* Label the information */
	prt("     Your Attributes:", 1, 0);

	/* Dump the info */
	for (k = 2, j = 0; j < i; j++)
	{
		/* Show the info */
		prt(info[j], k++, 0);

		/* Page wrap */
		if ((k == max_x) && (j+1 < i))
		{
			prt("-- more --", k, 0);
			anykey();

			/* Clear the screen */
			Term_clear();

			/* Label the information */
			prt("     Your Attributes:", 1, 0);

			/* Reset */
			k = 2;
		}
	}

	/* Pause */
	prt("[Press any key to continue]", k, 0);
	(void)anykey();


	/* Load screen */
	screen_load();
}



/*
 * Set word of recall as appropriate
 */
void set_recall(void)
{
	/* Ironman */
	if (OPT(adult_ironman) && !p_ptr->total_winner)
	{
		msg_print("Nothing happens.");
		return;
	}

	/* Activate recall */
	if (!p_ptr->word_recall)
	{
		/* Reset recall depth */
		if ((p_ptr->depth > 0) && (p_ptr->depth != p_ptr->max_depth))
		{
			/*
			 * ToDo: Add a new player_type field "recall_depth"
			 * ToDo: Poll: Always reset recall depth?
			 */
			 if (get_check("Reset recall depth? "))
				p_ptr->max_depth = p_ptr->depth;
		}

		p_ptr->word_recall = (s16b) randint0(20) + 15;
		msg_print("The air about you becomes charged...");
	}

	/* Deactivate recall */
	else
	{
		p_ptr->word_recall = 0;
		msg_print("A tension leaves the air around you...");
	}

	/* Redraw status line */
	p_ptr->redraw = PR_STATUS;
	handle_stuff();
}


/*** Detection spells ***/

/*
 * Useful constants for the area around the player to detect.
 * This is instead of using circular detection spells.
 */
#define DETECT_DIST_X	40	/* Detect 42 grids to the left & right */
#define DETECT_DIST_Y	22	/* Detect 22 grids to the top & bottom */



/*
 * Map an area around the player.
 *
 * We must never attempt to map the outer dungeon walls, or we
 * might induce illegal cave grid references.
 */
void map_area(void)
{
	s16b i, x, y;
	s16b x1, x2, y1, y2;

	/* Pick an area to map */
	y1 = p_ptr->py - DETECT_DIST_Y;
	y2 = p_ptr->py + DETECT_DIST_Y;
	x1 = p_ptr->px - DETECT_DIST_X;
	x2 = p_ptr->px + DETECT_DIST_X;

	if (y1 < 0) y1 = 0;
	if (x1 < 0) x1 = 0;

	/* Scan the dungeon */
	for (y = y1; y < y2; y++)
	{
		for (x = x1; x < x2; x++)
		{
			/* All non-walls are "checked" */
			if (cave_feat[y][x] < FEAT_SECRET)
			{
				if (!in_bounds_fully(y, x)) continue;

				/* Memorize normal features */
				if (cave_feat[y][x] > FEAT_INVIS)
				{
					/* Memorize the object */
					cave_info[y][x] |= (CAVE_MARK);
					lite_spot(y, x);
				}

				/* Memorize known walls */
				for (i = 0; i < 8; i++)
				{
					s16b yy = y + ddy_ddd[i];
					s16b xx = x + ddx_ddd[i];

					/* Memorize walls (etc) */
					if (cave_feat[yy][xx] >= FEAT_SECRET)
					{
						/* Memorize the walls */
						cave_info[yy][xx] |= (CAVE_MARK);
						lite_spot(yy, xx);
					}
				}
			}
		}
	}
}



/*
 * Detect traps around the player.
 */
bool detect_traps(bool aware)
{
	s16b y, x;
	s16b x1, x2, y1, y2;

	bool detect = FALSE;

	(void)aware;

	/* Pick an area to map */
	y1 = p_ptr->py - DETECT_DIST_Y;
	y2 = p_ptr->py + DETECT_DIST_Y;
	x1 = p_ptr->px - DETECT_DIST_X;
	x2 = p_ptr->px + DETECT_DIST_X;

	if (y1 < 0) y1 = 0;
	if (x1 < 0) x1 = 0;


	/* Scan the dungeon */
	for (y = y1; y < y2; y++)
	{
		for (x = x1; x < x2; x++)
		{
			if (!in_bounds_fully(y, x)) continue;

			/* Detect invisible traps */
			if (cave_feat[y][x] == FEAT_INVIS)
			{
				/* Pick a trap */
				pick_trap(y, x);
			}

			/* Detect traps */
			if ((cave_feat[y][x] >= FEAT_TRAP_HEAD) &&
			    (cave_feat[y][x] <= FEAT_TRAP_TAIL))
			{
				/* Hack -- Memorize */
				cave_info[y][x] |= (CAVE_MARK);

				/* We found something to detect */
				detect = TRUE;
			}

			/* Mark as trap-detected */
			cave_info2[y][x] |= (CAVE2_DTRAP);
		}
	}

	/* Rescan the map for the new dtrap edge */
	for (y = y1 - 1; y < y2 + 1; y++)
	{
		for (x = x1 - 1; x < x2 + 1; x++)
		{
			if (!in_bounds_fully(y, x)) continue;

			/* Redraw */
			lite_spot(y, x);
		}
	}


	/* Describe */
	if (detect)
		msg_print("You sense the presence of traps!");

	/* Trap detection always makes you aware, even if no traps are present */
	else
		msg_print("You sense no traps.");

	/* Mark the redraw flag */
	p_ptr->redraw |= (PR_DTRAP);

	/* Result */
	return (TRUE);
}



/*
 * Detect doors and stairs around the player.
 */
bool detect_doorstairs(bool aware)
{
	s16b y, x;
	s16b x1, x2, y1, y2;

	bool doors = FALSE, stairs = FALSE;


	/* Pick an area to map */
	y1 = p_ptr->py - DETECT_DIST_Y;
	y2 = p_ptr->py + DETECT_DIST_Y;
	x1 = p_ptr->px - DETECT_DIST_X;
	x2 = p_ptr->px + DETECT_DIST_X;

	if (y1 < 0) y1 = 0;
	if (x1 < 0) x1 = 0;


	/* Scan the dungeon */
	for (y = y1; y < y2; y++)
	{
		for (x = x1; x < x2; x++)
		{
			if (!in_bounds_fully(y, x)) continue;

			/* Detect secret doors */
			if (cave_feat[y][x] == FEAT_SECRET)
				place_closed_door(y, x);

			/* Detect doors */
			if (((cave_feat[y][x] >= FEAT_DOOR_HEAD) &&
			     (cave_feat[y][x] <= FEAT_DOOR_TAIL)) ||
			    ((cave_feat[y][x] == FEAT_OPEN) ||
			     (cave_feat[y][x] == FEAT_BROKEN)))
			{
				/* Hack -- Memorize */
				cave_info[y][x] |= (CAVE_MARK);

				/* Redraw */
				lite_spot(y, x);

				/* Obvious */
				doors = TRUE;
			}

			/* Detect stairs */
			if ((cave_feat[y][x] == FEAT_LESS) ||
			    (cave_feat[y][x] == FEAT_MORE))
			{
				/* Hack -- Memorize */
				cave_info[y][x] |= (CAVE_MARK);

				/* Redraw */
				lite_spot(y, x);

				/* Obvious */
				stairs = TRUE;
			}

		}
	}

	/* Describe */
	if (doors && !stairs)      msg_print("You sense the presence of doors!");
	else if (!doors && stairs) msg_print("You sense the presence of stairs!");
	else if (doors && stairs)  msg_print("You sense the presence of doors and stairs!");
	else if (aware && !doors && !stairs) msg_print("You sense no doors or stairs.");

	/* Result */
	return (doors || stairs);
}


/*
 * Detect all treasure around the player.
 */
bool detect_treasure(bool aware)
{
	int i;
	s16b y, x;
	s16b x1, x2, y1, y2;

	bool gold_buried = FALSE;
	bool objects = FALSE;


	/* Pick an area to map */
	y1 = p_ptr->py - DETECT_DIST_Y;
	y2 = p_ptr->py + DETECT_DIST_Y;
	x1 = p_ptr->px - DETECT_DIST_X;
	x2 = p_ptr->px + DETECT_DIST_X;

	if (y1 < 0) y1 = 0;
	if (x1 < 0) x1 = 0;


	/* Scan the dungeon */
	for (y = y1; y < y2; y++)
	{
		for (x = x1; x < x2; x++)
		{
			if (!in_bounds_fully(y, x)) continue;

			/* Notice embedded gold */
			if ((cave_feat[y][x] == FEAT_MAGMA_H) ||
			    (cave_feat[y][x] == FEAT_QUARTZ_H))
			{
				/* Expose the gold */
				cave_feat[y][x] += 0x02;
			}

			/* Magma/Quartz + Known Gold */
			if ((cave_feat[y][x] == FEAT_MAGMA_K) ||
			    (cave_feat[y][x] == FEAT_QUARTZ_K))
			{
				/* Hack -- Memorize */
				cave_info[y][x] |= (CAVE_MARK);

				/* Redraw */
				lite_spot(y, x);

				/* Detect */
				gold_buried = TRUE;
			}
		}
	}

	/* Scan objects */
	for (i = 1; i < o_max; i++)
	{
		object_type *o_ptr = &o_list[i];

		/* Skip dead objects */
		if (!o_ptr->k_idx) continue;

		/* Skip held objects */
		if (o_ptr->held_m_idx) continue;

		/* Location */
		y = o_ptr->iy;
		x = o_ptr->ix;

		/* Only detect nearby objects */
		if (x < x1 || y < y1 || x > x2 || y > y2) continue;

		/* Hack -- memorize it */
		o_ptr->marked = TRUE;

		/* Redraw */
		lite_spot(y, x);

		/* Detect */
		if (!squelch_hide_item(o_ptr))
			objects = TRUE;
	}

	if (gold_buried)
		msg_print("You sense the presence of buried treasure!");

	if (objects)
		msg_print("You sense the presence of objects!");

	if (aware && !gold_buried && !objects)
		msg_print("You sense no treasure or objects.");

	return gold_buried || objects;
}

/*
 * Detect gold around the player.
 */
bool detect_gold(bool aware)
{
	int i;
	s16b y, x;
	s16b x1, x2, y1, y2;

	bool gold = FALSE;

	/* Pick an area to map */
	y1 = p_ptr->py - DETECT_DIST_Y;
	y2 = p_ptr->py + DETECT_DIST_Y;
	x1 = p_ptr->px - DETECT_DIST_X;
	x2 = p_ptr->px + DETECT_DIST_X;

	if (y1 < 0) y1 = 0;
	if (x1 < 0) x1 = 0;


	/* Scan the dungeon */
	for (y = y1; y < y2; y++)
	{
		for (x = x1; x < x2; x++)
		{
			if (!in_bounds_fully(y, x)) continue;

			/* Notice embedded gold */
			if ((cave_feat[y][x] == FEAT_MAGMA_H) ||
			    (cave_feat[y][x] == FEAT_QUARTZ_H))
			{
				/* Expose the gold */
				cave_feat[y][x] += 0x02;
			}

			/* Magma/Quartz + Known Gold */
			if ((cave_feat[y][x] == FEAT_MAGMA_K) ||
			    (cave_feat[y][x] == FEAT_QUARTZ_K))
			{
				/* Hack -- Memorize */
				cave_info[y][x] |= (CAVE_MARK);

				/* Redraw */
				lite_spot(y, x);

				/* Detect */
				gold = TRUE;
			}
		}
	}

	/* Scan objects */
	for (i = 1; i < o_max; i++)
	{
		object_type *o_ptr = &o_list[i];

		/* Skip dead objects */
		if (!o_ptr->k_idx) continue;

		/* Skip held objects */
		if (o_ptr->held_m_idx) continue;

		/* Skip anything that isn't gold */
		if (o_ptr->tval != TV_GOLD)
			continue;

		/* Location */
		y = o_ptr->iy;
		x = o_ptr->ix;

		/* Only detect nearby objects */
		if (x < x1 || y < y1 || x > x2 || y > y2) continue;

		/* Hack -- memorize it */
		o_ptr->marked = TRUE;

		/* Redraw */
		lite_spot(y, x);

		/* Detect */
		gold = TRUE;
	}

	if (gold)
		msg_print("You sense the presence of buried treasure!");

	if (aware && !gold)
		msg_print("You sense no treasure.");

	return gold;
}

/*
 * Detect objects around the player.
 */
bool detect_objects(bool aware)
{
	int i;
	s16b y, x;
	s16b x1, x2, y1, y2;

	bool objects = FALSE;


	/* Pick an area to map */
	y1 = p_ptr->py - DETECT_DIST_Y;
	y2 = p_ptr->py + DETECT_DIST_Y;
	x1 = p_ptr->px - DETECT_DIST_X;
	x2 = p_ptr->px + DETECT_DIST_X;

	if (y1 < 0) y1 = 0;
	if (x1 < 0) x1 = 0;

	/* Scan objects */
	for (i = 1; i < o_max; i++)
	{
		object_type *o_ptr = &o_list[i];

		/* Skip dead objects */
		if (!o_ptr->k_idx) continue;

		/* Skip held objects */
		if (o_ptr->held_m_idx) continue;

		/* Location */
		y = o_ptr->iy;
		x = o_ptr->ix;

		/* Only detect nearby objects */
		if (x < x1 || y < y1 || x > x2 || y > y2) continue;

		/* Hack -- memorize it */
		o_ptr->marked = TRUE;

		/* Redraw */
		lite_spot(y, x);

		/* Detect */
		if (!squelch_hide_item(o_ptr))
			objects = TRUE;
	}

	if (objects)
		msg_print("You sense the presence of objects!");

	if (aware && !objects)
		msg_print("You sense no objects.");

	return objects;
}


/*
 * Detect "magic" objects around the player.
 *
 * This will light up all spaces with "magic" items, including artifacts,
 * ego-items, potions, scrolls, books, rods, wands, staves, amulets, rings,
 * and "enchanted" items of the "good" variety.
 *
 * It can probably be argued that this function is now too powerful.
 */
bool detect_objects_magic(bool aware)
{
	s16b i, y, x, tv;
	s16b x1, x2, y1, y2;

	bool detect = FALSE;


	/* Pick an area to map */
	y1 = p_ptr->py - DETECT_DIST_Y;
	y2 = p_ptr->py + DETECT_DIST_Y;
	x1 = p_ptr->px - DETECT_DIST_X;
	x2 = p_ptr->px + DETECT_DIST_X;

	if (y1 < 0) y1 = 0;
	if (x1 < 0) x1 = 0;


	/* Scan all objects */
	for (i = 1; i < o_max; i++)
	{
		object_type *o_ptr = &o_list[i];

		/* Skip dead objects */
		if (!o_ptr->k_idx) continue;

		/* Skip held objects */
		if (o_ptr->held_m_idx) continue;

		/* Location */
		y = o_ptr->iy;
		x = o_ptr->ix;


		/* Only detect nearby objects */
		if (x < x1 || y < y1 || x > x2 || y > y2) continue;

		/* Examine the tval */
		tv = o_ptr->tval;

		/* Artifacts, misc magic items, or enchanted wearables */ /* TODO: Book conversion */
		if (artifact_p(o_ptr) || ego_item_p(o_ptr) ||
		    (tv == TV_AMULET) || (tv == TV_RING) ||
		    (tv == TV_STAFF) || (tv == TV_WAND) || (tv == TV_ROD) ||
		    (tv == TV_SCROLL) || (tv == TV_SPELL) ||
			(tv == TV_POTION) || (tv == TV_BOOK) || 
		    ((o_ptr->to_a > 0) || (o_ptr->to_h + o_ptr->to_d > 0)))
		{
			/* Memorize the item */
			o_ptr->marked = TRUE;

			/* Redraw */
			lite_spot(y, x);

			/* Detect */
			if (!squelch_hide_item(o_ptr))
				detect = TRUE;
		}
	}

	if (detect)
		msg_print("You sense the presence of magic objects!");
	else if (aware && !detect)
		msg_print("You sense no magic objects.");

	return detect;
}


/*
 * Detect "normal" monsters around the player.
 */
bool detect_monsters_normal(bool aware)
{
	s16b i, y, x;
	s16b x1, x2, y1, y2;

	bool flag = FALSE;


	/* Pick an area to map */
	y1 = p_ptr->py - DETECT_DIST_Y;
	y2 = p_ptr->py + DETECT_DIST_Y;
	x1 = p_ptr->px - DETECT_DIST_X;
	x2 = p_ptr->px + DETECT_DIST_X;

	if (y1 < 0) y1 = 0;
	if (x1 < 0) x1 = 0;



	/* Scan monsters */
	for (i = 1; i < mon_max; i++)
	{
		monster_type *m_ptr = &mon_list[i];
		monster_race *r_ptr = &r_info[m_ptr->r_idx];

		/* Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Location */
		y = m_ptr->fy;
		x = m_ptr->fx;

		/* Only detect nearby monsters */
		if (x < x1 || y < y1 || x > x2 || y > y2) continue;

		/* Detect all non-invisible monsters */
		if (!(r_ptr->flags[1] & (RF1_INVISIBLE)))
		{
			/* Optimize -- Repair flags */
			repair_mflag_mark = repair_mflag_show = TRUE;

			/* Hack -- Detect the monster */
			m_ptr->mflag |= (MFLAG_MARK | MFLAG_SHOW);

			/* Update the monster */
			update_mon(i, FALSE);

			/* Detect */
			flag = TRUE;
		}
	}

	if (flag)
		msg_print("You sense the presence of monsters!");
	else if (aware && !flag)
		msg_print("You sense no monsters.");
		
	/* Result */
	return flag;
}


/*
 * Detect "invisible" monsters around the player.
 */
bool detect_monsters_invis(bool aware)
{
	s16b i, y, x;
	s16b x1, x2, y1, y2;

	bool flag = FALSE;

	/* Pick an area to map */
	y1 = p_ptr->py - DETECT_DIST_Y;
	y2 = p_ptr->py + DETECT_DIST_Y;
	x1 = p_ptr->px - DETECT_DIST_X;
	x2 = p_ptr->px + DETECT_DIST_X;

	if (y1 < 0) y1 = 0;
	if (x1 < 0) x1 = 0;


	/* Scan monsters */
	for (i = 1; i < mon_max; i++)
	{
		monster_type *m_ptr = &mon_list[i];
		monster_race *r_ptr = &r_info[m_ptr->r_idx];
		monster_lore *l_ptr = &l_list[m_ptr->r_idx];

		/* Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Location */
		y = m_ptr->fy;
		x = m_ptr->fx;

		/* Only detect nearby objects */
		if (x < x1 || y < y1 || x > x2 || y > y2) continue;

		/* Detect invisible monsters */
		if (r_ptr->flags[1] & (RF1_INVISIBLE))
		{
			/* Take note that they are invisible */
			l_ptr->flags[1] |= (RF1_INVISIBLE);

			/* Update monster recall window */
			if (p_ptr->monster_race_idx == m_ptr->r_idx)
			{
				/* Redraw stuff */
				p_ptr->redraw |= (PR_MONSTER);
			}

			/* Optimize -- Repair flags */
			repair_mflag_mark = repair_mflag_show = TRUE;

			/* Hack -- Detect the monster */
			m_ptr->mflag |= (MFLAG_MARK | MFLAG_SHOW);

			/* Update the monster */
			update_mon(i, FALSE);

			/* Detect */
			flag = TRUE;
		}
	}

	if (flag)
		msg_print("You sense the presence of invisible creatures!");
	else if (aware && !flag)
		msg_print("You sense no invisible creatures.");

	return (flag);
}



/*
 * Detect "evil" monsters around the player.
 */
bool detect_monsters_evil(bool aware)
{
	s16b i, y, x;
	s16b x1, x2, y1, y2;

	bool flag = FALSE;

	/* Pick an area to map */
	y1 = p_ptr->py - DETECT_DIST_Y;
	y2 = p_ptr->py + DETECT_DIST_Y;
	x1 = p_ptr->px - DETECT_DIST_X;
	x2 = p_ptr->px + DETECT_DIST_X;

	if (y1 < 0) y1 = 0;
	if (x1 < 0) x1 = 0;


	/* Scan monsters */
	for (i = 1; i < mon_max; i++)
	{
		monster_type *m_ptr = &mon_list[i];
		monster_race *r_ptr = &r_info[m_ptr->r_idx];
		monster_lore *l_ptr = &l_list[m_ptr->r_idx];

		/* Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Location */
		y = m_ptr->fy;
		x = m_ptr->fx;

		/* Only detect nearby objects */
		if (x < x1 || y < y1 || x > x2 || y > y2) continue;

		/* Detect evil monsters */
		if (r_ptr->flags[2] & (RF2_EVIL))
		{
			/* Take note that they are evil */
			l_ptr->flags[2] |= (RF2_EVIL);

			/* Update monster recall window */
			if (p_ptr->monster_race_idx == m_ptr->r_idx)
			{
				/* Redraw stuff */
				p_ptr->redraw |= (PR_MONSTER);
			}

			/* Optimize -- Repair flags */
			repair_mflag_mark = repair_mflag_show = TRUE;

			/* Detect the monster */
			m_ptr->mflag |= (MFLAG_MARK | MFLAG_SHOW);

			/* Update the monster */
			update_mon(i, FALSE);

			/* Detect */
			flag = TRUE;
		}
	}

	if (flag)
		msg_print("You sense the presence of evil creatures!");
	else if (aware && !flag)
		msg_print("You sense no evil creatures.");

	return flag;
}



/*
 * Detect everything
 */
bool detect_all(bool aware)
{
	bool detect = FALSE;

	/* Detect everything */
	if (detect_traps(aware)) detect = TRUE;
	if (detect_doorstairs(aware)) detect = TRUE;
	if (detect_treasure(aware)) detect = TRUE;
	if (detect_monsters_invis(aware)) detect = TRUE;
	if (detect_monsters_normal(aware)) detect = TRUE;

	/* Result */
	return (detect);
}



/*
 * Create stairs at the player location
 */
void stair_creation(void)
{
	s16b py = p_ptr->py;
	s16b px = p_ptr->px;

	/* XXX XXX XXX */
	if (!cave_valid_bold(py, px))
	{
		msg_print("The object resists the spell.");
		return;
	}

	/* XXX XXX XXX */
	delete_object(py, px);

	/* Create a staircase */
	if (!p_ptr->depth)
	{
		cave_set_feat(py, px, FEAT_MORE);
	}
	else if (is_quest(p_ptr->depth) || (p_ptr->depth >= MAX_DEPTH-1))
	{
		cave_set_feat(py, px, FEAT_LESS);
	}
	else if (randint0(100) < 50)
	{
		cave_set_feat(py, px, FEAT_MORE);
	}
	else
	{
		cave_set_feat(py, px, FEAT_LESS);
	}
}




/*
 * Hook to specify "weapon"
 */
static bool item_tester_hook_weapon(const object_type *o_ptr)
{
	switch (o_ptr->tval)
	{
		case TV_SWORD:
		case TV_HAFTED:
		case TV_POLEARM:
		case TV_DIGGING:
		case TV_BOW:
		case TV_BOLT:
		case TV_ARROW:
		case TV_SHOT:
		{
			return (TRUE);
		}
	}

	return (FALSE);
}


/*
 * Hook to specify "armor"
 */
static bool item_tester_hook_armor(const object_type *o_ptr)
{
	switch (o_ptr->tval)
	{
		case TV_DRAG_ARMOR:
		case TV_HARD_ARMOR:
		case TV_SOFT_ARMOR:
		case TV_SHIELD:
		case TV_CLOAK:
		case TV_CROWN:
		case TV_HELM:
		case TV_BOOTS:
		case TV_GLOVES:
		{
			return (TRUE);
		}
	}

	return (FALSE);
}


static bool item_tester_unknown(const object_type *o_ptr)
{
	if (object_known_p(o_ptr))
		return FALSE;
	else
		return TRUE;
}


/*
 * Enchant an item
 *
 * Revamped!  Now takes item pointer, number of times to try enchanting,
 * and a flag of what to try enchanting.  Artifacts resist enchantment
 * some of the time, and successful enchantment to at least +0 might
 * break a curse on the item.  -CFT
 *
 * Note that an item can technically be enchanted all the way to +15 if
 * you wait a very, very, long time.  Going from +9 to +10 only works
 * about 5% of the time, and from +10 to +11 only about 1% of the time.
 *
 * Note that this function can now be used on "piles" of items, and
 * the larger the pile, the lower the chance of success.
 */
bool enchant(object_type *o_ptr, int n, int eflag)
{
	int i, chance, prob;

	bool res = FALSE;

	bool a = artifact_p(o_ptr);

	u32b f[OBJ_FLAG_N];

	/* Extract the flags */
	object_flags(o_ptr, f);


	/* Large piles resist enchantment */
	prob = o_ptr->number * 100;

	/* Missiles are easy to enchant */
	if ((o_ptr->tval == TV_BOLT) ||
	    (o_ptr->tval == TV_ARROW) ||
	    (o_ptr->tval == TV_SHOT))
	{
		prob = prob / 20;
	}

	/* Try "n" times */
	for (i = 0; i < n; i++)
	{
		/* Hack -- Roll for pile resistance */
		if ((prob > 100) && (randint0(prob) >= 100)) continue;

		/* Enchant to hit */
		if (eflag & (ENCH_TOHIT))
		{
			if (o_ptr->to_h < 0) chance = 0;
			else if (o_ptr->to_h > 15) chance = 1000;
			else chance = enchant_table[o_ptr->to_h];

			/* Attempt to enchant */
			if ((randint1(1000) > chance) && (!a || (randint0(100) < 50)))
			{
				res = TRUE;

				/* Enchant */
				o_ptr->to_h++;

				/* Break curse */
				if (cursed_p(o_ptr) &&
				    (!(f[3] & (TR3_PERMA_CURSE))) &&
				    (o_ptr->to_h >= 0) && (randint0(100) < 25))
				{
					msg_print("The curse is broken!");

					/* Uncurse the object */
					uncurse_object(o_ptr);
				}
			}
		}

		/* Enchant to damage */
		if (eflag & (ENCH_TODAM))
		{
			if (o_ptr->to_d < 0) chance = 0;
			else if (o_ptr->to_d > 15) chance = 1000;
			else chance = enchant_table[o_ptr->to_d];

			/* Attempt to enchant */
			if ((randint1(1000) > chance) && (!a || (randint0(100) < 50)))
			{
				res = TRUE;

				/* Enchant */
				o_ptr->to_d++;

				/* Break curse */
				if (cursed_p(o_ptr) &&
				    (!(f[3] & (TR3_PERMA_CURSE))) &&
				    (o_ptr->to_d >= 0) && (randint0(100) < 25))
				{
					msg_print("The curse is broken!");

					/* Uncurse the object */
					uncurse_object(o_ptr);
				}
			}
		}

		/* Enchant to armor class */
		if (eflag & (ENCH_TOAC))
		{
			if (o_ptr->to_a < 0) chance = 0;
			else if (o_ptr->to_a > 15) chance = 1000;
			else chance = enchant_table[o_ptr->to_a];

			/* Attempt to enchant */
			if ((randint1(1000) > chance) && (!a || (randint0(100) < 50)))
			{
				res = TRUE;

				/* Enchant */
				o_ptr->to_a++;

				/* Break curse */
				if (cursed_p(o_ptr) &&
				    (!(f[3] & (TR3_PERMA_CURSE))) &&
				    (o_ptr->to_a >= 0) && (randint0(100) < 25))
				{
					msg_print("The curse is broken!");

					/* Uncurse the object */
					uncurse_object(o_ptr);
				}
			}
		}
	}

	/* Failure */
	if (!res) return (FALSE);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Redraw stuff */
	p_ptr->redraw |= (PR_INVEN | PR_EQUIP );

	/* Success */
	return (TRUE);
}



/*
 * Enchant an item (in the inventory or on the floor)
 * Note that "num_ac" requires armor, else weapon
 * Returns TRUE if attempted, FALSE if cancelled
 */
bool enchant_spell(int num_hit, int num_dam, int num_ac)
{
	int item;
	bool okay = FALSE;

	object_type *o_ptr;

	char o_name[80];

	cptr q, s;


	/* Assume enchant weapon */
	item_tester_hook = item_tester_hook_weapon;

	/* Enchant armor if requested */
	if (num_ac) item_tester_hook = item_tester_hook_armor;

	/* Get an item */
	q = "Enchant which item? ";
	s = "You have nothing to enchant.";
	if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR))) return (FALSE);

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


	/* Description */
	object_desc(o_name, sizeof(o_name), o_ptr, FALSE, ODESC_BASE);

	/* Describe */
	msg_format("%s %s glow%s brightly!",
	           ((item >= 0) ? "Your" : "The"), o_name,
	           ((o_ptr->number > 1) ? "" : "s"));

	/* Enchant */
	if (enchant(o_ptr, num_hit, ENCH_TOHIT)) okay = TRUE;
	if (enchant(o_ptr, num_dam, ENCH_TODAM)) okay = TRUE;
	if (enchant(o_ptr, num_ac, ENCH_TOAC)) okay = TRUE;

	/* Failure */
	if (!okay)
	{
		/* Flush */
		if (OPT(flush_failure)) flush();

		/* Message */
		msg_print("The enchantment failed.");
	}

	/* Something happened */
	return (TRUE);
}


/*
 * Identify an object in the inventory (or on the floor)
 * This routine does *not* automatically combine objects.
 * Returns TRUE if something was identified, else FALSE.
 */
bool ident_spell(void)
{
	int item;

	object_type *o_ptr;

	cptr q, s;

	/* Only un-id'ed items */
	item_tester_hook = item_tester_unknown;

	/* Get an item */
	q = "Identify which item? ";
	s = "You have nothing to identify.";
	if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR))) return (FALSE);

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


	/* Identify the object */
	do_ident_item(item, o_ptr);


	/* Something happened */
	return (TRUE);
}



/*
 * Hook for "get_item()".  Determine if something is rechargable.
 */
static bool item_tester_hook_recharge(const object_type *o_ptr)
{
	/* Recharge staves */
	if (o_ptr->tval == TV_STAFF) return (TRUE);

	/* Recharge wands */
	if (o_ptr->tval == TV_WAND) return (TRUE);

	/* Nope */
	return (FALSE);
}


/*
 * Recharge a wand or staff from the pack or on the floor.
 *
 * It is harder to recharge high level, and highly charged wands.
 *
 * XXX XXX XXX Beware of "sliding index errors".
 *
 * Should probably not "destroy" over-charged items, unless we
 * "replace" them by, say, a broken stick or some such.  The only
 * reason this is okay is because "scrolls of recharging" appear
 * BEFORE all staves/wands in the inventory.  Note that the
 * new "auto_sort_pack" option would correctly handle replacing
 * the "broken" wand with any other item (i.e. a broken stick).
 */
bool recharge(int num)
{
	int i, t, item, lev;

	object_type *o_ptr;

	cptr q, s;


	/* Only accept legal items */
	item_tester_hook = item_tester_hook_recharge;

	/* Get an item */
	q = "Recharge which item? ";
	s = "You have nothing to recharge.";
	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return (FALSE);

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


	/* Extract the object "level" */
	lev = k_info[o_ptr->k_idx].level;

	/* Recharge power */
	i = (num + 100 - lev - (10 * (o_ptr->pval / o_ptr->number))) / 15;

	/* Back-fire */
	if ((i <= 1) || one_in_(i))
	{
		msg_print("The recharge backfires!");
		msg_print("There is a bright flash of light.");

		/* Reduce the charges of rods/wands/staves */
		reduce_charges(o_ptr, 1);

		/* Reduce and describe inventory */
		if (item >= 0)
		{
			inven_item_increase(item, -1);
			inven_item_describe(item);
			inven_item_optimize(item);
		}
		/* Reduce and describe floor item */
		else
		{
			floor_item_increase(0 - item, -1);
			floor_item_describe(0 - item);
			floor_item_optimize(0 - item);
		}
	}

	/* Recharge */
	else
	{
		/* Extract a "power" */
		t = (num / (lev + 2)) + 1;

		/* Recharge based on the power */
		if (t > 0) o_ptr->pval += 2 + (s16b) randint1(t);

		/* We no longer think the item is empty */
		o_ptr->ident &= ~(IDENT_EMPTY);
	}

	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Redraw stuff */
	p_ptr->redraw |= (PR_INVEN);

	/* Something was done */
	return (TRUE);
}








/*
 * Apply a "project()" directly to all viewable monsters
 *
 * Note that affected monsters are NOT auto-tracked by this usage.
 */
bool project_los(int typ, int dam)
{
	int i;
	s16b x, y;

	int flg = PROJECT_JUMP | PROJECT_KILL | PROJECT_HIDE;

	bool obvious = FALSE;


	/* Affect all (nearby) monsters */
	for (i = 1; i < mon_max; i++)
	{
		monster_type *m_ptr = &mon_list[i];

		/* Paranoia -- Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Location */
		y = m_ptr->fy;
		x = m_ptr->fx;

		/* Require line of sight */
		if (!player_has_los_bold(y, x)) continue;

		/* Jump directly to the target monster */
		if (project(WHO_PLAYER, 0, y, x, dam, typ, flg)) obvious = TRUE;
	}

	/* Result */
	return (obvious);
}


/*
 * Speed monsters
 */
bool speed_monsters(void)
{
	return (project_los(GF_OLD_SPEED, p_ptr->lev));
}

/*
 * Slow monsters
 */
bool slow_monsters(void)
{
	return (project_los(GF_OLD_SLOW, p_ptr->lev));
}

/*
 * Sleep monsters
 */
bool sleep_monsters(void)
{
	return (project_los(GF_OLD_SLEEP, p_ptr->lev));
}

/*
 * Confuse monsters
 */
bool confuse_monsters(void)
{
	return (project_los(GF_OLD_CONF, p_ptr->lev));
}


/*
 * Banish evil monsters
 */
bool banish_evil(int dist)
{
	return (project_los(GF_AWAY_EVIL, dist));
}


/*
 * Turn undead
 */
bool turn_undead(void)
{
	return (project_los(GF_TURN_UNDEAD, p_ptr->lev));
}


/*
 * Dispel undead monsters
 */
bool dispel_undead(int dam)
{
	return (project_los(GF_DISP_UNDEAD, dam));
}

/*
 * Dispel evil monsters
 */
bool dispel_evil(int dam)
{
	return (project_los(GF_DISP_EVIL, dam));
}

/*
 * Dispel all monsters
 */
bool dispel_monsters(int dam)
{
	return (project_los(GF_DISP_ALL, dam));
}





/*
 * Wake up all monsters, and speed up "los" monsters.
 */
void aggravate_monsters(int who)
{
	int i;

	bool sleep = FALSE;
	bool speed = FALSE;

	/* Aggravate everyone nearby */
	for (i = 1; i < mon_max; i++)
	{
		monster_type *m_ptr = &mon_list[i];
		monster_race *r_ptr = &r_info[m_ptr->r_idx];

		/* Paranoia -- Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Skip aggravating monster (or player) */
		if (i == who) continue;

		/* Wake up nearby sleeping monsters */
		if (m_ptr->cdis < MAX_SIGHT * 2)
		{
			/* Wake up */
			if (m_ptr->csleep)
			{
				/* Wake up */
				wake_monster(m_ptr);
				sleep = TRUE;
			}
		}

		/* Speed up monsters in line of sight */
		if (player_has_los_bold(m_ptr->fy, m_ptr->fx))
		{
			/* Speed up (instantly) to racial base + 10 */
			if (m_ptr->mspeed < r_ptr->speed + 10)
			{
				/* Speed up */
				m_ptr->mspeed = r_ptr->speed + 10;
				speed = TRUE;
			}
		}
	}

	/* Messages */
	if (speed) msg_print("You feel a sudden stirring nearby!");
	else if (sleep) msg_print("You hear a sudden stirring in the distance!");
}



/*
 * Delete all non-unique monsters of a given "type" from the level
 */
bool banishment(void)
{
	int i;
	unsigned dam = 0;

	char typ;


	/* Mega-Hack -- Get a monster symbol */
	if (!get_com("Choose a monster race (by symbol) to banish: ", &typ))
		return FALSE;

	/* Delete the monsters of that "type" */
	for (i = 1; i < mon_max; i++)
	{
		monster_type *m_ptr = &mon_list[i];
		monster_race *r_ptr = &r_info[m_ptr->r_idx];

		/* Paranoia -- Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Hack -- Skip Unique Monsters */
		if (r_ptr->flags[0] & (RF0_UNIQUE)) continue;

		/* Skip "wrong" monsters */
		if (r_ptr->d_char != typ) continue;

		/* Delete the monster */
		delete_monster_idx(i);

		/* Take some damage */
		dam += randint1(4);
	}

	/* Hurt the player */
	take_hit(dam, "the strain of casting Banishment");

	/* Update monster list window */
	p_ptr->redraw |= PR_MONLIST;

	/* Success */
	return TRUE;
}


/*
 * Delete all nearby (non-unique) monsters
 */
bool mass_banishment(void) /* aka Mass genocide */
{
	int i;
	unsigned dam = 0;

	bool result = FALSE;


	/* Delete the (nearby) monsters */
	for (i = 1; i < mon_max; i++)
	{
		monster_type *m_ptr = &mon_list[i];
		monster_race *r_ptr = &r_info[m_ptr->r_idx];

		/* Paranoia -- Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Hack -- Skip unique monsters */
		if (r_ptr->flags[0] & (RF0_UNIQUE)) continue;

		/* Skip distant monsters */
		if (m_ptr->cdis > MAX_SIGHT) continue;

		/* Delete the monster */
		delete_monster_idx(i);

		/* Take some damage */
		dam += randint1(3);
	}

	/* Hurt the player */
	take_hit(dam, "the strain of casting Mass Banishment");

	/* Calculate result */
	result = (dam > 0) ? TRUE : FALSE;

	/* Update monster list window */
	if (result) p_ptr->redraw |= PR_MONLIST;

	return (result);
}



/*
 * Probe nearby monsters
 */
bool probing(void)
{
	int i;

	bool probe = FALSE;


	/* Probe all (nearby) monsters */
	for (i = 1; i < mon_max; i++)
	{
		monster_type *m_ptr = &mon_list[i];

		/* Paranoia -- Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Require line of sight */
		if (!player_has_los_bold(m_ptr->fy, m_ptr->fx)) continue;

		/* Probe visible monsters */
		if (m_ptr->ml)
		{
			char m_name[80];

			/* Start the message */
			if (!probe) msg_print("Probing...");

			/* Get "the monster" or "something" */
			monster_desc(m_name, sizeof(m_name), m_ptr, MDESC_IND1);

			/* Describe the monster */
			msg_format("%^s has %d hit points.", m_name, m_ptr->hp);

			/* Learn all of the non-spell, non-treasure flags */
			lore_do_probe(i);

			/* Probe worked */
			probe = TRUE;
		}
	}

	/* Done */
	if (probe)
	{
		msg_print("That's all.");
	}

	/* Result */
	return (probe);
}



/*
 * The spell of destruction
 *
 * This spell "deletes" monsters (instead of "killing" them).
 *
 * Later we may use one function for both "destruction" and
 * "earthquake" by using the "full" to select "destruction".
 */
void destroy_area(s16b y1, s16b x1, s16b r, bool full)
{
	s16b y, x, k, t;

	bool flag = FALSE;


	/* Unused parameter */
	(void)full;

	/* No effect in town */
	if (!p_ptr->depth)
	{
		msg_print("The ground shakes for a moment.");
		return;
	}

	/* Big area of affect */
	for (y = (y1 - r); y <= (y1 + r); y++)
	{
		for (x = (x1 - r); x <= (x1 + r); x++)
		{
			/* Skip illegal grids */
			if (!in_bounds_fully(y, x)) continue;

			/* Extract the distance */
			k = distance(y1, x1, y, x);

			/* Stay in the circle of death */
			if (k > r) continue;

			/* Lose room and vault */
			cave_info[y][x] &= ~(CAVE_ROOM | CAVE_ICKY);

			/* Lose light and knowledge */
			cave_info[y][x] &= ~(CAVE_GLOW | CAVE_MARK);
			
			lite_spot(y, x);

			/* Hack -- Notice player affect */
			if (cave_m_idx[y][x] < 0)
			{
				/* Hurt the player later */
				flag = TRUE;

				/* Do not hurt this grid */
				continue;
			}

			/* Hack -- Skip the epicenter */
			if ((y == y1) && (x == x1)) continue;

			/* Delete the monster (if any) */
			delete_monster(y, x);

			/* Destroy "valid" grids */
			if (cave_valid_bold(y, x))
			{
				int feat = FEAT_FLOOR;

				/* Delete objects */
				delete_object(y, x);

				/* Wall (or floor) type */
				t = (s16b) randint0(200);

				/* Granite */
				if (t < 20)
				{
					/* Create granite wall */
					feat = FEAT_WALL_EXTRA;
				}

				/* Quartz */
				else if (t < 70)
				{
					/* Create quartz vein */
					feat = FEAT_QUARTZ;
				}

				/* Magma */
				else if (t < 100)
				{
					/* Create magma vein */
					feat = FEAT_MAGMA;
				}

				/* Change the feature */
				cave_set_feat(y, x, feat);
			}
		}
	}


	/* Hack -- Affect player */
	if (flag)
	{
		/* Message */
		msg_print("There is a searing blast of light!");

		/* Blind the player */
		if (!p_ptr->state.resist_blind && !p_ptr->state.resist_lite)
		{
			/* Become blind */
			(void)inc_timed(TMD_BLIND, 10 + randint1(10), TRUE);
		}
	}


	/* Fully update the visuals */
	p_ptr->update |= (PU_FORGET_VIEW | PU_UPDATE_VIEW | PU_MONSTERS);

	/* Fully update the flow */
	p_ptr->update |= (PU_FORGET_FLOW | PU_UPDATE_FLOW);

	/* Redraw monster list */
	p_ptr->redraw |= (PR_MONLIST | PR_ITEMLIST);
}


/*
 * Induce an "earthquake" of the given radius at the given location.
 *
 * This will turn some walls into floors and some floors into walls.
 *
 * The player will take damage and "jump" into a safe grid if possible,
 * otherwise, he will "tunnel" through the rubble instantaneously.
 *
 * Monsters will take damage, and "jump" into a safe grid if possible,
 * otherwise they will be "buried" in the rubble, disappearing from
 * the level in the same way that they do when banished.
 *
 * Note that players and monsters (except eaters of walls and passers
 * through walls) will never occupy the same grid as a wall (or door).
 */
void earthquake(s16b cy, s16b cx, s16b r)
{
	s16b py = p_ptr->py;
	s16b px = p_ptr->px;

	int i, t;
	s16b y, x, sy = 0, sx = 0, yy, xx, dy, dx;

	int damage = 0;

	int sn = 0;

	bool hurt = FALSE;

	bool map[32][32];

	/* No effect in town */
	if (!p_ptr->depth)
	{
		msg_print("The ground shakes for a moment.");
		return;
	}

	/* Paranoia -- Enforce maximum range */
	if (r > 12) r = 12;

	/* Clear the "maximal blast" area */
	for (y = 0; y < 32; y++)
	{
		for (x = 0; x < 32; x++)
		{
			map[y][x] = FALSE;
		}
	}

	/* Check around the epicenter */
	for (dy = -r; dy <= r; dy++)
	{
		for (dx = -r; dx <= r; dx++)
		{
			/* Extract the location */
			yy = cy + dy;
			xx = cx + dx;

			/* Skip illegal grids */
			if (!in_bounds_fully(yy, xx)) continue;

			/* Skip distant grids */
			if (distance(cy, cx, yy, xx) > r) continue;

			/* Lose room and vault */
			cave_info[yy][xx] &= ~(CAVE_ROOM | CAVE_ICKY);

			/* Lose light and knowledge */
			cave_info[yy][xx] &= ~(CAVE_GLOW | CAVE_MARK);
			
			/* Skip the epicenter */
			if (!dx && !dy) continue;

			/* Skip most grids */
			if (randint0(100) < 85) continue;

			/* Damage this grid */
			map[16+yy-cy][16+xx-cx] = TRUE;

			/* Hack -- Take note of player damage */
			if ((yy == py) && (xx == px)) hurt = TRUE;
		}
	}

	/* First, affect the player (if necessary) */
	if (hurt)
	{
		/* Check around the player */
		for (i = 0; i < 8; i++)
		{
			/* Get the location */
			ISBYTE(py + ddy_ddd[i]);
			y = (byte) (py + ddy_ddd[i]);
			ISBYTE(px + ddx_ddd[i]);
			x = (byte) (px + ddx_ddd[i]);

			/* Skip non-empty grids */
			if (!cave_empty(y, x)) continue;

			/* Important -- Skip "quake" grids */
			if (map[16+y-cy][16+x-cx]) continue;

			/* Count "safe" grids, apply the randomizer */
			if ((++sn > 1) && (randint0(sn) != 0)) continue;

			/* Save the safe location */
			sy = y; sx = x;
		}

		/* Random message */
		switch (randint1(3))
		{
			case 1:
			{
				msg_print("The cave ceiling collapses!");
				break;
			}
			case 2:
			{
				msg_print("The cave floor twists in an unnatural way!");
				break;
			}
			default:
			{
				msg_print("The cave quakes!");
				msg_print("You are pummeled with debris!");
				break;
			}
		}

		/* Hurt the player a lot */
		if (!sn)
		{
			/* Message and damage */
			msg_print("You are severely crushed!");
			damage = 300;
		}

		/* Destroy the grid, and push the player to safety */
		else
		{
			/* Calculate results */
			switch (randint1(3))
			{
				case 1:
				{
					msg_print("You nimbly dodge the blast!");
					damage = 0;
					break;
				}
				case 2:
				{
					msg_print("You are bashed by rubble!");
					damage = damroll(10, 4);
					(void)inc_timed(TMD_STUN, randint1(50), TRUE);
					break;
				}
				case 3:
				{
					msg_print("You are crushed between the floor and ceiling!");
					damage = damroll(10, 4);
					(void)inc_timed(TMD_STUN, randint1(50), TRUE);
					break;
				}
			}

			/* Move player */
			monster_swap(py, px, sy, sx);
		}

		/* Take some damage */
		if (damage) take_hit(damage, "an earthquake");
	}


	/* Examine the quaked region */
	for (dy = -r; dy <= r; dy++)
	{
		for (dx = -r; dx <= r; dx++)
		{
			/* Extract the location */
			yy = cy + dy;
			xx = cx + dx;

			/* Skip unaffected grids */
			if (!map[16+yy-cy][16+xx-cx]) continue;

			/* Process monsters */
			if (cave_m_idx[yy][xx] > 0)
			{
				monster_type *m_ptr = &mon_list[cave_m_idx[yy][xx]];
				monster_race *r_ptr = &r_info[m_ptr->r_idx];

				/* Most monsters cannot co-exist with rock */
				if (!(r_ptr->flags[1] & (RF1_KILL_WALL | RF1_PASS_WALL)))
				{
					char m_name[80];

					/* Assume not safe */
					sn = 0;

					/* Monster can move to escape the wall */
					if (!(r_ptr->flags[0] & (RF0_NEVER_MOVE)))
					{
						/* Look for safety */
						for (i = 0; i < 8; i++)
						{
							/* Get the grid */
							ISBYTE(yy + ddy_ddd[i]);
							y = (byte) (yy + ddy_ddd[i]);
							ISBYTE(xx + ddx_ddd[i]);
							x = (byte) (xx + ddx_ddd[i]);

							/* Skip non-empty grids */
							if (!cave_empty(y, x)) continue;

							/* Hack -- no safety on glyph of warding */
							if (cave_feat[y][x] == FEAT_GLYPH) continue;

							/* Important -- Skip "quake" grids */
							if (map[16+y-cy][16+x-cx]) continue;

							/* Count "safe" grids, apply the randomizer */
							if ((++sn > 1) && (randint0(sn) != 0)) continue;

							/* Save the safe grid */
							sy = y;
							sx = x;
						}
					}

					/* Describe the monster */
					monster_desc(m_name, sizeof(m_name), m_ptr, 0);

					/* Scream in pain */
					msg_format("%^s wails out in pain!", m_name);

					/* Take damage from the quake */
					damage = (sn ? damroll(4, 8) : (m_ptr->hp + 1));

					/* Monster is certainly awake */
					wake_monster(m_ptr);

					/* Apply damage directly */
					m_ptr->hp -= INT2S16B(damage);

					/* Delete (not kill) "dead" monsters */
					if (m_ptr->hp < 0)
					{
						/* Message */
						msg_format("%^s is embedded in the rock!", m_name);

						/* Delete the monster */
						delete_monster(yy, xx);

						/* No longer safe */
						sn = 0;
					}

					/* Hack -- Escape from the rock */
					if (sn)
					{
						/* Move the monster */
						ISBYTE(yy);
						ISBYTE(xx);
						monster_swap((byte) yy, (byte) xx, sy, sx);
					}
				}
			}
		}
	}


	/* XXX XXX XXX */

	/* New location */
	py = p_ptr->py;
	px = p_ptr->px;

	/* Important -- no wall on player */
	map[16+py-cy][16+px-cx] = FALSE;


	/* Examine the quaked region */
	for (dy = -r; dy <= r; dy++)
	{
		for (dx = -r; dx <= r; dx++)
		{
			/* Extract the location */
			yy = cy + dy;
			xx = cx + dx;

			/* ignore invalid grids */
			if (!in_bounds_fully(yy, xx)) continue;

			/* Note unaffected grids for light changes, etc. */
			if (!map[16+yy-cy][16+xx-cx])
			{
				lite_spot(yy, xx);
			}

			/* Destroy location (if valid) */
			else if (cave_valid_bold(yy, xx))
			{
				int feat = FEAT_FLOOR;

				bool floor = cave_floor(yy, xx);

				/* Delete objects */
				delete_object(yy, xx);

				/* Wall (or floor) type */
				t = (floor ? randint0(100) : 200);

				/* Granite */
				if (t < 20)
				{
					/* Create granite wall */
					feat = FEAT_WALL_EXTRA;
				}

				/* Quartz */
				else if (t < 70)
				{
					/* Create quartz vein */
					feat = FEAT_QUARTZ;
				}

				/* Magma */
				else if (t < 100)
				{
					/* Create magma vein */
					feat = FEAT_MAGMA;
				}

				/* Change the feature */
				cave_set_feat(yy, xx, feat);
			}
		}
	}


	/* Fully update the visuals */
	p_ptr->update |= (PU_FORGET_VIEW | PU_UPDATE_VIEW | PU_MONSTERS);

	/* Fully update the flow */
	p_ptr->update |= (PU_FORGET_FLOW | PU_UPDATE_FLOW);

	/* Update the health bar */
	p_ptr->redraw |= (PR_HEALTH);

	/* Window stuff */
	p_ptr->redraw |= (PR_MONLIST | PR_ITEMLIST);
}



/*
 * This routine clears the entire "temp" set.
 *
 * This routine will Perma-Lite all "temp" grids.
 *
 * This routine is used (only) by "lite_room()"
 *
 * Dark grids are illuminated.
 *
 * Also, process all affected monsters.
 *
 * SMART monsters always wake up when illuminated
 * NORMAL monsters wake up 1/4 the time when illuminated
 * STUPID monsters wake up 1/10 the time when illuminated
 */
static void cave_temp_room_lite(void)
{
	int i;

	/* Apply flag changes */
	for (i = 0; i < temp_n; i++)
	{
		s16b y = temp_y[i];
		s16b x = temp_x[i];

		/* No longer in the array */
		cave_info[y][x] &= ~(CAVE_TEMP);

		/* Perma-Lite */
		cave_info[y][x] |= (CAVE_GLOW);
	}

	/* Fully update the visuals */
	p_ptr->update |= (PU_FORGET_VIEW | PU_UPDATE_VIEW | PU_MONSTERS);

	/* Update stuff */
	update_stuff();

	/* Process the grids */
	for (i = 0; i < temp_n; i++)
	{
		s16b y = temp_y[i];
		s16b x = temp_x[i];

		/* Redraw the grid */
		lite_spot(y, x);

		/* Process affected monsters */
		if (cave_m_idx[y][x] > 0)
		{
			int chance = 25;

			monster_type *m_ptr = &mon_list[cave_m_idx[y][x]];
			monster_race *r_ptr = &r_info[m_ptr->r_idx];

			/* Stupid monsters rarely wake up */
			if (r_ptr->flags[1] & (RF1_STUPID)) chance = 10;

			/* Smart monsters always wake up */
			if (r_ptr->flags[1] & (RF1_SMART)) chance = 100;

			/* Sometimes monsters wake up */
			if (m_ptr->csleep && (randint0(100) < chance))
			{
				/* Wake up! */
				wake_monster(m_ptr);

				/* Notice the "waking up" */
				if (m_ptr->ml)
				{
					char m_name[80];

					/* Get the monster name */
					monster_desc(m_name, sizeof(m_name), m_ptr, 0);

					/* Dump a message */
					msg_format("%^s wakes up.", m_name);
				}
			}
		}
	}

	/* None left */
	temp_n = 0;
}



/*
 * This routine clears the entire "temp" set.
 *
 * This routine will "darken" all "temp" grids.
 *
 * In addition, some of these grids will be "unmarked".
 *
 * This routine is used (only) by "unlite_room()"
 */
static void cave_temp_room_unlite(void)
{
	int i;

	/* Apply flag changes */
	for (i = 0; i < temp_n; i++)
	{
		s16b y = temp_y[i];
		s16b x = temp_x[i];

		/* No longer in the array */
		cave_info[y][x] &= ~(CAVE_TEMP);

		/* Darken the grid */
		cave_info[y][x] &= ~(CAVE_GLOW);

		/* Hack -- Forget "boring" grids */
		if (cave_feat[y][x] <= FEAT_INVIS)
		{
			/* Forget the grid */
			cave_info[y][x] &= ~(CAVE_MARK);
		}
	}

	/* Fully update the visuals */
	p_ptr->update |= (PU_FORGET_VIEW | PU_UPDATE_VIEW | PU_MONSTERS);

	/* Update stuff */
	update_stuff();

	/* Process the grids */
	for (i = 0; i < temp_n; i++)
	{
		s16b y = temp_y[i];
		s16b x = temp_x[i];

		/* Redraw the grid */
		lite_spot(y, x);
	}

	/* None left */
	temp_n = 0;
}




/*
 * Aux function -- see below
 */
static void cave_temp_room_aux(s16b y, s16b x)
{
	/* Avoid infinite recursion */
	if (cave_info[y][x] & (CAVE_TEMP)) return;

	/* Do not "leave" the current room */
	if (!(cave_info[y][x] & (CAVE_ROOM))) return;

	/* Paranoia -- verify space */
	if (temp_n == TEMP_MAX) return;

	/* Mark the grid as "seen" */
	cave_info[y][x] |= (CAVE_TEMP);

	/* Add it to the "seen" set */
	ISBYTE(y);
	temp_y[temp_n] = (byte) y;
	ISBYTE(x);
	temp_x[temp_n] = (byte) x;
	temp_n++;
}




/*
 * Illuminate any room containing the given location.
 */
void lite_room(s16b y1, s16b x1)
{
	s16b i, x, y;

	/* Add the initial grid */
	cave_temp_room_aux(y1, x1);

	/* While grids are in the queue, add their neighbors */
	for (i = 0; i < temp_n; i++)
	{
		x = temp_x[i], y = temp_y[i];

		/* Walls get lit, but stop light */
		if (!cave_floor(y, x)) continue;

		/* Spread adjacent */
		cave_temp_room_aux(y + 1, x);
		cave_temp_room_aux(y - 1, x);
		cave_temp_room_aux(y, x + 1);
		cave_temp_room_aux(y, x - 1);

		/* Spread diagonal */
		cave_temp_room_aux(y + 1, x + 1);
		cave_temp_room_aux(y - 1, x - 1);
		cave_temp_room_aux(y - 1, x + 1);
		cave_temp_room_aux(y + 1, x - 1);
	}

	/* Now, lite them all up at once */
	cave_temp_room_lite();
}


/*
 * Darken all rooms containing the given location
 */
void unlite_room(s16b y1, s16b x1)
{
	s16b i, x, y;

	/* Add the initial grid */
	cave_temp_room_aux(y1, x1);

	/* Spread, breadth first */
	for (i = 0; i < temp_n; i++)
	{
		x = temp_x[i], y = temp_y[i];

		/* Walls get dark, but stop darkness */
		if (!cave_floor(y, x)) continue;

		/* Spread adjacent */
		cave_temp_room_aux(y + 1, x);
		cave_temp_room_aux(y - 1, x);
		cave_temp_room_aux(y, x + 1);
		cave_temp_room_aux(y, x - 1);

		/* Spread diagonal */
		cave_temp_room_aux(y + 1, x + 1);
		cave_temp_room_aux(y - 1, x - 1);
		cave_temp_room_aux(y - 1, x + 1);
		cave_temp_room_aux(y + 1, x - 1);
	}

	/* Now, darken them all at once */
	cave_temp_room_unlite();
}



/*
 * Hack -- call light around the player
 * Affect all monsters in the projection radius
 */
bool lite_area(int dam, int rad)
{
	s16b py = p_ptr->py;
	s16b px = p_ptr->px;

	int flg = PROJECT_GRID | PROJECT_KILL;

	/* Hack -- Message */
	if (!p_ptr->timed[TMD_BLIND])
	{
		msg_print("You are surrounded by a white light.");
	}

	/* Hook into the "project()" function */
	(void)project(WHO_PLAYER, rad, py, px, dam, GF_LITE_WEAK, flg);

	/* Lite up the room */
	lite_room(py, px);

	/* Assume seen */
	return (TRUE);
}


/*
 * Hack -- call darkness around the player
 * Affect all monsters in the projection radius
 */
bool unlite_area(int dam, int rad)
{
	s16b py = p_ptr->py;
	s16b px = p_ptr->px;

	int flg = PROJECT_GRID | PROJECT_KILL;

	/* Hack -- Message */
	if (!p_ptr->timed[TMD_BLIND])
	{
		msg_print("Darkness surrounds you.");
	}

	/* Hook into the "project()" function */
	(void)project(-1, rad, py, px, dam, GF_DARK_WEAK, flg);

	/* Lite up the room */
	unlite_room(py, px);

	/* Assume seen */
	return (TRUE);
}



/*
 * Cast a ball spell
 * Stop if we hit a monster, act as a "ball"
 * Allow "target" mode to pass over monsters
 * Affect grids, objects, and monsters
 */
bool fire_ball(int typ, int dir, int dam, int rad)
{
	s16b py = p_ptr->py;
	s16b px = p_ptr->px;

	s16b ty, tx;

	int flg = PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL;

	/* Use the given direction */
	ISBYTE(py + 99 * ddy[dir]); /* TODO : This looks dodgy */
	ty = (byte) (py + 99 * ddy[dir]);
	ISBYTE(px + 99 * ddx[dir]);
	tx = (byte) (px + 99 * ddx[dir]);

	/* Hack -- Use an actual "target" */
	if ((dir == 5) && target_okay())
	{
		flg &= ~(PROJECT_STOP);

		target_get(&tx, &ty);
	}

	/* Analyze the "dir" and the "target".  Hurt items on floor. */
	return (project(WHO_PLAYER, rad, ty, tx, dam, typ, flg));
}


/*
 * Cast multiple non-jumping ball spells at the same target.
 *
 * Targets absolute coordinates instead of a specific monster, so that
 * the death of the monster doesn't change the target's location.
 */
bool fire_swarm(int num, int typ, int dir, int dam, int rad)
{
	bool noticed = FALSE;

	s16b py = p_ptr->py;
	s16b px = p_ptr->px;

	s16b ty, tx;

	int flg = PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL;

	/* Use the given direction */
	ISBYTE(py + 99 * ddy[dir]); /* TODO : This looks dodgy */
	ty = (byte) (py + 99 * ddy[dir]);
	ISBYTE(px + 99 * ddx[dir]);
	tx = (byte) (px + 99 * ddx[dir]);

	/* Hack -- Use an actual "target" (early detonation) */
	if ((dir == 5) && target_okay())
		target_get(&tx, &ty);

	while (num--)
	{
		/* Analyze the "dir" and the "target".  Hurt items on floor. */
		if (project(WHO_PLAYER, rad, ty, tx, dam, typ, flg)) noticed = TRUE;
	}

	return noticed;
}


/*
 * Hack -- apply a "projection()" in a direction (or at the target)
 */
static bool project_hook(int typ, int dir, int dam, int flg)
{
	s16b py = p_ptr->py;
	s16b px = p_ptr->px;

	s16b ty, tx;

	/* Pass through the target if needed */
	flg |= (PROJECT_THRU);

	/* Use the given direction */
	ISBYTE(py + ddy[dir]); /* TODO : This looks dodgy */
	ty = (byte) (py + ddy[dir]);
	ISBYTE(px + ddx[dir]);
	tx = (byte) (px + ddx[dir]);

	/* Hack -- Use an actual "target" */
	if ((dir == 5) && target_okay())
		target_get(&tx, &ty);

	/* Analyze the "dir" and the "target", do NOT explode */
	return (project(WHO_PLAYER, 0, ty, tx, dam, typ, flg));
}


/*
 * Cast a bolt spell
 * Stop if we hit a monster, as a "bolt"
 * Affect monsters (not grids or objects)
 */
bool fire_bolt(int typ, int dir, int dam)
{
	int flg = PROJECT_STOP | PROJECT_KILL;
	return (project_hook(typ, dir, dam, flg));
}

/*
 * Cast a beam spell
 * Pass through monsters, as a "beam"
 * Affect monsters (not grids or objects)
 */
bool fire_beam(int typ, int dir, int dam)
{
	int flg = PROJECT_BEAM | PROJECT_KILL;
	return (project_hook(typ, dir, dam, flg));
}

/*
 * Cast a bolt spell, or rarely, a beam spell
 */
bool fire_bolt_or_beam(int prob, int typ, int dir, int dam)
{
	if (randint0(100) < prob)
	{
		return (fire_beam(typ, dir, dam));
	}
	else
	{
		return (fire_bolt(typ, dir, dam));
	}
}


/*
 * Some of the old functions
 */

bool lite_line(int dir)
{
	int flg = PROJECT_BEAM | PROJECT_GRID | PROJECT_KILL;
	return (project_hook(GF_LITE_WEAK, dir, damroll(6, 8), flg));
}

bool strong_lite_line(int dir)
{
	int flg = PROJECT_BEAM | PROJECT_GRID | PROJECT_KILL;
	return (project_hook(GF_LITE, dir, damroll(10, 8), flg));
}

bool drain_life(int dir, int dam)
{
	int flg = PROJECT_STOP | PROJECT_KILL;
	return (project_hook(GF_OLD_DRAIN, dir, dam, flg));
}

bool wall_to_mud(int dir)
{
	int flg = PROJECT_BEAM | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL;
	return (project_hook(GF_KILL_WALL, dir, 20 + randint1(30), flg));
}

bool destroy_door(int dir)
{
	int flg = PROJECT_BEAM | PROJECT_GRID | PROJECT_ITEM;
	return (project_hook(GF_KILL_DOOR, dir, 0, flg));
}

bool disarm_trap(int dir)
{
	int flg = PROJECT_BEAM | PROJECT_GRID | PROJECT_ITEM;
	return (project_hook(GF_KILL_TRAP, dir, 0, flg));
}

bool heal_monster(int dir)
{
	int flg = PROJECT_STOP | PROJECT_KILL;
	return (project_hook(GF_OLD_HEAL, dir, damroll(4, 6), flg));
}

bool speed_monster(int dir)
{
	int flg = PROJECT_STOP | PROJECT_KILL;
	return (project_hook(GF_OLD_SPEED, dir, p_ptr->lev, flg));
}

bool slow_monster(int dir)
{
	int flg = PROJECT_STOP | PROJECT_KILL;
	return (project_hook(GF_OLD_SLOW, dir, p_ptr->lev, flg));
}

bool sleep_monster(int dir)
{
	int flg = PROJECT_STOP | PROJECT_KILL;
	return (project_hook(GF_OLD_SLEEP, dir, p_ptr->lev, flg));
}

bool confuse_monster(int dir, int plev)
{
	int flg = PROJECT_STOP | PROJECT_KILL;
	return (project_hook(GF_OLD_CONF, dir, plev, flg));
}

bool poly_monster(int dir)
{
	int flg = PROJECT_STOP | PROJECT_KILL;
	return (project_hook(GF_OLD_POLY, dir, p_ptr->lev, flg));
}

bool clone_monster(int dir)
{
	int flg = PROJECT_STOP | PROJECT_KILL;
	return (project_hook(GF_OLD_CLONE, dir, 0, flg));
}

bool fear_monster(int dir, int plev)
{
	int flg = PROJECT_STOP | PROJECT_KILL;
	return (project_hook(GF_TURN_ALL, dir, plev, flg));
}

bool teleport_monster(int dir)
{
	int flg = PROJECT_BEAM | PROJECT_KILL;
	return (project_hook(GF_AWAY_ALL, dir, MAX_SIGHT * 5, flg));
}

/*
 * Hooks -- affect adjacent grids (radius 1 ball attack)
 */

bool door_creation(void)
{
	s16b py = p_ptr->py;
	s16b px = p_ptr->px;

	int flg = PROJECT_GRID | PROJECT_ITEM | PROJECT_HIDE;
	return (project(WHO_PLAYER, 1, py, px, 0, GF_MAKE_DOOR, flg));
}

bool trap_creation(void)
{
	s16b py = p_ptr->py;
	s16b px = p_ptr->px;

	int flg = PROJECT_GRID | PROJECT_ITEM | PROJECT_HIDE;
	return (project(WHO_PLAYER, 1, py, px, 0, GF_MAKE_TRAP, flg));
}

bool destroy_doors_touch(void)
{
	s16b py = p_ptr->py;
	s16b px = p_ptr->px;

	int flg = PROJECT_GRID | PROJECT_ITEM | PROJECT_HIDE;
	return (project(WHO_PLAYER, 1, py, px, 0, GF_KILL_DOOR, flg));
}

bool sleep_monsters_touch(void)
{
	s16b py = p_ptr->py;
	s16b px = p_ptr->px;

	int flg = PROJECT_KILL | PROJECT_HIDE;
	return (project(WHO_PLAYER, 1, py, px, p_ptr->lev, GF_OLD_SLEEP, flg));
}


/*
 * Curse the players armor
 */
bool curse_armor(void)
{
	object_type *o_ptr;

	char o_name[80];


	/* Curse the body armor */
	o_ptr = &inventory[INVEN_BODY];

	/* Nothing to curse */
	if (!o_ptr->k_idx) return (FALSE);


	/* Describe */
	object_desc(o_name, sizeof(o_name), o_ptr, FALSE, ODESC_FULL);

	/* Attempt a saving throw for artifacts */
	if (artifact_p(o_ptr) && (randint0(100) < 50))
	{
		/* Cool */
		msg_format("A %s tries to %s, but your %s resists the effects!",
		           "terrible black aura", "surround your armor", o_name);
	}

	/* not artifact or failed save... */
	else
	{
		/* Oops */
		msg_format("A terrible black aura blasts your %s!", o_name);

		/* Take down bonus a wee bit */
		o_ptr->to_a -= (s16b) randint1(3);

		/* Curse it */
		o_ptr->flags[3] |= (TR3_LIGHT_CURSE | TR3_HEAVY_CURSE);

		/* Recalculate bonuses */
		p_ptr->update |= (PU_BONUS);

		/* Recalculate mana */
		p_ptr->update |= (PU_MANA);

		/* Window stuff */
		p_ptr->redraw |= (PR_INVEN | PR_EQUIP);
	}

	return (TRUE);
}


/*
 * Curse the players weapon
 */
bool curse_weapon(void)
{
	object_type *o_ptr;

	char o_name[80];


	/* Curse the weapon */
	o_ptr = &inventory[INVEN_WIELD];

	/* Nothing to curse */
	if (!o_ptr->k_idx) return (FALSE);


	/* Describe */
	object_desc(o_name, sizeof(o_name), o_ptr, FALSE, ODESC_FULL);

	/* Attempt a saving throw */
	if (artifact_p(o_ptr) && (randint0(100) < 50))
	{
		/* Cool */
		msg_format("A %s tries to %s, but your %s resists the effects!",
		           "terrible black aura", "surround your weapon", o_name);
	}

	/* not artifact or failed save... */
	else
	{
		/* Oops */
		msg_format("A terrible black aura blasts your %s!", o_name);

		/* Hurt it a bit */
		o_ptr->to_h = 0 - (s16b) randint1(3);
		o_ptr->to_d = 0 - (s16b) randint1(3);

		/* Curse it */
		o_ptr->flags[3] |= (TR3_LIGHT_CURSE | TR3_HEAVY_CURSE);

		/* Recalculate bonuses */
		p_ptr->update |= (PU_BONUS);

		/* Recalculate mana */
		p_ptr->update |= (PU_MANA);

		/* Window stuff */
		p_ptr->redraw |= (PR_INVEN | PR_EQUIP);
	}

	/* Notice */
	return (TRUE);
}


/*
 * Brand weapons (or ammo)
 *
 * Turns the (non-magical) object into an ego-item of 'brand_type'.
 */
void brand_object(object_type *o_ptr, byte brand_type)
{
	/* you can never modify artifacts / ego-items */
	/* you can never modify cursed / worthless items */
	if (o_ptr->k_idx && !cursed_p(o_ptr) && k_info[o_ptr->k_idx].cost &&
	    !artifact_p(o_ptr) && !ego_item_p(o_ptr))
	{
		cptr act = "magical";
		char o_name[80];

		object_desc(o_name, sizeof(o_name), o_ptr, FALSE, ODESC_BASE);

		switch (brand_type)
		{
			case EGO_BRAND_FIRE:
			case EGO_FLAME:
				act = "fiery";
				break;
			case EGO_BRAND_COLD:
			case EGO_FROST:
				act = "frosty";
				break;
		}

		/* Describe */
		msg_format("A %s aura surrounds the %s.", act, o_name);

		/* Brand the object */
		o_ptr->name2 = brand_type;

		/* Combine / Reorder the pack (later) */
		p_ptr->notice |= (PN_COMBINE | PN_REORDER);

		/* Window stuff */
		p_ptr->redraw |= (PR_INVEN | PR_EQUIP);

		/* Enchant */
		enchant(o_ptr, randint0(3) + 4, ENCH_TOHIT | ENCH_TODAM);
	}
	else
	{
		if (OPT(flush_failure)) flush();
		msg_print("The Branding failed.");
	}
}


/*
 * Brand the current weapon
 */
void brand_weapon(void)
{
	object_type *o_ptr;
	byte brand_type;

	o_ptr = &inventory[INVEN_WIELD];

	/* Select a brand */
	if (randint0(100) < 25)
		brand_type = EGO_BRAND_FIRE;
	else
		brand_type = EGO_BRAND_COLD;

	/* Brand the weapon */
	brand_object(o_ptr, brand_type);
}


/*
 * Hook to specify "ammo"
 */
static bool item_tester_hook_ammo(const object_type *o_ptr)
{
	switch (o_ptr->tval)
	{
		case TV_BOLT:
		case TV_ARROW:
		case TV_SHOT:
		{
			return (TRUE);
		}
	}

	return (FALSE);
}


/*
 * Brand some (non-magical) ammo
 */
bool brand_ammo(void)
{
	int item;
	object_type *o_ptr;
	cptr q, s;
	int r;
	byte brand_type;

	/* Only accept ammo */
	item_tester_hook = item_tester_hook_ammo;

	/* Get an item */
	q = "Brand which kind of ammunition? ";
	s = "You have nothing to brand.";
	if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR))) return (FALSE);

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

	r = (s16b) randint0(100);

	/* Select the brand */
	if (r < 50)
		brand_type = EGO_FLAME;
	else 
		brand_type = EGO_FROST;

	/* Brand the ammo */
	brand_object(o_ptr, brand_type);

	/* Done */
	return (TRUE);
}


/*
 * Enchant some (non-magical) bolts
 */
bool brand_bolts(void)
{
	int item;
	object_type *o_ptr;
	cptr q, s;


	/* Restrict choices to bolts */
	item_tester_tval = TV_BOLT;

	/* Get an item */
	q = "Brand which bolts? ";
	s = "You have no bolts to brand.";
	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return (FALSE);

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

	/* Brand the bolts */
	brand_object(o_ptr, EGO_FLAME);

	/* Done */
	return (TRUE);
}


/*
 * Hack -- activate the ring of power
 */
void ring_of_power(int dir)
{
	/* Pick a random effect */
	switch (randint1(10))
	{
		case 1:
		case 2:
		{
			/* Message */
			msg_print("You are surrounded by a malignant aura.");

			/* Decrease all stats (permanently) */
			(void)dec_stat(A_STR, TRUE);
			(void)dec_stat(A_INT, TRUE);
			(void)dec_stat(A_WIS, TRUE);
			(void)dec_stat(A_DEX, TRUE);
			(void)dec_stat(A_CON, TRUE);
			(void)dec_stat(A_CHR, TRUE);

			/* Lose some experience (permanently) */
			p_ptr->exp -= (p_ptr->exp / 4);
			p_ptr->max_exp -= (p_ptr->max_exp / 4);
			check_experience();

			break;
		}

		case 3:
		{
			/* Message */
			msg_print("You are surrounded by a powerful aura.");

			/* Dispel monsters */
			dispel_monsters(1000);

			break;
		}

		case 4:
		case 5:
		case 6:
		{
			/* Mana Ball */
			fire_ball(GF_MANA, dir, 300, 3);

			break;
		}

		case 7:
		case 8:
		case 9:
		case 10:
		{
			/* Mana Bolt */
			fire_bolt(GF_MANA, dir, 250);

			break;
		}
	}
}


/*
 * Identify an item.
 *
 * `item` is used to print the slot occupied by an object in equip/inven.
 * Any negative value assigned to "item" can be used for specifying an object
 * on the floor.
 */
void do_ident_item(int item, object_type *o_ptr)
{
	char o_name[80];

	/* Identify it */
	object_aware(o_ptr);
	object_known(o_ptr);

	/* Apply an autoinscription, if necessary */
	apply_autoinscription(o_ptr);

	/* Set squelch flag */
	p_ptr->notice |= PN_SQUELCH;

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Window stuff */
	p_ptr->redraw |= (PR_INVEN | PR_EQUIP);

	/* Description */
	object_desc(o_name, sizeof(o_name), o_ptr, TRUE, ODESC_FULL);

	/* Possibly play a sound depending on object quality. */ /* TODO Does not check p2val */
	if (o_ptr->pval < 0)
	{
		/* This is a bad item. */
		sound(MSG_IDENT_BAD);
	}
	else if (o_ptr->name1 != 0)
	{
		/* We have a good artifact. */
		sound(MSG_IDENT_ART);
	}
	else if (o_ptr->name2 != 0)
	{
		/* We have a good ego item. */
		sound(MSG_IDENT_EGO);
	}

	/* Log artifacts to the history list. */
	if (artifact_p(o_ptr))
		history_add_artifact(o_ptr->name1, TRUE);

	/* Describe */
	if (item >= INVEN_WIELD)
	{
		msg_format("%^s: %s (%c).",
			  describe_use(item), o_name, index_to_label(item));
	}
	else if (item >= 0)
	{
		msg_format("In your pack: %s (%c).",
			  o_name, index_to_label(item));
	}
	else
	{
		msg_format("On the ground: %s.", o_name);
	}
}
