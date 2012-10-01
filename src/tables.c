/* File: tables.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

#include "angband.h"




/*
 * Global array for looping through the "keypad directions".
 */
const s16b ddd[9] =
{ 2, 8, 6, 4, 3, 1, 9, 7, 5 };

/*
 * Global arrays for converting "keypad direction" into "offsets".
 */
const s16b ddx[10] =
{ 0, -1, 0, 1, -1, 0, 1, -1, 0, 1 };

const s16b ddy[10] =
{ 0, 1, 1, 1, 0, 0, 0, -1, -1, -1 };

/*
 * Global arrays for optimizing "ddx[ddd[i]]" and "ddy[ddd[i]]".
 */
const s16b ddx_ddd[9] =
{ 0, 0, 1, -1, 1, -1, 1, -1, 0 };

const s16b ddy_ddd[9] =
{ 1, -1, 0, 0, 1, 1, -1, -1, 0 };


/*
 * Global array for converting numbers to uppercase hecidecimal digit
 * This array can also be used to convert a number to an octal digit
 */
const char hexsym[16] =
{
	'0', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};


/*
 * Stat Table (INT/WIS) -- Number of half-spells per level
 */
const byte adj_mag_study[] =
{
	0	/* 3 */,
	0	/* 4 */,
	0	/* 5 */,
	0	/* 6 */,
	0	/* 7 */,
	1	/* 8 */,
	1	/* 9 */,
	1	/* 10 */,
	1	/* 11 */,
	2	/* 12 */,
	2	/* 13 */,
	2	/* 14 */,
	2	/* 15 */,
	2	/* 16 */,
	2	/* 17 */,
	2	/* 18/00-18/09 */,
	2	/* 18/10-18/19 */,
	2	/* 18/20-18/29 */,
	2	/* 18/30-18/39 */,
	2	/* 18/40-18/49 */,
	3	/* 18/50-18/59 */,
	3	/* 18/60-18/69 */,
	3	/* 18/70-18/79 */,
	3	/* 18/80-18/89 */,
	4	/* 18/90-18/99 */,
	4	/* 18/100-18/109 */,
	4	/* 18/110-18/119 */,
	5	/* 18/120-18/129 */,
	5	/* 18/130-18/139 */,
	5	/* 18/140-18/149 */,
	5	/* 18/150-18/159 */,
	5	/* 18/160-18/169 */,
	5	/* 18/170-18/179 */,
	5	/* 18/180-18/189 */,
	5	/* 18/190-18/199 */,
	5	/* 18/200-18/209 */,
	5	/* 18/210-18/219 */,
	5	/* 18/220+ */
};


/*
 * Stat Table (INT/WIS) -- extra half-mana-points per level
 */
const byte adj_mag_mana[] =
{
	0	/* 3 */,
	0	/* 4 */,
	0	/* 5 */,
	0	/* 6 */,
	0	/* 7 */,
	1	/* 8 */,
	2	/* 9 */,
	2	/* 10 */,
	2	/* 11 */,
	2	/* 12 */,
	2	/* 13 */,
	2	/* 14 */,
	2	/* 15 */,
	2	/* 16 */,
	2	/* 17 */,
	3	/* 18/00-18/09 */,
	3	/* 18/10-18/19 */,
	3	/* 18/20-18/29 */,
	3	/* 18/30-18/39 */,
	3	/* 18/40-18/49 */,
	4	/* 18/50-18/59 */,
	4	/* 18/60-18/69 */,
	5	/* 18/70-18/79 */,
	6	/* 18/80-18/89 */,
	7	/* 18/90-18/99 */,
	8	/* 18/100-18/109 */,
	9	/* 18/110-18/119 */,
	10	/* 18/120-18/129 */,
	11	/* 18/130-18/139 */,
	12	/* 18/140-18/149 */,
	13	/* 18/150-18/159 */,
	14	/* 18/160-18/169 */,
	15	/* 18/170-18/179 */,
	16	/* 18/180-18/189 */,
	16	/* 18/190-18/199 */,
	16	/* 18/200-18/209 */,
	16	/* 18/210-18/219 */,
	16	/* 18/220+ */
};


/*
 * Stat Table (INT/WIS) -- Minimum failure rate (percentage)
 */
const byte adj_mag_fail[] =
{
	99	/* 3 */,
	99	/* 4 */,
	99	/* 5 */,
	99	/* 6 */,
	99	/* 7 */,
	50	/* 8 */,
	30	/* 9 */,
	20	/* 10 */,
	15	/* 11 */,
	12	/* 12 */,
	11	/* 13 */,
	10	/* 14 */,
	9	/* 15 */,
	8	/* 16 */,
	7	/* 17 */,
	6	/* 18/00-18/09 */,
	6	/* 18/10-18/19 */,
	5	/* 18/20-18/29 */,
	5	/* 18/30-18/39 */,
	5	/* 18/40-18/49 */,
	4	/* 18/50-18/59 */,
	4	/* 18/60-18/69 */,
	4	/* 18/70-18/79 */,
	4	/* 18/80-18/89 */,
	3	/* 18/90-18/99 */,
	3	/* 18/100-18/109 */,
	2	/* 18/110-18/119 */,
	2	/* 18/120-18/129 */,
	2	/* 18/130-18/139 */,
	2	/* 18/140-18/149 */,
	1	/* 18/150-18/159 */,
	1	/* 18/160-18/169 */,
	1	/* 18/170-18/179 */,
	1	/* 18/180-18/189 */,
	1	/* 18/190-18/199 */,
	0	/* 18/200-18/209 */,
	0	/* 18/210-18/219 */,
	0	/* 18/220+ */
};


/*
 * Stat Table (INT/WIS) -- Various things
 */
const byte adj_mag_stat[] =
{
	0	/* 3 */,
	0	/* 4 */,
	0	/* 5 */,
	0	/* 6 */,
	0	/* 7 */,
	1	/* 8 */,
	1	/* 9 */,
	1	/* 10 */,
	1	/* 11 */,
	1	/* 12 */,
	1	/* 13 */,
	1	/* 14 */,
	2	/* 15 */,
	2	/* 16 */,
	2	/* 17 */,
	3	/* 18/00-18/09 */,
	3	/* 18/10-18/19 */,
	3	/* 18/20-18/29 */,
	3	/* 18/30-18/39 */,
	3	/* 18/40-18/49 */,
	4	/* 18/50-18/59 */,
	4	/* 18/60-18/69 */,
	5	/* 18/70-18/79 */,
	6	/* 18/80-18/89 */,
	7	/* 18/90-18/99 */,
	8	/* 18/100-18/109 */,
	9	/* 18/110-18/119 */,
	10	/* 18/120-18/129 */,
	11	/* 18/130-18/139 */,
	12	/* 18/140-18/149 */,
	13	/* 18/150-18/159 */,
	14	/* 18/160-18/169 */,
	15	/* 18/170-18/179 */,
	16	/* 18/180-18/189 */,
	17	/* 18/190-18/199 */,
	18	/* 18/200-18/209 */,
	19	/* 18/210-18/219 */,
	20	/* 18/220+ */
};


/*
 * Stat Table (CHR) -- payment percentages
 */
const byte adj_chr_gold[] =
{
	130	/* 3 */,
	125	/* 4 */,
	122	/* 5 */,
	120	/* 6 */,
	118	/* 7 */,
	116	/* 8 */,
	114	/* 9 */,
	112	/* 10 */,
	110	/* 11 */,
	108	/* 12 */,
	106	/* 13 */,
	104	/* 14 */,
	103	/* 15 */,
	102	/* 16 */,
	101	/* 17 */,
	100	/* 18/00-18/09 */,
	99	/* 18/10-18/19 */,
	98	/* 18/20-18/29 */,
	97	/* 18/30-18/39 */,
	96	/* 18/40-18/49 */,
	95	/* 18/50-18/59 */,
	94	/* 18/60-18/69 */,
	93	/* 18/70-18/79 */,
	92	/* 18/80-18/89 */,
	91	/* 18/90-18/99 */,
	90	/* 18/100-18/109 */,
	89	/* 18/110-18/119 */,
	88	/* 18/120-18/129 */,
	87	/* 18/130-18/139 */,
	86	/* 18/140-18/149 */,
	85	/* 18/150-18/159 */,
	84	/* 18/160-18/169 */,
	83	/* 18/170-18/179 */,
	82	/* 18/180-18/189 */,
	81	/* 18/190-18/199 */,
	80	/* 18/200-18/209 */,
	80	/* 18/210-18/219 */,
	80	/* 18/220+ */
};

/*
 * Stat Table (CHR) -- bonus to charm
 */
const byte adj_chr_charm[] =
{
	- 5	/* 3 */,
	- 4	/* 4 */,
	- 3	/* 5 */,
	- 2	/* 6 */,
	- 1	/* 7 */,
	0	/* 8 */,
	0	/* 9 */,
	0	/* 10 */,
	0	/* 11 */,
	0	/* 12 */,
	0	/* 13 */,
	0	/* 14 */,
	0	/* 15 */,
	1	/* 16 */,
	1	/* 17 */,
	1	/* 18/00-18/09 */,
	2	/* 18/10-18/19 */,
	2	/* 18/20-18/29 */,
	2	/* 18/30-18/39 */,
	2	/* 18/40-18/49 */,
	3	/* 18/50-18/59 */,
	3	/* 18/60-18/69 */,
	3	/* 18/70-18/79 */,
	4	/* 18/80-18/89 */,
	4	/* 18/90-18/99 */,
	4	/* 18/100-18/109 */,
	5	/* 18/110-18/119 */,
	5	/* 18/120-18/129 */,
	6	/* 18/130-18/139 */,
	7	/* 18/140-18/149 */,
	8	/* 18/150-18/159 */,
	9	/* 18/160-18/169 */,
	10	/* 18/170-18/179 */,
	11	/* 18/180-18/189 */,
	12	/* 18/190-18/199 */,
	13	/* 18/200-18/209 */,
	14	/* 18/210-18/219 */,
	15	/* 18/220+ */
};



/*
 * Stat Table (INT) -- Magic devices
 */
const byte adj_int_dev[] =
{
	0	/* 3 */,
	0	/* 4 */,
	0	/* 5 */,
	0	/* 6 */,
	0	/* 7 */,
	1	/* 8 */,
	1	/* 9 */,
	1	/* 10 */,
	1	/* 11 */,
	1	/* 12 */,
	1	/* 13 */,
	1	/* 14 */,
	2	/* 15 */,
	2	/* 16 */,
	2	/* 17 */,
	3	/* 18/00-18/09 */,
	3	/* 18/10-18/19 */,
	4	/* 18/20-18/29 */,
	4	/* 18/30-18/39 */,
	5	/* 18/40-18/49 */,
	5	/* 18/50-18/59 */,
	6	/* 18/60-18/69 */,
	6	/* 18/70-18/79 */,
	7	/* 18/80-18/89 */,
	7	/* 18/90-18/99 */,
	8	/* 18/100-18/109 */,
	9	/* 18/110-18/119 */,
	10	/* 18/120-18/129 */,
	11	/* 18/130-18/139 */,
	12	/* 18/140-18/149 */,
	13	/* 18/150-18/159 */,
	14	/* 18/160-18/169 */,
	15	/* 18/170-18/179 */,
	16	/* 18/180-18/189 */,
	17	/* 18/190-18/199 */,
	18	/* 18/200-18/209 */,
	19	/* 18/210-18/219 */,
	20	/* 18/220+ */
};


/*
 * Stat Table (WIS) -- Saving throw
 */
