/* File: mutation.c */

/* Purpose: Mutation effects (and racial powers) */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"


bool gain_random_mutation(int choose_mut, bool as_mutation)
{
	int     attempts_left = 20;
	cptr    muta_desc = "";
	bool    muta_chosen = FALSE;
	u32b    muta_which = 0;

	if (choose_mut) attempts_left = 1;

	while (attempts_left--)
	{
		switch (choose_mut ? choose_mut : randint1(38))
		{
		case 1:
			muta_which = MUT_ELEM_UNSTABLE;
#ifdef JP
			muta_desc = "���ʤ��Υ�����Ȥ��԰���ˤʤä���";
#else
			muta_desc = "Your element become unstable.";
#endif

			break;
		case 2:
			muta_which = MUT_ELEM_MULTI;
#ifdef JP
			muta_desc = "���ʤ��Υ�����Ȥ������Ƥˤʤä���";
#else
			muta_desc = "Your element become unstable.";
#endif

			break;
		case 3: case 4:
			muta_which = MUT_SPEED_FLUX;
#ifdef JP
			muta_desc = "���ʤ����ݵ���ˤʤä���";
#else
			muta_desc = "You become manic-depressive.";
#endif

			break;
		case 5: case 6:
			muta_which = MUT_BANISH_ALL;
#ifdef JP
			muta_desc = "�������Ϥ����ʤ����ظ������Ǥ��뵤�����롣";
#else
			muta_desc = "You feel a terrifying power lurking behind you.";
#endif

			break;
		case 7:
			muta_which = MUT_EAT_LIGHT;
#ifdef JP
			muta_desc = "���ʤ��ϥ��󥴥ꥢ��Ȥ˴�̯�ʿƤ��ߤ�Ф���褦�ˤʤä���";
#else
			muta_desc = "You feel a strange kinship with Ungoliant.";
#endif

			break;
		case 8: case 9: case 10:
			muta_which = MUT_NORMALITY;
#ifdef JP
			muta_desc = "���ʤ��ϴ�̯�ʤۤ����̤ˤʤä��������롣";
#else
			muta_desc = "You feel strangely normal.";
#endif

			break;
		case 11:
			muta_which = MUT_WASTING;
#ifdef JP
			muta_desc = "���ʤ������������ޤ�������¤ˤ����ä���";
#else
			muta_desc = "You suddenly contract a horrible wasting disease.";
#endif

			break;
		case 12:
			muta_which = MUT_ALTER_REALITY;
#ifdef JP
			muta_desc = "���ʤ��ϸ��¤���Τ褦�������ȴ�����褦�ˤʤä���";
#else
			muta_desc = "You feel like reality is as thin as paper.";
#endif

			break;
		case 13: case 14:
			muta_which = MUT_SP_TO_HP;
#ifdef JP
			muta_desc = "��ˡ�μ�����ȯ��򵯤����褦�ˤʤä���";
#else
			muta_desc = "You are subject to fits of magical healing.";
#endif

			break;
		case 15:
			muta_which = MUT_HP_TO_SP;
#ifdef JP
			muta_desc = "�ˤߤ�ȼ���������Ʋ���ȯ��򵯤����褦�ˤʤä���";
#else
			muta_desc = "You are subject to fits of painful clarity.";
#endif

			break;
		case 16: case 17:
			muta_which = MUT_ELEC_BODY;
#ifdef JP
			muta_desc = "���ʤ����Τϥ��ѡ����ˤĤĤޤ�Ƥ��롣";
#else
			muta_desc = "Electricity starts running through you!";
#endif

			break;
		case 18: case 19:
			muta_which = MUT_FIRE_BODY;
#ifdef JP
			muta_desc = "���ʤ����Τϱ�ˤĤĤޤ�Ƥ��롣";
#else
			muta_desc = "Your body is enveloped in flames!";
#endif

			break;
		case 20: case 21:
			muta_which = MUT_COLD_BODY;
#ifdef JP
			muta_desc = "���ʤ����Τ�ɹ�ˤĤĤޤ�Ƥ��롣";
#else
			muta_desc = "Your body is enveloped in flames!";
#endif

			break;
		case 22: case 23:
			muta_which = MUT_XTRA_FAT;
#ifdef JP
			muta_desc = "���ʤ��ϵ������������餤���ä���";
#else
			muta_desc = "You become sickeningly fat!";
#endif

			break;
		case 24: case 25: case 26:
			muta_which = MUT_FLESH_ROT;
#ifdef JP
			muta_desc = "���ʤ������Τ����Ԥ����µ��˿����줿��";
#else
			muta_desc = "Your flesh is afflicted by a rotting disease!";
#endif

			break;
		case 27: case 28: case 29:
			muta_which = MUT_THICK_SKIN;
#ifdef JP
			muta_desc = "ȩ��ʬ����������Ѥ�ä���";
#else
			muta_desc = "Disgusting warts appear everywhere on you!";
#endif

			break;
		case 30: case 31: case 32:
			muta_which = MUT_SCALES;
#ifdef JP
			muta_desc = "ȩ�������ڤ��Ѥ�ä���";
#else
			muta_desc = "Your skin turns into black scales!";
#endif

			break;
		case 33: case 34: case 35:
			muta_which = MUT_FUR;
#ifdef JP
			muta_desc = "���ʤ����Τ�Ĺ���Ӥ�ʤ��줿��";
#else
			muta_desc = "Your skin turns to steel!";
#endif

			break;
		case 36: case 37:
			muta_which = MUT_WINGS;
#ifdef JP
			muta_desc = "����˱�����������";
#else
			muta_desc = "You grow a pair of wings.";
#endif

			break;
		case 38:
			muta_which = MUT_VULN_ELEM;
#ifdef JP
			muta_desc = "̯��̵�����ˤʤä��������롣";
#else
			muta_desc = "You feel strangely exposed.";
#endif

			break;
		default:
			muta_which = 0;
		}

		if (muta_which)
		{
			if (!(p_ptr->mutation & muta_which))
			{
				muta_chosen = TRUE;
			}
		}
		if (muta_chosen == TRUE) break;
	}

	if (!muta_chosen)
	{
#ifdef JP
		msg_print("���̤ˤʤä��������롣");
#else
		msg_print("You feel normal.");
#endif

		return FALSE;
	}
	else
	{
		if (as_mutation)
		{
#ifdef JP
			msg_print("�����Ѱۤ�����");
#else
			msg_print("You mutate!");
#endif
		}

		msg_print(muta_desc);
		p_ptr->mutation |= muta_which;

		if ((muta_which == MUT_THICK_SKIN) || (muta_which == MUT_SCALES))
		{
			if (p_ptr->mutation & MUT_FUR)
			{
#ifdef JP
				msg_print("�Ӥ�ȴ���������");
#else
				msg_print("You lose your furs.");
#endif

				p_ptr->mutation &= ~(MUT_FUR);
			}
		}
		else if ((muta_which == MUT_SCALES) || (muta_which == MUT_FUR))
		{
			if (p_ptr->mutation & MUT_THICK_SKIN)
			{
#ifdef JP
				msg_print("���̤��������ä���");
#else
				msg_print("You lose your thick skin.");
#endif

				p_ptr->mutation &= ~(MUT_THICK_SKIN);
			}
		}
		else if ((muta_which == MUT_THICK_SKIN) || (muta_which == MUT_FUR))
		{
			if (p_ptr->mutation & MUT_SCALES)
			{
#ifdef JP
				msg_print("�ڤ��ʤ��ʤä���");
#else
				msg_print("You lose your scales.");
#endif

				p_ptr->mutation &= ~(MUT_SCALES);
			}
		}

		change_your_alignment(ALI_LNC, -10);

		mutant_regenerate_mod = calc_mutant_regenerate_mod();
		p_ptr->update |= PU_BONUS;
		handle_stuff();
		return TRUE;
	}
}


