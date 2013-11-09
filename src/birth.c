/* Purpose: create a player character */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"

/*
 * Forward declare
 */
typedef struct hist_type hist_type;
typedef struct birth_menu birth_menu;

/*
 * Player background information
 */
struct hist_type
{
	cptr info;			    /* Textual History */

	byte roll;			    /* Frequency of this entry */
	byte chart;			    /* Chart index */
	byte next;			    /* Next chart index */
	byte bonus;			    /* Social Class Bonus + 50 */
};

/*
 * A structure to hold the menus
 */
struct birth_menu
{
	cptr name;
	int real;
};


/*
 * Background information (see below)
 *
 * XXX XXX XXX This table *must* be correct or drastic errors may occur!
 */
static hist_type bg[] =
{
	/* Human*/
	{"���ʤ��ϥ����륹���ͤȥ��륬������ͤ�", 10, 1, 9,  65 },
	{"���ʤ��ϥ����륹���ͤȥХ����ͤ�",     15, 1, 9,  80 },
	{"���ʤ��ϥ��륬������ͤȥХ����ͤ�",   25, 1, 9,  90 },
	{"���ʤ��ϥ����륹���ͤ�",                 45, 1, 2,  70 },
	{"���ʤ��ϥ��륬������ͤ�",               75, 1, 2,  95 },
	{"���ʤ��ϥХ����ͤ�",                   93, 1, 2, 120 },
	{"���ʤ��ϥ��Υӥ��ͤ�",                   96, 1, 2, 130 },
	{"���ʤ��ϥ��ǥ����ͤ�",                 99, 1, 2, 130 },
	{"���ʤ��ϥɥ륬�륢���������˻��ޤ����Ҷ��Τ����ΰ�ͤǡ������夹��Τ�Ҥ˽Ф��졢", 100, 1, 10, 90 },

	{"�֤����ޤ줿",  20, 9, 11, 50 },
	{"�֤����ޤ줿", 100, 9,  2, 50 },

	{"�������ǡ�",                    20, 2,  3, 35 },
	{"���ͤ��λҶ��Τ����ΰ�ͤǡ�",  70, 2,  3, 45 },
	{"���Ҥǡ�",                      84, 2,  3, 50 },
	{"Ĺ�Ҥǡ�",                      98, 2,  3, 55 },
	{"�Ҷ��ǡ������ξ�Ƥ�˴������",  99, 2, 10, 20 },
	{"˾�ޤ�̻Ҥǡ��Ƥ˼ΤƤ���", 100, 2, 10, 15 },

	{"�Ҷ��ǡ������ξ�Ƥ�˴������",  45, 11, 10, 20 },
	{"�������ǡ��Ƥ˼ΤƤ���",      70, 11, 10, 10 },
	{"˾�ޤ�̻Ҥǡ��Ƥ˼ΤƤ���", 100, 11, 10, 15 },

	{"��̱����ʡ����ߤǰ���ޤ�����",            50, 10, 4,  50 },
	{"����ǰ���ޤ�����",                        90, 10, 4,  70 },
	{"�����̾���ƻֲȤ��ܻҤȤ��ư���ޤ�����", 100, 10, 4, 100 },
	{"��²�������Ʊ���˰����Ƥ��ޤ���",        15,  3, 4,  40 },
	{"��²�������ܤǸ����Ƥ��ޤ���",          20,  3, 4,  35 },
	{"��²�������ڤˤ���Ƥ��ޤ���",              80,  3, 4,  55 },
	{"��²�δ��Ԥ��Ȥ˼����Ƥ��ޤ���",         100,  3, 4,  60 },

	{"���ʤ���̲������",        10, 4, 5, 50 },
	{"���ʤ����ܤĤ��ΰ���",    20, 4, 5, 45 },
	{"���ʤ��Ͽͤ��ɤ�������",  30, 4, 5, 55 },
	{"���ʤ��ϰŤ�����",        40, 4, 5, 50 },
	{"���ʤ����ä�����",        50, 4, 5, 50 },
	{"���ʤ��Ϥ������",        60, 4, 5, 50 },
	{"���ʤ���������",          70, 4, 5, 55 },
	{"���ʤ��Ϸ����ä�",        80, 4, 5, 45 },
	{"���ʤ����ۤȤ���",        90, 4, 5, 55 },
	{"���ʤ��Ϥ��餭�鵱��",   100, 4, 5, 55 },

	{"�Ĥ�Ʒ�ȡ�",    20, 5, 6, 50 },
	{"����Ʒ�ȡ�",    50, 5, 6, 50 },
	{"�⿧��Ʒ�ȡ�",  55, 5, 6, 50 },
	{"���Ȥ�Ʒ�ȡ�",  60, 5, 6, 50 },
	{"�㿧��Ʒ�ȡ�",  90, 5, 6, 50 },
	{"�Ф�Ʒ�ȡ�",    95, 5, 6, 50 },
	{"������Ʒ�ȡ�", 100, 5, 6, 50 },

	{"����ľ����",      70, 6, 7, 50 },
	{"��뤯���Ǥä�",  90, 6, 7, 50 },
	{"���Ĥ�������",   100, 6, 7, 50 },

	{"����ȱ��",    30, 7, 8, 50 },
	{"������ȱ��",  40, 7, 8, 50 },
	{"�㿧��ȱ��",  60, 7, 8, 50 },
	{"�֤�ȱ��",    75, 7, 8, 50 },
	{"�⿧��ȱ��",  90, 7, 8, 50 },
	{"��ȱ��",   100, 7, 8, 50 },

	{"������ȩ�򤷤Ƥ��ޤ���",              10, 8, 0, 50 },
	{"����ȩ�򤷤Ƥ��ޤ���",                20, 8, 0, 50 },
	{"���̤�ȩ�򤷤Ƥ��ޤ���",              70, 8, 0, 50 },
	{"��ȩ�򤷤Ƥ��ޤ���",                90, 8, 0, 50 },
	{"Ʃ���̤�褦����ȩ�򤷤Ƥ��ޤ���", 100, 8, 0, 50 },

	/* Hawkman */
	{"���ʤ��ϥۡ����ޥ��", 100, 12, 13, 50 },

	{"��Ĺ�ҤǤ���",                      30, 14, 15, 55 },
	{"�����ҤǤ���",                      50, 14, 15, 50 },
	{"���ܻҤǤ���",                      55, 14, 15, 50 },
	{"�θɻ��Ǥ���",                      60, 14, 15, 45 },
	{"�δ��ͤ��λҶ��Τ����ΰ�ͤǤ���",  85, 14, 15, 50 },
	{"�ΰ�γ��Ǥ���",                   100, 14, 15, 55 },

	{"��",    10, 13, 14,  20 },
	{"��±",    21, 13, 14,  30 },
	{"����",    26, 13, 14,  45 },
	{"��ʼ",    42, 13, 14,  45 },
	{"���",    73, 13, 14,  50 },
	{"����",    78, 13, 14,  50 },
	{"���결",  85, 13, 14,  55 },
	{"���۲�",  89, 13, 14,  60 },
	{"��η",    94, 13, 14,  65 },
	{"��ѻ�",  97, 13, 14,  70 },
	{"�ؼ�",    99, 13, 14,  80 },
	{"��²",   100, 13, 14, 100 },

	{"���ʤ���", 100, 15, 16, 50 },

	{"�Ϲ����������ȩ�������Ƴ�����ȱ�򤷤Ƥ��ޤ���",    11, 16, 0, 50 },
	{"�֥�󥺿������ȩ��������Ƽ����ȱ�򤷤Ƥ��ޤ���",  16, 16, 0, 50 },
	{"������������������ȩ�򤷤Ƥ��ޤ���",            24, 16, 0, 50 },
	{"������������ȩ�򤷤Ƥ��ޤ���",                26, 16, 0, 60 },
	{"�Ĥ����ȩ�������ƿ忧��ȱ�򤷤Ƥ��ޤ���",          32, 16, 0, 50 },
	{"��������������ȩ�������Ǥ���",                    33, 16, 0, 70 },
	{"�㿧�����������㿧��ȩ�򤷤Ƥ��ޤ���",            37, 16, 0, 45 },
	{"�������ȩ����������ȱ�򤷤Ƥ��ޤ���",            41, 16, 0, 50 },
	{"���翧�����ȩ����������ȱ�򤷤Ƥ��ޤ���",        48, 16, 0, 50 },
	{"�п������ȩ�������Ʋ�����ȱ�򤷤Ƥ��ޤ���",        65, 16, 0, 50 },
	{"�п������������п���ȩ�򤷤Ƥ��ޤ���",            75, 16, 0, 50 },
	{"�֤����������֤�ȩ�򤷤Ƥ��ޤ���",                88, 16, 0, 50 },
	{"����������������ȩ�򤷤Ƥ��ޤ���",                94, 16, 0, 50 },
	{"����᤯����������°Ū��ȩ�򤷤Ƥ��ޤ���",       100, 16, 0, 55 },

	/* Lizardman */
	{"���ʤ�������Υꥶ���ɥޥ��",      10, 17, 18,  20 },
	{"���ʤ���ϫƯ�ԤΥꥶ���ɥޥ��",    35, 17, 18,  40 },
	{"���ʤ��Ͽ��դ˽���ꥶ���ɥޥ��",  70, 17, 18,  50 },
	{"���ʤ��Ͽ�����餹�ꥶ���ɥޥ��",  82, 17, 18,  50 },
	{"���ʤ���͵ʡ�ʥꥶ���ɥޥ��",      87, 17, 18,  80 },
	{"���ʤ��ϵ�²�Υꥶ���ɥޥ��",      92, 17, 18,  95 },
	{"���ʤ��������ԤΥꥶ���ɥޥ��",    97, 17, 18,  90 },
	{"���ʤ��ϥꥶ���ɥޥ�β���",       100, 17, 18, 110 },

	{"���ͤ��λҶ��Τ����ΰ�ͤǡ�",  60, 18, 19, 50 },
	{"��γ��ǡ�",                   100, 18, 19, 55 },

	{"���ʤ��ϰ�ȤΤ���ʪ�Ǥ���",          20, 19, 20, 20 },
	{"���ʤ��ϰ�Ȥθؤ�Ǥ���",            80, 19, 20, 55 },
	{"���ʤ��ϲ�²�����ڤˤ���Ƥ��ޤ���", 100, 19, 20, 60 },

	{"���ʤ���", 100, 20, 21, 50 },

	{"�п����ڤ�",    55, 21, 22, 50 },
	{"�Ĥ��ڤ�",      68, 21, 22, 50 },
	{"�ų쿧���ڤ�",  78, 21, 22, 50 },
	{"���������ڤ�",  84, 21, 22, 50 },
	{"�����ڤ�",    87, 21, 22, 60 },
	{"�⿧���ڤ�",    89, 21, 22, 55 },
	{"���ڤ�",      99, 21, 22, 40 },
	{"�ԥ󥯤��ڤ�", 100, 21, 22, 10 },

	{"��ʢ�������",      50, 22, 23, 50 },
	{"�п���ʢ�������",    75, 22, 23, 50 },
	{"������ʢ�������",    90, 22, 23, 50 },
	{"�ݲ翧��ʢ�������", 100, 22, 23, 60 },

	{"�Ĥ�����Ʒ�򤷤Ƥ��ޤ���",    25, 23, 0, 50 },
	{"�֤�����Ʒ�򤷤Ƥ��ޤ���",    50, 23, 0, 50 },
	{"�����ʹ���Ʒ�򤷤Ƥ��ޤ���",  75, 23, 0, 50 },
	{"��Ƽ����Ʒ�򤷤Ƥ��ޤ���",   100, 23, 0, 50 },

	/* Fairy */
	{"���ʤ���ξ�Ƥ�", 100, 24, 25, 50 },

	{"�ԥ������Ǥ�����",    20, 25, 26, 35 },
	{"�˥������Ǥ�����",    30, 25, 26, 25 },
	{"���������Ǥ�����",    75, 25, 26, 50 },
	{"��������Ǥ�����",    90, 25, 26, 75 },
	{"�����ε�²�Ǥ�����", 100, 25, 26, 85 },

	{"���ʤ�������˥饤�ȥ��꡼��α����������Ƥ��ơ�",  30, 26, 27, 50 },
	{"���ʤ�������˥饤�ȥ֥롼�α����������Ƥ��ơ�",   100, 26, 27, 50 },

	{"�ʤ�餫�ʶ�ȱ��",        50, 27, 28, 50},
	{"���Ǥä���ȱ��",          80, 27, 28, 50},
	{"�ʤ�餫�ʿ��п���ȱ��",  90, 27, 28, 50},
	{"���Ǥä����п���ȱ��",   100, 27, 28, 50},

	{"�Ĥ�Ʒ���������������������Ȥ���ȩ�򤷤Ƥ��ޤ���",        70, 28, 0, 50},
	{"���꡼���Ʒ���������������������Ȥ���ȩ�򤷤Ƥ��ޤ���", 100, 28, 0, 50},

	/* Gremlin */
	{"���ʤ�", 100, 29, 30, 50 },

	{"�ϸ����⥰�磻�Ȥ����Ի׵Ĥ�ưʪ�Ǥ��������򤫤ʻ����礬",  20, 30, 31, 65 },
	{"�������",                                                  70, 30, 34, 55 },
	{"�ϥ������ʬ���������ƤǤ����������Ǥ���",         100, 30, 33, 40 },

	{"���Ǩ�餷������",                    40, 31, 32, 50 },
	{"���ۤθ������ƤƤ��ޤ�",              80, 31, 32, 50 },
	{"�����12�������˿�����Ϳ����������", 100, 31, 32, 50 },

	{"���������Ѳ����ޤ�����", 100, 32, 33, 50 },

	{"��������ʤ��Ϲ���������ʪ�Ǥ�����",  30, 34, 33, 20 },
	{"���֥��Ǥ�����",                      60, 34, 33, 45 },
	{"���鰭��Ǥ�����",                      70, 34, 33, 55 },
	{"��鰭��Ǥ�����",                      90, 34, 33, 75 },
	{"�Ⲧ�Ǥ�����",                         100, 34, 33, 99 },

	{"���ʤ����֤�ȩ��",    10, 33, 35, 45 },
	{"���ʤ����㿧��ȩ��",  40, 33, 35, 50 },
	{"���ʤ��ϼ�����ȩ��",  60, 33, 35, 55 },
	{"���ʤ��Ϲ���ȩ��",   100, 33, 35, 50 },

	{"�֤�ǳ����Ʒ�򤷤Ƥ��ơ����ޤȲ�Ȼɤ��㤬�����Ƥ��ޤ���",  40, 35, 0, 55 },
	{"�֤�ǳ����Ʒ�򤷤Ƥ��ơ����ޤȲ���㤬�����Ƥ��ޤ���",      70, 35, 0, 50 },
	{"�֤�ǳ����Ʒ�򤷤Ƥ��ơ����ޤ��㤬�����Ƥ��ޤ���",         100, 35, 0, 45 },

	/* Skeleton */
	{"���ʤ���", 100, 36, 37, 50 },

	{"����ѻΤˤ����Ф���ޤ�����",      30, 37, 38, 50 },
	{"��ˡ�μ¸��ˤ����Ф���ޤ�����",    50, 37, 38, 50 },
	{"�ٰ�����η�ˤ����Ф���ޤ�����",    70, 37, 38, 50 },
	{"����Ȥη���ˤ�����߽Ф���ޤ�����",  75, 37, 38, 50 },
	{"��������ޤ�ޤ�����",                85, 37, 38, 50 },
	{"�����������ޤ�ޤ�����",                95, 37, 38, 30 },
	{"��̾���Ѥˤ�����߽Ф���ޤ�����",     100, 37, 38, 50 },

	{"���ʤ���", 100, 38, 39, 50 },

	{"�Ť����줿���ǽ���Ƥ��ơ�",        40, 39, 40, 50 },
	{"��ä��������ǽ���Ƥ��ơ�",        60, 39, 40, 50 },
	{"�������줿�㿧�����ǽ���Ƥ��ơ�",  80, 39, 40, 50 },
	{"�򤯵������ǽ���Ƥ��ơ�",         100, 39, 40, 50 },

	{"�����ܤ򤷤Ƥ��ޤ���",                    30, 40, 0, 50 },
	{"�Ϲ��ι�Ф�ǳ���������ܤ򤷤Ƥ��ޤ���",  50, 40, 0, 50 },
	{"���ݤϤ���äݤǤ���",                   100, 40, 0, 50 },

	/* Ghost */
	{"���ʤ���", 100, 41, 42, 50 },

	{"�ŻҥͥåȤγ����鼫��ȯ������������̿�ΤǤ������ʤ���̵���ο͡��ξ�ǰ�ˤ���������ޤ��������ʤ����ԲĻ��¸�ߤǤ��ꡢ̵�����ŻҤ�ʤ���Ƥ��ޤ���", 1, 42, 0, 50 },
	{"����ѻΤˤ����Ф���ޤ�����",      30, 42, 43, 50 },
	{"��ˡ�μ¸��ˤ����Ф���ޤ�����",    50, 42, 43, 50 },
	{"�ٰ�����η�ˤ����Ф���ޤ�����",    70, 42, 43, 50 },
	{"����Ȥη���ˤ�����߽Ф���ޤ�����",  75, 42, 43, 50 },
	{"��������ޤ�ޤ�����",                85, 42, 43, 50 },
	{"�����������ޤ�ޤ�����",                95, 42, 43, 30 },
	{"��̾���Ѥˤ�����߽Ф���ޤ�����",     100, 42, 43, 50 },

	{"���ʤ���", 100, 43, 44, 50 },

	{"������ȱ��",              25, 44, 45, 50 },
	{"��Ĥ줿�֥饦���ȱ��",  50, 44, 45, 50 },
	{"��ȱ��",                75, 44, 45, 50 },
	{"ȱ�Τʤ�Ƭ��",           100, 44, 45, 50 },

	{"ǳ������ú�Τ褦��Ʒ��",  25, 45, 46, 50 },
	{"Ʒ�Τʤ��ܡ�",            50, 45, 46, 50 },
	{"��˽�ʲ�����Ʒ��",        75, 45, 46, 50 },
	{"�����ä��֤�Ʒ��",       100, 45, 46, 50 },

	{"�����ƻ�ͤΤ褦���ڿ���ȩ�򤷤Ƥ��ޤ���", 100, 46, 47, 50 },

	{"���ʤ��μ���ˤ��Ե�̣���п��Υ����餬������äƤ��ޤ���", 100, 47, 0, 50 },

	/* Pumpkin-Head */
	{"���ʤ���", 100, 48, 49, 50 },
	
	{"����ǥ֤ͥ��᤯��Ȫ�������Ǥ���",          50, 49, 50, 45 },
	{"����ǥ֤ͥ��ץ���ȥ��ȤλԾ�ǹ�������",  55, 49, 50, 50 },
	{"����ǥ֤ͥ�������إ륹�λԾ�ǹ�������",  60, 49, 50, 50 },
	{"����ǥ֤ͥ��Х른�ӥ��λԾ�ǹ�������",      65, 49, 50, 50 },
	{"����ǥ֤ͥ����꥿�ˤλԾ�ǹ�������",        70, 49, 50, 50 },
	{"����ǥ֤ͥ����ԥϥ���λԾ�ǹ�������",      75, 49, 50, 50 },
	{"����ǥ֤ͥ������ꥫ�λԾ�ǹ�������",      85, 49, 50, 50 },
	{"����ǥ֤ͥ���Į������ȥ�λԾ�ǹ�������",  90, 49, 50, 50 },
	{"����ǥ֤ͥ���Į���ꥢ�ƤλԾ�ǹ�������",    95, 49, 50, 50 },
	{"����ǥ֤ͥ���Ԥε��¤ǽ��ä����饹��",     100, 49, 50, 60 },

	{"���ܥ��㤫����줿�͹���̿�ΤǤ���", 100, 50, 51, 50 },

	{"���ʤ���", 100, 51, 52, 50 },

	{"�������������в�����ȩ��",    30, 52, 53, 50 },
	{"���若�路�����п���ȩ��",    50, 52, 53, 50 },
	{"���������������㿧��ȩ��",    70, 52, 53, 50 },
	{"���若�路����󥬿���ȩ��", 100, 52, 53, 50 },

	{"�ʥ��դǤ���̤��줿�ܡ��ȤäƤĤ����褦�ʼ�­�򤷤Ƥ��ޤ���",  70, 53, 0, 50 },
	{"�ޥ��å��ǽ񤫤줿�ܡ��ȤäƤĤ����褦�ʼ�­�򤷤Ƥ��ޤ���",   100, 53, 0, 50 },

	/* Goblin */
	{"���ʤ��ϰ��������Υ��֥��",  40, 54, 55,  50 },
	{"���ʤ��ϥ��֥��",            75, 54, 55,  55 },
	{"���ʤ��ϥۥ֥��֥��",        95, 54, 55,  65 },
	{"���ʤ��ϸ��奪����������",   100, 54, 55, 100 },

	{"�β��ͤ��λҶ��Τ����ΰ�ͤǤ��� ", 100, 55, 56, 50 },

	{"���ʤ����Ĥ�Ʒ��",        25, 56, 57, 50 },
	{"���ʤ��ϸ���Ʒ��",        50, 56, 57, 50 },
	{"���ʤ��Ͼ����ʹ���Ʒ��",  75, 56, 57, 50 },
	{"���ʤ��Ϲ�������Ʒ��",   100, 56, 57, 50 },

	{"ȱ�Τʤ�Ƭ�˾�������ä��ѡ�",          20, 57, 58, 50 },
	{"����û��ȱ�Ⱦ�������ä��ѡ�",          40, 57, 58, 50 },
	{"����Ĺ��ȱ�Ⱦ�������ä��ѡ�",          60, 57, 58, 50 },
	{"ǳ����褦���֤�ȱ�Ⱦ�������ä��ѡ�",  96, 57, 58, 50 },
	{"���Τʤ���ȱ�Ⱦ�������ä��ѡ�",     100, 57, 58, 50 },

	{"�ڿ���",    25, 58, 59, 50},
	{"���㿧��",  70, 58, 59, 50},
	{"�Ĥ�",      85, 58, 59, 50},
	{"����",     100, 58, 59, 50},

	{"�֥ĥ֥Ĥ���ȩ�򤷤Ƥ��ޤ���",      33, 59, 0, 50},
	{"�����֥����餱��ȩ�򤷤Ƥ��ޤ���",  59, 59, 0, 50},
	{"����������ȩ�򤷤Ƥ��ޤ���",       100, 59, 0, 50},

	/* Gorgon */
	{"���ʤ���̾��ʤ����������",              90, 60, 61,  50 },
	{"���ʤ��ϥ�������λ����她�ƥ�Τ�",      94, 60, 61, 100 },
	{"���ʤ��ϥ�������λ����奨����奢���",  98, 60, 61, 110 },
	{"���ʤ��ϥ�������λ������ǥ塼����",   100, 60, 61, 120 },

	{"����ΰ�ͤǤ���", 100, 61, 62, 50 },

	{"���ʤ��ϸ���Ԥ��Ф��Ѥ���ٰ���Ʒ�ȡ�", 100, 62, 63, 50 },

	{"Ƭȱ�Τ���������12ɤ��",  60, 63, 64, 50 },
	{"Ƭȱ�Τ���������24ɤ��",  91, 63, 64, 50 },
	{"Ƭȱ�Τ���������36ɤ��", 100, 63, 64, 50 },

	{"�ФΤޤ������ͤμؤ�����",    47, 64, 65, 50 },
	{"�ĤΤޤ������ͤμؤ�����",    75, 64, 65, 50 },
	{"�֤Τޤ������ͤμؤ�����",    86, 64, 65, 50 },
	{"�����Τޤ������ͤμؤ�����",  94, 64, 65, 50 },
	{"�⿧�˵����ؤ�����",         100, 64, 65, 55 },

	{"����ϼؤ��Τ򤷤Ƥ��ޤ���", 100, 65,  0, 50 },

	/* Mermaid */
	{"���ʤ��Ͽ�β���Ĺ�ҤǤ���",                                     5, 66, 67, 140 },
	{"���ʤ���ξ�Ƥϥޡ��ᥤ�ɤε�²�Ǥ���",                          20, 66, 67, 120 },
	{"���ʤ���ξ�Ƥ�ʿ�¤򰦤��뼫ͳ�����ʥޡ��ᥤ�ɤǤ�����",        80, 66, 67,  55 },
	{"���ʤ��϶ؤ���줿���ˤ�äƻ��ޤ줿�ޡ��ᥤ�ɤλ������Ǥ���", 100, 66, 67,  50 },

	{"���ʤ��ϥѡ��륰�쥤�����Ǥä�������ȱ�������",  25, 67, 68, 55 },
	{"���ʤ������п��Τޤä�����ȱ�������",            75, 67, 68, 50 },
	{"���ʤ��Ϥ��餵�餷����������ȱ�ǡ�",             100, 67, 68, 55 },

	{"��Ⱦ�ȤϿ忧���ڤ�ʤ��줿�������򤷤Ƥ��ޤ���" ,   25, 68, 0, 50 },
	{"��Ⱦ�ȤϿ��п����ڤ�ʤ��줿�������򤷤Ƥ��ޤ���",  75, 68, 0, 50 },
	{"��Ⱦ�Ȥ϶⿧���ڤ�ʤ��줿�������򤷤Ƥ��ޤ���",   100, 68, 0, 60 },
};