const byte adj_wis_sav[] =
{
	0	/* 3 */,
	0	/* 4 */,
	0	/* 5 */,
	0	/* 6 */,
	0	/* 7 */,
	1	/* 8 */,
	1	/* 9 */,
	1	/* 10 */,
	1	/* 11 */,
	1	/* 12 */,
	1	/* 13 */,
	1	/* 14 */,
	2	/* 15 */,
	2	/* 16 */,
	2	/* 17 */,
	3	/* 18/00-18/09 */,
	3	/* 18/10-18/19 */,
	3	/* 18/20-18/29 */,
	3	/* 18/30-18/39 */,
	3	/* 18/40-18/49 */,
	4	/* 18/50-18/59 */,
	4	/* 18/60-18/69 */,
	5	/* 18/70-18/79 */,
	5	/* 18/80-18/89 */,
	6	/* 18/90-18/99 */,
	7	/* 18/100-18/109 */,
	8	/* 18/110-18/119 */,
	9	/* 18/120-18/129 */,
	10	/* 18/130-18/139 */,
	11	/* 18/140-18/149 */,
	12	/* 18/150-18/159 */,
	13	/* 18/160-18/169 */,
	14	/* 18/170-18/179 */,
	15	/* 18/180-18/189 */,
	16	/* 18/190-18/199 */,
	17	/* 18/200-18/209 */,
	18	/* 18/210-18/219 */,
	19	/* 18/220+ */
};


/*
 * Stat Table (DEX) -- disarming
 */
const byte adj_dex_dis[] =
{
	0	/* 3 */,
	0	/* 4 */,
	0	/* 5 */,
	0	/* 6 */,
	0	/* 7 */,
	0	/* 8 */,
	0	/* 9 */,
	0	/* 10 */,
	0	/* 11 */,
	0	/* 12 */,
	1	/* 13 */,
	1	/* 14 */,
	1	/* 15 */,
	2	/* 16 */,
	2	/* 17 */,
	4	/* 18/00-18/09 */,
	4	/* 18/10-18/19 */,
	4	/* 18/20-18/29 */,
	4	/* 18/30-18/39 */,
	5	/* 18/40-18/49 */,
	5	/* 18/50-18/59 */,
	5	/* 18/60-18/69 */,
	6	/* 18/70-18/79 */,
	6	/* 18/80-18/89 */,
	7	/* 18/90-18/99 */,
	8	/* 18/100-18/109 */,
	8	/* 18/110-18/119 */,
	8	/* 18/120-18/129 */,
	8	/* 18/130-18/139 */,
	8	/* 18/140-18/149 */,
	9	/* 18/150-18/159 */,
	9	/* 18/160-18/169 */,
	9	/* 18/170-18/179 */,
	9	/* 18/180-18/189 */,
	9	/* 18/190-18/199 */,
	10	/* 18/200-18/209 */,
	10	/* 18/210-18/219 */,
	10	/* 18/220+ */
};


/*
 * Stat Table (INT) -- disarming
 */
const byte adj_int_dis[] =
{
	0	/* 3 */,
	0	/* 4 */,
	0	/* 5 */,
	0	/* 6 */,
	0	/* 7 */,
	1	/* 8 */,
	1	/* 9 */,
	1	/* 10 */,
	1	/* 11 */,
	1	/* 12 */,
	1	/* 13 */,
	1	/* 14 */,
	2	/* 15 */,
	2	/* 16 */,
	2	/* 17 */,
	3	/* 18/00-18/09 */,
	3	/* 18/10-18/19 */,
	3	/* 18/20-18/29 */,
	4	/* 18/30-18/39 */,
	4	/* 18/40-18/49 */,
	5	/* 18/50-18/59 */,
	6	/* 18/60-18/69 */,
	7	/* 18/70-18/79 */,
	8	/* 18/80-18/89 */,
	9	/* 18/90-18/99 */,
	10	/* 18/100-18/109 */,
	10	/* 18/110-18/119 */,
	11	/* 18/120-18/129 */,
	12	/* 18/130-18/139 */,
	13	/* 18/140-18/149 */,
	14	/* 18/150-18/159 */,
	15	/* 18/160-18/169 */,
	16	/* 18/170-18/179 */,
	17	/* 18/180-18/189 */,
	18	/* 18/190-18/199 */,
	19	/* 18/200-18/209 */,
	19	/* 18/210-18/219 */,
	19	/* 18/220+ */
};


/*
 * Stat Table (DEX) -- bonus to ac (plus 128)
 */
const byte adj_dex_ta[] =
{
	128 + -4	/* 3 */,
	128 + -3	/* 4 */,
	128 + -2	/* 5 */,
	128 + -1	/* 6 */,
	128 + 0	/* 7 */,
	128 + 0	/* 8 */,
	128 + 0	/* 9 */,
	128 + 0	/* 10 */,
	128 + 0	/* 11 */,
	128 + 0	/* 12 */,
	128 + 0	/* 13 */,
	128 + 0	/* 14 */,
	128 + 1	/* 15 */,
	128 + 1	/* 16 */,
	128 + 1	/* 17 */,
	128 + 2	/* 18/00-18/09 */,
	128 + 2	/* 18/10-18/19 */,
	128 + 2	/* 18/20-18/29 */,
	128 + 2	/* 18/30-18/39 */,
	128 + 2	/* 18/40-18/49 */,
	128 + 3	/* 18/50-18/59 */,
	128 + 3	/* 18/60-18/69 */,
	128 + 3	/* 18/70-18/79 */,
	128 + 4	/* 18/80-18/89 */,
	128 + 5	/* 18/90-18/99 */,
	128 + 6	/* 18/100-18/109 */,
	128 + 7	/* 18/110-18/119 */,
	128 + 8	/* 18/120-18/129 */,
	128 + 9	/* 18/130-18/139 */,
	128 + 9	/* 18/140-18/149 */,
	128 + 10	/* 18/150-18/159 */,
	128 + 11	/* 18/160-18/169 */,
	128 + 12	/* 18/170-18/179 */,
	128 + 13	/* 18/180-18/189 */,
	128 + 14	/* 18/190-18/199 */,
	128 + 15	/* 18/200-18/209 */,
	128 + 15	/* 18/210-18/219 */,
	128 + 15	/* 18/220+ */
};


/*
 * Stat Table (STR) -- bonus to dam (plus 128)
 */
const byte adj_str_td[] =
{
	128 + -2	/* 3 */,
	128 + -2	/* 4 */,
	128 + -1	/* 5 */,
	128 + -1	/* 6 */,
	128 + 0	/* 7 */,
	128 + 0	/* 8 */,
	128 + 0	/* 9 */,
	128 + 0	/* 10 */,
	128 + 0	/* 11 */,
	128 + 0	/* 12 */,
	128 + 0	/* 13 */,
	128 + 0	/* 14 */,
	128 + 0	/* 15 */,
	128 + 1	/* 16 */,
	128 + 2	/* 17 */,
	128 + 2	/* 18/00-18/09 */,
	128 + 2	/* 18/10-18/19 */,
	128 + 3	/* 18/20-18/29 */,
	128 + 3	/* 18/30-18/39 */,
	128 + 3	/* 18/40-18/49 */,
	128 + 3	/* 18/50-18/59 */,
	128 + 3	/* 18/60-18/69 */,
	128 + 4	/* 18/70-18/79 */,
	128 + 5	/* 18/80-18/89 */,
	128 + 5	/* 18/90-18/99 */,
	128 + 6	/* 18/100-18/109 */,
	128 + 7	/* 18/110-18/119 */,
	128 + 8	/* 18/120-18/129 */,
	128 + 9	/* 18/130-18/139 */,
	128 + 10	/* 18/140-18/149 */,
	128 + 11	/* 18/150-18/159 */,
	128 + 12	/* 18/160-18/169 */,
	128 + 13	/* 18/170-18/179 */,
	128 + 14	/* 18/180-18/189 */,
	128 + 15	/* 18/190-18/199 */,
	128 + 16	/* 18/200-18/209 */,
	128 + 18	/* 18/210-18/219 */,
	128 + 20	/* 18/220+ */
};


/*
 * Stat Table (DEX) -- bonus to hit (plus 128)
 */
const byte adj_dex_th[] =
{
	128 + -3	/* 3 */,
	128 + -2	/* 4 */,
	128 + -2	/* 5 */,
	128 + -1	/* 6 */,
	128 + -1	/* 7 */,
	128 + 0	/* 8 */,
	128 + 0	/* 9 */,
	128 + 0	/* 10 */,
	128 + 0	/* 11 */,
	128 + 0	/* 12 */,
	128 + 0	/* 13 */,
	128 + 0	/* 14 */,
	128 + 0	/* 15 */,
	128 + 1	/* 16 */,
	128 + 2	/* 17 */,
	128 + 3	/* 18/00-18/09 */,
	128 + 3	/* 18/10-18/19 */,
	128 + 3	/* 18/20-18/29 */,
	128 + 3	/* 18/30-18/39 */,
	128 + 3	/* 18/40-18/49 */,
	128 + 4	/* 18/50-18/59 */,
	128 + 4	/* 18/60-18/69 */,
	128 + 4	/* 18/70-18/79 */,
	128 + 4	/* 18/80-18/89 */,
	128 + 5	/* 18/90-18/99 */,
	128 + 6	/* 18/100-18/109 */,
	128 + 7	/* 18/110-18/119 */,
	128 + 8	/* 18/120-18/129 */,
	128 + 9	/* 18/130-18/139 */,
	128 + 9	/* 18/140-18/149 */,
	128 + 10	/* 18/150-18/159 */,
	128 + 11	/* 18/160-18/169 */,
	128 + 12	/* 18/170-18/179 */,
	128 + 13	/* 18/180-18/189 */,
	128 + 14	/* 18/190-18/199 */,
	128 + 15	/* 18/200-18/209 */,
	128 + 15	/* 18/210-18/219 */,
	128 + 15	/* 18/220+ */
};


/*
 * Stat Table (STR) -- bonus to hit (plus 128)
 */
const byte adj_str_th[] =
{
	128 + -3	/* 3 */,
	128 + -2	/* 4 */,
	128 + -1	/* 5 */,
	128 + -1	/* 6 */,
	128 + 0	/* 7 */,
	128 + 0	/* 8 */,
	128 + 0	/* 9 */,
	128 + 0	/* 10 */,
	128 + 0	/* 11 */,
	128 + 0	/* 12 */,
	128 + 0	/* 13 */,
	128 + 0	/* 14 */,
	128 + 0	/* 15 */,
	128 + 0	/* 16 */,
	128 + 0	/* 17 */,
	128 + 1	/* 18/00-18/09 */,
	128 + 1	/* 18/10-18/19 */,
	128 + 1	/* 18/20-18/29 */,
	128 + 1	/* 18/30-18/39 */,
	128 + 1	/* 18/40-18/49 */,
	128 + 1	/* 18/50-18/59 */,
	128 + 1	/* 18/60-18/69 */,
	128 + 2	/* 18/70-18/79 */,
	128 + 3	/* 18/80-18/89 */,
	128 + 4	/* 18/90-18/99 */,
	128 + 5	/* 18/100-18/109 */,
	128 + 6	/* 18/110-18/119 */,
	128 + 7	/* 18/120-18/129 */,
	128 + 8	/* 18/130-18/139 */,
	128 + 9	/* 18/140-18/149 */,
	128 + 10	/* 18/150-18/159 */,
	128 + 11	/* 18/160-18/169 */,
	128 + 12	/* 18/170-18/179 */,
	128 + 13	/* 18/180-18/189 */,
	128 + 14	/* 18/190-18/199 */,
	128 + 15	/* 18/200-18/209 */,
	128 + 15	/* 18/210-18/219 */,
	128 + 15	/* 18/220+ */
};


/*
 * Stat Table (STR) -- weight limit in deca-pounds
 */