bool lose_mutation(int choose_mut)
{
	int attempts_left = 20;
	cptr muta_desc = "";
	bool muta_chosen = FALSE;
	u32b muta_which = 0;

	if (choose_mut) attempts_left = 1;

	while (attempts_left--)
	{
		switch (choose_mut ? choose_mut : randint1(38))
		{
		case 1:
			muta_which = MUT_ELEM_UNSTABLE;
#ifdef JP
			muta_desc = "���ʤ��Υ�����Ȥ��Ѳ����ߤޤä���";
#else
			muta_desc = "Changing of your element has stopped.";
#endif

			break;
		case 2:
			muta_which = MUT_ELEM_MULTI;
#ifdef JP
			muta_desc = "���ʤ��Υ�����Ȥϸ��ꤵ�줿��";
#else
			muta_desc = "Changing of your element has stopped.";
#endif
			p_ptr->celem = p_ptr->pelem;
			init_realm_table();
			p_ptr->update |= (PU_BONUS);
			p_ptr->notice |= (PN_REORDER);
			(void)combine_and_reorder_home(STORE_HOME);
			(void)combine_and_reorder_home(STORE_MUSEUM);
			load_all_pref_files();
			if (p_ptr->action == ACTION_ELEMSCOPE) lite_spot(py, px);

			break;
		case 3: case 4:
			muta_which = MUT_SPEED_FLUX;
#ifdef JP
			muta_desc = "�ݵ���Ǥʤ��ʤä���";
#else
			muta_desc = "You are no longer manic-depressive.";
#endif

			break;
		case 5: case 6:
			muta_which = MUT_BANISH_ALL;
#ifdef JP
			muta_desc = "�ظ�˶������Ϥ򴶤��ʤ��ʤä���";
#else
			muta_desc = "You no longer feel a terrifying power lurking behind you.";
#endif

			break;
		case 7:
			muta_which = MUT_EAT_LIGHT;
#ifdef JP
			muta_desc = "���������뤤�ȴ����롣";
#else
			muta_desc = "You feel the world's a brighter place.";
#endif

			break;
		case 8: case 9: case 10:
			muta_which = MUT_NORMALITY;
#ifdef JP
			muta_desc = "���̤˴�̯�ʴ��������롣";
#else
			muta_desc = "You feel normally strange.";
#endif

			break;
		case 11:
			muta_which = MUT_WASTING;
#ifdef JP
			muta_desc = "�����ޤ�������¤����ä���";
#else
			muta_desc = "You are cured of the horrible wasting disease!";
#endif

			break;
		case 12:
			muta_which = MUT_ALTER_REALITY;
#ifdef JP
			muta_desc = "���¤������Ƥ��뵤�����롣";
#else
			muta_desc = "You feel like you're trapped in reality.";
#endif

			break;
		case 13: case 14:
			muta_which = MUT_SP_TO_HP;
#ifdef JP
			muta_desc = "��ˡ�μ�����ȯ��˽����ʤ��ʤä���";
#else
			muta_desc = "You are no longer subject to fits of magical healing.";
#endif

			break;
		case 15:
			muta_which = MUT_HP_TO_SP;
#ifdef JP
			muta_desc = "�ˤߤ�ȼ���������Ʋ���ȯ��˽����ʤ��ʤä���";
#else
			muta_desc = "You are no longer subject to fits of painful clarity.";
#endif

			break;
		case 16: case 17:
			muta_which = MUT_ELEC_BODY;
#ifdef JP
			muta_desc = "�Τ����ѡ�������ޤ�ʤ��ʤä���";
#else
			muta_desc = "Electricity stops running through you.";
#endif

			break;
		case 18: case 19:
			muta_which = MUT_FIRE_BODY;
#ifdef JP
			muta_desc = "�Τ������ޤ�ʤ��ʤä���";
#else
			muta_desc = "Your body is no longer enveloped in flames.";
#endif

			break;
		case 20: case 21:
			muta_which = MUT_COLD_BODY;
#ifdef JP
			muta_desc = "�Τ�ɹ����ޤ�ʤ��ʤä���";
#else
			muta_desc = "Your body is no longer enveloped in ices.";
#endif

			break;
		case 22: case 23:
			muta_which = MUT_XTRA_FAT;
#ifdef JP
			muta_desc = "����Ū�ʥ������åȤ�����������";
#else
			muta_desc = "You benefit from a miracle diet!";
#endif

			break;
		case 24: case 25: case 26:
			muta_which = MUT_FLESH_ROT;
#ifdef JP
			muta_desc = "���Τ����Ԥ������µ������ä���";
#else
			muta_desc = "Your flesh is no longer afflicted by a rotting disease!";
#endif

			break;
		case 27: case 28: case 29:
			muta_which = MUT_THICK_SKIN;
#ifdef JP
			muta_desc = "���̤��������ä���";
#else
			muta_desc = "Your warts disappear!";
#endif

			break;
		case 30: case 31: case 32:
			muta_which = MUT_SCALES;
#ifdef JP
			muta_desc = "�ڤ��ä�����";
#else
			muta_desc = "Your scales vanish!";
#endif

			break;
		case 33: case 34: case 35:
			muta_which = MUT_FUR;
#ifdef JP
			muta_desc = "�Ӥ�ȴ���������";
#else
			muta_desc = "You lose your furs.";
#endif

			break;
		case 36: case 37:
			muta_which = MUT_WINGS;
#ifdef JP
			muta_desc = "����α���������������";
#else
			muta_desc = "Your wings fall off.";
#endif

			break;
		case 38:
			muta_which = MUT_VULN_ELEM;
#ifdef JP
			muta_desc = "̵�����ʴ����Ϥʤ��ʤä���";
#else
			muta_desc = "You feel less exposed.";
#endif

			break;
		default:
			muta_which = 0;
		}

		if (muta_which)
		{
			if (p_ptr->mutation & muta_which)
			{
				muta_chosen = TRUE;
			}
		}
		if (muta_chosen == TRUE) break;
	}

	if (!muta_chosen)
	{
		return FALSE;
	}
	else
	{
		change_your_alignment(ALI_LNC, 1);

		msg_print(muta_desc);
		p_ptr->mutation &= ~(muta_which);

		p_ptr->update |= PU_BONUS;
		handle_stuff();
		mutant_regenerate_mod = calc_mutant_regenerate_mod();
		return TRUE;
	}
}


