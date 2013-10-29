/*
 * File: attack.c
 * Purpose: Attacking (both throwing and melee) code
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

#include "object/object.h"
#include "object/tvalsval.h"

/**
 * Determines how likely an object is to break on throwing or shooting.
 *
 * \returns percentage change of breaking
 */
int breakage_chance(const object_type *o_ptr)
{
	/* Artifacts never break */
	if (artifact_p(o_ptr))
		return 0;

	switch (o_ptr->tval)
	{
		case TV_FLASK:
		case TV_POTION:
		case TV_BOTTLE:
		case TV_FOOD:
		case TV_JUNK:
			return 100;

		case TV_LITE:
		case TV_SCROLL:
		case TV_SPELL:
		case TV_SKELETON:
			return 50;

		case TV_ARROW:
			return 35;

		case TV_WAND:
		case TV_SHOT:
		case TV_BOLT:
		case TV_SPIKE:
			return 25;

		default:
			return 10;
	}
}


/*
 * Determine if the player "hits" a monster.
 *
 * Note -- Always miss 5%, always hit 5%, otherwise random.
 */
bool test_hit(int chance, int ac, int vis)
{
	int k;

	/* Percentile dice */
	k = randint0(100);

	/* Hack -- Instant miss or hit */
	if (k < 10) return (k < 5);

	/* Penalize invisible targets */
	if (!vis) chance = chance / 2;

	/* Power competes against armor */
	if ((chance > 0) && (randint0(chance) >= (ac * 3 / 4))) return (TRUE);

	/* Assume miss */
	return (FALSE);
}



/*
 * Critical hits (from objects thrown by player)
 * Factor in item weight, total plusses, and player level.
 */
static int critical_shot(int weight, int plus, int dam)
{
	int i, k;

	/* Extract "shot" power */
	i = (weight + ((p_ptr->state.to_h + plus) * 4) + (p_ptr->lev * 2));

	/* Critical hit */
	if (randint1(5000) <= i)
	{
		k = weight + randint1(500);

		if (k < 500)
		{
			msg_print("It was a good hit!");
			dam = 2 * dam + 5;
		}
		else if (k < 1000)
		{
			msg_print("It was a great hit!");
			dam = 2 * dam + 10;
		}
		else
		{
			msg_print("It was a superb hit!");
			dam = 3 * dam + 15;
		}
	}

	return (dam);
}



/*
 * Critical hits (by player)
 *
 * Factor in weapon weight, total plusses, player level.
 */
static int critical_norm(int weight, int plus, int dam)
{
	int i, k;

	/* Extract "blow" power */
	i = (weight + ((p_ptr->state.to_h + plus) * 5) + (p_ptr->lev * 3));

	/* Chance */
	if (randint1(5000) <= i)
	{
		k = weight + randint1(650);

		if (k < 400)
		{
			sound(MSG_HIT_GOOD);
			msg_print("It was a good hit!");
			dam = 2 * dam + 5;
		}
		else if (k < 700)
		{
			sound(MSG_HIT_GREAT);
			msg_print("It was a great hit!");
			dam = 2 * dam + 10;
		}
		else if (k < 900)
		{
			sound(MSG_HIT_SUPERB);
			msg_print("It was a superb hit!");
			dam = 3 * dam + 15;
		}
		else if (k < 1300)
		{
			sound(MSG_HIT_HI_GREAT);
			msg_print("It was a *GREAT* hit!");
			dam = 3 * dam + 20;
		}
		else
		{
			sound(MSG_HIT_HI_SUPERB);
			msg_print("It was a *SUPERB* hit!");
			dam = ((7 * dam) / 2) + 25;
		}
	}
	else
	{
		sound(MSG_HIT);
	}

	return (dam);
}

static bool elemental_skin_chance(s16b value)
{
   if (value>400) return(TRUE);
   else if (value>200) return(randint1(2)==1);
   else if (value>100) return(randint1(3)==1);
   else if (value>50) return(randint1(4)==1);
   else if (value>20) return(randint1(10)==1);
   else return (FALSE);
}