const byte adj_str_wgt[] =
{
	5	/* 3 */,
	6	/* 4 */,
	7	/* 5 */,
	8	/* 6 */,
	9	/* 7 */,
	10	/* 8 */,
	11	/* 9 */,
	12	/* 10 */,
	13	/* 11 */,
	14	/* 12 */,
	15	/* 13 */,
	16	/* 14 */,
	17	/* 15 */,
	18	/* 16 */,
	19	/* 17 */,
	20	/* 18/00-18/09 */,
	22	/* 18/10-18/19 */,
	24	/* 18/20-18/29 */,
	26	/* 18/30-18/39 */,
	28	/* 18/40-18/49 */,
	30	/* 18/50-18/59 */,
	30	/* 18/60-18/69 */,
	30	/* 18/70-18/79 */,
	30	/* 18/80-18/89 */,
	30	/* 18/90-18/99 */,
	30	/* 18/100-18/109 */,
	30	/* 18/110-18/119 */,
	30	/* 18/120-18/129 */,
	30	/* 18/130-18/139 */,
	30	/* 18/140-18/149 */,
	30	/* 18/150-18/159 */,
	30	/* 18/160-18/169 */,
	30	/* 18/170-18/179 */,
	30	/* 18/180-18/189 */,
	30	/* 18/190-18/199 */,
	30	/* 18/200-18/209 */,
	30	/* 18/210-18/219 */,
	30	/* 18/220+ */
};


/*
 * Stat Table (STR) -- weapon weight limit in pounds
 */
const byte adj_str_hold[] =
{
	4	/* 3 */,
	5	/* 4 */,
	6	/* 5 */,
	7	/* 6 */,
	8	/* 7 */,
	10	/* 8 */,
	12	/* 9 */,
	14	/* 10 */,
	16	/* 11 */,
	18	/* 12 */,
	20	/* 13 */,
	22	/* 14 */,
	24	/* 15 */,
	26	/* 16 */,
	28	/* 17 */,
	30	/* 18/00-18/09 */,
	30	/* 18/10-18/19 */,
	35	/* 18/20-18/29 */,
	40	/* 18/30-18/39 */,
	45	/* 18/40-18/49 */,
	50	/* 18/50-18/59 */,
	55	/* 18/60-18/69 */,
	60	/* 18/70-18/79 */,
	65	/* 18/80-18/89 */,
	70	/* 18/90-18/99 */,
	80	/* 18/100-18/109 */,
	80	/* 18/110-18/119 */,
	80	/* 18/120-18/129 */,
	80	/* 18/130-18/139 */,
	80	/* 18/140-18/149 */,
	90	/* 18/150-18/159 */,
	90	/* 18/160-18/169 */,
	90	/* 18/170-18/179 */,
	90	/* 18/180-18/189 */,
	90	/* 18/190-18/199 */,
	100	/* 18/200-18/209 */,
	100	/* 18/210-18/219 */,
	100	/* 18/220+ */
};


/*
 * Stat Table (STR) -- digging value
 */
const byte adj_str_dig[] =
{
	0	/* 3 */,
	0	/* 4 */,
	1	/* 5 */,
	2	/* 6 */,
	3	/* 7 */,
	4	/* 8 */,
	4	/* 9 */,
	5	/* 10 */,
	5	/* 11 */,
	6	/* 12 */,
	6	/* 13 */,
	7	/* 14 */,
	7	/* 15 */,
	8	/* 16 */,
	8	/* 17 */,
	9	/* 18/00-18/09 */,
	10	/* 18/10-18/19 */,
	12	/* 18/20-18/29 */,
	15	/* 18/30-18/39 */,
	20	/* 18/40-18/49 */,
	25	/* 18/50-18/59 */,
	30	/* 18/60-18/69 */,
	35	/* 18/70-18/79 */,
	40	/* 18/80-18/89 */,
	45	/* 18/90-18/99 */,
	50	/* 18/100-18/109 */,
	55	/* 18/110-18/119 */,
	60	/* 18/120-18/129 */,
	65	/* 18/130-18/139 */,
	70	/* 18/140-18/149 */,
	75	/* 18/150-18/159 */,
	80	/* 18/160-18/169 */,
	85	/* 18/170-18/179 */,
	90	/* 18/180-18/189 */,
	95	/* 18/190-18/199 */,
	100	/* 18/200-18/209 */,
	100	/* 18/210-18/219 */,
	100	/* 18/220+ */
};


/*
 * Stat Table (STR) -- help index into the "blow" table
 */
const byte adj_str_blow[] =
{
	3	/* 3 */,
	4	/* 4 */,
	5	/* 5 */,
	6	/* 6 */,
	7	/* 7 */,
	8	/* 8 */,
	9	/* 9 */,
	10	/* 10 */,
	11	/* 11 */,
	12	/* 12 */,
	13	/* 13 */,
	14	/* 14 */,
	15	/* 15 */,
	16	/* 16 */,
	17	/* 17 */,
	20 /* 18/00-18/09 */,
	30 /* 18/10-18/19 */,
	40 /* 18/20-18/29 */,
	50 /* 18/30-18/39 */,
	60 /* 18/40-18/49 */,
	70 /* 18/50-18/59 */,
	80 /* 18/60-18/69 */,
	90 /* 18/70-18/79 */,
	100 /* 18/80-18/89 */,
	110 /* 18/90-18/99 */,
	120 /* 18/100-18/109 */,
	130 /* 18/110-18/119 */,
	140 /* 18/120-18/129 */,
	150 /* 18/130-18/139 */,
	160 /* 18/140-18/149 */,
	170 /* 18/150-18/159 */,
	180 /* 18/160-18/169 */,
	190 /* 18/170-18/179 */,
	200 /* 18/180-18/189 */,
	210 /* 18/190-18/199 */,
	220 /* 18/200-18/209 */,
	230 /* 18/210-18/219 */,
	240 /* 18/220+ */
};


/*
 * Stat Table (DEX) -- index into the "blow" table
 */
const byte adj_dex_blow[] =
{
	0	/* 3 */,
	0	/* 4 */,
	0	/* 5 */,
	0	/* 6 */,
	0	/* 7 */,
	0	/* 8 */,
	0	/* 9 */,
	1	/* 10 */,
	1	/* 11 */,
	1	/* 12 */,
	1	/* 13 */,
	1	/* 14 */,
	1	/* 15 */,
	1	/* 16 */,
	1	/* 17 */,
	1	/* 18/00-18/09 */,
	2	/* 18/10-18/19 */,
	2	/* 18/20-18/29 */,
	2	/* 18/30-18/39 */,
	2	/* 18/40-18/49 */,
	3	/* 18/50-18/59 */,
	3	/* 18/60-18/69 */,
	4	/* 18/70-18/79 */,
	4	/* 18/80-18/89 */,
	5	/* 18/90-18/99 */,
	6	/* 18/100-18/109 */,
	7	/* 18/110-18/119 */,
	8	/* 18/120-18/129 */,
	9	/* 18/130-18/139 */,
	10	/* 18/140-18/149 */,
	11	/* 18/150-18/159 */,
	12	/* 18/160-18/169 */,
	14	/* 18/170-18/179 */,
	16	/* 18/180-18/189 */,
	18	/* 18/190-18/199 */,
	20	/* 18/200-18/209 */,
	20	/* 18/210-18/219 */,
	20	/* 18/220+ */
};


/*
 * Stat Table (DEX) -- chance of avoiding "theft" and "falling"
 */
const byte adj_dex_safe[] =
{
	0	/* 3 */,
	1	/* 4 */,
	2	/* 5 */,
	3	/* 6 */,
	4	/* 7 */,
	5	/* 8 */,
	5	/* 9 */,
	6	/* 10 */,
	6	/* 11 */,
	7	/* 12 */,
	7	/* 13 */,
	8	/* 14 */,
	8	/* 15 */,
	9	/* 16 */,
	9	/* 17 */,
	10	/* 18/00-18/09 */,
	10	/* 18/10-18/19 */,
	15	/* 18/20-18/29 */,
	15	/* 18/30-18/39 */,
	20	/* 18/40-18/49 */,
	25	/* 18/50-18/59 */,
	30	/* 18/60-18/69 */,
	35	/* 18/70-18/79 */,
	40	/* 18/80-18/89 */,
	45	/* 18/90-18/99 */,
	50	/* 18/100-18/109 */,
	60	/* 18/110-18/119 */,
	70	/* 18/120-18/129 */,
	80	/* 18/130-18/139 */,
	90	/* 18/140-18/149 */,
	95	/* 18/150-18/159 */,
	97	/* 18/160-18/169 */,
	99	/* 18/170-18/179 */,
	100	/* 18/180-18/189 */,
	100	/* 18/190-18/199 */,
	100	/* 18/200-18/209 */,
	100	/* 18/210-18/219 */,
	100	/* 18/220+ */
};


/*
 * Stat Table (CON) -- base regeneration rate
 */
const byte adj_con_fix[] =
{
	0	/* 3 */,
	0	/* 4 */,
	0	/* 5 */,
	0	/* 6 */,
	0	/* 7 */,
	0	/* 8 */,
	0	/* 9 */,
	0	/* 10 */,
	0	/* 11 */,
	0	/* 12 */,
	0	/* 13 */,
	1	/* 14 */,
	1	/* 15 */,
	1	/* 16 */,
	1	/* 17 */,
	2	/* 18/00-18/09 */,
	2	/* 18/10-18/19 */,
	2	/* 18/20-18/29 */,
	2	/* 18/30-18/39 */,
	2	/* 18/40-18/49 */,
	3	/* 18/50-18/59 */,
	3	/* 18/60-18/69 */,
	3	/* 18/70-18/79 */,
	3	/* 18/80-18/89 */,
	3	/* 18/90-18/99 */,
	4	/* 18/100-18/109 */,
	4	/* 18/110-18/119 */,
	5	/* 18/120-18/129 */,
	6	/* 18/130-18/139 */,
	6	/* 18/140-18/149 */,
	7	/* 18/150-18/159 */,
	7	/* 18/160-18/169 */,
	8	/* 18/170-18/179 */,
	8	/* 18/180-18/189 */,
	8	/* 18/190-18/199 */,
	9	/* 18/200-18/209 */,
	9	/* 18/210-18/219 */,
	9	/* 18/220+ */
};


/*
 * Stat Table (CON) -- extra half-hitpoints per level (plus 128)
 */
const byte adj_con_mhp[] =
{
	128 + -5	/* 3 */,
	128 + -3	/* 4 */,
	128 + -2	/* 5 */,
	128 + -1	/* 6 */,
	128 + 0	/* 7 */,
	128 + 0	/* 8 */,
	128 + 0	/* 9 */,
	128 + 0	/* 10 */,
	128 + 0	/* 11 */,
	128 + 0	/* 12 */,
	128 + 0	/* 13 */,
	128 + 0	/* 14 */,
	128 + 1	/* 15 */,
	128 + 1	/* 16 */,
	128 + 2	/* 17 */,
	128 + 3	/* 18/00-18/09 */,
	128 + 4	/* 18/10-18/19 */,
	128 + 4	/* 18/20-18/29 */,
	128 + 4	/* 18/30-18/39 */,
	128 + 4	/* 18/40-18/49 */,
	128 + 5	/* 18/50-18/59 */,
	128 + 6	/* 18/60-18/69 */,
	128 + 7	/* 18/70-18/79 */,
	128 + 8	/* 18/80-18/89 */,
	128 + 9	/* 18/90-18/99 */,
	128 + 10	/* 18/100-18/109 */,
	128 + 11	/* 18/110-18/119 */,
	128 + 12	/* 18/120-18/129 */,
	128 + 13	/* 18/130-18/139 */,
	128 + 14	/* 18/140-18/149 */,
	128 + 15	/* 18/150-18/159 */,
	128 + 16	/* 18/160-18/169 */,
	128 + 18	/* 18/170-18/179 */,
	128 + 20	/* 18/180-18/189 */,
	128 + 22	/* 18/190-18/199 */,
	128 + 25	/* 18/200-18/209 */,
	128 + 25	/* 18/210-18/219 */,
	128 + 25	/* 18/220+ */
};