static bool gain_random_grace(int choose_grace)
{
	int     attempts_left = 20;
	cptr    grace_desc = "";
	bool    grace_chosen = FALSE;
	u32b    grace_which = 0;

	if (choose_grace) attempts_left = 1;

	while (attempts_left--)
	{
		switch (choose_grace ? choose_grace : randint1(18))
		{
		case 1:
			grace_which = GRACE_GOOD_LUCK;
#ifdef JP
			grace_desc = "���ῼ���򤤥����餬���ʤ���Ȥ�ޤ���...";
#else
			grace_desc = "There is a benevolent white aura surrounding you...";
#endif

			break;
		case 2: case 3:
			grace_which = GRACE_ATT_ANIMAL;
#ifdef JP
			grace_desc = "ưʪ������դ���褦�ˤʤä���";
#else
			grace_desc = "You start attracting animals.";
#endif

			break;
		case 4:
			grace_which = GRACE_ATT_SERVANT;
#ifdef JP
			grace_desc = "���ͤ�����Ĥ���褦�ˤʤä���";
#else
			grace_desc = "You start attracting servants.";
#endif

			break;
		case 5:
			grace_which = GRACE_ATT_DRAGON;
#ifdef JP
			grace_desc = "�ɥ饴�������Ĥ���褦�ˤʤä���";
#else
			grace_desc = "You start attracting dragons.";
#endif

			break;
		case 6: case 7:
			grace_which = GRACE_REGEN;
#ifdef JP
			grace_desc = "��®�˲������Ϥ᤿��";
#else
			grace_desc = "You start regenerating.";
#endif

			break;
		case 8:
			grace_which = GRACE_HIGH_MELEE;
#ifdef JP
			grace_desc = "���⤬������Τˤʤä���";
#else
			grace_desc = "You feel";
#endif

			break;
		case 9: case 10: case 11:
			grace_which = GRACE_HIGH_MELEE2;
#ifdef JP
			grace_desc = "���⤬������Τˤʤä���";
#else
			grace_desc = "You feel";
#endif

			break;
		case 12:
			grace_which = GRACE_HIGH_AC;
#ifdef JP
			grace_desc = "��������ɤ���褦�ˤʤä���";
#else
			grace_desc = "You feel";
#endif

			break;
		case 13: case 14: case 15:
			grace_which = GRACE_HIGH_AC2;
#ifdef JP
			grace_desc = "��������ɤ���褦�ˤʤä���";
#else
			grace_desc = "You feel";
#endif

			break;
		case 16:
			grace_which = GRACE_HIGH_MAGIC;
#ifdef JP
			grace_desc = "��ʸ�αӾ���������Τˤʤä���";
#else
			grace_desc = "You feel.";
#endif

			break;
		case 17: case 18:
			grace_which = GRACE_HIGH_MAGIC2;
#ifdef JP
			grace_desc = "��ʸ�αӾ���������Τˤʤä���";
#else
			grace_desc = "You feel.";
#endif

			break;
		default:
			grace_which = 0;
		}

		if (grace_which)
		{
			if (!(p_ptr->grace & grace_which))
			{
				grace_chosen = TRUE;
			}
		}
		if (grace_chosen == TRUE) break;
	}

	if (!grace_chosen)
	{
#ifdef JP
		msg_print("�����������ʤ��ä���");
#else
		msg_print("You feel normal.");
#endif

		return FALSE;
	}
	else
	{
#ifdef JP
		msg_print("������������");
#else
		msg_print("You mutate!");
#endif

		msg_print(grace_desc);
		p_ptr->grace |= grace_which;

		change_your_alignment(ALI_GNE, 10);

		p_ptr->update |= PU_BONUS;
		handle_stuff();
		return TRUE;
	}
}


