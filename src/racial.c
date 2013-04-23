/* File: racial.c */

/* Purpose: Racial powers (and mutations) */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"



/*
 * Returns the chance to activate a racial power/mutation
 */
static int racial_chance(s16b min_level, int use_stat, int difficulty)
{
	int i;
	int val;
	int sum = 0;
	int stat = p_ptr->stat_cur[use_stat];

	/* No chance for success */
	if ((p_ptr->lev < min_level) || p_ptr->confused)
	{
		return (0);
	}

	/* Calculate difficulty */
	if (p_ptr->stun)
	{
		difficulty += p_ptr->stun;
	}
	else if (p_ptr->lev > min_level)
	{
		int lev_adj = ((p_ptr->lev - min_level) / 3);
		if (lev_adj > 10) lev_adj = 10;
		difficulty -= lev_adj;
	}

	if (difficulty < 5) difficulty = 5;

	/* We only need halfs of the difficulty */
	difficulty = difficulty / 2;

	for (i = 1; i <= stat; i++)
	{
		val = i - difficulty;
		if (val > 0)
			sum += (val <= difficulty) ? val : difficulty;
	}

	if (difficulty == 0)
		return (100);
	else
		return (((sum * 100) / difficulty) / stat);
}

/* 
 * I realize it is somewhat illogical to have this as a "power" rather
 * than an extension of the "eat" command, but I could not think of
 * a handy solution to the conceptual/UI problem of having food objects AND
 * an edible corpse in the same square...
 * Eating corpses should probably take more than 1 turn (realistically). 
 * (OTOH, you can swap your full-plate armour for a dragonscalemail in
 * 1 turn *shrug*) 
 */
static void eat_corpse(void)
{
	s16b fld_idx;
	field_type *f_ptr;
	
	fld_idx = area(p_ptr->py, p_ptr->px)->fld_idx;
	
	/* While there are fields in the linked list */
	while (fld_idx)
	{
		f_ptr = &fld_list[fld_idx];

		/* Want a corpse / skeleton */
		if ((f_ptr->t_idx == FT_CORPSE || f_ptr->t_idx == FT_SKELETON))
		{
			if (f_ptr->t_idx == FT_CORPSE)
			{
				msg_print("The corpse tastes delicious!");
				(void)set_food(p_ptr->food + 2000);
			}
			else
			{
				msg_print("The bones taste delicious!");
				(void)set_food(p_ptr->food + 1000);
			}

			/* Sound */
			sound(SOUND_EAT);

			delete_field_idx(fld_idx);
				
			/* Done */
			return;
		}

		/* Get next field in list */
		fld_idx = f_ptr->next_f_idx;
	} 

	/* Nothing to eat */
	msg_print("There is no fresh skeleton or corpse here!");
	p_ptr->energy_use = 0;
	
	/* Done */
    return;
}



/*
 * Note: return value indicates that we have succesfully used the power
 */