static cptr race_jouhou[MAX_RACES] =
{
"�ʹ֤ϴ��ܤȤʤ륭��饯���Ǥ���¾�����Ƥμ�²�Ͽʹ֤���Ӥ���ޤ����ʹ֤Ϥɤ�ʥ��饹�˽������Ȥ�Ǥ����ɤΥ��饹�Ǥ�ʿ��Ū�ˤ��ʤ��ޤ����ʹ֤ϼ�̿��û�����ᡢ��٥�徺��¾�Τɤ�ʼ�²�����᤯�ʤ뷹��������ޤ����ޤ������̤ʽ����������ϻ��äƤ��ޤ���",
"�ۡ����ޥ�ϵ������Ӥ�����Ʈ�򹥤�ͭ��μ�²�Ǥ�������Ĺ̿�ǿͤ�3�ܤμ�̿��ؤ�ޤ����ޤ����Ȥ����Ȥ���Ȥ��������Ϸ����򤱤뤳�Ȥ��Ǥ��ޤ���",
"�ꥶ���ɥޥ�ϹŤ���������Ŀ�Φξ�Ѥμ�²�Ǥ������Υ����ˤ������䵤������������ޤ����ޤ�������Ʈ��ͥ��Ƥ��ꡢHP�ξ徺Ψ���⤯��櫤β������ƻ����Ѥ⤽��ʤ�ˤ��ʤ��ޤ���",
"�ե����꡼�����˾������Ǥ������Ͼ�������������櫤�����Ϸ������ӱۤ��뤳�Ȥ��Ǥ��ޤ����������������ѹ��ߡ������Ф�����������äƤ��ޤ������ŰǤˤ�ä��礭�ʥ��᡼��������Ƥ��ޤ��ޤ������٥�ǤϤ��®�����֤��Ȥ��Ǥ���褦�ˤʤ�ޤ���",
"�������ϥե����꡼���жˤǤ�����������櫤�����Ϸ������ӱۤ��뤳�Ȥ��Ǥ��ޤ������ϰŰǤ����ѹ��ߡ��Ź����Ϲ����Ф�����������äƤ��ޤ��������ˤ�ä��礭�ʥ��᡼��������Ƥ��ޤ��ޤ������٥�ǤϤ��®�����֤��Ȥ��Ǥ���褦�ˤʤ�ޤ���",
"����ǥåɤǤ��륹����ȥ�ϡ��Ǥ���̿�ϵۼ�������ۤ���ɬ�פϤ���ޤ�������ʪ�Τ��γФ���Τ˴�����Ѥ��Ƥ��ʤ����ᡢ�����ʤ�ʪ���٤���ޤ������ι��ϤȤ��ä����ҤΤ褦�ʤ�Τ��������������٥뤬�夬����䵤���Ф���������������ޤ������俩ʪ�λ��ĸ��̤ϥ�����ȥ�ΰߡ�¸�ߤ��ޤ��󤬡ˤ��̲᤹�뤳�Ȥʤ������Ϥ�ȯ�����ޤ��������俩ʪ���Τ���γܤ��̤�ȴ��������Ƥ��ޤ������ܤ�ۼ����뤳�ȤϤǤ��ޤ��󡣥�����ȥ�Ͽ�����ɬ�פȤ��ޤ���",
"�������Ȥ϶��Ϥʥ���ǥåɤΰ��Ǥ���Ⱦʪ��Ū��¸�ߤǤ������ϡ��ɤ��̤�ȴ���뤳�Ȥ��Ǥ��ޤ��������ΤȤ��ˤ��ɤ�̩�٤ˤ�äƽ��Ĥ����Ƥ��ޤ��ޤ���¾�Υ���ǥå�Ʊ�͡�������̿�Ϥ�ۼ�����뤳�Ȥ��ʤ��������ʤ���Τ򸫤뤳�Ȥ��Ǥ����Ǥ��䵤���Ф���������������������Ϲ����Ф����ȱ֤���äƤ��ޤ�����٥뤬��ʬ�˾夬������ϥƥ�ѥ�����������ޤ����������Ȥ���ۤ�����ˡ�Ȥ��ˤʤ뤳�Ȥ��Ǥ��ޤ��������ο���Ū�����������ϼ�Ǥ����������ȤϿ�����ɬ�פȤ��ޤ���",
"�ѥ�ץ���إåɤ�����ǥ֤ͥˤ�äƺ��줿���ܥ���Τ��Ф��Ǥ����ѥ�ץ���إåɤ�Ũ���ϵԿ��򤯤����뤿�ᡢŨ����ͥ��Ū�������ޤ����ѥ�ץ���إåɤ�����Ū���ϼ�Ǥ����������ȱ֤ȥ������������ؤ��������������̿�Ϥ�ݻ�������٥뤬�夬��Хƥ�ѥ��������ޤ���",
"���֥��Ϥ褤��Τˤʤ�ޤ�������ˡ�ϴ��ԤǤ��ޤ������ϱ�̩��ư���������ޤ�õ������, �γФ�Ҥɤ��Ǥ������֥��Ͻ�����Ź�Ǥ��㤤ʪ�ǤϤ��⤤��ۤ��׵ᤵ�줬���Ǥ��������ϲ��˽��ळ�Ȥ򹥤ि�ᡢ���֥��ϰŰǤ��Ф��������������Ƥ��ޤ���",
"���������Ƭȱ�������̵���ΥإӤ��������͵��ʤ�̥إӤβ������ĥ�󥹥����Ǥ��������Ϥ����륹����򤽤Ĥʤ����ʤ�����Ǥ�ݤ�¾��²�����ʤ�ͥ��Ƥ��ޤ����������в����������������٥뤬�夬��л����䵤���Ǥ��Ϲ��䥫���������������뤳�Ȥ��Ǥ��ޤ��������˲ä��ơ������Ԥ��Ф��Ѥ��Ƥ��ޤ��ȼٴ�ɤ�ԻȲ�ǽ�ˤʤ�Τǡ����˶��Ϥʼ�²�Ǥ���",
"�ޡ��ᥤ�ɤϾ�Ⱦ�Ȥ��ʹ֤ǲ�Ⱦ�Ȥ����Ρ����˽����²�Ǥ������Ͽ���򹥤ߡ������ǤϹ⤤��Ʈǽ�Ϥ���ԤǤ��ޤ���ȿ��Φ��϶��ǡ���ư�˼�֤�������ޤ����ޤ�����ͥ���ʲΤ���Ǥ��ꡢ��󥹥�����̥�Ǥ���Τ�Τ����Ȥ��Ǥ��ޤ���",
};

