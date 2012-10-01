/* File: save.c */

/*
 * Copyright (c) 1997 Ben Harrison, and others
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

#include "angband.h"


#ifdef FUTURE_SAVEFILES

/*
 * XXX XXX XXX Ignore this for now...
 *
 * The basic format of Angband 2.8.0 (and later) savefiles is simple.
 *
 * The savefile itself is a "header" (4 bytes) plus a series of "blocks",
 * plus, perhaps, some form of "footer" at the end.
 *
 * The "header" contains information about the "version" of the savefile.
 * Conveniently, pre-2.8.0 savefiles also use a 4 byte header, though the
 * interpretation of the "sf_extra" byte is very different.  Unfortunately,
 * savefiles from Angband 2.5.X reverse the sf_major and sf_minor fields,
 * and must be handled specially, until we decide to start ignoring them.
 *
 * Each "block" is a "type" (2 bytes), plus a "size" (2 bytes), plus "data",
 * plus a "check" (2 bytes), plus a "stamp" (2 bytes).  The format of the
 * "check" and "stamp" bytes is still being contemplated, but it would be
 * nice for one to be a simple byte-checksum, and the other to be a complex
 * additive checksum of some kind.  Both should be zero if the block is empty.
 *
 * Standard types:
 *   TYPE_BIRTH --> creation info
 *   TYPE_OPTIONS --> option settings
 *   TYPE_MESSAGES --> message recall
 *   TYPE_PLAYER --> player information
 *   TYPE_SPELLS --> spell information
 *   TYPE_INVEN --> player inven/equip
 *   TYPE_STORES --> store information
 *   TYPE_RACES --> monster race data
 *   TYPE_KINDS --> object kind data
 *   TYPE_UNIQUES --> unique info
 *   TYPE_ARTIFACTS --> artifact info
 *   TYPE_QUESTS --> quest info
 *
 * Dungeon information:
 *   TYPE_DUNGEON --> dungeon info
 *   TYPE_FEATURES --> dungeon features
 *   TYPE_OBJECTS --> dungeon objects
 *   TYPE_MONSTERS --> dungeon monsters
 *
 * Conversions:
 *   Break old "races" into normals/uniques
 *   Extract info about the "unique" monsters
 *
 * Question:
 *   Should there be a single "block" for info about all the stores, or one
 *   "block" for each store?  Or one "block", which contains "sub-blocks" of
 *   some kind?  Should we dump every "sub-block", or just the "useful" ones?
 *
 * Question:
 *   Should the normals/uniques be broken for 2.8.0, or should 2.8.0 simply
 *   be a "fixed point" into which older savefiles are converted, and then
 *   future versions could ignore older savefiles, and the "conversions"
 *   would be much simpler.
 */


/*
 * XXX XXX XXX
 */
#define TYPE_OPTIONS 17362


/*
 * Hack -- current savefile
 */
static int data_fd = -1;


/*
 * Hack -- current block type
 */
static u16b data_type;

/*
 * Hack -- current block size
 */
static u16b data_size;

/*
 * Hack -- pointer to the data buffer
 */
static byte *data_head;

/*
 * Hack -- pointer into the data buffer
 */
static byte *data_next;



/*
 * Hack -- write the current "block" to the savefile
 */
static errr wr_block(void)
{
	errr err;

	byte fake[4];

	/* Save the type and size */
	fake[0] = (byte)(data_type);
	fake[1] = (byte)(data_type >> 8);
	fake[2] = (byte)(data_size);
	fake[3] = (byte)(data_size >> 8);

	/* Dump the head */
	err = fd_write(data_fd, (cptr)&fake, sizeof(fake));

	/* Dump the actual data */
	err = fd_write(data_fd, (cptr)data_head, data_size);

	/* XXX XXX XXX */
	fake[0] = 0;
	fake[1] = 0;
	fake[2] = 0;
	fake[3] = 0;

	/* Dump the tail */
	err = fd_write(data_fd, (cptr)&fake, sizeof(fake));

	/* Hack -- reset */
	data_next = data_head;

	/* Wipe the data block */
	C_WIPE(data_head, 65535, byte);

	/* Success */
	return (0);
}



/*
 * Hack -- add data to the current block
 */
static void put_byte(byte v)
{
	*data_next++ = v;
}

/*
 * Hack -- add data to the current block
 */
static void put_char(char v)
{
	put_byte((byte)(v));
}

/*
 * Hack -- add data to the current block
 */
static void put_u16b(u16b v)
{
	*data_next++ = (byte)(v);
	*data_next++ = (byte)(v >> 8);
}

/*
 * Hack -- add data to the current block
 */
static void put_s16b(s16b v)
{
	put_u16b((u16b)(v));
}

/*
 * Hack -- add data to the current block
 */
static void put_u32b(u32b v)
{
	*data_next++ = (byte)(v);
	*data_next++ = (byte)(v >> 8);
	*data_next++ = (byte)(v >> 16);
	*data_next++ = (byte)(v >> 24);
}

/*
 * Hack -- add data to the current block
 */
static void put_s32b(s32b v)
{
	put_u32b((u32b)(v));
}

/*
 * Hack -- add data to the current block
 */
static void put_string(char *str)
{
	while ((*data_next++ = *str++) != '\0');
}



/*
 * Write a savefile for Angband 2.8.0
 */
