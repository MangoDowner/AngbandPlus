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
	default:
		default_spell(cmd, res);
		break;
	}
}
*/

void albino_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Albino", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You turn into an albino! You feel frail...", "����ӥΤˤʤä����夯�ʤä���������..."));
		mut_lose(MUT_RESILIENT);
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You are no longer an albino!", "����ӥΤǤʤ��ʤä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You are albino (-4 CON).", "���ʤ��ϥ���ӥΤ���(�ѵ�-4)"));
		break;
	case SPELL_CALC_BONUS:
		p_ptr->stat_add[A_CON] -= 4;
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

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
					set_image(p_ptr->image + randint0(15) + 15, FALSE);
				}
			}
		}
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void ambidexterity_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Ambidexterity", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You feel like dual wielding.", ""));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You no longer feel like dual wielding.", ""));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You are ambidextrous.", ""));
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void arcane_mastery_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Arcane Mastery", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You gain arcane insights.", ""));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You feel your arcane mastery slipping away.", ""));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You have arcane mastery (-5% spell fail).", ""));
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void arthritis_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Arthritis", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("Your joints suddenly hurt.", "���᤬�����ˤ߽Ф�����"));
		mut_lose(MUT_LIMBER);
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("Your joints stop hurting.", "���᤬�ˤ��ʤ��ʤä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("Your joints ache constantly (-3 DEX).", "���ʤ��Ϥ��Ĥ������ˤߤ򴶤��Ƥ��롣(����-3)"));
		break;
	case SPELL_CALC_BONUS:
		p_ptr->stat_add[A_DEX] -= 3;
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void astral_guide_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Astral Guide", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You teleport quickly!", ""));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You no longer teleport quickly!", ""));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You are an astral guide (Teleport costs less energy).", ""));
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

void blank_face_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Blank Face", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("Your face becomes completely featureless!", "�Τäڤ�ܤ��ˤʤä���"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("Your facial features return.", "�����ɡ����ä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("Your face is featureless (-1 CHR).", "���ʤ��ϤΤäڤ�ܤ�����(̥��-1)"));
		break;
	case SPELL_CALC_BONUS:
		p_ptr->stat_add[A_CHR] -= 1;
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void bad_luck_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Black Aura", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("There is a malignant black aura surrounding you...", "���դ����������������餬���ʤ���Ȥ�ޤ���..."));
		mut_lose(MUT_GOOD_LUCK);
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("Your black aura swirls and fades.", "����������ϱ������ƾä�����"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("There is a black aura surrounding you.", "���ʤ��Ϲ���������ˤĤĤޤ�Ƥ��롣"));
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

void cult_of_personality_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Cult of Personality", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You gain control over your enemy's summons!", ""));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You lose control over your enemy's summons!", ""));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("Summoned monsters are sometimes friendly.", ""));
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void demonic_grasp_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Demonic Grasp", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You have a firm grasp on your magical devices.", ""));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You lose your firm grasp on your magical devices.", ""));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You can resist charge draining.", ""));
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

