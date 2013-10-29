/*
 * File: cmd6.c
 * Purpose: Eating/quaffing/reading/aiming/staving/zapping/activating
 *
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 * Copyright (c) 2007 Andrew Sidwell
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
#include "cmds.h"
#include "effects.h"



/*
 * Check to see if the player can use a rod/wand/staff/activatable object.
 */
static int check_devices(object_type *o_ptr)
{
	int lev, chance;
	const char *msg;
	const char *what = NULL;

	/* Get the right string */
	switch (o_ptr->tval)
	{
		case TV_ROD:   msg = "zap the rod";   break;
		case TV_WAND:  msg = "use the wand";  what = "wand";  break;
		case TV_STAFF: msg = "use the staff"; what = "staff"; break;
		default:       msg = "activate it";  break;
	}

	/* Extract the item level */
	if (artifact_p(o_ptr))
		lev = a_info[o_ptr->name1].level;
	else
		lev = k_info[o_ptr->k_idx].level;

	/* Base chance of success */
	chance = p_ptr->state.skills[SKILL_DEVICE];

	/* Confusion hurts skill */
	if (p_ptr->timed[TMD_CONFUSED] || p_ptr->timed[TMD_AMNESIA])
		chance = chance / 2;

	/* High level objects are harder */
	chance -= MIN(lev, 50);

	/* Give everyone a (slight) chance */
	if ((chance < USE_DEVICE) && one_in_(USE_DEVICE - chance + 1))
	{
		chance = USE_DEVICE;
	}

	/* Roll for usage */
	if ((chance < USE_DEVICE) || (randint1(chance) < USE_DEVICE))
	{
		if (OPT(flush_failure)) flush();
		msg_format("You failed to %s properly.", msg);
		return FALSE;
	}

	/* Notice empty staffs */
	if (what && o_ptr->pval <= 0)
	{
		if (OPT(flush_failure)) flush();
		msg_format("The %s has no charges left.", what);
		o_ptr->ident |= (IDENT_EMPTY);
		return FALSE;
	}

	return TRUE;
}

/*
 * Return the chance of an effect beaming, given a tval.
 */
static int beam_chance(int tval)
{
	switch (tval)
	{
		case TV_WAND: return 20;
		case TV_ROD:  return 10;
	}

	return 0;
}


typedef enum { 
	ART_TAG_NONE, 
	ART_TAG_NAME, 
	ART_TAG_KIND, 
	ART_TAG_VERB, 
	ART_TAG_VERB_IS 
} art_tag_t;

static art_tag_t art_tag_lookup(const char *tag)
{
	if (strncmp(tag, "name", 4) == 0)
		return ART_TAG_NAME;
	else if (strncmp(tag, "kind", 4) == 0)
		return ART_TAG_KIND;
	else if (strncmp(tag, "s", 1) == 0)
		return ART_TAG_VERB;
	else if (strncmp(tag, "is", 2) == 0)
		return ART_TAG_VERB_IS;
	else
		return ART_TAG_NONE;
}

/*
 * Print an artifact activation message.
 * 
 * In order to support randarts, with scrambled names, we re-write
 * the message to replace instances of {name} with the artifact name
 * and instances of {kind} with the type of object.
 *
 * This code deals with plural and singular forms of verbs correctly 
 * when encountering {s}, though in fact both names and kinds are 
 * always singular in the current code (gloves are "Set of" and boots
 * are "Pair of")
 */
static void activation_message(object_type *o_ptr, const char *message)
{
	char buf[1024] = "\0";
	const char *next;
	const char *s;
	const char *tag;
	const char *in_cursor;
	size_t end = 0;
 
	in_cursor = message;
 
	next = strchr(in_cursor, '{');
	while (next)
	{
		/* Copy the text leading up to this { */
		strnfcat(buf, 1024, &end, "%.*s", next - in_cursor, in_cursor); 

		s = next + 1;
		while (*s && isalpha((unsigned char) *s)) s++;

		if (*s == '}')		/* Valid tag */
		{
			tag = next + 1; /* Start the tag after the { */
			in_cursor = s + 1;
 
			switch(art_tag_lookup(tag))
			{
			case ART_TAG_NAME:
				end += object_desc(buf, 1024, o_ptr, TRUE, ODESC_BASE); 
				break;
			case ART_TAG_KIND:
				object_kind_name(&buf[end], 1024-end, o_ptr->k_idx, TRUE);
				end += strlen(&buf[end]);
				break;
			case ART_TAG_VERB:
				strnfcat(buf, 1024, &end, "s");
				break;
			case ART_TAG_VERB_IS:
				if((end > 2) && (buf[end-2] == 's'))
					strnfcat(buf, 1024, &end, "are");
				else
					strnfcat(buf, 1024, &end, "is");
			default:
				break;
			}
		}
		else    /* An invalid tag, skip it */
		{
			in_cursor = next + 1;
		} 

		next = strchr(in_cursor, '{');
	}
	strnfcat(buf, 1024, &end, in_cursor);
 
	msg_print(buf);
} 


