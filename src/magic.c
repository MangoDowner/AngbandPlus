#include "angband.h"

#define MN_CURE_LITE         0
#define MN_BLESSING          1
#define MN_INERTIA           2
#define MN_DEMON_AURA        3
#define MN_STINKING          4
#define MN_EXTRA_MIGHT       5
#define MN_CURSE_WEAPON      6
#define MN_DETECT_EVIL       7
#define MN_RES_DARK          8
#define MN_ICE_ARMOR         9
#define MN_CURE_SERIOUS      10
#define MN_TELEPORT_TO       11
#define MN_INHAIL_POTION     12
#define MN_VAMPIRIC_MIST     13
#define MN_RUNESWORD         14
#define MN_RES_NETHER        15
#define MN_CONFUSION         16
#define MN_BUILD_UP          17
#define MN_VANISH            18
#define MN_ANTI_TELEPORT     19
#define MN_ANTI_MULTIPLY     20
#define MN_CURE_CRITICAL     21
#define MN_BERSERK           22
#define MN_RECHARGING        23
#define MN_DRAIN_CURSE       24
#define MN_RESTORE_LIFE      25
#define MN_VAMPIRIC_ATTACK   26
#define MN_STUN              27
#define MN_SHADOW_MOVE       28
#define MN_EYE_FOR_EYE       29
#define MN_ANTI_MAGIC        30
#define MN_REVENGE_SENTENCE  31


static void stop_hex_spell_effect(int spell)
{
	switch(spell)
	{
	case MN_BLESSING:
		if (!p_ptr->blessed)
		{
#ifdef JP
			msg_print("���ʵ�ʬ���ä���������");
#else
			msg_print("The prayer has expired.");
#endif
		}
		break;
	case MN_BERSERK:
		if (!p_ptr->shero)
		{
#ifdef JP
			msg_print("���ڤʵ��������ä���������");
#else
			msg_print("You feel less Berserk.");
#endif
		}
		break;
	case MN_RES_DARK:
#ifdef JP
		msg_print("�Ź��ؤ����������줿�������롣");
#else
		msg_print("You feel less resistant to dark.");
#endif
		break;
	case MN_RES_NETHER:
#ifdef JP
		msg_print("�Ϲ��ؤ����������줿�������롣");
#else
		msg_print("You feel less resistant to nether.");
#endif
		break;
	case MN_DEMON_AURA:
#ifdef JP
		msg_print("��Υ����餬�ä���ä���");
#else
		msg_print("Fiery aura disappeared.");
#endif
		break;
	case MN_ICE_ARMOR:
#ifdef JP
		msg_print("ɹ�γ����ä���ä���");
#else
		msg_print("Ice armor disappeared.");
#endif
		break;
	case MN_CONFUSION:
#ifdef JP
		msg_print("��ε������ʤ��ʤä���");
#else
		msg_print("Brightness on your hands disappeard.");
#endif
		break;
	case MN_RUNESWORD:
#ifdef JP
		msg_print("���ε������ä���ä���");
#else
		msg_format("Brightness of weapon%s disappeared.", (is_two_handed()) ? "s" : "");
#endif
		break;
	case MN_VAMPIRIC_ATTACK:
#ifdef JP
		msg_print("���γ�˾���ä���ä���");
#else
		msg_format("Thirsty of weapon%s disappeared.", (is_two_handed()) ? "s" : "");
#endif
		break;
	}
}


bool stop_hex_spell_all(void)
{
	int i;

	for (i = 0; i < 32; i++)
	{
		u32b f = 1L << i;
		if (p_ptr->keep_spells & f) stop_hex_spell_effect(i);
	}

	p_ptr->keep_magic = 0;
	p_ptr->keep_spells = 0;

#ifdef JP
	msg_print("��ʸ�αӾ������Ǥ�����");
#else
	msg_print("You stopped casting all spells.");
#endif

	/* Redraw status */
	p_ptr->update |= (PU_BONUS | PU_HP | PU_MANA | PU_SPELLS);
	p_ptr->redraw |= (PR_EXTRA | PR_HP | PR_MANA);

	return TRUE;
}