void einstein_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Living Computer", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("Your brain evolves into a living computer!", "���ʤ���Ǿ�����Υ���ԥ塼���˿ʲ�������"));
		mut_lose(MUT_MORONIC);
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("Your brain reverts to normal.", "Ǿ�����̤���ä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("Your brain is a living computer (+4 INT/WIS).", "���ʤ���Ǿ�����Υ���ԥ塼������(��ǽ������+4)"));
		break;
	case SPELL_CALC_BONUS:
		p_ptr->stat_add[A_INT] += 4;
		p_ptr->stat_add[A_WIS] += 4;
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void elec_aura_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Electric Aura", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("Electricity starts running through you!", "��ɤ���ή��ή��Ϥ᤿��"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("Electricity stops running through you.", "�Τ���ή��ή��ʤ��ʤä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("Electricity is running through your veins.", "���ʤ��η�ɤˤ���ή��ή��Ƥ��롣"));
		break;
	case SPELL_CALC_BONUS:
		p_ptr->sh_elec = TRUE;
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void evasion_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Evasion", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You gain the power of evasion.", ""));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You lose the power of evasion.", ""));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You are never crushed by earthquakes and you dodge monster breath attacks.", ""));
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void extra_eyes_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Extra Eyes", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You grow an extra pair of eyes!", "��������Ĥ��ܤ����褿��"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("Your extra eyes vanish!", ";ʬ���ܤ��ä��Ƥ��ޤä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You have an extra pair of eyes (+15 search).", "���ʤ���;ʬ����Ĥ��ܤ���äƤ��롣(õ��+15)"));
		break;
	case SPELL_CALC_BONUS:
		p_ptr->skill_fos += 15;
		p_ptr->skill_srh += 15;
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void extra_legs_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Extra Legs", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You grow an extra pair of legs!", "���������ܤ�­�������Ƥ�����"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("Your extra legs disappear!", ";ʬ�ʵӤ��ä��Ƥ��ޤä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You have an extra pair of legs (+3 speed).", "���ʤ���;ʬ�����ܤ�­�������Ƥ��롣(��®+3)"));
		break;
	case SPELL_CALC_BONUS:
		p_ptr->pspeed += 3;
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void extra_noise_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Extra Noise", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You start making strange noise!", "���ʤ��ϴ�̯�ʲ���Ω�ƻϤ᤿��"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You stop making strange noise!", "��̯�ʲ���Ω�Ƥʤ��ʤä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You make a lot of strange noise (-3 stealth).", "���ʤ����Ѥʲ���ȯ���Ƥ��롣(��̩-3)"));
		break;
	case SPELL_CALC_BONUS:
		p_ptr->skill_stl -= 3;
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void fast_learner_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Fast Learner", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You learn things quickly...", ""));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You revert to you normal dull self!", ""));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You are a fast learner.", ""));
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void fat_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Extra Fat", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You become sickeningly fat!", "���ʤ��ϵ������������餤���ä���"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You benefit from a miracle diet!", "����Ū�ʥ������åȤ�����������"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You are extremely fat (+2 CON, -2 speed).", "���ʤ��϶�ü�����äƤ��롣(�ѵ�+2,���ԡ���-2)"));
		break;
	case SPELL_CALC_BONUS:
		p_ptr->stat_add[A_CON] += 2;
		p_ptr->pspeed -= 2;
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void fearless_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Fearless", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You become completely fearless.", "�������ݤ��Τ餺�ˤʤä���"));
		mut_lose(MUT_COWARDICE);
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You begin to feel fear again.", "�ƤӶ��ݤ򴶤���褦�ˤʤä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You are completely fearless.", "���ʤ����������ݤ򴶤��ʤ���"));
		break;
	case SPELL_CALC_BONUS:
		p_ptr->resist_fear = TRUE;
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void fire_aura_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Fire Aura", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("Your body is enveloped in flames!", "���ʤ����Τϱ�ˤĤĤޤ�Ƥ��롣"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("Your body is no longer enveloped in flames.", "�Τ������ޤ�ʤ��ʤä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("Your body is enveloped in flames.", "���ʤ����Τϱ�ˤĤĤޤ�Ƥ��롣"));
		break;
	case SPELL_CALC_BONUS:
		p_ptr->sh_fire = TRUE;
		p_ptr->lite = TRUE;
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

void fleet_of_foot_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Fleet of Foot", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You feel fleet of foot!", ""));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You feel like your old plodding self!", ""));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You are fleet of foot (Movement costs less energy).", ""));
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

void good_luck_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("White Aura", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("There is a benevolent white aura surrounding you...", "���ῼ���򤤥����餬���ʤ���Ȥ�ޤ���..."));
		mut_lose(MUT_BAD_LUCK);
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("Your white aura shimmers and fades.", "�򤤥�����ϵ����ƾä�����"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("There is a white aura surrounding you.", "���ʤ����򤤥�����ˤĤĤޤ�Ƥ��롣"));
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

void illusion_normal_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Reassuring Image", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You start projecting a reassuring image.", "���ΰ¤餰���Ƥ�Ǥ��Ф��褦�ˤʤä���"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You stop projecting a reassuring image.", "�����¤餰���Ƥ�Ǥ��Ф��ʤ��ʤä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("Your appearance is masked with illusion.", "���ʤ��ϸ��Ƥ�ʤ���Ƥ��롣"));
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void infernal_deal_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Infernal Deal", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You make a pact with the devil!", ""));
		mut_lose(MUT_ARTHRITIS);
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("Your infernal pact is broken.", ""));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You have made an infernal deal.", ""));
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void infravision_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Infravision", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("Your infravision is improved.", "�ֳ������Ϥ���������"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("Your infravision is degraded.", "�ֳ������Ϥ��������"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You have remarkable infravision (+3).", "���ʤ��������餷���ֳ������Ϥ���äƤ��롣(+3)"));
		break;
	case SPELL_CALC_BONUS:
		p_ptr->see_infra += 3;		
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