cptr class_jouhou[MAX_CLASS] =
{
"���른�㡼��������˥åȤδ��ܤȤʤ륯�饹�Ǥ������饹�Ȥ��Ƥ��ü쵻ǽ��ʤ����ѥ�᡼����ʿ��Ū�˾徺���뤿�ᡢ���른�㡼�Τޤް�Ƥ������Ⱦü�ʴ�����˳��˥åȤˤʤ�Ǥ��礦�������������ڥ���ꥹ�Ȥ˰�������������ζ����Ȥ�����Ū�Ǥ���Ф��ʤ�ͭ�Ѥʥ��饹�Ǥ���",
"�ʥ��Ȥ����ͥ�졢���Ť󤸤뵳��ƻ����ƻ�դǤ�����ˡ�ϻȤ��ޤ��󤬡����른�㡼�����ɸ��ͥ�졢����ࡢ�Ⱦ��������դȤ��ޤ������Ϥ⤽���������ʤ����ä˾��Ϥ��ƥ�󥹤򿶤뤦�ݤι����Ϥ��˲�Ū�Ǥ���",
"�С����������϶��٤����Τȿ��¤ߤϤ��줿�ѥ�Ƿ�ϩ���ڤ곫����˽����ΤǤ�������ߴ�����դȤ��ޤ�����ˡ�ϻȤ�������ƻ����ѤⲼ��Ǥ����������ä�������ϱԤ���������γС�õ��ǽ�Ϥ�ͥ��ޤ���",
"�ƥ顼�ʥ��ȤϺ��ޤǻ����Ƥ����Ԥ�����������դ��˰���Ϣ��붲�ݤε��ΤǤ������λ�٤ǽ�ϤϿ���Ū�Ǥ��餢��ޤ����Ǥ�����ˡŪ��ǽ�Ϥϳ�̵�ȸ��äƤ����Ǥ��礦��",
"�ӡ����ȥƥ��ޡ�����äȤȤ���臘��ΤǤ���������������Ȥ����ڥåȤ�ٱ礹�붦����ˡ��Ȥ����Ȥ��Ǥ��ޤ������Ϥ����դǡ����Ϥ˸����ʤ������������Ƥ�ڥʥ�ƥ������ʤ��Ȥ������åȤ�����ޤ�����ˡ�򾧤���Τ�ɬ�פ�ǽ�Ϥϸ����Ǥ���",
"�����ɥޥ������Ϸ�����ˤ�뤳�ȤǸʤκ����᤭���Ȥ���ۤ���Ȥ����ƻ�ԤǤ������Ф餷�����������������ή�����դǡ����ļ�°������ˡ��Ȥ����Ȥ��Ǥ��ޤ�����ˡ�򾧤���Τ�ɬ�פ�ǽ�Ϥ���ǽ�Ǥ���",
"�����������Ȥ����ɥ饴����Ф��ư���Ū�ʻ����Ϥ�ؤ륯�饹�Ǥ����ޤ���ñ����ˡ�Ǥ���л��ѤǤ��ޤ�����ˡ�򾧤����ɬ�פʤΤ���ǽ�Ǥ���",
"��̩���⤯��������Ĺ��������ή�򤹤뤳�Ȥ��Ǥ�����°������ˡ��Ȥ���ͥ���ʰǤ���ΤǤ�����ˡ�򾧤���Τ�ɬ�פ�ǽ�Ϥ���ǽ�Ǥ���",
"���������ɤ��������μ���˭�٤ʷи�����¿���ι�����ˡ��Ȥ����ʤ���ˡ�Ȥ��Ǥ�����ƻ����ˡ�˲ä��ƻ�°������ˡ���Ȥ��뤳�Ȥ��Ǥ��ޤ�����ˡ�򾧤���Τ�ɬ�פ�ǽ�Ϥ���ǽ�Ǥ���",
"��������å��ϳؼ�ȩ����ˡ�Ȥ��Ǥ��������å�����ˡ����ƻ����ˡ2����ε������ˡ��Ȥ����ʤ��������������Ƿ⹶���Ϥ����ꡢ�ޤ��������򾤴����뤳�Ȥ��Ǥ��ޤ�����ˡ�򾧤���Τ�ɬ�פ�ǽ�Ϥ���ǽ�Ǥ���",
"�����������Ȥϰ���䰭���༣�Υ��ڥ���ꥹ�ȤǤ����˼٤���ˡ���Ȥ��뤳�ȤǼٰ��ʰ���䥢��ǥåɤˤ�̵��ζ�����ȯ�����ޤ����ޤ��������������Ƿ��Ϥ⤢�ꡢ�Ƿ�ǳ�ϩ�򸫽Ф����Ȥ�Ǥ��ޤ�����ˡ�򾧤���Τ�ɬ�פ�ǽ�Ϥϸ����Ǥ���",
"���ޥ��ͥ��Ͻ����δ��ܥ��饹�Ǥ�����ˡ�����Ȥ��ޤ��󤬡�����ʳ��Τ��ȤϤ��Ĥʤ��ʤ����ʤ��ޤ���������������­�δ����ݤ�ʤ��Τǡ����ޥ��ͥ��Τޤ޺Ǹ�ޤ���Ĺ������Τ������ǤϤʤ��Ǥ��礦��",
"�����륭�꡼���䤬���դ���ˡ��ΤǤ����ޤ������Ϥ⤳�ʤ��ޤ����Ͼ夫�����ΰ����˲�Ū���Ϥ���Ĥ��ȤǤ��礦����ˡ�򾧤���Τ�ɬ�פ�ǽ�Ϥϸ����Ǥ���",
"�������㡼�ϵݷ���������ȤǤ�����ˡ�ϻȤ����Ȥ��Ǥ��ޤ��󤬡����μͷ�ǽ�Ϥ�����Ǥ���������Ũ��ͤ��������Ȥ��Ǥ���Ǥ��礦��",
"�ɥ饴��ƥ��ޡ��ϥɥ饴��ȶ��˼��������ä��臘��ˡ��ΤǤ�����������̤�ε�˹�����Ƥ��ơ����ĤǤ�ɥ饴����Ϥ�ڤ�뤳�Ȥ�����ޤ�����ˡ�򾧤���Τ�ɬ�פ�ǽ�Ϥϸ����Ǥ���",
"�����å��������ˡ����ƻ����Ѥ����դȤ�������Ǥ�����ˡ�ϥ����å�����ƻ������˰Ź���ˡ��1���ܤ��ԻȲ�ǽ�Ǥ�����ˡ�򾧤���Τ�ɬ�פ�ǽ�Ϥ���ǽ�Ǥ���",
"�����졼���¿���ι�����ˡ�ȹԻȤ�������������Ǥ������֤ϥ��������ɤȤۤȤ���Ѥ��ޤ�����ˡ�򾧤���Τ�ɬ�פ�ǽ�Ϥ���ǽ�Ǥ���",
"�����å��ϲ�����ˡ�����դȤ�����η�Ǥ�����������ˡ�ȼ�°������ˡ���ԻȤǤ��ޤ����ޤ���¾����ˡ�Ϥ���٤�ȵݤ�¿���Ȥ����Ȥ�����ޤ��������å��ϡ��ץ꡼���Ȥ˥����󥸤���ޤǤβ��Ѥߥ��饹���ȹͤ��Ƥ褤�Ǥ��礦����ˡ�򾧤���Τ�ɬ�פ�ǽ�Ϥϸ����Ǥ���",
"�ץ꡼���ȤϹ�����ˡ�Ȳ�����ˡ��ξ�����Ȥ�����η�Ǥ����������˼٤ȼ�°������ˡ���ԻȤǤ��ޤ��������å��ξ�饯�饹�ȹͤ��Ƥ褤�Ǥ��礦��������ˡ�ϥ����å�����٤�Ȥ������ޤ�����ˡ�򾧤���Τ�ɬ�פ�ǽ�Ϥϸ����Ǥ���",
"�ֻ�Ԥλ��ءפ������������������ɡ���������å��������������ȡ������졼�󡦥����å�����˴���ž������ž�����饹�Ǥ�����Ԥλ��ؤ��Ϥˤ�ä������뤲�����Ȥǡ�����˥ϥ���٥��ǽ�Ϥ�ȤˤĤ��Ƥ��ޤ��������ȶ������˼٤�ε����ʳ������Ƥ���ˡ�ΰ��Ȥ��ޤ�������Ϥ䥯�饹�����󥸤Ǥ��ޤ�����ˡ�򾧤���Τ�ɬ�פ�ǽ�Ϥ���ǽ�Ǥ�����å��Ͽ�����ɬ�פȤ��ޤ���",
"���󥸥���ʥ��ȤϿ�����������˥åȤ���˴���ž���Ǥ����礬����ž�����饹�Ǥ������ơ���ˡ���˹⤤��Ʈǽ�Ϥ�ؤ�ޤ�������Ϥ䥯�饹�����󥸤Ǥ��ޤ��󡣥��󥸥���ʥ��Ȥ��˼٤ȼ�°������ˡ��Ȥ����Ȥ��Ǥ��ޤ�����ˡ�򾧤���Τ�ɬ�פ�ǽ�Ϥϸ����Ǥ������󥸥���ʥ��ȤϿ�����ɬ�פȤ��ޤ���",
"�����ä��ϡ֥����å���Ʊ��������͡����ˤ��ʤ��Ǥ衣���������������ͤ�������ޡ���������ά�˿ͤ˲������䤤������Ȥ��Τ褦�ˤ���äȿ��夬���ȯ������Τ����ĤʤΡʤϤ��ȡˡפǤ������Υ��饹�ϥ��饹�����󥸤Ǥ��ʤ��ʤ�ޤ�����ˡ�򾧤���Τ�ɬ�פ�ǽ�Ϥ���ǽ�Ǥ���",
"��������Φ�Х�Х�������¤���줿�������붯�Ϥ����ؽơ٤򰷤���ͣ��Υ��饹�Ǥ�����ˡ�ʤɤ�Ķ�����ʳؤ��Ф��뿮����ΤƤ�������ˡ���Ф����ѵ��Ϥ��������㲼���Ƥ��ꡢ¾�Υ��饹�˥��饹�����󥸤Ǥ��ʤ��ʤ�ޤ���",
"�ƥ�ץ�ʥ��Ȥϥ����ꥢ����������������ΤǤ������������򰷤����Ȥ��Ǥ����Ź���ˡ�򾧤��뤳�Ȥ��Ǥ��ޤ����ޤ������Υ��饹�˽����Ƥ���֤�¾�Υ��饹�إ��饹�����󥸤Ǥ��ʤ��ʤ�ޤ�����ˡ�򾧤���Τ�ɬ�פ�ǽ�Ϥϸ����Ǥ���",
"�ۥ磻�ȥʥ��ȤϿ������Υӥ�������������������ΤǤ������������򰷤����Ȥ��Ǥ���������ˡ�򾧤��뤳�Ȥ��Ǥ��ޤ����ޤ������Υ��饹�˽����Ƥ���֤�¾�Υ��饹�إ��饹�����󥸤Ǥ��ʤ��ʤ�ޤ�����ˡ�򾧤���Τ�ɬ�פ�ǽ�Ϥϸ����Ǥ���",
"���ɤϥ�����ꥢ����θ�ѼԤȤ��ƥե��顼�϶��Ĥ�ǧ�᤿�ԤǤ�������ǽ�Ϥϥѥ�ǥ���򤷤Τ�����������«�ͤ��Ƴ�Ԥˤդ��路������ˡ�򾧤���Τ�ɬ�פ�ǽ�Ϥϸ����Ǥ���",
"�췳��Ψ����ǽ�Ϥȡ����λ�ʤ��̤ε��Ρ������˽��ǡ���®��Ķ������褫��׷��Ȥ����ġ�",
"��̩���⤯��������Ĺ��������ή�򤹤뤳�Ȥ��Ǥ�����°������ˡ��Ȥ���ͥ���ʰǤ���ΤǤ�����ˡ�򾧤���Τ�ɬ�פ�ǽ�Ϥ���ǽ�Ǥ���",
"˰���ʤ�õ������ˡ���������Ϥ��ˤ����ԡ���ˡ�򾧤���Τ�ɬ�פ�ǽ�Ϥ���ǽ�Ǥ���",
"�ե쥤��Ͽ��˻Ť������βø�����������Ρ����٤����Τ�������ʻ����������Ѥ���ˡ���Ȥ����臘����ˡ�򾧤���Τ�ɬ�פ�ǽ�Ϥϸ����Ǥ���",
"���쥻��Ȥ������ΰ����ˡ���Ȥ����Ʈ�ΤǤ������Ϥ�ȿ��ˡ�ե�����ɤ�Ϥ뤳�Ȥ��Ǥ��ޤ�����ˡ�򾧤���Τ�ɬ�פ�ǽ�Ϥ���ǽ�Ǥ���",
"������ѥ�������ΰǤ���ǿ����Ϥ�ȯ�������������ޤ������ˤ��ľ�ܹ���ȰŹ���ˡ���Ȥ����臘���Ȥ��Ǥ��ޤ�����ˡ��ɬ�פ�ǽ�Ϥ���ǽ�Ǥ���",
"����Ͽ��ˤ��οȤ������������Ǥ������Ϥ����ã������ľ��Ũ���臘���Ȥ϶��Ǥ������ݤ򹽤����򸦤����ޤ���������ϡ�Ũ��Ϥ������Ȥ�����ޤ��󡣤ޤ�����٥뤬�夬��ȡ��ڤ�ʤ��ݤ�Ũ��ʹӤ����ʤ��Ϥ���ˤ�������Ĥ��Ȥ��Ǥ���褦�ˤʤ�ޤ����ޤ�����ξ�ǿȤ�����뤳�Ȥˤ�ꡢ�ٰ����Ҥ��ʧ�����Ȥ�Ǥ��ޤ������˻Ť������ã�ϵ���򾧤��뤳�Ȥ��Ǥ��ޤ�������Ͽ��˽�ʡ���줿���ʳ���Ȥ��դ��뤳�Ȥ򹥤ޤ����ݤ�����Τ˼���ˤʤ�����Ĥ��Ȥ⹥�ߤޤ��󡣤�������������ȵ���ν��椬���Ƽ��Ԥ��䤹���ʤꡢ�ޤ�����⤦�ޤ���Ƥʤ��ʤäƤ��ޤ��ޤ��������ɬ�פ�ǽ�Ϥϴ��Ѥ��ȸ����Ǥ���",
};