static errr wr_savefile(void)
{
	int i;

	u32b now;

	byte tmp8u;
	u16b tmp16u;

	errr err;

	byte fake[4];


	/*** Hack -- extract some data ***/

	/* Hack -- Get the current time */
	now = time((time_t*)(NULL));

	/* Note the operating system */
	sf_xtra = 0L;

	/* Note when the file was saved */
	sf_when = now;

	/* Note the number of saves */
	sf_saves++;


	/*** Actually write the file ***/

	/* Open the file XXX XXX XXX */
	data_fd = -1;

	/* Dump the version */
	fake[0] = (byte)(VERSION_MAJOR);
	fake[1] = (byte)(VERSION_MINOR);
	fake[2] = (byte)(VERSION_PATCH);
	fake[3] = (byte)(VERSION_EXTRA);

	/* Dump the data */
	err = fd_write(data_fd, (cptr)&fake, sizeof(fake));


	/* Make array XXX XXX XXX */
	C_MAKE(data_head, 65535, byte);

	/* Hack -- reset */
	data_next = data_head;


#if 0
	/* Operating system */
	wr_u32b(sf_xtra);

	/* Time file last saved */
	wr_u32b(sf_when);

	/* Number of past lives */
	wr_u16b(sf_lives);

	/* Number of times saved */
	wr_u16b(sf_saves);

	/* XXX XXX XXX */

	/* Set the type */
	data_type = TYPE_BASIC;

	/* Set the "options" size */
	data_size = (data_next - data_head);

	/* Write the block */
	wr_block();
#endif


	/* Dump the "options" */
	put_options();

	/* Set the type */
	data_type = TYPE_OPTIONS;

	/* Set the "options" size */
	data_size = (data_next - data_head);

	/* Write the block */
	wr_block();

	/* XXX XXX XXX */

#if 0

	/* Dump the "messages" */

	/* Dump the number of "messages" */
	tmp16u = message_num();
	if (compress_savefile && (tmp16u > 40)) tmp16u = 40;
	wr_u16b(tmp16u);

	/* Dump the messages and types (oldest first!) */
	for (i = tmp16u - 1; i >= 0; i--)
	{
		wr_string(message_str(i));
		wr_u16b(message_type(i));
	}

	/* Dump the monster lore */
	tmp16u = z_info->r_max;
	wr_u16b(tmp16u);
	for (i = 0; i < tmp16u; i++) wr_monster_lore(i);


	/* Dump the object memory */
	tmp16u = z_info->k_max;
	wr_u16b(tmp16u);
	for (i = 0; i < tmp16u; i++) wr_xtra(i);


	/* Hack -- Dump the quests */
	tmp16u = MAX_Q_IDX;
	wr_u16b(tmp16u);
	for (i = 0; i < tmp16u; i++)
	{
		wr_byte(q_list[i].level);
		wr_byte(0);
		wr_byte(0);
		wr_byte(0);
	}

	/* Hack -- Dump the artifacts */
	tmp16u = z_info->a_max;
	wr_u16b(tmp16u);
	for (i = 0; i < tmp16u; i++)
	{
		artifact_type *a_ptr = &a_info[i];
		wr_byte(a_ptr->cur_num);
		wr_byte(0);
		wr_byte(0);
		wr_byte(0);
	}



	/* Write the "extra" information */
	wr_extra();


	/* Dump the "player hp" entries */
	tmp16u = PY_MAX_LEVEL;
	wr_u16b(tmp16u);
	for (i = 0; i < tmp16u; i++)
	{
		wr_s16b(player_hp[i]);
	}


	/* Write spell data */
	wr_u32b(spell_learned1);
	wr_u32b(spell_learned2);
	wr_u32b(spell_worked1);
	wr_u32b(spell_worked2);
	wr_u32b(spell_forgotten1);
	wr_u32b(spell_forgotten2);

	/* Dump the ordered spells */
	for (i = 0; i < PY_MAX_SPELLS; i++)
	{
		wr_byte(spell_order[i]);
	}


	/* Write the inventory */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		object_type *o_ptr = &inventory[i];

		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;

		/* Dump index */
		wr_u16b(i);

		/* Dump object */
		wr_item(o_ptr);
	}

	/* Add a sentinel */
	wr_u16b(0xFFFF);


	/* Note the stores */
	tmp16u = MAX_STORES;
	wr_u16b(tmp16u);

	/* Dump the stores */
	for (i = 0; i < tmp16u; i++) wr_store(&store[i]);


	/* Player is not dead, write the dungeon */
	if (!p_ptr->is_dead)
	{
		/* Dump the dungeon */
		wr_dungeon();

	}

#endif

	/* Dump the "final" marker XXX XXX XXX */
	/* Type zero, Size zero, Contents zero, etc */


	/* XXX XXX XXX Check for errors */


	/* Kill array XXX XXX XXX */
	KILL(data_head);


	/* Success */
	return (0);
}





/*
 * Hack -- read the next "block" from the savefile
 */
static errr rd_block(void)
{
	errr err;

	byte fake[4];

	/* Read the head data */
	err = fd_read(data_fd, (char*)&fake, sizeof(fake));

	/* Extract the type and size */
	data_type = (fake[0] | ((u16b)fake[1] << 8));
	data_size = (fake[2] | ((u16b)fake[3] << 8));

	/* Wipe the data block */
	C_WIPE(data_head, 65535, byte);

	/* Read the actual data */
	err = fd_read(data_fd, (char*)data_head, data_size);

	/* Read the tail data */
	err = fd_read(data_fd, (char*)&fake, sizeof(fake));

	/* XXX XXX XXX Verify */

	/* Hack -- reset */
	data_next = data_head;

	/* Success */
	return (0);
}


/*
 * Hack -- get data from the current block
 */
static void get_byte(byte *ip)
{
	byte d1;
	d1 = (*data_next++);
	(*ip) = (d1);
}

/*
 * Hack -- get data from the current block
 */
static void get_char(char *ip)
{
	get_byte((byte*)ip);
}

/*
 * Hack -- get data from the current block
 */
static void get_u16b(u16b *ip)
{
	u16b d0, d1;
	d0 = (*data_next++);
	d1 = (*data_next++);
	(*ip) = (d0 | (d1 << 8));
}

/*
 * Hack -- get data from the current block
 */
static void get_s16b(s16b *ip)
{
	get_u16b((u16b*)ip);
}

/*
 * Hack -- get data from the current block
 */
static void get_u32b(u32b *ip)
{
	u32b d0, d1, d2, d3;
	d0 = (*data_next++);
	d1 = (*data_next++);
	d2 = (*data_next++);
	d3 = (*data_next++);
	(*ip) = (d0 | (d1 << 8) | (d2 << 16) | (d3 << 24));
}

/*
 * Hack -- get data from the current block
 */
static void get_s32b(s32b *ip)
{
	get_u32b((u32b*)ip);
}



/*
 * Read a savefile for Angband 2.8.0
 */
static errr rd_savefile(void)
{
	bool done = FALSE;

	byte fake[4];


	/* Open the savefile */
	data_fd = fd_open(savefile, O_RDONLY);

	/* No file */
	if (data_fd < 0) return (1);

	/* Strip the first four bytes (see below) */
	if (fd_read(data_fd, (char*)(fake), sizeof(fake))) return (1);


	/* Make array XXX XXX XXX */
	C_MAKE(data_head, 65535, byte);

	/* Hack -- reset */
	data_next = data_head;


	/* Read blocks */
	while (!done)
	{
		/* Read the block */
		if (rd_block()) break;

		/* Analyze the type */
		switch (data_type)
		{
			/* Done XXX XXX XXX */
			case 0:
			{
				done = TRUE;
				break;
			}

			/* Grab the options */
			case TYPE_OPTIONS:
			{
				if (get_options()) err = -1;
				break;
			}
		}

		/* XXX XXX XXX verify "data_next" */
		if (data_next - data_head > data_size) break;
	}


	/* XXX XXX XXX Check for errors */


	/* Kill array XXX XXX XXX */
	KILL(data_head);


	/* Success */
	return (0);
}