/*
 * This table is used to help calculate the number of blows the player can
 * make in a single round of attacks (one player turn) with a normal weapon.
 *
 * This number ranges from a single blow/round for weak players to up to six
 * blows/round for powerful warriors.
 *
 * Note that certain artifacts and ego-items give "bonus" blows/round.
 *
 * First, from the player class, we extract some values:
 *
 *    Warrior --> num = 6; mul = 5; div = MAX(30, weapon_weight);
 *    Mage    --> num = 4; mul = 2; div = MAX(40, weapon_weight);
 *    Priest  --> num = 5; mul = 3; div = MAX(35, weapon_weight);
 *    Rogue   --> num = 5; mul = 3; div = MAX(30, weapon_weight);
 *    Ranger  --> num = 5; mul = 4; div = MAX(35, weapon_weight);
 *    Paladin --> num = 5; mul = 4; div = MAX(30, weapon_weight);
 *
 * To get "P", we look up the relevant "adj_str_blow[]" (see above),
 * multiply it by "mul", and then divide it by "div", rounding down.
 *
 * To get "D", we look up the relevant "adj_dex_blow[]" (see above),
 * note especially column 6 (DEX 18/101) and 11 (DEX 18/150).
 *
 * The player gets "blows_table[P][D]" blows/round, as shown below,
 * up to a maximum of "num" blows/round, plus any "bonus" blows/round.
 */
const byte blows_table[12][12] =
{
	/* P/D */
	/* 0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11+ */

	/* 0  */
	{  1,   1,   1,   1,   1,   1,   2,   2,   2,   2,   2,   3 },

	/* 1  */
	{  1,   1,   1,   1,   2,   2,   3,   3,   3,   4,   4,   4 },

	/* 2  */
	{  1,   1,   2,   2,   3,   3,   4,   4,   4,   5,   5,   5 },

	/* 3  */
	{  1,   2,   2,   3,   3,   4,   4,   4,   5,   5,   5,   5 },

	/* 4  */
	{  1,   2,   2,   3,   3,   4,   4,   5,   5,   5,   5,   5 },

	/* 5  */
	{  2,   2,   3,   3,   4,   4,   5,   5,   5,   5,   5,   5 },

	/* 6  */
	{  2,   2,   3,   3,   4,   4,   5,   5,   5,   5,   5,   5 },

	/* 7  */
	{  2,   3,   3,   4,   4,   4,   5,   5,   5,   5,   5,   5 },

	/* 8  */
	{  3,   3,   3,   4,   4,   4,   5,   5,   5,   5,   5,   5 },

	/* 9  */
	{  3,   3,   4,   4,   4,   4,   5,   5,   5,   5,   5,   5 },

	/* 10 */
	{  3,   3,   4,   4,   4,   4,   5,   5,   5,   5,   5,   5 },

	/* 11+ */
	{  3,   3,   4,   4,   4,   4,   5,   5,   5,   5,   5,   5 },
};


/*
 * This table allows quick conversion from "speed" to "energy"
 * The basic function WAS ((S>=110) ? (S-110) : (100 / (120-S)))
 * Note that table access is *much* quicker than computation.
 *
 * Note that the table has been changed at high speeds.  From
 * "Slow (-40)" to "Fast (+30)" is pretty much unchanged, but
 * at speeds above "Fast (+30)", one approaches an asymptotic
 * effective limit of 50 energy per turn.  This means that it
 * is relatively easy to reach "Fast (+30)" and get about 40
 * energy per turn, but then speed becomes very "expensive",
 * and you must get all the way to "Fast (+50)" to reach the
 * point of getting 45 energy per turn.  After that point,
 * furthur increases in speed are more or less pointless,
 * except to balance out heavy inventory.
 *
 * Note that currently the fastest monster is "Fast (+30)".
 *
 * It should be possible to lower the energy threshhold from
 * 100 units to 50 units, though this may interact badly with
 * the (compiled out) small random energy boost code.  It may
 * also tend to cause more "clumping" at high speeds.
 */
const byte extract_energy[200] =
{
	/* Slow */     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
	/* Slow */     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
	/* Slow */     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
	/* Slow */     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
	/* Slow */     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
	/* Slow */     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
	/* S-50 */     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
	/* S-40 */     2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
	/* S-30 */     2,  2,  2,  2,  2,  2,  2,  3,  3,  3,
	/* S-20 */     3,  3,  3,  3,  3,  4,  4,  4,  4,  4,
	/* S-10 */     5,  5,  5,  5,  6,  6,  7,  7,  8,  9,
	/* Norm */    10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
	/* F+10 */    20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
	/* F+20 */    30, 31, 32, 33, 34, 35, 36, 36, 37, 37,
	/* F+30 */    38, 38, 39, 39, 40, 40, 40, 41, 41, 41,
	/* F+40 */    42, 42, 42, 43, 43, 43, 44, 44, 44, 44,
	/* F+50 */    45, 45, 45, 45, 45, 46, 46, 46, 46, 46,
	/* F+60 */    47, 47, 47, 47, 47, 48, 48, 48, 48, 48,
	/* F+70 */    49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
	/* Fast */    49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
};


/*
 * Base experience levels, may be adjusted up for race and/or class
 */
const s32b player_exp[PY_MAX_LEVEL] =
{
	10,
	25,
	45,
	70,
	100,
	140,
	200,
	280,
	380,
	500,
	650,
	850,
	1100,
	1400,
	1800,
	2300,
	2900,
	3600,
	4400,
	5400,
	6800,
	8400,
	10200,
	12500,
	17500,
	25000,
	35000L,
	50000L,
	75000L,
	100000L,
	150000L,
	200000L,
	275000L,
	350000L,
	450000L,
	550000L,
	700000L,
	850000L,
	1000000L,
	1250000L,
	1500000L,
	1800000L,
	2100000L,
	2400000L,
	2700000L,
	3000000L,
	3500000L,
	4000000L,
	4500000L,
	5000000L
};


/*
 * Player Sexes
 *
 *	Title,
 *	Winner
 */
const player_sex sex_info[MAX_SEXES] =
{
	{
		"Female",
		"Queen"
	},

	{
		"Male",
		"King"
	}
};


/*
 * Each chest has a certain set of traps, determined by pval
 * Each chest has a "pval" from 1 to the chest level (max 55)
 * If the "pval" is negative then the trap has been disarmed
 * The "pval" of a chest determines the quality of its treasure
 * Note that disarming a trap on a chest also removes the lock.
 */
const byte chest_traps[64] =
{
	0,					/* 0 == empty */
	(CHEST_POISON),
	(CHEST_LOSE_STR),
	(CHEST_LOSE_CON),
	(CHEST_LOSE_STR),
	(CHEST_LOSE_CON),			/* 5 == best small wooden */
	0,
	(CHEST_POISON),
	(CHEST_POISON),
	(CHEST_LOSE_STR),
	(CHEST_LOSE_CON),
	(CHEST_POISON),
	(CHEST_LOSE_STR | CHEST_LOSE_CON),
	(CHEST_LOSE_STR | CHEST_LOSE_CON),
	(CHEST_LOSE_STR | CHEST_LOSE_CON),
	(CHEST_SUMMON),			/* 15 == best large wooden */
	0,
	(CHEST_LOSE_STR),
	(CHEST_LOSE_CON),
	(CHEST_PARALYZE),
	(CHEST_LOSE_STR | CHEST_LOSE_CON),
	(CHEST_SUMMON),
	(CHEST_PARALYZE),
	(CHEST_LOSE_STR),
	(CHEST_LOSE_CON),
	(CHEST_EXPLODE),			/* 25 == best small iron */
	0,
	(CHEST_POISON | CHEST_LOSE_STR),
	(CHEST_POISON | CHEST_LOSE_CON),
	(CHEST_LOSE_STR | CHEST_LOSE_CON),
	(CHEST_EXPLODE | CHEST_SUMMON),
	(CHEST_PARALYZE),
	(CHEST_POISON | CHEST_SUMMON),
	(CHEST_SUMMON),
	(CHEST_EXPLODE),
	(CHEST_EXPLODE | CHEST_SUMMON),	/* 35 == best large iron */
	0,
	(CHEST_SUMMON),
	(CHEST_EXPLODE),
	(CHEST_EXPLODE | CHEST_SUMMON),
	(CHEST_EXPLODE | CHEST_SUMMON),
	(CHEST_POISON | CHEST_PARALYZE),
	(CHEST_EXPLODE),
	(CHEST_EXPLODE | CHEST_SUMMON),
	(CHEST_EXPLODE | CHEST_SUMMON),
	(CHEST_POISON | CHEST_PARALYZE),	/* 45 == best small steel */
	0,
	(CHEST_LOSE_STR | CHEST_LOSE_CON),
	(CHEST_LOSE_STR | CHEST_LOSE_CON),
	(CHEST_POISON | CHEST_PARALYZE | CHEST_LOSE_STR),
	(CHEST_POISON | CHEST_PARALYZE | CHEST_LOSE_CON),
	(CHEST_POISON | CHEST_LOSE_STR | CHEST_LOSE_CON),
	(CHEST_POISON | CHEST_LOSE_STR | CHEST_LOSE_CON),
	(CHEST_POISON | CHEST_PARALYZE | CHEST_LOSE_STR | CHEST_LOSE_CON),
	(CHEST_POISON | CHEST_PARALYZE),
	(CHEST_POISON | CHEST_PARALYZE),	/* 55 == best large steel */
	(CHEST_EXPLODE | CHEST_SUMMON),
	(CHEST_EXPLODE | CHEST_SUMMON),
	(CHEST_EXPLODE | CHEST_SUMMON),
	(CHEST_EXPLODE | CHEST_SUMMON),
	(CHEST_EXPLODE | CHEST_SUMMON),
	(CHEST_EXPLODE | CHEST_SUMMON),
	(CHEST_EXPLODE | CHEST_SUMMON),
	(CHEST_EXPLODE | CHEST_SUMMON),
};


/*
 * Hack -- the "basic" color names (see "TERM_xxx")
 */
cptr color_names[16] =
{
	"Dark",
	"White",
	"Slate",
	"Orange",
	"Red",
	"Green",
	"Blue",
	"Umber",
	"Light Dark",
	"Light Slate",
	"Violet",
	"Yellow",
	"Light Red",
	"Light Green",
	"Light Blue",
	"Light Umber",
};


/*
 * Abbreviations of healthy stats
 */
cptr stat_names[A_MAX] =
{
	"STR: ", "INT: ", "WIS: ", "DEX: ", "CON: ", "CHR: "
};

/*
 * Abbreviations of damaged stats
 */
cptr stat_names_reduced[A_MAX] =
{
	"Str: ", "Int: ", "Wis: ", "Dex: ", "Con: ", "Chr: "
};

/*
 * Full stat names
 */
cptr stat_names_full[A_MAX] =
{
	"strength",
	"intelligence",
	"wisdom",
	"dexterity",
	"constitution",
	"charisma"
};


/*
 * Certain "screens" always use the main screen, including News, Birth,
 * Dungeon, Tomb-stone, High-scores, Macros, Colors, Visuals, Options.
 *
 * Later, special flags may allow sub-windows to "steal" stuff from the
 * main window, including File dump (help), File dump (artifacts, uniques),
 * Character screen, Small scale map, Previous Messages, Store screen, etc.
 */
cptr window_flag_desc[32] =
{
	"Display inven/equip",
	"Display equip/inven",
	"Display player (basic)",
	"Display player (extra)",
	NULL,
	NULL,
	"Display messages",
	"Display overhead view",
	"Display monster recall",
	"Display object recall",
	NULL,
	"Display snap-shot",
	"Display script variables",
	"Display script source",
	"Display borg messages",
	"Display borg status",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};


/*
 * Options -- textual names (where defined)
 */
