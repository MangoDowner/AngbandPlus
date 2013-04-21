/* File: use_obj.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

#include "angband.h"

static cptr desc_stat_neg[] =
{
	"weak",
	"stupid",
	"naive",
	"clumsy",
	"sickly",
	"slow",
	"noisy",
	"short-sighted",
	"unlucky"
};

static bool eat_food(object_type *o_ptr, bool *ident)
{
	/* Analyze the food */
	switch (o_ptr->sval)
	{/*
		case SV_FOOD_POISON:
		{
			if (!(p_ptr->resist_pois || p_ptr->oppose_pois || p_ptr->immune_pois))
			{
				if (set_poisoned(p_ptr->poisoned + rand_int(10) + 10))
				{
					*ident = TRUE;
				}
			}
			break;
		}

		case SV_FOOD_BLINDNESS:
		{
			if (!p_ptr->resist_blind)
			{
				if (set_blind(p_ptr->blind + rand_int(200) + 200))
				{
					*ident = TRUE;
				}
			}
			break;
		}

		case SV_FOOD_PARANOIA:
		{
			if (!p_ptr->resist_fear)
			{
				if (set_afraid(p_ptr->afraid + rand_int(10) + 10))
				{
					*ident = TRUE;
				}
			}
			break;
		}

		case SV_FOOD_CONFUSION:
		{
			if (allow_player_confusion())
			{
				if (set_confused(p_ptr->confused + rand_int(10) + 10))
				{
					*ident = TRUE;
				}
			}
			break;
		}

		case SV_FOOD_HALLUCINATION:
		{
			if (!p_ptr->resist_chaos)
			{
				if (set_image(p_ptr->image + rand_int(250) + 250))
				{
					*ident = TRUE;
				}
			}
			break;
		}

		case SV_FOOD_PARALYSIS:
		{
			if (!p_ptr->free_act)
			{
				if (set_paralyzed(p_ptr->paralyzed + rand_int(10) + 10))
				{
					*ident = TRUE;
				}
			}
			break;
		}

		case SV_FOOD_WEAKNESS:
		{
			take_hit(damroll(6, 6), "poisonous food");
			(void)do_dec_stat(A_STR);
			*ident = TRUE;
			break;
		}

		case SV_FOOD_SICKNESS:
		{
			take_hit(damroll(6, 6), "poisonous food");
			(void)do_dec_stat(A_CON);
			*ident = TRUE;
			break;
		}

		case SV_FOOD_STUPIDITY:
		{
			take_hit(damroll(8, 8), "poisonous food");
			(void)do_dec_stat(A_INT);
			*ident = TRUE;
			break;
		}

		case SV_FOOD_NAIVETY:
		{
			take_hit(damroll(8, 8), "poisonous food");
			(void)do_dec_stat(A_WIS);
			*ident = TRUE;
			break;
		}

		case SV_FOOD_UNHEALTH:
		{
			take_hit(damroll(10, 10), "poisonous food");
			(void)do_dec_stat(A_CON);
			*ident = TRUE;
			break;
		}

		case SV_FOOD_DISEASE:
		{
			take_hit(damroll(10, 10), "poisonous food");
			(void)do_dec_stat(A_STR);
			*ident = TRUE;
			break;
		}

		case SV_FOOD_CURE_POISON:
		{
			if (set_poisoned(0)) *ident = TRUE;
			break;
		}

		case SV_FOOD_CURE_BLINDNESS:
		{
			if (set_blind(0)) *ident = TRUE;
			break;
		}

		case SV_FOOD_CURE_PARANOIA:
		{
			if (set_afraid(0)) *ident = TRUE;
			break;
		}

		case SV_FOOD_CURE_CONFUSION:
		{
			if (set_confused(0)) *ident = TRUE;
			break;
		}

		case SV_FOOD_CURE_SERIOUS:
		{
			if (hp_player(damroll(6, 8))) *ident = TRUE;
			break;
		}

		case SV_FOOD_RESTORE_STR:
		{
			if (do_res_stat(A_STR)) *ident = TRUE;
			break;
		}

		case SV_FOOD_RESTORE_CON:
		{
			if (do_res_stat(A_CON)) *ident = TRUE;
			break;
		}

		case SV_FOOD_RESTORING:
		{
			if (do_res_stat(A_STR)) *ident = TRUE;
			if (do_res_stat(A_INT)) *ident = TRUE;
			if (do_res_stat(A_WIS)) *ident = TRUE;
			if (do_res_stat(A_DEX)) *ident = TRUE;
			if (do_res_stat(A_CON)) *ident = TRUE;
			if (do_res_stat(A_AGI)) *ident = TRUE;
			if (do_res_stat(A_STE)) *ident = TRUE;
			if (do_res_stat(A_PER)) *ident = TRUE;
			if (do_res_stat(A_LUC)) *ident = TRUE;
			break;
		}*/

		case SV_FOOD_APPLE:
		{
			msg_print("(crunch, crunch)");
			*ident = TRUE;
			break;
		}
		
		case SV_FOOD_JERKY:
		{
			msg_print("Chewy!");
			*ident = TRUE;
			break;
		}

		case SV_FOOD_RATION:
		{
			msg_print("That tastes good.");
			*ident = TRUE;
			break;
		}

		case SV_FOOD_WAYBREAD:
		{
			msg_print("That tastes good.");
			(void)set_poisoned(0);
			(void)hp_player(damroll(4, 8));
			*ident = TRUE;
			break;
		}

	}

	/* Food can feed the player */
	(void)set_food(p_ptr->food + o_ptr->pval);

	return (TRUE);
}


