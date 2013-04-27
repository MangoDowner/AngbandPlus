#include "angband.h"

bool spoiler_hack = FALSE;

#ifdef ALLOW_SPOILERS

typedef void(*_file_fn)(FILE*);
static void _text_file(cptr name, _file_fn fn)
{
	int		fd = -1;
	FILE	*fff = NULL;
	char	buf[1024];

	path_build(buf, sizeof(buf), ANGBAND_DIR_HELP, name);
	fff = my_fopen(buf, "w");

	if (!fff)
	{
		path_build(buf, sizeof(buf), ANGBAND_DIR_USER, name);
		fff = my_fopen(buf, "w");

		if (!fff)
		{
			prt("Failed!", 0, 0);
			(void)inkey();
			return;
		}
	}

	fn(fff);
	fprintf(fff, "\n\nAutomatically generated for PosChengband %d.%d.%d.\n",
		    VER_MAJOR, VER_MINOR, VER_PATCH);

	my_fclose(fff);
	msg_format("Created %s", buf);
}

static void _wrap_text(FILE *fff, cptr text, int col, int width)
{
	char buf[5*1024];
	char fmt[128];
	cptr t;

	sprintf(fmt, "%%%ds%%s\n", col);
	
	roff_to_buf(text, width-col, buf, sizeof(buf));
	for (t = buf; t[0]; t += strlen(t) + 1)
		fprintf(fff, fmt, "", t);
}

static void _race_help(FILE *fff, int idx)
{
	race_t *race_ptr = get_race_t_aux(idx, 0);

	fprintf(fff, "***** <%s>\n", race_ptr->name);
	fprintf(fff, "--- %s ---\n\n", race_ptr->name);
	_wrap_text(fff, race_ptr->desc, 2, 80);

	fprintf(fff, "\n\n");
}

static void _races_help(FILE* fff)
{
	int i;

	fprintf(fff, "=== The Races ===\n\n");
	for (i = 0; i < MAX_RACES; i++)
	{
		race_t *race_ptr = get_race_t_aux(i, 0);
		if (race_ptr->flags & RACE_IS_MONSTER) continue;
		_race_help(fff, i);
	}

	fprintf(fff, "***** <Tables>\n");
	fprintf(fff, "--- Table 1 - Race Statistic Bonus Table ---\n\n");
	fprintf(fff, "               STR  INT  WIS  DEX  CON  CHR  Life  Exp\n");	

	for (i = 0; i < MAX_RACES; i++)
	{
		race_t *race_ptr = get_race_t_aux(i, 0);
		if (race_ptr->flags & RACE_IS_MONSTER) continue;

		fprintf(fff, "%-14s %+3d  %+3d  %+3d  %+3d  %+3d  %+3d  %3d%%  %3d%%\n", 
			race_ptr->name,
			race_ptr->stats[A_STR], race_ptr->stats[A_INT], race_ptr->stats[A_WIS], 
			race_ptr->stats[A_DEX], race_ptr->stats[A_CON], race_ptr->stats[A_CHR], 
			race_ptr->life, race_ptr->exp
		);
	}
	fprintf(fff, "\n\n");

	fprintf(fff, "--- Table 2 - Race Skill Bonus Table ---\n\n");
	fprintf(fff, "               Dsrm  Dvce  Save  Stlh  Srch  Prcp  Melee  Bows  Infra\n");
	for (i = 0; i < MAX_RACES; i++)
	{
		race_t *race_ptr = get_race_t_aux(i, 0);
		if (race_ptr->flags & RACE_IS_MONSTER) continue;

		fprintf(fff, "%-14s %+4d  %+4d  %+4d  %+4d  %+4d  %+4d  %+5d  %+4d  %4d'\n", 
			race_ptr->name,
			race_ptr->skills.dis, race_ptr->skills.dev, race_ptr->skills.sav,
			race_ptr->skills.stl, race_ptr->skills.srh, race_ptr->skills.fos,
			race_ptr->skills.thn, race_ptr->skills.thb, race_ptr->infra*10
		);
	}
	fprintf(fff, "\n\n");
}