bool stop_hex_spell(void)
{
	int i;
	char choice;
	char out_val[160];
	bool flag = FALSE;
	int y = 1;
	int x = 20;
	int sp[BM_MAX_KEEP];

	if (!p_ptr->keep_magic)
	{
#ifdef JP
		msg_print("��ʸ��Ӿ����Ƥ��ޤ���");
#else
		msg_print("You are casting no spell.");
#endif
		return FALSE;
	}

	/* Stop all spells */
	else if (p_ptr->keep_magic == 1)
	{
		return stop_hex_spell_all();
	}
	else
	{
#ifdef JP
		strnfmt(out_val, 78, "�ɤμ�ʸ�αӾ�����ޤ�����(��ʸ %c-%c, 'l'����, ESC)",
			I2A(0), I2A(p_ptr->keep_magic - 1));
#else
		strnfmt(out_val, 78, "Which spell do you stop casting? (Spell %c-%c, 'l' to all, ESC)",
			I2A(0), I2A(p_ptr->keep_magic - 1));
#endif

		screen_save();

		while (!flag)
		{
			int n = 0;
			Term_erase(x, y, 255);
			prt("     ̾��", y, x + 5);
			for (i = 0; i < 32; i++)
			{
				u32b f = 1L << i;
				if (p_ptr->keep_spells & f)
				{
					Term_erase(x, y + n + 1, 255);
					put_str(format("%c)  %s", I2A(n), spell_names[REALM_HEX-1][i]), y + n + 1, x + 2);
					sp[n++] = i;
				}
			}

			if (!get_com(out_val, &choice)) break;
			if (isupper(choice)) choice = tolower(choice);

			if (choice == 'l')	/* All */
			{
				screen_load();
				return stop_hex_spell_all();
			}
			if ((choice < I2A(0)) || (choice > I2A(p_ptr->keep_magic - 1))) continue;
			flag = TRUE;
		}
	}

	screen_load();

	if (flag)
	{
		int n = sp[A2I(choice)];

		stop_hex_spell_effect(n);
		p_ptr->keep_spells &= ~(1L << n);
		p_ptr->keep_magic--;
	}

	/* Redraw status */
	p_ptr->update |= (PU_BONUS | PU_HP | PU_MANA | PU_SPELLS);
	p_ptr->redraw |= (PR_EXTRA | PR_HP | PR_MANA);

	return flag;
}