/*
 * does a ring of flames work?
 * note that the chance is high, but it only works on non-ego, non-artifact weapons!
 */
static bool elemental_ring_chance()
{
   return (randint1(6-(p_ptr->lev/10)) == 1);
}

bool test_brand_extra(int brandno)
{
	int dif = SV_RING_FLAMES - brandno;/* TODO Check in right order and adjacent! */
	bool result = FALSE;

	if (inventory[INVEN_LEFT].k_idx)
	{
		object_type *o_ptr = &inventory[INVEN_LEFT];

		if (!cursed_p(o_ptr) &&
				(o_ptr->tval == TV_RING) && (o_ptr->sval == SV_RING_FLAMES + dif) &&
				elemental_ring_chance())
		{
			result = TRUE;
		}
	}
	if (inventory[INVEN_RIGHT].k_idx)
	{
		object_type *o_ptr = &inventory[INVEN_RIGHT];

		if (!cursed_p(o_ptr) &&
				(o_ptr->tval == TV_RING) && (o_ptr->sval == SV_RING_FLAMES + dif) &&
				elemental_ring_chance())
		{
			result = TRUE;
		}
	}
	if (result)
	{
		switch(dif)
		{
			case 0: /* fire */
				msg_print("Your weapon suddenly glows hot in your hands!");
				break;
			case 1: /* cold */
				msg_print("Your weapon suddenly leaves a trail of cold mist!");
				break;
			case 2: /* acid */
				msg_print("Your weapon suddenly drips a an acid rain!");
				break;
			case 3: /* elec */
				msg_print("Your weapon suddenly leaves a trail of sparks!");
				break;
		}
	}
	if ((p_ptr->timed[TMD_FIRE + dif]) && (elemental_skin_chance(p_ptr->timed[TMD_FIRE + dif])))
	{
		switch(dif)
		{
			case 0: /* fire */
			{
				cptr who = "fiery skin";
				result = TRUE;
			    msg_print("Fire erupts from your fingers.");
				fire_dam(randint1(10)+5, who);
				break;
			}
			case 1: /* cold */
			{
				cptr who = "cold skin";
				result = TRUE;
				msg_print("A mist of cold whafts from your hands.");
				cold_dam(randint1(10)+5, who);
				break;
			}
			case 2: /* acid */
			{
				cptr who = "acid skin";
				result = TRUE;
				msg_print("Acid splatters from your skin.");
				acid_dam(randint1(10)+5, who);
				break;
			}
			case 3: /* elec */
			{
				cptr who = "electric skin";
				result = TRUE;
				msg_print("Suddenly sparks sizzle between your fingers.");
				elec_dam(randint1(10)+5, who);
				break;
			}
		}
	}
	return (result);
}

/**
 * Extract the multiplier from a given object hitting a given monster.
 *
 * If there is a slay or brand in effect, change the verb for hitting
 * to something interesting ('burn', 'smite', etc.).  Also, note which
 * flags had an effect in o_ptr->known_flags[].
 *
 * \param o_ptr is the object being used to attack
 * \param m_ptr is the monster being attacked
 * \param hit_verb is where a new verb is returned
 * \param is_ranged should be true for ranged attacks
 *
 * \returns attack multiplier
 */