static void _monster_races_help(FILE* fff)
{
	int i;

	fprintf(fff, "=== Monster Races ===\n\n");
	for (i = 0; i < MAX_RACES; i++)
	{
		race_t *race_ptr = get_race_t_aux(i, 0);
		if (!(race_ptr->flags & RACE_IS_MONSTER)) continue;
		_race_help(fff, i);
	}

	fprintf(fff, "***** <Tables>\n");
	fprintf(fff, "--- Table 1 - Race Statistic Bonus Table ---\n\n");
	fprintf(fff, "               STR  INT  WIS  DEX  CON  CHR  Life  Exp\n");	

	for (i = 0; i < MAX_RACES; i++)
	{
		race_t *race_ptr = get_race_t_aux(i, 0);
		if (!(race_ptr->flags & RACE_IS_MONSTER)) continue;

		fprintf(fff, "%-14s %+3d  %+3d  %+3d  %+3d  %+3d  %+3d  %3d%%  %3d%%\n", 
			race_ptr->name,
			race_ptr->stats[A_STR], race_ptr->stats[A_INT], race_ptr->stats[A_WIS], 
			race_ptr->stats[A_DEX], race_ptr->stats[A_CON], race_ptr->stats[A_CHR], 
			race_ptr->life, race_ptr->exp
		);
	}
	fprintf(fff, "\n\n");

	fprintf(fff, "--- Table 2 - Race Skill Bonus Table ---\n\n");
	fprintf(fff, "               Dsrm  Dvce  Save  Stlh  Srch  Prcp  Melee  Bows  Infra\n");
	for (i = 0; i < MAX_RACES; i++)
	{
		race_t *race_ptr = get_race_t_aux(i, 0);
		if (!(race_ptr->flags & RACE_IS_MONSTER)) continue;

		fprintf(fff, "%-14s %+4d  %+4d  %+4d  %+4d  %+4d  %+4d  %+5d  %+4d  %4d'\n", 
			race_ptr->name,
			race_ptr->skills.dis, race_ptr->skills.dev, race_ptr->skills.sav,
			race_ptr->skills.stl, race_ptr->skills.srh, race_ptr->skills.fos,
			race_ptr->skills.thn, race_ptr->skills.thb, race_ptr->infra*10
		);
	}
	fprintf(fff, "\n\n");
}

static void _demigod_help(FILE *fff, int idx)
{
	fprintf(fff, "***** <%s>\n", demigod_info[idx].name);
	fprintf(fff, "--- %s ---\n\n", demigod_info[idx].name);
	_wrap_text(fff, demigod_info[idx].desc, 2, 80);

	fprintf(fff, "\n\n");
}

static void _demigods_help(FILE* fff)
{
	int i;

	fprintf(fff, "=== Demigod Parentage ===\n\n");
	for (i = 0; i < MAX_DEMIGOD_TYPES; i++)
	{
		_demigod_help(fff, i);
	}

	fprintf(fff, "***** <Tables>\n");
	fprintf(fff, "--- Table 1 - Race Statistic Bonus Table ---\n\n");
	fprintf(fff, "               STR  INT  WIS  DEX  CON  CHR  Life  Exp\n");	

	for (i = 0; i < MAX_DEMIGOD_TYPES; i++)
	{
		race_t *race_ptr = get_race_t_aux(RACE_DEMIGOD, i);

		fprintf(fff, "%-14s %+3d  %+3d  %+3d  %+3d  %+3d  %+3d  %3d%%  %3d%%\n", 
			demigod_info[i].name,
			race_ptr->stats[A_STR], race_ptr->stats[A_INT], race_ptr->stats[A_WIS], 
			race_ptr->stats[A_DEX], race_ptr->stats[A_CON], race_ptr->stats[A_CHR], 
			race_ptr->life, race_ptr->exp
		);
	}
	fprintf(fff, "\n\n");

	fprintf(fff, "--- Table 2 - Race Skill Bonus Table ---\n\n");
	fprintf(fff, "               Dsrm  Dvce  Save  Stlh  Srch  Prcp  Melee  Bows  Infra\n");
	for (i = 0; i < MAX_DEMIGOD_TYPES; i++)
	{
		race_t *race_ptr = get_race_t_aux(RACE_DEMIGOD, i);

		fprintf(fff, "%-14s %+4d  %+4d  %+4d  %+4d  %+4d  %+4d  %+5d  %+4d  %4d'\n", 
			demigod_info[i].name,
			race_ptr->skills.dis, race_ptr->skills.dev, race_ptr->skills.sav,
			race_ptr->skills.stl, race_ptr->skills.srh, race_ptr->skills.fos,
			race_ptr->skills.thn, race_ptr->skills.thb, race_ptr->infra*10
		);
	}
	fprintf(fff, "\n\n");

	fprintf(fff, "--- Table 3 - Demigod Special Powers ---\n\n");
	for (i = 0; i < MAX_MUTATIONS; i++)
	{
		if (mut_demigod_pred(i))
		{
			char b1[255], b2[255];
			mut_name(i, b1);
			mut_help_desc(i, b2);
			fprintf(fff, "%-19s %s\n", b1, b2);
		}
	}
	fprintf(fff, "\n\n");
}

static void _class_help(FILE *fff, int idx)
{
	class_t *class_ptr = get_class_t_aux(idx, 0);
	
	fprintf(fff, "***** <%s>\n", class_ptr->name);
	fprintf(fff, "--- %s ---\n\n", class_ptr->name);
	_wrap_text(fff, class_ptr->desc, 2, 80);
	fprintf(fff, "\n\n");
}

