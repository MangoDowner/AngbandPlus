#include "angband.h"

void alchemy_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Alchemy", "�ߥ����μ�"));
		break;
	case SPELL_DESC:
		var_set_string(res, "Turns valuable items into gold.");
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You gain the Midas touch.", "�֥ߥ������μ�פ�ǽ�Ϥ�������"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You lose the Midas touch.", "�ߥ����μ��ǽ�Ϥ򼺤ä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You can turn ordinary items to gold.", "���ʤ����̾異���ƥ�����Ѥ��뤳�Ȥ��Ǥ��롣"));
		break;
	case SPELL_CAST:
		var_set_bool(res, FALSE);
		if (alchemy())
			var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_alchemy(void) { return cast_spell(alchemy_spell); }

void alter_reality_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Alter Reality", "���¤κ���"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Recreates current dungeon level.", "���ߤγ���ƹ������롣"));
		break;
	case SPELL_INFO:
		var_set_string(res, info_delay(15, 20));
		break;
	case SPELL_CAST:
		alter_reality();
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}


void android_ray_gun_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Ray Gun", "�쥤����"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("", ""));
		break;
	case SPELL_INFO:
		var_set_string(res, info_damage(0, 0, spell_power(5 + (p_ptr->lev+1) / 2)));
		break;
	case SPELL_CAST:
	{
		int dir = 0;
		var_set_bool(res, FALSE);
		if (!get_aim_dir(&dir)) return;
		
		msg_print(T("You fire your ray gun.", "�쥤�����ȯ�ͤ�����"));
		fire_bolt(GF_MISSILE, dir, spell_power(5 + (p_ptr->lev+1) / 2));
		
		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void android_blaster_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Blaster", "�֥饹����"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("", ""));
		break;
	case SPELL_INFO:
		var_set_string(res, info_damage(0, 0, spell_power(5 + p_ptr->lev)));
		break;
	case SPELL_CAST:
	{
		int dir = 0;
		var_set_bool(res, FALSE);
		if (!get_aim_dir(&dir)) return;

		msg_print(T("You fire your blaster.", "�֥饹������ȯ�ͤ�����"));
		fire_bolt(GF_MISSILE, dir, spell_power(5 + p_ptr->lev));

		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void android_bazooka_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Bazooka", "�Х�����"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Fires your bazooka at a nearby monster.", ""));
		break;
	case SPELL_INFO:
		var_set_string(res, info_damage(0, 0, spell_power(25 + p_ptr->lev * 2)));
		break;
	case SPELL_CAST:
	{
		int dir = 0;
		var_set_bool(res, FALSE);
		if (!get_aim_dir(&dir)) return;

		msg_print(T("You fire your bazooka.", "�Х�������ȯ�ͤ�����"));
		fire_ball(GF_MISSILE, dir, spell_power(25 + p_ptr->lev * 2), 2);

		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void android_beam_cannon_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Beam Cannon", "�ӡ��७��Υ�"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("", ""));
		break;
	case SPELL_INFO:
		var_set_string(res, info_damage(0, 0, spell_power(25 + p_ptr->lev * 3)));
		break;
	case SPELL_CAST:
	{
		int dir = 0;
		var_set_bool(res, FALSE);
		if (!get_aim_dir(&dir)) return;

		msg_print(T("You fire a beam cannon.", "�ӡ��७��Υ��ȯ�ͤ�����"));
		fire_beam(GF_MISSILE, dir, spell_power(25 + p_ptr->lev * 3));

		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void android_rocket_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Rocket Launcher", "���å�"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Launches a powerful rocket at your opponent.", ""));
		break;
	case SPELL_INFO:
		var_set_string(res, info_damage(0, 0, spell_power(p_ptr->lev * 7)));
		break;
	case SPELL_CAST:
	{
		int dir = 0;
		var_set_bool(res, FALSE);
		if (!get_aim_dir(&dir)) return;

		msg_print(T("You launch a rocket.", "���åȤ�ȯ�ͤ�����"));
		fire_rocket(GF_ROCKET, dir, spell_power(p_ptr->lev * 7), 2);

		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void animate_dead_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Animate Dead", "ȿ���ν�"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Resurrects nearby corpse and skeletons. And makes these your pets.", "���Ϥλ��Τ���������֤���"));
		break;
	case SPELL_CAST:
		animate_dead(0, py, px);
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void awesome_blow_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Awesome Blow");
		break;
	case SPELL_DESC:
		var_set_string(res, "Attack a monster with a single melee blow. If blow hits, does normal melee damage and propels the monster backwards.");
		break;
	case SPELL_CAST:
	{
		int y, x, dir;
		var_set_bool(res, FALSE);

		if (!get_rep_dir2(&dir)) return;
		if (dir == 5) return;

		y = py + ddy[dir];
		x = px + ddx[dir];

		if (cave[y][x].m_idx)
		{
			py_attack(y, x, MELEE_AWESOME_BLOW);
		}
		else
		{
			msg_print(T("There is no monster.", "���������ˀϥ�󥹥����π��ހ���"));
			return;
		}

		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void banish_evil_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Banish Evil", "�ٰ�����"));
		break;
	case SPELL_DESC:
		var_set_string(res, "Attempts to remove a single evil opponent.");
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You feel a holy wrath fill you.", "�������ܤ���Ϥ��������줿��"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You no longer feel a holy wrath.", "�������ܤ���Ϥ򴶤��ʤ��ʤä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You can send evil creatures directly to Hell.", "���ʤ��ϼٰ��ʥ�󥹥������Ϲ�����Ȥ����Ȥ��Ǥ��롣"));
		break;
	case SPELL_CAST:
	{
		int dir = 0;
		int x, y;
		cave_type *c_ptr;
		monster_type *m_ptr;
		monster_race *r_ptr;

		if (!get_rep_dir2(&dir)) 
		{
			var_set_bool(res, FALSE);
			break;
		}

		var_set_bool(res, TRUE);

		y = py + ddy[dir];
		x = px + ddx[dir];
		c_ptr = &cave[y][x];

		if (!c_ptr->m_idx)
		{
			msg_print(T("You sense no evil there!", "�ٰ���¸�ߤ򴶤��Ȥ�ޤ���"));
			break;
		}

		m_ptr = &m_list[c_ptr->m_idx];
		r_ptr = &r_info[m_ptr->r_idx];

		if ((r_ptr->flags3 & RF3_EVIL) &&
			!(r_ptr->flags1 & RF1_QUESTOR) &&
			!(r_ptr->flags1 & RF1_UNIQUE) &&
			!p_ptr->inside_arena && !p_ptr->inside_quest &&
			(r_ptr->level < randint1(p_ptr->lev+50)) &&
			!(m_ptr->mflag2 & MFLAG2_NOGENO))
		{
			if (record_named_pet && is_pet(m_ptr) && m_ptr->nickname)
			{
				char m_name[80];

				monster_desc(m_name, m_ptr, MD_INDEF_VISIBLE);
				do_cmd_write_nikki(NIKKI_NAMED_PET, RECORD_NAMED_PET_GENOCIDE, m_name);
			}

			/* Delete the monster, rather than killing it. */
			delete_monster_idx(c_ptr->m_idx);
			msg_print(T("The evil creature vanishes in a puff of sulfurous smoke!", "���μٰ��ʥ�󥹥�����β��������ȤȤ�˾ä���ä���"));
		}
		else
		{
			msg_print(T("Your invocation is ineffectual!", "����ϸ��̤��ʤ��ä���"));
			if (one_in_(13)) m_ptr->mflag2 |= MFLAG2_NOGENO;
		}
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_banish_evil(void) { return cast_spell(banish_evil_spell); }

void battle_frenzy_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Battle Frenzy", "�������"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Gives another bonus to hit and HP, immunity to fear for a while. Hastes you. But decreases AC.", "����β��������ݤ�������®���롣"));
		break;
	case SPELL_CAST:
	{
		int b_base = spell_power(25);
		int sp_base = spell_power(p_ptr->lev / 2);
		int sp_sides = 20 + p_ptr->lev / 2;

		set_shero(randint1(b_base) + b_base, FALSE);
		set_afraid(0, TRUE);
		set_fast(randint1(sp_sides) + sp_base, FALSE);

		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void berserk_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Berserk", "����β�"));
		break;
	case SPELL_DESC:
		var_set_string(res, "Enter a berserk frenzy, gaining great combat bonuses, but losing the ability to think clearly.");
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You feel a controlled rage.", "����Ǥ�����򴶤��롣"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You no longer feel a controlled rage.", "����Ǥ�����򴶤��ʤ��ʤä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You can drive yourself into a berserk frenzy.", "���ʤ��ϼ�ʬ�ΰջפǶ�����Ʈ���֤ˤʤ뤳�Ȥ��Ǥ��롣"));
		break;
	case SPELL_CAST:
	{
		msg_print("Raaagh!  You feel like hitting something.");
		set_afraid(0, TRUE);
		set_shero(10 + randint1(p_ptr->lev), FALSE);
		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_berserk(void) { return cast_spell(berserk_spell); }

void bless_spell(int cmd, variant *res)
{
	int base = spell_power(12);
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Bless", "��ʡ"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Gives bonus to hit and AC for a few turns.", "������֡�̿��Ψ��AC�˥ܡ��ʥ������롣"));
		break;
	case SPELL_INFO:
		var_set_string(res, info_duration(base, base));
		break;
	case SPELL_CAST:
		set_blessed(randint1(base) + base, FALSE);
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void bless_weapon_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Bless Weapon", "����ʡ"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Blesses your current weapon.", ""));
		break;
	case SPELL_CAST:
		var_set_bool(res, bless_weapon());
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void brain_smash_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Brain Smash", ""));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Gaze intently at a single foe, causing damage, confusion and stunning", ""));
		break;
	case SPELL_INFO:
		var_set_string(res, info_damage(12, spell_power(12), 0));
		break;
	case SPELL_CAST:
	{
		int dir = 0;
		var_set_bool(res, FALSE);
		if (!get_aim_dir(&dir)) return;
		fire_ball_hide(GF_BRAIN_SMASH, dir, spell_power(damroll(12, 12)), 0);
		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void breathe_disintegration_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Breathe Disintegration", ""));
		break;
	case SPELL_DESC:
		var_set_string(res, T("A disintegration breath.  Not even the dungeon walls can withstand its power!", ""));
		break;
	case SPELL_INFO:
		var_set_string(res, info_damage(0, 0, spell_power(MIN(p_ptr->chp / 6, 150))));
		break;
	case SPELL_CAST:
	{
		int dir = 0;
		var_set_bool(res, FALSE);
		if (!get_aim_dir(&dir)) return;

		stop_mouth();
		msg_print(T("You breathe disintegration.", "ʬ��Υ֥쥹���Ǥ�����"));
		fire_ball(GF_DISINTEGRATE, dir, 
			spell_power(MIN(p_ptr->chp / 6, 150)), 
			(p_ptr->lev > 40 ? -3 : -2));

		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void breathe_fire_I_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Breathe Fire", "��Υ֥쥹"));
		break;
	case SPELL_DESC:
		var_set_string(res, "Breathes Fire at your opponent.");
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You gain the ability to breathe fire.", "�Ф��Ǥ�ǽ�Ϥ�������"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You lose the ability to breathe fire.", "��Υ֥쥹���Ǥ�ǽ�Ϥ򼺤ä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You can breathe fire (dam lvl * 2).", "���ʤ��ϱ�Υ֥쥹���Ǥ����Ȥ��Ǥ��롣(���᡼�� ��٥�X2)"));
		break;
	case SPELL_INFO:
		var_set_string(res, info_damage(0, 0, spell_power(2 * p_ptr->lev)));
		break;
	case SPELL_COST_EXTRA:
		var_set_int(res, (p_ptr->lev+1)/2);
		break;
	case SPELL_CAST:
	{
		int dir = 0;
		var_set_bool(res, FALSE);
		if (get_aim_dir(&dir))
		{
			stop_mouth();
			msg_print(T("You breathe fire...", "���ʤ��ϲб�Υ֥쥹���Ǥ���..."));
			fire_ball(GF_FIRE, dir, spell_power(2 * p_ptr->lev), -1 - (p_ptr->lev / 20));
			var_set_bool(res, TRUE);
		}
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_breathe_fire_I(void) { return cast_spell(breathe_fire_I_spell); }

void breathe_fire_II_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Breathe Fire", "��Υ֥쥹"));
		break;
	case SPELL_DESC:
		var_set_string(res, "Breathes Fire at your opponent.");
		break;
	case SPELL_INFO:
		var_set_string(res, info_damage(0, 0, spell_power(p_ptr->chp*2/5)));
		break;
	case SPELL_COST_EXTRA:
		var_set_int(res, p_ptr->lev);
		break;
	case SPELL_CAST:
	{
		int dir = 0;
		var_set_bool(res, FALSE);
		if (get_aim_dir(&dir))
		{
			stop_mouth();
			msg_print(T("You breathe fire...", "���ʤ��ϲб�Υ֥쥹���Ǥ���..."));
			fire_ball(GF_FIRE, dir, spell_power(p_ptr->chp*2/5), -1 - (p_ptr->lev / 20));
			var_set_bool(res, TRUE);
		}
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void building_up_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Building Up", ""));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Increases your physical prowess", ""));
		break;
	case SPELL_CAST:
		set_tim_building_up(20 + randint1(20), FALSE);
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}
