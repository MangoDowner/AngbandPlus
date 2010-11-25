#include "angband.h"

void rodeo_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Rodeo", "���Ϥʤ餷"));
		break;
	case SPELL_DESC:
		var_set_string(res, "");
		break;
	case SPELL_CAST:
	{
		char m_name[80];
		monster_type *m_ptr;
		monster_race *r_ptr;
		int rlev;

		var_set_bool(res, FALSE);
		if (p_ptr->riding)
		{
			msg_print(T("You are already riding.", "���Ͼ��������"));
			return;
		}
		if (!do_riding(TRUE)) return;
		
		var_set_bool(res, TRUE);

		m_ptr = &m_list[p_ptr->riding];
		r_ptr = &r_info[m_ptr->r_idx];
		monster_desc(m_name, m_ptr, 0);
		msg_format(T("You ride on %s.", "%s�˾�ä���"),m_name);
		if (is_pet(m_ptr)) break;
		rlev = r_ptr->level;
		if (r_ptr->flags1 & RF1_UNIQUE) rlev = rlev * 3 / 2;
		if (rlev > 60) rlev = 60+(rlev-60)/2;
		if ((randint1(p_ptr->skill_exp[GINOU_RIDING] / 120 + p_ptr->lev * 2 / 3) > rlev)
			&& one_in_(2) && !p_ptr->inside_arena && !p_ptr->inside_battle
			&& !(r_ptr->flags7 & (RF7_GUARDIAN)) && !(r_ptr->flags1 & (RF1_QUESTOR))
			&& (rlev < p_ptr->lev * 3 / 2 + randint0(p_ptr->lev / 5)))
		{
			msg_format(T("You tame %s.", "%s���ʤ�������"),m_name);
			set_pet(m_ptr);
		}
		else
		{
			msg_format(T("You have thrown off by %s.", "%s�˿�����Ȥ��줿��"),m_name);
			rakuba(1,TRUE);

			/* Paranoia */
			/* ���Ͻ����˼��Ԥ��Ƥ�Ȥˤ������ϲ�� */
			p_ptr->riding = 0;
		}
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}