static int get_brand_mult(object_type *o_ptr, const monster_type *m_ptr,
		const char **hit_verb, bool is_ranged, bool secondary)
{
	int mult = 1;
	const slay_t *s_ptr;

	monster_race *r_ptr = &r_info[m_ptr->r_idx];
	monster_lore *l_ptr = &l_list[m_ptr->r_idx];

	u32b f[OBJ_FLAG_N];
	object_flags(o_ptr, f);

	for (s_ptr = slay_table; s_ptr->slay_flag; s_ptr++)
	{
		if (!(f[1] & s_ptr->slay_flag)) continue;

		/* If the monster doesn't match or the slay flag does */
		if ((s_ptr->brand && !(r_ptr->flags[2] & s_ptr->resist_flag)) ||
				(r_ptr->flags[2] & s_ptr->monster_flag))
		{
			/* Learn the flag */
			if (m_ptr->ml)
				l_ptr->flags[2] |= s_ptr->monster_flag;

			if (mult < s_ptr->mult)
				mult = s_ptr->mult;

			/* Set the hit verb appropriately */
			if (is_ranged)
				*hit_verb = s_ptr->range_verb;
			else
				*hit_verb = s_ptr->melee_verb;

			/* Do something a bit cleverer here */
			o_ptr->known_flags[1] |= s_ptr->slay_flag;

			/* Print a cool message for branded rings et al */
			if (s_ptr->active_verb && secondary)
			{
				char o_name[40];
				object_desc(o_name, sizeof(o_name), o_ptr,
						FALSE, ODESC_BASE);
				msg_format("Your %s %s!", o_name,
						s_ptr->active_verb);
			}
		}

		/* If the monster resisted, add to the monster lore */
		if (r_ptr->flags[2] & s_ptr->resist_flag)
		{
			if (m_ptr->ml)
				l_ptr->flags[2] |= s_ptr->resist_flag;
		}
	}
	return mult;
}

/*
 * Attack the monster at the given location
 *
 * If no "weapon" is available, then "punch" the monster one time.
 */