void upkeep_hex_spell(void)
{
	int n, i, sp[BM_MAX_KEEP];
	int cost = 0;
	int rate = (!p_ptr->dec_mana) ? 3 : 4;

	if (p_ptr->realm1 != REALM_HEX) return;
	if (!p_ptr->keep_magic) return;

	for (n = 0, i = 0; i < 32; i++)
	{
		u32b f = 1L << i;

		if (p_ptr->keep_spells & f)
		{
			cost += MAX(1, mp_ptr->info[REALM_HEX-1][i].smana / rate) + n;
			sp[n++] = i;
		}
	}

	if (p_ptr->csp < cost)
	{
		(void) stop_hex_spell_all();
		return;
	}

	p_ptr->csp -= cost;

	for (i = 0; i < p_ptr->keep_magic; i++)
	{
		switch(sp[i])
		{
		case MN_CURE_LITE:	/* Cure light */
			(void)hp_player(damroll(1, 10));
			(void)set_cut(p_ptr->cut - 10);
			sound(SOUND_HEAL);
			break;
		case MN_INERTIA: /* Inertia */
			(void)project_hack(GF_OLD_SLOW, (p_ptr->lev * 2));
			break;
		case MN_STINKING: /* Stinking */
			(void)project_hack(GF_POIS, randint1((p_ptr->lev / 2) + 5));
			break;
		case MN_CURE_SERIOUS: /* Cure serious */
			(void)hp_player(damroll(2, 10));
			(void)set_cut((p_ptr->cut / 2) - 10);
			sound(SOUND_HEAL);
			break;
		case MN_VAMPIRIC_MIST: /* Vampiric */
			(void)project_hack(GF_OLD_DRAIN, randint1((p_ptr->lev / 2) + 5));
			break;
		case MN_VANISH:
			project_hack(GF_AWAY_ALL, p_ptr->lev);
			break;
		case MN_DRAIN_CURSE: /* Drain curse */
			break;
		case MN_CURE_CRITICAL: /* Cure critical */
			(void)hp_player(damroll(4, 10));
			(void)set_stun(0);
			(void)set_cut(0);
			(void)set_poisoned(0);
			sound(SOUND_HEAL);
			break;
		case MN_RESTORE_LIFE: /* Restore Life */
			{
				bool flag = FALSE;
				int d = (p_ptr->max_exp - p_ptr->exp);
				int r = (p_ptr->exp / 15);
				int i;

				if (d > 0)
				{
					if (d < r)
						p_ptr->exp = p_ptr->max_exp;
					else
						p_ptr->exp += r;

					/* Check the experience */
					check_experience();

					flag = TRUE;
				}
				for (i = A_STR; i < A_MAX; i ++)
				{
					if (p_ptr->stat_cur[i] < p_ptr->stat_max[i])
					{
						if (p_ptr->stat_cur[i] < 18)
							p_ptr->stat_cur[i]++;
						else
							p_ptr->stat_cur[i] += 10;

						if (p_ptr->stat_cur[i] > p_ptr->stat_max[i])
							p_ptr->stat_cur[i] = p_ptr->stat_max[i];

						/* Recalculate bonuses */
						p_ptr->update |= (PU_BONUS);

						flag = TRUE;
					}
				}

				if (!flag)
				{
#ifdef JP
					msg_format("%s�μ�ʸ�αӾ����᤿��", spell_names[REALM_HEX-1][MN_RESTORE_LIFE]);
#else
					msg_format("Finish casting '%^s'.", spell_names[REALM_HEX-1][MN_RESTORE_LIFE]);
#endif
					p_ptr->keep_magic--;
					p_ptr->keep_spells &= ~(MS_RESTORE_LIFE);

					/* Redraw status */
					p_ptr->update |= (PU_BONUS | PU_HP | PU_MANA | PU_SPELLS);
					p_ptr->redraw |= (PR_EXTRA);
				}
			}
			break;
		case MN_STUN: /* Stun */
			(void)stun_monsters(p_ptr->lev * 2);
			break;
		}
	}

	/* Redraw */
	p_ptr->redraw |= (PR_MANA);
}

bool item_tester_hook_cursed(object_type *o_ptr)
{
	u32b f1, f2, f3;

	object_flags(o_ptr, &f1, &f2, &f3);

	if (cursed_p(o_ptr)) return TRUE;

	return FALSE;
}

static bool item_tester_hook_weapon_except_bow(object_type *o_ptr)
{
	switch (o_ptr->tval)
	{
		case TV_SWORD:
		case TV_HAFTED:
		case TV_POLEARM:
		case TV_DIGGING:
		{
			return (TRUE);
		}
	}

	return (FALSE);
}


bool keeping_hex_spell_fully(void)
{
	int k_max = 0;

	k_max = (p_ptr->lev / 15) + 1;

	/* Paranoia */
	k_max = MIN(k_max, BM_MAX_KEEP);

	if (p_ptr->keep_magic < k_max) return FALSE;

	return TRUE;
}