cptr option_text[OPT_MAX] =
{
	"rogue_like_commands",		/* OPT_rogue_like_commands */
	"quick_messages",			/* OPT_quick_messages */
	"floor_query_flag",			/* OPT_floor_query_flag */
	"carry_query_flag",			/* OPT_carry_query_flag */
	"use_old_target",			/* OPT_use_old_target */
	"always_pickup",			/* OPT_always_pickup */
	"always_repeat",			/* OPT_always_repeat */
	"depth_in_feet",			/* OPT_depth_in_feet */
	"stack_force_notes",		/* OPT_stack_force_notes */
	"stack_force_costs",		/* OPT_stack_force_costs */
	"show_labels",				/* OPT_show_labels */
	"show_weights",				/* OPT_show_weights */
	"show_choices",				/* OPT_show_choices */
	"show_details",				/* OPT_show_details */
	"ring_bell",				/* OPT_ring_bell */
	"show_flavors",				/* OPT_flavors */
	"run_ignore_stairs",		/* OPT_run_ignore_stairs */
	"run_ignore_doors",			/* OPT_run_ignore_doors */
	"run_cut_corners",			/* OPT_run_cut_corners */
	"run_use_corners",			/* OPT_run_use_corners */
	"disturb_move",				/* OPT_disturb_move */
	"disturb_near",				/* OPT_disturb_near */
	"disturb_panel",			/* OPT_disturb_panel */
	"disturb_state",			/* OPT_disturb_state */
	"disturb_minor",			/* OPT_disturb_minor */
	NULL,						/* xxx disturb_other */
	NULL,						/* xxx alert_hitpoint */
	NULL,						/* xxx alert_failure */
	"verify_destroy",			/* OPT_verify_destroy */
	"verify_special",			/* OPT_verify_special */
	"allow_quantity",			/* OPT_allow_quantity */
	NULL,						/* xxx */
	"auto_haggle",				/* OPT_auto_haggle */
	"auto_scum",				/* OPT_auto_scum */
	NULL,						/* xxx testing_stack */
	NULL,						/* xxx testing_carry */
	"expand_look",				/* OPT_expand_look */
	"expand_list",				/* OPT_expand_list */
	"view_perma_grids",			/* OPT_view_perma_grids */
	"view_torch_grids",			/* OPT_view_torch_grids */
	"dungeon_align",			/* OPT_dungeon_align */
	"dungeon_stair",			/* OPT_dungeon_stair */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx track_follow */
	NULL,						/* xxx track_target */
	NULL,						/* xxx track_target */
	"smart_cheat",				/* OPT_smart_cheat */
	"view_reduce_lite",			/* OPT_view_reduce_lite */
	"hidden_player",			/* OPT_hidden_player */
	"avoid_abort",				/* OPT_avoid_abort */
	"avoid_other",				/* OPT_avoid_other */
	"flush_failure",			/* OPT_flush_failure */
	"flush_disturb",			/* OPT_flush_disturb */
	NULL,						/* xxx flush_command */
	"fresh_before",				/* OPT_fresh_before */
	"fresh_after",				/* OPT_fresh_after */
	NULL,						/* xxx fresh_message */
	"compress_savefile",		/* OPT_compress_savefile */
	"hilite_player",			/* OPT_hilite_player */
	"view_yellow_lite",			/* OPT_view_yellow_lite */
	"view_bright_lite",			/* OPT_view_bright_lite */
	"view_granite_lite",		/* OPT_view_granite_lite */
	"view_special_lite",		/* OPT_view_special_lite */
	"easy_open",				/* OPT_easy_open */
	"easy_alter",				/* OPT_easy_alter */
	"easy_floor",				/* OPT_easy_floor */
	"show_piles",				/* OPT_show_piles */
	"center_player",			/* OPT_center_player */
	"run_avoid_center",			/* OPT_run_avoid_center */
	"scroll_target",			/* OPT_scroll_target */
	"auto_more",				/* OPT_auto_more */
	NULL,						/* xxx */
	NULL,						/* xxx */
	"hp_changes_color",			/* OPT_hp_changes_color*/
	"verify_leave_quests",		/* OPT_verify_leave_quests*/
	"mark_squelch_items",		/* opt_mark_squelch_items */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	"birth_point_based",		/* OPT_birth_point_based */
	"birth_auto_roller",		/* OPT_birth_auto_roller */
	"birth_maximize",			/* OPT_birth_maximize */
	"birth_preserve",			/* OPT_birth_preserve */
	"birth_ironman",			/* OPT_birth_ironman */
	"birth_no_stores",			/* OPT_birth_no_stores */
	"birth_no_artifacts",		/* OPT_birth_no_artifacts */
	"birth_rand_artifacts",		/* OPT_birth_rand_artifacts */
	"birth_no_stacking",		/* OPT_birth_no_stacking */
 	"birth_take_notes",			/* OPT_birth_auto_notes */
 	"birth_force_small_lev",	/* OPT_birth_force_small_lev */
	"birth_retain_squelch", 	/* OPT_birth_retain_squelch */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	"cheat_peek",				/* OPT_cheat_peek */
	"cheat_hear",				/* OPT_cheat_hear */
	"cheat_room",				/* OPT_cheat_room */
	"cheat_xtra",				/* OPT_cheat_xtra */
	"cheat_know",				/* OPT_cheat_know */
	"cheat_live",				/* OPT_cheat_live */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	"adult_point_based",		/* OPT_adult_point_based */
	"adult_auto_roller",		/* OPT_adult_auto_roller */
	"adult_maximize",			/* OPT_adult_maximize */
	"adult_preserve",			/* OPT_adult_preserve */
	"adult_ironman",			/* OPT_adult_ironman */
	"adult_no_stores",			/* OPT_adult_no_stores */
	"adult_no_artifacts",		/* OPT_adult_no_artifacts */
	"adult_rand_artifacts",		/* OPT_adult_rand_artifacts */
	"adult_no_stacking",		/* OPT_adult_no_stacking */
	"adult_take_notes",			/* OPT_adult_auto_notes */
	"adult_force_small_lev",	/* OPT_adult_force_small_lev*/
	"adult_retain_squelch",		/* OPT_adult_retain_squelch */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	"score_peek",				/* OPT_score_peek */
	"score_hear",				/* OPT_score_hear */
	"score_room",				/* OPT_score_room */
	"score_xtra",				/* OPT_score_xtra */
	"score_know",				/* OPT_score_know */
	"score_live",				/* OPT_score_live */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL,						/* xxx */
	NULL						/* xxx */
};


/*
 * Options -- descriptions (where defined)
 */
cptr option_desc[OPT_MAX] =
{
	"Rogue-like commands",						/* OPT_rogue_like_commands */
	"Activate quick messages",					/* OPT_quick_messages */
	"Prompt for floor item selection",			/* OPT_floor_query_flag */
	"Prompt before picking things up",			/* OPT_carry_query_flag */
	"Use old target by default",				/* OPT_use_old_target */
	"Pick things up by default",				/* OPT_always_pickup */
	"Repeat obvious commands",					/* OPT_always_repeat */
	"Show dungeon level in feet",				/* OPT_depth_in_feet */
	"Merge inscriptions when stacking",			/* OPT_stack_force_notes */
	"Merge discounts when stacking",			/* OPT_stack_force_costs */
	"Show labels in equipment listings",		/* OPT_show_labels */
	"Show weights in all object listings",		/* OPT_show_weights */
	"Show choices in inven/equip windows",		/* OPT_show_choices */
	"Show details in monster descriptions",		/* OPT_show_details */
	"Audible bell (on errors, etc)",			/* OPT_ring_bell */
	"Show flavors in object descriptions",		/* OPT_show_flacors */
	"When running, ignore stairs",				/* OPT_run_ignore_stairs */
	"When running, ignore doors",				/* OPT_run_ignore_doors */
	"When running, cut corners",				/* OPT_run_cut_corners */
	"When running, use corners",				/* OPT_run_use_corners */
	"Disturb whenever any monster moves",		/* OPT_disturb_move */
	"Disturb whenever viewable monster moves",	/* OPT_disturb_near */
	"Disturb whenever map panel changes",		/* OPT_disturb_panel */
	"Disturb whenever player state changes",	/* OPT_disturb_state */
	"Disturb whenever boring things happen",	/* OPT_disturb_minor */
	NULL,										/* xxx disturb_other */
	NULL,										/* xxx alert_hitpoint */
	NULL,										/* xxx alert_failure */
	"Verify destruction of objects",			/* OPT_verify_destroy */
	"Verify use of special commands",			/* OPT_verify_special */
	"Allow quantity specification",				/* OPT_allow_quantity */
	NULL,										/* xxx */
	"Auto-haggle in stores",					/* OPT_auto_haggle */
	"Auto-scum for good levels",				/* OPT_auto_scum */
	NULL,										/* xxx testing_stack */
	NULL,										/* xxx testing_carry */
	"Expand the power of the look command",		/* OPT_expand_look */
	"Expand the power of the list commands",	/* OPT_expand_list */
	"Map remembers all perma-lit grids",		/* OPT_view_perma_grids */
	"Map remembers all torch-lit grids",		/* OPT_view_torch_grids */
	"Generate dungeons with aligned rooms",		/* OPT_dungeon_align */
	"Generate dungeons with connected stairs",	/* OPT_dungeon_stair */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx track_follow */
	NULL,										/* xxx track_target */
	NULL,										/* xxx track_target */
	"Monsters exploit players weaknesses",		/* OPT_smart_cheat */
	"Reduce lite-radius when running",			/* OPT_view_reduce_lite */
	"Hide player symbol when running",			/* OPT_hidden_player */
	"Avoid checking for user abort",			/* OPT_avoid_abort */
	"Avoid processing special colors",			/* OPT_avoid_other */
	"Flush input on various failures",			/* OPT_flush_failure */
	"Flush input whenever disturbed",			/* OPT_flush_disturb */
	NULL,										/* xxx */
	"Flush output before every command",		/* OPT_fresh_before */
	"Flush output after various things",		/* OPT_fresh_after */
	NULL,										/* xxx */
	"Compress messages in savefiles",			/* OPT_compress_savefile */
	"Hilite the player with the cursor",		/* OPT_hilite_player */
	"Use special colors for torch lite",		/* OPT_view_yellow_lite */
	"Use special colors for field of view",		/* OPT_view_bright_lite */
	"Use special colors for wall grids",		/* OPT_view_granite_lite */
	"Use special colors for floor grids",		/* OPT_view_special_lite */
	"Open/Disarm/Close without direction",		/* OPT_easy_open */
	"Open/Disarm doors/traps on movement",		/* OPT_easy_alter */
	"Display floor stacks in a list",   		/* OPT_easy_floor */
	"Show stacks using special attr/char",		/* OPT_show_piles */
	"Center map continuously (very slow)",		/* OPT_center_player */
	"Avoid centering while running",			/* OPT_run_avoid_center */
	"Scroll map while targetting",				/* OPT_scroll_target */
	"Automatically clear '-more-' prompts",		/* OPT_auto_more */
	NULL,										/* xxx */
	NULL,										/* xxx */
	"Player color indicates low hit points",	/* OPT_hp_changes_color */
	"Verify before descending from quest level",/* OPT_verify_leave_quest */
	"Items marked for squelch appear as dot",   /* OPT_mark_squelch_items */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	"Birth: Allow purchase of stats using points",	/* OPT_birth_point_based */
	"Birth: Allow specification of minimal stats",	/* OPT_birth_auto_roller */
	"Birth: Maximize effect of race/class bonuses",	/* OPT_birth_maximize */
	"Birth: Preserve artifacts when leaving level",	/* OPT_birth_preserve */
	"Birth: Restrict the use of stairs/recall",	/* OPT_birth_ironman */
	"Birth: Restrict the use of stores/home",	/* OPT_birth_no_stores */
	"Birth: Restrict creation of artifacts",	/* OPT_birth_no_artifacts */
	"Birth: Randomize some of the artifacts (beta)",	/* OPT_birth_rand_artifacts */
	"Birth: Never stack objects on the floor",	/* OPT_birth_no_stacking */
 	"Birth: Have notes written to a file",		/* OPT_birth_take_notes */
 	"Birth: All levels will be generated as small",	/* OPT_birth_force_small_lev */
	"Birth: Retain squelch settings",			/*OPT_birth_retain_squelch*/
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	"Cheat: Peek into object creation",			/* OPT_cheat_peek */
	"Cheat: Peek into monster creation",		/* OPT_cheat_hear */
	"Cheat: Peek into dungeon creation",		/* OPT_cheat_room */
	"Cheat: Peek into something else",			/* OPT_cheat_xtra */
	"Cheat: Know complete monster info",		/* OPT_cheat_know */
	"Cheat: Allow player to avoid death",		/* OPT_cheat_live */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	"Adult: Allow purchase of stats using points",	/* OPT_adult_point_based */
	"Adult: Allow specification of minimal stats",	/* OPT_adult_auto_roller */
	"Adult: Maximize effect of race/class bonuses",	/* OPT_adult_maximize */
	"Adult: Preserve artifacts when leaving level",	/* OPT_adult_preserve */
	"Adult: Restrict the use of stairs/recall",	/* OPT_adult_ironman */
	"Adult: Restrict the use of stores/home",	/* OPT_adult_no_stores */
	"Adult: Restrict creation of artifacts",	/* OPT_adult_no_artifacts */
	"Adult: Randomize some of the artifacts (beta)",	/* OPT_adult_rand_artifacts */
	"Adult: Never stack objects on the floor",	/* OPT_adult_adult_no_stacking */
	"Adult: Have notes to written to a file",	/* OPT_adult_take_notes */
	"Adult: All levels generated small",		/* OPT_adult_force_small_lev */
	"Adult: Retain squelch settings",			/* OPT_adult_retain_squelch*/
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	"Score: Peek into object creation",			/* OPT_score_peek */
	"Score: Peek into monster creation",		/* OPT_score_hear */
	"Score: Peek into dungeon creation",		/* OPT_score_room */
	"Score: Peek into something else",			/* OPT_score_xtra */
	"Score: Know complete monster info",		/* OPT_score_know */
	"Score: Allow player to avoid death",		/* OPT_score_live */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL,										/* xxx */
	NULL										/* xxx */
};