static bool quaff_potion(object_type *o_ptr, bool *ident)
{
	/* Analyze the potion */
	switch (o_ptr->sval)
	{
		case SV_POTION_WATER:
		case SV_POTION_APPLE_JUICE:
		case SV_POTION_SLIME_MOLD:
		{
			msg_print("You feel less thirsty.");
			*ident = TRUE;
			break;
		}

		case SV_POTION_SLOWNESS:
		{
			if (set_slow(p_ptr->slow + randint(25) + 15)) *ident = TRUE;
			break;
		}

		case SV_POTION_SALT_WATER:
		{
			msg_print("The potion makes you vomit!");
			(void)set_food(PY_FOOD_STARVE - 1);
			(void)set_poisoned(0);
			(void)set_paralyzed(p_ptr->paralyzed + 4);
			*ident = TRUE;
			break;
		}

		case SV_POTION_POISON:
		{
			if (!(p_ptr->resist_pois || p_ptr->oppose_pois || p_ptr->immune_pois))
			{
				if (set_poisoned(p_ptr->poisoned + rand_int(15) + 10))
				{
					*ident = TRUE;
				}
			}
			break;
		}

		case SV_POTION_BLINDNESS:
		{
			if (!p_ptr->resist_blind)
			{
				if (set_blind(p_ptr->blind + rand_int(100) + 100))
				{
					*ident = TRUE;
				}
			}
			break;
		}

		case SV_POTION_CONFUSION:
		{
			if (allow_player_confusion())
			{
				if (set_confused(p_ptr->confused + rand_int(20) + 15))
				{
					*ident = TRUE;
				}
			}
			break;
		}

		case SV_POTION_SLEEP:
		{
			if (!p_ptr->free_act)
			{
				if (set_paralyzed(p_ptr->paralyzed + rand_int(4) + 4))
				{
					*ident = TRUE;
				}
			}
			break;
		}

		case SV_POTION_LOSE_MEMORIES:
		{
			if (!p_ptr->hold_life && (p_ptr->exp > 0))
			{
				msg_print("You feel your memories fade.");
				lose_exp(p_ptr->exp / 4);
				*ident = TRUE;
			}
			break;
		}

		case SV_POTION_DRAIN_MANA:
		{
			if (p_ptr->csp)
			{
				p_ptr->csp /= 2;
				msg_print("You feel your head cloud up.");
				p_ptr->redraw |= (PR_MANA);
				p_ptr->window |= (PW_PLAYER_0 | PW_PLAYER_1);
				*ident = TRUE;
			}
			break;
		}

		case SV_POTION_RUINATION:
		{
			msg_print("Your nerves and muscles feel weak and lifeless!");
			take_hit(damroll(10, 10), "a potion of Ruination");
			(void)dec_stat(A_DEX, 25, TRUE);
			(void)dec_stat(A_WIS, 25, TRUE);
			(void)dec_stat(A_CON, 25, TRUE);
			(void)dec_stat(A_STR, 25, TRUE);
			(void)dec_stat(A_INT, 25, TRUE);
			(void)dec_stat(A_AGI, 25, TRUE);
			(void)dec_stat(A_STE, 25, TRUE);
			(void)dec_stat(A_PER, 25, TRUE);
			(void)dec_stat(A_LUC, 25, TRUE);
			*ident = TRUE;
			break;
		}

		case SV_POTION_INFRAVISION:
		{
			if (set_tim_infra(p_ptr->tim_infra + 100 + randint(100)))
			{
				*ident = TRUE;
			}
			break;
		}

		case SV_POTION_DETECT_INVIS:
		{
			if (set_tim_invis(p_ptr->tim_invis + 12 + randint(12)))
			{
				*ident = TRUE;
			}
			break;
		}

		case SV_POTION_CLARITY:
		{
			set_confused(0);
			set_oppose_conf(50+randint(50));
			*ident = TRUE;
			break;
		}

		case SV_POTION_DISAPPEARANCE:
		{
			teleport_player(30);
			*ident = TRUE;
			break;
		}

		case SV_POTION_SLOW_POISON:
		{
			if (set_poisoned(p_ptr->poisoned / 2)) *ident = TRUE;
			break;
		}

		case SV_POTION_CURE_POISON:
		{
			if (set_poisoned(0)) *ident = TRUE;
			break;
		}

		case SV_POTION_BOLDNESS:
		{
			if (set_afraid(0)) *ident = TRUE;
			break;
		}

		case SV_POTION_SPEED:
		{
			if (!p_ptr->fast)
			{
				if (set_fast(randint(25) + 15)) *ident = TRUE;
			}
			else
			{
				(void)set_fast(p_ptr->fast + 5);
			}
			break;
		}

		case SV_POTION_RESIST_HEAT:
		{
			if (set_oppose_fire(p_ptr->oppose_fire + randint(10) + 10))
			{
				*ident = TRUE;
			}
			break;
		}

		case SV_POTION_RESIST_COLD:
		{
			if (set_oppose_cold(p_ptr->oppose_cold + randint(10) + 10))
			{
				*ident = TRUE;
			}
			break;
		}

		case SV_POTION_HEROISM:
		{
			if (hp_player(10)) *ident = TRUE;
			if (set_afraid(0)) *ident = TRUE;
			if (set_hero(p_ptr->hero + randint(25) + 25)) *ident = TRUE;
			break;
		}

		case SV_POTION_BERSERK_STRENGTH:
		{
			if (hp_player(30)) *ident = TRUE;
			if (set_afraid(0)) *ident = TRUE;
			if (set_shero(p_ptr->shero + randint(25) + 25)) *ident = TRUE;
			break;
		}

		case SV_POTION_CURE_LIGHT:
		{
			if (hp_player(damroll(3, 10))) *ident = TRUE;
			if (set_blind(0)) *ident = TRUE;
			if (set_cut(p_ptr->cut - 10)) *ident = TRUE;
			break;
		}

		case SV_POTION_CURE_SERIOUS:
		{
			if (hp_player(damroll(8, 10))) *ident = TRUE;
			if (set_blind(0)) *ident = TRUE;
			if (set_confused(0)) *ident = TRUE;
			if (set_cut((p_ptr->cut / 2) - 50)) *ident = TRUE;
			break;
		}

		case SV_POTION_CURE_CRITICAL:
		{
			if (hp_player(damroll(12, 12))) *ident = TRUE;
			if (set_blind(0)) *ident = TRUE;
			if (set_confused(0)) *ident = TRUE;
			if (set_stun(0)) *ident = TRUE;
			if (set_cut(0)) *ident = TRUE;
			break;
		}

		case SV_POTION_HEALING:
		{
			if (hp_player(200)) *ident = TRUE;
			if (set_blind(0)) *ident = TRUE;
			if (set_confused(0)) *ident = TRUE;
			if (set_poisoned(0)) *ident = TRUE;
			if (set_stun(0)) *ident = TRUE;
			if (set_cut(0)) *ident = TRUE;
			break;
		}

		case SV_POTION_STAR_HEALING:
		{
			if (hp_player(400)) *ident = TRUE;
			if (set_blind(0)) *ident = TRUE;
			if (set_confused(0)) *ident = TRUE;
			if (set_poisoned(0)) *ident = TRUE;
			if (set_stun(0)) *ident = TRUE;
			if (set_cut(0)) *ident = TRUE;
			break;
		}

		case SV_POTION_LIFE:
		{
			msg_print("You feel life flow through your body!");
			restore_level();
			(void)set_poisoned(0);
			(void)set_blind(0);
			(void)set_confused(0);
			(void)set_image(0);
			(void)set_stun(0);
			(void)set_cut(0);
			(void)do_res_stat(A_STR);
			(void)do_res_stat(A_CON);
			(void)do_res_stat(A_DEX);
			(void)do_res_stat(A_WIS);
			(void)do_res_stat(A_INT);
			(void)do_res_stat(A_AGI);
			(void)do_res_stat(A_STE);
			(void)do_res_stat(A_PER);
			(void)do_res_stat(A_LUC);

			/* Recalculate max. hitpoints */
			update_stuff();

			hp_player(5000);

			*ident = TRUE;
			break;
		}

		case SV_POTION_RESTORE_MANA:
		{
			if (p_ptr->csp < p_ptr->msp)
			{
				regenmana(10);
				msg_print("You feel your head clear a little.");
				p_ptr->redraw |= (PR_MANA);
				p_ptr->window |= (PW_PLAYER_0 | PW_PLAYER_1);
				*ident = TRUE;
			}
			break;
		}

		case SV_POTION_RESTORE_EXP:
		{
			if (restore_level()) *ident = TRUE;
			break;
		}

		case SV_POTION_RES_STR:
		{
			if (do_res_stat(A_STR)) *ident = TRUE;
			break;
		}

		case SV_POTION_RES_INT:
		{
			if (do_res_stat(A_INT)) *ident = TRUE;
			if (set_confused(0)) *ident = TRUE;
			break;
		}

		case SV_POTION_RES_WIS:
		{
			if (do_res_stat(A_WIS)) *ident = TRUE;
			if (set_afraid(0)) *ident = TRUE;
			(void)set_image(0);
			break;
		}

		case SV_POTION_RES_DEX:
		{
			if (do_res_stat(A_DEX)) *ident = TRUE;
			if (set_stun(0)) *ident = TRUE;
			break;
		}

		case SV_POTION_RES_CON:
		{
			if (do_res_stat(A_CON)) *ident = TRUE;
			if (set_poisoned(0)) *ident = TRUE;
			if (set_cut(0)) *ident = TRUE;
			break;
		}

		case SV_POTION_RES_AGI:
		{
			if (do_res_stat(A_AGI)) *ident = TRUE;
			break;
		}

		case SV_POTION_RES_STE:
		{
			if (do_res_stat(A_STE)) *ident = TRUE;
			break;
		}

		case SV_POTION_RES_PER:
		{
			if (do_res_stat(A_PER)) *ident = TRUE;
			if (set_blind(0)) *ident = TRUE;
			break;
		}

		case SV_POTION_RES_LUC:
		{
			if (do_res_stat(A_LUC)) *ident = TRUE;
			break;
		}


		case SV_POTION_ENLIGHTENMENT:
		{
			msg_print("An image of your surroundings forms in your mind...");
			wiz_lite();
			*ident = TRUE;
			break;
		}

		case SV_POTION_SELF_KNOWLEDGE:
		{
			msg_print("You begin to know yourself a little better...");
			message_flush();
			self_knowledge();
			*ident = TRUE;
			break;
		}

		case SV_POTION_EXCH_1:
		{

			*ident = TRUE;
			do_inc_stat(A_LUC);
			new_dec_stat(A_WIS,1,1);
			new_dec_stat(A_WIS,1,0);
			message_format(MSG_DRAIN_STAT, A_WIS, "You feel very %s.", desc_stat_neg[A_WIS]);
			message_flush();
			break;
		}
		
		case SV_POTION_EXCH_2:
		{

			*ident = TRUE;
			do_inc_stat(A_PER);
			new_dec_stat(A_LUC,1,1);
			new_dec_stat(A_LUC,1,0);
			message_format(MSG_DRAIN_STAT, A_LUC, "You feel very %s.", desc_stat_neg[A_LUC]);
			message_flush();
			break;
		}
		
		case SV_POTION_EXCH_3:
		{

			*ident = TRUE;
			do_inc_stat(A_STE);
			new_dec_stat(A_AGI,1,1);
			new_dec_stat(A_AGI,1,0);
			message_format(MSG_DRAIN_STAT, A_AGI, "You feel very %s.", desc_stat_neg[A_AGI]);
			message_flush();
			break;
		}
		
		case SV_POTION_EXCH_4:
		{

			*ident = TRUE;
			do_inc_stat(A_CON);
			new_dec_stat(A_STE,1,1);
			new_dec_stat(A_STE,1,0);
			message_format(MSG_DRAIN_STAT, A_STE, "You feel very %s.", desc_stat_neg[A_STE]);
			message_flush();
			break;
		}
		
		case SV_POTION_EXCH_5:
		{

			*ident = TRUE;
			do_inc_stat(A_DEX);
			new_dec_stat(A_STR,1,1);
			new_dec_stat(A_STR,1,0);
			message_format(MSG_DRAIN_STAT, A_STR, "You feel very %s.", desc_stat_neg[A_STR]);
			message_flush();
			break;
		}
		
		case SV_POTION_EXCH_6:
		{

			*ident = TRUE;
			do_inc_stat(A_STR);
			new_dec_stat(A_INT,1,1);
			new_dec_stat(A_INT,1,0);
			message_format(MSG_DRAIN_STAT, A_INT, "You feel very %s.", desc_stat_neg[A_INT]);
			message_flush();
			break;
		}
		
		case SV_POTION_EXCH_7:
		{

			*ident = TRUE;
			do_inc_stat(A_AGI);
			new_dec_stat(A_PER,1,1);
			new_dec_stat(A_PER,1,0);
			message_format(MSG_DRAIN_STAT, A_PER, "You feel very %s.", desc_stat_neg[A_PER]);
			message_flush();
			break;
		}
		
		case SV_POTION_EXCH_8:
		{

			*ident = TRUE;
			do_inc_stat(A_INT);
			new_dec_stat(A_CON,1,1);
			new_dec_stat(A_CON,1,0);
			message_format(MSG_DRAIN_STAT, A_CON, "You feel very %s.", desc_stat_neg[A_CON]);
			message_flush();
			break;
		}
		
		case SV_POTION_EXCH_9:
		{

			*ident = TRUE;
			do_inc_stat(A_WIS);
			new_dec_stat(A_DEX,1,1);
			new_dec_stat(A_DEX,1,0);
			message_format(MSG_DRAIN_STAT, A_DEX, "You feel very %s.", desc_stat_neg[A_DEX]);
			message_flush();
			break;
		}
		
		case SV_POTION_RESTORING:
		{
			(void)do_res_stat(A_STR);
			(void)do_res_stat(A_INT);
			(void)do_res_stat(A_WIS);
			(void)do_res_stat(A_DEX);
			(void)do_res_stat(A_CON);
			(void)do_res_stat(A_AGI);
			(void)do_res_stat(A_STE);
			(void)do_res_stat(A_PER);
			(void)do_res_stat(A_LUC);
			*ident = TRUE;
			break;
		}

		case SV_POTION_RESIST_ACID:
		{
			if (set_oppose_acid(p_ptr->oppose_acid + randint(10) + 10))
			{
				*ident = TRUE;
			}
			break;
		}

		case SV_POTION_RESIST_ELECTRICITY:
		{
			if (set_oppose_elec(p_ptr->oppose_elec + randint(10) + 10))
			{
				*ident = TRUE;
			}
			break;
		}

		case SV_POTION_RESIST_POISON:
		{
			if (set_oppose_pois(p_ptr->oppose_pois + randint(15) + 15))
			{
				*ident = TRUE;
			}
			break;
		}

		case SV_POTION_RESISTANCE:
		{
			int act_time = randint(30) + 30;
			if(set_oppose_acid(p_ptr->oppose_acid + act_time)) *ident = TRUE;
			if(set_oppose_elec(p_ptr->oppose_elec + act_time)) *ident = TRUE;
			if(set_oppose_fire(p_ptr->oppose_fire + act_time)) *ident = TRUE;
			if(set_oppose_cold(p_ptr->oppose_cold + act_time)) *ident = TRUE;
			if(set_oppose_pois(p_ptr->oppose_pois + act_time)) *ident = TRUE;
			break;
		}

	}

	return (TRUE);
}