static bool lose_grace(int choose_grace)
{
	int attempts_left = 20;
	cptr grace_desc = "";
	bool grace_chosen = FALSE;
	u32b grace_which = 0;

	if (choose_grace) attempts_left = 1;

	while (attempts_left--)
	{
		switch (choose_grace ? choose_grace : randint1(18))
		{
		case 1:
			grace_which = GRACE_GOOD_LUCK;
#ifdef JP
			grace_desc = "�򤤥�����ϵ����ƾä�����";
#else
			grace_desc = "Your white aura shimmers and fades.";
#endif

			break;
		case 2: case 3:
			grace_which = GRACE_ATT_ANIMAL;
#ifdef JP
			grace_desc = "ưʪ������󤻤ʤ��ʤä���";
#else
			grace_desc = "You stop attracting animals.";
#endif

			break;
		case 4:
			grace_which = GRACE_ATT_SERVANT;
#ifdef JP
			grace_desc = "���ͤ�����󤻤ʤ��ʤä���";
#else
			grace_desc = "You stop attracting servants.";
#endif

			break;
		case 5:
			grace_which = GRACE_ATT_DRAGON;
#ifdef JP
			grace_desc = "�ɥ饴�������󤻤ʤ��ʤä���";
#else
			grace_desc = "You stop attracting dragons.";
#endif

			break;
		case 6: case 7:
			grace_which = GRACE_REGEN;
#ifdef JP
			grace_desc = "��®�˲������ʤ��ʤä���";
#else
			grace_desc = "You stop regenerating.";
#endif

			break;
		case 8:
			grace_which = GRACE_HIGH_MELEE;
#ifdef JP
			grace_desc = "��������Τ�������줿��";
#else
			grace_desc = "You feel";
#endif

			break;
		case 9: case 10: case 11:
			grace_which = GRACE_HIGH_MELEE2;
#ifdef JP
			grace_desc = "��������Τ�������줿��";
#else
			grace_desc = "You feel";
#endif

			break;
		case 12:
			grace_which = GRACE_HIGH_AC;
#ifdef JP
			grace_desc = "�ɸ椬������ä���";
#else
			grace_desc = "You feel";
#endif

			break;
		case 13: case 14: case 15:
			grace_which = GRACE_HIGH_AC2;
#ifdef JP
			grace_desc = "�ɸ椬������ä���";
#else
			grace_desc = "You feel";
#endif

			break;
		case 16:
			grace_which = GRACE_HIGH_MAGIC;
#ifdef JP
			grace_desc = "��ʸ�αӾ������Τ�������줿��";
#else
			grace_desc = "You feel.";
#endif

			break;
		case 17: case 18:
			grace_which = GRACE_HIGH_MAGIC2;
#ifdef JP
			grace_desc = "��ʸ�αӾ������Τ�������줿��";
#else
			grace_desc = "You feel.";
#endif

			break;
		default:
			grace_which = 0;
		}

		if (grace_which)
		{
			if (p_ptr->grace & grace_which)
			{
				grace_chosen = TRUE;
			}
		}
		if (grace_chosen == TRUE) break;
	}

	if (!grace_chosen)
	{
		return FALSE;
	}
	else
	{
		change_your_alignment(ALI_GNE, -10);

		msg_print(grace_desc);
		p_ptr->grace &= ~(grace_which);

		p_ptr->update |= PU_BONUS;
		handle_stuff();
		return TRUE;
	}
}