/*
 * Options -- normal values
 */
const bool option_norm[OPT_MAX] =
{
	FALSE,		/* OPT_rogue_like_commands */
	TRUE,		/* OPT_quick_messages */
	FALSE,		/* OPT_floor_query_flag */
	TRUE,		/* OPT_carry_query_flag */
	FALSE,		/* OPT_use_old_target */
	TRUE,		/* OPT_always_pickup */
	FALSE,		/* OPT_always_repeat */
	FALSE,		/* OPT_depth_in_feet */
	FALSE,		/* OPT_stack_force_notes */
	FALSE,		/* OPT_stack_force_costs */
	TRUE,		/* OPT_show_labels */
	TRUE,		/* OPT_show_weights */
	TRUE,		/* OPT_show_choices */
	TRUE,		/* OPT_show_details */
	TRUE,		/* OPT_ring_bell */
	TRUE,		/* OPT_show_flavors */
	TRUE,		/* OPT_run_ignore_stairs */
	TRUE,		/* OPT_run_ignore_doors */
	TRUE,		/* OPT_run_cut_corners */
	TRUE,		/* OPT_run_use_corners */
	TRUE,		/* OPT_disturb_move */
	TRUE,		/* OPT_disturb_near */
	TRUE,		/* OPT_disturb_panel */
	TRUE,		/* OPT_disturb_state */
	TRUE,		/* OPT_disturb_minor */
	FALSE,		/* xxx disturb_other */
	FALSE,		/* xxx alert_hitpoint */
	FALSE,		/* xxx alert_failure */
	TRUE,		/* OPT_verify_destroy */
	TRUE,		/* OPT_verify_special */
	TRUE,		/* OPT_allow_quantity */
	FALSE,		/* xxx */
	TRUE,		/* OPT_auto_haggle */
	FALSE,		/* OPT_auto_scum */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	TRUE,		/* OPT_expand_look */
	TRUE,		/* OPT_expand_list */
	TRUE,		/* OPT_view_perma_grids */
	FALSE,		/* OPT_view_torch_grids */
	TRUE,		/* OPT_dungeon_align */
	TRUE,		/* OPT_dungeon_stair */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx track_follow */
	FALSE,		/* xxx track_target */
	FALSE,		/* xxx */
	FALSE,		/* OPT_smart_cheat */
	FALSE,		/* OPT_view_reduce_lite */
	FALSE,		/* OPT_hidden_player */
	FALSE,		/* OPT_avoid_abort */
	FALSE,		/* OPT_avoid_other */
	TRUE,		/* OPT_flush_failure */
	FALSE,		/* OPT_flush_disturb */
	FALSE,		/* xxx */
	TRUE,		/* OPT_fresh_before */
	FALSE,		/* OPT_fresh_after */
	FALSE,		/* xxx */
	TRUE,		/* OPT_compress_savefile */
	FALSE,		/* OPT_hilite_player */
	FALSE,		/* OPT_view_yellow_lite */
	FALSE,		/* OPT_view_bright_lite */
	FALSE,		/* OPT_view_granite_lite */
	FALSE,		/* OPT_view_special_lite */
	FALSE,		/* OPT_easy_open */
	FALSE,		/* OPT_easy_alter */
	FALSE,		/* OPT_easy_floor */
	FALSE,		/* OPT_show_piles */
	FALSE,		/* OPT_center_player */
	FALSE,		/* OPT_run_avoid_center */
	FALSE,		/* OPT_scroll_target */
	FALSE,		/* OPT_auto_more */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	TRUE,		/* OPT_verify_leave_quests */
	FALSE,		/* OPT_mark_squelch_items */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* OPT_birth_point_based */
	FALSE,		/* OPT_birth_auto_roller */
	TRUE,		/* OPT_birth_maximize */
	TRUE,		/* OPT_birth_preserve */
	FALSE,		/* OPT_birth_ironman */
	FALSE,		/* OPT_birth_no_stores */
	FALSE,		/* OPT_birth_no_artifacts */
	FALSE,		/* OPT_birth_rand_artifacts */
	FALSE,		/* OPT_birth_no_stacking */
	TRUE,		/* OPT_birth_take_notes */
	FALSE,		/* OPT_birth_force_small_lev */
	FALSE,		/* OPT_birth_retain_squelch */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* OPT_cheat_peek */
	FALSE,		/* OPT_cheat_hear */
	FALSE,		/* OPT_cheat_room */
	FALSE,		/* OPT_cheat_xtra */
	FALSE,		/* OPT_cheat_know */
	FALSE,		/* OPT_cheat_live */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* OPT_adult_point_based */
	FALSE,		/* OPT_adult_auto_roller */
	TRUE,		/* OPT_adult_maximize */
	TRUE,		/* OPT_adult_preserve */
	FALSE,		/* OPT_adult_ironman */
	FALSE,		/* OPT_adult_no_stores */
	FALSE,		/* OPT_adult_no_artifacts */
	FALSE,		/* OPT_adult_rand_artifacts */
	FALSE,		/* OPT_adult_no_stacking */
	TRUE,		/* OPT_adult_take_notes */
	FALSE,		/* OPT_adult_force_small_lev*/
	FALSE,		/* OPT_adult_retain_squelch */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* OPT_score_peek */
	FALSE,		/* OPT_score_hear */
	FALSE,		/* OPT_score_room */
	FALSE,		/* OPT_score_xtra */
	FALSE,		/* OPT_score_know */
	FALSE,		/* OPT_score_live */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE,		/* xxx */
	FALSE		/* xxx */
};


/*
 * Option screen interface
 */
const byte option_page[OPT_PAGE_MAX][OPT_PAGE_PER] =
{
	/*** User-Interface ***/

	{
		OPT_rogue_like_commands,
		OPT_quick_messages,
		OPT_floor_query_flag,
		OPT_carry_query_flag,
		OPT_use_old_target,
		OPT_always_pickup,
		OPT_always_repeat,
		OPT_stack_force_notes,
		OPT_stack_force_costs,
		OPT_ring_bell,
		OPT_easy_open,
		OPT_easy_alter,
		OPT_easy_floor,
		OPT_scroll_target,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE
	},

	/*** Disturbance ***/

	{
		OPT_run_ignore_stairs,
		OPT_run_ignore_doors,
		OPT_run_cut_corners,
		OPT_run_use_corners,
		OPT_disturb_move,
		OPT_disturb_near,
		OPT_disturb_panel,
		OPT_disturb_state,
		OPT_disturb_minor,
		OPT_verify_destroy,
		OPT_verify_special,
		OPT_allow_quantity,
		OPT_auto_more,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE
	},

	/*** Game-Play ***/

	{
		OPT_auto_haggle,
		OPT_auto_scum,
		OPT_expand_look,
		OPT_expand_list,
		OPT_view_perma_grids,
		OPT_view_torch_grids,
		OPT_dungeon_align,
		OPT_dungeon_stair,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_smart_cheat,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE
	},

	/*** Efficiency ***/

	{
		OPT_view_reduce_lite,
		OPT_hidden_player,
		OPT_avoid_abort,
		OPT_avoid_other,
		OPT_flush_failure,
		OPT_flush_disturb,
		OPT_fresh_before,
		OPT_fresh_after,
		OPT_compress_savefile,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
	},

	/*** Display ***/

	{
		OPT_depth_in_feet,
		OPT_show_labels,
		OPT_show_weights,
		OPT_show_choices,
		OPT_show_details,
		OPT_show_flavors,
		OPT_hilite_player,
		OPT_view_yellow_lite,
		OPT_view_bright_lite,
		OPT_view_granite_lite,
		OPT_view_special_lite,
 		OPT_center_player,
 		OPT_run_avoid_center,
		OPT_show_piles,
		OPT_hp_changes_color,
		OPT_verify_leave_quest,
		OPT_mark_squelch_items,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE
	},

	/*** Birth ***/

	{
		OPT_birth_point_based,
		OPT_birth_auto_roller,
		OPT_birth_maximize,
		OPT_birth_preserve,
		OPT_birth_ironman,
		OPT_birth_no_stores,
		OPT_birth_no_artifacts,
		OPT_birth_rand_artifacts,
		OPT_birth_no_stacking,
	 	OPT_birth_take_notes,
		OPT_birth_force_small_lev,
		OPT_birth_retain_squelch,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE
	},

	/*** Cheat ***/

	{
		OPT_cheat_peek,
		OPT_cheat_hear,
		OPT_cheat_room,
		OPT_cheat_xtra,
		OPT_cheat_know,
		OPT_cheat_live,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE,
		OPT_NONE
	}
};


cptr inscrip_text[MAX_INSCRIP] =
{
	NULL,
	"terrible",
	"worthless",
	"cursed",
	"broken",
	"average",
	"good",
	"excellent",
	"special",
	"uncursed",
	"indestructible"
};