static cptr realm_jouhou[MAX_REALM] =
{
"��ƻ�ϴ��Ρ����ꡢ�����ͥ�줿�ᥤ���Ϥδ��ܤȤʤ���ˡ�ΰ�Ǥ���",
"��Ϥ���ˡ�Ǥ�����Ϥι�����ˡ�����¤��Ƥ��ޤ����ޤ���¿������ˡ�ϲФΥ�����Ȥαƶ�������ޤ���",
"��Ϥ���ˡ�Ǥ�����Ϥι�����ˡ�����¤��Ƥ��ޤ����ޤ���¿������ˡ�Ͽ�Υ�����Ȥαƶ�������ޤ���",
"���ϷϤ���ˡ�Ǥ������ϷϤι�����ˡ�����¤��Ƥ��ޤ����ޤ���¿������ˡ�����ϤΥ�����Ȥαƶ�������ޤ���",
"���Ϥ���ˡ�Ǥ������Ϥι�����ˡ�����¤��Ƥ��ޤ����ޤ���¿������ˡ�����Υ�����Ȥαƶ�������ޤ���",
"�����ϲ���ǽ�Ϥ�ͥ�줿��ˡ�Ǥ������Ť��ɸ桢������ˡ��¿���ޤޤ�Ƥ��ޤ����������ʸ��鷺���˻��äƤ��ޤ����ä˹��٥�μ�ʸ�ˤϥ���ǥåɤ�Ф˵����Ϥ�����ȸ����Ƥ��ޤ���",
"����ѤǤ���Ź�����ˡ�ۤɼٰ��ʥ��ƥ��꡼�Ϥ���ޤ��󡣤����μ�ʸ�����Ū�ؤ֤Τ�����Ǥ��������٥�ˤʤ�ȽѼԤ���ʪ�ȥ���ǥåɤ�ͳ�����ǽ�Ϥ�Ϳ���ޤ�����ǰ�ʤ��Ȥˡ���äȤ⶯�Ϥʼ�ʸ�Ϥ��ο��ޤȤ��ƽѼԼ��Ȥη��ɬ�פȤ����Ӿ���ˤ��Ф��нѼԤ���Ĥ��ޤ���",
"�����ϡ��ڥåȤȽ����礦���Ȥ˽Ť����֤�����ˡ�ΰ�Ǥ����ڥåȤ���������ꡢ�ڥåȤ򾤴������ꤹ�뤳�Ȥ��Ǥ��ޤ���",
"�����å��������ˡ�ΰ�Ǥ���������ˡ�ϤۤȤ�ɤ���ޤ��󤬡���ʬ��ǽ�Ϥ򶯲������ꡢ������Ȥ˱ƶ���ڤܤ����ꤹ����ˡ��¿������ޤ����ޤ������٥�ˤʤ�ȼ�ʬ��Ũ�Υ�����Ȥ���Ū���ѹ��Ǥ�����ˡ�⤢��ޤ���",
"ε������ˡ�ϸ���ʸ���ˤ�곫ȯ���졢���ߤǤϼ����Ƥ��ޤä���ˡ�ΰ�Ǥ���������ˡ�����ޤ����˲��Ϥ������ޤ�����¾����ˡ�Ͽ������ʤ��褦�ʸ��̤�ȯư���ޤ���",
"�˼٤ϡ������פ���ˡ�Ǥ���ľ��Ũ����Ĥ�����ˡ��¿���ޤޤ졢�ä˼ٰ���Ũ���Ф����Ϥ϶���٤���Τ�����ޤ��������������ɤ�Ũ�ˤϤ��ޤ���̤�����ޤ���",
};


static void birth_quit(void)
{
	remove_loc();
	quit(NULL);
}


/*
 *  Show specific help file
 */
static void show_help(cptr helpfile)
{
	/* Save screen */
	screen_save();

	/* Peruse the help file */
	(void)show_file(TRUE, helpfile, NULL, 0, 0);

	/* Load screen */
	screen_load();
}


/*
 * Save the current data for later
 */
static void save_prev_data(birther *birther_ptr)
{
	int i;

	/* Save the data */
	birther_ptr->psex = p_ptr->psex;
	birther_ptr->prace = p_ptr->prace;
	birther_ptr->pclass = p_ptr->pclass;
	birther_ptr->pelem = p_ptr->pelem;
	birther_ptr->age = p_ptr->age;
	birther_ptr->ht = p_ptr->ht;
	birther_ptr->wt = p_ptr->wt;
	birther_ptr->sc = p_ptr->sc;

	for (i = 0; i <= MAX_GOLD; i++)
	{
		birther_ptr->au[i] = p_ptr->au[i];
	}

	/* Save the stats */
	for (i = 0; i < A_MAX; i++)
	{
		birther_ptr->stat_max[i] = p_ptr->stat_max[i];
	}

	/* Save the hp & mana */
	birther_ptr->race_hp_lv1 = p_ptr->race_hp[0];
	birther_ptr->race_sp_lv1 = p_ptr->race_sp[0];
	birther_ptr->class_hp_lv1 = p_ptr->class_hp[p_ptr->pclass][0];
	birther_ptr->class_sp_lv1 = p_ptr->class_sp[p_ptr->pclass][0];

	/* Save the history */
	for (i = 0; i < 4; i++)
	{
		strcpy(birther_ptr->history[i], p_ptr->history[i]);
	}
}


/*
 * Load the previous data
 */
static void load_prev_data(bool swap)
{
	int i;

	birther	temp;

	/*** Save the current data ***/
	if (swap) save_prev_data(&temp);


	/*** Load the previous data ***/

	/* Load the data */
	p_ptr->psex = previous_char.psex;
	p_ptr->prace = previous_char.prace;
	p_ptr->pclass = previous_char.pclass;
	p_ptr->pelem = previous_char.pelem;
	p_ptr->age = previous_char.age;
	p_ptr->ht = previous_char.ht;
	p_ptr->wt = previous_char.wt;
	p_ptr->sc = previous_char.sc;

	for (i = 0; i <= MAX_GOLD; i++)
	{
		p_ptr->au[i] = previous_char.au[i];
	}

	/* Load the stats */
	for (i = 0; i < A_MAX; i++)
	{
		p_ptr->stat_cur[i] = p_ptr->stat_max[i] = previous_char.stat_max[i];
	}

	/* Load the hp & mana */
	p_ptr->race_hp[0] = previous_char.race_hp_lv1;
	p_ptr->race_sp[0] = previous_char.race_sp_lv1;
	p_ptr->class_hp[p_ptr->pclass][0] = previous_char.class_hp_lv1;
	p_ptr->class_sp[p_ptr->pclass][0] = previous_char.class_sp_lv1;
	p_ptr->mhp = p_ptr->race_hp[0] + p_ptr->class_hp[p_ptr->pclass][0];
	p_ptr->chp = p_ptr->mhp;
	p_ptr->msp = p_ptr->race_sp[0] + p_ptr->class_sp[p_ptr->pclass][0];
	p_ptr->csp = p_ptr->msp;

	/* Load the history */
	for (i = 0; i < 4; i++)
	{
		strcpy(p_ptr->history[i], previous_char.history[i]);
	}

	/*** Save the previous data ***/
	if (swap)
	{
		COPY(&previous_char, &temp, birther);
	}
}




/*
 * Returns adjusted stat -JK-  Algorithm by -JWT-
 */
int adjust_stat(int value, int amount)
{
	int i;

	/* Negative amounts */
	if (amount < 0)
	{
		/* Apply penalty */
		for (i = 0; i < (0 - amount); i++)
		{
			if (value >= 18+10)
			{
				value -= 10;
			}
			else if (value > 18)
			{
				value = 18;
			}
			else if (value > 3)
			{
				value--;
			}
		}
	}

	/* Positive amounts */
	else if (amount > 0)
	{
		/* Apply reward */
		for (i = 0; i < amount; i++)
		{
			if (value < 18)
			{
				value++;
			}
			else
			{
				value += 10;
			}
		}
	}

	/* Return the result */
	return value;
}




/*
 * Roll for some info that the auto-roller ignores
 */
static void get_extra(bool roll_hitdie)
{
	int            i, j;
	cexp_info_type *cexp_ptr = &p_ptr->cexp_info[p_ptr->pclass];
	s32b           tmp32s;

	/* Experience factor */
	p_ptr->expfact = rp_ptr->r_exp;
	for (i = 0; i < MAX_CLASS; i++) p_ptr->cexpfact[i] = class_info[i].c_exp;

	(void)C_WIPE(p_ptr->cexp_info, MAX_CLASS, cexp_info_type);

	for (i = 0; i < MAX_WT; i++)
		p_ptr->weapon_exp[i] = 0;

	for (i = 0; i < 10; i++)
		p_ptr->skill_exp[i] = 0;

	for (i = 0; i <= MAX_REALM; i++)
		p_ptr->magic_exp[i] = p_ptr->s_ptr->s_eff[i];

	/* Roll for hit point unless quick-start */
	if (roll_hitdie)
	{
		p_ptr->race_hp[0] = p_ptr->race_sp[0] = p_ptr->class_hp[p_ptr->pclass][0] = p_ptr->class_sp[p_ptr->pclass][0] = 0;

		/* Gain level 1 HP/mana */
		for (i = 1; i < 4; i++)
		{
			tmp32s = rand_spread(rp_ptr->r_mhp, 1);
			p_ptr->race_hp[0] += MAX(tmp32s, 0);

			tmp32s = rand_spread(rp_ptr->r_msp, 1);
			p_ptr->race_sp[0] += MAX(tmp32s, 0);

			tmp32s = rand_spread(cp_ptr->c_mhp, 1);
			p_ptr->class_hp[p_ptr->pclass][0] += MAX(tmp32s, 0);

			tmp32s = rand_spread(cp_ptr->c_msp, 1);
			p_ptr->class_sp[p_ptr->pclass][0] += MAX(tmp32s, 0);
		}
	}

	/* Paranoia */
	for (i = 1; i < PY_MAX_LEVEL; i++)
	{
		p_ptr->race_hp[i] = p_ptr->race_sp[i] = 0;
	}

	for (i = 0; i < MAX_CLASS; i++)
	{
		for (j = (i == p_ptr->pclass) ? 1 : 0; j < PY_MAX_LEVEL; j++)
		{
			p_ptr->class_hp[i][j] = p_ptr->class_sp[i][j] = 0;
		}
	}

	/* Initial hitpoints */
	p_ptr->mhp = p_ptr->race_hp[0] + p_ptr->class_hp[p_ptr->pclass][0];

	/* Initial mana */
	p_ptr->msp = p_ptr->race_sp[0] + p_ptr->class_sp[p_ptr->pclass][0];

	/* Initial class */
	cexp_ptr->max_max_clev = cexp_ptr->max_clev = cexp_ptr->clev = 1;

	/* No feedback HP & mana */
	p_ptr->player_ghp = p_ptr->player_gsp = 0;

	/* Initial skills */
	p_ptr->gx_dis = cp_ptr->x_dis;
	p_ptr->gx_dev = cp_ptr->x_dev;
	p_ptr->gx_sav = cp_ptr->x_sav;
	p_ptr->gx_stl = cp_ptr->x_stl;
	p_ptr->gx_srh = cp_ptr->x_srh;
	p_ptr->gx_fos = cp_ptr->x_fos;
	p_ptr->gx_spd = rp_ptr->rx_spd + cp_ptr->x_spd;
	p_ptr->gx_thn = cp_ptr->x_thn;
	p_ptr->gx_thb = cp_ptr->x_thb;

	p_ptr->death_regen = 999;
}


/*
 * Get the racial history, and social class, using the "history charts".
 */
static void get_history(void)
{
	int i, n, chart, roll, social_class;

	char *s, *t;

	char buf[240];

	/* Clear the previous history strings */
	for (i = 0; i < 4; i++) p_ptr->history[i][0] = '\0';

	/* Clear the history text */
	buf[0] = '\0';

	if (astral_mode)
	{
		/* Acquire the textual history */
		strcat(buf, "���ʤ������Ĥη���Ƴ����Ƹ��Ƥγ��˹ߤ�Ω���ޤ��������η��Ϥ��ʤ��μ�˵ۤ��դ���Υ��褦�Ȥ��ޤ��󡣤��ʤ�������Ϥ��ʤ���ͦ��������տ������ײ����������Ƥ��ʤ��λ��ı��ˤ����äƤ��ޤ������ʤ��˹����ȡ��ե��顼�Ϥβø��");
	}
	else
	{
		/* Initial social class */
		social_class = randint1(4);

		/* Starting place */
		switch (p_ptr->prace)
		{
			case RACE_HUMAN:
			{
				chart = 1;
				break;
			}
			case RACE_HAWKMAN:
			{
				chart = 12;
				break;
			}
			case RACE_LIZARDMAN:
			{
				chart = 17;
				break;
			}
			case RACE_FAIRY:
			{
				chart = 24;
				break;
			}
			case RACE_GREMLIN:
			{
				chart = 29;
				break;
			}
			case RACE_SKELETON:
			{
				chart = 36;
				break;
			}
			case RACE_GHOST:
			{
				chart = 41;
				break;
			}
			case RACE_PUMPKINHEAD:
			{
				chart = 48;
				break;
			}
			case RACE_GOBLIN:
			{
				chart = 54;
				break;
			}
			case RACE_GORGON:
			{
				chart = 60;
				break;
			}
			case RACE_MERMAID:
			{
				chart = 66;
				break;
			}
			default:
			{
				chart = 0;
				break;
			}
		}


		/* Process the history */
		while (chart)
		{
			/* Start over */
			i = 0;

			/* Roll for nobility */
			roll = randint1(100);


			/* Access the proper entry in the table */
			while ((chart != bg[i].chart) || (roll > bg[i].roll)) i++;

			/* Acquire the textual history */
			(void)strcat(buf, bg[i].info);

			/* Add in the social class */
			social_class += (int)(bg[i].bonus) - 50;

			/* Enter the next chart */
			chart = bg[i].next;
		}


		/* Verify social class */
		if (social_class > 100) social_class = 100;
		else if (social_class < 1) social_class = 1;

		/* Save the social class */
		p_ptr->sc = social_class;
	}


	/* Skip leading spaces */
	for (s = buf; *s == ' '; s++) /* loop */;

	/* Get apparent length */
	n = strlen(s);

	/* Kill trailing spaces */

	while ((n > 0) && (s[n-1] == ' ')) s[--n] = '\0';

	{
		char temp[64 * 4];

		roff_to_buf(s, 60, temp, sizeof temp);
		t = temp;
		for (i = 0; i < 4; i++)
		{
			if (t[0] == 0) break;
			else
			{
				strcpy(p_ptr->history[i], t);
				t += strlen(t) + 1;
			}
		}
	}
}


/*
 * Computes character's age, height, and weight
 * by henkma
 */
static void get_ahw(void)
{
  int h_percent; /* ��Ĺ��ʿ�Ѥˤ���٤ƤɤΤ��餤�㤦��. */


  /* Calculate the age */
  p_ptr->age = rp_ptr->b_age + randint1(rp_ptr->m_age);
  
  /* Calculate the height/weight for males */
  if (p_ptr->psex == SEX_MALE)
	{
	  p_ptr->ht = randnor(rp_ptr->m_b_ht, rp_ptr->m_m_ht);
	  h_percent = (int)(p_ptr->ht) * 100 / (int)(rp_ptr->m_b_ht);
	  p_ptr->wt = randnor((int)(rp_ptr->m_b_wt) * h_percent /100
						  , (int)(rp_ptr->m_m_wt) * h_percent / 300 );
	}
  
  /* Calculate the height/weight for females */
  else if (p_ptr->psex == SEX_FEMALE)
	{
	  p_ptr->ht = randnor(rp_ptr->f_b_ht, rp_ptr->f_m_ht);
	  h_percent = (int)(p_ptr->ht) * 100 / (int)(rp_ptr->f_b_ht);
	  p_ptr->wt = randnor((int)(rp_ptr->f_b_wt) * h_percent /100
						  , (int)(rp_ptr->f_m_wt) * h_percent / 300 );
	}
}



/*
 * Clear all the global "character" data
 */