#endif /* FUTURE_SAVEFILES */




/*
 * Some "local" parameters, used to help write savefiles
 */

static FILE	*fff;		/* Current save "file" */

static byte	xor_byte;	/* Simple encryption */

static u32b	v_stamp = 0L;	/* A simple "checksum" on the actual values */
static u32b	x_stamp = 0L;	/* A simple "checksum" on the encoded bytes */



/*
 * These functions place information into a savefile a byte at a time
 */

static void sf_put(byte v)
{
	/* Encode the value, write a character */
	xor_byte ^= v;
	(void)putc((int)xor_byte, fff);

	/* Maintain the checksum info */
	v_stamp += v;
	x_stamp += xor_byte;
}

static void wr_byte(byte v)
{
	sf_put(v);
}

static void wr_u16b(u16b v)
{
	sf_put((byte)(v & 0xFF));
	sf_put((byte)((v >> 8) & 0xFF));
}

static void wr_s16b(s16b v)
{
	wr_u16b((u16b)v);
}

static void wr_u32b(u32b v)
{
	sf_put((byte)(v & 0xFF));
	sf_put((byte)((v >> 8) & 0xFF));
	sf_put((byte)((v >> 16) & 0xFF));
	sf_put((byte)((v >> 24) & 0xFF));
}

static void wr_s32b(s32b v)
{
	wr_u32b((u32b)v);
}

static void wr_string(cptr str)
{
	while (*str)
	{
		wr_byte(*str);
		str++;
	}
	wr_byte(*str);
}


/*
 * These functions write info in larger logical records
 */


/*
 * Write an "item" record
 */
static void wr_item(const object_type *o_ptr)
{
	wr_s16b(o_ptr->k_idx);

	/* Location */
	wr_byte(o_ptr->iy);
	wr_byte(o_ptr->ix);

	wr_byte(o_ptr->tval);
	wr_byte(o_ptr->sval);
	wr_s16b(o_ptr->pval);

	wr_byte(o_ptr->discount);

	wr_byte(o_ptr->number);
	wr_s16b(o_ptr->weight);

	wr_byte(o_ptr->art_num);
	wr_byte(o_ptr->ego_num);

	wr_s16b(o_ptr->timeout);

	wr_s16b(o_ptr->to_h);
	wr_s16b(o_ptr->to_d);
	wr_s16b(o_ptr->to_a);
	wr_s16b(o_ptr->ac);
	wr_byte(o_ptr->dd);
	wr_byte(o_ptr->ds);

	wr_u32b(o_ptr->ident);

	wr_byte(o_ptr->marked);

	/* Old flags */
	wr_u32b(0L);
	wr_u32b(0L);
	wr_u32b(0L);

	/* Held by monster index */
	wr_s16b(o_ptr->held_m_idx);

	/* Extra information */
	wr_byte(o_ptr->xtra1);
	wr_u32b(o_ptr->xtra2);

	/* Save the inscription (if any) */
	if (o_ptr->obj_note)
	{
		wr_string(quark_str(o_ptr->obj_note));
	}
	else
	{
		wr_string("");
	}

	/* Object history */
	wr_byte(o_ptr->origin_nature);
	wr_s16b(o_ptr->origin_dlvl);
	wr_s16b(o_ptr->origin_r_idx);
	if (o_ptr->origin_m_name)
	{
		wr_string(quark_str(o_ptr->origin_m_name));
	}
	else
	{
		wr_string("");
	}
}



/*
 * Special monster flags that get saved in the savefile
 */
#define SAVE_MON_FLAGS (MFLAG_MIMIC | MFLAG_STERILE | MFLAG_ACTV | MFLAG_TOWN | MFLAG_WARY | \
						MFLAG_SLOWER | MFLAG_FASTER | MFLAG_ALWAYS_CAST | \
						MFLAG_AGGRESSIVE | MFLAG_ATTACKED_BAD | \
						MFLAG_HIT_BY_RANGED | MFLAG_HIT_BY_MELEE | MFLAG_QUEST | \
						MFLAG_NEED_PASS_WALL_FLOW | MFLAG_QUEST_SUMMON | MFLAG_JUST_SCARED)


/*
 * Write a "monster" record
 */
static void wr_monster(const monster_type *m_ptr)
{
	u32b tmp32u;

	wr_s16b(m_ptr->r_idx);
	wr_byte(m_ptr->fy);
	wr_byte(m_ptr->fx);
	wr_s16b(m_ptr->hp);
	wr_s16b(m_ptr->maxhp);
	wr_s16b(m_ptr->csleep);
	wr_byte(m_ptr->mspeed);
	wr_s16b(m_ptr->m_energy);
	wr_byte(m_ptr->stunned);
	wr_byte(m_ptr->confused);
	wr_byte(m_ptr->monfear);
	wr_s16b(m_ptr->hasted);
	wr_s16b(m_ptr->slowed);

	/*save the temporary flags*/
	tmp32u = m_ptr->mflag & (SAVE_MON_FLAGS);
	wr_u32b(tmp32u);

	wr_u32b(m_ptr->smart);
	wr_byte(m_ptr->target_y);
	wr_byte(m_ptr->target_x);
	wr_byte(m_ptr->mana);
	wr_s16b(m_ptr->mimic_k_idx);
	wr_byte(0);
}

/*
 * Write an "item" record
 */
static void wr_effect(const effect_type *x_ptr)
{

	wr_byte(x_ptr->x_type);
	wr_u16b(x_ptr->x_f_idx);

	wr_byte(x_ptr->x_cur_y);
	wr_byte(x_ptr->x_cur_x);


	wr_byte(x_ptr->x_countdown);
	wr_byte(x_ptr->x_repeats);

	wr_u16b(x_ptr->x_power);

	wr_s16b(x_ptr->x_source);

	wr_u16b(x_ptr->x_flags);

	wr_s16b(x_ptr->x_r_idx);
}