byte mana_cost_RF4[32]=
{
	1,			/* RF4_SHIEIK */
	0,			/* RF4_LASH */
	0,			/* RF4_BOULDER */
	0,			/* RF4_SHOT */
	0,			/* RF4_ARROW */
	0,			/* RF4_BOLT */
	0,			/* RF4_MISSL */
	0,			/* RF4_PMISSLE */
	0,			/* RF4_BRTH_ACID */
	0,			/* RF4_BRTH_ELEC */
	0,			/* RF4_BRTH_FIRE */
	0,			/* RF4_BRTH_COLD */
	0,			/* RF4_BRTH_POIS */
	0,			/* RF4_BRTH_PLAS */
	0,			/* RF4_BRTH_LITE */
	0,			/* RF4_BRTH_DARK */
	0,			/* RF4_BRTH_CONFU */
	0,			/* RF4_BRTH_SOUND */
	0,			/* RF4_BRTH_SHARD */
	0,			/* RF4_BRTH_INER */
	0,			/* RF4_BRTH_GRAV */
	0,			/* RF4_BRTH_FORCE */
	0,			/* RF4_BRTH_NEXUS */
	0,			/* RF4_BRTH_NETHR */
	0,			/* RF4_BRTH_CHAOS */
	0,			/* RF4_BRTH_DISEN */
	0,			/* RF4_BRTH_TIME */
	0,			/* RF4_BRTH_MANA */
	0,			/* RF4_XXX3 */
	0,			/* RF4_XXX4 */
	0,			/* RF4_XXX5 */
	0			/* RF4_XXX6 */
};

byte mana_cost_RF5[32]=
{
	4,			/* RF5_BALL_ACID */
	4,			/* RF5_BALL_ELEC */
	4,			/* RF5_BALL_FIRE */
	4,			/* RF5_BALL_COLD */
	4,			/* RF5_BALL_POIS */
	5, 			/* RF5_BALL_LITE */
	5, 			/* RF5_BALL_DARK */
	6, 			/* RF5_BALL_CONFU */
	4, 			/* RF5_BALL_SOUND */
	4, 			/* RF5_BALL_SHARD */
	5, 			/* RF5_BALL_WIND */
	5, 			/* RF5_BALL_STORM */
	6, 			/* RF5_BALL_NETHR */
	7, 			/* RF5_BALL_CHAOS */
	7, 			/* RF5_BALL_MANA */
	7, 			/* RF5_BALL_WATER */
	4, 			/* RF5_BOLT_ACID */
	4, 			/* RF5_BOLT_ELEC */
	4, 			/* RF5_BOLT_FIRE */
	4, 			/* RF5_BOLT_COLD */
	4, 			/* RF5_BOLT_POIS */
	5, 			/* RF5_BOLT_PLAS */
	5, 			/* RF5_BOLT_ICE */
	5, 			/* RF5_BOLT_WATER */
	5, 			/* RF5_BOLT_NETHER */
	5, 			/* RF5_BOLT_MANA */
	0, 			/* RF5_XXX3 */
	6, 			/* RF5_BEAM_ELEC */
	6, 			/* RF5_BEAM_ICE */
	6, 			/* RF5_BEAM_NETHER */
	0, 			/* RF5_XXX4 */
	5  			/* RF5_HOLY_ORB */
};

byte mana_cost_RF6[32]=
{
	6, 			/* RF6_HASTE */
	0, 			/* RF6_ADD_MANA */
	3, 			/* RF6_HEAL */
	3, 			/* RF6_CURE */
	3, 			/* RF6_BLINK */
	6, 			/* RF6_TPORT */
	0, 			/* RF6_XXX1 */
	4, 			/* RF6_TELE_SELF_TO */
	4, 			/* RF6_TELE_TO */
	8, 			/* RF6_TELE_AWAY */
	8, 			/* RF6_TELE_LEVEL */
	0, 			/* RF6_XXX2 */
	1, 			/* RF6_DARKNESS */
	2, 			/* RF6_TRAPS */
	6, 			/* RF6_FORGET */
	2, 			/* RF6_DRAIN_MANA */
	0, 			/* RF6_XXX4 */
	0, 			/* RF6_XXX5 */
	3, 			/* RF6_MIND_BLAST */
	4, 			/* RF6_BRAIN_SMASH */
	4, 			/* RF6_WOUND */
	0, 			/* RF6_XXX6 */
	0, 			/* RF6_XXX7 */
	0, 			/* RF6_XXX8 */
	0, 			/* RF6_XXX9 */
	2, 			/* RF6_HUNGER */
	0, 			/* RF6_XX11 */
	1, 			/* RF6_SCARE */
	3, 			/* RF6_BLIND */
	4, 			/* RF6_CONF */
	5, 			/* RF6_SLOW */
	6  			/* RF6_HOLD */
};

byte mana_cost_RF7[32]=
{
	12,			/* RF7_S_KIN */ /* Summon - 6 */
	0, 			/* RF7_XXX1 */
	0, 			/* RF7_XXX2 */
	10,			/* RF7_S_MONSTER */ /* Summon - 1 */
	15,			/* RF7_S_MONSTERS */ /* Summon - 8 */
	0,			/* RF7_XXX3 */
	0, 			/* RF7_XXX4 */
	0, 			/* RF7_XXX5 */
	10, 		/* RF7_S_ANT */ /* Summon - 6 */
	12,			/* RF7_S_SPIDER */ /* Summon - 6 */
	14,			/* RF7_S_HOUND */ /* Summon - 6 */
	15,			/* RF7_S_ANIMAL */ /* Summon - 6 */
	15, 		/* RF7_S_HYDRA */
	0, 			/* RF7_XXX7 */
	15,			/* RF7_S_THIEF */ /* Summon - 6 */
	5, 			/* RF7_S_BERTBILLTOM */ /* Summon - 2 */
	0, 			/* RF7_XXX8 */
	15, 			/* RF7_S_AINU */
	0, 			/* RF7_XX10 */
	0, 			/* RF7_XX11 */
	14,			/* RF7_S_DRAGON */ /* Summon - 1 */
	20,			/* RF7_S_HI_DRAGON */ /* Summon - 8 */
	0, 			/* RF7_XX12 */
	0, 			/* RF7_XX13 */
	14,			/* RF7_S_DEMON */ /* Summon - 1 / 2-3 */
	20,			/* RF7_S_HI_DEMON */ /* Summon - 8 */
	0, 			/* RF7_XX14 */
	0, 			/* RF7_XX15 */
	12,			/* RF7_S_UNDEAD */ /* Summon - 1 */
	20,			/* RF7_S_HI_UNDEAD */ /* Summon - 8 */
	20,			/* RF7_S_WRAITH */ /* Summon - 8 */
	20 			/* RF7_S_UNIQUE */ /* Summon - 8 */
};

/*
 * d_base:     base desirability for AI.
 * d_summ:     desriability for AI per monster level
 *                  times 0-3 based on number of clear spaces
 * d_hurt:     desirability for AI per monster spell power
 *                  times 0-3 based on damage taken
 * d_mana:     desirability for AI per monster spell power
 *                  times 0-2 based on mana shortage
 * d_esc:      desirability for AI per monster level
 *                  times 0-3 based on fear, and damage taken
 * d_tact:     desirability for AI per monster level, modified
 *                  times 0-3 based on proximity, min_range, and best_range
 * d_res:      category of 'resistability' checked by monster AI
 *                 for purposes of desirability.
 * d_range:    % of spell desirability retained for each step past 'range'
 */

byte spell_desire_RF4[32][8] =
{
/*     d_base	  d_hurt    d_esc	 d_res				    */
/*	     d_summ	d_mana	  d_tact	   d_range		    */
	{ 30,  0,   0,   5,	0,   0,	   0	  ,  100}, /* RF4_SHRIEK    */
	{ 40,  0,   0,   5,	0,   0,	   0	  ,    0}, /* RF4_LASH	    */
	{ 40,  0,   0,   5,	0,   0, LRN_ARCH  ,  100}, /* RF4_BOULDER   */
	{ 40,  0,   0,   5,	0,   0, LRN_ARCH  ,  100}, /* RF4_SHOT	    */
	{ 40,  0,   0,   5,	0,   0, LRN_ARCH  ,  100}, /* RF4_ARROW	    */
	{ 40,  0,   0,   5,	0,   0, LRN_ARCH  ,  100}, /* RF4_BOLT	    */
	{ 40,  0,   0,   5,	0,   0, LRN_ARCH  ,  100}, /* RF4_MISSL	    */
	{ 40,  0,   0,   5,	0,   0, LRN_PARCH ,  100}, /* RF4_PMISSL    */
	{ 75,  0,   0,   5,	0,   0, LRN_ACID  ,   90}, /* RF4_BRTH_ACID */
	{ 75,  0,   0,   5,	0,   0, LRN_ELEC  ,   90}, /* RF4_BRTH_ELEC */
	{ 75,  0,   0,   5,	0,   0, LRN_FIRE  ,   90}, /* RF4_BRTH_FIRE */
	{ 75,  0,   0,   5,	0,   0, LRN_COLD  ,   90}, /* RF4_BRTH_COLD */
	{ 65,  0,   0,   5,	0,   0, LRN_POIS  ,   90}, /* RF4_BRTH_POIS */
	{ 65,  0,   0,   5,	0,   0, LRN_PLAS  ,   90}, /* RF4_BRTH_PLAS */
	{ 65,  0,   0,   5,	0,   0, LRN_LITE  ,   90}, /* RF4_BRTH_LITE */
	{ 65,  0,   0,   5,	0,   0, LRN_DARK  ,   90}, /* RF4_BRTH_DARK */
	{ 65,  0,   0,   5,	0,   0, LRN_CONFU ,   90}, /* RF4_BRTH_CONFU*/
	{ 65,  0,   0,   5,	0,   0, LRN_SOUND ,   90}, /* RF4_BRTH_SOUND*/
	{ 65,  0,   0,   5,	0,   0, LRN_SHARD ,   90}, /* RF4_BRTH_SHARD*/
	{ 65,  0,   0,   5,	0,   0,	   0	  ,   90}, /* RF4_BRTH_INER */
	{ 65,  0,   0,   5,	0,   0, LRN_SOUND2,   90}, /* RF4_BRTH_GRAV */
	{ 65,  0,   0,   5,	0,   0,	   0	  ,   90}, /* RF4_BRTH_WIND */
	{ 65,  0,   0,   5,	0,   0, LRN_SOUND2,   90}, /* RF4_BRTH_FORCE*/
	{ 65,  0,   0,   5,	0,   0, LRN_NEXUS ,   90}, /* RF4_BRTH_NEXUS*/
	{ 65,  0,   0,   5,	0,   0, LRN_NETHR ,   90}, /* RF4_BRTH_NETHR*/
	{ 65,  0,   0,   5,	0,   0, LRN_CHAOS ,   90}, /* RF4_BRTH_CHAOS*/
	{ 65,  0,   0,   5,	0,   0, LRN_DISEN ,   90}, /* RF4_BRTH_DISEN*/
	{ 65,  0,   0,   5,	0,   0,	   0	  ,   90}, /* RF4_BRTH_TIME */
	{ 65,  0,   0,   5,	0,   0,	   0	  ,   90}, /* RF4_BRTH_MANA */
	{ 0,   0,   0,   0,	0,   0,	   0	  ,  100}, /* RF4_XXX1 */
	{ 0,   0,   0,   0,	0,   0,	   0	  ,  100}, /* RF4_XXX2 */
	{ 0,   0,   0,   0,	0,   0,	   0	  ,  100}  /* RF4_XXX3 */
};