static void player_wipe(void)
{
	int i;

	/* Hack -- zero the struct */
	(void)WIPE(p_ptr, player_type);

	/* Wipe the quests */
	for (i = MIN_RANDOM_QUEST; i <= MAX_RANDOM_QUEST_ASTRAL; i++)
	{
		r_info[quest[i].r_idx].flags1 &= ~(RF1_QUESTOR);
	}
	for (i = 0; i < max_quests; i++)
	{
		quest[i].status = QUEST_STATUS_UNTAKEN;

		quest[i].cur_num = 0;
		quest[i].max_num = 0;
		quest[i].type = 0;
		quest[i].level = 0;
		quest[i].r_idx = 0;
		quest[i].complev = 0;
	}

	/* No items */
	inven_cnt = 0;
	equip_cnt = 0;

	/* Clear the inventory */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		object_wipe(&inventory[i]);
	}

	mw_old_weight = 0;
	mw_diff_to_melee = 0;


	/* Reset the "monsters" */
	for (i = 1; i < max_r_idx; i++)
	{
		monster_race *r_ptr = &r_info[i];

		/* Hack -- Reset the counter */
		r_ptr->cur_num = 0;

		/* Hack -- Reset the max counter */
		r_ptr->max_num = 100;

		/* Hack -- Reset the max counter */
		if (r_ptr->flags1 & RF1_UNIQUE) r_ptr->max_num = 1;
		if (r_ptr->flags7 & RF7_NAZGUL) r_ptr->max_num = 8;

		/* Hack -- Ronwe is dead */
		if (i == MON_RONWE) r_ptr->max_num = 0;

		/* Clear player kills */
		r_ptr->r_pkills = 0;

		/* Default elements */
		if (r_ptr->flags3 & RF3_ELEM_MULTI) r_ptr->r_elem = NO_ELEM;
		else if (r_ptr->flags3 & RF3_ELEM_FIRE) r_ptr->r_elem = ELEM_FIRE;
		else if (r_ptr->flags3 & RF3_ELEM_AQUA) r_ptr->r_elem = ELEM_AQUA;
		else if (r_ptr->flags3 & RF3_ELEM_EARTH) r_ptr->r_elem = ELEM_EARTH;
		else if (r_ptr->flags3 & RF3_ELEM_WIND) r_ptr->r_elem = ELEM_WIND;
		else if (r_ptr->flags1 & RF1_UNIQUE) r_ptr->r_elem = randint0(ELEM_NUM);
		else r_ptr->r_elem = NO_ELEM;
	}


	/* Hack -- Well fed player */
	p_ptr->food = PY_FOOD_FULL - 1;

	p_ptr->infected = FALSE;

	/* Clean the mutation count */
	mutant_regenerate_mod = 100;

	/* Clear "cheat" options */
	cheat_peek = FALSE;
	cheat_hear = FALSE;
	cheat_room = FALSE;
	cheat_xtra = FALSE;
	cheat_know = FALSE;
	cheat_live = FALSE;
	cheat_save = FALSE;

	stop_the_time_player = FALSE;

	/* Default pet command settings */
	p_ptr->pet_follow_distance = PET_FOLLOW_DIST;
	p_ptr->pet_extra_flags = (PF_TELEPORT | PF_ATTACK_SPELL | PF_SUMMON_SPELL | PF_DISI_SPELL);

	/* Wipe the recall depths */
	for (i = 0; i < max_d_idx; i++)
	{
		max_dlv[i] = 0;
	}

	/* Level one */
	p_ptr->max_max_plv = p_ptr->max_plv = p_ptr->lev = 1;

	/* Initialize arena and rewards information -KMW- */
	p_ptr->arena_number = 0;
	p_ptr->inside_arena = FALSE;
	p_ptr->inside_quest = 0;
	p_ptr->exit_bldg = TRUE; /* only used for arena now -KMW- */

	/* Set the recall dungeon accordingly */
	dungeon_type = 0;
	p_ptr->recall_dungeon = DUNGEON_ARMORICA;

	/* Reset chaos frame */
	for (i = 0; i < ETHNICITY_NUM; i++)
	{
		chaos_frame[i] = 0;
	}

	/* Reset effect of "The Fool" */
	fool_effect_status = FOOL_STATUS_NONE;

	misc_event_flags = 0L;

	ambush_flag = FALSE;
}


/*
 *  Hook function for quest monsters
 */
static bool mon_hook_quest(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Random quests are in the dungeon */
	if (r_ptr->flags8 & RF8_WILD_ONLY) return FALSE;

	/* No random quests for aquatic monsters */
	if (r_ptr->flags7 & RF7_AQUATIC) return FALSE;

	/* No random quests for multiplying monsters */
	if (r_ptr->flags2 & RF2_MULTIPLY) return FALSE;

	/* No quests to kill friendly monsters */
	if (r_ptr->flags7 & RF7_FRIENDLY) return FALSE;

	return TRUE;
}


/*
 *  Initialize random quests and final quests
 */
static void init_dungeon_quests(void)
{
	int i;
	monster_race    *r_ptr;
	int min_random_quest = astral_mode ? MIN_RANDOM_QUEST_ASTRAL : MIN_RANDOM_QUEST;
	int number_of_quests = astral_mode ? MAX_RANDOM_QUEST_ASTRAL - MIN_RANDOM_QUEST_ASTRAL + 1 : MAX_RANDOM_QUEST - MIN_RANDOM_QUEST + 1;

	/* Init the random quests */
	init_flags = INIT_ASSIGN;
	p_ptr->inside_quest = min_random_quest;

	process_dungeon_file("q_info.txt", 0, 0, 0, 0);

	p_ptr->inside_quest = 0;

	/* Prepare allocation table */
	get_mon_num_prep(mon_hook_quest, NULL);

	/* Generate quests */
	for (i = min_random_quest + number_of_quests - 1; i >= min_random_quest; i--)
	{
		quest_type      *q_ptr = &quest[i];
		monster_race    *quest_r_ptr;
		int             r_idx;
		int             mon_lev = astral_mode ? (100 - q_ptr->level) : q_ptr->level;

		q_ptr->status = QUEST_STATUS_TAKEN;

		while (1)
		{
			/*
			 * Random monster 5 - 10 levels out of depth
			 * (depending on level)
			 */
			r_idx = get_mon_num(mon_lev + 5 + randint1(mon_lev / 10));
			r_ptr = &r_info[r_idx];

			if(!(r_ptr->flags1 & RF1_UNIQUE)) continue;

			if(r_ptr->flags1 & RF1_QUESTOR) continue;

			if(r_ptr->flags7 & RF7_FRIENDLY) continue;

			if(r_ptr->flags7 & RF7_AQUATIC) continue;

			if(r_ptr->flags8 & RF8_WILD_ONLY) continue;

			if (monster_is_runeweapon(r_idx)) continue;

			/*
			 * Accept monsters that are 2 - 6 levels
			 * out of depth depending on the quest level
			 */
			if (r_ptr->level > (mon_lev + (mon_lev / 20))) break;
		}

		q_ptr->r_idx = r_idx;
		quest_r_ptr = &r_info[q_ptr->r_idx];

		/* Mark uniques */
		quest_r_ptr->flags1 |= RF1_QUESTOR;

		q_ptr->max_num = 1;
	}

	if (astral_mode)
	{
		/* Init the quest (Compensation of Runeweapon) */
		init_flags = INIT_ASSIGN;
		p_ptr->inside_quest = QUEST_RUNEWEAPON;

		process_dungeon_file("q_info.txt", 0, 0, 0, 0);

		quest[QUEST_RUNEWEAPON].status = QUEST_STATUS_TAKEN;
	}
	else
	{
		/* Init the two main quests (Lancelot + Dolgarua) */
		init_flags = INIT_ASSIGN;
		p_ptr->inside_quest = QUEST_LANCELOT;

		process_dungeon_file("q_info.txt", 0, 0, 0, 0);

		quest[QUEST_LANCELOT].status = QUEST_STATUS_TAKEN;

		p_ptr->inside_quest = QUEST_DOLGARUA;

		process_dungeon_file("q_info.txt", 0, 0, 0, 0);

		quest[QUEST_DOLGARUA].status = QUEST_STATUS_TAKEN;

		p_ptr->inside_quest = QUEST_ARMORICA;

		process_dungeon_file("q_info.txt", 0, 0, 0, 0);

		quest[QUEST_ARMORICA].status = QUEST_STATUS_TAKEN;
	}

	p_ptr->inside_quest = 0;
}

/*
 * Reset turn
 */
static void init_turn(void)
{
	turn = 1;
	turn_limit = TURNS_PER_TICK * TOWN_DAWN * (MAX_DAYS - 1) + TURNS_PER_TICK * TOWN_DAWN * 3 / 4;

	dungeon_turn = 1;
	dungeon_turn_limit = TURNS_PER_TICK * TOWN_DAWN * (MAX_DAYS - 1) + TURNS_PER_TICK * TOWN_DAWN * 3 / 4;
}


/*
 * Init players with some belongings
 *
 * Having an item makes the player "aware" of its purpose.
 */
void player_outfit(void)
{
	object_type	forge;
	object_type	*q_ptr;


	/* Get local object */
	q_ptr = &forge;

	/* Give the player some food */
	switch (p_ptr->prace)
	{
	case RACE_SKELETON:
	case RACE_GHOST:
		/* Nothing */
		break;
	default:
		/* Food rations */
		object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_RATION));
		q_ptr->number = (byte)rand_range(3, 7);
		object_aware(q_ptr);
		object_known(q_ptr);
		(void)inven_carry(q_ptr);
		break;
	}

	/* Get local object */
	q_ptr = &forge;

	/* Hack -- Give the player some torches */
	if (p_ptr->pclass != CLASS_NINJA)
	{
		object_prep(q_ptr, lookup_kind(TV_LITE, SV_LITE_TORCH));
		q_ptr->number = (byte)rand_range(3, 6);
		q_ptr->xtra4 = 2500;
		object_aware(q_ptr);
		object_known(q_ptr);
		(void)inven_carry(q_ptr);
	}

	/* Get local object */
	q_ptr = &forge;

	/* Hack -- Give the player three useful objects */
	switch (p_ptr->pclass)
	{
	case CLASS_SOLDIER:
	case CLASS_NINJA:
	case CLASS_AMAZONESS:
		object_prep(q_ptr, lookup_kind(TV_SWORD, SV_MINIMUM_DAGGER));
		q_ptr->number = (byte)rand_range(1, 4);
		object_aware(q_ptr);
		object_known(q_ptr);
		(void)inven_carry(q_ptr);
		object_prep(q_ptr, lookup_kind(TV_SOFT_ARMOR, SV_SOFT_LEATHER_ARMOR));
		object_aware(q_ptr);
		object_known(q_ptr);
		(void)inven_carry(q_ptr);
		if (p_ptr->pclass == CLASS_NINJA)
		{
			object_prep(q_ptr, lookup_kind(TV_SWORD, SV_MADU));
			object_aware(q_ptr);
			object_known(q_ptr);
			(void)inven_carry(q_ptr);
		}
		break;

	case CLASS_KNIGHT:
		object_prep(q_ptr, lookup_kind(TV_SWORD, SV_LONG_SWORD));
		object_aware(q_ptr);
		object_known(q_ptr);
		(void)inven_carry(q_ptr);
		object_prep(q_ptr, lookup_kind(TV_SOFT_ARMOR, SV_LEATHER_SCALE_MAIL));
		object_aware(q_ptr);
		object_known(q_ptr);
		(void)inven_carry(q_ptr);
		break;

	case CLASS_BERSERKER:
		object_prep(q_ptr, lookup_kind(TV_POLEARM, SV_BROAD_AXE));
		object_aware(q_ptr);
		object_known(q_ptr);
		(void)inven_carry(q_ptr);
		object_prep(q_ptr, lookup_kind(TV_SOFT_ARMOR, SV_HARD_LEATHER_ARMOR));
		object_aware(q_ptr);
		object_known(q_ptr);
		(void)inven_carry(q_ptr);
		break;

	case CLASS_BEASTTAMER:
		object_prep(q_ptr, lookup_kind(TV_HAFTED, SV_WHIP));
		object_aware(q_ptr);
		object_known(q_ptr);
		(void)inven_carry(q_ptr);
		object_prep(q_ptr, lookup_kind(TV_SOFT_ARMOR, SV_HARD_LEATHER_ARMOR));
		object_aware(q_ptr);
		object_known(q_ptr);
		(void)inven_carry(q_ptr);
		break;

	case CLASS_WIZARD:
		object_prep(q_ptr, lookup_kind(TV_HAFTED, SV_SCIPPLAYS_STAFF));
		object_aware(q_ptr);
		object_known(q_ptr);
		(void)inven_carry(q_ptr);
		object_prep(q_ptr, lookup_kind(TV_SOFT_ARMOR, SV_ROBE));
		object_aware(q_ptr);
		object_known(q_ptr);
		(void)inven_carry(q_ptr);
		object_prep(q_ptr, lookup_kind(TV_MAGERY_BOOK, 0));
		object_aware(q_ptr);
		object_known(q_ptr);
		(void)inven_carry(q_ptr);
		break;

	case CLASS_EXORCIST:
		object_prep(q_ptr, lookup_kind(TV_HAFTED, SV_FLAIL));
		object_aware(q_ptr);
		object_known(q_ptr);
		(void)inven_carry(q_ptr);
		object_prep(q_ptr, lookup_kind(TV_SOFT_ARMOR, SV_HARD_LEATHER_ARMOR));
		object_aware(q_ptr);
		object_known(q_ptr);
		(void)inven_carry(q_ptr);
		object_prep(q_ptr, lookup_kind(TV_CRUSADE_BOOK, 0));
		object_aware(q_ptr);
		object_known(q_ptr);
		(void)inven_carry(q_ptr);
		break;

	case CLASS_VALKYRIE:
		object_prep(q_ptr, lookup_kind(TV_POLEARM, SV_SPEAR));
		object_aware(q_ptr);
		object_known(q_ptr);
		(void)inven_carry(q_ptr);
		object_prep(q_ptr, lookup_kind(TV_SOFT_ARMOR, SV_LEATHER_SCALE_MAIL));
		object_aware(q_ptr);
		object_known(q_ptr);
		(void)inven_carry(q_ptr);
		break;

	case CLASS_ARCHER:
		object_prep(q_ptr, lookup_kind(TV_BOW, SV_LONG_BOW));
		object_aware(q_ptr);
		object_known(q_ptr);
		(void)inven_carry(q_ptr);
		object_prep(q_ptr, lookup_kind(TV_ARROW, SV_AMMO_NORMAL));
		q_ptr->number = (byte)rand_range(15, 20);
		object_aware(q_ptr);
		object_known(q_ptr);
		(void)inven_carry(q_ptr);
		object_prep(q_ptr, lookup_kind(TV_SOFT_ARMOR, SV_SOFT_LEATHER_ARMOR));
		object_aware(q_ptr);
		object_known(q_ptr);
		(void)inven_carry(q_ptr);
		break;

	case CLASS_WITCH:
		object_prep(q_ptr, lookup_kind(TV_HAFTED, SV_SCIPPLAYS_STAFF));
		object_aware(q_ptr);
		object_known(q_ptr);
		(void)inven_carry(q_ptr);
		object_prep(q_ptr, lookup_kind(TV_SOFT_ARMOR, SV_ROBE));
		object_aware(q_ptr);
		object_known(q_ptr);
		(void)inven_carry(q_ptr);
		object_prep(q_ptr, lookup_kind(TV_WITCH_BOOK, 0));
		object_aware(q_ptr);
		object_known(q_ptr);
		(void)inven_carry(q_ptr);
		break;

	case CLASS_CLERIC:
		object_prep(q_ptr, lookup_kind(TV_HAFTED, SV_SCIPPLAYS_STAFF));
		object_aware(q_ptr);
		object_known(q_ptr);
		(void)inven_carry(q_ptr);
		object_prep(q_ptr, lookup_kind(TV_SOFT_ARMOR, SV_ROBE));
		object_aware(q_ptr);
		object_known(q_ptr);
		(void)inven_carry(q_ptr);
		object_prep(q_ptr, lookup_kind(TV_HOLY_BOOK, 0));
		object_aware(q_ptr);
		object_known(q_ptr);
		(void)inven_carry(q_ptr);
		break;
	}

	if (astral_mode)
	{
		runeweapon_type *runeweapon = &runeweapon_list[1];
		u32b flgs[TR_FLAG_SIZE];

		object_copy(q_ptr, &runeweapon->weapon);
		q_ptr->curse_flags |= (TRC_CURSED | TRC_HEAVY_CURSE | TRC_PERMA_CURSE);
		object_copy(&inventory[(runeweapon->weapon.tval != TV_BOW) ? INVEN_RARM : INVEN_BOW], q_ptr);

		runeweapon->status |= (RW_STATUS_FOUND);

		/* Increase the weight */
		p_ptr->total_weight += q_ptr->weight;

		/* Increment the equip counter by hand */
		equip_cnt++;

		/* Recalculate bonuses */
		p_ptr->update |= (PU_BONUS);

		/* Recalculate torch */
		p_ptr->update |= (PU_TORCH);

		/* Recalculate mana */
		p_ptr->update |= (PU_MANA);

		p_ptr->redraw |= (PR_EQUIPPY);

		/* Window stuff */
		p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);

		object_flags(q_ptr, flgs);
	}
}