static bool read_scroll(object_type *o_ptr, bool *ident)
{
	int py = p_ptr->py;
	int px = p_ptr->px;

	int k;

	bool used_up = TRUE;


	/* Analyze the scroll */
	switch (o_ptr->sval)
	{
		case SV_SCROLL_DARKNESS:
		{
			if (!p_ptr->resist_blind)
			{
				(void)set_blind(p_ptr->blind + 3 + randint(5));
			}
			if (unlite_area(10, 3)) *ident = TRUE;
			break;
		}

		case SV_SCROLL_AGGRAVATE_MONSTER:
		{
			msg_print("There is a high pitched humming noise.");
			aggravate_monsters(SOURCE_PLAYER);
			*ident = TRUE;
			break;
		}

		case SV_SCROLL_CURSE_ARMOR:
		{
			if (curse_armor()) *ident = TRUE;
			break;
		}

		case SV_SCROLL_CURSE_WEAPON:
		{
			if (curse_weapon()) *ident = TRUE;
			break;
		}

		case SV_SCROLL_SUMMON_MONSTER:
		{
			sound(MSG_SUM_MONSTER);
			for (k = 0; k < randint(3); k++)
			{
				if (summon_specific(py, px, danger(p_ptr->depth), 0))
				{
					*ident = TRUE;
				}
			}
			break;
		}

		case SV_SCROLL_SUMMON_UNDEAD:
		{
			sound(MSG_SUM_UNDEAD);
			for (k = 0; k < randint(3); k++)
			{
				if (summon_specific(py, px, danger(p_ptr->depth), SUMMON_UNDEAD))
				{
					*ident = TRUE;
				}
			}
			break;
		}

		case SV_SCROLL_SUMMON_UNIQUE:
		{

			if (summon_specific(py, px, danger(p_ptr->depth), SUMMON_UNIQUE))
			{
				*ident = TRUE;
			}

			break;
		}

		case SV_SCROLL_TRAP_CREATION:
		{
			if (trap_creation(SOURCE_OTHER)) *ident = TRUE;
			break;
		}

		case SV_SCROLL_PHASE_DOOR:
		{
			teleport_player(10);
			*ident = TRUE;
			break;
		}

		case SV_SCROLL_TELEPORT:
		{
			msg_print("You feel magical forces starting to build up.");
			delayed_teleport_player(100);
			*ident = TRUE;
			break;
		}

		case SV_SCROLL_IDENTIFY:
		{
			*ident = TRUE;
			if (!ident_spell()) used_up = FALSE;
			break;
		}

		case SV_SCROLL_STAR_IDENTIFY:
		{
			*ident = TRUE;
			if (!identify_fully()) used_up = FALSE;
			break;
		}

		case SV_SCROLL_REMOVE_CURSE:
		{
			if (remove_curse(FALSE))
			{
				msg_print("You feel as if someone is watching over you.");
				*ident = TRUE;
			}
			break;
		}

		case SV_SCROLL_STAR_REMOVE_CURSE:
		{
			remove_curse(TRUE);
			*ident = TRUE;
			break;
		}

		case SV_SCROLL_ENCHANT_ARMOR:
		{
			*ident = TRUE;
			if (!enchant_spell(0, 0, 1)) used_up = FALSE;
			break;
		}

		case SV_SCROLL_ENCHANT_WEAPON_TO_HIT:
		{
			if (!enchant_spell(1, 0, 0)) used_up = FALSE;
			*ident = TRUE;
			break;
		}

		case SV_SCROLL_ENCHANT_WEAPON_TO_DAM:
		{
			if (!enchant_spell(0, 1, 0)) used_up = FALSE;
			*ident = TRUE;
			break;
		}

		case SV_SCROLL_STAR_ENCHANT_ARMOR:
		{
			if (!enchant_spell(0, 0, randint(3) + 2)) used_up = FALSE;
			*ident = TRUE;
			break;
		}

		case SV_SCROLL_STAR_ENCHANT_WEAPON:
		{
			if (!enchant_spell(randint(3), randint(3), 0)) used_up = FALSE;
			*ident = TRUE;
			break;
		}

		case SV_SCROLL_MASS_SLEEP:
		{
			if (sleep_monsters(30)) *ident = TRUE;
			break;
		}

		case SV_SCROLL_MASS_CONF:
		{
			if (project_los(p_ptr->py, p_ptr->px, 30, GF_OLD_CONF)) *ident = TRUE;
			break;
		}

		case SV_SCROLL_LIGHT:
		{
			if (lite_area(damroll(2, 8), 2)) *ident = TRUE;
			break;
		}

		case SV_SCROLL_MAPPING:
		{
			map_area();
			*ident = TRUE;
			break;
		}

		case SV_SCROLL_DETECT_GOLD:
		{
			if (detect_treasure()) *ident = TRUE;
			if (detect_objects_gold()) *ident = TRUE;
			break;
		}

		case SV_SCROLL_DETECT_ITEM:
		{
			if (detect_objects_normal()) *ident = TRUE;
			break;
		}

		case SV_SCROLL_DETECT_TRAP:
		{
			if (detect_traps()) *ident = TRUE;
			break;
		}

		case SV_SCROLL_DETECT_DOOR:
		{
			if (detect_doors()) *ident = TRUE;
			if (detect_stairs()) *ident = TRUE;
			break;
		}

		case SV_SCROLL_DETECT_INVIS:
		{
			if (detect_monsters_invis()) *ident = TRUE;
			break;
		}

		case SV_SCROLL_SATISFY_HUNGER:
		{
			if (set_food(PY_FOOD_MAX - 1)) *ident = TRUE;
			break;
		}

		case SV_SCROLL_BLESSING:
		{
			if (set_blessed(p_ptr->blessed + randint(12) + 6)) *ident = TRUE;
			break;
		}

		case SV_SCROLL_HOLY_CHANT:
		{
			if (set_blessed(p_ptr->blessed + randint(24) + 12)) *ident = TRUE;
			break;
		}

		case SV_SCROLL_HOLY_PRAYER:
		{
			if (set_blessed(p_ptr->blessed + randint(48) + 24)) *ident = TRUE;
			break;
		}

		case SV_SCROLL_MONSTER_CONFUSION:
		{
			if (p_ptr->confusing == 0)
			{
				msg_print("Your hands begin to glow.");
				p_ptr->confusing = TRUE;
				*ident = TRUE;
			}
			break;
		}

		case SV_SCROLL_PROTECTION_FROM_EVIL:
		{
			k = 3 * p_ptr->lev;
			if (set_protevil(p_ptr->protevil + randint(25) + k)) *ident = TRUE;
			break;
		}

		case SV_SCROLL_RUNE_OF_PROTECTION:
		{
			if (!warding_glyph()) used_up = FALSE;
			*ident = TRUE;
			break;
		}

		case SV_SCROLL_TRAP_DOOR_DESTRUCTION:
		{
			if (destroy_doors_touch()) *ident = TRUE;
			break;
		}

		case SV_SCROLL_STAR_DESTRUCTION:
		{
			destroy_area(py, px, 15, TRUE);
			*ident = TRUE;
			break;
		}

		case SV_SCROLL_CREATE_MONSTER_TRAP:
		{
			if (make_monster_trap()) *ident = TRUE;
			else used_up = FALSE;
			break;
		}

		case SV_SCROLL_DISPEL_UNDEAD:
		{
			if (dispel_undead(60)) *ident = TRUE;
			break;
		}

		case SV_SCROLL_CREATE_RANDART:
		{
			object_type *o_ptr;

			int item;

			/*artifact power is based on depth*/
			int randart_power = 10 + danger(p_ptr->depth);

			/* Get an item */
			cptr q = "Choose an item to be made into an artifact. ";
			cptr s = "You have no eligible item.";
			/* Only accept legal items. */
			item_tester_hook = item_tester_hook_randart;

			if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN))) break;

			/*Got the item*/
			o_ptr = &inventory[item];

			if ((adult_no_artifacts) || (adult_no_xtra_artifacts))
			{
				msg_print("Nothing happens.");
				break;
			}

			/*occasional power boost*/
			while (one_in_(25)) randart_power += 25;

			if (make_one_randart(o_ptr, randart_power, FALSE))
			{
				*ident = TRUE;
				used_up = TRUE;

				/* Identify it fully */
				object_aware(o_ptr);
				object_known(o_ptr);

				/* Mark the item as fully known */
				o_ptr->ident |= (IDENT_MENTAL);

				object_history(o_ptr, ORIGIN_ACQUIRE, 0);

				/*Let the player know what they just got*/
				object_info_screen(o_ptr);

			}
			else msg_print("The Artifact creation failed");
			break;
		}

		case SV_SCROLL_BANISHMENT:
		{
			if (!banishment()) used_up = FALSE;
			*ident = TRUE;
			break;
		}

		case SV_SCROLL_MASS_BANISHMENT:
		{
			(void)mass_banishment();
			*ident = TRUE;
			break;
		}

		case SV_SCROLL_ACQUIREMENT:
		{
			acquirement(py, px, 1, TRUE);
			*ident = TRUE;
			break;
		}

	}

	return (used_up);
}