static bool gain_random_curse(int choose_curse)
{
	int     attempts_left = 20;
	cptr    curse_desc = "";
	bool    curse_chosen = FALSE;
	u32b    curse_which = 0;

	if (choose_curse) attempts_left = 1;

	while (attempts_left--)
	{
		switch (choose_curse ? choose_curse : randint1(18))
		{
		case 1:
			curse_which = CURSE_BAD_LUCK;
#ifdef JP
			curse_desc = "���դ����������������餬���ʤ���Ȥ�ޤ���...";
#else
			curse_desc = "There is a malignant black aura surrounding you...";
#endif

			break;
		case 2: case 3:
			curse_which = CURSE_ATT_ANIMAL;
#ifdef JP
			curse_desc = "ưʪ������դ���褦�ˤʤä���";
#else
			curse_desc = "You start attracting animals.";
#endif

			break;
		case 4:
			curse_which = CURSE_ATT_DEMON;
#ifdef JP
			curse_desc = "���������Ĥ���褦�ˤʤä���";
#else
			curse_desc = "You start attracting demons.";
#endif

			break;
		case 5:
			curse_which = CURSE_ATT_DRAGON;
#ifdef JP
			curse_desc = "�ɥ饴�������Ĥ���褦�ˤʤä���";
#else
			curse_desc = "You start attracting dragons.";
#endif

			break;
		case 6: case 7:
			curse_which = CURSE_SLOW_REGEN;
#ifdef JP
			curse_desc = "�������٤��ʤä���";
#else
			curse_desc = "You regenerating slowly.";
#endif

			break;
		case 8:
			curse_which = CURSE_LOW_MELEE;
#ifdef JP
			curse_desc = "����򳰤��䤹���ʤä���";
#else
			curse_desc = "You feel";
#endif

			break;
		case 9: case 10: case 11:
			curse_which = CURSE_LOW_MELEE2;
#ifdef JP
			curse_desc = "����򳰤��䤹���ʤä���";
#else
			curse_desc = "You feel";
#endif

			break;
		case 12:
			curse_which = CURSE_LOW_AC;
#ifdef JP
			curse_desc = "���������䤹���ʤä���";
#else
			curse_desc = "You feel";
#endif

			break;
		case 13: case 14: case 15:
			curse_which = CURSE_LOW_AC2;
#ifdef JP
			curse_desc = "���������䤹���ʤä���";
#else
			curse_desc = "You feel";
#endif

			break;
		case 16:
			curse_which = CURSE_LOW_MAGIC;
#ifdef JP
			curse_desc = "��ʸ�αӾ����Ԥ��䤹���ʤä���";
#else
			curse_desc = "You feel.";
#endif

			break;
		case 17: case 18:
			curse_which = CURSE_LOW_MAGIC2;
#ifdef JP
			curse_desc = "��ʸ�αӾ����Ԥ��䤹���ʤä���";
#else
			curse_desc = "You feel.";
#endif

			break;
		default:
			curse_which = 0;
		}

		if (curse_which)
		{
			if (!(p_ptr->grace & curse_which))
			{
				curse_chosen = TRUE;
			}
		}
		if (curse_chosen == TRUE) break;
	}

	if (!curse_chosen)
	{
#ifdef JP
		msg_print("�����ϼ����ʤ��ä���");
#else
		msg_print("You feel normal.");
#endif

		return FALSE;
	}
	else
	{
#ifdef JP
		msg_print("�������������");
#else
		msg_print("You cursed!");
#endif

		msg_print(curse_desc);
		p_ptr->grace |= curse_which;

		change_your_alignment(ALI_GNE, -10);

		p_ptr->update |= PU_BONUS;
		handle_stuff();
		return TRUE;
	}
}


static bool lose_curse(int choose_curse)
{
	int attempts_left = 20;
	cptr curse_desc = "";
	bool curse_chosen = FALSE;
	u32b curse_which = 0;

	if (choose_curse) attempts_left = 1;

	while (attempts_left--)
	{
		switch (choose_curse ? choose_curse : randint1(18))
		{
		case 1:
			curse_which = CURSE_BAD_LUCK;
#ifdef JP
			curse_desc = "����������ϱ������ƾä�����";
#else
			curse_desc = "Your black aura swirls and fades.";
#endif

			break;
		case 2: case 3:
			curse_which = CURSE_ATT_ANIMAL;
#ifdef JP
			curse_desc = "ưʪ������󤻤ʤ��ʤä���";
#else
			curse_desc = "You stop attracting animals.";
#endif

			break;
		case 4:
			curse_which = CURSE_ATT_DEMON;
#ifdef JP
			curse_desc = "���������󤻤ʤ��ʤä���";
#else
			curse_desc = "You stop attracting demons.";
#endif

			break;
		case 5:
			curse_which = CURSE_ATT_DRAGON;
#ifdef JP
			curse_desc = "�ɥ饴�������󤻤ʤ��ʤä���";
#else
			curse_desc = "You stop attracting dragons.";
#endif

			break;
		case 6: case 7:
			curse_which = CURSE_SLOW_REGEN;
#ifdef JP
			curse_desc = "���̤˲�������褦�ˤʤä���";
#else
			curse_desc = "You start regenerating normaly.";
#endif

			break;
		case 8:
			curse_which = CURSE_LOW_MELEE;
#ifdef JP
			curse_desc = "������Ԥ��ˤ����ʤä���";
#else
			curse_desc = "You feel";
#endif

			break;
		case 9: case 10: case 11:
			curse_which = CURSE_LOW_MELEE2;
#ifdef JP
			curse_desc = "������Ԥ��ˤ����ʤä���";
#else
			curse_desc = "You feel";
#endif

			break;
		case 12:
			curse_which = CURSE_LOW_AC;
#ifdef JP
			curse_desc = "�ɸ椬������ä���";
#else
			curse_desc = "You feel";
#endif

			break;
		case 13: case 14: case 15:
			curse_which = CURSE_LOW_AC2;
#ifdef JP
			curse_desc = "�ɸ椬������ä���";
#else
			curse_desc = "You feel";
#endif

			break;
		case 16:
			curse_which = CURSE_LOW_MAGIC;
#ifdef JP
			curse_desc = "��ʸ�αӾ������Τˤʤä���";
#else
			curse_desc = "You feel.";
#endif

			break;
		case 17: case 18:
			curse_which = CURSE_LOW_MAGIC2;
#ifdef JP
			curse_desc = "��ʸ�αӾ������Τˤʤä���";
#else
			curse_desc = "You feel.";
#endif

			break;
		default:
			curse_which = 0;
		}

		if (curse_which)
		{
			if (p_ptr->grace & curse_which)
			{
				curse_chosen = TRUE;
			}
		}
		if (curse_chosen == TRUE) break;
	}

	if (!curse_chosen)
	{
		return FALSE;
	}
	else
	{
		change_your_alignment(ALI_GNE, 10);

		msg_print(curse_desc);
		p_ptr->grace &= ~(curse_which);

		p_ptr->update |= PU_BONUS;
		handle_stuff();
		return TRUE;
	}
}

