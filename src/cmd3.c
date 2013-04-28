/* File: cmd3.c */

/* Purpose: Inventory commands */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"




/*
 * Move an item from equipment list to pack
 * Note that only one item at a time can be wielded per slot.
 * Note that taking off an item when "full" will cause that item
 * to fall to the ground.
 */
static void inven_takeoff(int item, int amt)
{
	int			posn;

	object_type		*o_ptr;
	object_type		tmp_obj;

	cptr		act;

	char		o_name[80];


	/* Get the item to take off */
	o_ptr = &inventory[item];

	/* Paranoia */
	if (amt <= 0) return;

	/* Verify */
	if (amt > o_ptr->number) amt = o_ptr->number;

	/* Make a copy to carry */
	tmp_obj = *o_ptr;
	tmp_obj.number = amt;

	/* What are we "doing" with the object */
	if (amt < o_ptr->number)
	{
		act = "Took off";
	}
	else if (item == INVEN_WIELD)
	{
		act = "Was wielding";
	}
	else if (item == INVEN_BOW)
	{
		act = "Was shooting with";
	}
	else if (item == INVEN_LITE)
	{
		act = "Light source was";
	}
	else
	{
		act = "Was wearing";
	}

	/* Carry the object, saving the slot it went in */
	posn = inven_carry(&tmp_obj);

	/* Describe the result */
	object_desc(o_name, o_ptr, TRUE, 3);

	/* Message */
	msg_format("%^s %s (%c).", act, o_name, index_to_label(posn));

	/* Delete (part of) it */
	inven_item_increase(item, -amt);
	inven_item_optimize(item);
}




/*
 * Drops (some of) an item from inventory to "near" the current location
 */
static void inven_drop(int item, int amt)
{
	object_type		*o_ptr;
	object_type		 tmp_obj;

	cptr		act;

	char		o_name[80];


	/* Access the slot to be dropped */
	o_ptr = &inventory[item];

	/* Error check */
	if (amt <= 0) return;

	/* Not too many */
	if (amt > o_ptr->number) amt = o_ptr->number;
	/* Nothing done? */
	if (amt <= 0) return;

	/* Make a "fake" object */
	tmp_obj = *o_ptr;
	tmp_obj.number = amt;

	/* What are we "doing" with the object */
	if (amt < o_ptr->number)
	{
		act = "Dropped";
	}
	else if (item == INVEN_WIELD)
	{
		act = "Was wielding";
	}
	else if (item == INVEN_BOW)
	{
		act = "Was shooting with";
	}
	else if (item == INVEN_LITE)
	{
		act = "Light source was";
	}
	else if (item >= INVEN_WIELD)
	{
		act = "Was wearing";
	}
	else
	{
		act = "Dropped";
	}

	/* Message */
	object_desc(o_name, &tmp_obj, TRUE, 3);

	/* Message */
	msg_format("%^s %s (%c).", act, o_name, index_to_label(item));

	/* Drop it (carefully) near the player */
	drop_near(&tmp_obj, 0, py, px);

	/* Decrease the item, optimize. */
	inven_item_increase(item, -amt);
	inven_item_describe(item);
	inven_item_optimize(item);
}





/*
 * Display inventory
 */
void do_cmd_inven(void)
{
	char out_val[160];


	/* Note that we are in "inventory" mode */
	command_wrk = FALSE;


	/* Save the screen */
	Term_save();

	/* Hack -- show empty slots */
	item_tester_full = TRUE;

	/* Display the inventory */
	show_inven();

	/* Hack -- hide empty slots */
	item_tester_full = FALSE;

	/* Build a prompt */
	sprintf(out_val, "Inventory (carrying %d.%d pounds). Command: ",
	        total_weight / 10, total_weight % 10);

	/* Get a command */
	prt(out_val, 0, 0);

	/* Get a new command */
	command_new = inkey();

	/* Restore the screen */
	Term_load();


	/* Process "Escape" */
	if (command_new == ESCAPE)
	{
		/* Reset stuff */
		command_new = 0;
		command_gap = 50;
	}

	/* Process normal keys */
	else
	{
		/* Hack -- Use "display" mode */
		command_see = TRUE;
	}
}


/*
 * Display equipment
 */
void do_cmd_equip(void)
{
	char out_val[160];


	/* Note that we are in "equipment" mode */
	command_wrk = TRUE;


	/* Save the screen */
	Term_save();

	/* Hack -- show empty slots */
	item_tester_full = TRUE;

	/* Display the equipment */
	show_equip();

	/* Hack -- undo the hack above */
	item_tester_full = FALSE;

	/* Build a prompt */
	sprintf(out_val, "Equipment (carrying %d.%d pounds). Command: ",
	        total_weight / 10, total_weight % 10);

	/* Get a command */
	prt(out_val, 0, 0);

	/* Get a new command */
	command_new = inkey();

	/* Restore the screen */
	Term_load();


	/* Process "Escape" */
	if (command_new == ESCAPE)
	{
		/* Reset stuff */
		command_new = 0;
		command_gap = 50;
	}

	/* Process normal keys */
	else
	{
		/* Enter "display" mode */
		command_see = TRUE;
	}
}


/*
 * The "wearable" tester
 */
bool item_tester_hook_wear(object_type *o_ptr)
{
	/* Check for a usable slot */
	if (wield_slot(o_ptr) >= INVEN_WIELD) return (TRUE);

	/* Assume not wearable */
	return (FALSE);
}


/*
 * Wield or wear a single item from the pack or floor
 */