bool racial_aux(s16b min_level, int cost, int use_stat, int fail_stat, int difficulty)
{
	bool use_life = FALSE;

	/* Power is not available yet */
	if (p_ptr->lev < min_level)
	{
		msg_format("You need to attain level %d to use this power.", min_level);
		p_ptr->energy_use = 0;
		return FALSE;
	}

	/* Too confused */
	else if (p_ptr->confused)
	{
		msg_print("You are too confused to use this power.");
		p_ptr->energy_use = 0;
		return FALSE;
	}

	/* Risk death? */
	else if ((use_stat == HP) && (p_ptr->chp < cost))
	{
		if (!get_check("Really use the power in your weakened state? "))
		{
			p_ptr->energy_use = 0;
			return FALSE;
		}
	}

	else if ((use_stat == SP) && (p_ptr->csp < cost))
	{
		if (!get_check("Use life force in spell? "))
		{
			p_ptr->energy_use = 0;
			return FALSE;
		}
		else
		{
			use_life = TRUE;
		}
	}
	
	else if ((use_stat == RP) && (p_ptr->crp < cost))
	{
		msg_print("You do not have enough resources. ");
		p_ptr->energy_use = 0;
		return FALSE;
	}

	/* Else attempt to do it! */

	if (p_ptr->stun)
	{
		difficulty += p_ptr->stun;
	}
	else if (p_ptr->lev > min_level)
	{
		int lev_adj = ((p_ptr->lev - min_level) / 3);
		if (lev_adj > 10) lev_adj = 10;
		difficulty -= lev_adj;
	}

	if (difficulty < 5) difficulty = 5;

	/* take time and pay the price */
	p_ptr->energy_use = 100;

	if (use_life)
	{
		cost -= p_ptr->csp;
		take_hit(rand_range(cost/2,cost*2), "using lifeforce as mana");
		p_ptr->csp = 0;
	}
	else if (use_stat == HP)
	{
		take_hit(cost, "concentrating too hard");
	}
	else if (use_stat == SP)
	{
		p_ptr->csp -= cost;
	}
	else if (use_stat == RP)
	{
		p_ptr->crp -= cost;
	}
	else if (use_stat == FP)
	{
		p_ptr->fatigue += cost;
	}

	/* Redraw mana and hp */
	p_ptr->redraw |= (PR_HP | PR_MANA | PR_FATIGUE);

	/* Window stuff */
	p_ptr->window |= (PW_PLAYER | PW_SPELL);

	/* Success? */
	if (randint1(p_ptr->stat_cur[fail_stat]) >= 
		rand_range(difficulty / 2, difficulty))
	{
		return TRUE;
	}

	msg_print("You've failed to concentrate hard enough.");
	return FALSE;
}