/*
 * Write a "lore" record
 */
static void wr_monster_lore(int r_idx)
{
	int i;

	monster_race *r_ptr = &r_info[r_idx];
	monster_lore *l_ptr = &l_list[r_idx];

	/* Count sights/deaths/kills */
	wr_s16b(l_ptr->sights);
	wr_s16b(l_ptr->deaths);
	wr_s16b(l_ptr->pkills);
	wr_s16b(l_ptr->tkills);

	/* Count wakes and ignores */
	wr_byte(l_ptr->wake);
	wr_byte(l_ptr->ignore);

	/* Extra stuff */
	wr_byte(l_ptr->xtra1);
	wr_byte(l_ptr->xtra2);

	/* Count drops */
	wr_byte(l_ptr->drop_gold);
	wr_byte(l_ptr->drop_item);

	/* Count spells */
	wr_byte(l_ptr->ranged);

	/* Count blows of each type */
	for (i = 0; i < MONSTER_BLOW_MAX; i++)
		wr_byte(l_ptr->blows[i]);

	/* Memorize flags */
	wr_u32b(l_ptr->r_l_flags1);
	wr_u32b(l_ptr->r_l_flags2);
	wr_u32b(l_ptr->r_l_flags3);
	wr_u32b(l_ptr->r_l_flags4);
	wr_u32b(l_ptr->r_l_flags5);
	wr_u32b(l_ptr->r_l_flags6);
	wr_u32b(l_ptr->r_l_flags7);
	wr_u32b(l_ptr->r_l_native);

	/* Monster limit per level */
	wr_byte(r_ptr->max_num);

	/* Later (?) */
	wr_byte(0);
	wr_byte(0);
	wr_byte(0);
}


/*
 * Write an "xtra" record
 */
static void wr_xtra(int k_idx)
{
	byte tmp8u = 0;

	object_kind *k_ptr = &k_info[k_idx];

	if (k_ptr->aware) tmp8u |= 0x01;
	if (k_ptr->tried) tmp8u |= 0x02;

	if (k_ptr->everseen) tmp8u |= 0x08;

	wr_byte(tmp8u);

	/*write the squelch settings*/
	tmp8u = k_ptr->squelch;

	wr_byte(tmp8u);
}


/*
 * Write a "store" record
 */
static void wr_store(const store_type *st_ptr)
{
	int j;

	/* Save the "open" counter */
	wr_u32b(st_ptr->store_open);

	/* Save the "insults" */
	wr_s16b(st_ptr->insult_cur);

	/* Save the current owner */
	wr_byte(st_ptr->owner);

	/* Save the stock size */
	wr_byte(st_ptr->stock_num);

	/* Save the "haggle" info */
	wr_s16b(st_ptr->good_buy);
	wr_s16b(st_ptr->bad_buy);

	/* Save the stock */
	for (j = 0; j < st_ptr->stock_num; j++)
	{
		/* Save each item in stock */
		wr_item(&st_ptr->stock[j]);
	}
}

/*
 * Write an "artifact lore" record
 */
static void wr_artifact_lore(int a_idx)
{
	byte tmp8u = 0;

	/* We know about this artifact */
	if (a_l_list[a_idx].was_fully_identified) tmp8u |= 0x01;

	/* Write the flags */
	wr_byte(tmp8u);

	/* For future use */
	wr_byte(0);
	wr_byte(0);
	wr_byte(0);
}


/*
 * Write a "terrain lore" record
 */
static void wr_feature_lore(int f_idx)
{
	int i;

	/* Get the feature */
	feature_type *f_ptr = &f_info[f_idx];
	feature_lore *f_l_ptr = &f_l_list[f_idx];
	byte tmp8u = 0;

	/* Save the "everseen flag" */
	if (f_ptr->f_everseen) tmp8u |= 0x01;

	wr_byte(tmp8u);

	/* Write the terrain_lore memory*/
	wr_byte(f_l_ptr->f_l_sights);

	/*Write the lore flags*/
	wr_u32b(f_l_ptr->f_l_flags1);
	wr_u32b(f_l_ptr->f_l_flags2);
	wr_u32b(f_l_ptr->f_l_flags3);

	wr_byte(f_l_ptr->f_l_defaults);

	/*record the max amount of feat states*/
	wr_byte(MAX_FEAT_STATES);

	for (i = 0; i < MAX_FEAT_STATES; i++)
	{
		wr_byte(f_l_ptr->f_l_state[i]);
	}

	wr_byte(f_l_ptr->f_l_power);

	wr_byte(f_l_ptr->f_l_dam_non_native);
	wr_byte(f_l_ptr->f_l_native_moves);
	wr_byte(f_l_ptr->f_l_non_native_moves);
	wr_byte(f_l_ptr->f_l_native_to_hit_adj);
	wr_byte(f_l_ptr->f_l_non_native_to_hit_adj);
	wr_byte(f_l_ptr->f_l_stealth_adj);

}


/*
 * Write RNG state
 */
static errr wr_randomizer(void)
{
	int i;

	/* Zero */
	wr_u16b(0);

	/* Place */
	wr_u16b(Rand_place);

	/* State */
	for (i = 0; i < RAND_DEG; i++)
	{
		wr_u32b(Rand_state[i]);
	}

	/* Success */
	return (0);
}


/*
 * Write the "options"
 */
static void wr_options(void)
{
	int i, k;

	u32b flag[8];
	u32b mask[8];
	u32b window_flag[ANGBAND_TERM_MAX];
	u32b window_mask[ANGBAND_TERM_MAX];


	/*** Oops ***/

	/* Oops */
	for (i = 0; i < 4; i++) wr_u32b(0L);


	/*** Special Options ***/

	/* Write "delay_factor" */
	wr_byte(op_ptr->delay_factor);

	/* Write "hitpoint_warn" */
	wr_byte(op_ptr->hitpoint_warn);

	wr_u16b(0);	/* oops */


	/*** Normal options ***/

	/* Reset */
	for (i = 0; i < 8; i++)
	{
		flag[i] = 0L;
		mask[i] = 0L;
	}

	/* Analyze the options */
	for (i = 0; i < OPT_MAX; i++)
	{
		int os = i / 32;
		int ob = i % 32;

		/* Process real entries */
		if (options[i].text)
		{
			/* Set flag */
			if (op_ptr->opt[i])
			{
				/* Set */
				flag[os] |= (1L << ob);
			}

			/* Set mask */
			mask[os] |= (1L << ob);
		}
	}

	/* Dump the flags */
	for (i = 0; i < 8; i++) wr_u32b(flag[i]);

	/* Dump the masks */
	for (i = 0; i < 8; i++) wr_u32b(mask[i]);


	/*** Window options ***/

	/* Reset */
	for (i = 0; i < ANGBAND_TERM_MAX; i++)
	{
		/* Flags */
		window_flag[i] = op_ptr->window_flag[i];

		/* Mask */
		window_mask[i] = 0L;

		/* Build the mask */
		for (k = 0; k < 32; k++)
		{
			/* Set mask */
			if (window_flag_desc[k])
			{
				window_mask[i] |= (1L << k);
			}
		}
	}

	/* Dump the flags */
	for (i = 0; i < ANGBAND_TERM_MAX; i++) wr_u32b(window_flag[i]);

	/* Dump the masks */
	for (i = 0; i < ANGBAND_TERM_MAX; i++) wr_u32b(window_mask[i]);
}