static bool use_staff(object_type *o_ptr, bool *ident)
{
	int py = p_ptr->py;
	int px = p_ptr->px;

	int k;

	bool use_charge = TRUE;

	/* Analyze the staff 
	switch (o_ptr->sval)
	{
		case SV_STAFF_DARKNESS:
		{
			if (!p_ptr->resist_blind)
			{
				if (set_blind(p_ptr->blind + 3 + randint(5))) *ident = TRUE;
			}
			if (unlite_area(10, 3)) *ident = TRUE;
			break;
		}

		case SV_STAFF_SLOWNESS:
		{
			if (set_slow(p_ptr->slow + randint(30) + 15)) *ident = TRUE;
			break;
		}

		case SV_STAFF_HASTE_MONSTERS:
		{
			if (speed_monsters()) *ident = TRUE;
			break;
		}

		case SV_STAFF_SUMMONING:
		{
			sound(MSG_SUM_MONSTER);
			for (k = 0; k < randint(4); k++)
			{
				if (summon_specific(py, px, danger(p_ptr->depth), 0))
				{
					*ident = TRUE;
				}
			}
			break;
		}

		case SV_STAFF_TELEPORTATION:
		{
			msg_print("You feel magical forces starting to build up.");
			delayed_teleport_player(100);
			*ident = TRUE;
			break;
		}

		case SV_STAFF_IDENTIFY:
		{
			if (!ident_spell()) use_charge = FALSE;
			*ident = TRUE;
			break;
		}

		case SV_STAFF_STARLITE:
		{
			if (!p_ptr->blind)
			{
				msg_print("The end of the staff glows brightly...");
			}
			for (k = 0; k < 8; k++) lite_line(ddd[k], damroll(6, 8));
			*ident = TRUE;
			break;
		}

		case SV_STAFF_LITE:
		{
			if (lite_area(damroll(2, 8), 2)) *ident = TRUE;
			break;
		}

		case SV_STAFF_MAPPING:
		{
			map_area();
			*ident = TRUE;
			break;
		}

		case SV_STAFF_DETECT_GOLD:
		{
			if (detect_treasure()) *ident = TRUE;
			if (detect_objects_gold()) *ident = TRUE;
			break;
		}

		case SV_STAFF_DETECT_ITEM:
		{
			if (detect_objects_normal()) *ident = TRUE;
			break;
		}

		case SV_STAFF_DETECT_TRAP:
		{
			if (detect_traps()) *ident = TRUE;
			break;
		}

		case SV_STAFF_DETECT_DOOR:
		{
			if (detect_doors()) *ident = TRUE;
			if (detect_stairs()) *ident = TRUE;
			break;
		}

		case SV_STAFF_DETECT_INVIS:
		{
			if (detect_monsters_invis()) *ident = TRUE;
			break;
		}

		case SV_STAFF_DETECT_EVIL:
		{
			if (detect_monsters_evil()) *ident = TRUE;
			break;
		}

		case SV_STAFF_CURE_LIGHT:
		{
			if (hp_player(damroll(2, 10))) *ident = TRUE;
			break;
		}

		case SV_STAFF_CURING:
		{
			if (set_blind(0)) *ident = TRUE;
			if (set_poisoned(0)) *ident = TRUE;
			if (set_confused(0)) *ident = TRUE;
			if (set_stun(0)) *ident = TRUE;
			if (set_cut(0)) *ident = TRUE;
			break;
		}

		case SV_STAFF_HEALING:
		{
			if (hp_player(325)) *ident = TRUE;
			if (set_stun(0)) *ident = TRUE;
			if (set_cut(0)) *ident = TRUE;
			break;
		}

		case SV_STAFF_THE_MAGI:
		{
			if (do_res_stat(A_INT)) *ident = TRUE;
			if (p_ptr->csp < p_ptr->msp)
			{
				p_ptr->csp = p_ptr->msp;
				p_ptr->csp_frac = 0;
				*ident = TRUE;
				msg_print("Your feel your head clear.");
				p_ptr->redraw |= (PR_MANA);
				p_ptr->window |= (PW_PLAYER_0 | PW_PLAYER_1);
			}
			break;
		}

		case SV_STAFF_SLEEP_MONSTERS:
		{
			if (sleep_monsters(damroll(2, p_ptr->lev))) *ident = TRUE;
			break;
		}

		case SV_STAFF_SLOW_MONSTERS:
		{
			if (slow_monsters(damroll (2, p_ptr->lev))) *ident = TRUE;
			break;
		}

		case SV_STAFF_SPEED:
		{
			if (!p_ptr->fast)
			{
				if (set_fast(randint(30) + 15)) *ident = TRUE;
			}
			else
			{
				(void)set_fast(p_ptr->fast + 5);
			}
			break;
		}

		case SV_STAFF_PROBING:
		{
			probing();
			*ident = TRUE;
			break;
		}

		case SV_STAFF_DISPEL_EVIL:
		{
			if (dispel_evil(60)) *ident = TRUE;
			break;
		}

		case SV_STAFF_POWER:
		{
			if (dispel_monsters(120)) *ident = TRUE;
			break;
		}

		case SV_STAFF_HOLINESS:
		{
			if (dispel_evil(120)) *ident = TRUE;
			k = 3 * p_ptr->lev;
			if (set_protevil(p_ptr->protevil + randint(25) + k)) *ident = TRUE;
			if (set_poisoned(0)) *ident = TRUE;
			if (set_afraid(0)) *ident = TRUE;
			if (hp_player(75)) *ident = TRUE;
			if (set_stun(0)) *ident = TRUE;
			if (set_cut(0)) *ident = TRUE;
			break;
		}

		case SV_STAFF_BANISHMENT:
		{
			if (!banishment()) use_charge = FALSE;
			*ident = TRUE;
			break;
		}

		case SV_STAFF_EARTHQUAKES:
		{
			earthquake(py, px, 10);
			*ident = TRUE;
			break;
		}

		case SV_STAFF_DESTRUCTION:
		{
			destroy_area(py, px, 15, TRUE);
			*ident = TRUE;
			break;
		}

		case SV_STAFF_MASS_IDENTIFY:
		{
		  	mass_identify(3);
			*ident = TRUE;
		  	break;
		}

	} */

	return (use_charge);
}