static void cmd_racial_power_aux(const mutation_type *mut_ptr)
{
	s16b        plev = p_ptr->lev;
	int         dir = 0;

	if (racial_aux(mut_ptr->level, mut_ptr->cost, mut_ptr->use, mut_ptr->stat, mut_ptr->diff))
	{

		switch (p_ptr->prace)
		{
			case RACE_DWARF:
			{
				msg_print("You examine your surroundings.");
				(void)detect_traps();
				(void)detect_doors();
				(void)detect_stairs();
				break;
			}		

			case RACE_HALFLING:
			{
				object_type *q_ptr;
				object_type forge;

				/* Get local object */
				q_ptr = &forge;

				/* Create the food ration */
				object_prep(q_ptr, 21);

				/* Drop the object from heaven */
				(void)drop_near(q_ptr, -1, p_ptr->py, p_ptr->px);
				msg_print("You cook some food.");
				
				break;
			}

			case RACE_GNOME:
			{
				msg_print("Blink!");
				teleport_player(10 + plev);
				break;
			}

			case RACE_GOBLIN:
			{
				msg_print("You focus magic.");
				(void)recharge(plev * 2);
				break;
			}

			case RACE_DRYAD:
			{
				(void)hp_player(damroll(4, 10));
				(void)set_cut((p_ptr->cut / 2) - 20);
				(void)set_poisoned(0);
				msg_print("You cure your wounds");
				break;
			}

			case RACE_DRACONIAN:
			{
				if (!get_aim_dir(&dir)) return FALSE;

				(void)fire_ball(GF_DOMINATION, dir, plev*2, -3);
			}
			break;
				
/*			{	
				int px = p_ptr->px;
				int py = p_ptr->py;
				for (dir != ( 0 || 5 ); dir <= 9; dir++)
				/* Hack - use levels to choose ability * /
				if (mut_ptr->level == 20)
				{	
					int  Type = (one_in_(3) ? GF_COLD : GF_FIRE);
					int  DamM;
					cptr Name = ((Type == GF_COLD) ? "cold" : "fire");
					/* No effect if not a magic user * /
					if (p_ptr->realm1 == REALM_NONE)
					{
						msg_print("You don't use magic.");
					}
					else
					{
						switch (p_ptr->realm1)
						{
							case REALM_FIRE:
							DamM == 2;
								if (!one_in_(3))
								{
									Type = GF_FIRE;
									Name = "fire";
								}
								else if (one_in_(2))
									{
										Type = GF_LAVA;
										Name = "lava";
									}
									else
									{
										Type = GF_PLASMA;
										Name = "plasma";
									}
								break;
							case REALM_WATER:
							DamM == 2;
								if (!one_in_(3))
								{
									Type = GF_COLD;
									Name = "cold";
								}
								else if (!one_in_(3))
									{
										Type = GF_WATER;
										Name = "water";
									}
								else if (one_in_(2))
									{
										Type = GF_ICE;
										Name = "ice";
									}
									else
									{
										Type = GF_ACID;
										Name = "acid";
									}
								break;
							case REALM_EARTH:
							DamM == 2;
								if (!one_in_(3))
								{
									Type = GF_STUN;
									Name = "stunning";
								}
								else if (!one_in_(2))
								{
									Type = GF_GRAVITY;
									Name = "gravity";
								}
								else
								{
									Type = GF_QUAKE;
									Name = "earthquake";
								}
								break;
							case REALM_AIR:
							DamM == 2;
								if (!one_in_(3))
								{
									Type = GF_ELEC;
									Name = "lightning";
								}
								else
								{
									Type = GF_SOUND;
									Name = "thunder";
								}
								break;
							case REALM_LIFE:
							DamM == 2;
								if (!one_in_(3))
								{
									Type = GF_LITE;
									Name = "light";
								}
								else
								{
									Type = GF_HOLY_FIRE;
									Name = "holiness";
								}
								break;
							case REALM_DEATH:
							DamM == 2;
								if (one_in_(2))
									{
										Type = GF_DARK;
										Name = "dark";
									}
									else if (!one_in_(3))
										{
											Type = GF_NETHER;
											Name = "nether";
											
										}
										else if (one_in_(2))
										{
											Type = GF_POIS;
											Name = "poison";
										}
										else
										{
											Type = GF_HELL_FIRE;
											Name = "unholiness";
										}
								break;
							case REALM_ORDER:
							DamM == 2;
								if (!one_in_(3))
								{
									Type = GF_TEMP;
									Name = "temperature";
								}
								else
								{
									Type = GF_INERTIA;
									Name = "inertia";
								}
								break;
							case REALM_CHAOS:
							DamM == 2;
								if (one_in_(2))
								{
									Type = GF_CHAOS;
									Name = "chaos";
								}
								else
								{
									Type = GF_NEXUS;
									Name = "warp";
								}
								break;
							case REALM_ASTRAL:
							DamM == 1;
								if (one_in_(2))
								{
									Type = GF_MISSILE;
									Name = "magic";
								}
								else
								{
									Type = GF_FORCE;
									Name = "force";
								}
								break;
							case REALM_WIZARD:
							DamM == 2;
								if (one_in_(2))
								{
									Type = GF_MANA;
									Name = "pure magic";
								}
								else
								{
									Type = GF_METEOR;
									Name = "meteor";
								}
								break;							
						}
						
				if (plev < 25)
				{ 
				msg_format( "You invoke %s.", Name);
				(void)fire_bolt(Type, dir, plev * DamM);
				}
				else if (plev < 35)
				{ 
				msg_format( "You invoke %s.", Name);
				(void)fire_beam(Type, dir, plev * DamM);
				}
				else if (plev < 45)
				{ 
				msg_format( "You invoke %s.", Name);
				(void)fire_ball(Type, dir, plev * DamM, 3);
				}
				else 
				{ 
				msg_format( "You invoke %s.", Name);
				(void)fire_ball(Type, dir, plev * DamM, -3);
				}
				
			
					
					}
				}

			} */
			
			case RACE_ENT:
			{
				if (mut_ptr->level == 25)
				{
					(void)set_cut(0);
					(void)do_res_stat(A_STR);
					(void)do_res_stat(A_INT);
					(void)do_res_stat(A_WIS);
					(void)do_res_stat(A_DEX);
					(void)do_res_stat(A_CON);
					(void)do_res_stat(A_CHR);
					msg_print("You regrow");
					break;
				}
				else if (mut_ptr->level == 20)
				{
					if (!get_aim_dir(&dir)) return FALSE;
					(void)wall_to_mud(dir);
					break;
				}
			}

			case RACE_YEEK:
			{
				if (!get_aim_dir(&dir)) break;
				msg_print("You make a horrible scream!");
				(void)fear_monster(dir, plev);
				break;
			}

			case RACE_KOBOLD:
			{
				if (!get_aim_dir(&dir)) break;
				msg_print("You throw a dart of poison.");
				(void)fire_bolt(GF_POIS, dir, plev);
				break;
			}

			case RACE_ILLITHID:
			{
				if (!get_aim_dir(&dir)) break;
				else
				{
					msg_print("You concentrate and your eyes glow red...");
					(void)fire_bolt(GF_PSI, dir, plev);
				}
				
				break;
			}

			case RACE_VAMPIRE:
			{
				int y, x, dummy;
				cave_type *c_ptr;

				/* Only works on adjacent monsters */
				if (!get_rep_dir(&dir)) break;
				y = p_ptr->py + ddy[dir];
				x = p_ptr->px + ddx[dir];

				/* Paranoia */
				if (!in_bounds2(y, x)) break;

				c_ptr = area(y, x);

				if (!c_ptr->m_idx)
				{
					msg_print("You bite into thin air!");
					break;
				}

				msg_print("You grin and bare your fangs...");
				dummy = plev + randint1(plev) * MAX(1, plev / 10);   /* Dmg */
				if (drain_gain_life(dir, dummy))
				{
					/* Gain nutritional sustenance: 150/hp drained */
					/* A Food ration gives 5000 food points (by contrast) */
					/* Don't ever get more than "Full" this way */
					/* But if we ARE Gorged,  it won't cure us */
					dummy = p_ptr->food + MIN(5000, 100 * dummy);
					if (p_ptr->food < PY_FOOD_MAX)   /* Not gorged already */
						(void)set_food(dummy >= PY_FOOD_MAX ? PY_FOOD_MAX - 1 : dummy);
				}
				else
					msg_print("Yechh. That tastes foul.");
				break;
			}

			case RACE_FAUN:
			{
				msg_print("You throw a dream");
				if (!get_aim_dir(&dir));
				(void)fire_bolt(GF_OLD_SLEEP, dir, damroll((2+plev/2), 3));
				break;
			}

			case RACE_PIXIE:
			{
				msg_print("You throw some magic dust...");
				if (plev < 25)
					(void)sleep_monsters_touch();
				else
					(void)sleep_monsters();
				break;
			}
			
			case RACE_TROLL_SWAMP:
			case RACE_WOLFMAN:
			{
				eat_corpse();
			}
	
		}
					
	}

	/* Redraw mana and hp */
	p_ptr->redraw |= (PR_HP | PR_MANA | PR_FATIGUE);

	/* Window stuff */
	p_ptr->window |= (PW_PLAYER | PW_SPELL);
}