void py_attack(s16b y, s16b x)
{
	int num = 0, bonus, chance;

	monster_type *m_ptr;
	monster_race *r_ptr;
	monster_lore *l_ptr;

	object_type *o_ptr;

	char m_name[80];

	bool fear = FALSE;

	bool do_quake = FALSE;

	/* Get the monster */
	m_ptr = &mon_list[cave_m_idx[y][x]];
	r_ptr = &r_info[m_ptr->r_idx];
	l_ptr = &l_list[m_ptr->r_idx];

	/* Disturb the player */
	disturb(0, 0);


	/* Extract monster name (or "it") */
	monster_desc(m_name, sizeof(m_name), m_ptr, 0);

	/* Auto-Recall if possible and visible */
	if (m_ptr->ml) monster_race_track(m_ptr->r_idx);

	/* Track a new monster */
	if (m_ptr->ml) health_track(cave_m_idx[y][x]);

	/* Player has no physical attacks */
	if (cp_ptr->flags & CF_NO_ATTACK)
	{
		/* Message */
		message_format(MSG_GENERIC, 0, "You bump into %s!", m_name);

		/* Done */
		return;
	}

	/* Disturb the monster */
	wake_monster(m_ptr);


	/* Get the weapon */
	o_ptr = &inventory[INVEN_WIELD];

	/* Calculate the "attack quality" */
	bonus = p_ptr->state.to_h + o_ptr->to_h;
	chance = (p_ptr->state.skills[SKILL_TO_HIT_MELEE] + (bonus * BTH_PLUS_ADJ));


	/* Attack once for each legal blow */
	while (num++ < p_ptr->state.num_blow)
	{
		/* Test for hit */
		if (test_hit(chance, r_ptr->ac, m_ptr->ml))
		{
			/* Default to punching for one damage */
			const char *hit_verb = "punch";
			int k = 1;

			/* Handle normal weapon */
			if (o_ptr->k_idx)
			{
				int weapon_brand_mult, ring_brand_mult[2];
				int use_mult = 1;

				hit_verb = "hit";

				/* Hack-- put rings first, because they can
				 * only be brands right now */
				ring_brand_mult[0] = get_brand_mult(
						&inventory[INVEN_LEFT],
						m_ptr, &hit_verb, FALSE, TRUE);
				ring_brand_mult[1] = get_brand_mult(
						&inventory[INVEN_RIGHT],
						m_ptr, &hit_verb, FALSE, TRUE);
				weapon_brand_mult = get_brand_mult(
						o_ptr,
						m_ptr, &hit_verb, FALSE, FALSE);

				/* Message. Need to do this after tot_dam_aux, which sets hit_verb, but before critical_norm, which may print further messages. */
				message_format(MSG_GENERIC, m_ptr->r_idx, "You %s %s.", hit_verb, m_name);
						
				if (ring_brand_mult[0] > use_mult)
					use_mult = ring_brand_mult[0];
				if (ring_brand_mult[1] > use_mult)
					use_mult = ring_brand_mult[1];
				if (weapon_brand_mult > use_mult)
					use_mult = weapon_brand_mult;

				k = damroll(o_ptr->dd, o_ptr->ds);
				k *= use_mult;

				if (p_ptr->state.impact && (k > 50))
					do_quake = TRUE;

				k += o_ptr->to_d;
				k = critical_norm(o_ptr->weight, o_ptr->to_h, k);

				/* Learn by use */
				object_notice_on_attack();
				if (do_quake)
					object_notice_flag(3, TR3_IMPACT);
			}
			else
			{
				message_format(MSG_GENERIC, m_ptr->r_idx, "You %s %s.", hit_verb, m_name);
			}

			/* Apply the player damage bonuses */
			k += p_ptr->state.to_d;

			/* No negative damage */
			if (k < 0) k = 0;

			/* Complex message */
			if (p_ptr->wizard)
				msg_format("You do %d (out of %d) damage.", k, m_ptr->hp);

			/* Damage, check for fear and death */
			if (mon_take_hit(cave_m_idx[y][x], k, &fear, NULL)) break;

			/* Confusion attack */
			if (p_ptr->confusing)
			{
				/* Cancel glowing hands */
				p_ptr->confusing = FALSE;

				/* Message */
				msg_print("Your hands stop glowing.");

				/* Confuse the monster */
				if (r_ptr->flags[2] & (RF2_NO_CONF))
				{
					if (m_ptr->ml)
					{
						l_ptr->flags[2] |= (RF2_NO_CONF);
					}

					msg_format("%^s is unaffected.", m_name);
				}
				else if (randint0(100) < r_ptr->level)
				{
					msg_format("%^s is unaffected.", m_name);
				}
				else
				{
					msg_format("%^s appears confused.", m_name);
					m_ptr->confused += (m_ptr->confused>236)?10 + (byte)randint0(p_ptr->lev) / 5 : 0;
				}
			}
		}

		/* Player misses */
		else
		{
			/* Message */
			message_format(MSG_MISS, m_ptr->r_idx, "You miss %s.", m_name);
		}
	}


	/* Hack -- delay fear messages */
	if (fear && m_ptr->ml)
	{
		/* Message */
		message_format(MSG_FLEE, m_ptr->r_idx, "%^s flees in terror!", m_name);
	}


	/* Mega-Hack -- apply earthquake brand */
	if (do_quake) earthquake(p_ptr->py, p_ptr->px, 10);
}





/*
 * Fire an object from the pack or floor.
 *
 * You may only fire items that "match" your missile launcher.
 *
 * See "calc_bonuses()" for more calculations and such.
 *
 * Note that "firing" a missile is MUCH better than "throwing" it.
 *
 * Note: "unseen" monsters are very hard to hit.
 *
 * Objects are more likely to break if they "attempt" to hit a monster.
 *
 * Rangers (with Bows) and Anyone (with "Extra Shots") get extra shots.
 * The "extra shot" code works by decreasing the amount of energy
 * required to make each shot, spreading the shots out over time.
 *
 * Note that when firing missiles, the launcher multiplier is applied
 * after all the bonuses are added in, making multipliers very useful.
 *
 * Note that Bows of "Extra Might" get extra range and an extra bonus
 * for the damage multiplier.
 */