void do_cmd_wield(void)
{
	int item, slot;
	object_type tmp_obj;
	object_type *o_ptr;

	cptr act;

	char o_name[80];


	/* Restrict the choices */
	item_tester_hook = item_tester_hook_wear;

	/* Get an item (from inven or floor) */
	if (!get_item(&item, "Wear/Wield which item? ", FALSE, TRUE, TRUE))
	{
		if (item == -2) msg_print("You have nothing you can wear or wield.");
		return;
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


	/* Check the slot */
	slot = wield_slot(o_ptr);

	/* Prevent wielding into a cursed slot */
	if (cursed_p(&inventory[slot]))
	{
		/* Describe it */
		object_desc(o_name, &inventory[slot], FALSE, 0);

		/* Message */
		msg_format("The %s you are %s appears to be cursed.",
		           o_name, describe_use(slot));

		/* Cancel the command */
		return;
	}

	/* Verify potential overflow */
	if ((inven_cnt >= INVEN_PACK) &&
	    ((item < 0) || (o_ptr->number > 1)))
	{
		/* Verify with the player */
		if (other_query_flag &&
		    !get_check("Your pack may overflow.  Continue? ")) return;
	}


	/* Take a turn */
	energy_use = 100;

	/* Get a copy of the object to wield */
	tmp_obj = *o_ptr;
	tmp_obj.number = 1;

	/* Decrease the item (from the pack) */
	if (item >= 0)
	{
		inven_item_increase(item, -1);
		inven_item_optimize(item);
	}

	/* Decrease the item (from the floor) */
	else
	{
		floor_item_increase(0 - item, -1);
		floor_item_optimize(0 - item);
	}

	/* Access the wield slot */
	o_ptr = &inventory[slot];

	/* Take off the "entire" item if one is there */
	if (inventory[slot].k_idx) inven_takeoff(slot, 255);

	/*** Could make procedure "inven_wield()" ***/

	/* Wear the new stuff */
	*o_ptr = tmp_obj;

	/* Increase the weight */
	total_weight += o_ptr->weight;

	/* Increment the equip counter by hand */
	equip_cnt++;

	/* Where is the item now */
	if (slot == INVEN_WIELD)
	{
		act = "You are wielding";
	}
	else if (slot == INVEN_BOW)
	{
		act = "You are shooting with";
	}
	else if (slot == INVEN_LITE)
	{
		act = "Your light source is";
	}
	else
	{
		act = "You are wearing";
	}

	/* Describe the result */
	object_desc(o_name, o_ptr, TRUE, 3);

	/* Message */
	msg_format("%^s %s (%c).", act, o_name, index_to_label(slot));

	/* Cursed! */
	if (cursed_p(o_ptr))
	{
		/* Warn the player */
		msg_print("Oops! It feels deathly cold!");

		/* Note the curse */
		o_ptr->ident |= ID_SENSE;
	}

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Recalculate torch */
	p_ptr->update |= (PU_TORCH);

	/* Recalculate mana */
	p_ptr->update |= (PU_MANA);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);
}

/*
 * Take off an item
 */
