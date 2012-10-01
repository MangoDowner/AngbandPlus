/* File: dump_items.c */

/*
 * Copyright (c) 2004 Jeff Greene, Leon Marrick
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */




#include "angband.h"

#ifdef ALLOW_DATA_DUMP


/*dumps any u32b flags to be easily parsed by a spreadsheet or database*/
static void dump_flags(FILE *fff, u32b flag, int whatflag, int counter)
{
	int i;

	u32b is_true = 0x00000001;

	/*dump the monster/object/artifact number*/
	fprintf(fff, "flags%d:%d:", whatflag, counter);

	/* using 32 assumes a u32b flag size*/
	for (i = 1; i <= 32; i++)
	{

		if (flag & is_true)
		{
			/*dump T for True*/
			fprintf(fff, ":T");
		}

		/*dump F for True*/
		else fprintf(fff, ":F");

		/*shift one bit*/
		is_true = is_true << 1;
	}

	fprintf(fff, "\n");
}

/*
 * Use the monster racial information to a format easily parsed by a spreadsheet.
 *
 * Original function by -EB- (probably), revisions by -LM- & JG.
 */
void write_r_info_txt(void)
{
	int i, j, bc;
	int dlen;

	char buf[1024];

	int fd;
	FILE *fff = NULL;

	cptr desc;

	monster_race *r_ptr;

	/* We allow 75 characters on the line (plus 2) */
	u16b line_length = 75;

	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_EDIT, "r_output.txt");

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Check for existing file */
	fd = fd_open(buf, O_RDONLY);

	/* Existing file */
	if (fd >= 0)
	{
		char out_val[160];

		/* Close the file */
		fd_close(fd);

		/* Build query */
		sprintf(out_val, "Replace existing file %s? ", buf);

		/* Ask */
		if (get_check(out_val)) fd = -1;
	}

	/* Open the non-existing file */
	if (fd < 0) fff = my_fopen(buf, "w");

	/* No output file - fail */
	if (!fff) return;

	/* Write a note */
	fprintf(fff, "# File: r_info.txt (autogenerated)\n\n");

	/* Write a note */
	fprintf(fff, "##### The Player #####\n\n");

	/* Read and print out all the monsters */
	for (i = 0; i < z_info->r_max; i++)
	{
		char what_char;

		cptr color_name;

		int counter = 1;

		/* Get the monster */
		r_ptr = &r_info[i];

		/* Ignore empty monsters */
		if (!strlen(r_name + r_ptr->name))
		{
			fprintf(fff, "## empty space (available for monsters) ##\n\n");
			continue;
		}

		/* Ignore useless monsters */
		if (i && !r_ptr->speed)
		{
			fprintf(fff, "## empty space (available for monsters) ##\n\n");
			continue;
		}

		/* Perform any translations */

		/* Write New/Number/Name */
		fprintf(fff, "N:%d:%d:%s\n", i, i, r_name + r_ptr->name);

		/*get the color name*/
		color_name = get_ext_color_name(r_ptr->d_attr);

		/* Write G: line */
		if (r_ptr->rarity) fprintf(fff, "G:%d:%c:%s \n",
			i, r_ptr->d_char, color_name);

		/*don't do rest for player*/
		if (i == 0)
		{
			/* Write a note */
			fprintf(fff, "\n\n ##### The Monsters #####\n\n");

			continue;
		}


		/* Write I: line */
		fprintf(fff, "I:%d:%d:%dd%d:%d:%d:%d\n", i,
		r_ptr->speed, r_ptr->hdice, r_ptr->hside,
		r_ptr->aaf, r_ptr->ac, r_ptr->sleep);

		/* Write W: line */
		fprintf(fff, "W:%d:%d::%d:%d:%ld\n", i, r_ptr->level, r_ptr->rarity,
		    r_ptr->mana, r_ptr->mexp);

		/* Write blows */
		for(j = 0; j < 4; j++)
		{

			/* Write this blow */
			fprintf(fff, format("B-%d:%d:%d:%d:%dd%d\n", j, i,
				r_ptr->blow[j].method, r_ptr->blow[j].effect,
				r_ptr->blow[j].d_dice, r_ptr->blow[j].d_side));
		}

		what_char = 'F';

		/* Get the flags, store flag text in a format easily parsed by a
		 * database, but pretty much illegible to a person.
		 */
		dump_flags(fff, r_ptr->flags1, 1, i);
		dump_flags(fff, r_ptr->flags2, 2, i);
		dump_flags(fff, r_ptr->flags3, 3, i);
		dump_flags(fff, r_ptr->flags4, 4, i);
		dump_flags(fff, r_ptr->flags5, 5, i);
		dump_flags(fff, r_ptr->flags6, 6, i);

		/* Write the terrain native flag. */
		what_char = 'T';

		dump_flags(fff, r_ptr->r_native, 8, i);

		fprintf(fff, "S:%d:%d:%d\n", i, r_ptr->freq_ranged, r_ptr->spell_power);

		/*now the summon flag*/
		what_char = 'S';

		dump_flags(fff, r_ptr->flags7, 7, i);


		/* Acquire the description */
		desc = r_text + r_ptr->text;
		dlen = strlen(desc);

		/* Write Description */
		for (j = 0; j < dlen;)
		{

			char buf[160], *t;

			/* Initialize */
			t = buf;
			bc = 0;

			/* Build this line */
			while (TRUE)
			{
				/* Insert this character, count it */
				*t++ = desc[j++];

				/* Oops.  Line is too long. */
				if (bc++ >= line_length)
				{
					/* Parse backwards until we find a space */
					while (!my_isspace(desc[j-1]) && (bc-- > 40))
					{
						j--;
						t--;
					}

					/* Time to end this line */
					*t++ = '\0';
					break;
				}

				/* All done */
				if (j >= dlen)
				{
					/* Time to end this line */
					*t++ = '\0';
					break;
				}
			}

			/* Done with this line; write it */
			fprintf(fff, "D-%d:%d:%s\n", counter, i, buf);

			counter++;
		}

		/*
		 * Print out empty lines, so all monsters have
		 * an equal number of lines
		 * makes parsing and combining the description easier
		 */
		for (; counter <+ 12; counter++)
		{
			fprintf(fff, "D-%d:%d\n", counter, i);
		}

		/* Space between entries */
		fprintf(fff, "\n\n\n");

	}

	/* Done */
	fclose(fff);
}