/*
 * Use an object the right way.
 *
 * There may be a BIG problem with any "effect" that can cause "changes"
 * to the inventory.  For example, a "scroll of recharging" can cause
 * a wand/staff to "disappear", moving the inventory up.  Luckily, the
 * scrolls all appear BEFORE the staffs/wands, so this is not a problem.
 * But, for example, a "staff of recharging" could cause MAJOR problems.
 * In such a case, it will be best to either (1) "postpone" the effect
 * until the end of the function, or (2) "change" the effect, say, into
 * giving a staff "negative" charges, or "turning a staff into a stick".
 * It seems as though a "rod of recharging" might in fact cause problems.
 * The basic problem is that the act of recharging (and destroying) an
 * item causes the inducer of that action to "move", causing "o_ptr" to
 * no longer point at the correct item, with horrifying results.
 */
void do_cmd_use(object_type *o_ptr, int item, u16b snd, use_type use)
{
	int effect;
	bool ident = FALSE, used;
	bool was_aware = object_aware_p(o_ptr);
	int dir = 5;
	s16b px = p_ptr->px, py = p_ptr->py;

	/* Figure out effect to use */
	if (o_ptr->name1)
		effect = a_info[o_ptr->name1].effect;
	else
		effect = k_info[o_ptr->k_idx].effect;

	/* If the item requires a direction, get one (allow cancelling) */
	if (effect_aim(effect) ||
	    (o_ptr->tval == TV_WAND) ||
	    (o_ptr->tval == TV_ROD && !object_aware_p(o_ptr)))
	{
		/* Get a direction, allow cancel */
		if (!get_aim_dir(&dir))
			return;
	}

	/* Use energy regardless of failure */
	p_ptr->energy_use = 100;

	/* Check for use */
	if (use == USE_CHARGE || use == USE_TIMEOUT)
	{
		if (!check_devices(o_ptr))
			return;
	}

	/* Special message for artifacts */
	if (artifact_p(o_ptr))
	{
		message(snd, 0, "You activate it.");
		activation_message(o_ptr, a_text + a_info[o_ptr->name1].effect_msg);
	}
	else
	{
		/* Make a noise! */
		sound(snd);
	}

	/* A bit of a hack to make ID work better.
	   -- Check for "obvious" effects beforehand. */
	if (effect_obvious(effect)) object_aware(o_ptr);

	/* Do effect */
	used = effect_do(effect, &ident, was_aware, dir, beam_chance(o_ptr->tval));

	/* Food feeds the player */
	if (o_ptr->tval == TV_FOOD || o_ptr->tval == TV_POTION)
		(void)set_food(p_ptr->food + o_ptr->pval);

	if (!used && !ident) return;

	/* Mark as tried and redisplay */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);
	p_ptr->redraw |= (PR_INVEN | PR_EQUIP);


	/*
	 * If the player becomes aware of the item's function, then mark it as
	 * aware and reward the player with some experience.  Otherwise, mark
	 * it as "tried".
	 */
	if (ident && !was_aware)
	{
		/* Object level */
		int lev = k_info[o_ptr->k_idx].level;

		object_aware(o_ptr);
		if (o_ptr->tval == TV_ROD) object_known(o_ptr);
		gain_exp((lev + (p_ptr->lev / 2)) / p_ptr->lev);
		p_ptr->notice |= PN_SQUELCH;
	}
	else
	{
		object_tried(o_ptr);
	}

	/* Chargeables act differently to single-used items when not used up */
	if (used && use == USE_CHARGE)
	{
		/* Use a single charge */
		o_ptr->pval--;

		/* Describe charges */
		if (item >= 0)
			inven_item_charges(item);
		else
			floor_item_charges(0 - item);
	}
	else if (used && use == USE_TIMEOUT)
	{
		/* Artifacts use their own special field */
		if (o_ptr->name1)
		{
			const artifact_type *a_ptr = &a_info[o_ptr->name1];
			o_ptr->timeout = INT2S16B(a_ptr->time_base + damroll(a_ptr->time_dice, a_ptr->time_sides));
		}
		else
		{
			const object_kind *k_ptr = &k_info[o_ptr->k_idx];
			o_ptr->timeout += INT2S16B(k_ptr->time_base + damroll(k_ptr->time_dice, k_ptr->time_sides));
		}
	}
	else if (used && use == USE_SINGLE)
	{
		/* Destroy a potion in the pack */
		if (item >= 0)
		{
			inven_item_increase(item, -1);
			inven_item_describe(item);
			inven_item_optimize(item);
		}

		/* Destroy a potion on the floor */
		else
		{
			floor_item_increase(0 - item, -1);
			floor_item_describe(0 - item);
			floor_item_optimize(0 - item);
		}
	}
	
	/* Hack to make Glyph of Warding work properly */
	if (cave_feat[py][px] == FEAT_GLYPH)
	{
		/* Shift any objects to further away */
		for (o_ptr = get_first_object(py, px); o_ptr; o_ptr = get_next_object(o_ptr))
		{
			drop_near(o_ptr, 0, py, px);
		}
		
		/* Delete the "moved" objects from their original position */
		delete_object(py, px);
	}
}