bool do_grace(int choose, int gain_or_lose)
{
	if (gain_or_lose == 1) return gain_random_grace(choose);
	else if (gain_or_lose == 2) return lose_curse(choose);

	else if (!p_ptr->grace || one_in_(2)) return gain_random_grace(choose);
	else return lose_curse(choose);
}

bool do_curse(int choose, int gain_or_lose)
{
	if (gain_or_lose == 1) return gain_random_curse(choose);
	else if (gain_or_lose == 2) return lose_grace(choose);

	else if (!p_ptr->grace || one_in_(2)) return gain_random_curse(choose);
	else return lose_grace(choose);
}

bool gain_gift(int choose_gift)
{
	cptr    gift_desc = "";
	bool    gift_chosen = FALSE;
	u32b    gift_which = 0;

	switch (choose_gift)
	{
	case 1:
		gift_which = GIFT_TAROT;
#ifdef JP
		gift_desc = "���ʤ��ϥ���åȥ����ɤ��Ϥ򴶤���褦�ˤʤä���";
#else
		gift_desc = "You begin feeling the power of tarot cards.";
#endif

		break;
	default:
		gift_which = 0;
	}

	if (gift_which)
	{
		if (!(p_ptr->gift & gift_which))
		{
			gift_chosen = TRUE;
		}
	}

	if (!gift_chosen)
	{
#ifdef JP
		msg_print("������ǽ�Ϥ������ʤ��ä���");
#else
		msg_print("You feel normal.");
#endif

		return FALSE;
	}
	else
	{
		msg_print(gift_desc);
		p_ptr->gift |= gift_which;

		p_ptr->update |= PU_BONUS;
		handle_stuff();
		return TRUE;
	}
}


bool lose_gift(int choose_gift)
{
	cptr gift_desc = "";
	bool gift_chosen = FALSE;
	u32b gift_which = 0;

	switch (choose_gift)
	{
	case 1:
		gift_which = GIFT_TAROT;
#ifdef JP
		gift_desc = "����åȥ����ɤ��Ϥ򴶤��ʤ��ʤä���";
#else
		gift_desc = "You stop feeling the power of tarot cards.";
#endif

		break;
	default:
		gift_which = 0;
	}

	if (gift_which)
	{
		if (p_ptr->gift & gift_which)
		{
			gift_chosen = TRUE;
		}
	}

	if (!gift_chosen)
	{
		return FALSE;
	}
	else
	{
		msg_print(gift_desc);
		p_ptr->gift &= ~(gift_which);

		p_ptr->update |= PU_BONUS;
		handle_stuff();
		return TRUE;
	}
}