/*
 * Dump the object information a format easily parsed by a spreadsheet.
 *
 * Original function by -EB- (probably), revisions by -LM- & JG.
 */
void write_o_info_txt(void)
{
	int i, j, bc;
	int dlen;

	char buf[1024];

	int fd;
	FILE *fff = NULL;

	cptr desc;

	object_kind *k_ptr;

	/* We allow 75 characters on the line (plus 2) */
	u16b line_length = 75;

	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_EDIT, "o_output.txt");

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Check for existing file */
	fd = fd_open(buf, O_RDONLY);

	/* Existing file */
	if (fd >= 0)
	{
		char out_val[160];

		/* Close the file */
		fd_close(fd);

		/* Build query */
		sprintf(out_val, "Replace existing file %s? ", buf);

		/* Ask */
		if (get_check(out_val)) fd = -1;
	}

	/* Open the non-existing file */
	if (fd < 0) fff = my_fopen(buf, "w");

	/* No output file - fail */
	if (!fff) return;

	/* Write a note */
	fprintf(fff, "# File: o_info.txt (autogenerated)\n\n");

	/* Read and print out all the objects */
	for (i = 0; i < z_info->k_max; i++)
	{

		int counter = 1;

		cptr color_name;

		/* Get the object */
		k_ptr = &k_info[i];

		/* Ignore unused objects */
		if (!strlen((k_name + k_ptr->name)))
		{
			fprintf(fff, "## empty space (available for object) ##\n\n");
			continue;
		}

		/* Perform any translations */

		/* Write New/Number/Name */
		fprintf(fff, "N:%d:%s\n", i, k_name + k_ptr->name);

		/*get the color name*/
		color_name = get_ext_color_name(k_ptr->d_attr);

		/* Write G: line */
		fprintf(fff, "G:%d:%c:%s\n", i, k_ptr->d_char, color_name);

		/*don't do the rest for the pile symbol*/
		if (i == 0)
		{

			/* Write G: line */
			fprintf(fff, "\n\n\n");
			continue;
		}

		/* Write I: line */
		fprintf(fff, "I:%d:%d:%d:%d\n", i, k_ptr->tval, k_ptr->sval, k_ptr->pval);

		/* Write W: line */
		fprintf(fff, "W:%d:%d:%d:%d\n", i, k_ptr->k_level, k_ptr->weight, k_ptr->cost);

		/* Write P: line */
		fprintf(fff, "P:%d:%d:%d:d:%d:%d:%d:%d\n", i, k_ptr->ac, k_ptr->dd, k_ptr->ds,
						k_ptr->to_h, k_ptr->to_d, k_ptr->to_a);

		/* Write this A line */
		fprintf(fff, "A:%d:%d:%d:%d:%d:%d:%d\n", i,
				k_ptr->locale[0], k_ptr->chance[0],
				k_ptr->locale[1], k_ptr->chance[1],
				k_ptr->locale[2], k_ptr->chance[2]);

		/* Get the flags, store flag text in a format easily parsed by a
		 * database, but pretty much illegible to a person.
		 */
		dump_flags(fff, k_ptr->k_flags1, 1, i);
		dump_flags(fff, k_ptr->k_flags2, 2, i);
		dump_flags(fff, k_ptr->k_flags3, 3, i);
		dump_flags(fff, k_ptr->k_native, 4, i);

		/* Acquire the description */
		desc = k_text + k_ptr->text;
		dlen = strlen(desc);

		if (dlen < 1)
		{
			/* Space between entries */
			fprintf(fff, "\n\n\n");
			continue;
		}

		/* Write Description */
		for (j = 0; j < dlen;)
		{

			char buf[160], *t;

			/* Initialize */
			t = buf;
			bc = 0;

			/* Build this line */
			while (TRUE)
			{
				/* Insert this character, count it */
				*t++ = desc[j++];

				/* Oops.  Line is too long. */
				if (bc++ >= line_length)
				{
					/* Parse backwards until we find a space */
					while (!my_isspace(desc[j-1]) && (bc-- > 40))
					{
						j--;
						t--;
					}

					/* Time to end this line */
					*t++ = '\0';
					break;
				}

				/* All done */
				if (j >= dlen)
				{
					/* Time to end this line */
					*t++ = '\0';
					break;
				}
			}

			/* Done with this line; write it */
			fprintf(fff, "D-%d:%d:%s\n", counter, i, buf);

			counter++;
		}

		/*
		 * Print out empty lines, so all objects have
		 * an equal number of lines
		 * makes parsing and combining the description easier
		 */
		for (; counter <+ 9; counter++)
		{
			fprintf(fff, "D-%d:%d\n", counter, i);
		}

		/* Space between entries */
		fprintf(fff, "\n\n\n");

	}

	/* Done */
	fclose(fff);
}

