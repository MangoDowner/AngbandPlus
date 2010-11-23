/*
 * Mutations that cannot be activated as normal spells.
 * These mutations might be timed effects, or just things
 * like "Horns" that you simply have.  They might be augmentations
 * like "Super Human He-man".
 *
 * We are still implementing all mutations as spells for
 * uniformity.
 *
 * Again, spells are (stateless) objects, implemented as functions.
 */

#include "angband.h"

/*
void foo_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("", ""));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("", ""));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("", ""));
		break;
	case SPELL_CALC_BONUS:
		break;
	case SPELL_PROCESS:
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}
*/

void alcohol_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Alcohol", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("Your body starts producing alcohol!", "���ʤ��ϥ��륳�����ʬ�礹��褦�ˤʤä���"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("Your body stops producing alcohol!", "���ʤ��ϥ��륳�����ʬ�礷�ʤ��ʤä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("Your body produces alcohol.", "���ʤ����Τϥ��륳�����ʬ�礹�롣"));
		break;
	case SPELL_PROCESS:
		if (randint1(6400) == 321)
		{
			if (!p_ptr->resist_conf && !p_ptr->resist_chaos)
			{
				disturb(0, 0);
				p_ptr->redraw |= PR_EXTRA;
				msg_print(T("You feel a SSSCHtupor cOmINg over yOu... *HIC*!", "���Ҥ����⡼���ȤҤƤ��������դ�...�ҥå���"));
			}

			if (!p_ptr->resist_conf)
				set_confused(p_ptr->confused + randint0(20) + 15, FALSE);

			if (!p_ptr->resist_chaos)
			{
				if (one_in_(20))
				{
					msg_print(NULL);
					if (one_in_(3)) lose_all_info();
					else wiz_dark();
					teleport_player_aux(100, TELEPORT_NONMAGICAL | TELEPORT_PASSIVE);
					wiz_dark();
					msg_print(T("You wake up somewhere with a sore head...", "���ʤ��ϸ��Τ�̾����ܤ��ä᤿...Ƭ���ˤ���"));
					msg_print(T("You can't remember a thing, or how you got here!", "����Ф��Ƥ��ʤ����ɤ���äƤ������褿����ʬ����ʤ���"));
				}
				else if (one_in_(3))
				{
					msg_print(T("Thishcischs GooDSChtuff!", "�����줤�ʤ��礪�����Ȥ�줤���"));
					set_image(p_ptr->image + randint0(150) + 150, FALSE);
				}
			}
		}
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void attract_animal_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Attract Animals", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You start attracting animals.", "ưʪ������դ���褦�ˤʤä���"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You stop attracting animals.", "ưʪ������󤻤ʤ��ʤä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You attract animals.", "���ʤ���ưʪ������Ĥ��롣"));
		break;
	case SPELL_PROCESS:
		if (!p_ptr->anti_magic && one_in_(7000))
		{
			bool pet = one_in_(3);
			u32b mode = PM_ALLOW_GROUP;

			if (pet) mode |= PM_FORCE_PET;
			else mode |= (PM_ALLOW_UNIQUE | PM_NO_PET);

			if (summon_specific((pet ? -1 : 0), py, px, dun_level, SUMMON_ANIMAL, mode))
			{
				msg_print(T("You have attracted an animal!", "ưʪ������󤻤���"));
				disturb(0, 0);
			}
		}
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void attract_demon_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Attract Demons", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You start attracting demons.", "���������դ���褦�ˤʤä���"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You stop attracting demons.", "�ǡ���������󤻤ʤ��ʤä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You attract demons.", "���ʤ��ϥǡ���������Ĥ��롣"));
		break;
	case SPELL_PROCESS:
		if (!p_ptr->anti_magic && (randint1(6666) == 666))
		{
			bool pet = one_in_(6);
			u32b mode = PM_ALLOW_GROUP;

			if (pet) mode |= PM_FORCE_PET;
			else mode |= (PM_ALLOW_UNIQUE | PM_NO_PET);

			if (summon_specific((pet ? -1 : 0), py, px,
						dun_level, SUMMON_DEMON, mode))
			{
				msg_print(T("You have attracted a demon!", "���ʤ��ϥǡ���������󤻤���"));
				disturb(0, 0);
			}
		}
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void attract_dragon_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Attract Dragon", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You start attracting dragons.", "���ʤ��ϥɥ饴�������Ĥ���褦�ˤʤä���"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You stop attracting dragons.", "�ɥ饴�������󤻤ʤ��ʤä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You attract dragons.", "���ʤ��ϥɥ饴�������Ĥ��롣"));
		break;
	case SPELL_PROCESS:
		if (!p_ptr->anti_magic && one_in_(3000))
		{
			bool pet = one_in_(5);
			u32b mode = PM_ALLOW_GROUP;

			if (pet) mode |= PM_FORCE_PET;
			else mode |= (PM_ALLOW_UNIQUE | PM_NO_PET);

			if (summon_specific((pet ? -1 : 0), py, px, dun_level, SUMMON_DRAGON, mode))
			{
				msg_print(T("You have attracted a dragon!", "�ɥ饴�������󤻤���"));
				disturb(0, 0);
			}
		}
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void beak_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Beak", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("Your mouth turns into a sharp, powerful beak!", "�����Ԥ����������Х����Ѳ�������"));
		mut_lose(MUT_TRUNK);
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("Your mouth reverts to normal!", "�������̤���ä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You have a beak (dam. 2d4).", "���ʤ��ϥ����Х��������Ƥ��롣(���᡼�� 2d4)"));
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void berserk_rage_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Berserk Rage", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You become subject to fits of berserk rage!", "���ʤ��϶�˽����ȯ��򵯤����褦�ˤʤä���"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You are no longer subject to fits of berserk rage!", "��˽����ȯ��ˤ��餵��ʤ��ʤä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You are subject to berserker fits.", "���ʤ��϶���β���ȯ��򵯤�����"));
		break;
	case SPELL_PROCESS:
		if (!p_ptr->shero && one_in_(3000))
		{
			disturb(0, 0);
			cast_berserk();
		}
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void chaos_deity_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Chaos Deity", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You attract the notice of a chaos deity!", "���ʤ��ϥ������μ������դ�椯�褦�ˤʤä���"));
		/* In case it isn't obvious, every character has a chaos deity assigned at birth. */
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You lose the attention of the chaos deities.", "���٤ο����ζ�̣��椫�ʤ��ʤä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("Chaos deities give you gifts.", "���ʤ��ϥ������μ��⤫��˫���򤦤��Ȥ롣"));
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void cowardice_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Cowardice", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You become an incredible coward!", "�������ʤ����餤���¤ˤʤä���"));
		mut_lose(MUT_FEARLESS);
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You are no longer an incredible coward!", "�⤦�����������ۤɲ��¤ǤϤʤ��ʤä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You are subject to cowardice.", "���ʤ��ϻ������¤ˤʤ롣"));
		break;
	case SPELL_PROCESS:
		if (!p_ptr->resist_fear && (randint1(3000) == 13))
		{
			disturb(0, 0);
			msg_print(T("It's so dark... so scary!", "�ȤƤ�Ť�... �ȤƤⶲ����"));
			set_afraid(p_ptr->afraid + 13 + randint1(26), FALSE);
		}
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void eat_light_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Eat Light", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You feel a strange kinship with Ungoliant.", "���ʤ��ϥ��󥴥ꥢ��Ȥ˴�̯�ʿƤ��ߤ�Ф���褦�ˤʤä���"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You feel the world's a brighter place.", "���������뤤�ȴ����롣"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You sometimes feed off of the light around you.", "���ʤ��ϻ������Ϥθ���ۼ����Ʊ��ܤˤ��롣"));
		break;
	case SPELL_PROCESS:
		if (one_in_(3000))
		{
			object_type *o_ptr;

			msg_print(T("A shadow passes over you.", "�ƤˤĤĤޤ줿��"));
			msg_print(NULL);

			/* Absorb light from the current possition */
			if ((cave[py][px].info & (CAVE_GLOW | CAVE_MNDK)) == CAVE_GLOW)
			{
				hp_player(10);
			}

			o_ptr = &inventory[INVEN_LITE];

			/* Absorb some fuel in the current lite */
			if (o_ptr->tval == TV_LITE)
			{
				/* Use some fuel (except on artifacts) */
				if (!object_is_fixed_artifact(o_ptr) && (o_ptr->xtra4 > 0))
				{
					/* Heal the player a bit */
					hp_player(o_ptr->xtra4 / 20);

					/* Decrease life-span of lite */
					o_ptr->xtra4 /= 2;

					msg_print(T("You absorb energy from your light!", "�������饨�ͥ륮����ۼ�������"));
					notice_lite_change(o_ptr);
				}
			}

			/*
			 * Unlite the area (radius 10) around player and
			 * do 50 points damage to every affected monster
			 */
			unlite_area(50, 10);
		}
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void flatulence_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Flatulence", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You become subject to uncontrollable flatulence.", "���ʤ���������ǽ�ʶ��������򤳤��褦�ˤʤä���"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You are no longer subject to uncontrollable flatulence.", "�⤦���������Ϥ����ʤ��ʤä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You are subject to uncontrollable flatulence.", "���ʤ�������Ǥ��ʤ����������򤳤���"));
		break;
	case SPELL_PROCESS:
		if (randint1(3000) == 13)
		{
			disturb(0, 0);
			/* Seriously, this the best mutation!  Ever!! :D */
			msg_print(T("BRRAAAP! Oops.", "�֥������á����äȡ�"));
			msg_print(NULL);
			fire_ball(GF_POIS, 0, p_ptr->lev, 3);
		}
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void fumbling_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Fumbling", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("Your feet grow to four times their former size.", "���ʤ��εӤ�Ĺ�������ܤˤʤä���"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("Your feet shrink to their former size.", "�Ӥ������礭������ä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You occasionally stumble and drop things.", "���ʤ��Ϥ褯�ĤޤŤ���ʪ����Ȥ���"));
		break;
	case SPELL_PROCESS:
		if (one_in_(10000))
		{
			int slot = 0;
			object_type *o_ptr = NULL;

			disturb(0, 0);
			msg_print(T("You trip over your own feet!", "­����Ĥ��ž�����"));
			take_hit(DAMAGE_NOESCAPE, randint1(p_ptr->wt / 6), T("tripping", "ž��"), -1);
			msg_print(NULL);

			if (buki_motteruka(INVEN_RARM))
			{
				slot = INVEN_RARM;
				o_ptr = &inventory[INVEN_RARM];

				if (buki_motteruka(INVEN_LARM) && one_in_(2))
				{
					o_ptr = &inventory[INVEN_LARM];
					slot = INVEN_LARM;
				}
			}
			else if (buki_motteruka(INVEN_LARM))
			{
				o_ptr = &inventory[INVEN_LARM];
				slot = INVEN_LARM;
			}

			if (slot && !object_is_cursed(o_ptr))
			{
				msg_print(T("You drop your weapon!", "������Ȥ��Ƥ��ޤä���"));
				inven_drop(slot, 1);
				msg_print("Press 'Y' to continue.");
				flush();
				for (;;)
				{
					char ch = inkey();
					if (ch == 'Y') break;
				}
				prt("", 0, 0);
				msg_flag = FALSE;
			}
		}
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void hallucination_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Hallucination", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You are afflicted by a hallucinatory insanity!", "���ʤ��ϸ��Ф������������������˿����줿��"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You are no longer afflicted by a hallucinatory insanity!", "���Ф�Ҥ������������㳲�򵯤����ʤ��ʤä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You have a hallucinatory insanity.", "���ʤ��ϸ��Ф������������������˿�����Ƥ��롣"));
		break;
	case SPELL_PROCESS:
		if (!p_ptr->resist_chaos && randint1(6400) == 42)
		{
			disturb(0, 0);
			p_ptr->redraw |= PR_EXTRA;
			set_image(p_ptr->image + randint0(50) + 20, FALSE);
		}
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void he_man_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("He-man", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You turn into a superhuman he-man!", "Ķ��Ū�˶����ʤä���"));
		mut_lose(MUT_PUNY);
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("Your muscles revert to normal.", "���������̤���ä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You are superhumanly strong (+4 STR).", "���ʤ���Ķ��Ū�˶�����(����+4)"));
		break;
	case SPELL_CALC_BONUS:
		p_ptr->stat_add[A_STR] += 4;
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void horns_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Horns", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("Horns pop forth into your forehead!", "�ۤ˳Ѥ���������"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("Your horns vanish from your forehead!", "�ۤ���Ѥ��ä�����"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You have horns (dam. 2d6).", "���ʤ��ϳѤ������Ƥ��롣(���᡼�� 2d6)"));
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void hp_to_sp_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("HP to SP", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You are subject to fits of painful clarity.", "�ˤߤ�ȼ���������Ʋ���ȯ��򵯤����褦�ˤʤä���"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You are no longer subject to fits of painful clarity.", "�ˤߤ�ȼ���������Ʋ���ȯ��˽����ʤ��ʤä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("Your blood sometimes rushes to your head.", "���ʤ��ϻ���Ƭ�˷줬�ɤä�ή��롣"));
		break;
	case SPELL_PROCESS:
		if (!p_ptr->anti_magic && one_in_(4000))
		{
			int wounds = p_ptr->msp - p_ptr->csp;

			if (wounds > 0)
			{
				int healing = p_ptr->chp;

				if (healing > wounds)
					healing = wounds;

				p_ptr->csp += healing;

				p_ptr->redraw |= (PR_MANA);
				take_hit(DAMAGE_LOSELIFE, healing, T("blood rushing to the head", "Ƭ�˾��ä���"), -1);
			}
		}
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void invulnerability_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Invulnerability", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You are blessed with fits of invulnerability.", "���ʤ��Ͻ�ʡ���졢̵Ũ���֤ˤʤ�ȯ��򵯤����褦�ˤʤä���"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You are no longer blessed with fits of invulnerability.", "̵Ũ���֤�ȯ��򵯤����ʤ��ʤä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You occasionally feel invincible.", "���ʤ��ϻ����餱�Τ餺�ʵ�ʬ�ˤʤ롣"));
		break;
	case SPELL_PROCESS:
		if (!p_ptr->anti_magic && one_in_(5000))
		{
			disturb(0, 0);
			msg_print(T("You feel invincible!", "̵Ũ�ʵ������롪"));

			msg_print(NULL);
			set_invuln(randint1(8) + 8, FALSE);
		}
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void nausea_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Nausea", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("Your stomach starts to roil nauseously.", "���ޤ��ԥ��ԥ����Ϥ��᤿��"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("Your stomach stops roiling.", "�ߤ���ڻ���ʤ��ʤä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You have a seriously upset stomach.", "���ʤ��ΰߤ���������夭���ʤ���"));
		break;
	case SPELL_PROCESS:
		if (!p_ptr->slow_digest && one_in_(9000))
		{
			disturb(0, 0);
			
			msg_print(T("Your stomach roils, and you lose your lunch!", "�ߤ���ڻ���������򼺤ä���"));
			msg_print(NULL);

			set_food(PY_FOOD_WEAK);
			
			if (music_singing_any()) stop_singing();
			if (hex_spelling_any()) stop_hex_spell_all();
		}
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void normality_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Normality", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You feel strangely normal.", "���ʤ��ϴ�̯�ʤۤ����̤ˤʤä��������롣"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You feel normally strange.", "���̤˴�̯�ʴ��������롣"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You may be mutated, but you're recovering.", "���ʤ����Ѱۤ��Ƥ��������������Ƥ��Ƥ��롣"));
		break;
	case SPELL_PROCESS:
		if (one_in_(5000))
		{
			if (mut_lose_random(NULL))
				msg_print(T("You feel oddly normal.", "��̯�ʤ��餤���̤ˤʤä��������롣"));
		}
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void polymorph_wounds_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Polymorph Wounds", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You feel forces of chaos entering your old scars.", "���ʤ��ϥ��������Ϥ��Ť������������Ǥ���Τ򴶤�����"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You feel forces of chaos departing your old scars.", "�Ť������饫�������Ϥ���äƤ��ä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("Your health is subject to chaotic forces.", "���ʤ��η򹯤ϥ��������Ϥ˱ƶ�������롣"));
		break;
	case SPELL_PROCESS:
		if (one_in_(3000))
			do_poly_wounds();
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void produce_mana_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Produce Mana", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You start producing magical energy uncontrollably.", "���ʤ���������ǽ����ˡ�Υ��ͥ륮����ȯ������褦�ˤʤä���"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You stop producing magical energy uncontrollably.", "������ǽ����ˡ�Υ��ͥ륮����ȯ�����ʤ��ʤä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You are producing magical energy uncontrollably.", "���ʤ���������ǽ����ˡ�Υ��ͥ륮����ȯ���Ƥ��롣"));
		break;
	case SPELL_PROCESS:
		if (!p_ptr->anti_magic && one_in_(9000))
		{
			int dir = 0;
			disturb(0, 0);
			msg_print(T("Magical energy flows through you! You must release it!", "��ˡ�Υ��ͥ륮�����������ʤ������ή�����Ǥ��������ͥ륮����������ʤ���Фʤ�ʤ���"));
			flush();
			msg_print(NULL);
			(void)get_hack_dir(&dir);
			fire_ball(GF_MANA, dir, p_ptr->lev * 2, 3);
		}
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void puny_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Puny", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("Your muscles wither away...", "��������äƤ��ޤä�..."));
		mut_lose(MUT_HYPER_STR);
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("Your muscles revert to normal.", "���������̤���ä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You are puny (-4 STR).", "���ʤ��ϵ������(����-4)"));
		break;
	case SPELL_CALC_BONUS:
		p_ptr->stat_add[A_STR] -= 4;
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void random_banish_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Random Banish", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You feel a terrifying power lurking behind you.", "�������Ϥ����ʤ����ظ������Ǥ��뵤�����롣"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You no longer feel a terrifying power lurking behind you.", "�ظ�˶������Ϥ򴶤��ʤ��ʤä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You sometimes cause nearby creatures to vanish.", "���ʤ��ϻ����᤯�Υ�󥹥�������Ǥ����롣"));
		break;
	case SPELL_PROCESS:
		if (one_in_(9000))
		{
			disturb(0, 0);
			msg_print(T("You suddenly feel almost lonely.", "�����ۤȤ�ɸ��Ȥˤʤä��������롣"));

			banish_monsters(100);
			if (!dun_level && p_ptr->town_num)
			{
				int n;

				/* Pick a random shop (except home) */
				do
				{
					n = randint0(MAX_STORES);
				}
				while ((n == STORE_HOME) || (n == STORE_MUSEUM));

				msg_print(T("You see one of the shopkeepers running for the hills!", "Ź�μ�ͤ��֤˸����ä����äƤ��롪"));
				store_shuffle(n);
			}
			msg_print(NULL);
		}
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void random_teleport_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Random Teleportation", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("Your position seems very uncertain...", "���ʤ��ΰ��֤������Գ���ˤʤä���"));
		mut_lose(MUT_TELEPORT);
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("Your position seems more certain.", "���ʤ��ΰ��֤Ϥ�����Ū�ˤʤä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You are teleporting randomly.", "���ʤ��ϥ�����˥ƥ�ݡ��Ȥ��롣"));
		break;
	case SPELL_PROCESS:
		if (!p_ptr->resist_nexus && !p_ptr->anti_tele && (randint1(5000) == 88))
		{
			disturb(0, 0);
			msg_print(T("Your position suddenly seems very uncertain...", "���ʤ��ΰ��֤������Ҥ��礦���Գ���ˤʤä�..."));
			msg_print(NULL);
			teleport_player(40, TELEPORT_PASSIVE);
		}
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void raw_chaos_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Raw Chaos", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You feel the universe is less stable around you.", "���Ϥζ��֤��԰���ˤʤä��������롣"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You feel the universe is more stable around you.", "���Ϥζ��֤����ꤷ���������롣"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You occasionally are surrounded with raw chaos.", "���ʤ��Ϥ��Ф��н㥫��������ޤ�롣"));
		break;
	case SPELL_PROCESS:
		if (!p_ptr->anti_magic && one_in_(8000))
		{
			disturb(0, 0);
			msg_print(T("You feel the world warping around you!", "����ζ��֤��Ĥ�Ǥ��뵤�����롪"));
			msg_print(NULL);
			fire_ball(GF_CHAOS, 0, p_ptr->lev, 8);
		}
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void scorpion_tail_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Scorpion Tail", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You grow a scorpion tail!", "������ο����������Ƥ�����"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You lose your scorpion tail!", "������ο������ʤ��ʤä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You have a scorpion tail (poison, 3d7).", "���ʤ��ϥ�����ο����������Ƥ��롣(�ǡ����᡼�� 3d7)"));
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void shadow_walk_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Shadow Walk", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You feel like reality is as thin as paper.", "���ʤ��ϸ��¤���Τ褦�������ȴ�����褦�ˤʤä���"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You feel like you're trapped in reality.", "ʪ�������������Ƥ��뵤�����롣"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You occasionally stumble into other shadows.", "���ʤ��Ϥ��Ф���¾�Ρֱơפ��¤����ࡣ"));
		break;
	case SPELL_PROCESS:
		if (!p_ptr->anti_magic && one_in_(12000) && !p_ptr->inside_arena)
			alter_reality();
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void sp_to_hp_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("SP to HP", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You are subject to fits of magical healing.", "��ˡ�μ�����ȯ��򵯤����褦�ˤʤä���"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You are no longer subject to fits of magical healing.", "��ˡ�μ�����ȯ��˽����ʤ��ʤä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("Your blood sometimes rushes to your muscles.", "���ʤ��ϻ����줬�����ˤɤä�ή��롣"));
		break;
	case SPELL_PROCESS:
		if (one_in_(2000))
		{
			int wounds = p_ptr->mhp - p_ptr->chp;

			if (wounds > 0)
			{
				int healing = p_ptr->csp;

				if (healing > wounds)
					healing = wounds;

				hp_player(healing);
				p_ptr->csp -= healing;

				p_ptr->redraw |= (PR_MANA);
			}
		}
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void speed_flux_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Speed Flux", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You become manic-depressive.", "���ʤ����ݵ���ˤʤä���"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You are no longer manic-depressive.", "�ݵ���Ǥʤ��ʤä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You move faster or slower randomly.", "���ʤ��ϥ�������᤯ư�������٤�ư�����ꤹ�롣"));
		break;
	case SPELL_PROCESS:
		if (one_in_(6000))
		{
			disturb(0, 0);
			if (one_in_(2))
			{
				msg_print(T("You feel less energetic.", "����Ū�Ǥʤ��ʤä��������롣"));
				if (p_ptr->fast > 0)
					set_fast(0, TRUE);
				else
					set_slow(randint1(30) + 10, FALSE);
			}
			else
			{
				msg_print(T("You feel more energetic.", "����Ū�ˤʤä��������롣"));
				if (p_ptr->slow > 0)
					set_slow(0, TRUE);
				else
					set_fast(randint1(30) + 10, FALSE);
			}
			msg_print(NULL);
		}
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void tentacles_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Tentacles", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("Evil-looking tentacles sprout from your sides.", "�ٰ��ʿ��꤬�Τ�ξ¦�������Ƥ�����"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("Your tentacles vanish from your sides.", "���꤬�ä�����"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You have evil looking tentacles (dam 2d5).", "���ʤ��ϼٰ��ʿ������äƤ��롣(���᡼�� 2d5)"));
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void trunk_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Elephantine Trunk", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("Your nose grows into an elephant-like trunk.", "���ʤ���ɡ�Ͽ��Ӥƾݤ�ɡ�Τ褦�ˤʤä���"));
		mut_lose(MUT_BEAK);
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("Your nose returns to a normal length.", "ɡ�����̤�Ĺ������ä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You have an elephantine trunk (dam 1d4).", "���ʤ��ϾݤΤ褦��ɡ����äƤ��롣(���᡼�� 1d4)"));
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void warning_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Warning", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You suddenly feel paranoid.", "���ʤ��������ѥ�Υ����ˤʤä��������롣"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You no longer feel paranoid.", "�ѥ�Υ����Ǥʤ��ʤä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You receive warnings about your foes.", "���ʤ���Ũ�˴ؤ���ٹ�򴶤��롣"));
		break;
	case SPELL_PROCESS:
		if (one_in_(1000))
		{
			int danger_amount = 0;
			int monster;

			for (monster = 0; monster < m_max; monster++)
			{
				monster_type    *m_ptr = &m_list[monster];
				monster_race    *r_ptr = &r_info[m_ptr->r_idx];

				/* Skip dead monsters */
				if (!m_ptr->r_idx) continue;

				if (r_ptr->level >= p_ptr->lev)
				{
					danger_amount += r_ptr->level - p_ptr->lev + 1;
				}
			}

			if (danger_amount > 100)
				msg_print(T("You feel utterly terrified!", "���˶������������롪"));

			else if (danger_amount > 50)
				msg_print(T("You feel terrified!", "�������������롪"));

			else if (danger_amount > 20)
				msg_print(T("You feel very worried!", "���˿��ۤʵ������롪"));

			else if (danger_amount > 10)
				msg_print(T("You feel paranoid!", "���ۤʵ������롪"));

			else if (danger_amount > 5)
				msg_print(T("You feel almost safe.", "�ۤȤ�ɰ����ʵ������롣"));

			else
				msg_print(T("You feel lonely.", "�䤷���������롣"));
		}
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void wasting_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Horrible Wasting", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You suddenly contract a horrible wasting disease.", "���ʤ������������ޤ�������¤ˤ����ä���"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You are cured of the horrible wasting disease!", "�����ޤ�������¤����ä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You have a horrible wasting disease.", "���ʤ��Ͽ�夹�붲�����µ��ˤ����äƤ��롣"));
		break;
	case SPELL_PROCESS:
		if (one_in_(3000))
		{
			int which_stat = randint0(6);
			int sustained = FALSE;

			switch (which_stat)
			{
			case A_STR:
				if (p_ptr->sustain_str) sustained = TRUE;
				break;
			case A_INT:
				if (p_ptr->sustain_int) sustained = TRUE;
				break;
			case A_WIS:
				if (p_ptr->sustain_wis) sustained = TRUE;
				break;
			case A_DEX:
				if (p_ptr->sustain_dex) sustained = TRUE;
				break;
			case A_CON:
				if (p_ptr->sustain_con) sustained = TRUE;
				break;
			case A_CHR:
				if (p_ptr->sustain_chr) sustained = TRUE;
				break;
			default:
				msg_print(T("Invalid stat chosen!", "�����ʾ��֡�"));
				sustained = TRUE;
				break;
			}

			if (!sustained)
			{
				disturb(0, 0);
				msg_print(T("You can feel yourself wasting away!", "��ʬ����夷�Ƥ����Τ�ʬ���롪"));
				msg_print(NULL);
				dec_stat(which_stat, randint1(6) + 6, one_in_(6));
			}
		}
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void weird_mind_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Weird Mind", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("Your thoughts suddenly take off in strange directions.", "���ʤ��λ׹ͤ������������������˸����Ϥ᤿��"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("Your thoughts return to boring paths.", "�׹ͤ��������������ä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("Your mind randomly expands and contracts.", "���ʤ��������ϥ�����˳��礷����̾������ꤷ�Ƥ��롣"));
		break;
	case SPELL_PROCESS:
		if (!p_ptr->anti_magic && one_in_(3000))
		{
			if (p_ptr->tim_esp > 0)
			{
				msg_print(T("Your mind feels cloudy!", "�����ˤ�䤬�����ä���"));
				set_tim_esp(0, TRUE);
			}
			else
			{
				msg_print(T("Your mind expands!", "�����������ä���"));
				set_tim_esp(p_ptr->lev, FALSE);
			}
		}
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void wraith_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Wraithform", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You start to fade in and out of the physical world.", "���ʤ���ͩ�β���������β������ꤹ��褦�ˤʤä���"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You are firmly in the physical world.", "���ʤ���ʪ�������ˤ��ä���¸�ߤ��Ƥ��롣"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You fade in and out of physical reality.", "���ʤ������Τ�ͩ�β���������β������ꤹ�롣"));
		break;
	case SPELL_PROCESS:
		if (!p_ptr->anti_magic && one_in_(3000))
		{
			disturb(0, 0);
			msg_print(T("You feel insubstantial!", "��ʪ����������"));
			msg_print(NULL);
			set_wraith_form(randint1(p_ptr->lev / 2) + (p_ptr->lev / 2), FALSE);
		}
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