void do_cmd_fire(void)
{
	int dir, item;
	int i, j;
	s16b ty, tx, y, x;
	int tdam, tdis, thits;
	int bonus, chance;

	object_type *o_ptr;
	object_type *j_ptr;

	object_type *i_ptr;
	object_type object_type_body;

	bool hit_body = FALSE;

	byte missile_attr;
	char missile_char;

	char o_name[80];

	int path_n;
	u16b path_g[256];

	cptr q, s;

	int msec = op_ptr->delay_factor * op_ptr->delay_factor;


	/* Get the "bow" (if any) */
	j_ptr = &inventory[INVEN_BOW];

	/* Require a usable launcher */
	if (!j_ptr->tval || !p_ptr->state.ammo_tval)
	{
		msg_print("You have nothing to fire with.");
		return;
	}


	/* Require proper missile */
	item_tester_tval = p_ptr->state.ammo_tval;

	/* Get an item */
	q = "Fire which item? ";
	s = "You have nothing to fire.";
	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return;

	/* Get the object */
	if (item >= 0)
	{
		o_ptr = &inventory[item];
	}
	else
	{
		o_ptr = &o_list[0 - item];
	}

	/* Get a direction (or cancel) */
	if (!get_aim_dir(&dir)) return;

	/* Base range XXX XXX */ 
	tdis = 6 + 2 * p_ptr->state.ammo_mult; 

	/* Start at the player */ 
	x = p_ptr->px; 
	y = p_ptr->py; 

	/* Predict the "target" location */ 
	ISBYTE(y + 99 * ddy[dir]);
	ty = (byte) (y + 99 * ddy[dir]); 
	ISBYTE(x + 99 * ddx[dir])
	tx = (byte) (x + 99 * ddx[dir]); 

	/* Check for target validity */ 
	if ((dir == 5) && target_okay()) 
	{ 
		target_get(&tx, &ty); 
		if (distance(y, x, ty, tx) > tdis) 
		{ 
			msg_print("Target out of range"); 
			return; 
		} 
	} 

	/* Sound */
	sound(MSG_SHOOT);

	/* Describe the object */
	object_desc(o_name, sizeof(o_name), o_ptr, FALSE,
			ODESC_FULL | ODESC_SINGULAR);

	/* Find the color and symbol for the object for throwing */
	missile_attr = object_attr(o_ptr);
	missile_char = object_char(o_ptr);


	/* Use the proper number of shots */
	thits = p_ptr->state.num_fire;

	/* Actually "fire" the object */
	bonus = (p_ptr->state.to_h + o_ptr->to_h + j_ptr->to_h);
	chance = p_ptr->state.skills[SKILL_TO_HIT_BOW] +
			(bonus * BTH_PLUS_ADJ);

	/* Take a (partial) turn */
	p_ptr->energy_use = INT2S16B((100 / thits));

	/* Calculate the path */
	path_n = project_path(path_g, tdis, y, x, ty, tx, 0);

	/* Hack -- Handle stuff */
	handle_stuff();

	/* Project along the path */
	for (i = 0; i < path_n; ++i)
	{
		s16b ny = GRID_Y(path_g[i]);
		s16b nx = GRID_X(path_g[i]);

		/* Hack -- Stop before hitting walls */
		if (!cave_floor(ny, nx)) break;

		/* Advance */
		ISBYTE(nx);
		x = (byte) nx;
		ISBYTE(ny);
		y = (byte) ny;

		/* Only do visuals if the player can "see" the missile */
		if (player_can_see_bold(y, x))
		{
			/* Visual effects */
			print_rel(missile_char, missile_attr, y, x);
			move_cursor_relative(y, x);

			Term_fresh();
			if (p_ptr->redraw) redraw_stuff();

			Term_xtra(TERM_XTRA_DELAY, msec);
			lite_spot(y, x);

			Term_fresh();
			if (p_ptr->redraw) redraw_stuff();
		}

		/* Delay anyway for consistency */
		else
		{
			/* Pause anyway, for consistancy */
			Term_xtra(TERM_XTRA_DELAY, msec);
		}

		/* Handle monster */
		if (cave_m_idx[y][x] > 0)
		{
			monster_type *m_ptr = &mon_list[cave_m_idx[y][x]];
			monster_race *r_ptr = &r_info[m_ptr->r_idx];

			int chance2 = chance - distance(p_ptr->py, p_ptr->px, y, x);
			int visible = m_ptr->ml;

			const char *hit_verb = "hits";

			int ammo_mult = get_brand_mult(o_ptr, m_ptr,
					&hit_verb, TRUE, FALSE);
			int shoot_mult = get_brand_mult(j_ptr, m_ptr,
					&hit_verb, TRUE, FALSE);
			/* Note the collision */
			hit_body = TRUE;

			/* Did we hit it (penalize distance travelled) */
			if (test_hit(chance2, r_ptr->ac, m_ptr->ml))
			{
				bool fear = FALSE;

				/* Assume a default death */
				cptr note_dies = " dies.";

				/* Some monsters get "destroyed" */
				if ((r_ptr->flags[2] & RF2_DEMON) ||
					(r_ptr->flags[2] & RF2_UNDEAD) ||
					(r_ptr->flags[1] & RF1_STUPID) ||
					strchr("Evg", r_ptr->d_char))
				{
					/* Special note at death */
					note_dies = " is destroyed.";
				}


				/* Handle unseen monster */
				if (!visible)
				{
					/* Invisible monster */
					message_format(MSG_SHOOT_HIT, 0, "The %s finds a mark.", o_name);
				}

				/* Handle visible monster */
				else
				{
					char m_name[80];

					/* Get "the monster" or "it" */
					monster_desc(m_name, sizeof(m_name), m_ptr, 0);

					/* Message */
					message_format(MSG_SHOOT_HIT, 0, "The %s %s %s.", o_name, hit_verb, m_name);

					/* Hack -- Track this monster race */
					if (m_ptr->ml) monster_race_track(m_ptr->r_idx);

					/* Hack -- Track this monster */
					if (m_ptr->ml) health_track(cave_m_idx[y][x]);
				}

				/* Apply damage: multiplier, slays, criticals, bonuses */
				tdam = damroll(o_ptr->dd, o_ptr->ds);
				tdam += o_ptr->to_d + j_ptr->to_d;
				tdam *= p_ptr->state.ammo_mult;
				tdam *= MAX(ammo_mult, shoot_mult);
				tdam = critical_shot(o_ptr->weight, o_ptr->to_h, tdam);

				/* No negative damage */
				if (tdam < 0) tdam = 0;

				/* Complex message */
				if (p_ptr->wizard)
				{
					msg_format("You do %d (out of %d) damage.",
					           tdam, m_ptr->hp);
				}

				/* Hit the monster, check for death */
				if (mon_take_hit(cave_m_idx[y][x], tdam, &fear, note_dies))
				{
					/* Dead monster */
				}

				/* No death */
				else
				{
					/* Message */
					message_pain(cave_m_idx[y][x], tdam);

					/* Take note */
					if (fear && m_ptr->ml)
					{
						char m_name[80];

						/* Get the monster name (or "it") */
						monster_desc(m_name, sizeof(m_name), m_ptr, 0);

						/* Message */
						message_format(MSG_FLEE, m_ptr->r_idx,
						               "%^s flees in terror!", m_name);
					}
				}
			}

			/* Stop looking */
			break;
		}
	}

	/* Get local object */
	i_ptr = &object_type_body;

	/* Obtain a local object */
	object_copy(i_ptr, o_ptr);

	/* Single object */
	i_ptr->number = 1;


	/* Reduce and describe inventory */
	if (item >= 0)
	{
		inven_item_increase(item, -1);
		inven_item_describe(item);
		inven_item_optimize(item);
	}
	else  /* Reduce and describe floor item */
	{
		floor_item_increase(0 - item, -1);
		floor_item_optimize(0 - item);
	}

	/* Chance of breakage (during attacks) */
	j = (hit_body ? breakage_chance(i_ptr) : 0);

	/* Drop (or break) near that location */
	drop_near(i_ptr, j, y, x);
}