/*
 * Write some "extra" info
 */
static void wr_extra(void)
{
	int i;

	wr_string(op_ptr->full_name);

	wr_string(p_ptr->died_from);

	wr_string(p_ptr->history);

	/* Race/Class/Gender/Spells */
	wr_byte(p_ptr->prace);
	wr_byte(p_ptr->pclass);
	wr_byte(p_ptr->psex);
	wr_byte(0);	/* oops */

	wr_byte(p_ptr->hitdie);
	wr_byte(p_ptr->expfact);

	wr_s16b(p_ptr->age);
	wr_s16b(p_ptr->ht);
	wr_s16b(p_ptr->wt);

	/* Dump the stats (maximum and current) */
	for (i = 0; i < A_MAX; ++i) wr_s16b(p_ptr->stat_max[i]);
	for (i = 0; i < A_MAX; ++i) wr_s16b(p_ptr->stat_cur[i]);

	/* Ignore the transient stats */
	for (i = 0; i < 12; ++i) wr_s16b(0);

	wr_u16b(p_ptr->fame);

	wr_u32b(p_ptr->au);

	wr_u32b(p_ptr->max_exp);
	wr_u32b(p_ptr->exp);
	wr_u16b(p_ptr->exp_frac);
	wr_s16b(p_ptr->lev);

	wr_s16b(p_ptr->mhp);
	wr_s16b(p_ptr->chp);
	wr_u16b(p_ptr->chp_frac);

	wr_s16b(p_ptr->msp);
	wr_s16b(p_ptr->csp);
	wr_u16b(p_ptr->csp_frac);

	/* Max Player and Dungeon Levels */
	wr_s16b(p_ptr->max_lev);
	wr_s16b(p_ptr->max_depth);
	wr_s16b(p_ptr->recall_depth);

	/* More info */
	wr_s16b(0);	/* oops */
	wr_s16b(0);	/* oops */
	wr_s16b(0);	/* oops */
	wr_s16b(0);	/* oops */
	wr_s16b(p_ptr->sc);
	wr_s16b(0);	/* oops */

	wr_s16b(0);		/* old "rest" */
	wr_s16b(p_ptr->blind);
	wr_s16b(p_ptr->paralyzed);
	wr_s16b(p_ptr->confused);
	wr_s16b(p_ptr->food);
	wr_s16b(0);	/* old "food_digested" */
	wr_s16b(0);	/* old "protection" */
	wr_s16b(p_ptr->p_energy);
	wr_s16b(p_ptr->fast);
	wr_s16b(p_ptr->slow);
	wr_s16b(p_ptr->afraid);
	wr_s16b(p_ptr->cut);
	wr_s16b(p_ptr->stun);
	wr_s16b(p_ptr->poisoned);
	wr_s16b(p_ptr->image);
	wr_s16b(p_ptr->protevil);
	wr_s16b(p_ptr->invuln);
	wr_s16b(p_ptr->hero);
	wr_s16b(p_ptr->shero);
	wr_s16b(p_ptr->shield);
	wr_s16b(p_ptr->blessed);
	wr_s16b(p_ptr->tim_invis);
	wr_s16b(p_ptr->word_recall);
	wr_s16b(p_ptr->see_infra);
	wr_s16b(p_ptr->tim_infra);
	wr_s16b(p_ptr->oppose_fire);
	wr_s16b(p_ptr->oppose_cold);
	wr_s16b(p_ptr->oppose_acid);
	wr_s16b(p_ptr->oppose_elec);
	wr_s16b(p_ptr->oppose_pois);

	wr_s16b(p_ptr->temp_native_lava);
	wr_s16b(p_ptr->temp_native_oil);
	wr_s16b(p_ptr->temp_native_sand);
	wr_s16b(p_ptr->temp_native_forest);
	wr_s16b(p_ptr->temp_native_water);
	wr_s16b(p_ptr->temp_native_mud);

	wr_byte(p_ptr->confusing);
	wr_s16b(p_ptr->slay_elements);
	wr_byte(p_ptr->flying);
	wr_byte(p_ptr->searching);
	wr_byte(0);	/* oops */
	wr_byte(0);	/* oops */
	wr_byte(0);

	/* 4gai use */
	wr_s16b(p_ptr->base_wakeup_chance);
	wr_s16b(total_wakeup_chance);

	/* Save item-quality squelch sub-menu */
	for (i = 0; i < SQUELCH_BYTES; i++) wr_byte(squelch_level[i]);

	/* Store the name of the current greater vault */
	wr_string(g_vault_name);

	/* Save the current number of ego-item types */
	wr_u16b(z_info->e_max);

	/* Save ego-item squelch settings */
	for (i = 0; i < z_info->e_max; i++)
	{
		ego_item_type *e_ptr = &e_info[i];
		byte tmp8u = 0;

		if (e_ptr->squelch) tmp8u |= 0x01;
		if (e_ptr->everseen) tmp8u |= 0x02;

		wr_byte(tmp8u);
	}

	/*Write the current number of auto-inscriptions*/
	wr_u16b(inscriptionsCount);

	/*Write the autoinscriptions array*/
	for(i = 0; i < inscriptionsCount; i++)
	{
		wr_s16b(inscriptions[i].kindIdx);
		wr_string(quark_str(inscriptions[i].inscriptionIdx));
	}

	/* Store the bones file selector, if the player is not dead. -LM- */
	wr_byte(bones_selector);

	/*save the bones template as part of the savefile*/
	if (bones_selector)
	{
		FILE	*fp = FALSE;
		char	path[1024];

		sprintf(path, "%s/bone.%03d", ANGBAND_DIR_BONE, bones_selector);

		/* Attempt to open the bones file. */
		fp = my_fopen(path, "r");

		/*something would have to be very strange for this not to be true*/
		if (fp)
		{
			int ghost_sex = 0, ghost_race = 0, ghost_class = 0;
			bool err = FALSE;

			/* Ghost name is a global variable and is not needed */
			char dummy[80];

			/* XXX XXX XXX Scan the file to get the basic info of the ghost  */
			if (my_fgets(fp, dummy, sizeof(dummy)) ||
				next_line_to_number(fp, &ghost_sex) ||
				next_line_to_number(fp, &ghost_race) ||
				next_line_to_number(fp, &ghost_class))
			{
				err = TRUE;
			}

			/* Close the file */
			my_fclose(fp);

			wr_string(ghost_name);
			wr_byte(ghost_sex);
			wr_byte(ghost_race);
			wr_byte(ghost_class);
		}
	}


	/* Store the number of thefts on the level. -LM- */
	wr_byte(recent_failed_thefts);

	/* Store number of monster traps on this level. -LM- */
	wr_byte(num_trap_on_level);

	/* Future use */
	for (i = 0; i < 13; i++) wr_byte(0);

	/* Random artifact version */
	wr_u32b(RANDART_VERSION);

	/* Random artifact seed */
	wr_u32b(seed_randart);


	/* Ignore some flags */
	wr_u32b(0L);	/* oops */
	wr_u32b(0L);	/* oops */
	wr_u32b(0L);	/* oops */


	/* Write the "object seeds" */
	wr_u32b(seed_flavor);
	wr_u32b(seed_town);


	/* Special stuff */
	wr_u16b(p_ptr->panic_save);
	wr_u16b(p_ptr->total_winner);
	wr_u16b(p_ptr->noscore);


	/* Write death */
	wr_byte(p_ptr->is_dead);

	/* Write feeling */
	wr_byte(feeling);

	/* Turn of last "feeling" */
	wr_byte(do_feeling);

	/* Current turn */
	wr_s32b(turn);

	/*Current Player Turn*/
	wr_s32b(p_ptr->p_turn);

	/* Turn counter for the quest indicator */
	{
		/* Get the timer value. Clear the last bit */
		u16b tmp16u = quest_indicator_timer & ~(QUEST_INDICATOR_COMPLETE_BIT);

		/* Turn on the last bit if the quest was completed */
		if (quest_indicator_complete) tmp16u |= (QUEST_INDICATOR_COMPLETE_BIT);

		/* Write timer + complete bit */
		wr_u16b(tmp16u);
	}

	/* Panel change offsets */
	wr_u16b(panel_change_offset_y);
	wr_u16b(panel_change_offset_x);
}