byte spell_desire_RF5[32][8] =
{
/*     d_base	  d_hurt    d_esc	 d_res				    */
/*	     d_summ	d_mana	  d_tact	   d_range		    */
	{ 50,  0,   0,   0,	0,   0, LRN_ACID  ,  100}, /* RF5_BALL_ACID */
	{ 50,  0,   0,   0,	0,   0, LRN_ELEC  ,  100}, /* RF5_BALL_ELEC */
	{ 50,  0,   0,   0,	0,   0, LRN_FIRE  ,  100}, /* RF5_BALL_FIRE */
	{ 50,  0,   0,   0,	0,   0, LRN_COLD  ,  100}, /* RF5_BALL_COLD */
	{ 50,  0,   0,   0,	0,   0, LRN_POIS  ,  100}, /* RF5_BALL_POIS */
	{ 40,  0,   0,   0,	0,   0, LRN_LITE  ,  100}, /* RF5_BALL_LITE */
	{ 40,  0,   0,   0,	0,   0, LRN_DARK  ,  100}, /* RF5_BALL_DARK */
	{ 40,  0,   0,   0,	0,   0, LRN_CONFU ,  100}, /* RF5_BALL_CONFU*/
	{ 40,  0,   0,   0,	0,   0, LRN_SOUND ,  100}, /* RF5_BALL_SOUND*/
	{ 40,  0,   0,   0,	0,   0, LRN_SHARD ,  100}, /* RF5_BALL_SHARD*/
	{ 40,  0,   0,   0,	0,   0,	   0	  ,  100}, /* RF5_XXX1 */
	{ 40,  0,   0,   0,	0,   0, LRN_STORM ,  100}, /* RF5_BALL_STORM*/
	{ 40,  0,   0,   0,	0,   0, LRN_NETHR ,  100}, /* RF5_BALL_NETHR*/
	{ 40,  0,   0,   0,	0,   0, LRN_CHAOS ,  100}, /* RF5_BALL_CHAOS*/
	{ 40,  0,   0,   0,	0,   0,	   0	  ,  100}, /* RF5_BALL_MANA */
	{ 40,  0,   0,   0,	0,   0, LRN_WATER ,  100}, /* RF5_BALL_WATER*/
	{ 40,  0,   0,   0,	0,   0, LRN_ACID  ,  100}, /* RF5_BOLT_ACID */
	{ 40,  0,   0,   0,	0,   0, LRN_ELEC  ,  100}, /* RF5_BOLT_ELEC */
	{ 40,  0,   0,   0,	0,   0, LRN_FIRE  ,  100}, /* RF5_BOLT_FIRE */
	{ 40,  0,   0,   0,	0,   0, LRN_COLD  ,  100}, /* RF5_BOLT_COLD */
	{ 40,  0,   0,   0,	0,   0, LRN_POIS  ,  100}, /* RF5_BOLT_POIS */
	{ 50,  0,   0,   0,	0,   0, LRN_PLAS  ,  100}, /* RF5_BOLT_PLAS */
	{ 50,  0,   0,   0,	0,   0, LRN_ICE	  ,  100}, /* RF5_BOLT_ICE  */
	{ 35,  0,   0,   0,	0,   0, LRN_WATER ,  100}, /* RF5_BOLT_WATER*/
	{ 35,  0,   0,   0,	0,   0, LRN_NETHR ,  100}, /* RF5_BOLT_NETHR*/
	{ 30,  0,   0,   0,	0,   0,	   0	  ,  100}, /* RF5_BOLT_MANA */
	{ 0,  0,   0,   0,	0,   0,    0  	  ,  100}, /* RF5_XXX3*/
	{ 50,  0,   0,   0,	0,   0, LRN_ELEC  ,   90}, /* RF5_BEAM_ELEC */
	{ 50,  0,   0,   0,	0,   0, LRN_ICE	  ,   90}, /* RF5_BEAM_ICE  */
	{ 50,  0,   0,   0,	0,   0, LRN_NETHR ,   90}, /* RF5_BEAM_NETHR*/
	{ 0,  0,   0,   0,	0,   0,	   0	  ,  100},  /* RF5_XXX4*/
	{ 60,  0,   0,   0,	0,   0,	   0	  ,  100} 	/* RF5_HOLY_ORB */
};


byte spell_desire_RF6[32][8] =
{
/*     d_base	  d_hurt    d_esc	 d_res				    */
/*	     d_summ	d_mana	  d_tact	   d_range		    */
	{ 50,  0,   0,   0,	0,   0,	   0	  ,  100}, /* RF6_HASTE	    */
	{ 15,  0,   0,  25,	0,   0,	   0  ,  100}, /* RF6_ADD_MANA  */
	{ 10,  0,   30,  0,	0,   0,	   0	  ,  100}, /* RF6_HEAL	    */
	{ 50,  0,   0,   0,	0,   0,	   0	  ,  100}, /* RF6_CURE	    */
	{ 27,  0,   0,   0,	10,  15,   0	  ,  100}, /* RF6_BLINK	    */
	{ 10,  0,   0,   0,	20,  10,   0	  ,  100}, /* RF6_TPORT	    */
	{ 0,   0,   0,   0,	0,   0,	   0	  ,  100}, /* RF6_XXX1	    */
	{ 30,  0,   0,   0,	0,   0,	   0	  ,  100}, /* RF6_TELE_SELF_TO*/
	{ 30,  0,   0,   0,	0,   10,   0	  ,  100}, /* RF6_TELE_TO   */
	{ 10,  0,   0,   0,	20,  10,   0	  ,  100}, /* RF6_TELE_AWAY */
	{ 10,  0,   0,   0,	20,  10,LRN_NEXUS_SAVE,	   100}, /* RF6_TELE_LEVEL */
	{ 0,   0,   0,   0,	0,   0,	   0	  ,  100}, /* RF6_XXX3	    */
	{ 20,  0,   0,   0,	5,   0,	   0	  ,  100}, /* RF6_DARKNESS  */
	{ 25,  0,   0,   0,	5,   0,	   0	  ,  100}, /* RF6_TRAPS	    */
	{ 25,  0,   0,   0,	5,   0, LRN_SAVE  ,  100}, /* RF6_FORGET    */
	{ 25,  0,   0,   15,	0,   0, LRN_MANA  ,  100}, /* RF6_DRAIN_MANA*/
	{ 0,   0,   0,   0,	0,   0,	   0	  ,  100}, /* RF6_XXX4	    */
	{ 0,   0,   0,   0,	0,   0,	   0	  ,  100}, /* RF6_XXX5	    */
	{ 30,  0,   0,   0,	0,   0, LRN_SAVE  ,  100}, /* RF6_MIND_BLAST*/
	{ 40,  0,   0,   0,	0,   0, LRN_SAVE  ,  100}, /* RF6_BRAIN_SMASH*/
	{ 40,  0,   0,   0,	0,   0, LRN_SAVE  ,  100}, /* RF6_WOUND	    */
	{ 0,   0,   0,   0,	0,   0,	   0	  ,  100}, /* RF6_XXX6	    */
	{ 0,   0,   0,   0,	0,   0,	   0	  ,  100}, /* RF6_XXX7	    */
	{ 0,   0,   0,   0,	0,   0,	   0	  ,  100}, /* RF6_XXX8	    */
	{ 0,   0,   0,   0,	0,   0,	   0	  ,  100}, /* RF6_XXX9	    */
	{ 25,  0,   0,   0,	0,   0,	 LRN_SAVE ,  100}, /* RF6_HUNGER    */
	{ 0,   0,   0,   0,	0,   0,	   0	  ,  100}, /* RF6_XX11	    */
	{ 25,  0,   0,   0,	0,   0, LRN_FEAR_SAVE,	  100}, /* RF6_SCARE	 */
	{ 30,  0,   0,   0,	0,   0, LRN_BLIND_SAVE,	   100}, /* RF6_BLIND	  */
	{ 30,  0,   0,   0,	0,   0, LRN_CONFU_SAVE,	   100}, /* RF6_CONF	  */
	{ 40,  0,   0,   0,	0,   0, LRN_FREE_SAVE,	  100}, /* RF6_SLOW	 */
	{ 35,  0,   0,   0,	0,   0, LRN_FREE_SAVE,	  100} /* RF6_HOLD	*/
};

byte spell_desire_RF7[32][8] =
{
 /*     d_base	  d_hurt    d_esc	 d_res				    */
 /*	     d_summ	d_mana	  d_tact	   d_range		    */
	{ 0,   15,  0,   0,	0,   0,	   0	  ,  100}, /* RF7_S_KIN	    */
	{ 0,   0,   0,   0,	0,   0,	   0	  ,  100}, /* RF7_XXX1	    */
	{ 0,   0,   0,   0,	0,   0,	   0	  ,  100}, /* RF7_XXX2	    */
	{ 0,   15,  0,   0,	0,   0,	   0	  ,  100}, /* RF7_S_MONSTER */
	{ 0,   15,  0,   0,	0,   0,	   0	  ,  100}, /* RF7_S_MONSTERS*/
	{ 0,   0,   0,   0,	0,   0,	   0	  ,  100}, /* RF7_XXX3	    */
	{ 0,   0,   0,   0,	0,   0,	   0	  ,  100}, /* RF7_XXX4	    */
	{ 0,   0,   0,   0,	0,   0,	   0	  ,  100}, /* RF7_XXX5	    */
	{ 0,   15,  0,   0,	0,   0,	   0	  ,  100}, /* RF7_S_ANT	    */
	{ 0,   15,  0,   0,	0,   0,	   0	  ,  100}, /* RF7_S_SPIDER  */
	{ 0,   15,  0,   0,	0,   0,	   0	  ,  100}, /* RF7_S_HOUND   */
	{ 0,   15,  0,   0,	0,   0,	   0	  ,  100}, /* RF7_S_ANIMAL  */
	{ 0,   15,  0,   0,	0,   0,	   0	  ,  100}, /* RF7_RF7XXX7    */
	{ 0,   0,   0,   0,	0,   0,	   0	  ,  100}, /* RF7_XXX7	    */
	{ 0,   15,  0,   0,	0,   0,	   0	  ,  100}, /* RF7_S_THIEF   */
	{ 0,   15,  0,   0,	0,   0,	   0	  ,  100}, /* RF7_S_BERTBILLTOM*/
	{ 0,   0,   0,   0,	0,   0,	   0	  ,  100}, /* RF7_XXX8	    */
	{ 0,   15,  0,   0,	0,   0,	   0	  ,  100}, /* RF7_S_AINU    */
	{ 0,   0,   0,   0,	0,   0,	   0	  ,  100}, /* RF7_XX10	    */
	{ 0,   0,   0,   0,	0,   0,	   0	  ,  100}, /* RF7_XX11	    */
	{ 0,   15,  0,   0,	0,   0,	   0	  ,  100}, /* RF7_S_DRAGON  */
	{ 0,   17,  0,   0,	0,   0,	   0	  ,  100}, /* RF7_S_HI_DRAGON*/
	{ 0,   0,   0,   0,	0,   0,	   0	  ,  100}, /* RF7_XX12	    */
	{ 0,   0,   0,   0,	0,   0,	   0	  ,  100}, /* RF7_XX13	    */
	{ 0,   15,  0,   0,	0,   0,	   0	  ,  100}, /* RF7_S_DEMON   */
	{ 0,   17,  0,   0,	0,   0,	   0	  ,  100}, /* RF7_S_HI_DEMON*/
	{ 0,   0,   0,   0,	0,   0,	   0	  ,  100}, /* RF7_XX14	    */
	{ 0,   0,   0,   0,	0,   0,	   0	  ,  100}, /* RF7_XX15	    */
	{ 0,   15,  0,   0,	0,   0,	   0	  ,  100}, /* RF7_S_UNDEAD  */
	{ 0,   17,  0,   0,	0,   0,	   0	  ,  100}, /* RF7_S_HI_UNDEAD*/
	{ 0,   18,  0,   0,	0,   0,	   0	  ,  100}, /* RF7_S_WRAITH  */
	{ 0,   18,  0,   0,	0,   0,	   0	  ,  100}  /* RF7_S_UNIQUE  */
};

/*
 * Optimal Ranges for various spells.
 * 6 is optimal for Breath Weapons, Beams, and Arcs.
 * 3 is hard maximum for Lash/Spit.
 * 0 indicates no range limitation for other spells.
 *
 * This range is considered a preference if d_range in spell_desire is > 0.
 * It is a hard limit if d_range = 0.
 */
byte spell_range_RF4[32] =
{
	0,3,0,0,0,0,0,0,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,0,0,0
};

byte spell_range_RF5[32] =
{
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

byte spell_range_RF6[32] =
{
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

byte spell_range_RF7[32] =
{
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