/* Locations of the tables on the screen */
#define HEADER_ROW		1
#define INSTRUCT_ROW	3
#define QUESTION_ROW	7
#define TABLE_ROW		10
#define POINT_ROW		14

#define QUESTION_COL	2
#define SEX_COL			2
#define RACE_COL		14
#define RACE_AUX_COL    34
#define CLASS_COL		34
#define CLASS_AUX_COL   54
#define ELEM_COL		54
#define POINT_COL		10

#define INVALID_CHOICE 255


/*
 * Clear the previous question
 */
static void clear_question(void)
{
	int i;

	for (i = QUESTION_ROW; i < TABLE_ROW; i++)
	{
		/* Clear line, position cursor */
		Term_erase(0, i, 255);
	}
}

/*
 * Generic "get choice from menu" function
 */
static int get_player_choice(birth_menu *choices, int num, int col, int wid,
                             cptr helpfile, void (*hook)(int))
{
	int top = 0, cur = 0, i;
	char c;
	char buf[80];
	bool done = FALSE;
	int hgt;
	byte attr;

	/* Autoselect if able */
	if (num == 1) done = TRUE;

	/* Clear */
	for (i = TABLE_ROW; i < Term->hgt; i++)
	{
		/* Clear */
		Term_erase(col, i, Term->wid - wid);
	}

	/* Choose */
	while (TRUE)
	{
		hgt = Term->hgt - TABLE_ROW - 1;

		/* Redraw the list */
		for (i = 0; ((i + top < num) && (i <= hgt)); i++)
		{
			if (i + top < 26)
			{
				sprintf(buf, "%c) %s", I2A(i + top), choices[i + top].name);
			}
			else
			{
				/* ToDo: Fix the ASCII dependency */
				sprintf(buf, "%c) %s", 'A' + (i + top - 26), choices[i + top].name);
			}

			/* Clear */
			Term_erase(col, i + TABLE_ROW, wid);

			/* Display */
			/* Highlight the current selection */
			if (i == (cur - top)) attr = TERM_L_BLUE;
			else attr = TERM_WHITE;

			Term_putstr(col, i + TABLE_ROW, wid, attr, buf);
		}

		if (done) return choices[cur].real;

		/* Display auxiliary information if any is available. */
		if (hook) hook(choices[cur].real);

		/* Move the cursor */
		put_str("", TABLE_ROW + cur - top, col);

		c = inkey();

		switch (c)
		{
		case 'Q':
			quit(NULL);

		case 'S':
			/* Mega Hack - go back. */
			return INVALID_CHOICE;

		case '*':
			/* Select a legal choice at random */
			cur = randint0(num);

			/* Move it onto the screen */
			if ((cur < top) || (cur > top + hgt))
			{
				top = cur;
			}

			/* Done */
			done = TRUE;
			break;

		case '?':
			show_help(helpfile);
			break;

		case '=':
			screen_save();
#ifdef JP
			do_cmd_options_aux(6, "������ץ����((*)�ϥ������˱ƶ�)");
#else
			do_cmd_options_aux(6, "Startup Opts((*)s effect score)");
#endif
			screen_load();
			break;

		case '\n':
		case '\r':
		case ' ':
			/* Done */
			return choices[cur].real;

		/* Going up? */
		case '8':
			if (cur != 0)
			{
				/* Move selection */
				cur--;
			}

			if ((top > 0) && ((cur - top) < 4))
			{
				/* Scroll up */
				top--;
			}
			break;

		/* Going down? */
		case '2':
			if (cur != (num - 1))
			{
				/* Move selection */
				cur++;
			}

			if ((top + hgt < (num - 1)) && ((top + hgt - cur) < 4))
			{
				/* Scroll down */
				top++;
			}
			break;

		default:
			if (isalpha(c))
			{
				int choice;

				if (islower(c))
				{
					choice = A2I(c);
				}
				else
				{
					choice = c - 'A' + 26;
				}

				/* Validate input */
				if ((choice > -1) && (choice < num))
				{
					cur = choice;

					/* Move it onto the screen */
					if ((cur < top) || (cur > top + hgt))
					{
						top = cur;
					}

					/* Done */
					done = TRUE;
				}
				else
				{
					bell();
				}
			}

			/* Invalid input */
			else bell();
			break;
		}
	}

	/* NOTREACHED */
}

/*
 * Display additional information about each race during the selection.
 */
static void race_aux_hook(int race)
{
	int i;
	char s[50];

	if ((race < 0) || (race >= MAX_RACES)) return;

	/* Display relevant details. */
	for (i = 0; i < A_MAX; i++)
	{
		sprintf(s, "%s%+d", stat_names[i],
		race_info[race].r_adj[i]);
		Term_putstr(RACE_AUX_COL, TABLE_ROW + i, -1, TERM_WHITE, s);
	}

#ifdef JP
	sprintf(s, "�ҥåȥ�����: %ld ", race_info[race].r_mhp);
	Term_putstr(RACE_AUX_COL, TABLE_ROW + A_MAX, -1, TERM_WHITE, s);
	sprintf(s, "�ޥ��å�������: %ld ", race_info[race].r_msp);
	Term_putstr(RACE_AUX_COL, TABLE_ROW + A_MAX + 1, -1, TERM_WHITE, s);
	sprintf(s, "�и���: %d%% ", race_info[race].r_exp);
	Term_putstr(RACE_AUX_COL, TABLE_ROW + A_MAX + 2, -1, TERM_WHITE, s);
	sprintf(s, "�ֳ�������: %d ft ", race_info[race].infra * 10);
	Term_putstr(RACE_AUX_COL, TABLE_ROW + A_MAX + 3, -1, TERM_WHITE, s);
#else
	sprintf(s, "Hit die: %d ", race_info[race].r_mhp);
	Term_putstr(RACE_AUX_COL, TABLE_ROW + A_MAX, -1, TERM_WHITE, s);
	sprintf(s, "Mana die: %d ", race_info[race].r_msp);
	Term_putstr(RACE_AUX_COL, TABLE_ROW + A_MAX + 1, -1, TERM_WHITE, s);
	sprintf(s, "Experience: %d%% ", race_info[race].r_exp);
	Term_putstr(RACE_AUX_COL, TABLE_ROW + A_MAX + 2, -1, TERM_WHITE, s);
	sprintf(s, "Infravision: %d ft ", race_info[race].infra * 10);
	Term_putstr(RACE_AUX_COL, TABLE_ROW + A_MAX + 3, -1, TERM_WHITE, s);
#endif
}

/*
 * Player race
 */
static bool get_player_race(void)
{
	int i, num = 0;
	birth_menu races[MAX_RACES];

	/* Extra info */
#ifdef JP
	Term_putstr(QUESTION_COL, QUESTION_ROW, -1, TERM_YELLOW,
		"�Լ�²�դˤ�äƥ���饯��������ŷŪ�ʻ����ܡ��ʥ����Ѳ����ޤ���");
#else
	Term_putstr(QUESTION_COL, QUESTION_ROW, -1, TERM_YELLOW,
		"Your 'race' determines various intrinsic factors and bonuses.");
#endif

	/* Tabulate races */
	for (i = 0; i < MAX_RACES; i++)
	{
		if (((race_info[i].sex == RACE_TYPE_MALE) && (p_ptr->psex == SEX_FEMALE))
			|| ((race_info[i].sex == RACE_TYPE_FEMALE) && (p_ptr->psex == SEX_MALE)))
			continue;

		races[num].name = race_info[i].title;
		races[num++].real = i;
	}

#ifdef JP
	p_ptr->prace = get_player_choice(races, num, RACE_COL, 20, "jraceclas.txt#TheRaces", race_aux_hook);
#else
	p_ptr->prace = get_player_choice(races, num, RACE_COL, 20, "raceclas.txt#TheRaces", race_aux_hook);
#endif

	/* No selection? */
	if (p_ptr->prace == INVALID_CHOICE)
	{
		p_ptr->prace = 0;
		return (FALSE);
	}

	/* Save the race pointer */
	rp_ptr = &race_info[p_ptr->prace];

	/* Success */
	return (TRUE);
}


/*
 * Display additional information about each class during the selection.
 */
static void class_aux_hook(int class_idx)
{
	int i;
	char s[128], buf[8];

	if ((class_idx < 0) || (class_idx >= MAX_CLASS)) return;

	/* Display relevant details. */
	for (i = 0; i < A_MAX; i++)
	{
		if (class_info[class_idx].c_need[i]) cnv_stat(class_info[class_idx].c_need[i], buf);
		else strcpy(buf, "      ");
		sprintf(s, "%s%s", stat_names[i], buf);
		Term_putstr(CLASS_AUX_COL, TABLE_ROW + i, -1, TERM_WHITE, s);
	}
}


/*
 * Player class
 */
static bool get_player_class(void)
{
	int  i, num = 0;
	birth_menu classes[MAX_CLASS];

#ifdef JP
	Term_putstr(QUESTION_COL, QUESTION_ROW, -1, TERM_YELLOW,
		"�ԥ��饹�դˤ�äƥ���饯�����ε�ǽ����ˡ�ʤɤ��Ѳ����ޤ���");
#else
	Term_putstr(QUESTION_COL, QUESTION_ROW, -1, TERM_YELLOW,
		"Your 'class' determines skill, magic realm, etc.");
#endif

	/* Tabulate classes */
	for (i = 0; i < MAX_CLASS; i++)
	{
		if (can_choose_class(i, CLASS_CHOOSE_MODE_BIRTH))
		{
			/* Save the string */
			classes[num].name = class_info[i].title;
			classes[num++].real = i;
		}
	}

#ifdef JP
	p_ptr->pclass = get_player_choice(classes, num, CLASS_COL, 20, "jraceclas.txt#TheClasses", class_aux_hook);
#else
	p_ptr->pclass = get_player_choice(classes, num, CLASS_COL, 20, "raceclas.txt#TheClasses", class_aux_hook);
#endif

	/* No selection? */
	if (p_ptr->pclass == INVALID_CHOICE)
	{
		p_ptr->pclass = 0;

		return (FALSE);
	}

	/* Set class */
	cp_ptr = &class_info[p_ptr->pclass];
	mp_ptr = &m_info[p_ptr->pclass];
	p_ptr->s_ptr = &s_info[p_ptr->pclass];

	return (TRUE);
}

/*
 * Player sex
 */
static bool get_player_sex(void)
{
	int i;
	birth_menu genders[MAX_SEXES];

	/* Extra info */
#ifdef JP
	Term_putstr(QUESTION_COL, QUESTION_ROW, -1, TERM_L_RED,
		"�����̡դΰ㤤�ϼ�²�ȥ��饹������˱ƶ���ڤܤ��ޤ���");
#else
	Term_putstr(QUESTION_COL, QUESTION_ROW, -1, TERM_L_RED,
		"Your 'sex' effects race choice and class choice.");
#endif

	/* Tabulate genders */
	for (i = 0; i < MAX_SEXES; i++)
	{
		genders[i].name = sex_info[i].title;
		genders[i].real = i;
	}

#ifdef JP
	p_ptr->psex = get_player_choice(genders, MAX_SEXES, SEX_COL, 15, "jhelp.hlp", NULL);
#else
	p_ptr->psex = get_player_choice(genders, MAX_SEXES, SEX_COL, 15, "help.hlp", NULL);
#endif

	/* No selection? */
	if (p_ptr->psex == INVALID_CHOICE)
	{
		p_ptr->psex = 0;
		return (FALSE);
	}

	/* Save the sex pointer */
	sp_ptr = &sex_info[p_ptr->psex];

	return (TRUE);
}

/*
 * Player element
 */
static bool get_player_element(void)
{
	int i;
	birth_menu elements[ELEM_NUM];

#ifdef JP
	Term_putstr(QUESTION_COL, QUESTION_ROW, -1, TERM_YELLOW,
		"�ԥ�����ȡդΰ㤤�ϰ����Υ��饹����ˡ�˱ƶ���ڤܤ��ޤ���");
#else
	Term_putstr(QUESTION_COL, QUESTION_ROW, -1, TERM_YELLOW,
		"Your 'element' effects magic realm of several classes.");
#endif

	/* Tabulate elements */
	for (i = 0; i < ELEM_NUM; i++)
	{
		elements[i].name = elem_names[i];
		elements[i].real = i;
	}

#ifdef JP
	i = get_player_choice(elements, ELEM_NUM, ELEM_COL, 15, "jraceclas.txt#TheElements", NULL);
#else
	i = get_player_choice(elements, ELEM_NUM, ELEM_COL, 15, "raceclas.txt#TheElements", NULL);
#endif

	/* No selection? */
	if (i == INVALID_CHOICE)
	{
		p_ptr->pelem = -1;
		return (FALSE);
	}

	/* Save the element */
	p_ptr->pelem = i;

	return (TRUE);
}

/*
 * Helper function for 'player_birth()'.
 *
 * This function allows the player to select a sex, race, and class, and
 * modify options (including the birth options).
 */