/*
 * Dump the random artifacts
 */
static void wr_randarts(void)
{
	int i, begin;

	if (adult_rand_artifacts) begin = 0;
	else begin = z_info->art_norm_max;

	wr_u16b(begin);
	wr_u16b(z_info->art_max);
	wr_u16b(z_info->art_norm_max);


	for (i = begin; i < z_info->art_max; i++)
	{
		artifact_type *a_ptr = &a_info[i];

		wr_string(a_ptr->name);

		wr_byte(a_ptr->tval);
		wr_byte(a_ptr->sval);
		wr_s16b(a_ptr->pval);

		wr_s16b(a_ptr->to_h);
		wr_s16b(a_ptr->to_d);
		wr_s16b(a_ptr->to_a);
		wr_s16b(a_ptr->ac);

		wr_byte(a_ptr->dd);
		wr_byte(a_ptr->ds);

		wr_s16b(a_ptr->weight);

		wr_s32b(a_ptr->cost);

		wr_u32b(a_ptr->a_flags1);
		wr_u32b(a_ptr->a_flags2);
		wr_u32b(a_ptr->a_flags3);
		wr_u32b(a_ptr->a_native);

		wr_byte(a_ptr->a_level);
		wr_byte(a_ptr->a_rarity);

		wr_byte(a_ptr->activation);
		wr_u16b(a_ptr->time);
		wr_u16b(a_ptr->randtime);
	}

}

/*
 * Write the notes into the savefile. Every savefile has at least NOTES_MARK.
 */
static void wr_notes(void)
{
	char end_note[80];

	/* Paranoia */
	if (adult_take_notes && notes_file)
	{
    	char tmpstr[100];

    	my_fclose(notes_file);

      	/* Re-open for readding */
    	notes_file = my_fopen(notes_fname, "r");

    	while (TRUE)
    	{
			/* Read the note from the tempfile */
			if (my_fgets(notes_file, tmpstr, sizeof(tmpstr)))
			{
				/* Found the end */
				break;
			}

			/* Paranoia */
			if (strcmp(tmpstr, NOTES_MARK) == 0) continue;

      		/* Write it into the savefile */
      		wr_string(tmpstr);
    	}

    	my_fclose(notes_file);

    	/* Re-open for appending */
    	notes_file = my_fopen(notes_fname, "a");
  	}

	my_strcpy(end_note, NOTES_MARK, sizeof(end_note));

  	/* Always write NOTES_MARK */
  	wr_string(end_note);
}


/*
 * Write variable extensions to the savefile
 */
static void wr_extensions(void)
{
	/* Write call huorns time if present */
	if (p_ptr->temp_call_huorns > 0)
	{
		/* Write extension id */
		wr_s16b(EXTENSION_CALL_HUORNS);
		/* Write field type */
		wr_byte(EXTENSION_TYPE_U16B);
		/* Write field value */
		wr_u16b(p_ptr->temp_call_huorns);
		/* Write end mark for fields */
		wr_byte(EXTENSION_TYPE_END);
	}

	/* Write end mark for extensions */
	wr_s16b(END_EXTENSIONS);
}


/*
 * The cave grid flags that get saved in the savefile
 */
#define IMPORTANT_FLAGS (CAVE_MARK | CAVE_GLOW | CAVE_ICKY | \
						 CAVE_ROOM | CAVE_MARKED | CAVE_G_VAULT)


/*
 * Write the current dungeon
 */