void dump_mutations(FILE *OutFile)
{
	if (!OutFile) return;

	if (p_ptr->mutation)
	{
		if (p_ptr->mutation & MUT_ELEM_UNSTABLE)
		{
#ifdef JP
			fprintf(OutFile, " ���ʤ��θ�ͭ������Ȥ��䤨���Ѳ����Ƥ��롣\n");
#else
			fprintf(OutFile, " Your fixed element is constantly changing.\n");
#endif

		}
		if (p_ptr->mutation & MUT_ELEM_MULTI)
		{
#ifdef JP
			fprintf(OutFile, " ���ʤ��Υ�����Ȥ��䤨���Ѳ����Ƥ��롣\n");
#else
			fprintf(OutFile, " Your fixed element is constantly changing.\n");
#endif

		}
		if (p_ptr->mutation & MUT_SPEED_FLUX)
		{
#ifdef JP
			fprintf(OutFile, " ���ʤ��ϥ�������᤯ư�������٤�ư�����ꤹ�롣\n");
#else
			fprintf(OutFile, " You move faster or slower randomly.\n");
#endif

		}
		if (p_ptr->mutation & MUT_BANISH_ALL)
		{
#ifdef JP
			fprintf(OutFile, " ���ʤ��ϻ����᤯�Υ�󥹥�������Ǥ����롣\n");
#else
			fprintf(OutFile, " You sometimes cause nearby creatures to vanish.\n");
#endif

		}
		if (p_ptr->mutation & MUT_EAT_LIGHT)
		{
#ifdef JP
			fprintf(OutFile, " ���ʤ��ϻ������Ϥθ���ۼ����Ʊ��ܤˤ��롣\n");
#else
			fprintf(OutFile, " You sometimes feed off of the light around you.\n");
#endif

		}
		if (p_ptr->mutation & MUT_NORMALITY)
		{
#ifdef JP
			fprintf(OutFile, " ���ʤ����Ѱۤ��Ƥ��������������Ƥ��Ƥ��롣\n");
#else
			fprintf(OutFile, " You may be mutated, but you're recovering.\n");
#endif

		}
		if (p_ptr->mutation & MUT_WASTING)
		{
#ifdef JP
			fprintf(OutFile, " ���ʤ��Ͽ�夹�붲�����µ��ˤ����äƤ��롣\n");
#else
			fprintf(OutFile, " You have a horrible wasting disease.\n");
#endif

		}
		if (p_ptr->mutation & MUT_ALTER_REALITY)
		{
#ifdef JP
			fprintf(OutFile, " ���ʤ��μ���θ��¤��Ĥ�Ǥ��롣\n");
#else
			fprintf(OutFile, " Reality around you is distorted.\n");
#endif

		}
		if (p_ptr->mutation & MUT_SP_TO_HP)
		{
#ifdef JP
			fprintf(OutFile, " ���ʤ��ϻ����줬�����ˤɤä�ή��롣\n");
#else
			fprintf(OutFile, " Your blood sometimes rushes to your muscles.\n");
#endif

		}
		if (p_ptr->mutation & MUT_HP_TO_SP)
		{
#ifdef JP
			fprintf(OutFile, " ���ʤ��ϻ���Ƭ�˷줬�ɤä�ή��롣\n");
#else
			fprintf(OutFile, " Your blood sometimes rushes to your head.\n");
#endif

		}
		if (p_ptr->mutation & MUT_ELEC_BODY)
		{
#ifdef JP
			fprintf(OutFile, " ���ʤ����Τϥ��ѡ����ˤĤĤޤ�Ƥ��롣\n");
#else
			fprintf(OutFile, " Electricity is running through your veins.\n");
#endif

		}
		if (p_ptr->mutation & MUT_FIRE_BODY)
		{
#ifdef JP
			fprintf(OutFile, " ���ʤ����Τϱ�ˤĤĤޤ�Ƥ��롣\n");
#else
			fprintf(OutFile, " Your body is enveloped in flames.\n");
#endif

		}
		if (p_ptr->mutation & MUT_COLD_BODY)
		{
#ifdef JP
			fprintf(OutFile, " ���ʤ����Τ�ɹ�ˤĤĤޤ�Ƥ��롣\n");
#else
			fprintf(OutFile, " Your body is enveloped in flames.\n");
#endif

		}
		if (p_ptr->mutation & MUT_XTRA_FAT)
		{
#ifdef JP
			fprintf(OutFile, " ���ʤ��϶�ü�����äƤ��롣(�ѵ�+2,���ԡ���-2)\n");
#else
			fprintf(OutFile, " You are extremely fat (+2 CON, -2 speed).\n");
#endif

		}
		if (p_ptr->mutation & MUT_FLESH_ROT)
		{
#ifdef JP
			fprintf(OutFile, " ���ʤ������Τ����Ԥ��Ƥ��롣(�ѵ�-2,̥��-1)\n");
#else
			fprintf(OutFile, " Your flesh is rotting (-2 CON, -1 CHR).\n");
#endif

		}
		if (p_ptr->mutation & MUT_THICK_SKIN)
		{
#ifdef JP
			fprintf(OutFile, " ���ʤ���ȩ��ʬ��������ˤʤäƤ��롣(̥��-2, AC+10)\n");
#else
			fprintf(OutFile, " Your skin is covered with warts (-2 CHR, +10 AC).\n");
#endif

		}
		if (p_ptr->mutation & MUT_SCALES)
		{
#ifdef JP
			fprintf(OutFile, " ���ʤ���ȩ���ڤˤʤäƤ��롣(̥��-1, AC+10)\n");
#else
			fprintf(OutFile, " Your skin has turned into scales (-1 CHR, +10 AC).\n");
#endif

		}
		if (p_ptr->mutation & MUT_FUR)
		{
#ifdef JP
			fprintf(OutFile, " ���ʤ���ȩ�������ʤ���Ƥ��롣(AC+5)\n");
#else
			fprintf(OutFile, " Your skin is made of steel (+5 AC).\n");
#endif

		}
		if (p_ptr->mutation & MUT_WINGS)
		{
#ifdef JP
			fprintf(OutFile, " ���ʤ��ϱ�����äƤ��롣\n");
#else
			fprintf(OutFile, " You have wings.\n");
#endif

		}
		if (p_ptr->mutation & MUT_VULN_ELEM)
		{
#ifdef JP
			fprintf(OutFile, " ���ʤ��ϸ��Ǥι���˼夤��\n");
#else
			fprintf(OutFile, " You are susceptible to damage from the elements.\n");
#endif

		}
	}

	if (p_ptr->grace)
	{
		if (p_ptr->grace & GRACE_GOOD_LUCK)
		{
#ifdef JP
			fprintf(OutFile, " ���ʤ����򤤥�����ˤĤĤޤ�Ƥ��롣\n");
#else
			fprintf(OutFile, " There is a white aura surrounding you.\n");
#endif

		}
		if (p_ptr->grace & GRACE_ATT_ANIMAL)
		{
#ifdef JP
			fprintf(OutFile, " ���ʤ���ưʪ������Ĥ��롣\n");
#else
			fprintf(OutFile, " You attract animals.\n");
#endif

		}
		if (p_ptr->grace & GRACE_ATT_SERVANT)
		{
#ifdef JP
			fprintf(OutFile, " ���ʤ��ϲ��ͤ�����Ĥ��롣\n");
#else
			fprintf(OutFile, " You attract servants.\n");
#endif

		}
		if (p_ptr->grace & GRACE_ATT_DRAGON)
		{
#ifdef JP
			fprintf(OutFile, " ���ʤ��ϥɥ饴�������Ĥ��롣\n");
#else
			fprintf(OutFile, " You attract dragons.\n");
#endif

		}
		if (p_ptr->grace & GRACE_REGEN)
		{
#ifdef JP
			fprintf(OutFile, " ���ʤ��ϵ�®�˲������롣\n");
#else
			fprintf(OutFile, " You are regenerating.\n");
#endif

		}
		if (p_ptr->grace & (GRACE_HIGH_MELEE | GRACE_HIGH_MELEE2))
		{
#ifdef JP
			fprintf(OutFile, " ���ʤ��Ϲ����̿�椵���䤹����\n");
#else
			fprintf(OutFile, " You are regenerating.\n");
#endif

		}
		if (p_ptr->grace & (GRACE_HIGH_AC | GRACE_HIGH_AC2))
		{
#ifdef JP
			fprintf(OutFile, " ���ʤ��Ϲ���򤫤路�䤹����\n");
#else
			fprintf(OutFile, " You are regenerating.\n");
#endif

		}
		if (p_ptr->grace & (GRACE_HIGH_MAGIC | GRACE_HIGH_MAGIC2))
		{
#ifdef JP
			fprintf(OutFile, " ���ʤ�����ˡ�����������䤹����\n");
#else
			fprintf(OutFile, " You are subject to success spellcasting.\n");
#endif

		}

		if (p_ptr->grace & CURSE_BAD_LUCK)
		{
#ifdef JP
			fprintf(OutFile, " ���ʤ��Ϲ���������ˤĤĤޤ�Ƥ��롣\n");
#else
			fprintf(OutFile, " There is a black aura surrounding you.\n");
#endif

		}
		if (p_ptr->grace & CURSE_ATT_ANIMAL)
		{
#ifdef JP
			fprintf(OutFile, " ���ʤ���ưʪ������Ĥ��롣\n");
#else
			fprintf(OutFile, " You attract animals.\n");
#endif

		}
		if (p_ptr->grace & CURSE_ATT_DEMON)
		{
#ifdef JP
			fprintf(OutFile, " ���ʤ��ϰ��������Ĥ��롣\n");
#else
			fprintf(OutFile, " You attract demons.\n");
#endif

		}
		if (p_ptr->grace & CURSE_ATT_DRAGON)
		{
#ifdef JP
			fprintf(OutFile, " ���ʤ��ϥɥ饴�������Ĥ��롣\n");
#else
			fprintf(OutFile, " You attract dragons.\n");
#endif

		}
		if (p_ptr->grace & CURSE_SLOW_REGEN)
		{
#ifdef JP
			fprintf(OutFile, " ���ʤ��β����Ϥ������٤���\n");
#else
			fprintf(OutFile, " You regenerate slowly.\n");
#endif

		}
		if (p_ptr->grace & (CURSE_LOW_MELEE | CURSE_LOW_MELEE2))
		{
#ifdef JP
			fprintf(OutFile, " ���ʤ��Ϲ���򳰤��䤹����\n");
#else
			fprintf(OutFile, " You are subject to miss blows.\n");
#endif

		}
		if (p_ptr->grace & (CURSE_LOW_AC | CURSE_LOW_AC2))
		{
#ifdef JP
			fprintf(OutFile, " ���ʤ��Ϲ��������䤹����\n");
#else
			fprintf(OutFile, " You are subject to be hit.\n");
#endif

		}
		if (p_ptr->grace & (CURSE_LOW_MAGIC | CURSE_LOW_MAGIC2))
		{
#ifdef JP
			fprintf(OutFile, " ���ʤ�����ˡ���Ԥ��䤹����\n");
#else
			fprintf(OutFile, " You are subject to fail spellcasting.\n");
#endif

		}
	}

	if (p_ptr->gift)
	{
		if (p_ptr->gift & GIFT_TAROT)
		{
#ifdef JP
			fprintf(OutFile, " ���ʤ��ϥ���åȥ����ɤ��Ϥ򴶤��롣\n");
#else
			fprintf(OutFile, " You feel the power of tarot cards.\n");
#endif

		}
	}
}