static bool aim_wand(object_type *o_ptr, bool *ident)
{
	int lev, dir, sval, mana_cost, chance;

	/* Take a turn */
	p_ptr->p_energy_use = BASE_ENERGY_MOVE;

	/* Not identified yet */
	*ident = FALSE;

	sval = o_ptr->sval;

	mana_cost = get_mana_cost(sval,1); 
	chance = get_success_prob(sval,1);

	/* Disallow "dangerous" spells */
	if (mana_cost > p_ptr->csp || p_ptr->csp<=0)
	{
		/* Warning */
		msg_print("You do not have enough mana to zap this wand.");

		/* Flush input */
		flush();

		return FALSE;
	}

	/* Failed spell */
	if (randint(100) > chance)
	{
		if (flush_failure) flush();
		msg_print("You failed to get the spell off!");
	}

	/* Process spell */
	else
	{
		/* Cast the spell */
		sound(MSG_SPELL);
		if (do_mage_spell(MODE_SPELL_CAST, sval) == NULL) return FALSE;

	}

	/* Use some mana */
	if (mana_cost >= 1){
		p_ptr->csp -= mana_cost;
	} else if (mana_cost == 0){
		p_ptr->csp -= one_in_(2);
	} else if (mana_cost == -1){
		p_ptr->csp -= one_in_(4);
	} else if (mana_cost == -2){
		p_ptr->csp -= one_in_(8);
	} else {
		p_ptr->csp -= one_in_(16);
	}

	/* Redraw mana */
	p_ptr->redraw |= (PR_MANA);
	
	/* Take a turn */
	p_ptr->p_energy_use = BASE_ENERGY_MOVE;

	/* Window stuff */
	p_ptr->window |= (PW_PLAYER_0 | PW_PLAYER_1);
	
	/* Analyze the wand 
	switch (sval)
	{ 
		case SV_WAND_HEAL_MONSTER:
		{
			if (heal_monster(dir, damroll(4, 6))) *ident = TRUE;
			break;
		}

		case SV_WAND_HASTE_MONSTER:
		{
			if (speed_monster(dir)) *ident = TRUE;
			break;
		}

		case SV_WAND_CLONE_MONSTER:
		{
			if (clone_monster(dir)) *ident = TRUE;
			break;
		}

		case SV_WAND_TELEPORT_AWAY:
		{
			if (teleport_monster(dir)) *ident = TRUE;
			break;
		}

		case SV_WAND_DISARMING:
		{
			if (disarm_trap(dir)) *ident = TRUE;
			break;
		}

		case SV_WAND_TRAP_DOOR_DEST:
		{
			if (destroy_door(dir)) *ident = TRUE;
			break;
		}

		case SV_WAND_STONE_TO_MUD:
		{
			if (wall_to_mud(dir, 20 + randint(30))) *ident = TRUE;
			break;
		}

		case SV_WAND_LITE:
		{
			msg_print("A line of blue shimmering light appears.");
			lite_line(dir, damroll(6, 8));
			*ident = TRUE;
			break;
		}

		case SV_WAND_SLEEP_MONSTER:
		{
			if (sleep_monster(dir)) *ident = TRUE;
			break;
		}

		case SV_WAND_SLOW_MONSTER:
		{
			if (slow_monster(dir)) *ident = TRUE;
			break;
		}

		case SV_WAND_CONFUSE_MONSTER:
		{
			if (confuse_monster(dir, (p_ptr->lev * 2 / 3))) *ident = TRUE;
			break;
		}

		case SV_WAND_FEAR_MONSTER:
		{
			if (fear_monster(dir, 10)) *ident = TRUE;
			break;
		}

		case SV_WAND_DRAIN_LIFE:
		{
			if (drain_life(dir, 150)) *ident = TRUE;
			break;
		}

		case SV_WAND_POLYMORPH:
		{
			if (poly_monster(dir)) *ident = TRUE;
			break;
		}

		case SV_WAND_STINKING_CLOUD:
		{
			fire_ball(GF_POIS, dir, 12, 2);
			*ident = TRUE;
			break;
		}

		case SV_WAND_MAGIC_MISSILE:
		{
			fire_bolt_or_beam(20, GF_MISSILE, dir, damroll(3, 4));
			*ident = TRUE;
			break;
		}

		case SV_WAND_ACID_BOLT:
		{
			fire_bolt_or_beam(20, GF_ACID, dir, damroll(10, 8));
			*ident = TRUE;
			break;
		}

		case SV_WAND_ELEC_BOLT:
		{
			fire_bolt_or_beam(20, GF_ELEC, dir, damroll(6, 6));
			*ident = TRUE;
			break;
		}

		case SV_WAND_FIRE_BOLT:
		{
			fire_bolt_or_beam(20, GF_FIRE, dir, damroll(12, 8));
			*ident = TRUE;
			break;
		}

		case SV_WAND_COLD_BOLT:
		{
			fire_bolt_or_beam(20, GF_COLD, dir, damroll(6, 8));
			*ident = TRUE;
			break;
		}

		case SV_WAND_ACID_BALL:
		{
			fire_ball(GF_ACID, dir, 120, 2);
			*ident = TRUE;
			break;
		}

		case SV_WAND_ELEC_BALL:
		{
			fire_ball(GF_ELEC, dir, 64, 2);
			*ident = TRUE;
			break;
		}

		case SV_WAND_FIRE_BALL:
		{
			fire_ball(GF_FIRE, dir, 144, 2);
			*ident = TRUE;
			break;
		}

		case SV_WAND_COLD_BALL:
		{
			fire_ball(GF_COLD, dir, 96, 2);
			*ident = TRUE;
			break;
		}

		case SV_WAND_WONDER:
		{
			msg_print("Oops.  Wand of wonder activated.");
			break;
		}

		case SV_WAND_DRAGON_FIRE:
		{
			fire_ball(GF_FIRE, dir, 200, 3);
			*ident = TRUE;
			break;
		}

		case SV_WAND_DRAGON_COLD:
		{
			fire_ball(GF_COLD, dir, 160, 3);
			*ident = TRUE;
			break;
		}

		case SV_WAND_DRAGON_BREATH:
		{
			switch (randint(5))
			{
				case 1:
				{
					fire_ball(GF_ACID, dir, 200, 3);
					break;
				}

				case 2:
				{
					fire_ball(GF_ELEC, dir, 160, 3);
					break;
				}

				case 3:
				{
					fire_ball(GF_FIRE, dir, 200, 3);
					break;
				}

				case 4:
				{
					fire_ball(GF_COLD, dir, 160, 3);
					break;
				}

				default:
				{
					fire_ball(GF_POIS, dir, 120, 3);
					break;
				}
			}

			*ident = TRUE;
			break;
		}

		case SV_WAND_ANNIHILATION:
		{
			if (drain_life(dir, 250)) *ident = TRUE;
			break;
		}
	} */

	return (TRUE);
}