static void wr_dungeon(void)
{
	int i, y, x;

	byte tmp8u;

	byte count;
	byte prev_char;


	/*** Basic info ***/

	/* Dungeon specific info follows */
	wr_u16b(p_ptr->depth);
	wr_u16b(p_ptr->dungeon_type);
	wr_u16b(p_ptr->py);
	wr_u16b(p_ptr->px);
	wr_byte(p_ptr->cur_map_hgt);
	wr_byte(p_ptr->cur_map_wid);
	wr_u16b(0);
	wr_u16b(0);


	/*** Simple "Run-Length-Encoding" of cave ***/

	/* Note that this will induce two wasted bytes */
	count = 0;
	prev_char = 0;

	/* Dump the cave */
	for (y = 0; y < p_ptr->cur_map_hgt; y++)
	{
		for (x = 0; x < p_ptr->cur_map_wid; x++)
		{
			/* Extract the important cave_info flags */
			tmp8u = (cave_info[y][x] & (IMPORTANT_FLAGS));

			/* If the run is broken, or too full, flush it */
			if ((tmp8u != prev_char) || (count == MAX_UCHAR))
			{
				wr_byte((byte)count);
				wr_byte((byte)prev_char);
				prev_char = tmp8u;
				count = 1;
			}

			/* Continue the run */
			else
			{
				count++;
			}
		}
	}

	/* Flush the data (if any) */
	if (count)
	{
		wr_byte((byte)count);
		wr_byte((byte)prev_char);
	}


	/*** Simple "Run-Length-Encoding" of cave ***/

	/* Note that this will induce two wasted bytes */
	count = 0;
	prev_char = 0;

	/* Dump the cave */
	for (y = 0; y < p_ptr->cur_map_hgt; y++)
	{
		for (x = 0; x < p_ptr->cur_map_wid; x++)
		{
			/* Extract a byte */
			tmp8u = cave_feat[y][x];

			/* If the run is broken, or too full, flush it */
			if ((tmp8u != prev_char) || (count == MAX_UCHAR))
			{
				wr_byte((byte)count);
				wr_byte((byte)prev_char);
				prev_char = tmp8u;
				count = 1;
			}

			/* Continue the run */
			else
			{
				count++;
			}
		}
	}

	/* Flush the data (if any) */
	if (count)
	{
		wr_byte((byte)count);
		wr_byte((byte)prev_char);
	}


	/*** Compact ***/

	/* Compact the objects */
	compact_objects(0);

	/* Compact the monsters */
	compact_monsters(0);


	/*** Dump objects ***/

	/* Total objects */
	wr_u16b(o_max);

	/* Dump the objects */
	for (i = 1; i < o_max; i++)
	{
		object_type *o_ptr = &o_list[i];

		/* Dump it */
		wr_item(o_ptr);
	}


	/*** Dump the monsters ***/

	/* Total monsters */
	wr_u16b(mon_max);

	/* Dump the monsters */
	for (i = 1; i < mon_max; i++)
	{
		monster_type *m_ptr = &mon_list[i];

		/* Dump it */
		wr_monster(m_ptr);
	}

	/*** Dump the effects ***/

	/* Total Effects */
	wr_u16b(x_max);

	/* Dump the Effects */
	for (i = 1; i < x_max; i++)
	{
		effect_type *x_ptr = &x_list[i];

		/* Dump it */
		wr_effect(x_ptr);
	}
}



/*
 * Actually write a save-file
 */
static bool wr_savefile_new(void)
{
	int i;

	u32b now;

	u16b tmp16u;


	/* Guess at the current time */
	now = time((time_t *)0);


	/* Note the operating system */
	sf_xtra = 0L;

	/* Note when the file was saved */
	sf_when = now;

	/* Note the number of saves */
	sf_saves++;


	/*** Actually write the file ***/

	/* Dump the file header */
	xor_byte = 0;
	wr_byte(VERSION_MAJOR);
	xor_byte = 0;
	wr_byte(VERSION_MINOR);
	xor_byte = 0;
	wr_byte(VERSION_PATCH);
	xor_byte = 0;
	wr_byte(VERSION_EXTRA);


	/* Reset the checksum */
	v_stamp = 0L;
	x_stamp = 0L;


	/* Operating system */
	wr_u32b(sf_xtra);

	/* Time file last saved */
	wr_u32b(sf_when);

	/* Number of past lives */
	wr_u16b(sf_lives);

	/* Number of times saved */
	wr_u16b(sf_saves);


	/* Space */
	wr_u32b(0L);
	wr_u32b(0L);


	/* Write the RNG state */
	wr_randomizer();


	/* Write the boolean "options" */
	wr_options();


	/* Dump the number of "messages" */
	tmp16u = message_num();
	if (compress_savefile && (tmp16u > 40)) tmp16u = 40;
	wr_u16b(tmp16u);

	/* Dump the messages (oldest first!) */
	for (i = tmp16u - 1; i >= 0; i--)
	{
		wr_string(message_str((s16b)i));
		wr_u16b(message_type((s16b)i));
	}


	/* Dump the monster lore */
	tmp16u = z_info->r_max;
	wr_u16b(tmp16u);
	for (i = 0; i < tmp16u; i++) wr_monster_lore(i);

	/* Dump the object memory */
	tmp16u = z_info->k_max;
	wr_u16b(tmp16u);
	for (i = 0; i < tmp16u; i++) wr_xtra(i);

	/* Hack -- Dump the quests */
	tmp16u = z_info->q_max;
	wr_u16b(tmp16u);
	for (i = 0; i < tmp16u; i++)
	{
		wr_byte(q_info[i].type);

		if ((q_info[i].type == QUEST_FIXED) || (q_info[i].type == QUEST_FIXED_U))
		{
			wr_byte(q_info[i].active_level);
			wr_s16b(q_info[i].cur_num);
		}

		else if ((q_info[i].type == QUEST_MONSTER) || (q_info[i].type == QUEST_UNIQUE))
		{
			wr_byte(q_info[i].reward);
			wr_byte(q_info[i].active_level);
			wr_byte(q_info[i].base_level);

			wr_s16b(q_info[i].mon_idx);

			wr_s16b(q_info[i].cur_num);
			wr_s16b(q_info[i].max_num);
			wr_byte(q_info[i].started);
		}
		else if (q_info[i].type == QUEST_VAULT)
		{
			wr_byte(q_info[i].reward);
			wr_byte(q_info[i].active_level);
			wr_byte(q_info[i].base_level);
			wr_byte(q_info[i].started);
		}
		else if ((q_info[i].type == QUEST_THEMED_LEVEL) ||
			     (q_info[i].type == QUEST_NEST) ||
			     (q_info[i].type == QUEST_PIT))
		{
			wr_byte(q_info[i].reward);
			wr_byte(q_info[i].active_level);
			wr_byte(q_info[i].base_level);
			wr_byte(q_info[i].theme);
			wr_s16b(q_info[i].cur_num);
			wr_s16b(q_info[i].max_num);
			wr_byte(q_info[i].started);
		}

	}

	/* Hack -- Dump the artifacts */
	tmp16u = z_info->art_max;
	wr_u16b(tmp16u);
	for (i = 0; i < tmp16u; i++)
	{
		artifact_type *a_ptr = &a_info[i];
		wr_byte(a_ptr->a_cur_num);
		wr_byte(0);
		wr_byte(0);
		wr_byte(0);
	}


	/* Write the "extra" information */
	wr_extra();


	/* Dump the "player hp" entries */
	tmp16u = PY_MAX_LEVEL;
	wr_u16b(tmp16u);
	for (i = 0; i < tmp16u; i++)
	{
		wr_s16b(p_ptr->player_hp[i]);
	}

	/* Write spell data */
	wr_u16b(PY_MAX_SPELLS);

	for (i = 0; i < PY_MAX_SPELLS; i++)
	{
		wr_byte(p_ptr->spell_flags[i]);
	}

	/* Dump the ordered spells */
	for (i = 0; i < PY_MAX_SPELLS; i++)
	{
		wr_byte(p_ptr->spell_order[i]);
	}


	/*Write the randarts*/
	wr_randarts();

	/*Copy the notes file into the savefile*/
	wr_notes();

	/* Extensions */
	wr_extensions();

	/* Write the inventory */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		object_type *o_ptr = &inventory[i];

		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;

		/* Dump index */
		wr_u16b((u16b)i);

		/* Dump object */
		wr_item(o_ptr);
	}

	/* Add a sentinel */
	wr_u16b(0xFFFF);


	/* Note the stores */
	tmp16u = MAX_STORES;
	wr_u16b(tmp16u);

	/* Dump the stores */
	for (i = 0; i < tmp16u; i++) wr_store(&store[i]);

	/* Dump the current number of terrain features */
	tmp16u = z_info->f_max;
	wr_u16b(tmp16u);

	/* Dump terrain lore */
	for (i = 0; i < tmp16u; i++) wr_feature_lore(i);

	/* Dump the current number of artifacts (normal + special) */
	tmp16u = z_info->art_norm_max;
	wr_u16b(tmp16u);

	/* Dump artifact lore */
	for (i = 0; i < tmp16u; i++) wr_artifact_lore(i);

	/* Player is not dead, write the dungeon */
	if (!p_ptr->is_dead)
	{
		/* Dump the dungeon */
		wr_dungeon();
	}

	/* Write the "value check-sum" */
	wr_u32b(v_stamp);

	/* Write the "encoded checksum" */
	wr_u32b(x_stamp);


	/* Error in save */
	if (ferror(fff) || (fflush(fff) == EOF)) return FALSE;

	/* Successful save */
	return TRUE;
}