/*
 * Dump the ego-item information in a format easily parsed by a spreadsheet.
 *
 * Original function by -EB- (probably), revisions by -LM- & JG.
 */
void write_e_info_txt(void)
{
	int i, j, bc;
	int dlen;

	char buf[1024];

	int fd;
	FILE *fff = NULL;

	cptr desc;

	ego_item_type *e_ptr;

	/* We allow 75 characters on the line (plus 2) */
	u16b line_length = 75;

	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_EDIT, "e_output.txt");

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Check for existing file */
	fd = fd_open(buf, O_RDONLY);

	/* Existing file */
	if (fd >= 0)
	{
		char out_val[160];

		/* Close the file */
		fd_close(fd);

		/* Build query */
		sprintf(out_val, "Replace existing file %s? ", buf);

		/* Ask */
		if (get_check(out_val)) fd = -1;
	}

	/* Open the non-existing file */
	if (fd < 0) fff = my_fopen(buf, "w");

	/* No output file - fail */
	if (!fff) return;

	/* Write a note */
	fprintf(fff, "# File: e_info.txt (autogenerated)\n\n");

	/* Read and print out all the objects */
	for (i = 0; i < z_info->e_max; i++)
	{

		int counter = 1;

		/* Get the ego-item */
		e_ptr = &e_info[i];

		/* Ignore empty monsters */
		if (!strlen((e_name + e_ptr->name)))
		{
			fprintf(fff, "## empty space (available for ego-item) ##\n\n");
			continue;
		}

		/* Perform any translations */

		/* Write New/Number/Name */
		fprintf(fff, "N:%d:%s\n", i, e_name + e_ptr->name);

		/* Write X: line */
		fprintf(fff, "X:%d:%d:%d\n", i, e_ptr->rating, e_ptr->xtra);

		/* Write C: line */
		fprintf(fff, "C:%d:%d:%d:%d:%d\n", i, e_ptr->max_to_h, e_ptr->max_to_d,
										   e_ptr->max_to_a, e_ptr->max_pval);

		/* Write W: line */
		fprintf(fff, "W:%d:%d:%d:0:%d\n", i, e_ptr->level, e_ptr->rarity, e_ptr->cost);

		/* Write the T lines */
		fprintf(fff, "T:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d\n", i,
				e_ptr->tval[0], e_ptr->min_sval[0], e_ptr->max_sval[0],
				e_ptr->tval[1], e_ptr->min_sval[1], e_ptr->max_sval[1],
				e_ptr->tval[2], e_ptr->min_sval[2], e_ptr->max_sval[2]);

		/* Get the flags, store flag text in a format easily parsed by a
		 * database, but pretty much illegible to a person.
		 */
		dump_flags(fff, e_ptr->flags1, 1, i);
		dump_flags(fff, e_ptr->flags2, 2, i);
		dump_flags(fff, e_ptr->flags3, 3, i);
		dump_flags(fff, e_ptr->e_native, 4, i);

		/* Acquire the description */
		desc = e_text + e_ptr->text;
		dlen = strlen(desc);

		if (dlen < 1)
		{
			/* Space between entries */
			fprintf(fff, "\n\n\n");
			continue;
		}

		/* Write Description */
		for (j = 0; j < dlen;)
		{

			char buf[160], *t;

			/* Initialize */
			t = buf;
			bc = 0;

			/* Build this line */
			while (TRUE)
			{
				/* Insert this character, count it */
				*t++ = desc[j++];

				/* Oops.  Line is too long. */
				if (bc++ >= line_length)
				{
					/* Parse backwards until we find a space */
					while (!my_isspace(desc[j-1]) && (bc-- > 40))
					{
						j--;
						t--;
					}

					/* Time to end this line */
					*t++ = '\0';
					break;
				}

				/* All done */
				if (j >= dlen)
				{
					/* Time to end this line */
					*t++ = '\0';
					break;
				}
			}

			/* Done with this line; write it */
			fprintf(fff, "D-%d:%d:%s\n", counter, i, buf);

			counter++;
		}

		/*
		 * Print out empty lines, so all objects have
		 * an equal number of lines
		 * makes parsing and combining the description easier
		 */
		for (; counter <+ 6; counter++)
		{
			fprintf(fff, "D-%d:%d\n", counter, i);
		}

		/* Space between entries */
		fprintf(fff, "\n\n\n");

	}

	/* Done */
	fclose(fff);
}

