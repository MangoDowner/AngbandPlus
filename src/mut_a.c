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