/*
 * List mutations we have...
 */
void do_cmd_knowledge_mutations(void)
{
	FILE *fff;
	char file_name[1024];

	/* Open a new file */
	fff = my_fopen_temp(file_name, 1024);

	/* Dump the mutations to file */
	if (fff) dump_mutations(fff);

	/* Close the file */
	my_fclose(fff);

	/* Display the file contents */
#ifdef JP
	show_file(TRUE, file_name, "�����Ѱ�", 0, 0);
#else
	show_file(TRUE, file_name, "Mutations", 0, 0);
#endif


	/* Remove the file */
	fd_kill(file_name);
}


int count_bits(u32b x)
{
	int n = 0;

	if (x) do
	{
		n++;
	}
	while (0 != (x = x&(x-1)));

	return (n);
}


static int count_mutations(void)
{
	return (count_bits(p_ptr->mutation));
}


/*
 * Return the modifier to the regeneration rate
 * (in percent)
 */
int calc_mutant_regenerate_mod(void)
{
	int regen;
	int mod = 10;
	int count = count_mutations();

	/*
	 * Beastman get 10 "free" mutations and
	 * only 5% decrease per additional mutation
	 */

	/* No negative modifier */
	if (count <= 0) return 100;

	regen = 100 - count * mod;

	/* Max. 90% decrease in regeneration speed */
	if (regen < 10) regen = 10;

	return (regen);
}