/*
 * Use the monster racial information to a format easily parsed by a spreadsheet.
 *
 * Original function by -EB- (probably), revisions by -LM- & JG.
 * Note: This function needs some updating as it is missing some fields.
 */
void write_f_info_txt(void)
{
	int i, j, bc, x;
	int dlen;

	char buf[1024];

	int fd;
	FILE *fff = NULL;

	cptr desc;

	feature_type *f_ptr;

	/* We allow 75 characters on the line (plus 2) */
	u16b line_length = 75;

	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_EDIT, "f_output.txt");

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Check for existing file */
	fd = fd_open(buf, O_RDONLY);

	/* Existing file */
	if (fd >= 0)
	{
		char out_val[160];

		/* Close the file */
		fd_close(fd);

		/* Build query */
		sprintf(out_val, "Replace existing file %s? ", buf);

		/* Ask */
		if (get_check(out_val)) fd = -1;
	}

	/* Open the non-existing file */
	if (fd < 0) fff = my_fopen(buf, "w");

	/* No output file - fail */
	if (!fff) return;

	/* Write a note */
	fprintf(fff, "# File: f_info.txt (autogenerated)\n\n");

	/* Read and print out all the features */
	for (i = 0; i < z_info->f_max; i++)
	{

		cptr color_name;

		int counter = 1;

		desc = NULL;

		/* Get the monster */
		f_ptr = &f_info[i];

		/* Ignore empty monsters */
		if (!strlen(f_name + f_ptr->name))
		{
			fprintf(fff, "## empty space (available for terrain) ##\n\n");
			continue;
		}

		/* Perform any translations */

		/* Write New/Number/Name */
		fprintf(fff, "N:%d:%d:%s\n", i, i, f_name + f_ptr->name);

		/* Write Mimic  */
		if (f_ptr->f_mimic > 0)
		{
			/*hack, switch to a new f_ptr- t outpit out the name*/
			f_ptr = &f_info[f_info[i].f_mimic];
			fprintf(fff, "M:%d:%s\n", i, f_name + f_ptr->name);
			/* Switch Back*/
			f_ptr = &f_info[i];
		}

		/* Write Edge  */
		if (f_ptr->f_edge > 0)
		{
			/*hack, switch to a new f_ptr- t outpit out the name*/
			f_ptr = &f_info[f_info[i].f_edge];
			fprintf(fff, "E:%d:%s\n", i, f_name + f_ptr->name);
			/* Switch Back*/
			f_ptr = &f_info[i];
		}

		/*get the color name*/
		color_name = get_ext_color_name(f_ptr->d_attr);

		/* Write G: line */
		fprintf(fff, "G:%d:%c:%s\n",i, f_ptr->d_char, color_name);

		if (f_ptr->f_edge > 0)
		{
			/*hack, switch to a new f_ptr- t outpit out the name*/
			f_ptr = &f_info[f_info[i].f_edge];
			desc = (f_name + f_ptr->name);
			/* Switch Back*/
			f_ptr = &f_info[i];

		}
		else desc = NULL;

		/* Write W: line */
		fprintf(fff, "W:%d:%d:%d:%d:%d:\n", i, f_ptr->f_level, f_ptr->f_rarity,
	    	f_ptr->priority, f_ptr->f_power);

		/* Write C: line */
		fprintf(fff, "C:%d:%d:%d:%d:%d:%d:%d:\n", i, f_ptr->dam_non_native, f_ptr->native_energy_move,
	   	 	f_ptr->non_native_energy_move, f_ptr->native_to_hit_adj,
			f_ptr->non_native_to_hit_adj, f_ptr->f_stealth_adj);


		/* Write X: line */
		fprintf(fff, "X:%d:%d:%d:%d:%d:%d:%d:%d:\n", i, f_ptr->f_level, f_ptr->f_rarity,
	   	 	f_ptr->f_power, f_ptr->x_damage, f_ptr->x_gf_type,
			f_ptr->x_timeout_set, f_ptr->x_timeout_rand);


		/* Get the flags, store flag text in a format easily parsed by a
		 * database, but pretty much illegible to a person.
		 */
		dump_flags(fff, f_ptr->f_flags1, 1, i);
		dump_flags(fff, f_ptr->f_flags2, 2, i);
		dump_flags(fff, f_ptr->f_flags3, 3, i);

		/*Drop the default*/
		fprintf(fff, "DEFAULT:%d:%d\n", i, f_ptr->defaults);

		/* Find the next empty state slot (if any) */
		for (x = 0; x < MAX_FEAT_STATES; x++)
		{

			if (f_ptr->state[x].fs_action != FS_FLAGS_END)
			{
				char action_desc[80];

				get_feature_name(action_desc, sizeof(action_desc),
											f_ptr->state[x].fs_action);

				if (f_ptr->state[x].fs_result > 0)
				{
					/*Get the name of terrain switching to*/
					f_ptr = &f_info[f_ptr->state[x].fs_result];
					desc = (f_name + f_ptr->name);
					/* Switch Back*/
					f_ptr = &f_info[i];

				}
				else desc = NULL;

				/*drop the "K" line*/
				fprintf(fff, "K%d:%d:%s:%s:%d\n", i, x, action_desc, desc, f_ptr->state[x].fs_power);
			}
		}

		/* Acquire the description */
		desc = f_text + f_ptr->f_text;
		dlen = strlen(desc);

		/* Write Description */
		for (j = 0; j < dlen;)
		{

			char buf[160], *t;

			/* Initialize */
			t = buf;
			bc = 0;

			/* Build this line */
			while (TRUE)
			{
				/* Insert this character, count it */
				*t++ = desc[j++];

				/* Oops.  Line is too long. */
				if (bc++ >= line_length)
				{
					/* Parse backwards until we find a space */
					while (!my_isspace(desc[j-1]) && (bc-- > 40))
					{
						j--;
						t--;
					}

					/* Time to end this line */
					*t++ = '\0';
					break;
				}

				/* All done */
				if (j >= dlen)
				{
					/* Time to end this line */
					*t++ = '\0';
					break;
				}
			}

			/* Done with this line; write it */
			fprintf(fff, "D-%d:%d:%s\n", counter, i, buf);

			counter++;
		}

		/*
		 * Print out empty lines, so all monsters have
		 * an equal number of lines
		 * makes parsing and combining the description easier
		 */
		for (; counter <+ 3; counter++)
		{
			fprintf(fff, "D-%d:%d\n", counter, i);
		}

		/* Space between entries */
		fprintf(fff, "\n\n\n");

	}

	/* Done */
	fclose(fff);
}


