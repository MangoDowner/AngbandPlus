#include "angband.h"

bool player_is_monster_king(void)
{
	race_t *race_ptr;
	if (p_ptr->pclass != CLASS_MONSTER) return FALSE;
	race_ptr = get_race_t();
	if (!race_ptr->boss_r_idx) return FALSE;
	if (r_info[race_ptr->boss_r_idx].max_num) return FALSE;
	return TRUE;
}

class_t *monster_get_class_t(void)
{
	static class_t me = {0};
	static bool init = FALSE;

	if (!init)
	{
		me.name = "Monster";
		me.desc = "";
		me.life = 100;
		me.exp = 100;
		me.pets = 25;
		init = TRUE;
	}

	return &me;
}