void limber_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Limber", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("Your muscles become limber.", "���������ʤ䤫�ˤʤä���"));
		mut_lose(MUT_ARTHRITIS);
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("Your muscles stiffen.", "�������Ť��ʤä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("Your body is very limber (+3 DEX).", "���ʤ����Τ����ˤ��ʤ䤫����(����+3)"));
		break;
	case SPELL_CALC_BONUS:
		p_ptr->stat_add[A_DEX] += 3;
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void loremaster_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Loremaster", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You feel quite knowledgeable.", ""));
		mut_lose(MUT_ARTHRITIS);
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You know longer know so much.", ""));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You are a Loremaster.", ""));
		break;
	case SPELL_CAST:
		var_set_bool(res, cast_probing());
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void magic_resistance_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Magic Resistance", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You become resistant to magic.", "��ˡ�ؤ��������Ĥ�����"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You become susceptible to magic again.", "��ˡ�˼夯�ʤä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You are resistant to magic.", "���ʤ�����ˡ�ؤ��������äƤ��롣"));
		break;
	case SPELL_CALC_BONUS:
		p_ptr->skill_sav += (15 + (p_ptr->lev / 5));
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void merchants_friend_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Merchant's Friend", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You feel an intense desire to shop!", ""));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You no longer feel like shopping.", ""));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You are a Merchant's Friend.", ""));
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void moron_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Moron", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("Your brain withers away...", "Ǿ����̤��Ƥ��ޤä�..."));
		mut_lose(MUT_HYPER_INT);
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("Your brain reverts to normal.", "Ǿ�����̤���ä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You are moronic (-4 INT/WIS).", "���ʤ��������������(��ǽ������-4)"));
		break;
	case SPELL_CALC_BONUS:
		p_ptr->stat_add[A_INT] -= 4;
		p_ptr->stat_add[A_WIS] -= 4;
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void motion_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Motion", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You move with new assurance.", "�Τ�ư�������Τˤʤä���"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You move with less assurance.", "ư������Τ����ʤ��ʤä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("Your movements are precise and forceful (+1 STL).", "���ʤ���ư������Τ��϶�����(��̩+1)"));
		break;
	case SPELL_CALC_BONUS:
		p_ptr->free_act = TRUE;
		p_ptr->skill_stl += 1;
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
			
			if (music_singing_any()) bard_stop_singing();
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

void one_with_magic_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("One with Magic", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You feel one with magic.", ""));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You no longer feel one with magic.", ""));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You have a chance of resisting Dispel Magic and Antimagic.", ""));
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void peerless_sniper_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Peerless Sniper", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You feel distant monsters relax...", ""));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You feel distant monsters return to their normal grouchy selves.", ""));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("Your missiles are less likely to anger monsters.", ""));
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void peerless_tracker_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Peerless Tracker", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You feel able to track anything...", ""));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You can no longer track so well.", ""));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You are a peerless tracker.", ""));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Maps nearby area. Detects all monsters, traps, doors and stairs.", ""));
		break;
	case SPELL_CAST:
	{
		int rad1 = DETECT_RAD_MAP;
		int rad2 = DETECT_RAD_DEFAULT;
		map_area(rad1);
		detect_traps(rad2, TRUE);
		detect_doors(rad2);
		detect_stairs(rad2);
		detect_monsters_normal(rad2);
		var_set_bool(res, TRUE);
		break;
	}
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

void potion_chugger_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Potion Chugger", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You feel like chugging a six pack of healing potions.", ""));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You no longer feel like chugging your potions.", ""));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You chug potions twice as fast as normal.", ""));
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

void regeneration_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Regeneration", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You start regenerating.", "��®�˲������Ϥ᤿��"));
		mut_lose(MUT_FLESH_ROT);
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You stop regenerating.", "��®�������ʤ��ʤä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You are regenerating.", "���ʤ��ϵ�®�˲������롣"));
		break;
	case SPELL_CALC_BONUS:
		p_ptr->regenerate = TRUE;
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void resilient_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Resilient", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You become extraordinarily resilient.", "�³���ƥ��դˤʤä���"));
		mut_lose(MUT_ALBINO);
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You become ordinarily resilient again.", "���̤ξ��פ�����ä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You are very resilient (+4 CON).", "���ʤ����Τ����������٤�Ǥ��롣(�ѵ�+4)"));
		break;
	case SPELL_CALC_BONUS:
		p_ptr->stat_add[A_CON] += 4;
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void rotting_flesh_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Rotting Flesh", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("Your flesh is afflicted by a rotting disease!", "���ʤ������Τ����Ԥ����µ��˿����줿��"));
		mut_lose(MUT_STEEL_SKIN);
		mut_lose(MUT_REGEN);
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("Your flesh is no longer afflicted by a rotting disease!", "���Τ����Ԥ������µ������ä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("Your flesh is rotting (-2 CON, -1 CHR).", "���ʤ������Τ����Ԥ��Ƥ��롣(�ѵ�-2,̥��-1)"));
		break;
	case SPELL_CALC_BONUS:
		p_ptr->stat_add[A_CON] -= 2;
		p_ptr->stat_add[A_CHR] -= 1;
		p_ptr->regenerate = FALSE; /* Equip and spells processed later ... */
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void sacred_vitality_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Sacred Vitality", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You gain the power of Sacred Vitality!", ""));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You lose the power of Sacred Vitality!", ""));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You gain +30% from all healing effects.", ""));
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void scales_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Scales", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("Your skin turns into black scales!", "ȩ�������ڤ��Ѥ�ä���"));
		mut_lose(MUT_STEEL_SKIN);
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("Your scales vanish!", "�ڤ��ä�����"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("Your skin has turned into scales (-1 CHR, +10 AC).", "���ʤ���ȩ���ڤˤʤäƤ��롣(̥��-1, AC+10)"));
		break;
	case SPELL_CALC_BONUS:
		p_ptr->stat_add[A_CHR] -= 1;
		p_ptr->to_a += 10;
		p_ptr->dis_to_a += 10;
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