/*
 * Dump the object information a format easily parsed by a spreadsheet.
 *
 * Original function by -EB- (probably), revisions by -LM- & JG.
 */
void write_a_info_txt(void)
{
	int i, j, bc;
	int dlen;

	char buf[1024];



	int fd;
	FILE *fff = NULL;

	cptr desc;

	artifact_type *a_ptr;

	/* We allow 75 characters on the line (plus 2) */
	u16b line_length = 75;

	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_EDIT, "a_output.txt");

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Check for existing file */
	fd = fd_open(buf, O_RDONLY);

	/* Existing file */
	if (fd >= 0)
	{
		char out_val[160];

		/* Close the file */
		fd_close(fd);

		/* Build query */
		sprintf(out_val, "Replace existing file %s? ", buf);

		/* Ask */
		if (get_check(out_val)) fd = -1;
	}

	/* Open the non-existing file */
	if (fd < 0) fff = my_fopen(buf, "w");

	/* No output file - fail */
	if (!fff) return;

	/* Write a note */
	fprintf(fff, "# File: a_info.txt (autogenerated)\n\n");

	/* Read and print out all the objects */
	for (i = 0; i < z_info->art_norm_max; i++)
	{

		int counter = 1;
		char o_name[80];
		object_type *i_ptr;
		object_type object_type_body;

		/* Get local object */
		i_ptr = &object_type_body;

		/* Get the object */
		a_ptr = &a_info[i];

		/* Ignore unused objects */
		if (!strlen(a_ptr->name))
		{
			fprintf(fff, "## empty space (available for artifact) ##\n\n");
			continue;
		}

		/* Perform any translations */

		/* Write New/Number/Name */
		fprintf(fff, "N:%d:%s\n", i, a_ptr->name);

		/* Write the complete name of the artifact*/
		make_fake_artifact(i_ptr, i);

		/*identify it*/
		/* Identify it */
		object_aware(i_ptr);
		object_known(i_ptr);
		i_ptr->ident |= (IDENT_MENTAL);

		/* Get a description to dump */
		object_desc(o_name, sizeof(o_name), i_ptr, TRUE, 0);

		/*dump the long name*/
		fprintf(fff, "desc:%d: # %s\n", i, o_name);

		/* Write I: line */
		fprintf(fff, "I:%d:%d:%d:%d\n", i, a_ptr->tval, a_ptr->sval, a_ptr->pval);

		/* Write W: line */
		fprintf(fff, "W:%d:%d:%d:%d:%d\n", i, a_ptr->a_level, a_ptr->a_rarity,
											a_ptr->weight, a_ptr->cost);

		/* Write P: line */
		fprintf(fff, "P:%d:%d:%d:d:%d:%d:%d:%d\n", i, a_ptr->ac, a_ptr->dd, a_ptr->ds,
						a_ptr->to_h, a_ptr->to_d, a_ptr->to_a);

		/* Get the flags, store flag text in a format easily parsed by a
		 * database, but pretty much illegible to a person.
		 */
		dump_flags(fff, a_ptr->a_flags1, 1, i);
		dump_flags(fff, a_ptr->a_flags2, 2, i);
		dump_flags(fff, a_ptr->a_flags3, 3, i);
		dump_flags(fff, a_ptr->a_native, 4, i);

		/* Write the A line */
		fprintf(fff, "A:%d:%d:%d:%d\n", i,
				a_ptr->activation, a_ptr->time, a_ptr->randtime);

		/* Acquire the description */
		desc = a_text + a_ptr->text;
		dlen = strlen(desc);

		if (dlen < 1)
		{
			/* Space between entries */
			fprintf(fff, "\n\n\n");
			continue;
		}

		/* Write Description */
		for (j = 0; j < dlen;)
		{

			char buf[160], *t;

			/* Initialize */
			t = buf;
			bc = 0;

			/* Build this line */
			while (TRUE)
			{
				/* Insert this character, count it */
				*t++ = desc[j++];

				/* Oops.  Line is too long. */
				if (bc++ >= line_length)
				{
					/* Parse backwards until we find a space */
					while (!my_isspace(desc[j-1]) && (bc-- > 40))
					{
						j--;
						t--;
					}

					/* Time to end this line */
					*t++ = '\0';
					break;
				}

				/* All done */
				if (j >= dlen)
				{
					/* Time to end this line */
					*t++ = '\0';
					break;
				}
			}

			/* Done with this line; write it */
			fprintf(fff, "D-%d:%d:%s\n", counter, i, buf);

			counter++;
		}

		/*
		 * Print out empty lines, so all objects have
		 * an equal number of lines
		 * makes parsing and combining the description easier
		 */
		for (; counter <+ 12; counter++)
		{
			fprintf(fff, "D-%d:%d\n", counter, i);
		}

		/* Space between entries */
		fprintf(fff, "\n\n\n");

	}

	/* Done */
	fclose(fff);
}