/*
 * Medium level player saver
 *
 * XXX XXX XXX Angband 2.8.0 will use "fd" instead of "fff" if possible
 */
static bool save_player_aux(cptr name)
{
	bool ok = FALSE;

	int fd;

	int mode = 0644;


	/* No file yet */
	fff = NULL;


	/* File type is "SAVE" */
	FILE_TYPE(FILE_TYPE_SAVE);


	/* Grab permissions */
	safe_setuid_grab();

	/* Create the savefile */
	fd = fd_make(name, mode);

	/* Drop permissions */
	safe_setuid_drop();

	/* File is okay */
	if (fd >= 0)
	{
		/* Close the "fd" */
		fd_close(fd);

		/* Grab permissions */
		safe_setuid_grab();

		/* Open the savefile */
		fff = my_fopen(name, "wb");

		/* Drop permissions */
		safe_setuid_drop();

		/* Successful open */
		if (fff)
		{
			/* Write the savefile */
			if (wr_savefile_new()) ok = TRUE;

			/* Attempt to close it */
			if (my_fclose(fff)) ok = FALSE;
		}

		/* Grab permissions */
		safe_setuid_grab();

		/* Remove "broken" files */
		if (!ok) fd_kill(name);

		/* Drop permissions */
		safe_setuid_drop();
	}


	/* Failure */
	if (!ok) return (FALSE);

	/* Successful save */
	character_saved = TRUE;

	/* Success */
	return (TRUE);
}



/*
 * Attempt to save the player in a savefile
 */
bool save_player(void)
{
	int result = FALSE;

	char safe[1024];

	/* New savefile */
	my_strcpy(safe, savefile, sizeof(safe));
	strcat(safe, ".new");

#ifdef VM
	/* Hack -- support "flat directory" usage on VM/ESA */
	my_strcpy(safe, savefile, sizeof(safe));
	strcat(safe, "n");
#endif /* VM */

	/* Grab permissions */
	safe_setuid_grab();

	/* Remove it */
	fd_kill(safe);

	/* Drop permissions */
	safe_setuid_drop();

	/* Attempt to save the player */
	if (save_player_aux(safe))
	{
		char temp[1024];

		/* Old savefile */
		my_strcpy(temp, savefile, sizeof(temp));
		strcat(temp, ".old");

#ifdef VM
		/* Hack -- support "flat directory" usage on VM/ESA */
		my_strcpy(temp, savefile, sizeof(temp));
		strcat(temp, "o");
#endif /* VM */

		/* Grab permissions */
		safe_setuid_grab();

		/* Remove it */
		fd_kill(temp);

		/* Preserve old savefile */
		fd_move(savefile, temp);

		/* Activate new savefile */
		fd_move(safe, savefile);

		/* Remove preserved savefile */
		fd_kill(temp);

		/* Drop permissions */
		safe_setuid_drop();

		/* Hack -- Pretend the character was loaded */
		character_loaded = TRUE;

#ifdef VERIFY_SAVEFILE

		/* Lock on savefile */
		my_strcpy(temp, savefile, sizeof(temp));
		strcat(temp, ".lok");

		/* Grab permissions */
		safe_setuid_grab();

		/* Remove lock file */
		fd_kill(temp);

		/* Drop permissions */
		safe_setuid_drop();

#endif /* VERIFY_SAVEFILE */

		/* Success */
		result = TRUE;
	}


	/* Return the result */
	return (result);
}