static bool zap_rod(object_type *o_ptr, bool *ident)
{
	int chance, dir, lev;
	bool used_charge = TRUE;
	object_kind *k_ptr = &k_info[o_ptr->k_idx];

	/* Take a turn */
	p_ptr->p_energy_use = BASE_ENERGY_MOVE;

	/* Not identified yet */
	*ident = FALSE;

	/* Extract the item level */
	lev = k_info[o_ptr->k_idx].k_level;

	/* Base chance of success */
	chance = p_ptr->skill_dev;

	/* Confusion hurts skill */
	if (p_ptr->confused) chance = chance / 2;

	/* High level objects are harder */
	chance = chance - ((lev > 50) ? 50 : lev);

	/* Give everyone a (slight) chance */
	if ((chance < USE_DEVICE) && (rand_int(USE_DEVICE - chance + 1) == 0))
	{
		chance = USE_DEVICE;
	}

	/* Roll for usage */
	if ((chance < USE_DEVICE) || (randint(chance) < USE_DEVICE))
	{
		if (flush_failure) flush();
		msg_print("You failed to use the rod properly.");
		return FALSE;
	}

	/* Still charging? */
	if (o_ptr->timeout > (o_ptr->pval - k_ptr->pval))
	{

		if (o_ptr->number == 1)
			msg_print("The rod is still charging");
		else
			msg_print("The rods are all still charging");

 		return FALSE;
	}

	/* Sound */
	sound(MSG_ZAP_ROD);

	/* Analyze the rod 
	switch (o_ptr->sval)
	{
		case SV_ROD_DETECT_TRAP:
		{
			if (detect_traps()) *ident = TRUE;
			break;
		}

		case SV_ROD_DETECT_DOOR:
		{
			if (detect_doors()) *ident = TRUE;
			if (detect_stairs()) *ident = TRUE;
			break;
		}

		case SV_ROD_IDENTIFY:
		{
			*ident = TRUE;
			if (!ident_spell()) used_charge = FALSE;
			break;
		}

		case SV_ROD_ILLUMINATION:
		{
			if (lite_area(damroll(2, 8), 2)) *ident = TRUE;
			break;
		}

		case SV_ROD_MAPPING:
		{
			map_area();
			*ident = TRUE;
			break;
		}

		case SV_ROD_DETECTION:
		{
			detect_all();
			*ident = TRUE;
			break;
		}

		case SV_ROD_PROBING:
		{
			probing();
			*ident = TRUE;
			break;
		}

		case SV_ROD_CURING:
		{
			if (set_blind(0)) *ident = TRUE;
			if (set_poisoned(0)) *ident = TRUE;
			if (set_confused(0)) *ident = TRUE;
			if (set_stun(0)) *ident = TRUE;
			if (set_cut(0)) *ident = TRUE;
			break;
		}

		case SV_ROD_HEALING:
		{
			if (hp_player(550)) *ident = TRUE;
			if (set_stun(0)) *ident = TRUE;
			if (set_cut(0)) *ident = TRUE;
			break;
		}

		case SV_ROD_RESTORATION:
		{
			if (restore_level()) *ident = TRUE;
			if (do_res_stat(A_STR)) *ident = TRUE;
			if (do_res_stat(A_INT)) *ident = TRUE;
			if (do_res_stat(A_WIS)) *ident = TRUE;
			if (do_res_stat(A_DEX)) *ident = TRUE;
			if (do_res_stat(A_CON)) *ident = TRUE;
			if (do_res_stat(A_AGI)) *ident = TRUE;
			if (do_res_stat(A_STE)) *ident = TRUE;
			if (do_res_stat(A_PER)) *ident = TRUE;
			if (do_res_stat(A_LUC)) *ident = TRUE;
			break;
		}

		case SV_ROD_SPEED:
		{
			if (!p_ptr->fast)
			{
				if (set_fast(randint(30) + 15)) *ident = TRUE;
			}
			else
			{
				(void)set_fast(p_ptr->fast + 5);
			}
			break;
		}

		case SV_ROD_STONE_TO_MUD:
		{
			if (wall_to_mud(dir, 20 + randint(30))) *ident = TRUE;
			break;
		}

		case SV_ROD_TELEPORT_AWAY:
		{
			if (teleport_monster(dir)) *ident = TRUE;
			break;
		}

		case SV_ROD_DISARMING:
		{
			if (disarm_trap(dir)) *ident = TRUE;
			break;
		}

		case SV_ROD_LITE:
		{
			msg_print("A line of blue shimmering light appears.");
			lite_line(dir, damroll(6, 8));
			*ident = TRUE;
			break;
		}

		case SV_ROD_SLEEP_MONSTER:
		{
			if (sleep_monster(dir)) *ident = TRUE;
			break;
		}

		case SV_ROD_SLOW_MONSTER:
		{
			if (slow_monster(dir)) *ident = TRUE;
			break;
		}

		case SV_ROD_DRAIN_LIFE:
		{
			if (drain_life(dir, 150)) *ident = TRUE;
			break;
		}

		case SV_ROD_POLYMORPH:
		{
			if (poly_monster(dir)) *ident = TRUE;
			break;
		}

		case SV_ROD_ACID_BOLT:
		{
			fire_bolt_or_beam(10, GF_ACID, dir, damroll(12, 8));
			*ident = TRUE;
			break;
		}

		case SV_ROD_ELEC_BOLT:
		{
			fire_bolt_or_beam(10, GF_ELEC, dir, damroll(6, 6));
			*ident = TRUE;
			break;
		}

		case SV_ROD_FIRE_BOLT:
		{
			fire_bolt_or_beam(10, GF_FIRE, dir, damroll(16, 8));
			*ident = TRUE;
			break;
		}

		case SV_ROD_COLD_BOLT:
		{
			fire_bolt_or_beam(10, GF_COLD, dir, damroll(10, 8));
			*ident = TRUE;
			break;
		}

		case SV_ROD_ACID_BALL:
		{
			fire_ball(GF_ACID, dir, 120, 2);
			*ident = TRUE;
			break;
		}

		case SV_ROD_ELEC_BALL:
		{
			fire_ball(GF_ELEC, dir, 64, 2);
			*ident = TRUE;
			break;
		}

		case SV_ROD_FIRE_BALL:
		{
			fire_ball(GF_FIRE, dir, 144, 2);
			*ident = TRUE;
			break;
		}

		case SV_ROD_COLD_BALL:
		{
			fire_ball(GF_COLD, dir, 96, 2);
			*ident = TRUE;
			break;
		}
		case SV_ROD_STAR_IDENTIFY:
		{
			if (!identify_fully()) used_charge = FALSE;
			*ident = TRUE;
			break;
		}
		case SV_ROD_MASS_IDENTIFY:
		{
			mass_identify(3);
			*ident = TRUE;
			break;
		}
	} */

	/* Drain the charge */
	if (used_charge) o_ptr->timeout += k_ptr->pval;

	return TRUE;
}


/*
 * Activate a wielded object.  Wielded objects never stack.
 * And even if they did, activatable objects never stack.
 *
 * Currently, only (some) artifacts, and Dragon Scale Mail, can be activated.
 * But one could, for example, easily make an activatable "Ring of Plasma".
 *
 * Note that it always takes a turn to activate an artifact, even if
 * the user hits "escape" at the "direction" prompt.
 */