void do_cmd_takeoff(void)
{
	int item;

	object_type *o_ptr;


	/* Verify potential overflow */
	if (inven_cnt >= INVEN_PACK)
	{
		/* Verify with the player */
		if (other_query_flag &&
		    !get_check("Your pack may overflow.  Continue? ")) return;
	}


	/* Get an item (from equip) */
	if (!get_item(&item, "Take off which item? ", TRUE, FALSE, FALSE))
	{
		if (item == -2) msg_print("You are not wearing anything to take off.");
		return;
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


	/* Item is cursed */
	if (cursed_p(o_ptr))
	{
		/* Oops */
		msg_print("Hmmm, it seems to be cursed.");

		/* Nope */
		return;
	}


	/* Take a partial turn */
	energy_use = 50;
	/* Take off the item */
	inven_takeoff(item, 255);
}


/*
 * Drop an item
 */
void do_cmd_drop(void)
{
	int item, amt = 1;

	object_type *o_ptr;


	/* Get an item (from equip or inven) */
	if (!get_item(&item, "Drop which item? ", TRUE, TRUE, FALSE))
	{
		if (item == -2) msg_print("You have nothing to drop.");
		return;
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


	/* Cannot remove cursed items */
	if ((item >= INVEN_WIELD) && cursed_p(o_ptr))
	{
		/* Oops */
		msg_print("Hmmm, it seems to be cursed.");

		/* Nope */
		return;
	}


	/* See how many items */
	if (o_ptr->number > 1)
	{
		/* Get a quantity */
		amt = get_quantity(NULL, o_ptr->number);

		/* Allow user abort */
		if (amt <= 0) return;
	}


	/* Mega-Hack -- verify "dangerous" drops */
	if (cave[py][px].o_idx)
	{
		/* XXX XXX Verify with the player */
		if (other_query_flag &&
		    !get_check("The item may disappear.  Continue? ")) return;
	}


	/* Take a partial turn */
	energy_use = 50;

	/* Drop (some of) the item */
	inven_drop(item, amt);
}



/*
 * Destroy an item
 */
void do_cmd_destroy(void)
{
	int			item, amt = 1;
	int			old_number;

	bool		force = FALSE;

	object_type		*o_ptr;

	char		o_name[80];

	char		out_val[160];


	/* Hack -- force destruction */
	if (command_arg > 0) force = TRUE;


	/* Get an item (from inven or floor) */
	if (!get_item(&item, "Destroy which item? ", FALSE, TRUE, TRUE))
	{
		if (item == -2) msg_print("You have nothing to destroy.");
		return;
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


	/* See how many items */
	if (o_ptr->number > 1)
	{
		/* Get a quantity */
		amt = get_quantity(NULL, o_ptr->number);

		/* Allow user abort */
		if (amt <= 0) return;
	}


	/* Describe the object */
	old_number = o_ptr->number;
	o_ptr->number = amt;
	object_desc(o_name, o_ptr, TRUE, 3);
	o_ptr->number = old_number;

	/* Verify if needed */
	if (!force || other_query_flag)
	{
		/* Make a verification */
		sprintf(out_val, "Really destroy %s? ", o_name);
		if (!get_check(out_val)) return;
	}

	/* Take a turn */
	energy_use = 100;

	/* Artifacts cannot be destroyed */
	if (artifact_p(o_ptr))
	{
		cptr feel = "special";

		/* Message */
		msg_format("You cannot destroy %s.", o_name);

		/* Hack -- Handle icky artifacts */
		if (cursed_p(o_ptr) || broken_p(o_ptr)) feel = "terrible";

		/* Hack -- inscribe the artifact */
		o_ptr->note = quark_add(feel);

		/* We have "felt" it (again) */
		o_ptr->ident |= (ID_SENSE);

		/* Combine the pack */
		p_ptr->notice |= (PN_COMBINE);
		/* Window stuff */
		p_ptr->window |= (PW_INVEN | PW_EQUIP);

		/* Done */
		return;
	}

	/* Message */
	msg_format("You destroy %s.", o_name);

	/* Eliminate the item (from the pack) */
	if (item >= 0)
	{
		inven_item_increase(item, -amt);
		inven_item_describe(item);
		inven_item_optimize(item);
	}

	/* Eliminate the item (from the floor) */
	else
	{
		floor_item_increase(0 - item, -amt);
		floor_item_describe(0 - item);
		floor_item_optimize(0 - item);
	}
}


/*
 * Observe an item which has been *identify*-ed
 */
void do_cmd_observe(void)
{
	int			item;

	object_type		*o_ptr;

	char		o_name[80];


	/* Get an item (from equip or inven or floor) */
	if (!get_item(&item, "Examine which item? ", TRUE, TRUE, TRUE))
	{
		if (item == -2) msg_print("You have nothing to examine.");
		return;
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

	/* Require full knowledge */
	if (!(o_ptr->ident & ID_MENTAL))
	{
		msg_print("You have no special knowledge about that item.");
		return;
	}


	/* Description */
	object_desc(o_name, o_ptr, TRUE, 3);

	/* Describe */
	msg_format("Examining %s...", o_name);

	/* Describe it fully */
	if (!identify_fully_aux(o_ptr)) msg_print("You see nothing special.");
}



/*
 * Remove the inscription from an object
 * XXX Mention item (when done)?
 */
void do_cmd_uninscribe(void)
{
	int   item;

	object_type *o_ptr;


	/* Get an item (from equip or inven or floor) */
	if (!get_item(&item, "Un-inscribe which item? ", TRUE, TRUE, TRUE))
	{
		if (item == -2) msg_print("You have nothing to un-inscribe.");
		return;
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
	/* Nothing to remove */
	if (!o_ptr->note)
	{
		msg_print("That item had no inscription to remove.");
		return;
	}

	/* Message */
	msg_print("Inscription removed.");

	/* Remove the incription */
	o_ptr->note = 0;

	/* Combine the pack */
	p_ptr->notice |= (PN_COMBINE);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP);
}


/*
 * Inscribe an object with a comment
 */
void do_cmd_inscribe(void)
{
	int			item;

	object_type		*o_ptr;

	char		o_name[80];

	char		out_val[80];


	/* Get an item (from equip or inven or floor) */
	if (!get_item(&item, "Inscribe which item? ", TRUE, TRUE, TRUE))
	{
		if (item == -2) msg_print("You have nothing to inscribe.");
		return;
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

	/* Describe the activity */
	object_desc(o_name, o_ptr, TRUE, 3);

	/* Message */
	msg_format("Inscribing %s.", o_name);
	msg_print(NULL);

	/* Start with nothing */
	strcpy(out_val, "");

	/* Use old inscription */
	if (o_ptr->note)
	{
		/* Start with the old inscription */
		strcpy(out_val, quark_str(o_ptr->note));
	}

	/* Get a new inscription (possibly empty) */
	if (get_string("Inscription: ", out_val, 80))
	{
		/* Save the inscription */
		o_ptr->note = quark_add(out_val);

		/* Combine the pack */
		p_ptr->notice |= (PN_COMBINE);

		/* Window stuff */
		p_ptr->window |= (PW_INVEN | PW_EQUIP);
	}
}



/*
 * An "item_tester_hook" for refilling lanterns
 */
static bool item_tester_refill_lantern(object_type *o_ptr)
{
	/* Flasks of oil are okay */
	if (o_ptr->tval == TV_FLASK) return (TRUE);

	/* Torches are okay */
	if ((o_ptr->tval == TV_LITE) &&
	    (o_ptr->sval == SV_LITE_LANTERN)) return (TRUE);

	/* Assume not okay */
	return (FALSE);
}


/*
 * Refill the players lamp (from the pack or floor)
 */
static void do_cmd_refill_lamp(void)
{
	int item;

	object_type *o_ptr;
	object_type *j_ptr;


	/* Restrict the choices */
	item_tester_hook = item_tester_refill_lantern;

	/* Get an item (from inven or floor) */
	if (!get_item(&item, "Refill with which flask? ", FALSE, TRUE, TRUE))
	{
		if (item == -2) msg_print("You have no flasks of oil.");
		return;
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


	/* Take a partial turn */
	energy_use = 50;

	/* Access the lantern */
	j_ptr = &inventory[INVEN_LITE];

	/* Refuel */
	j_ptr->pval += o_ptr->pval;

	/* Message */
	msg_print("You fuel your lamp.");

	/* Comment */
	if (j_ptr->pval >= FUEL_LAMP)
	{
		j_ptr->pval = FUEL_LAMP;
		msg_print("Your lamp is full.");
	}

	/* Decrease the item (from the pack) */
	if (item >= 0)
	{
		inven_item_increase(item, -1);
		inven_item_describe(item);
		inven_item_optimize(item);
	}

	/* Decrease the item (from the floor) */
	else
	{
		floor_item_increase(0 - item, -1);
		floor_item_describe(0 - item);
		floor_item_optimize(0 - item);
	}

	/* Recalculate torch */
	p_ptr->update |= (PU_TORCH);
}



/*
 * An "item_tester_hook" for refilling torches
 */
static bool item_tester_refill_torch(object_type *o_ptr)
{
	/* Torches are okay */
	if ((o_ptr->tval == TV_LITE) &&
	    (o_ptr->sval == SV_LITE_TORCH)) return (TRUE);

	/* Assume not okay */
	return (FALSE);
}


/*
 * Refuel the players torch (from the pack or floor)
 */
static void do_cmd_refill_torch(void)
{
	int item;

	object_type *o_ptr;
	object_type *j_ptr;


	/* Restrict the choices */
	item_tester_hook = item_tester_refill_torch;

	/* Get an item (from inven or floor) */
	if (!get_item(&item, "Refuel with which torch? ", FALSE, TRUE, TRUE))
	{
		if (item == -2) msg_print("You have no extra torches.");
		return;
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


	/* Take a partial turn */
	energy_use = 50;

	/* Access the primary torch */
	j_ptr = &inventory[INVEN_LITE];

	/* Refuel */
	j_ptr->pval += o_ptr->pval + 5;

	/* Message */
	msg_print("You combine the torches.");

	/* Over-fuel message */
	if (j_ptr->pval >= FUEL_TORCH)
	{
		j_ptr->pval = FUEL_TORCH;
		msg_print("Your torch is fully fueled.");
	}

	/* Refuel message */
	else
	{
		msg_print("Your torch glows more brightly.");
	}

	/* Decrease the item (from the pack) */
	if (item >= 0)
	{
		inven_item_increase(item, -1);
		inven_item_describe(item);
		inven_item_optimize(item);
	}

	/* Decrease the item (from the floor) */
	else
	{
		floor_item_increase(0 - item, -1);
		floor_item_describe(0 - item);
		floor_item_optimize(0 - item);
	}

	/* Recalculate torch */
	p_ptr->update |= (PU_TORCH);
}



/*
 * Refill the players lamp, or restock his torches
 */
void do_cmd_refill(void)
{
	object_type *o_ptr;

	/* Get the light */
	o_ptr = &inventory[INVEN_LITE];

	/* It is nothing */
	if (o_ptr->tval != TV_LITE)
	{
		msg_print("You are not wielding a light.");
	}

	/* It's a lamp */
	else if (o_ptr->sval == SV_LITE_LANTERN)
	{
		do_cmd_refill_lamp();
	}

	/* It's a torch */
	else if (o_ptr->sval == SV_LITE_TORCH)
	{
		do_cmd_refill_torch();
	}

	/* No torch to refill */
	else
	{
		msg_print("Your light cannot be refilled.");
	}
}






/*
 * Target command
 */
void do_cmd_target(void)
{
	/* Set the target */
	if (target_set())
	{
		msg_print("Target Selected.");
	}
	else
	{
		msg_print("Target Aborted.");
	}
}





/*
 * Examine a grid, return a keypress (normally space or escape).
 *
 * Assume the player is not blind or hallucinating.
 *
 * Note that if a monster is in the grid, we update both the monster
 * recall info and the health bar info to track that monster.
 *
 * XXX XXX XXX We should allow the use of a "set target" command.
 *
 * XXX XXX XXX Note new terrain features for 2.8.0, and note that
 * eventually, we may allow objects and terrain features in the
 * same grid, or multiple objects per grid.
 *
 * We may assume that the grid is supposed to be "interesting".
 */
static int do_cmd_look_aux(int y, int x)
{
	cave_type		*c_ptr = &cave[y][x];

	object_type		*o_ptr = &o_list[c_ptr->o_idx];
	monster_type	*m_ptr = &m_list[c_ptr->m_idx];
	monster_race	*r_ptr = &r_info[m_ptr->r_idx];

	cptr		s1 = "You see ", s2 = "";

	bool		prep = FALSE;

	int			feat;

	int			query = ' ';

	char		m_name[80];
	char		o_name[80];

	char		out_val[160];


	/* Feature (apply "mimic") */
	feat = f_info[c_ptr->feat].mimic;

	/* Hack -- under the player */
	if ((y == py) && (x == px))
	{
		/* Description */
		s1 = "You are on ";
	}

	/* Normal */
	else
	{
		/* Hack -- Unknown terrain cannot be seen */
		if (!(c_ptr->info & CAVE_MARK)) feat = FEAT_NONE;	
	}

	/* Actual monsters */
	if (c_ptr->m_idx && m_ptr->ml)
	{
		/* Get the monster name ("a kobold") */
		monster_desc(m_name, m_ptr, 0x08);

		/* Hack -- track this monster race */
		recent_track(m_ptr->r_idx);

		/* Hack -- health bar for this monster */
		health_track(c_ptr->m_idx);

		/* Hack -- handle stuff */
		handle_stuff();

		/* Describe, and prompt for recall */
		sprintf(out_val, "%s%s%s (%s) [(r)ecall]",
		        s1, s2, m_name, look_mon_desc(c_ptr->m_idx));
		prt(out_val, 0, 0);

		/* Get a command */
		move_cursor_relative(y, x);
		query = inkey();

		/* Recall as needed */
		while (query == 'r')
		{
			/* Recall */
			Term_save();
			screen_roff(m_ptr->r_idx);
			Term_addstr(-1, TERM_WHITE, "  --pause--");
			query = inkey();
			Term_load();

			/* Continue if desired */
			if (query != ' ') return (query);

			/* Get a new command */
			move_cursor_relative(y, x);
			query = inkey();
		}

		/* Continue if allowed */
		if (query != ' ') return (query);

		/* Change the intro */
		s1 = "It is ";

		/* Hack -- take account of gender */
		if (r_ptr->flags1 & RF1_FEMALE) s1 = "She is ";
		else if (r_ptr->flags1 & RF1_MALE) s1 = "He is ";

		/* Use a preposition with objects */
		s2 = "on ";
		/* Use a preposition with terrain */
		prep = TRUE;

		/* Ignore boring terrain */
		if (feat <= FEAT_INVIS) feat = FEAT_NONE;
	}


	/* Actual items */
	if (c_ptr->o_idx && o_ptr->marked)
	{
		/* Obtain an object description */
		object_desc(o_name, o_ptr, TRUE, 3);

		/* Describe the object */
		sprintf(out_val, "%s%s%s.  --pause--", s1, s2, o_name);
		prt(out_val, 0, 0);
		move_cursor_relative(y, x);
		query = inkey();

		/* Use "space" to advance */
		if (query != ' ') return (query);

		/* Change the intro */
		s1 = "It is ";

		/* Plurals */
		if (o_ptr->number > 1) s1 = "They are ";

		/* Use a preposition with terrain */
		prep = TRUE;

		/* Ignore boring terrain */
		if (feat <= FEAT_INVIS) feat = FEAT_NONE;
	}


	/* Interesting terrain */
	if (feat != FEAT_NONE)
	{
		cptr p1 = "";

		cptr p2 = "a ";
		cptr name = f_name + f_info[feat].name;

		/* Pick a prefix */
		if (prep) p2 = ((feat >= FEAT_DOOR_HEAD) ? "in " : "on ");

		/* Note leading vowel */
		if (is_a_vowel(name[0])) p2 = "an ";

		/* Hack -- special description for store doors */
		if ((feat >= FEAT_SHOP_HEAD) && (feat <= FEAT_SHOP_TAIL))
		{
			p2 = "the entrance to the ";
		}

		/* Display a message */
		sprintf(out_val, "%s%s%s%s.  --pause--", s1, p1, p2, name);
		prt(out_val, 0, 0);
		move_cursor_relative(y, x);
		query = inkey();

		/* Continue if allowed */
		if (query != ' ') return (query);
	}


	/* Keep going */
	return (query);
}




/*
 * Hack -- determine if a given location is "interesting"
 */
static bool do_cmd_look_accept(int y, int x)
{
	cave_type *c_ptr;


	/* Player grid is always interesting */
	if ((y == py) && (x == px)) return (TRUE);


	/* Examine the grid */
	c_ptr = &cave[y][x];

	/* Visible monsters */
	if (c_ptr->m_idx)
	{
		monster_type *m_ptr = &m_list[c_ptr->m_idx];

		/* Visible monsters */
		if (m_ptr->ml) return (TRUE);
	}

	/* Objects */
	if (c_ptr->o_idx)
	{
		object_type *o_ptr = &o_list[c_ptr->o_idx];

		/* Memorized object */
		if (o_ptr->marked) return (TRUE);
	}

	/* Interesting memorized features */
	if (c_ptr->info & CAVE_MARK)
	{
		/* Notice glyphs */
		if (c_ptr->feat == FEAT_GLYPH) return (TRUE);
		/* Notice doors */
		if (c_ptr->feat == FEAT_OPEN) return (TRUE);
		if (c_ptr->feat == FEAT_BROKEN) return (TRUE);

		/* Notice stairs */
		if (c_ptr->feat == FEAT_LESS) return (TRUE);
		if (c_ptr->feat == FEAT_MORE) return (TRUE);

		/* Notice shops */
		if ((c_ptr->feat >= FEAT_SHOP_HEAD) &&
		    (c_ptr->feat <= FEAT_SHOP_TAIL)) return (TRUE);

		/* Notice traps */
		if ((c_ptr->feat >= FEAT_TRAP_HEAD) &&
		    (c_ptr->feat <= FEAT_TRAP_TAIL)) return (TRUE);

		/* Notice doors */
		if ((c_ptr->feat >= FEAT_DOOR_HEAD) &&
		    (c_ptr->feat <= FEAT_DOOR_TAIL)) return (TRUE);

		/* Notice rubble */
		if (c_ptr->feat == FEAT_RUBBLE) return (TRUE);

		/* Notice veins with treasure */
		if (c_ptr->feat == FEAT_MAGMA_K) return (TRUE);
		if (c_ptr->feat == FEAT_QUARTZ_K) return (TRUE);
	}

	/* Nope */
	return (FALSE);
}



/*
 * A new "look" command, similar to the "target" command.
 *
 * The "player grid" is always included in the "look" array, so
 * that this command will normally never "fail".
 *
 * XXX XXX XXX Allow "target" inside the "look" command (?)
 */
void do_cmd_look(void)
{
	int		y, x, i, d, m;

	bool	done = FALSE;

	char	query;


	/* Blind */
	if (p_ptr->blind)
	{
		msg_print("You can't see a damn thing!");
		return;
	}

	/* Hallucinating */
	if (p_ptr->image)
	{
		msg_print("You can't believe what you are seeing!");
		return;
	}


	/* Reset "temp" array */
	temp_n = 0;

	/* Scan the current panel */
	for (y = panel_row_min; y <= panel_row_max; y++)
	{
		for (x = panel_col_min; x <= panel_col_max; x++)
		{
			/* Require line of sight, unless "look" is "expanded" */
			if (!expand_look && !player_has_los_bold(y, x)) continue;

			/* Require interesting contents */
			if (!do_cmd_look_accept(y, x)) continue;

			/* Save the location */
			temp_x[temp_n] = x;
			temp_y[temp_n] = y;
			temp_n++;
		}
	}

	/* Paranoia */
	if (!temp_n)
	{
		msg_print("You see nothing special.");
		return;
	}


	/* Set the sort hooks */
	ang_sort_comp = ang_sort_comp_distance;
	ang_sort_swap = ang_sort_swap_distance;

	/* Sort the positions */
	ang_sort(temp_x, temp_y, temp_n);


	/* Start near the player */
	m = 0;

	/* Interact */
	while (!done)
	{
		/* Describe and Prompt */
		query = do_cmd_look_aux(temp_y[m], temp_x[m]);

		/* Assume no "direction" */
		d = 0;

		/* Analyze (non "recall") command */
		switch (query)
		{
			case ESCAPE:
			case 'q':
			{
				done = TRUE;
				break;
			}

			case '*':
			case '+':
			case ' ':
			{
				if (++m == temp_n)
				{
					m = 0;
					if (!expand_list) done = TRUE;
				}
				break;
			}

			case '-':
			{
				if (m-- == 0)
				{
					m = temp_n - 1;
					if (!expand_list) done = TRUE;
				}
				break;
			}
			default:
			{
				d = keymap_dirs[query & 0x7F];
				if (!d) bell();
				break;
			}
		}

		/* Motion */
		if (d)
		{
			/* Find a new grid if possible */
			i = target_pick(temp_y[m], temp_x[m], ddy[d], ddx[d]);

			/* Use that grid */
			if (i >= 0) m = i;
		}
	}

	/* Clear the prompt */
	prt("", 0, 0);
}




/*
 * Allow the player to examine other sectors on the map
 */
void do_cmd_locate(void)
{
	int		dir, y1, x1, y2, x2;

	char	tmp_val[80];

	char	out_val[160];


	/* Start at current panel */
	y2 = y1 = panel_row;
	x2 = x1 = panel_col;

	/* Show panels until done */
	while (1)
	{
		/* Describe the location */
		if ((y2 == y1) && (x2 == x1))
		{
			tmp_val[0] = '\0';
		}
		else
		{
			sprintf(tmp_val, "%s%s of",
			        ((y2 < y1) ? " North" : (y2 > y1) ? " South" : ""),
			        ((x2 < x1) ? " West" : (x2 > x1) ? " East" : ""));
		}

		/* Prepare to ask which way to look */
		sprintf(out_val,
		        "Map sector [%d,%d], which is%s your sector.  Direction?",
		        y2, x2, tmp_val);

		/* Assume no direction */
		dir = 0;

		/* Get a direction */
		while (!dir)
		{
			char command;
			/* Get a command (or Cancel) */
			if (!get_com(out_val, &command)) break;

			/* Extract direction */
			dir = keymap_dirs[command & 0x7F];

			/* Error */
			if (!dir) bell();
		}

		/* No direction */
		if (!dir) break;

		/* Apply the motion */
		y2 += ddy[dir];
		x2 += ddx[dir];

		/* Verify the row */
		if (y2 > max_panel_rows) y2 = max_panel_rows;
		else if (y2 < 0) y2 = 0;

		/* Verify the col */
		if (x2 > max_panel_cols) x2 = max_panel_cols;
		else if (x2 < 0) x2 = 0;

		/* Handle "changes" */
		if ((y2 != panel_row) || (x2 != panel_col))
		{
			/* Save the new panel info */
			panel_row = y2;
			panel_col = x2;

			/* Recalculate the boundaries */
			panel_bounds();

			/* Update stuff */
			p_ptr->update |= (PU_MONSTERS);

			/* Redraw map */
			p_ptr->redraw |= (PR_MAP);

			/* Handle stuff */
			handle_stuff();
		}
	}


	/* Recenter the map around the player */
	verify_panel();

	/* Update stuff */
	p_ptr->update |= (PU_MONSTERS);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD);

	/* Handle stuff */
	handle_stuff();
}






/*
 * The table of "symbol info" -- each entry is a string of the form
 * "X:desc" where "X" is the trigger, and "desc" is the "info".
 */
static cptr ident_info[] =
{
	" :A dark grid",
	"!:A potion (or oil)",
	"\":An amulet (or necklace)",
	"#:A wall (or secret door)",
	"$:Treasure (gold or gems)",
	"%:A vein (magma or quartz) or component",
	/* "&:unused", */
	"':An open door",
	"(:Soft armor",
	"):A shield",
	"*:A vein with treasure",
	"+:A closed door",
	",:Food (or mushroom patch)",
	"-:A wand (or rod)",
	".:Floor",
	"/:A polearm (Axe/Pike/etc)",
	/* "0:unused", */
	"1:Entrance to General Store",
	"2:Entrance to Armory",
	"3:Entrance to Weaponsmith",
	"4:Entrance to Temple",
	"5:Entrance to Alchemy shop",
	"6:Entrance to Magic store",
	"7:Entrance to Black Market",
	"8:Entrance to your home",
	/* "9:unused", */
	"::Rubble",
	";:A glyph of warding",
	"<:An up staircase",
	"=:A ring",
	">:A down staircase",
	"?:A scroll",
	"@:You",
	"A:Angel",
	"B:Bird",
	"C:Canine",
	"D:Ancient Dragon/Wyrm",
	"E:Elemental",
	"F:Dragon Fly",
	"G:Ghost",
	"H:Hybrid",
	"I:Insect",
	"J:Snake",
	"K:Killer Beetle",
	"L:Lich",
	"M:Multi-Headed Reptile",
	/* "N:unused", */
	"O:Ogre",
	"P:Giant Humanoid",
	"Q:Quylthulg (Pulsing Flesh Mound)",
	"R:Reptile/Amphibian",
	"S:Spider/Scorpion/Tick",
	"T:Troll",
	"U:Major Demon",
	"V:Vampire",
	"W:Wight/Wraith/etc",
	"X:Xorn/Xaren/etc",
	"Y:Yeti",
	"Z:Zephyr Hound",
	"[:Hard armor",
	"\\:A hafted weapon (mace/whip/etc)",
	"]:Misc. armor",
	"^:A trap",
	"_:A staff",
	/* "`:unused", */
	"a:Ant",
	"b:Bat",
	"c:Centipede",
	"d:Dragon",
	"e:Floating Eye",
	"f:Feline",
	"g:Golem",
	"h:Hobbit/Elf/Dwarf",
	"i:Icky Thing",
	"j:Jelly",
	"k:Kobold",
	"l:Louse",
	"m:Mold",
	"n:Naga",
	"o:Orc",
	"p:Person/Human",
	"q:Quadruped",
	"r:Rodent",
	"s:Skeleton",
	"t:Townsperson",
	"u:Minor Demon",
	"v:Vortex",
	"w:Worm/Worm-Mass",
	/* "x:unused", */
	"y:Yeek",
	"z:Zombie/Mummy",
	"{:A missile (arrow/bolt/shot)",
	"|:An edged weapon (sword/dagger/etc)",
	"}:A launcher (bow/crossbow/sling)",
	"~:A tool (or miscellaneous item)",
	NULL
};



/*
 * Sorting hook -- Comp function -- see below
 *
 * We use "u" to point to array of monster indexes,
 * and "v" to select the type of sorting to perform on "u".
 */
static bool ang_sort_comp_hook(vptr u, vptr v, int a, int b)
{
	u16b *who = (u16b*)(u);

	u16b *why = (u16b*)(v);

	int w1 = who[a];
	int w2 = who[b];

	int z1, z2;


	/* Sort by player kills */
	if (*why >= 4)
	{
		/* Extract player kills */
		z1 = r_info[w1].r_pkills;
		z2 = r_info[w2].r_pkills;

		/* Compare player kills */
		if (z1 < z2) return (TRUE);
		if (z1 > z2) return (FALSE);
	}


	/* Sort by total kills */
	if (*why >= 3)
	{
		/* Extract total kills */
		z1 = r_info[w1].r_tkills;
		z2 = r_info[w2].r_tkills;

		/* Compare total kills */
		if (z1 < z2) return (TRUE);
		if (z1 > z2) return (FALSE);
	}

	/* Sort by monster level */
	if (*why >= 2)
	{
		/* Extract levels */
		z1 = r_info[w1].level;
		z2 = r_info[w2].level;

		/* Compare levels */
		if (z1 < z2) return (TRUE);
		if (z1 > z2) return (FALSE);
	}


	/* Sort by monster experience */
	if (*why >= 1)
	{
		/* Extract experience */
		z1 = r_info[w1].mexp;
		z2 = r_info[w2].mexp;

		/* Compare experience */
		if (z1 < z2) return (TRUE);
		if (z1 > z2) return (FALSE);
	}


	/* Compare indexes */
	return (w1 <= w2);
}


/*
 * Sorting hook -- Swap function -- see below
 *
 * We use "u" to point to array of monster indexes,
 * and "v" to select the type of sorting to perform.
 */
static void ang_sort_swap_hook(vptr u, vptr v, int a, int b)
{
	u16b *who = (u16b*)(u);

	u16b holder;

	/* XXX XXX */
	v = v ? v : 0;

	/* Swap */
	holder = who[a];
	who[a] = who[b];
	who[b] = holder;
}


/*
 * Hack -- Display the "name" and "attr/chars" of a monster race
 */
static void roff_top(int r_idx)
{
	monster_race	*r_ptr = &r_info[r_idx];

	byte		a1, a2;
	char		c1, c2;


	/* Access the chars */
	c1 = r_ptr->d_char;
	c2 = r_ptr->x_char;

	/* Access the attrs */
	a1 = r_ptr->d_attr;
	a2 = r_ptr->x_attr;

	/* Hack -- fake monochrome */
	if (!use_color) a1 = TERM_WHITE;
	if (!use_color) a2 = TERM_WHITE;


	/* Clear the top line */
	Term_erase(0, 0, 255);

	/* Reset the cursor */
	Term_gotoxy(0, 0);

	/* A title (use "The" for non-uniques) */
	if (!(r_ptr->flags1 & RF1_UNIQUE))
	{
		Term_addstr(-1, TERM_WHITE, "The ");
	}

	/* Dump the name */
	Term_addstr(-1, TERM_WHITE, (r_name + r_ptr->name));

	/* Append the "standard" attr/char info */
	Term_addstr(-1, TERM_WHITE, " ('");
	Term_addch(a1, c1);
	Term_addstr(-1, TERM_WHITE, "')");

	/* Append the "optional" attr/char info */
	Term_addstr(-1, TERM_WHITE, "/('");
	Term_addch(a2, c2);
	Term_addstr(-1, TERM_WHITE, "'):");
}


/*
 * Identify a character, allow recall of monsters
 *
 * Several "special" responses recall "mulitple" monsters:
 *   ^A (all monsters)
 *   ^U (all unique monsters)
 *   ^N (all non-unique monsters)
 *
 * The responses may be sorted in several ways, see below.
 *
 * Note that the player ghosts are ignored. XXX XXX XXX
 */
void do_cmd_query_symbol(void)
{
	int		i, n, r_idx;
	char	sym, query;
	char	buf[128];

	bool	all = FALSE;
	bool	uniq = FALSE;
	bool	norm = FALSE;

	bool	recall = FALSE;

	u16b	why = 0;
	u16b	who[MAX_R_IDX];


	/* Get a character, or abort */
	if (!get_com("Enter character to be identified: ", &sym)) return;

	/* Find that character info, and describe it */
	for (i = 0; ident_info[i]; ++i)
	{
		if (sym == ident_info[i][0]) break;
	}

	/* Describe */
	if (sym == KTRL('A'))
	{
		all = TRUE;
		strcpy(buf, "Full monster list.");
	}
	else if (sym == KTRL('U'))
	{
		all = uniq = TRUE;
		strcpy(buf, "Unique monster list.");
	}
	else if (sym == KTRL('N'))
	{
		all = norm = TRUE;
		strcpy(buf, "Non-unique monster list.");
	}
	else if (ident_info[i])
	{
		sprintf(buf, "%c - %s.", sym, ident_info[i] + 2);
	}
	else
	{
		sprintf(buf, "%c - %s.", sym, "Unknown Symbol");
	}

	/* Display the result */
	prt(buf, 0, 0);


	/* Collect matching monsters */
	for (n = 0, i = 1; i < MAX_R_IDX-1; i++)
	{
		monster_race *r_ptr = &r_info[i];

		/* Nothing to recall */
		if (!cheat_know && !r_ptr->r_sights) continue;

		/* Require non-unique monsters if needed */
		if (norm && (r_ptr->flags1 & RF1_UNIQUE)) continue;

		/* Require unique monsters if needed */
		if (uniq && !(r_ptr->flags1 & RF1_UNIQUE)) continue;

		/* Collect "appropriate" monsters */
		if (all || (r_ptr->d_char == sym)) who[n++] = i;
	}

	/* Nothing to recall */
	if (!n) return;


	/* Prompt XXX XXX XXX */
	put_str("Recall details? (k/p/y/n): ", 0, 40);

	/* Query */
	query = inkey();

	/* Restore */
	prt(buf, 0, 0);

	/* Sort by kills (and level) */
	if (query == 'k')
	{
		why = 4;
		query = 'y';
	}

	/* Sort by level */
	if (query == 'p')
	{
		why = 2;
		query = 'y';
	}
	/* Catch "escape" */
	if (query != 'y') return;


	/* Sort if needed */
	if (why)
	{
		/* Select the sort method */
		ang_sort_comp = ang_sort_comp_hook;
		ang_sort_swap = ang_sort_swap_hook;

		/* Sort the array */
		ang_sort(who, &why, n);
	}


	/* Start at the end */
	i = n - 1;

	/* Scan the monster memory */
	while (1)
	{
		/* Extract a race */
		r_idx = who[i];

		/* Hack -- Auto-recall */
		recent_track(r_idx);

		/* Hack -- Handle stuff */
		handle_stuff();

		/* Hack -- Begin the prompt */
		roff_top(r_idx);

		/* Hack -- Complete the prompt */
		Term_addstr(-1, TERM_WHITE, " [(r)ecall, ESC]");

		/* Interact */
		while (1)
		{
			/* Recall */
			if (recall)
			{
				/* Save the screen */
				Term_save();

				/* Recall on screen */
				screen_roff(who[i]);

				/* Hack -- Complete the prompt (again) */
				Term_addstr(-1, TERM_WHITE, " [(r)ecall, ESC]");
			}

			/* Command */
			query = inkey();

			/* Unrecall */
			if (recall)
			{
				/* Restore */
				Term_load();
			}

			/* Normal commands */
			if (query != 'r') break;

			/* Toggle recall */
			recall = !recall;
		}

		/* Stop scanning */
		if (query == ESCAPE) break;

		/* Move to "prev" monster */
		if (query == '-')
		{
			if (++i == n)
			{
				i = 0;
				if (!expand_list) break;
			}
		}

		/* Move to "next" monster */
		else
		{
			if (i-- == 0)
			{
				i = n - 1;
				if (!expand_list) break;
			}
		}
	}

	/* Re-display the identity */
	prt(buf, 0, 0);
}