/*
 * Throw an object from the pack or floor.
 *
 * Note: "unseen" monsters are very hard to hit.
 *
 * Should throwing a weapon do full damage?  Should it allow the magic
 * to hit bonus of the weapon to have an effect?  Should it ever cause
 * the item to be destroyed?  Should it do any damage at all?
 */
void do_cmd_throw(void)
{
	int dir, item;
	int i, j;
	s16b ty, tx, y, x;
	int chance, tdam, tdis;
	int weight;

	object_type *o_ptr;

	object_type *i_ptr;
	object_type object_type_body;

	bool hit_body = FALSE;

	byte missile_attr;
	char missile_char;

	char o_name[80];

	int path_n;
	u16b path_g[256];

	cptr q, s;

	int msec = op_ptr->delay_factor * op_ptr->delay_factor;


	/* Get an item */
	q = "Throw which item? ";
	s = "You have nothing to throw.";
	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return;

	/* Get the object */
	if (item >= 0)
	{
		o_ptr = &inventory[item];
	}
	else
	{
		o_ptr = &o_list[0 - item];
	}


	/* Get a direction (or cancel) */
	if (!get_aim_dir(&dir)) return;


	/* Get local object */
	i_ptr = &object_type_body;

	/* Obtain a local object */
	object_copy(i_ptr, o_ptr);

	/* Distribute the charges of rods/wands/staves between the stacks */
	distribute_charges(o_ptr, i_ptr, 1);

	/* Single object */
	i_ptr->number = 1;

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
		floor_item_optimize(0 - item);
	}


	/* Description */
	object_desc(o_name, sizeof(o_name), i_ptr, FALSE, ODESC_FULL);

	/* Find the color and symbol for the object for throwing */
	missile_attr = object_attr(i_ptr);
	missile_char = object_char(i_ptr);


	/* Enforce a minimum "weight" of one pound */
	weight = ((i_ptr->weight > 10) ? i_ptr->weight : 10);

	/* Hack -- Distance -- Reward strength, penalize weight */
	tdis = (adj_str_blow[p_ptr->state.stat_ind[A_STR]] + 20) * 10 / weight;

	/* Max distance of 10 */
	if (tdis > 10) tdis = 10;

	/* Hack -- Base damage from thrown object */
	tdam = damroll(i_ptr->dd, i_ptr->ds);
	if (!tdam) tdam = 1;
	tdam += i_ptr->to_d;

	/* Chance of hitting */
	chance = (p_ptr->state.skills[SKILL_TO_HIT_THROW] + (p_ptr->state.to_h * BTH_PLUS_ADJ));


	/* Take a turn */
	p_ptr->energy_use = 100;


	/* Start at the player */
	y = p_ptr->py;
	x = p_ptr->px;

	/* Predict the "target" location */
	ISBYTE(p_ptr->py + 99 * ddy[dir]);
	ty = (byte) (p_ptr->py + 99 * ddy[dir]);
	ISBYTE(p_ptr->px + 99 * ddx[dir]);
	tx = (byte) (p_ptr->px + 99 * ddx[dir]);

	/* Check for "target request" */
	if ((dir == 5) && target_okay())
	{
		target_get(&tx, &ty);
	}

	/* Calculate the path */
	path_n = project_path(path_g, tdis, p_ptr->py, p_ptr->px, ty, tx, 0);


	/* Hack -- Handle stuff */
	handle_stuff();

	/* Project along the path */
	for (i = 0; i < path_n; ++i)
	{
		s16b ny = GRID_Y(path_g[i]);
		s16b nx = GRID_X(path_g[i]);

		/* Hack -- Stop before hitting walls */
		if (!cave_floor(ny, nx)) break;

		/* Advance */
		x = nx;
		y = ny;

		/* Only do visuals if the player can "see" the missile */
		if (player_can_see_bold(y, x))
		{
			/* Visual effects */
			print_rel(missile_char, missile_attr, y, x);
			move_cursor_relative(y, x);

			Term_fresh();
			if (p_ptr->redraw) redraw_stuff();

			Term_xtra(TERM_XTRA_DELAY, msec);
			lite_spot(y, x);

			Term_fresh();
			if (p_ptr->redraw) redraw_stuff();
		}

		/* Delay anyway for consistency */
		else
		{
			/* Pause anyway, for consistancy */
			Term_xtra(TERM_XTRA_DELAY, msec);
		}

		/* Handle monster */
		if (cave_m_idx[y][x] > 0)
		{
			monster_type *m_ptr = &mon_list[cave_m_idx[y][x]];
			monster_race *r_ptr = &r_info[m_ptr->r_idx];

			int chance2 = chance - distance(p_ptr->py, p_ptr->px, y, x);

			int visible = m_ptr->ml;

			/* Note the collision */
			hit_body = TRUE;

			/* Did we hit it (penalize range) */
			if (test_hit(chance2, r_ptr->ac, m_ptr->ml))
			{
				const char *hit_verb = "hits";
				bool fear = FALSE;

				/* Assume a default death */
				cptr note_dies = " dies.";

				/* Some monsters get "destroyed" */
				if ((r_ptr->flags[2] & (RF2_DEMON)) ||
				    (r_ptr->flags[2] & (RF2_UNDEAD)) ||
				    (r_ptr->flags[1] & (RF1_STUPID)) ||
				    (strchr("Evg", r_ptr->d_char)))
				{
					/* Special note at death */
					note_dies = " is destroyed.";
				}

				/* Apply special damage  - brought forward to fill in hit_verb XXX XXX XXX */
				tdam *= get_brand_mult(i_ptr, m_ptr,
						&hit_verb, TRUE, FALSE);

				/* Handle unseen monster */
				if (!visible)
				{
					/* Invisible monster */
					msg_format("The %s finds a mark.", o_name);
				}

				/* Handle visible monster */
				else
				{
					char m_name[80];

					/* Get "the monster" or "it" */
					monster_desc(m_name, sizeof(m_name), m_ptr, 0);

					/* Message */
					msg_format("The %s %s %s.", o_name, hit_verb, m_name);

					/* Hack -- Track this monster race */
					if (m_ptr->ml) monster_race_track(m_ptr->r_idx);

					/* Hack -- Track this monster */
					if (m_ptr->ml) health_track(cave_m_idx[y][x]);
				}

				/* Apply special damage XXX XXX XXX */
				tdam = critical_shot(i_ptr->weight, i_ptr->to_h, tdam);

				/* No negative damage */
				if (tdam < 0) tdam = 0;

				/* Complex message */
				if (p_ptr->wizard)
				{
					msg_format("You do %d (out of %d) damage.",
					           tdam, m_ptr->hp);
				}

				/* Hit the monster, check for death */
				if (mon_take_hit(cave_m_idx[y][x], tdam, &fear, note_dies))
				{
					/* Dead monster */
				}

				/* No death */
				else
				{
					/* Message */
					message_pain(cave_m_idx[y][x], tdam);

					/* Take note */
					if (fear && m_ptr->ml)
					{
						char m_name[80];

						/* Get the monster name (or "it") */
						monster_desc(m_name, sizeof(m_name), m_ptr, 0);

						/* Message */
						message_format(MSG_FLEE, m_ptr->r_idx,
						               "%^s flees in terror!", m_name);
					}
				}
			}

			/* Stop looking */
			break;
		}
	}

	/* Chance of breakage (during attacks) */
	j = (hit_body ? breakage_chance(i_ptr) : 0);

	/* Drop (or break) near that location */
	drop_near(i_ptr, j, y, x);
}