static bool player_birth_aux_1(void)
{
	/*** Instructions ***/

	/* Clear screen */
	Term_clear();

	/* Display some helpful information */
#ifdef JP
	Term_putstr(QUESTION_COL, HEADER_ROW, -1, TERM_L_BLUE,
	            "�ʲ��Υ�˥塼���饭��饯�������Ǥ�����Ǥ���������");
	Term_putstr(QUESTION_COL, INSTRUCT_ROW, -1, TERM_WHITE,
	            "��ư�����ǹ��ܤ򥹥����뤵����Enter�Ƿ��ꤷ�ޤ���");
	Term_putstr(QUESTION_COL, INSTRUCT_ROW + 1, -1, TERM_WHITE,
	            "'*' �ǥ�������ꡢ'S' �Ǥ��ľ���ޤ���");
	Term_putstr(QUESTION_COL, INSTRUCT_ROW + 2, -1, TERM_WHITE,
	            "'=' �ǽ�����ץ�������ꡢ'?' �ǥإ�ס�'Q' �ǽ�λ���ޤ���");

	/* Hack - highlight the key names */
	Term_putstr(QUESTION_COL + 0, INSTRUCT_ROW, -1, TERM_L_GREEN, "��ư����");
	Term_putstr(QUESTION_COL + 32, INSTRUCT_ROW, -1, TERM_L_GREEN, "Enter");
	Term_putstr(QUESTION_COL + 1, INSTRUCT_ROW + 1, -1, TERM_L_GREEN, "*");
	Term_putstr(QUESTION_COL + 21, INSTRUCT_ROW + 1, -1, TERM_L_GREEN, "S");
	Term_putstr(QUESTION_COL + 1, INSTRUCT_ROW + 2, -1, TERM_L_GREEN, "=");
	Term_putstr(QUESTION_COL + 27, INSTRUCT_ROW + 2, -1, TERM_L_GREEN, "?");
	Term_putstr(QUESTION_COL + 41, INSTRUCT_ROW + 2, -1, TERM_L_GREEN, "Q");
#else
	Term_putstr(QUESTION_COL, HEADER_ROW, -1, TERM_L_BLUE,
	            "Please select your character from the menu below:");
	Term_putstr(QUESTION_COL, INSTRUCT_ROW, -1, TERM_WHITE,
	            "Use the movement keys to scroll the menu, Enter to select the current");
	Term_putstr(QUESTION_COL, INSTRUCT_ROW + 1, -1, TERM_WHITE,
	            "menu item, '*' for a random menu item, 'S' to restart the character");
	Term_putstr(QUESTION_COL, INSTRUCT_ROW + 2, -1, TERM_WHITE,
	            "selection, '=' for the birth options, '?' for help, or 'Q' to quit.");

	/* Hack - highlight the key names */
	Term_putstr(QUESTION_COL + 8, INSTRUCT_ROW, -1, TERM_L_GREEN, "movement keys");
	Term_putstr(QUESTION_COL + 42, INSTRUCT_ROW, -1, TERM_L_GREEN, "Enter");
	Term_putstr(QUESTION_COL + 12, INSTRUCT_ROW + 1, -1, TERM_L_GREEN, "*");
	Term_putstr(QUESTION_COL + 40, INSTRUCT_ROW + 1, -1, TERM_L_GREEN, "S");
	Term_putstr(QUESTION_COL + 12, INSTRUCT_ROW + 2, -1, TERM_L_GREEN, "=");
	Term_putstr(QUESTION_COL + 39, INSTRUCT_ROW + 2, -1, TERM_L_GREEN, "?");
	Term_putstr(QUESTION_COL + 56, INSTRUCT_ROW + 2, -1, TERM_L_GREEN, "Q");
#endif

	/* Choose the player's sex */
	if (!get_player_sex()) return (FALSE);

	/* Clean up */
	clear_question();

	/* Choose the player's race */
	if (!get_player_race()) return (FALSE);

	/* Clean up */
	clear_question();

	/* Choose the player's class */
	if (!get_player_class()) return (FALSE);

	/* Clean up */
	clear_question();

	/* Choose the player's element */
	if (!get_player_element()) return (FALSE);

	/* Clear */
	Term_clear();

	screen_save();
#ifdef JP
	do_cmd_options_aux(6, "������ץ����((*)�ϥ������˱ƶ�)");
#else
	do_cmd_options_aux(6, "Startup Opts((*)s effect score)");
#endif
	screen_load();

	/* Clear */
	Term_clear();

	/* Reset turn; before auto-roll and after choosing race */
	init_turn();

	/* Done */
	return (TRUE);
}

/*
 * Initial stat costs (initial stats always range from 10 to 18 inclusive).
 */
#define POINTS		60
#define BASE_STAT	10
#define STAT_LIMIT  18

static int birth_stat_costs[(STAT_LIMIT - BASE_STAT) + 1] = { 0, 1, 2, 4, 7, 11, 16, 22, 30 };

/*
 * Helper function for 'player_birth()'.
 *
 * This function handles "point-based" character creation.
 *
 * The player selects, for each stat, a value from 10 to 18 (inclusive),
 * each costing a certain amount of points (as above), from a pool of 54
 * available points, to which race/class modifiers are then applied.
 *
 * Each unused point is converted into 50 gold pieces.
 */

static bool player_birth_aux_2(void)
{
	int i, j;

	byte stat = 0, old_stat = 0;

	byte stats[A_MAX];
	byte base_stats[A_MAX];

	int cost = 0, fix_cost, remain_point;

	bool flag = FALSE;

	char buf[80];
	char base[8], min[8], max[8], inp[8];

	/* Initialize stats */
	for (i = 0; i < A_MAX; i++)
	{
		int c_need = cp_ptr->c_need[i] ? adjust_stat(cp_ptr->c_need[i], -rp_ptr->r_adj[i]) : 0;

		/* Initial base stats */
		if (c_need > BASE_STAT) base_stats[i] = (byte)c_need;
		else base_stats[i] = BASE_STAT;

		/* Initial stats */
		stats[i] = base_stats[i];
		cost += birth_stat_costs[stats[i] - BASE_STAT];
	}
	fix_cost = cost;

	/* Roll for base hitpoints */
	get_extra(TRUE);

	/* Roll for age/height/weight */
	get_ahw();

	/* Roll for social class */
	get_history();

#ifdef JP
	Term_putstr(QUESTION_COL, HEADER_ROW, -1, TERM_L_BLUE,
	            "����饯����ǽ���ͤ����ꤷ�Ƥ���������");
	put_str("����   : ", INSTRUCT_ROW, QUESTION_COL - 1);
	put_str("��²   : ", INSTRUCT_ROW + 1, QUESTION_COL - 1);
	put_str("���饹 : ", INSTRUCT_ROW + 2, QUESTION_COL - 1);
	Term_putstr(QUESTION_COL, QUESTION_ROW, -1, TERM_YELLOW,
		"���򤷤����饹�ˤ�ä�ɬ�פʥ����Ȥ������ä����֤Ǥ�����ȤʤäƤ��ޤ���");
	put_str("        ������  �Ǿ���  ������  (������)  ��²     �����", POINT_ROW - 1, POINT_COL);
#else
	Term_putstr(QUESTION_COL, HEADER_ROW, -1, TERM_L_BLUE,
	            "Configure stats of character.");
	put_str("Sex    : ", INSTRUCT_ROW, QUESTION_COL - 1);
	put_str("Race   : ", INSTRUCT_ROW + 1, QUESTION_COL - 1);
	put_str("Class  : ", INSTRUCT_ROW + 2, QUESTION_COL - 1);
	Term_putstr(QUESTION_COL, QUESTION_ROW, -1, TERM_YELLOW,
		"Points are already reduced by chosen class.");
	put_str("          Base     Min     Max    (Cost)  Race      Total", POINT_ROW - 1, POINT_COL);
#endif
	c_put_str(TERM_L_BLUE, sp_ptr->title, INSTRUCT_ROW, QUESTION_COL + 8);
	c_put_str(TERM_L_BLUE, rp_ptr->title, INSTRUCT_ROW + 1, QUESTION_COL + 8);
	c_put_str(TERM_L_BLUE, cp_ptr->title, INSTRUCT_ROW + 2, QUESTION_COL + 8);

	for (i = 0; i < A_MAX; i++)
	{
		/* Prepare a prompt */
		cnv_stat(stats[i], base);
		cnv_stat(base_stats[i], min);
		remain_point = POINTS - fix_cost + birth_stat_costs[base_stats[i] - BASE_STAT];
		for (j = base_stats[i] - BASE_STAT; j < (STAT_LIMIT - BASE_STAT); j++)
		{
			if (birth_stat_costs[j + 1] > remain_point) break;
		}
		cnv_stat(BASE_STAT + j, max);
		cnv_stat(adjust_stat(stats[i], rp_ptr->r_adj[i]), inp);
		sprintf(buf, "%6s  %6s  %6s  %6s      (%2d)   %+3d  =  %6s",
			stat_names[i], base, min, max, birth_stat_costs[stats[i] - BASE_STAT],
			rp_ptr->r_adj[i], inp);
		put_str(buf, POINT_ROW + i, POINT_COL);
	}

	/* Interact */
	while (TRUE)
	{
		/* Reset cost */
		cost = 0;

		/* Process stats */
		for (i = 0; i < A_MAX; i++)
		{
			/* Total cost */
			cost += birth_stat_costs[stats[i] - BASE_STAT];
		}

		/* Restrict cost */
		if (cost > POINTS)
		{
			/* Warning */
			bell();

			/* Reduce stat */
			stats[stat]--;

			/* Recompute costs */
			continue;
		}

		c_put_str(TERM_WHITE, buf, POINT_ROW + old_stat, POINT_COL);

		/* Prompt XXX XXX XXX */
#ifdef JP
		sprintf(buf, "�����ȹ��: %2d/%d  2/8/j/k �ǰ�ư,  4/6/h/l ���ѹ�,  Enter �Ƿ���", cost, POINTS);
#else
		sprintf(buf, "Total Cost %2d/%d.  Use 2/8/j/k to move, 4/6/h/l to modify, Enter to accept.", cost, POINTS);
#endif
		c_put_str(TERM_L_BLUE, buf, POINT_ROW - 3, POINT_COL);

		if (cost < POINTS)
		{
#ifdef JP
			sprintf(buf, "�Ȥ��Ƥ��ʤ��ݥ���Ȥ�$%d�λ�ʾ���Ѵ�����ޤ���        ", (50 * (POINTS - cost)));
#else
			sprintf(buf, "Unused points are converted into $%d notes.        ", (50 * (POINTS - cost)));
#endif
			put_str(buf, POINT_ROW + A_MAX + 1, POINT_COL);
		}
		else clear_from(POINT_ROW + A_MAX + 1);

		/* Prepare a prompt */
		cnv_stat(stats[stat], base);
		cnv_stat(base_stats[stat], min);
		remain_point = POINTS - fix_cost + birth_stat_costs[base_stats[stat] - BASE_STAT];
		for (j = base_stats[stat] - BASE_STAT; j < (STAT_LIMIT - BASE_STAT); j++)
		{
			if (birth_stat_costs[j + 1] > remain_point) break;
		}
		cnv_stat(BASE_STAT + j, max);
		cnv_stat(adjust_stat(stats[stat], rp_ptr->r_adj[stat]), inp);
		sprintf(buf, "%6s  %6s  %6s  %6s      (%2d)   %+3d  =  %6s",
			stat_names[stat], base, min, max, birth_stat_costs[stats[stat] - BASE_STAT],
			rp_ptr->r_adj[stat], inp);
		c_put_str(TERM_L_GREEN, buf, POINT_ROW + stat, POINT_COL);
		c_put_str(TERM_YELLOW, base, POINT_ROW + stat, POINT_COL + 8);

		old_stat = stat;

		/* Place cursor just after current base stat */
		Term_gotoxy(POINT_COL + 14, POINT_ROW + stat);

		/* Get key */
		switch (inkey())
		{
		/* Quit */
		case 'Q':
			quit(NULL);

		/* Start over */
		case 'S':
			return (FALSE);

		/* Done */
		case '\r':
		case '\n':
		case ESCAPE:
			flag = TRUE;
			break;

		/* Prev stat */
		case '8':
		case 'k':
			stat = (stat + A_MAX - 1) % A_MAX;
			break;

		/* Next stat */
		case '2':
		case 'j':
			stat = (stat + 1) % A_MAX;
			break;

		/* Decrease stat */
		case '4':
		case 'h':
			if (stats[stat] > base_stats[stat]) stats[stat]--;
			break;

		/* Increase stat */
		case '6':
		case 'l':
			if (stats[stat] < STAT_LIMIT) stats[stat]++;
			break;
		}
		if (flag) break;
	}

	/* Process stats */
	for (i = 0; i < A_MAX; i++)
	{
		p_ptr->stat_cur[i] = p_ptr->stat_max[i] = stats[i];
	}

	/* Gold is inversely proportional to cost */
	p_ptr->au[SV_GOLD_NOTE] = (50 * (POINTS - cost)) + (easy_band ? 10000000L : 100);

	/* Calculate the bonuses and hitpoints */
	p_ptr->update |= (PU_BONUS | PU_HP | PU_MANA | PU_GOLD);

	/* Update stuff */
	update_stuff();

	/* Fully healed */
	p_ptr->chp = p_ptr->mhp;

	/* Fully rested */
	p_ptr->csp = p_ptr->msp;

	/* Done */
	return (TRUE);
}


/*
 *  Character background edit-mode
 */
static void edit_history(void)
{
	char old_history[4][60];
	char c;
	int y = 0, x = 0;
	int i, j;

	/* Edit character background */
	for (i = 0; i < 4; i++)
	{
		sprintf(old_history[i], "%s", p_ptr->history[i]);
	}
	/* Turn 0 to space */
	for (i = 0; i < 4; i++)
	{
		for (j = 0; p_ptr->history[i][j]; j++) /* loop */;

		for (; j < 59; j++) p_ptr->history[i][j] = ' ';
		p_ptr->history[i][59] = '\0';
	}
	display_player(1);
#ifdef JP
	c_put_str(TERM_L_GREEN, "(����饯����������Ω�� - �Խ��⡼��)", 11, 20);
#else
	c_put_str(TERM_L_GREEN, "(Character Background - Edit Mode)", 11, 20);
#endif

	while (TRUE)
	{
		for (i = 0; i < 4; i++)
		{
			put_str(p_ptr->history[i], i + 12, 10);
		}
#ifdef JP
		if (iskanji2(p_ptr->history[y], x))
			c_put_str(TERM_L_BLUE, format("%c%c", p_ptr->history[y][x],p_ptr->history[y][x+1]), y + 12, x + 10);
		else
#endif
		c_put_str(TERM_L_BLUE, format("%c", p_ptr->history[y][x]), y + 12, x + 10);

		/* Place cursor just after cost of current stat */
		Term_gotoxy(x + 10, y + 12);

		c = inkey();

		if (c == '8')
		{
			y--;
			if (y < 0) y = 3;
#ifdef JP
			if ((x > 0) && (iskanji2(p_ptr->history[y], x-1))) x--;
#endif
		}
		else if (c == '2')
		{
			y++;
			if (y > 3) y = 0;
#ifdef JP
			if ((x > 0) && (iskanji2(p_ptr->history[y], x-1))) x--;
#endif
		}
		else if (c == '6')
		{
#ifdef JP
			if (iskanji2(p_ptr->history[y], x)) x++;
#endif
			x++;
			if (x > 58)
			{
				x = 0;
				if (y < 3) y++;
			}
		}
		else if (c == '4')
		{
			x--;
			if (x < 0)
			{
				if (y)
				{
					y--;
					x = 58;
				}
				else x = 0;
			}

#ifdef JP
			if ((x > 0) && (iskanji2(p_ptr->history[y], x-1))) x--;
#endif
		}
		else if (c == '\r' || c == '\n')
		{
			break;
		}
		else if (c == ESCAPE)
		{
			for (i = 0; i < 4; i++)
			{
				sprintf(p_ptr->history[i], "%s", old_history[i]);
				put_str(p_ptr->history[i], i + 12, 10);
			}
			break;
		}
		else if (c == '\010')
		{
			x--;
			if (x < 0)
			{
				if (y)
				{
					y--;
					x = 58;
				}
				else x = 0;
			}

			p_ptr->history[y][x] = ' ';
#ifdef JP
			if ((x > 0) && (iskanji2(p_ptr->history[y], x - 1)))
			{
				x--;
				p_ptr->history[y][x] = ' ';
			}
#endif
		}
#ifdef JP
		else if (iskanji(c) || isprint(c))
#else
		else if (isprint(c)) /* BUGFIX */
#endif
		{
#ifdef JP
			if (iskanji2(p_ptr->history[y], x))
			{
				p_ptr->history[y][x+1] = ' ';
			}

			if (iskanji(c))
			{
				if (x > 57)
				{
					x = 0;
					y++;
					if (y > 3) y = 0;
				}

				if (iskanji2(p_ptr->history[y], x+1))
				{
					p_ptr->history[y][x+2] = ' ';
				}

				p_ptr->history[y][x++] = c;

				c = inkey();
			}
#endif
			p_ptr->history[y][x++] = c;
			if (x > 58)
			{
				x = 0;
				y++;
				if (y > 3) y = 0;
			}
		}
	} /* while (TRUE) */

}


/*
 * Helper function for 'player_birth()'
 */