bool cast_hex_spell(int spell)
{
	int i;
	int	dir;
	int plev = p_ptr->lev;
	int x, y;
	int add = TRUE;
	u32b f = 1L << spell;

	if (p_ptr->keep_spells & f)
	{
#ifdef JP
		msg_print("���μ�ʸ�Ϥ��Ǥ˱Ӿ������");
#else
		msg_print("You are already casting it.");
#endif
		return FALSE;
	}

	if (keeping_hex_spell_fully())
	{
#ifdef JP
		msg_print("����ʾ�Ʊ���˼�ʸ�򾧤��뤳�ȤϤǤ��ʤ���");
#else
		msg_print("Can not cast any more spell at sametime.");
#endif
		if (p_ptr->lev < 35)
			return FALSE;
		else /* Continue casting spells */
		{
			if (!stop_hex_spell()) return FALSE;
		}
	}

	switch (spell)
	{
	case MN_RECHARGING:	/* Mana Recharging */
		(void)recharge(plev * 2);
		add = FALSE;
		break;
	case MN_DETECT_EVIL: /* Detect evil */
		(void)detect_monsters_evil(MAX_SIGHT);
		break;
	case MN_TELEPORT_TO:	/* Teleport to */
		{
			char t_name[80];
			monster_type *m_ptr;
			monster_race *r_ptr;

			add = FALSE;

			if (!target_set(TARGET_KILL)) return FALSE;
			if (!cave[target_row][target_col].m_idx) break;
			if (!player_has_los_bold(target_row, target_col)) break;

			m_ptr = &m_list[cave[target_row][target_col].m_idx];
			r_ptr = &r_info[m_ptr->r_idx];
			monster_desc(t_name, m_ptr, 0);

			if (r_ptr->flags3 & (RF3_RES_TELE))
			{
				if (r_ptr->flags1 & (RF1_UNIQUE))
				{
					r_ptr->r_flags3 |= RF3_RES_TELE;
#ifdef JP
					msg_format("%s�ˤϸ��̤��ʤ��ä���", t_name);
#else
					msg_format("%s is unaffected!", t_name);
#endif
				break;
				}
				else if (r_ptr->level > randint1(100))
				{
					r_ptr->r_flags3 |= RF3_RES_TELE;
#ifdef JP
					msg_format("%s�ˤ����������롪", t_name);
#else
					msg_format("%s resists!", t_name);
#endif
				break;
				}
			}
#ifdef JP
			msg_format("%s������ᤷ����", t_name);
#else
			msg_format("You command %s to return.", t_name);
#endif
			teleport_to_player(cave[target_row][target_col].m_idx, 100);
			break;
		}
	case MN_INHAIL_POTION:	/* Inhail Potion */
		p_ptr->keep_spells |= (MS_INHAIL_POTION);
		do_cmd_quaff_potion();
		p_ptr->keep_spells &= ~(MS_INHAIL_POTION);
		add = FALSE;
		break;
	case MN_VANISH:	/* Teleport away monsters */
		break;
	case MN_DRAIN_CURSE:	/* Drain curse */
		{
			int item;
			char *s, *q;
			u32b f1, f2, f3;
			object_type *o_ptr;

			item_tester_hook = item_tester_hook_cursed;
#ifdef JP
			q = "�ɤ������ʤ���ۼ����ޤ�����";
			s = "����줿�����ƥ���������Ƥ��ʤ���";
#else
			q = "Which cursed equipment do you drain mana from?";
			s = "You have no cursed equipment.";
#endif

			if (!get_item(&item, q, s, (USE_EQUIP))) return FALSE;

			o_ptr = &inventory[item];
			object_flags(o_ptr, &f1, &f2, &f3);

			p_ptr->csp += (p_ptr->lev / 5) + randint1(p_ptr->lev / 5);
			if (f3 & TR3_TY_CURSE) p_ptr->csp += randint1(5);
			if (p_ptr->csp > p_ptr->msp) p_ptr->csp = p_ptr->msp;

			if (f3 & TR3_PERMA_CURSE)
			{
				/* Nothing */
			}
			else if (f3 & TR3_HEAVY_CURSE)
			{
				if (one_in_(7))
				{
#ifdef JP
					msg_print("���������Ƶۤ���ä���");
#else
					msg_print("Heavy curse vanished away.");
#endif
					o_ptr->ident &= ~(IDENT_CURSED);
					o_ptr->art_flags3 &= ~(TR3_CURSED | TR3_HEAVY_CURSE);
				}
			}
			else if ((o_ptr->ident & (IDENT_CURSED)) && one_in_(3))
			{
#ifdef JP
				msg_print("���������Ƶۤ���ä���");
#else
				msg_print("Curse vanished away.");
#endif
				o_ptr->ident &= ~(IDENT_CURSED);
				o_ptr->art_flags3 &= ~(TR3_CURSED);
			}
		}
		add = FALSE;
		break;
	case MN_CURSE_WEAPON:	/* Curse weapon */
		{
			int item;
			char *q, *s;
			char o_name[MAX_NLEN];
			object_type *o_ptr;
			u32b f1, f2, f3;

			item_tester_hook = item_tester_hook_weapon_except_bow;
#ifdef JP
			q = "�ɤ������ޤ�����";
			s = "�����������Ƥ��ʤ���";
#else
			q = "Which weapon do you curse?";
			s = "You wield no weapons.";
#endif

			if (!get_item(&item, q, s, (USE_EQUIP))) return FALSE;

			o_ptr = &inventory[item];
			object_desc(o_name, o_ptr, OD_NAME_ONLY);
			object_flags(o_ptr, &f1, &f2, &f3);

#ifdef JP
			if (!get_check(format("������ %s ������ޤ�����", o_name))) return FALSE;
#else
			if (!get_check(format("Do you curse %s, really��", o_name))) return FALSE;
#endif

			if (!one_in_(3) &&
				(artifact_p(o_ptr) || (o_ptr->art_name) || (f3 & TR3_BLESSED)))
			{
#ifdef JP
				msg_format("%s �ϼ�����ķ���֤�����", o_name);
#else
				msg_format("%s resists the effect.", o_name);
#endif
				if (one_in_(3))
				{
					if (o_ptr->to_d > 0)
					{
						o_ptr->to_d -= randint1(3) % 2;
						if (o_ptr->to_d < 0) o_ptr->to_d = 0;
					}
					if (o_ptr->to_h > 0)
					{
						o_ptr->to_h -= randint1(3) % 2;
						if (o_ptr->to_h < 0) o_ptr->to_h = 0;
					}
					if (o_ptr->to_a > 0)
					{
						o_ptr->to_a -= randint1(3) % 2;
						if (o_ptr->to_a < 0) o_ptr->to_a = 0;
					}
#ifdef JP
					msg_format("%s ���������Ƥ��ޤä���", o_name);
#else
					msg_format("Your %s was disenchanted!", o_name);
#endif
				}
			}
			else
			{
#ifdef JP
				msg_format("���ݤΰŹ������餬���ʤ���%s����߹������", o_name);
#else
				msg_format("A terrible black aura blasts your %s!", o_name);
#endif
				o_ptr->ident |= (IDENT_CURSED);
				o_ptr->art_flags3 |= (TR3_CURSED);

				if (artifact_p(o_ptr) || ego_item_p(o_ptr) || o_ptr->art_name)
				{

					if (one_in_(3)) o_ptr->art_flags3 |= (TR3_HEAVY_CURSE);
					if (one_in_(666))
					{
						o_ptr->art_flags3 |= (TR3_TY_CURSE);

						o_ptr->art_flags3 |= (TR3_AGGRAVATE);
						if (one_in_(666)) o_ptr->art_flags3 |= (TR3_PERMA_CURSE);
						o_ptr->art_flags1 |= (TR1_VORPAL);
						o_ptr->art_flags1 |= (TR1_VAMPIRIC);
#ifdef JP
						msg_print("���������������");
#else
						msg_print("Blood, Blood, Blood!");
#endif
					}
				}
			}
		}
		add = FALSE;
		break;
	case MN_SHADOW_MOVE:	/* Shadow move */
		{
			bool flag;

			for (i = 0; i < 3; i++)
			{
				if (!tgt_pt(&x, &y)) return FALSE;

				flag = FALSE;

				for (dir = 0; dir < 8; dir++)
				{
					int dy = y + ddy_ddd[dir];
					int dx = x + ddx_ddd[dir];
					if (dir == 5) continue;
					if(cave[dy][dx].m_idx) flag = TRUE;
				}

				if (!cave_empty_bold(y, x) || (cave[y][x].info & CAVE_ICKY) ||
					(distance(y, x, py, px) > plev + 2))
				{
#ifdef JP
					msg_print("�����ˤϰ�ư�Ǥ��ʤ���");
#else
					msg_print("Can not teleport to there.");
#endif
					continue;
				}
				break;
			}

			if (flag && randint0(plev * plev / 2))
			{
				teleport_player_to(y, x);
			}
			else
			{
#ifdef JP
				msg_print("���äȡ�");
#else
				msg_print("Oops!");
#endif
				teleport_player(30);
			}
		}
		add = FALSE;
		break;
	case MN_REVENGE_SENTENCE:	/* Revenge_sentence */
		if (p_ptr->tim_sentence)
		{
#ifdef JP
			msg_print("���Ǥ����������Ѥߤ���");
#else
			msg_print("You already pronounced your revenge.");
#endif
			return FALSE;
		}
		else
		{
			s16b r;
			s16b a = 3 - (p_ptr->pspeed - 100) / 10;
			r = 1 + randint1(2) + MAX(0, MIN(3, a));

		#ifdef JP
			msg_format("���ʤ�����������𤷤������� %d ������", r);
		#else
			msg_format("You pronounce your revenge. %d turns left.", r);
		#endif
			p_ptr->rvs_d = 0;
			p_ptr->rvs_x = 0;
			p_ptr->rvs_y = 0;
			p_ptr->tim_sentence = r;
		}
		add = FALSE;
		break;

	/* Message Only */
	case MN_CURE_LITE:	/* Cure light wounds */
	case MN_CURE_SERIOUS:	/* Cure serious wounds */
	case MN_CURE_CRITICAL:	/* Cure criticul wounds */
#ifdef JP
		msg_print("��ʬ���ɤ�ɤ��ɤ��ʤäƤ��롣");
#else
		msg_print("You feel better and better.");
#endif
		break;
	case MN_BLESSING:	/* Blessing */
		if (!p_ptr->blessed)
		{
#ifdef JP
			msg_print("���ʵ�ʬ�ˤʤä���");
#else
			msg_print("You feel righteous!");
#endif
		}
		break;
	case MN_INERTIA:	/* Slow monsters */
		break;
	case MN_DEMON_AURA:	/* Demonic aura */
#ifdef JP
		msg_print("�Τ���Υ������ʤ��줿��");
#else
		msg_print("You have enveloped by fiery aura!");
#endif
		break;
	case MN_STINKING:	/* Stinking mist */
		break;
	case MN_EXTRA_MIGHT:	/* Extra Might */
#ifdef JP
		msg_print("�������Ϥ�ͯ������롣");
#else
		msg_print("You feel you get stronger.");
#endif
		break;
	case MN_RES_DARK:	/* Dark resistance */
#ifdef JP
		msg_print("�Ź��ؤ��������Ĥ����������롪");
#else
		msg_print("You feel resistant to dark!");
#endif
		break;
	case MN_ICE_ARMOR:	/* Ice armor */
#ifdef JP
		msg_print("�Τ�ɹ�γ���ʤ��줿��");
#else
		msg_print("You have enveloped by ice armor!");
#endif
		break;
	case MN_BERSERK:	/* Berserk */
		if (!p_ptr->shero)
		{
#ifdef JP
			msg_print("��٤�ޥ�����ˤʤä��������롪");
#else
			msg_print("You feel like a killing machine!");
#endif
		}
		break;
	case MN_RES_NETHER:	/* Nether resistance */
#ifdef JP
		msg_print("�Ϲ��ؤ��������Ĥ����������롪");
#else
		msg_print("You feel resistant to nether!");
#endif
		break;
	case MN_VAMPIRIC_MIST:	/* Vampiric mist */
		break;
	case MN_CONFUSION:	/* Confuse monsters */
#ifdef JP
		msg_print("���ʤ��μ꤬�֤������Ϥ᤿��");
#else
		msg_print("Your hands glow bright red.");
#endif
		break;
	case MN_BUILD_UP:	/* Build Up */
#ifdef JP
		msg_print("���Τ������ʤä�����������");
#else
		msg_print("You feel your body is developed more now.");
#endif
		break;
	case MN_ANTI_TELEPORT:	/* Anti teleport barrier */
#ifdef JP
		msg_print("�ƥ�ݡ��Ȥ��ɤ������򤫤�����");
#else
		msg_print("You feel anyone can not teleport except you.");
#endif
		break;
	case MN_ANTI_MULTIPLY:	/* Anti Multiply */
#ifdef JP
		msg_print("�������˻ߤ�������򤫤�����");
#else
		msg_print("You feel anyone can not already multiply.");
#endif
		break;
	case MN_RESTORE_LIFE:	/* Restore Life */
#ifdef JP
		msg_print("��̿�Ϥ����Ϥ᤿��");
#else
		msg_print("You feel your life energy starting to return.");
#endif
		break;
	case MN_STUN:	/* Stun monsters */
		break;
	case MN_RUNESWORD:
#ifdef JP
		msg_print("���ʤ�����郎������������");
#else
		if (is_two_handed())
			msg_print("Your weapons glow bright black.");
		else
			msg_print("Your weapon slows bright black.");
#endif
		break;
	case MN_VAMPIRIC_ATTACK:
#ifdef JP
		msg_print("���ʤ�����郎����ߤ��Ƥ��롣");
#else
		if (is_two_handed())
			msg_print("Your weapons want more blood now.");
		else
			msg_print("Your weapon wants more blood now.");
#endif
		break;
	case MN_EYE_FOR_EYE:	/* Eye for an eye */
#ifdef JP
		msg_print("������������˾�ˤ���줿��");
#else
		msg_print("You wish strongly you want to revenge anything.");
#endif
		break;
	case MN_ANTI_MAGIC:	/* Anti magic barrier */
#ifdef JP
		msg_print("��ˡ���ɤ������򤫤�����");
#else
		msg_print("You feel anyone can not cast spells except you.");
#endif
		break;
	default:
#ifdef JP
		msg_format("���ʤ��������ʼ��Ѥμ�ʸ %d �򾧤�����", spell);
#else
		msg_format("You cast an unknown Black Magic spell: %d.", spell);
#endif
		msg_print(NULL);
	}

	if (add)
	{
		p_ptr->keep_spells |= 1L << (spell);
		p_ptr->keep_magic++;
	}

	/* Redraw status */
	p_ptr->update |= (PU_BONUS | PU_HP | PU_MANA | PU_SPELLS);
	p_ptr->redraw |= (PR_EXTRA | PR_HP | PR_MANA);

	return TRUE;
}