/*used to check the power of artifacts.  Currently unused*/
void dump_artifact_power(void)
{
	int i;

	char buf[1024];

	int fd;
	FILE *fff = NULL;

	artifact_type *a_ptr;

	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_EDIT, "power.txt");

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Check for existing file */
	fd = fd_open(buf, O_RDONLY);

	/* Existing file */
	if (fd >= 0)
	{
		char out_val[160];

		/* Close the file */
		fd_close(fd);

		/* Build query */
		sprintf(out_val, "Replace existing file %s? ", buf);

		/* Ask */
		if (get_check(out_val)) fd = -1;
	}

	/* Open the non-existing file */
	if (fd < 0) fff = my_fopen(buf, "w");

	/* No output file - fail */
	if (!fff) return;

	/* Write a note */
	fprintf(fff, "# File: artifact_power.txt (autogenerated)\n\n");

	/* Read and print out all the objects */
	for (i = 1; i < z_info->art_norm_max; i++)
	{
		s32b power;
		char o_name[80];
		object_type *i_ptr;
		object_type object_type_body;

		/* Get local object */
		i_ptr = &object_type_body;

		/* Get the object */
		a_ptr = &a_info[i];

		/* Ignore unused objects */
		if (!strlen(a_ptr->name))
		{
			fprintf(fff, "## empty space (available for artifact) ##\n\n");
			continue;
		}

		/* Write the complete name of the artifact*/
		make_fake_artifact(i_ptr, i);

		/* Identify it */
		object_aware(i_ptr);
		object_known(i_ptr);
		i_ptr->ident |= (IDENT_MENTAL);

		/* Get a description to dump */
		object_desc(o_name, sizeof(o_name), i_ptr, TRUE, 0);

		power = artifact_power(i);

		/*dump the information*/
		fprintf(fff, "%9d is the power of %55s, tval is %6d \n", power, o_name, a_ptr->tval);

	}

	/* Done */
	fclose(fff);

}