static bool activate_object(object_type *o_ptr)
{
	int k, dir, i, chance;


	/* Check the recharge */
	if (o_ptr->timeout)
	{
		msg_print("It whines, glows and fades...");
		return FALSE;
	}

	/* Activate the artifact */
	message(MSG_ACT_ARTIFACT, 0, "You activate it...");

	/* Artifacts, except for special artifacts with dragon scale mail*/
	if ((o_ptr->art_num) && (o_ptr->art_num < z_info->art_norm_max))
	{
		bool did_activation = TRUE;

		artifact_type *a_ptr = &a_info[o_ptr->art_num];
		char o_name[80];

		/* Get the basic name of the object */
		object_desc(o_name, sizeof(o_name), o_ptr, FALSE, 0);

		switch (a_ptr->activation)
		{
			case ACT_ILLUMINATION:
			{
				msg_format("The %s wells with clear light...", o_name);
				lite_area(damroll(2, 15), 3);
				break;
			}

			case ACT_MAGIC_MAP:
			{
				msg_format("The %s shines brightly...", o_name);
				map_area();
				break;
			}

			case ACT_CLAIRVOYANCE:
			{
				msg_format("The %s glows a deep green...", o_name);
				wiz_lite();
				(void)detect_traps();
				(void)detect_doors();
				(void)detect_stairs();
				break;
			}

			case ACT_PROT_EVIL:
			{
				msg_format("The %s lets out a shrill wail...", o_name);
				k = 3 * p_ptr->lev;
				(void)set_protevil(p_ptr->protevil + randint(25) + k);
				break;
			}

			case ACT_DISP_EVIL:
			{
				msg_format("The %s floods the area with goodness...", o_name);
				dispel_evil(p_ptr->lev * 5);
				break;
			}

			case ACT_HASTE2:
			{
				msg_format("The %s glows brightly...", o_name);
				if (!p_ptr->fast)
				{
					(void)set_fast(randint(75) + 75);
				}
				else
				{
					(void)set_fast(p_ptr->fast + 5);
				}
				break;
			}

			case ACT_FIRE3:
			{
				msg_format("The %s glows deep red...", o_name);
				if (!get_aim_dir(&dir, FALSE)) return FALSE;
				fire_ball(GF_FIRE, dir, 120, 3);
				break;
			}

			case ACT_FROST5:
			{
				msg_format("The %s glows bright white...", o_name);
				if (!get_aim_dir(&dir, FALSE)) return FALSE;
				fire_ball(GF_COLD, dir, 200, 3);
				break;
			}

			case ACT_ELEC2:
			{
				msg_format("The %s glows deep blue...", o_name);
				if (!get_aim_dir(&dir, FALSE)) return FALSE;
				fire_ball(GF_ELEC, dir, 250, 3);
				break;
			}

			case ACT_BIZZARE:
			{
				msg_format("The %s glows intensely black...", o_name);
				if (!get_aim_dir(&dir, FALSE)) return FALSE;
				ring_of_power(dir);
				break;
			}


			case ACT_STAR_BALL:
			{
				msg_format("Your %s is surrounded by lightning...", o_name);
				for (i = 0; i < 8; i++) fire_ball(GF_ELEC, ddd[i], 150, 3);
				break;
			}

			case ACT_RAGE_BLESS_RESIST:
			{
				int act_time = randint(50) + 50;
				msg_format("Your %s glows many colours...", o_name);
				(void)hp_player(30);
				(void)set_afraid(0);
				(void)set_shero(p_ptr->shero + randint(50) + 50);
				(void)set_blessed(p_ptr->blessed + randint(50) + 50);
				(void)set_oppose_acid(p_ptr->oppose_acid + act_time);
				(void)set_oppose_elec(p_ptr->oppose_elec + act_time);
				(void)set_oppose_fire(p_ptr->oppose_fire + act_time);
				(void)set_oppose_cold(p_ptr->oppose_cold + act_time);
				(void)set_oppose_pois(p_ptr->oppose_pois + act_time);
				break;
			}

			case ACT_HEAL2:
			{
				msg_format("Your %s glows a bright white...", o_name);
				msg_print("You feel much better...");
				(void)hp_player(1000);
				(void)set_cut(0);
				break;
			}

			case ACT_PHASE:
			{
				msg_format("Your %s twists space around you...", o_name);
				teleport_player(10);
				break;
			}

			case ACT_BANISHMENT:
			{
				msg_format("Your %s glows deep blue...", o_name);
				if (!banishment()) return FALSE;
				break;
			}

			case ACT_TRAP_DOOR_DEST:
			{
				msg_format("Your %s glows bright red...", o_name);
				destroy_doors_touch();
				break;
			}

			case ACT_DETECT:
			{
				msg_format("Your %s glows bright white...", o_name);
				msg_print("An image forms in your mind...");
				detect_all();
				break;
			}

			case ACT_HEAL1:
			{
				msg_format("Your %s glows deep blue...", o_name);
				msg_print("You feel a warm tingling inside...");
				(void)hp_player(500);
				(void)set_cut(0);
				break;
			}

			case ACT_RESIST:
			{
				int act_time = randint(20) + 20;
				msg_format("Your %s glows many colours...", o_name);
				(void)set_oppose_acid(p_ptr->oppose_acid + act_time);
				(void)set_oppose_elec(p_ptr->oppose_elec + act_time);
				(void)set_oppose_fire(p_ptr->oppose_fire + act_time);
				(void)set_oppose_cold(p_ptr->oppose_cold + act_time);
				break;
			}

			case ACT_SLEEP:
			{
				msg_format("Your %s glows deep blue...", o_name);
				sleep_monsters_touch();
				break;
			}

			case ACT_RECHARGE1:
			{
				break;
			}

			case ACT_TELEPORT:
			{
				msg_format("Your %s starts flashing...", o_name);
				delayed_teleport_player(100);
				break;
			}

			case ACT_RESTORE_LIFE:
			{
				msg_format("Your %s glows a deep red...", o_name);
				restore_level();
				break;
			}

			case ACT_MISSILE:
			{
				msg_format("Your %s glows extremely brightly...", o_name);
				if (!get_aim_dir(&dir, FALSE)) return FALSE;
				fire_bolt(GF_MISSILE, dir, damroll(2, 6));
				break;
			}

			case ACT_FIRE1:
			{
				msg_format("Your %s is covered in fire...", o_name);
				if (!get_aim_dir(&dir, FALSE)) return FALSE;
				fire_bolt(GF_FIRE, dir, damroll(9, 8));
				break;
			}

			case ACT_FROST1:
			{
				msg_format("Your %s is covered in frost...", o_name);
				if (!get_aim_dir(&dir, FALSE)) return FALSE;
				fire_bolt(GF_COLD, dir, damroll(6, 8));
				break;
			}

			case ACT_LIGHTNING_BOLT:
			{
				msg_format("Your %s is covered in sparks...", o_name);
				if (!get_aim_dir(&dir, FALSE)) return FALSE;
				fire_bolt(GF_ELEC, dir, damroll(4, 8));
				break;
			}

			case ACT_ACID1:
			{
				msg_format("Your %s is covered in acid...", o_name);
				if (!get_aim_dir(&dir, FALSE)) return FALSE;
				fire_bolt(GF_ACID, dir, damroll(5, 8));
				break;
			}

			case ACT_ARROW:
			{
				msg_format("Your %s grows magical spikes...", o_name);
				if (!get_aim_dir(&dir, FALSE)) return FALSE;
				fire_bolt(GF_ARROW, dir, 150);
				break;
			}

			case ACT_HASTE1:
			{
				msg_format("Your %s glows bright green...", o_name);
				if (!p_ptr->fast)
				{
					(void)set_fast(randint(20) + 20);
				}
				else
				{
					(void)set_fast(p_ptr->fast + 5);
				}
				break;
			}

			case ACT_REM_FEAR_POIS:
			{
				msg_format("Your %s glows deep blue...", o_name);
				(void)set_afraid(0);
				(void)set_poisoned(0);
				break;
			}

			case ACT_STINKING_CLOUD:
			{
				msg_format("Your %s throbs deep green...", o_name);
				if (!get_aim_dir(&dir, FALSE)) return FALSE;
				fire_ball(GF_POIS, dir, 12, 3);
				break;
			}

			case ACT_FROST2:
			{
				msg_format("Your %s is covered in frost...", o_name);
				if (!get_aim_dir(&dir, FALSE)) return FALSE;
				fire_ball(GF_COLD, dir, 48, 2);
				break;
			}

			case ACT_FROST4:
			{
				msg_format("Your %s glows a pale blue...", o_name);
				if (!get_aim_dir(&dir, FALSE)) return FALSE;
				fire_bolt(GF_COLD, dir, damroll(12, 8));
				break;
			}

			case ACT_FROST3:
			{
				msg_format("Your %s glows a intense blue...", o_name);
				if (!get_aim_dir(&dir, FALSE)) return FALSE;
				fire_ball(GF_COLD, dir, 100, 2);
				break;
			}

			case ACT_FIRE2:
			{
				msg_format("Your %s rages in fire...", o_name);
				if (!get_aim_dir(&dir, FALSE)) return FALSE;
				fire_ball(GF_FIRE, dir, 72, 2);
				break;
			}

			case ACT_DRAIN_LIFE2:
			{
				msg_format("Your %s glows black...", o_name);
				if (!get_aim_dir(&dir, FALSE)) return FALSE;
				drain_life(dir, 120);
				break;
			}

			case ACT_STONE_TO_MUD:
			{
				msg_format("Your %s pulsates...", o_name);
				if (!get_aim_dir(&dir, FALSE)) return FALSE;
				(void)wall_to_mud(dir, 20 + randint(30));
				break;
			}

			case ACT_MASS_BANISHMENT:
			{
				msg_format("Your %s lets out a long, shrill note...", o_name);
				(void)mass_banishment();
				break;
			}

			case ACT_CURE_WOUNDS:
			{
				msg_format("Your %s radiates deep purple...", o_name);
				hp_player(damroll(8, 10));
				(void)set_cut((p_ptr->cut / 2) - 50);
				break;
			}

			case ACT_TELE_AWAY:
			{
				msg_format("Your %s glows deep red...", o_name);
				if (!get_aim_dir(&dir, FALSE)) return FALSE;
				teleport_monster(dir);
				break;
			}

			case ACT_WOR:
			{
				msg_format("Your %s glows soft white...", o_name);
				set_recall();
				break;
			}

			case ACT_CONFUSE:
			{
				msg_format("Your %s glows in scintillating colours...", o_name);
				if (!get_aim_dir(&dir, FALSE)) return FALSE;
				confuse_monster(dir, (p_ptr->lev * 2 / 3));
				break;
			}

			case ACT_IDENTIFY:
			{
				msg_format("Your %s glows yellow...", o_name);
				if (!identify_fully()) return FALSE;
				break;
			}

			case ACT_PROBE:
			{
				msg_format("Your %s glows brightly...", o_name);
				probing();
				break;
			}

			case ACT_DRAIN_LIFE1:
			{
				msg_format("Your %s glows white...", o_name);
				if (!get_aim_dir(&dir, FALSE)) return FALSE;
				drain_life(dir, 90);
				break;
			}

			case ACT_FIREBRAND:
			{
				msg_format("Your %s glows deep red...", o_name);
				if (!brand_bolts(TRUE)) return FALSE;
				break;
			}

			case ACT_STARLIGHT:
			{
				msg_format("Your %s glows with the light of a thousand stars...", o_name);
				for (k = 0; k < 8; k++) strong_lite_line(ddd[k]);
				break;
			}

			case ACT_MANA_BOLT:
			{
				msg_format("Your %s glows white...", o_name);
				if (!get_aim_dir(&dir, FALSE)) return FALSE;
				fire_bolt(GF_MANA, dir, damroll(12, 8));
				break;
			}

			case ACT_BERSERKER:
			{
				msg_format("Your %s glows in anger...", o_name);
				set_shero(p_ptr->shero + randint(50) + 50);
				break;
			}
			case ACT_RES_ACID:
			{
				msg_format("Your %s glows light gray...", o_name);
				(void)set_oppose_acid(p_ptr->oppose_acid + randint(20) + 20);
				break;
			}
			case ACT_RES_ELEC:
			{
				msg_format("Your %s glows light blue...", o_name);
				(void)set_oppose_elec(p_ptr->oppose_elec + randint(20) + 20);
				break;
			}
			case ACT_RES_FIRE:
			{
				msg_format("Your %s glows light red...", o_name);
				(void)set_oppose_fire(p_ptr->oppose_fire + randint(20) + 20);
				break;
			}
			case ACT_RES_COLD:
			{
				msg_format("Your %s glows bright white...", o_name);
				(void)set_oppose_cold(p_ptr->oppose_cold + randint(20) + 20);
				break;
			}
			case ACT_RES_POIS:
			{
				msg_format("Your %s glows light green...", o_name);
				(void)set_oppose_cold(p_ptr->oppose_elec + randint(20) + 20);
				break;
			}

			default:
		   	{
				if ((a_ptr->tval != TV_DRAG_ARMOR) &&
					(a_ptr->tval != TV_DRAG_SHIELD)) return (FALSE);
				else did_activation = FALSE;
			}
		}

		if (did_activation)
		{

			/* Set the recharge time */
			if (a_ptr->randtime)
				o_ptr->timeout = a_ptr->time + (byte)randint(a_ptr->randtime);
			else o_ptr->timeout = a_ptr->time;

			/* Window stuff */
			p_ptr->window |= (PW_INVEN | PW_EQUIP);

			/* Done */
			return FALSE;
		}
	}


	/* Hack -- Dragon Scale Mail can be activated as well */
	if ((o_ptr->tval == TV_DRAG_ARMOR) ||
		(o_ptr->tval == TV_DRAG_SHIELD))
	{

		u16b value = o_ptr->sval;
		int time_2 = randint(10) + 10;

		if (o_ptr->tval == TV_DRAG_ARMOR) value *= 2;

		/* Get a direction for breathing (or abort) */
		if (!get_aim_dir(&dir, FALSE)) return FALSE;

		/* Branch on the sub-type */
		switch (o_ptr->ego_num)
		{
			case EGO_DRAGON_BLUE:
			{
				value *= 50;

				sound(MSG_BR_ELEC);
				msg_print("You breathe lightning.");
				fire_arc(GF_ELEC, dir, value, 0, 30);

				set_oppose_elec(p_ptr->oppose_elec + time_2);

				o_ptr->timeout = rand_int(value) + (value);
				break;
			}

			case EGO_DRAGON_WHITE:
			{
				value *= 50;

				sound(MSG_BR_FROST);
				msg_print("You breathe frost.");
				fire_arc(GF_COLD, dir, value, 0, 30);

				set_oppose_cold(p_ptr->oppose_cold + time_2);

				o_ptr->timeout = rand_int(value) + (value);
				break;
			}

			case EGO_DRAGON_BLACK:
			{
				value *= 50;

				sound(MSG_BR_ACID);
				msg_print("You breathe acid.");
				fire_arc(GF_ACID, dir, value, 0, 30);

				set_oppose_acid(p_ptr->oppose_acid + time_2);

				o_ptr->timeout = rand_int(value) + value;
				break;
			}

			case EGO_DRAGON_GREEN:
			{
				value *= 50;

				sound(MSG_BR_GAS);
				msg_print("You breathe poison gas.");
				fire_arc(GF_POIS, dir, value, 0, 30);

				set_oppose_pois(p_ptr->oppose_pois + time_2);

				o_ptr->timeout = rand_int(value) + value;
				break;
			}

			case EGO_DRAGON_RED:
			{
				value *= 50;

				sound(MSG_BR_FIRE);
				msg_print("You breathe fire.");
				fire_arc(GF_FIRE, dir, value, 0, 30);

				set_oppose_fire(p_ptr->oppose_fire + time_2);

				o_ptr->timeout = rand_int(value) + value;
				break;
			}

			case EGO_DRAGON_MULTIHUED:
			{
				value *= 75;

				chance = rand_int(5);
				sound(((chance == 1) ? MSG_BR_ELEC :
				       ((chance == 2) ? MSG_BR_FROST :
				        ((chance == 3) ? MSG_BR_ACID :
				         ((chance == 4) ? MSG_BR_GAS : MSG_BR_FIRE)))));
				msg_format("You breathe %s.",
				           ((chance == 1) ? "lightning" :
				            ((chance == 2) ? "frost" :
				             ((chance == 3) ? "acid" :
				              ((chance == 4) ? "poison gas" : "fire")))));
				fire_arc(((chance == 1) ? GF_ELEC :
				           ((chance == 2) ? GF_COLD :
				            ((chance == 3) ? GF_ACID :
				             ((chance == 4) ? GF_POIS : GF_FIRE)))),
				          dir, value, 0, 30);

				/* Increase the bonus to resistances */
				time_2 = randint(20) + 20;
				set_oppose_elec(p_ptr->oppose_elec + time_2);
				set_oppose_cold(p_ptr->oppose_cold + time_2);
				set_oppose_acid(p_ptr->oppose_acid + time_2);
				set_oppose_pois(p_ptr->oppose_pois + time_2);
				set_oppose_fire(p_ptr->oppose_fire + time_2);

				o_ptr->timeout = rand_int(value * 3 / 4) + (value * 3 / 4);
				break;
			}

			case EGO_DRAGON_BRONZE:
			{
				value *= 50;

				sound(MSG_BR_CONF);
				msg_print("You breathe confusion.");
				fire_arc(GF_CONFUSION, dir, value, 0, 30);
				o_ptr->timeout = rand_int(value) + value;
				break;
			}

			case EGO_DRAGON_GOLD:
			{
				value *= 50;

				sound(MSG_BR_SOUND);
				msg_print("You breathe sound.");
				fire_arc(GF_SOUND, dir, value, 0, 30);
				o_ptr->timeout = rand_int(value) + value;
				break;
			}

			case EGO_DRAGON_CHAOS:
			{
				value *= 60;

				chance = rand_int(2);

				sound(((chance == 1 ? MSG_BR_CHAOS : MSG_BR_DISENCHANT)));

				msg_format("You breathe %s.",
				           ((chance == 1 ? "chaos" : "disenchantment")));
				fire_arc((chance == 1 ? GF_CHAOS : GF_DISENCHANT),
				          dir, value, 0, 30);
				o_ptr->timeout = rand_int(value) + value;
				break;
			}

			case EGO_DRAGON_LAW:
			{
				value *= 60;

				chance = rand_int(2);
				sound(((chance == 1 ? MSG_BR_SOUND : MSG_BR_SHARDS)));
				msg_format("You breathe %s.",
				           ((chance == 1 ? "sound" : "shards")));
				fire_arc((chance == 1 ? GF_SOUND : GF_SHARD),
				          dir, value, 0, 30);
				o_ptr->timeout = rand_int(value) + value;
				break;
			}

			case EGO_DRAGON_BALANCE:
			{
				value *= 75;

				chance = rand_int(4);

				sound(((chance == 1) ? MSG_BR_CHAOS :
				       ((chance == 2) ? MSG_BR_DISENCHANT :
				        ((chance == 3) ? MSG_BR_SOUND : MSG_BR_SHARDS))));

				msg_format("You breathe %s.",
				           ((chance == 1) ? "chaos" :
				            ((chance == 2) ? "disenchantment" :
				             ((chance == 3) ? "sound" : "shards"))));

				fire_arc(((chance == 1) ? GF_CHAOS :
				           ((chance == 2) ? GF_DISENCHANT :
				            ((chance == 3) ? GF_SOUND : GF_SHARD))),
				          dir, value, 0, 30);

				o_ptr->timeout = rand_int(value) + value;
				break;
			}

			case EGO_DRAGON_PSEUDO:
			{
				value *= 65;

				chance = rand_int(2);

				sound(((chance == 0 ? MSG_BR_LIGHT : MSG_BR_DARK)));

				msg_format("You breathe %s.",
				           ((chance == 0 ? "light" : "darkness")));

				fire_arc((chance == 0 ? GF_LITE : GF_DARK), dir, value, 0, 30);

				o_ptr->timeout = rand_int(value) + value;
				break;
			}

			case EGO_DRAGON_POWER:
			{
				value *= 100;

				sound(MSG_BR_ELEMENTS);
				msg_print("You breathe the elements.");
				fire_arc(GF_MISSILE, dir, value, 0, 30);
				o_ptr->timeout = rand_int(value) + value;
				break;
			}
		}

		/* Window stuff */
		p_ptr->window |= (PW_INVEN | PW_EQUIP);

		/* Success */
		return FALSE;
	}

	/* Hack -- some Rings can be activated for double resist and element ball */
	if (o_ptr->tval == TV_RING)
	{
		/* Get a direction for firing (or abort) */
		if (!get_aim_dir(&dir, FALSE)) return FALSE;

		/* Branch on the sub-type */
		switch (o_ptr->sval)
		{
			case SV_RING_ACID:
			{
				fire_ball(GF_ACID, dir, 70, 2);
				o_ptr->timeout = rand_int(50) + 50;
				break;
			}

			case SV_RING_FLAMES:
			{
				fire_ball(GF_FIRE, dir, 80, 2);
				o_ptr->timeout = rand_int(50) + 50;
				break;
			}

			case SV_RING_ICE:
			{
				fire_ball(GF_COLD, dir, 75, 2);
				o_ptr->timeout = rand_int(50) + 50;
				break;
			}

			case SV_RING_LIGHTNING:
			{
				fire_ball(GF_ELEC, dir, 85, 2);
				o_ptr->timeout = rand_int(50) + 50;
				break;
			}
		}

		/* Window stuff */
		p_ptr->window |= (PW_EQUIP);

		/* Success */
		return FALSE;
	}

	/* Mistake */
	msg_print("Oops.  That object cannot be activated.");

	/* Not used up */
	return (FALSE);
}


bool use_object(object_type *o_ptr, bool *ident)
{
	bool used;

	/* Analyze the object */
	switch (o_ptr->tval)
	{
		case TV_FOOD:
		{
			used = eat_food(o_ptr, ident);
			break;
		}

		case TV_POTION:
		{
			used = quaff_potion(o_ptr, ident);
			break;
		}

		case TV_SCROLL:
		{
			used = read_scroll(o_ptr, ident);
			break;
		}

		case TV_TALISMAN:
		{
			break;
		}

		case TV_WAND:
		{
			used = aim_wand(o_ptr, ident);
			break;
		}

		case TV_ROD:
		{
			break;
		}

		default:
		{
			used = activate_object(o_ptr);
			break;
		}
	}

	return (used);
}


#ifdef MACINTOSH
static int i = 0;
#endif