void short_legs_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Short Legs", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("Your legs turn into short stubs!", "­��û���͵��ˤʤäƤ��ޤä���"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("Your legs lengthen to normal.", "�Ӥ�Ĺ�������̤���ä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("Your legs are short stubs (-3 speed).", "���ʤ���­��û���͵�����(��®-3)"));
		break;
	case SPELL_CALC_BONUS:
		p_ptr->pspeed -= 3;
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void silly_voice_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Silly Voice", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("Your voice turns into a ridiculous squeak!", "������ȴ���ʥ����������ˤʤä���"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("Your voice returns to normal.", "���������̤���ä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("Your voice is a silly squeak (-4 CHR).", "���ʤ������ϴ�ȴ���ʥ�������������(̥��-4)"));
		break;
	case SPELL_CALC_BONUS:
		p_ptr->stat_add[A_CHR] -= 4;		
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

void steel_skin_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Steel Skin", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("Your skin turns to steel!", "���ʤ���ȩ��Ŵ�ˤʤä���"));
		mut_lose(MUT_SCALES);
		mut_lose(MUT_WARTS);
		mut_lose(MUT_FLESH_ROT);
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("Your skin reverts to flesh!", "ȩ�����ˤ�ɤä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("Your skin is made of steel (-1 DEX, +25 AC).", "���ʤ���ȩ��Ŵ�ǤǤ��Ƥ��롣(����-1, AC+25)"));
		break;
	case SPELL_CALC_BONUS:
		p_ptr->stat_add[A_DEX] -= 1;
		p_ptr->to_a += 25;
		p_ptr->dis_to_a += 25;
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void subtle_casting_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Subtle Casting", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You feel distant monsters relax...", ""));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You feel distant monsters return to their normal grouchy selves.", ""));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("Your spells are less likely to anger monsters.", ""));
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void telepathy_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Telepathy", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You develop a telepathic ability!", "�ƥ�ѥ�����ǽ�Ϥ�������"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You lose your telepathic ability!", "�ƥ�ѥ�����ǽ�Ϥ򼺤ä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You are telepathic.", "���ʤ��ϥƥ�ѥ�������äƤ��롣"));
		break;
	case SPELL_CALC_BONUS:
		p_ptr->telepathy = TRUE;
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

void untouchable_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Untouchable", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You feel untouchable!", ""));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("Your feel touchable!", ""));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You are untouchable (+30 AC).", ""));
		break;
	case SPELL_CALC_BONUS:
		p_ptr->to_a += 30;
		p_ptr->dis_to_a += 30;
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void unyielding_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Unyielding", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You will never yield!!", ""));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("Heck!  Might as well give up ...", ""));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You are unyielding (+1.5 HP per level).", ""));
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void vulnerability_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Vulnerability", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You feel strangely exposed.", "̯��̵�����ˤʤä��������롣"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You feel less exposed.", "̵�����ʴ����Ϥʤ��ʤä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You are susceptible to damage from the elements.", "���ʤ��ϸ��Ǥι���˼夤��"));
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

void warts_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Warts", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("Disgusting warts appear everywhere on you!", "�������������ܥ��ܤ�����ˤǤ�����"));
		mut_lose(MUT_STEEL_SKIN);
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("Your warts disappear!", "���ܥ��ܤ��ä�����"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("Your skin is covered with warts (-2 CHR, +5 AC).", "���ʤ���ȩ�ϥ��ܤ�����Ƥ��롣(̥��-2, AC+5)"));
		break;
	case SPELL_CALC_BONUS:
		p_ptr->stat_add[A_CHR] -= 2;
		p_ptr->to_a += 5;
		p_ptr->dis_to_a += 5;
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

void weapon_skills_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Weapon Versatility", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You feel you may master anything...", ""));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You no longer feel so masterful.", ""));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You may master any weapon.", ""));
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

void wings_mut(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Wings", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You grow a pair of wings.", "����˱�����������"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("Your wings fall off.", "����α���������������"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You have wings.", "���ʤ��ϱ�����äƤ��롣"));
		break;
	case SPELL_CALC_BONUS:
		p_ptr->levitation = TRUE;
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