/*
 * Use the monster racial information to a format easily parsed by a spreadsheet.
 *
 * Original function by -EB- (probably), revisions by -LM- & JG.
 */
void write_mon_power(void)
{
	int i;

	char buf[1024];

	int fd;
	FILE *fff = NULL;

	monster_race *r_ptr;

	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_EDIT, "mon_power_output.txt");

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Check for existing file */
	fd = fd_open(buf, O_RDONLY);

	/* Existing file */
	if (fd >= 0)
	{
		char out_val[160];

		/* Close the file */
		fd_close(fd);

		/* Build query */
		sprintf(out_val, "Replace existing file %s? ", buf);

		/* Ask */
		if (get_check(out_val)) fd = -1;
	}

	/* Open the non-existing file */
	if (fd < 0) fff = my_fopen(buf, "w");

	/* No output file - fail */
	if (!fff) return;

	/* Write a note */
	fprintf(fff, "# File: mon_power_output.txt (autogenerated)\n\n");

	/* Write a note */
	fprintf(fff, "##### The Player #####\n\n");

	/* Read and print out all the monsters */
	for (i = 0; i < z_info->r_max; i++)
	{

		/* Get the monster */
		r_ptr = &r_info[i];

		/* Ignore empty monsters */
		if (!strlen(r_name + r_ptr->name))
		{
			fprintf(fff, "## empty space (available for monsters) ##\n\n");
			continue;
		}

		/* Write New/Number/Name */
		fprintf(fff, "%3d:lvl: %3d power:%9d hp:%9d dam:%9d name: %s\n",
						i, r_ptr->level, r_ptr->mon_power, r_ptr->mon_eval_hp,
								r_ptr->mon_eval_dam,(r_name + r_ptr->name));

	}

	/*a few blank lines*/
	fprintf(fff, "\n\n");

	/*first clear the tables*/
	for (i = 0; i < MAX_DEPTH; i++)
	{
		/* Write New/Number/Name */
		fprintf(fff, "lvl: %3d unique_ave_power:%9d creature_ave_power:%9d \n", i,
					mon_power_ave[i][CREATURE_UNIQUE], mon_power_ave[i][CREATURE_NON_UNIQUE]);

	}

	/* Done */
	fclose(fff);
}



#endif /*ALLOW_DATA_DUMP*/