static bool player_birth_full(void)
{
	char c;

	/* Ask questions */
	if (!player_birth_aux_1()) return FALSE;
	if (!player_birth_aux_2()) return FALSE;

	/*** Edit character background ***/
	edit_history();

	/* Get a name, recolor it, prepare savefile */

	get_name();

	/* Display the player */
	display_player(0);

	/*** Finish up ***/

	/* Prompt for it */
#ifdef JP
	prt("[ 'Q' ����, 'S' ��ᤫ��, Enter �����೫�� ]", 23, 14);
#else
	prt("['Q'uit, 'S'tart over, or Enter to continue]", 23, 10);
#endif


	/* Get a key */
	c = inkey();

	/* Quit */
	if (c == 'Q') birth_quit();

	/* Start over */
	if (c == 'S') return (FALSE);


	/* Initialize random quests */
	init_dungeon_quests();

	/* Save character data for quick start */
	save_prev_data(&previous_char);
	previous_char.quick_ok = TRUE;

	/* Process the player name */
	process_player_name(FALSE);

	/* Accept */
	return (TRUE);
}


/*
 * Helper function for 'player_birth()'
 */
static bool player_birth_quick(bool prepare_astral)
{
	runeweapon_type *runeweapon = &runeweapon_list[1];
	bool illegal_generated = FALSE;

	if (prepare_astral)
	{
		/* Clear screen */
		Term_clear();

		illegal_generated = (runeweapon->status & RW_STATUS_ILLEGAL) ? TRUE : FALSE;

#ifdef JP
		put_str("�����ȥ��⡼�ɤ������Ǥ��ޤ���", 2, 2);
		put_str("(���ʥåץɥ饴�������������ƻ�Ԥε��¤�98�����饹������)", 3, 2);

		if (illegal_generated)
			c_put_str(TERM_L_RED, "�����ʥ��ʥåץɥ饴�����ʤΤǥ������ˤϵ�Ͽ����ޤ���", 7, 2);

		put_str("�������ޤ�����[y/n, 'Q'��λ]", 5, 2);
#else
		put_str("You can enter astral mode.", 2, 2);
		put_str("(Start from dungeon level 98 of Death Palace with Runeweapon)", 3, 2);

		if (illegal_generated)
			c_put_str(TERM_L_RED, "Illegal snap dragon weapon, so your score will not be recorded.", 7, 2);

		put_str("Enter astral mode? [y/n, 'Q' Quit]", 5, 2);
#endif
	}
	else
	{
		if (!previous_char.quick_ok) return FALSE;

		/* Clear screen */
		Term_clear();

		/* Extra info */
#ifdef JP
		put_str("�����å����������Ȥ�Ȥ��Ȱ���������Ʊ������饯�����ǻϤ���ޤ���", 2, 2);
		put_str("�����å����������Ȥ�Ȥ��ޤ�����[y/n, 'Q'��λ]", 4, 2);
#else
		put_str("Do you want to use the quick start function(same character as your last one).", 2, 2);
		put_str("Use quick start? [y/n, 'Q' Quit]", 4, 2);
#endif
	}

	/* Choose */
	while (1)
	{
		switch (inkey())
		{
		case 'Q':
			quit(NULL);

		case 'Y':
		case 'y':
			load_prev_data(FALSE);

			if (prepare_astral)
			{
				astral_mode = TRUE;

				/* Create "the Body of Ancestor" */
				create_runeweapon(1);

				/* Special history for astral mode */
				get_history();
			}

			init_dungeon_quests();
			init_turn();

			sp_ptr = &sex_info[p_ptr->psex];
			rp_ptr = &race_info[p_ptr->prace];
			cp_ptr = &class_info[p_ptr->pclass];
			mp_ptr = &m_info[p_ptr->pclass];
			p_ptr->s_ptr = &s_info[p_ptr->pclass];

			/* Calc hitdie, but don't roll */
			get_extra(FALSE);

			/* Calculate the bonuses and hitpoints */
			p_ptr->update |= (PU_BONUS | PU_HP | PU_MANA | PU_GOLD);

			/* Update stuff */
			update_stuff();

			/* Fully healed */
			p_ptr->chp = p_ptr->mhp;

			/* Fully rested */
			p_ptr->csp = p_ptr->msp;

			/* Process the player name */
			process_player_name(FALSE);

			if (prepare_astral)
			{
				int  i;
				char tmp_str[80];

				/* Clear screen */
				Term_clear();

				/* Clear the text */
				for (i = 0; i < 10; i++)
				{
					quest_text[i][0] = '\0';
				}

				quest_text_line = 0;

				/* Get the quest text */
				init_flags = INIT_SHOW_TEXT | INIT_ASSIGN;
				p_ptr->inside_quest = QUEST_RUNEWEAPON;
				process_dungeon_file("q_info.txt", 0, 0, 0, 0);
				p_ptr->inside_quest = 0;

				sprintf(tmp_str, "%s (%s)", runeweapon->ancestor, race_info[runeweapon->race].title);
				prt(tmp_str, 2, 1);

				prt(quest[QUEST_RUNEWEAPON].name, 7, 0);

				for (i = 0; i < 10; i++)
				{
					c_put_str(TERM_YELLOW, quest_text[i], i + 8, 0);
				}

				/* Wait for response */
				pause_line(20);

				if (illegal_generated)
				{
					/* Mark savefile */
					p_ptr->noscore |= 0x0002;
				}
			}

			/* Generated */
			return TRUE;

		case 'N':
		case 'n':
			/* Don't use quick start */
			return FALSE;

		case '?':
#ifdef JP
			show_help("jbirth.txt#QuickStart");
#else
			show_help("birth.txt#QuickStart");
#endif
			break;
		}
	}

	/* NOTREACHED */
}


/*
 * Create a new character.
 *
 * Note that we may be called with "junk" leftover in the various
 * fields, so we must be sure to clear them first.
 */
void player_birth(void)
{
	int i, j;
	char buf[80];
	bool allow_astral_mode;
	s32b ancestor_au[MAX_GOLD + 1];

	playtime = 0;

	/* 
	 * Wipe monsters in old dungeon
	 * This wipe destroys value of m_list[].cur_num .
	 */
	wipe_m_list();

	process_runeweapon_list();
	allow_astral_mode = astral_mode;
	astral_mode = FALSE;

	if (allow_astral_mode)
	{
		object_type *o_ptr;

		/* Preserve gold of ancestor */
		for (i = 0; i <= MAX_GOLD; i++)
		{
			ancestor_au[i] = p_ptr->au[i];
		}

		/* Allocate it */
		C_MAKE(ancestor_inventory, INVEN_TOTAL, object_type);
		ancestor_inven_cnt = 0;
		for (i = 0; i < INVEN_TOTAL; i++)
		{
			o_ptr = &inventory[i];

			if (!o_ptr->k_idx) continue;

			if (object_is_runeweapon(o_ptr) && o_ptr->art_name && (o_ptr->xtra3 > 0)) continue;

			/* Hack -- memorize it */
			o_ptr->marked |= OM_FOUND;

			object_copy(&ancestor_inventory[ancestor_inven_cnt], o_ptr);
			ancestor_inven_cnt++;
		}
		/* If no items are in the inventory, free the array */
		if (!ancestor_inven_cnt) C_KILL(ancestor_inventory, INVEN_TOTAL, object_type);
	}

	/* Create a new character */
	while (1)
	{
		/* Wipe the player */
		player_wipe();

		/* Astral mode creation */
		if (allow_astral_mode)
		{
			if (player_birth_quick(TRUE)) break;
		}

		/* Quick creation */
		if (player_birth_quick(FALSE)) break;

		/* Roll up a new character */
		if (player_birth_full()) break;
	}

	/* Note player birth in the message recall */
	message_add(" ");
	message_add("  ");
	message_add("====================");
	message_add(" ");
	message_add("  ");

#ifdef JP
	do_cmd_write_nikki(NIKKI_GAMESTART, 1, "-------- ���������೫�� --------");
#else
	do_cmd_write_nikki(NIKKI_GAMESTART, 1, "-------- Start New Game --------");
#endif
	do_cmd_write_nikki(NIKKI_HIGAWARI, 0, NULL);

#ifdef JP
	sprintf(buf,"                            ���̤�%s�����򤷤���", sex_info[p_ptr->psex].title);
#else
	sprintf(buf,"                            choose %s personality.", sex_info[p_ptr->psex].title);
#endif
	do_cmd_write_nikki(NIKKI_BUNSHOU, 1, buf);

#ifdef JP
	sprintf(buf,"                            ��²��%s�����򤷤���", race_info[p_ptr->prace].title);
#else
	sprintf(buf,"                            choose %s race.", race_info[p_ptr->prace].title);
#endif
	do_cmd_write_nikki(NIKKI_BUNSHOU, 1, buf);

#ifdef JP
	sprintf(buf,"                            ���饹��%s�����򤷤���", class_info[p_ptr->pclass].title);
#else
	sprintf(buf,"                            choose %s class.", class_info[p_ptr->pclass].title);
#endif
	do_cmd_write_nikki(NIKKI_BUNSHOU, 1, buf);

#ifdef JP
	sprintf(buf,"                            ������Ȥ�%s�����򤷤���", elem_names[p_ptr->pelem]);
#else
	sprintf(buf,"                            choose %s element.", elem_names[p_ptr->pelem]);
#endif
	do_cmd_write_nikki(NIKKI_BUNSHOU, 1, buf);

	if (p_ptr->noscore)
#ifdef JP
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "�����ʥ���饯���ǳ��Ϥ��ƥ�������Ĥ��ʤ��ʤä���");
#else
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "give up recording score to use illegal character.");
#endif

	/* Default elements of Denim & Vice are dependent on player's element*/
	r_info[MON_VICE].r_elem = get_opposite_elem(p_ptr->pelem);
	r_info[MON_DENIM].r_elem = p_ptr->pelem;

	if (astral_mode)
	{
		for (i = 0; i <= MAX_GOLD; i++) p_ptr->au[i] += ancestor_au[i];
		p_ptr->update |= (PU_GOLD);
		update_stuff();
		move_inventory_to_home();
	}
	else
	{
		/* Free the array */
		if (ancestor_inventory) C_KILL(ancestor_inventory, INVEN_TOTAL, object_type);

		/* Init array for "Monster Stock" */
		(void)C_WIPE(stock_mon, MAX_STOCK_MON, monster_type);

		/* Start with no artifacts made yet */
		for (i = 0; i < max_a_idx; i++)
		{
			artifact_type *a_ptr = &a_info[i];
			a_ptr->cur_num = 0;
		}

		/* Reset the objects */
		k_info_reset();
	}

	/* Init the shops */
	for (i = 1; i < max_towns; i++)
	{
		for (j = 0; j < MAX_STORES; j++)
		{
			if (!astral_mode || (i != 1) || (j != STORE_HOME))
			{
				/* Initialize */
				store_init(i, j);
			}
		}
	}

	/* Generate the random seeds for the wilderness */
	seed_wilderness();

	/* Set the message window flag as default */
	if (!window_flag[1])
		window_flag[1] |= PW_MESSAGE;

	/* Set the inv/equip window flag as default */
	if (!window_flag[2])
		window_flag[2] |= PW_INVEN;
}


void dump_yourself(FILE *fff)
{
	char temp[80*10];
	int i, j;
	cptr t;

	if (!fff) return;

	roff_to_buf(race_jouhou[p_ptr->prace], 78, temp, sizeof temp);
	fprintf(fff, "\n\n");
#ifdef JP
	fprintf(fff, "��²: %s\n", race_info[p_ptr->prace].title);
#else
	fprintf(fff, "Race: %s\n", race_info[p_ptr->prace].title);
#endif
	t = temp;
	for (i = 0; i < 10; i++)
	{
		if (t[0] == 0) break;
		fprintf(fff, "%s\n",t);
		t += strlen(t) + 1;
	}
	roff_to_buf(class_jouhou[p_ptr->pclass], 78, temp, sizeof temp);
	fprintf(fff, "\n");
#ifdef JP
	fprintf(fff, "���饹: %s\n", class_info[p_ptr->pclass].title);
#else
	fprintf(fff, "Class: %s\n", class_info[p_ptr->pclass].title);
#endif
	t = temp;
	for (i = 0; i < 10; i++)
	{
		if (t[0] == 0) break; 
		fprintf(fff, "%s\n",t);
		t += strlen(t) + 1;
	}
	for (i = 1; i <= MAX_REALM; i++)
	{
		if (can_use_realm(i))
		{
			fprintf(fff, "\n");
			roff_to_buf(realm_jouhou[i - 1], 78, temp, sizeof temp);
#ifdef JP
			fprintf(fff, "��ˡ: %s\n", realm_names[i]);
#else
			fprintf(fff, "Realm: %s\n", realm_names[i]);
#endif
			t = temp;
			for (j = 0; j < 10; j++)
			{
				if (t[0] == 0) break;
				fprintf(fff, "%s\n",t);
				t += strlen(t) + 1;
			}
		}
	}

	if (p_ptr->special_blow)
	{
		special_blow_type *sb_ptr;

		for (i = 0; i < MAX_SB + MAX_TEMPLE_SB; i++)
		{
			if (p_ptr->special_blow & (0x00000001L << i))
			{
				if (i >= MAX_SB) sb_ptr = &temple_blow_info[i - MAX_SB];
				else sb_ptr = &special_blow_info[i];

				fprintf(fff, "\n");
				roff_to_buf(sb_ptr->text, 78, temp, sizeof temp);

				if (i >= MAX_SB)
#ifdef JP
					fprintf(fff, "ɬ����: %s (�ƥ�ץ�ʥ��ȥ�٥�: %2d, ������: %2d)\n", sb_ptr->name, sb_ptr->level, sb_ptr->cost);
#else
					fprintf(fff, "Special Blow: %s (Level: %2d, Cost: %2d)\n", sb_ptr->name, sb_ptr->level, sb_ptr->cost);
#endif
				else
#ifdef JP
					fprintf(fff, "ɬ����: %s (��٥�: %2d, ������: %2d)\n", sb_ptr->name, sb_ptr->level, sb_ptr->cost);
#else
					fprintf(fff, "Special Blow: %s (Level: %2d, Cost: %2d)\n", sb_ptr->name, sb_ptr->level, sb_ptr->cost);
#endif

				fprintf(fff, "�о����:");
				for (j = 1; j <= MAX_WT; j++)
				{
					if (weapon_type_bit(j) & sb_ptr->weapon_type) fprintf(fff, " %s", wt_desc[j]);
				}
				fprintf(fff, "\n");
				t = temp;
				for (j = 0; j < 10; j++)
				{
					if (t[0] == 0) break;
					fprintf(fff, "%s\n",t);
					t += strlen(t) + 1;
				}
			}
		}
	}

	if (p_ptr->resurrection_cnt)
	{
#ifdef JP
		fprintf(fff,"\n ��ˡ�䥢���ƥ�ʤɤˤ���������: %d ��\n", p_ptr->resurrection_cnt);
#else
		fprintf(fff,"\n Count of self resurrection by magic, item, etc.: %d times\n", p_ptr->resurrection_cnt);
#endif
	}

	if (p_ptr->materialize_cnt)
	{
#ifdef JP
		fprintf(fff,"\n��ˡ�������󥹥����������: %d ��\n", p_ptr->materialize_cnt);
#else
		fprintf(fff,"\nUnique monster resurrection count: %d times\n", p_ptr->materialize_cnt);
#endif
	}

	if (p_ptr->reincarnate_cnt)
	{
#ifdef JP
#ifdef L64
		fprintf(fff,"\n�꡼�󥫡��ͥ��Ȳ��: %d �� (�ǹ��٥�: %d)\n", p_ptr->reincarnate_cnt, p_ptr->max_max_plv);
#else
		fprintf(fff,"\n�꡼�󥫡��ͥ��Ȳ��: %d �� (�ǹ��٥�: %ld)\n", p_ptr->reincarnate_cnt, p_ptr->max_max_plv);
#endif
#else
#ifdef L64
		fprintf(fff,"\nReincarnation count: %d times (Maximum level: %d)\n", p_ptr->reincarnate_cnt, p_ptr->max_max_plv);
#else
		fprintf(fff,"\nReincarnation count: %d times (Maximum level: %ld)\n", p_ptr->reincarnate_cnt, p_ptr->max_max_plv);
#endif
#endif
	}
}