typedef struct power_desc_type power_desc_type;

struct power_desc_type
{
	char name[40];
	int  level;
	int  cost;
	int  use;
	int  fail;
	int  number;
	const mutation_type *power;
};


/*
 * Allow user to choose a power (racial / mutation) to activate
 */
void do_cmd_racial_power(void)
{
	power_desc_type power_desc[36];
	int             num, ask, i = 0;
	bool            flag, redraw;
	char            choice;
	char            out_val[160];
	
	const mutation_type	*mut_ptr;

	/* Wipe desc */
	for (num = 0; num < 36; num++)
	{
		strcpy(power_desc[num].name, "");
		power_desc[num].number = 0;
	}

	/* Reset num */
	num = 0;

	/* Not when we're confused */
	if (p_ptr->confused)
	{
		msg_print("You are too confused to use any powers!");
		p_ptr->energy_use = 0;
		return;
	}

	/* Look for racial powers */
	for (i = 0; i < MAX_RACE_POWERS; i++)
	{
		mut_ptr = &race_powers[i];
		
		if (mut_ptr->which == p_ptr->prace)
		{
			strcpy(power_desc[num].name, mut_ptr->name);
			power_desc[num].level = mut_ptr->level;
			power_desc[num].cost = mut_ptr->cost;
			power_desc[num].fail = 100 -
				racial_chance(mut_ptr->level, mut_ptr->stat, mut_ptr->diff);
			power_desc[num].number = -1;
			power_desc[num++].power = mut_ptr;
		}	
	}

	/* Not if we don't have any */
	if (num == 0 && !p_ptr->muta1)
	{
		msg_print("You have no powers to activate.");
		p_ptr->energy_use = 0;
		return;
	}
	
	/* Look for appropriate mutations */
	if (p_ptr->muta1)
	{
		for (i = 0; i < MUT_PER_SET; i++)
		{
			mut_ptr = &mutations[i];
			
			if (p_ptr->muta1 & mut_ptr->which)
			{
				strcpy(power_desc[num].name, mut_ptr->name);
				power_desc[num].level = mut_ptr->level;
				power_desc[num].cost = mut_ptr->cost;
				power_desc[num].fail = 100 -
					racial_chance(mut_ptr->level, mut_ptr->stat, mut_ptr->diff);
				power_desc[num].number = mut_ptr->which;
				power_desc[num++].power = mut_ptr;
			}
		}
	}

	/* Nothing chosen yet */
	flag = FALSE;

	/* No redraw yet */
	redraw = FALSE;

	/* Build a prompt */
	(void)strnfmt(out_val, 78, "(Powers %c-%c, *=List, ESC=exit) Use which power? ",
		I2A(0), (num <= 26) ? I2A(num - 1) : '0' + num - 27);

	if (!repeat_pull(&i) || i<0 || i>=num)
	{
		/* Get a spell from the user */
		while (!flag && get_com(out_val, &choice))
		{
			/* Request redraw */
			if ((choice == ' ') || (choice == '*') || (choice == '?'))
			{
				/* Show the list */
				if (!redraw)
				{
					byte y = 1, x = 0;
					int ctr = 0;
					char dummy[80];
					char letter;
					int x1, y1;

					strcpy(dummy, "");

					/* Show list */
					redraw = TRUE;

					/* Save the screen */
					screen_save();

					/* Print header(s) */
					if (num < 17)
						prt("                            Lv Cost Fail", y++, x);
					else
						prt("                            Lv Cost Fail                            Lv Cost Fail", y++, x);

					/* Print list */
					while (ctr < num)
					{
						/* letter/number for power selection */
						if (ctr < 26)
							letter = I2A(ctr);
						else
							letter = '0' + ctr - 26;
						x1 = ((ctr < 17) ? x : x + 40);
						y1 = ((ctr < 17) ? y + ctr : y + ctr - 17);

						sprintf(dummy, " %c) %-23.23s %2d %4d %3d%%",
							letter,
							power_desc[ctr].name,
							power_desc[ctr].level,
							power_desc[ctr].cost,
							power_desc[ctr].fail);
						prt(dummy, y1, x1);
						ctr++;
					}
				}

				/* Hide the list */
				else
				{
					/* Hide list */
					redraw = FALSE;

					/* Restore the screen */
					screen_load();
				}

				/* Redo asking */
				continue;
			}

			if (choice == '\r' && num == 1)
			{
				choice = 'a';
			}

			if (isalpha(choice))
			{
				/* Note verify */
				ask = (isupper(choice));

				/* Lowercase */
				if (ask) choice = tolower(choice);

				/* Extract request */
				i = (islower(choice) ? A2I(choice) : -1);
			}
			else
			{
				ask = FALSE; /* Can't uppercase digits */

				i = choice - '0' + 26;
			}

			/* Totally Illegal */
			if ((i < 0) || (i >= num))
			{
				bell();
				continue;
			}

			/* Verify it */
			if (ask)
			{
				char tmp_val[160];
	
				/* Prompt */
				(void)strnfmt(tmp_val, 78, "Use %s? ", power_desc[i].name);

				/* Belay that order */
				if (!get_check(tmp_val)) continue;
			}

			/* Stop the loop */
			flag = TRUE;
		}

		/* Restore the screen */
		if (redraw) screen_load();

		/* Abort if needed */
		if (!flag)
		{
			p_ptr->energy_use = 0;
			return;
		}

        repeat_push(i);
	}

	
	
	if (power_desc[i].number == -1)
	{
		cmd_racial_power_aux(power_desc[i].power);
	}
	else
	{
		mutation_power_aux(power_desc[i].power);
	}

	/* Success */
	return;
}