void calc_bonuses_hex_spell(void)
{
	if (p_ptr->realm1 != REALM_HEX) return;

	if (is_keeping_spell(MS_EXTRA_MIGHT)) p_ptr->stat_add[A_STR] += 4;
	if (is_keeping_spell(MS_BUILD_UP))
	{
		p_ptr->stat_add[A_STR] += 4;
		p_ptr->stat_add[A_DEX] += 4;
		p_ptr->stat_add[A_CON] += 4;
		/* max blows + 1 -> xtra1.c */
		/* mhp + 100 -> xtra1.c */
	}
	if (is_keeping_spell(MS_DEMON_AURA))
	{
		p_ptr->sh_fire = TRUE;
		p_ptr->regenerate = TRUE;
	}
	if (is_keeping_spell(MS_ICE_ARMOR))
	{
		p_ptr->sh_cold = TRUE;
		p_ptr->to_a += 30;
		p_ptr->dis_to_a += 30;
	}
	if (is_keeping_spell(MS_RES_DARK)) p_ptr->resist_dark = TRUE;
	if (is_keeping_spell(MS_RES_NETHER)) p_ptr->resist_neth = TRUE;

	/* to-hit bonus -> calc_melee_bonus at cmd1.c */
}


bool teleport_barrier(int m_idx)
{
	monster_type *m_ptr = &m_list[m_idx];
	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	if (!is_keeping_spell(MS_ANTI_TELEPORT)) return FALSE;
	if ((p_ptr->lev * 3 / 2) < randint1(r_ptr->level)) return FALSE;

	return TRUE;
}


bool magic_barrier(int m_idx)
{
	monster_type *m_ptr = &m_list[m_idx];
	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	if (!is_keeping_spell(MS_ANTI_MAGIC)) return FALSE;
	if ((p_ptr->lev * 3 / 2) < randint1(r_ptr->level)) return FALSE;

	return TRUE;
}

bool multiply_barrier(int m_idx)
{
	monster_type *m_ptr = &m_list[m_idx];
	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	if (!is_keeping_spell(MS_ANTI_MULTIPLY)) return FALSE;
	if ((p_ptr->lev * 3 / 2) < randint1(r_ptr->level)) return FALSE;

	return TRUE;
}