static void _classes_help(FILE* fff)
{
	int i;

	fprintf(fff, "=== The Classes ===\n\n");
	for (i = 0; i < MAX_CLASS; i++)
	{
		_class_help(fff, i);
	}

	fprintf(fff, "***** <Tables>\n");
	fprintf(fff, "--- Table 1 - Class Statistic Bonus Table ---\n\n");
	fprintf(fff, "               STR  INT  WIS  DEX  CON  CHR  Life  Exp\n");	

	for (i = 0; i < MAX_CLASS; i++)
	{
		class_t *class_ptr = get_class_t_aux(i, 0);
		if (i == CLASS_MONSTER) continue;

		fprintf(fff, "%-14s %+3d  %+3d  %+3d  %+3d  %+3d  %+3d  %3d%%  %3d%%\n", 
			class_ptr->name,
			class_ptr->stats[0], class_ptr->stats[1], class_ptr->stats[2], 
			class_ptr->stats[3], class_ptr->stats[4], class_ptr->stats[5], 
			class_ptr->life, class_ptr->exp
		);
	}
	fprintf(fff, "\n\n");

	fprintf(fff, "--- Table 2 - Class Skill Bonus Table ---\n\n");
	fprintf(fff, "               Dsrm   Dvce   Save   Stlh  Srch  Prcp  Melee  Bows\n");
	for (i = 0; i < MAX_CLASS; i++)
	{
		class_t *class_ptr = get_class_t_aux(i, 0);

		if (i == CLASS_BERSERKER) continue;
		if (i == CLASS_MONSTER) continue;

		fprintf(fff, "%-14s %2d+%-2d  %2d+%-2d  %2d+%-2d  %4d  %4d  %4d  %2d+%-2d  %2d+%-2d\n", 
			class_ptr->name,
			class_ptr->base_skills.dis, class_ptr->extra_skills.dis, 
			class_ptr->base_skills.dev, class_ptr->extra_skills.dev, 
			class_ptr->base_skills.sav, class_ptr->extra_skills.sav,
			class_ptr->base_skills.stl, 
			class_ptr->base_skills.srh, 
			class_ptr->base_skills.fos,
			class_ptr->base_skills.thn, class_ptr->extra_skills.thn, 
			class_ptr->base_skills.thb, class_ptr->extra_skills.thb
		);
	}
	fprintf(fff, "\n\n");
}

static void _personality_help(FILE *fff, int idx)
{
	player_seikaku *a_ptr = &seikaku_info[idx];
	fprintf(fff, "***** <%s>\n", a_ptr->title);
	fprintf(fff, "--- %s ---\n\n", a_ptr->title);
	_wrap_text(fff, birth_get_personality_desc(idx), 2, 80);
	fprintf(fff, "\n\n");
}

static void _personalities_help(FILE* fff)
{
	int i;

	fprintf(fff, "=== The Personalities ===\n\n");
	for (i = 0; i < MAX_SEIKAKU; i++)
	{
		_personality_help(fff, i);
	}

	fprintf(fff, "***** <Tables>\n");
	fprintf(fff, "--- Table 1 - Personality Statistic Bonus Table ---\n\n");
	fprintf(fff, "               STR  INT  WIS  DEX  CON  CHR  Life  Exp\n");	

	for (i = 0; i < MAX_SEIKAKU; i++)
	{
		player_seikaku *a_ptr = &seikaku_info[i];

		fprintf(fff, "%-14s %+3d  %+3d  %+3d  %+3d  %+3d  %+3d  %3d%%  %3d%%\n", 
			a_ptr->title,
			a_ptr->a_adj[0], a_ptr->a_adj[1], a_ptr->a_adj[2], 
			a_ptr->a_adj[3], a_ptr->a_adj[4], a_ptr->a_adj[5], 
			a_ptr->life, a_ptr->a_exp
		);
	}
	fprintf(fff, "\n\n");

	fprintf(fff, "--- Table 2 - Personality Skill Bonus Table ---\n\n");
	fprintf(fff, "               Dsrm  Dvce  Save  Stlh  Srch  Prcp  Melee  Bows\n");
	for (i = 0; i < MAX_SEIKAKU; i++)
	{
		player_seikaku *a_ptr = &seikaku_info[i];

		fprintf(fff, "%-14s %+4d  %+4d  %+4d  %+4d  %+4d  %+4d  %+5d  %+4d\n", 
			a_ptr->title,
			a_ptr->skills.dis, a_ptr->skills.dev, a_ptr->skills.sav,
			a_ptr->skills.stl, a_ptr->skills.srh, a_ptr->skills.fos,
			a_ptr->skills.thn, a_ptr->skills.thb
		);
	}
	fprintf(fff, "\n\n");
}

static void _show_help(cptr helpfile)
{
	screen_save();
	show_file(TRUE, helpfile, NULL, 0, 0);
	screen_load();
}
void generate_spoilers(void)
{
	spoiler_hack = TRUE;
	_text_file("Races.txt", _races_help);
	_text_file("MonsterRaces.txt", _monster_races_help);
	_text_file("Demigods.txt", _demigods_help);
	_text_file("Classes.txt", _classes_help);
	_text_file("Personalities.txt", _personalities_help);

	_show_help("Personalities.txt");
	spoiler_hack = FALSE;
}

#endif