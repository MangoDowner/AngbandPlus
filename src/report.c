/* File: report.c */

#define _GNU_SOURCE
#include "angband.h"

#ifdef WORLD_SCORE

#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>

#if defined(WINDOWS)
#include <winsock.h>
#elif defined(MACINTOSH)
#include <OpenTransport.h>
#include <OpenTptInternet.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/time.h>

#include <setjmp.h>
#include <signal.h>
#endif

#ifdef JP
#define SCORE_PATH "http://www.kmc.kyoto-u.ac.jp/~habu/local/hengscore/score.cgi"
#else
#define SCORE_PATH "http://www.kmc.kyoto-u.ac.jp/~habu/local/hengscore-en/score.cgi"
#endif
//#define SCORE_PATH "http://www.kmc.kyoto-u.ac.jp/~habu/local/scoretest/score.cgi"

/*
  simple buffer library
 */

typedef struct _buf{
  size_t max_size;
  size_t size;
  char *data;
} BUF;

#define	BUFSIZE	(65536)

#if defined(WINDOWS) || defined(SUNOS4) || defined(MACINTOSH) || defined(SGI)
#define vasprintf	Vasprintf
#endif

#ifdef SUNOS4
static int Vasprintf(char **buf, const char *fmt, va_list ap)
{
    int ret;

    *buf = malloc(BUFSIZE);

    ret = vsnprintf(*buf, BUFSIZE, fmt, ap);

    return ret;
}
#endif

#if defined(WINDOWS) || defined(MACINTOSH) || defined(SGI)
static int Vasprintf(char **buf, const char *fmt, va_list ap)
{
    int ret;
    *buf = malloc(BUFSIZE * 4);

    ret = vsprintf(*buf, fmt, ap);

    return ret;
}
#endif

static BUF* buf_new(void)
{
    BUF *p;

    p = malloc(sizeof(BUF));
    if((p = malloc(sizeof(BUF))) == NULL)
	return NULL;

    p->size = 0;
    p->max_size = BUFSIZE;
    if((p->data = malloc(BUFSIZE)) == NULL){
	free(p);
	return NULL;
    }
    return p;
}

#if 0
static void buf_delete(BUF *b)
{
    free(b->data);
    free(b);
}
#endif

static int buf_append(BUF *buf, const char *data, size_t size)
{
    while(buf->size + size > buf->max_size){
	char *tmp;
	if((tmp = malloc(buf->max_size * 2)) == NULL) return -1;

	memcpy(tmp, buf->data, buf->max_size);
	free(buf->data);

	buf->data = tmp;

	buf->max_size *= 2;
    }
    memcpy(buf->data + buf->size, data, size);
    buf->size += size;

    return buf->size;
}

static int buf_sprintf(BUF *buf, const char *fmt, ...)
{
    int		ret;
    char	*tmpbuf;
    va_list	ap;

    va_start(ap, fmt);
    vasprintf(&tmpbuf, fmt, ap);
    va_end(ap);

    if(!tmpbuf)
	return -1;

    ret = buf_append(buf, tmpbuf, strlen(tmpbuf));

    free(tmpbuf);

    return ret;
}

#if 0
static int buf_read(BUF *buf, int fd)
{
    int len;
#ifndef MACINTOSH
    char tmp[BUFSIZE];
#else
	char *tmp;
	
	tmp = calloc( BUFSIZE , sizeof(char) );
#endif

    while((len = read(fd, tmp, BUFSIZE)) > 0)
	buf_append(buf, tmp, len);

    return buf->size;
}
#endif

#if 0
static int buf_write(BUF *buf, int fd)
{
    write(fd, buf->data, buf->size);

    return buf->size;
}

static int buf_search(BUF *buf, const char *str)
{
    char *ret;

    ret = strstr(buf->data, str);

    if(!ret)
	return -1;

    return ret - buf->data;
}

static BUF * buf_subbuf(BUF *buf, int pos1, size_t sz)
{
    BUF *ret;

    if(pos1 < 0)
	return NULL;

    ret = buf_new();

    if(sz <= 0)
	sz = buf->size - pos1;

    buf_append(ret, buf->data + pos1, sz);

    return ret;
}
#endif

static void http_post(int sd, char *url, BUF *buf)
{
  BUF *output;

  output = buf_new();
  buf_sprintf(output, "POST %s HTTP/1.0\r\n", url);
  buf_sprintf(output, "User-Agent: Hengband %d.%d.%d\r\n",
	      FAKE_VER_MAJOR-10, FAKE_VER_MINOR, FAKE_VER_PATCH);

  buf_sprintf(output, "Content-Length: %d\r\n", buf->size);
  buf_sprintf(output, "Content-Encoding: binary\r\n");
  buf_sprintf(output, "Content-Type: application/octet-stream\r\n");
  buf_sprintf(output, "\r\n");
  buf_append(output, buf->data, buf->size);

  soc_write(sd, output->data, output->size);
}
  
static void dumpbuf_virtues(BUF* dumpbuf)
{
    int v_nr = 0;

        for (v_nr = 0; v_nr < 8; v_nr++)
        {
                char v_name [20];

                int tester = p_ptr->virtues[v_nr];

                strcpy(v_name, virtue[(p_ptr->vir_types[v_nr])-1]);

                if (p_ptr->vir_types[v_nr] == 0 || p_ptr->vir_types[v_nr] >
                    MAX_VIRTUE)
#ifdef JP
                 buf_sprintf(dumpbuf, "���äȡ�%s�ξ���ʤ���", v_name);
#else
                 buf_sprintf(dumpbuf, "Oops. No info about %s.", v_name);
#endif

                else if (tester < -100)
#ifdef JP
                  buf_sprintf(dumpbuf, "[%s]���ж�",
#else
                  buf_sprintf(dumpbuf, "You are the polar opposite of %s.",
#endif

                   v_name);
                else if (tester < -80)
#ifdef JP
                  buf_sprintf(dumpbuf, "[%s]����Ũ",
#else
                  buf_sprintf(dumpbuf, "You are an arch-enemy of %s.",
#endif

                   v_name);
                else if (tester < -60)
#ifdef JP
                  buf_sprintf(dumpbuf, "[%s]�ζ�Ũ",
#else
                  buf_sprintf(dumpbuf, "You are a bitter enemy of %s.",
#endif

                   v_name);
                else if (tester < -40)
#ifdef JP
                  buf_sprintf(dumpbuf, "[%s]��Ũ",
#else
                  buf_sprintf(dumpbuf, "You are an enemy of %s.",
#endif

                   v_name);
                else if (tester < -20)
#ifdef JP
                  buf_sprintf(dumpbuf, "[%s]�κ��",
#else
                  buf_sprintf(dumpbuf, "You have sinned against %s.",
#endif

                   v_name);
                else if (tester < 0)
#ifdef JP
                  buf_sprintf(dumpbuf, "[%s]����ƻ��",
#else
                  buf_sprintf(dumpbuf, "You have strayed from the path of %s.",
#endif

                   v_name);
                else if (tester == 0)                   
#ifdef JP
                  buf_sprintf(dumpbuf,"[%s]����Ω��",
#else
                  buf_sprintf(dumpbuf,"You are neutral to %s.",
#endif

                   v_name);
                else if (tester < 20)
#ifdef JP
                  buf_sprintf(dumpbuf,"[%s]�ξ�����",
#else
                  buf_sprintf(dumpbuf,"You are somewhat virtuous in %s.",
#endif

                   v_name);
                else if (tester < 40)
#ifdef JP
                  buf_sprintf(dumpbuf,"[%s]��������",
#else
                  buf_sprintf(dumpbuf,"You are virtuous in %s.",
#endif

                   v_name);
                else if (tester < 60)
#ifdef JP
                  buf_sprintf(dumpbuf,"[%s]�ι�����",
#else
                  buf_sprintf(dumpbuf,"You are very virtuous in %s.",
#endif

                   v_name);
                else if (tester < 80)
#ifdef JP
                  buf_sprintf(dumpbuf,"[%s]���Ƽ�",
#else
                  buf_sprintf(dumpbuf,"You are a champion of %s.",
#endif

                   v_name);
                else if (tester < 100)
#ifdef JP
                  buf_sprintf(dumpbuf,"[%s]�ΰ�����Ƽ�",
#else
                  buf_sprintf(dumpbuf,"You are a great champion of %s.",
#endif

                   v_name);
                else
#ifdef JP
                  buf_sprintf(dumpbuf,"[%s]�ζ񸽼�",
#else
                  buf_sprintf(dumpbuf,"You are the living embodiment of %s.",
#endif

                   v_name);

            buf_sprintf(dumpbuf, "\n");
        }


}


void dumpbuf_mutations(BUF* dumpbuf)
{
	if (p_ptr->muta1)
	{
		if (p_ptr->muta1 & MUT1_SPIT_ACID)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��ϻ���᤭�����뤳�Ȥ��Ǥ��롣(���᡼�� ��٥�X1)\n");
#else
			buf_sprintf(dumpbuf, " You can spit acid (dam lvl).\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_BR_FIRE)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��ϱ�Υ֥쥹���Ǥ����Ȥ��Ǥ��롣(���᡼�� ��٥�X2)\n");
#else
			buf_sprintf(dumpbuf, " You can breathe fire (dam lvl * 2).\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_HYPN_GAZE)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ����ˤߤϺ�̲���̤��ġ�\n");
#else
			buf_sprintf(dumpbuf, " Your gaze is hypnotic.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_TELEKINES)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ���ǰư�Ϥ��äƤ��롣\n");
#else
			buf_sprintf(dumpbuf, " You are telekinetic.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_VTELEPORT)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��ϼ�ʬ�ΰջפǥƥ�ݡ��ȤǤ��롣\n");
#else
			buf_sprintf(dumpbuf, " You can teleport at will.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_MIND_BLST)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ���Ũ����������Ǥ��롣\n");
#else
			buf_sprintf(dumpbuf, " You can Mind Blast your enemies.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_RADIATION)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��ϼ�ʬ�ΰջפ�����ǽ��ȯ�����뤳�Ȥ��Ǥ��롣\n");
#else
			buf_sprintf(dumpbuf, " You can emit hard radiation at will.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_VAMPIRISM)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��ϵ۷쵴�Τ褦��Ũ������̿�Ϥ�ۼ����뤳�Ȥ��Ǥ��롣\n");
#else
			buf_sprintf(dumpbuf, " You can drain life from a foe like a vampire.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_SMELL_MET)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��϶᤯�ˤ��뵮��°�򤫤�ʬ���뤳�Ȥ��Ǥ��롣\n");
#else
			buf_sprintf(dumpbuf, " You can smell nearby precious metal.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_SMELL_MON)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��϶᤯�Υ�󥹥�����¸�ߤ򤫤�ʬ���뤳�Ȥ��Ǥ��롣\n");
#else
			buf_sprintf(dumpbuf, " You can smell nearby monsters.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_BLINK)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ���û����Υ��ƥ�ݡ��ȤǤ��롣\n");
#else
			buf_sprintf(dumpbuf, " You can teleport yourself short distances.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_EAT_ROCK)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��ϹŤ���򿩤٤뤳�Ȥ��Ǥ��롣\n");
#else
			buf_sprintf(dumpbuf, " You can consume solid rock.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_SWAP_POS)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ���¾�μԤȾ��������ؤ�뤳�Ȥ��Ǥ��롣\n");
#else
			buf_sprintf(dumpbuf, " You can switch locations with another being.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_SHRIEK)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��ϿȤ��Ӥ����Ķ�������ȯ���뤳�Ȥ��Ǥ��롣\n");
#else
			buf_sprintf(dumpbuf, " You can emit a horrible shriek.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_ILLUMINE)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ������뤤�������Ĥ��Ȥ��Ǥ��롣\n");
#else
			buf_sprintf(dumpbuf, " You can emit bright light.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_DET_CURSE)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��ϼٰ�����ˡ�δ��򴶤��Ȥ뤳�Ȥ��Ǥ��롣\n");
#else
			buf_sprintf(dumpbuf, " You can feel the danger of evil magic.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_BERSERK)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��ϼ�ʬ�ΰջפǶ�����Ʈ���֤ˤʤ뤳�Ȥ��Ǥ��롣\n");
#else
			buf_sprintf(dumpbuf, " You can drive yourself into a berserk frenzy.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_POLYMORPH)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��ϼ�ʬ�ΰջ֤��Ѳ��Ǥ��롣\n");
#else
			buf_sprintf(dumpbuf, " You can polymorph yourself at will.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_MIDAS_TCH)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ����̾異���ƥ�����Ѥ��뤳�Ȥ��Ǥ��롣\n");
#else
			buf_sprintf(dumpbuf, " You can turn ordinary items to gold.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_GROW_MOLD)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��ϼ��Ϥ˥��Υ������䤹���Ȥ��Ǥ��롣\n");
#else
			buf_sprintf(dumpbuf, " You can cause mold to grow near you.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_RESIST)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��ϸ��Ǥι�����Ф��ƿȤ�Ť����뤳�Ȥ��Ǥ��롣\n");
#else
			buf_sprintf(dumpbuf, " You can harden yourself to the ravages of the elements.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_EARTHQUAKE)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��ϼ��ϤΥ��󥸥������������뤳�Ȥ��Ǥ��롣\n");
#else
			buf_sprintf(dumpbuf, " You can bring down the dungeon around your ears.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_EAT_MAGIC)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ�����ˡ�Υ��ͥ륮����ʬ��ʪ�Ȥ��ƻ��ѤǤ��롣\n");
#else
			buf_sprintf(dumpbuf, " You can consume magic energy for your own use.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_WEIGH_MAG)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��ϼ�ʬ�˱ƶ���Ϳ������ˡ���Ϥ򴶤��뤳�Ȥ��Ǥ��롣\n");
#else
			buf_sprintf(dumpbuf, " You can feel the strength of the magics affecting you.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_STERILITY)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��Ͻ���Ū������ǽ�򵯤������Ȥ��Ǥ��롣\n");
#else
			buf_sprintf(dumpbuf, " You can cause mass impotence.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_PANIC_HIT)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��Ϲ��⤷����Ȥ��뤿��ƨ���뤳�Ȥ��Ǥ��롣\n");
#else
			buf_sprintf(dumpbuf, " You can run for your life after hitting something.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_DAZZLE)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��Ϻ�������ܤ��������������ǽ��ȯ�����뤳�Ȥ��Ǥ��롣 \n");
#else
			buf_sprintf(dumpbuf, " You can emit confusing, blinding radiation.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_LASER_EYE)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ����ܤ���졼����������ȯ�ͤ��뤳�Ȥ��Ǥ��롣\n");
#else
			buf_sprintf(dumpbuf, " Your eyes can fire laser beams.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_RECALL)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��ϳ��ȥ��󥸥��δ֤�Ԥ��褹�뤳�Ȥ��Ǥ��롣\n");
#else
			buf_sprintf(dumpbuf, " You can travel between town and the depths.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_BANISH)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��ϼٰ��ʥ�󥹥������Ϲ�����Ȥ����Ȥ��Ǥ��롣\n");
#else
			buf_sprintf(dumpbuf, " You can send evil creatures directly to Hell.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_COLD_TOUCH)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ���ʪ�򿨤ä���餻�뤳�Ȥ��Ǥ��롣\n");
#else
			buf_sprintf(dumpbuf, " You can freeze things with a touch.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_LAUNCHER)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��ϥ����ƥ���϶����ꤲ�뤳�Ȥ��Ǥ��롣\n");
#else
			buf_sprintf(dumpbuf, " You can hurl objects with great force.\n");
#endif

		}
	}

	if (p_ptr->muta2)
	{
		if (p_ptr->muta2 & MUT2_BERS_RAGE)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��϶���β���ȯ��򵯤�����\n");
#else
			buf_sprintf(dumpbuf, " You are subject to berserker fits.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_COWARDICE)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��ϻ������¤ˤʤ롣\n");
#else
			buf_sprintf(dumpbuf, " You are subject to cowardice.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_RTELEPORT)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��ϥ�����˥ƥ�ݡ��Ȥ��롣\n");
#else
			buf_sprintf(dumpbuf, " You are teleporting randomly.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_ALCOHOL)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ����Τϥ��륳�����ʬ�礹�롣\n");
#else
			buf_sprintf(dumpbuf, " Your body produces alcohol.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_HALLU)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��ϸ��Ф������������������˿�����Ƥ��롣\n");
#else
			buf_sprintf(dumpbuf, " You have a hallucinatory insanity.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_FLATULENT)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ�������Ǥ��ʤ����������򤳤���\n");
#else
			buf_sprintf(dumpbuf, " You are subject to uncontrollable flatulence.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_PROD_MANA)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ���������ǽ����ˡ�Υ��ͥ륮����ȯ���Ƥ��롣\n");
#else
			buf_sprintf(dumpbuf, " You are producing magical energy uncontrollably.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_ATT_DEMON)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��ϥǡ���������Ĥ��롣\n");
#else
			buf_sprintf(dumpbuf, " You attract demons.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_SCOR_TAIL)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��ϥ�����ο����������Ƥ��롣(�ǡ����᡼�� 3d7)\n");
#else
			buf_sprintf(dumpbuf, " You have a scorpion tail (poison, 3d7).\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_HORNS)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��ϳѤ������Ƥ��롣(���᡼�� 2d6)\n");
#else
			buf_sprintf(dumpbuf, " You have horns (dam. 2d6).\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_BEAK)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��ϥ����Х��������Ƥ��롣(���᡼�� 2d4)\n");
#else
			buf_sprintf(dumpbuf, " You have a beak (dam. 2d4).\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_SPEED_FLUX)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��ϥ�������᤯ư�������٤�ư�����ꤹ�롣\n");
#else
			buf_sprintf(dumpbuf, " You move faster or slower randomly.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_BANISH_ALL)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��ϻ����᤯�Υ�󥹥�������Ǥ����롣\n");
#else
			buf_sprintf(dumpbuf, " You sometimes cause nearby creatures to vanish.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_EAT_LIGHT)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��ϻ������Ϥθ���ۼ����Ʊ��ܤˤ��롣\n");
#else
			buf_sprintf(dumpbuf, " You sometimes feed off of the light around you.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_TRUNK)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��ϾݤΤ褦��ɡ����äƤ��롣(���᡼�� 1d4)\n");
#else
			buf_sprintf(dumpbuf, " You have an elephantine trunk (dam 1d4).\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_ATT_ANIMAL)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ���ưʪ������Ĥ��롣\n");
#else
			buf_sprintf(dumpbuf, " You attract animals.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_TENTACLES)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��ϼٰ��ʿ������äƤ��롣(���᡼�� 2d5)\n");
#else
			buf_sprintf(dumpbuf, " You have evil looking tentacles (dam 2d5).\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_RAW_CHAOS)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��Ϥ��Ф��н㥫��������ޤ�롣\n");
#else
			buf_sprintf(dumpbuf, " You occasionally are surrounded with raw chaos.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_NORMALITY)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ����Ѱۤ��Ƥ��������������Ƥ��Ƥ��롣\n");
#else
			buf_sprintf(dumpbuf, " You may be mutated, but you're recovering.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_WRAITH)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ������Τ�ͩ�β���������β������ꤹ�롣\n");
#else
			buf_sprintf(dumpbuf, " You fade in and out of physical reality.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_POLY_WOUND)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��η򹯤ϥ��������Ϥ˱ƶ�������롣\n");
#else
			buf_sprintf(dumpbuf, " Your health is subject to chaotic forces.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_WASTING)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��Ͽ�夹�붲�����µ��ˤ����äƤ��롣\n");
#else
			buf_sprintf(dumpbuf, " You have a horrible wasting disease.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_ATT_DRAGON)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��ϥɥ饴�������Ĥ��롣\n");
#else
			buf_sprintf(dumpbuf, " You attract dragons.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_WEIRD_MIND)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��������ϥ�����˳��礷����̾������ꤷ�Ƥ��롣\n");
#else
			buf_sprintf(dumpbuf, " Your mind randomly expands and contracts.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_NAUSEA)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��ΰߤ���������夭���ʤ���\n");
#else
			buf_sprintf(dumpbuf, " You have a seriously upset stomach.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_CHAOS_GIFT)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��ϥ������μ��⤫��˫���򤦤��Ȥ롣\n");
#else
			buf_sprintf(dumpbuf, " Chaos deities give you gifts.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_WALK_SHAD)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��Ϥ��Ф���¾�Ρֱơפ��¤����ࡣ\n");
#else
			buf_sprintf(dumpbuf, " You occasionally stumble into other shadows.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_WARNING)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ���Ũ�˴ؤ���ٹ�򴶤��롣\n");
#else
			buf_sprintf(dumpbuf, " You receive warnings about your foes.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_INVULN)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��ϻ����餱�Τ餺�ʵ�ʬ�ˤʤ롣\n");
#else
			buf_sprintf(dumpbuf, " You occasionally feel invincible.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_SP_TO_HP)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��ϻ����줬�����ˤɤä�ή��롣\n");
#else
			buf_sprintf(dumpbuf, " Your blood sometimes rushes to your muscles.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_HP_TO_SP)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��ϻ���Ƭ�˷줬�ɤä�ή��롣\n");
#else
			buf_sprintf(dumpbuf, " Your blood sometimes rushes to your head.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_DISARM)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��Ϥ褯�ĤޤŤ���ʪ����Ȥ���\n");
#else
			buf_sprintf(dumpbuf, " You occasionally stumble and drop things.\n");
#endif

		}
	}

	if (p_ptr->muta3)
	{
		if (p_ptr->muta3 & MUT3_HYPER_STR)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ���Ķ��Ū�˶�����(����+4)\n");
#else
			buf_sprintf(dumpbuf, " You are superhumanly strong (+4 STR).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_PUNY)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��ϵ������(����-4)\n");
#else
			buf_sprintf(dumpbuf, " You are puny (-4 STR).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_HYPER_INT)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ���Ǿ�����Υ���ԥ塼������(��ǽ������+4)\n");
#else
			buf_sprintf(dumpbuf, " Your brain is a living computer (+4 INT/WIS).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_MORONIC)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��������������(��ǽ������-4)\n");
#else
			buf_sprintf(dumpbuf, " You are moronic (-4 INT/WIS).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_RESILIENT)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ����Τ����������٤�Ǥ��롣(�ѵ�+4)\n");
#else
			buf_sprintf(dumpbuf, " You are very resilient (+4 CON).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_XTRA_FAT)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��϶�ü�����äƤ��롣(�ѵ�+2,���ԡ���-2)\n");
#else
			buf_sprintf(dumpbuf, " You are extremely fat (+2 CON, -2 speed).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_ALBINO)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��ϥ���ӥΤ���(�ѵ�-4)\n");
#else
			buf_sprintf(dumpbuf, " You are albino (-4 CON).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_FLESH_ROT)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ������Τ����Ԥ��Ƥ��롣(�ѵ�-2,̥��-1)\n");
#else
			buf_sprintf(dumpbuf, " Your flesh is rotting (-2 CON, -1 CHR).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_SILLY_VOI)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ������ϴ�ȴ���ʥ�������������(̥��-4)\n");
#else
			buf_sprintf(dumpbuf, " Your voice is a silly squeak (-4 CHR).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_BLANK_FAC)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��ϤΤäڤ�ܤ�����(̥��-1)\n");
#else
			buf_sprintf(dumpbuf, " Your face is featureless (-1 CHR).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_ILL_NORM)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��ϸ��Ƥ�ʤ���Ƥ��롣\n");
#else
			buf_sprintf(dumpbuf, " Your appearance is masked with illusion.\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_XTRA_EYES)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ���;ʬ����Ĥ��ܤ���äƤ��롣(õ��+15)\n");
#else
			buf_sprintf(dumpbuf, " You have an extra pair of eyes (+15 search).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_MAGIC_RES)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ�����ˡ�ؤ��������äƤ��롣\n");
#else
			buf_sprintf(dumpbuf, " You are resistant to magic.\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_XTRA_NOIS)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ����Ѥʲ���ȯ���Ƥ��롣(��̩-3)\n");
#else
			buf_sprintf(dumpbuf, " You make a lot of strange noise (-3 stealth).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_INFRAVIS)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��������餷���ֳ������Ϥ���äƤ��롣(+3)\n");
#else
			buf_sprintf(dumpbuf, " You have remarkable infravision (+3).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_XTRA_LEGS)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ���;ʬ�����ܤ�­�������Ƥ��롣(��®+3)\n");
#else
			buf_sprintf(dumpbuf, " You have an extra pair of legs (+3 speed).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_SHORT_LEG)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ���­��û���͵�����(��®-3)\n");
#else
			buf_sprintf(dumpbuf, " Your legs are short stubs (-3 speed).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_ELEC_TOUC)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��η�ɤˤ���ή��ή��Ƥ��롣\n");
#else
			buf_sprintf(dumpbuf, " Electricity is running through your veins.\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_FIRE_BODY)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ����Τϱ�ˤĤĤޤ�Ƥ��롣\n");
#else
			buf_sprintf(dumpbuf, " Your body is enveloped in flames.\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_WART_SKIN)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ���ȩ�ϥ��ܤ�����Ƥ��롣(̥��-2, AC+5)\n");
#else
			buf_sprintf(dumpbuf, " Your skin is covered with warts (-2 CHR, +5 AC).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_SCALES)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ���ȩ���ڤˤʤäƤ��롣(̥��-1, AC+10)\n");
#else
			buf_sprintf(dumpbuf, " Your skin has turned into scales (-1 CHR, +10 AC).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_IRON_SKIN)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ���ȩ��Ŵ�ǤǤ��Ƥ��롣(����-1, AC+25)\n");
#else
			buf_sprintf(dumpbuf, " Your skin is made of steel (-1 DEX, +25 AC).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_WINGS)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��ϱ�����äƤ��롣\n");
#else
			buf_sprintf(dumpbuf, " You have wings.\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_FEARLESS)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ����������ݤ򴶤��ʤ���\n");
#else
			buf_sprintf(dumpbuf, " You are completely fearless.\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_REGEN)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��ϵ�®�˲������롣\n");
#else
			buf_sprintf(dumpbuf, " You are regenerating.\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_ESP)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��ϥƥ�ѥ�������äƤ��롣\n");
#else
			buf_sprintf(dumpbuf, " You are telepathic.\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_LIMBER)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ����Τ����ˤ��ʤ䤫����(����+3)\n");
#else
			buf_sprintf(dumpbuf, " Your body is very limber (+3 DEX).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_ARTHRITIS)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��Ϥ��Ĥ������ˤߤ򴶤��Ƥ��롣(����-3)\n");
#else
			buf_sprintf(dumpbuf, " Your joints ache constantly (-3 DEX).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_VULN_ELEM)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��ϸ��Ǥι���˼夤��\n");
#else
			buf_sprintf(dumpbuf, " You are susceptible to damage from the elements.\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_MOTION)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ���ư������Τ��϶�����(��̩+1)\n");
#else
			buf_sprintf(dumpbuf, " Your movements are precise and forceful (+1 STL).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_GOOD_LUCK)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ����򤤥�����ˤĤĤޤ�Ƥ��롣\n");
#else
			buf_sprintf(dumpbuf, " There is a white aura surrounding you.\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_BAD_LUCK)
		{
#ifdef JP
buf_sprintf(dumpbuf, " ���ʤ��Ϲ���������ˤĤĤޤ�Ƥ��롣\n");
#else
			buf_sprintf(dumpbuf, " There is a black aura surrounding you.\n");
#endif

		}
	}
}


/* ����饯������פ��ä� BUF����¸ */
static errr make_dump(BUF* dumpbuf)
{
	int		i, x, y;
	byte		a;
	char		c;
	cptr		paren = ")";
	store_type  *st_ptr;
	char		o_name[MAX_NLEN];
	char		buf[1024];
	cptr            disp_align;

#ifndef FAKE_VERSION
	/* Begin dump */
	buf_sprintf(dumpbuf, "  [Angband %d.%d.%d Character Dump]\n\n",
	        VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
#else
#ifdef JP
	buf_sprintf(dumpbuf, "  [�Ѷ����� %d.%d.%d ����饯������]\n\n",
	        FAKE_VER_MAJOR-10, FAKE_VER_MINOR, FAKE_VER_PATCH);
#else
	buf_sprintf(dumpbuf, "  [Hengband %d.%d.%d Character Dump]\n\n",
	        FAKE_VER_MAJOR-10, FAKE_VER_MINOR, FAKE_VER_PATCH);
#endif

#endif

	update_playtime();

	/* Display player */
	display_player(0);

	/* Dump part of the screen */
	for (y = 1; y < 22; y++)
	{
		/* Dump each row */
		for (x = 0; x < 79; x++)
		{
			/* Get the attr/char */
			(void)(Term_what(x, y, &a, &c));

			/* Dump it */
			buf[x] = c;
		}

		/* End the string */
		buf[x] = '\0';

		/* Kill trailing spaces */
		while ((x > 0) && (buf[x-1] == ' ')) buf[--x] = '\0';

		/* End the row */
#ifdef JP
                        buf_sprintf(dumpbuf, "%s\n", buf);
#else
		buf_sprintf(dumpbuf, "%s\n", buf);
#endif

	}

	/* Display history */
	display_player(1);

	/* Dump part of the screen */
	for (y = 16; y < 21; y++)
	{
		/* Dump each row */
		for (x = 0; x < 79; x++)
		{
			/* Get the attr/char */
			(void)(Term_what(x, y, &a, &c));

			/* Dump it */
			buf[x] = c;
		}

		/* End the string */
		buf[x] = '\0';

		/* Kill trailing spaces */
		while ((x > 0) && (buf[x-1] == ' ')) buf[--x] = '\0';

		/* End the row */
		buf_sprintf(dumpbuf, "%s\n", buf);
	}

	buf_sprintf(dumpbuf, "\n");
	/* Display history */
	display_player(2);

	/* Dump part of the screen */
	for (y = 3; y < 22; y++)
	{
		/* Dump each row */
		for (x = 0; x < 79; x++)
		{
			/* Get the attr/char */
			(void)(Term_what(x, y, &a, &c));

			/* Dump it */
			buf[x] = c;
		}

		/* End the string */
		buf[x] = '\0';

		/* Kill trailing spaces */
		while ((x > 0) && (buf[x-1] == ' ')) buf[--x] = '\0';

		/* End the row */
		buf_sprintf(dumpbuf, "%s\n", buf);
	}


	for (i = 0; i < p_ptr->count / 80; i++)
		buf_sprintf(dumpbuf, " ");
	buf_sprintf(dumpbuf, "\n");
	for (i = 0; i < p_ptr->count % 80; i++)
		buf_sprintf(dumpbuf, " ");
	{
		bool pet = FALSE;

		for (i = m_max - 1; i >= 1; i--)
		{
			monster_type *m_ptr = &m_list[i];
			char pet_name[80];

			if (!m_ptr->r_idx) continue;
			if (!is_pet(m_ptr)) continue;
			if (!m_ptr->nickname && (p_ptr->jouba != i)) continue;
			if (!pet)
			{
#ifdef JP
buf_sprintf(dumpbuf, "\n  [��ʥڥå�]\n\n");
#else
		buf_sprintf(dumpbuf, "\n\n  [leading pets]\n\n");
#endif
				pet = TRUE;
			}
			monster_desc(pet_name, m_ptr, 0x88);
			buf_sprintf(dumpbuf, "%s", pet_name);
			if (p_ptr->jouba == i)
#ifdef JP
				buf_sprintf(dumpbuf, " ������");
#else
				buf_sprintf(dumpbuf, " riding");
#endif
			buf_sprintf(dumpbuf, "\n");
		}
		if (pet) buf_sprintf(dumpbuf, "\n");
	}

	if (death && !total_winner)
	{
#ifdef JP
buf_sprintf(dumpbuf, "\n  [���ľ���Υ�å�����]\n\n");
#else
		buf_sprintf(dumpbuf, "\n\n  [Last messages]\n\n");
#endif
		for (i = MIN(message_num(), 15); i >= 0; i--)
		{
			buf_sprintf(dumpbuf,"> %s\n",message_str((s16b)i));
		}
		buf_sprintf(dumpbuf, "\n");
	}

#ifdef JP
buf_sprintf(dumpbuf, "\n  [����¾�ξ���]       \n");
#else
	buf_sprintf(dumpbuf, "\n  [Miscellaneous information]\n");
#endif


#ifdef JP
	buf_sprintf(dumpbuf, "\n ���Ծ��:\n");
#else
        buf_sprintf(dumpbuf, "\n Recall Depth:\n");
#endif
	for (y = 1; y < max_d_idx; y++)
	{
		bool seiha = FALSE;

		if (!d_info[y].maxdepth) continue;
		if (!max_dlv[y]) continue;
		if (d_info[y].final_guardian)
		{
			if (!r_info[d_info[y].final_guardian].max_num) seiha = TRUE;
		}
		else if (max_dlv[y] == d_info[y].maxdepth) seiha = TRUE;

#ifdef JP
		buf_sprintf(dumpbuf, "   %c%-12s: %3d ��\n", seiha ? '!' : ' ', d_name+d_info[y].name, max_dlv[y]);
#else
		buf_sprintf(dumpbuf, "   %c%-12s: level %3d\n", seiha ? '!' : ' ', d_name+d_info[y].name, max_dlv[y]);
#endif
	}

	if (preserve_mode)
#ifdef JP
buf_sprintf(dumpbuf, "\n ��¸�⡼��:         ON");
#else
		buf_sprintf(dumpbuf, "\n Preserve Mode:      ON");
#endif

	else
#ifdef JP
buf_sprintf(dumpbuf, "\n ��¸�⡼��:         OFF");
#else
		buf_sprintf(dumpbuf, "\n Preserve Mode:      OFF");
#endif


	if (ironman_autoscum)
#ifdef JP
	        buf_sprintf(dumpbuf, "\n ��ư���깥��  :     ALWAYS");
#else
		buf_sprintf(dumpbuf, "\n Autoscum:           ALWAYS");
#endif

	else if (auto_scum)
#ifdef JP
	        buf_sprintf(dumpbuf, "\n ��ư���깥��  :     ON");
#else
		buf_sprintf(dumpbuf, "\n Autoscum:           ON");
#endif

	else
#ifdef JP
	        buf_sprintf(dumpbuf, "\n ��ư���깥��  :     OFF");
#else
		buf_sprintf(dumpbuf, "\n Autoscum:           OFF");
#endif


	if (ironman_small_levels)
#ifdef JP
buf_sprintf(dumpbuf, "\n ���������󥸥��:   ALWAYS");
#else
		buf_sprintf(dumpbuf, "\n Small Levels:       ALWAYS");
#endif

	else if (always_small_levels)
#ifdef JP
buf_sprintf(dumpbuf, "\n ���������󥸥��:   ON");
#else
		buf_sprintf(dumpbuf, "\n Small Levels:       ON");
#endif

	else if (small_levels)
#ifdef JP
buf_sprintf(dumpbuf, "\n ���������󥸥��:   ENABLED");
#else
		buf_sprintf(dumpbuf, "\n Small Levels:       ENABLED");
#endif

	else
#ifdef JP
buf_sprintf(dumpbuf, "\n ���������󥸥��:   OFF");
#else
		buf_sprintf(dumpbuf, "\n Small Levels:       OFF");
#endif


	if (vanilla_town)
#ifdef JP
buf_sprintf(dumpbuf, "\n ���Ĥ�Į�Τ�: ON");
#else
		buf_sprintf(dumpbuf, "\n Vanilla Town:       ON");
#endif

	else if (lite_town)
#ifdef JP
buf_sprintf(dumpbuf, "\n �����Ϥ�Į:         ON");
#else
		buf_sprintf(dumpbuf, "\n Lite Town:          ON");
#endif


	if (ironman_shops)
#ifdef JP
buf_sprintf(dumpbuf, "\n Ź�ʤ�:             ON");
#else
		buf_sprintf(dumpbuf, "\n No Shops:           ON");
#endif


	if (ironman_downward)
#ifdef JP
buf_sprintf(dumpbuf, "\n ���ʤ�夬��ʤ�:   ON");
#else
		buf_sprintf(dumpbuf, "\n Diving only:        ON");
#endif


	if (ironman_rooms)
#ifdef JP
buf_sprintf(dumpbuf, "\n ���̤Ǥʤ�����������:         ON");
#else
		buf_sprintf(dumpbuf, "\n Unusual rooms:      ON");
#endif


	if (ironman_nightmare)
#ifdef JP
buf_sprintf(dumpbuf, "\n ��̴�⡼��:         ON");
#else
		buf_sprintf(dumpbuf, "\n Nightmare Mode:     ON");
#endif


	if (ironman_empty_levels)
#ifdef JP
		buf_sprintf(dumpbuf, "\n ���꡼��:           ALWAYS");
#else
		buf_sprintf(dumpbuf, "\n Arena Levels:       ALWAYS");
#endif

	else if (empty_levels)
#ifdef JP
		buf_sprintf(dumpbuf, "\n ���꡼��:           ON");
#else
		buf_sprintf(dumpbuf, "\n Arena Levels:       ENABLED");
#endif

	else
#ifdef JP
	        buf_sprintf(dumpbuf, "\n ���꡼��:           OFF");
#else
		buf_sprintf(dumpbuf, "\n Arena Levels:       OFF");
#endif


#ifdef JP
	buf_sprintf(dumpbuf, "\n �����९�����ȿ�: %d", number_of_quests());
#else
	buf_sprintf(dumpbuf, "\n Num. Random Quests: %d", number_of_quests());
#endif

	if (p_ptr->arena_number == 99)
	{
		buf_sprintf(dumpbuf, "\n Ʈ����: ����\n");
	}
	else if (p_ptr->arena_number > MAX_ARENA_MONS+2)
	{
		buf_sprintf(dumpbuf, "\n Ʈ����: ���Υ����ԥ���\n");
	}
	else if (p_ptr->arena_number > MAX_ARENA_MONS-1)
	{
		buf_sprintf(dumpbuf, "\n Ʈ����: �����ԥ���\n");
	}
	else
	{
#ifdef JP
buf_sprintf(dumpbuf, "\n Ʈ����Ǥξ�����:   %2d\n", (p_ptr->arena_number > MAX_ARENA_MONS ? MAX_ARENA_MONS : p_ptr->arena_number));
#endif
	}

	if (noscore)
#ifdef JP
buf_sprintf(dumpbuf, "\n ���������ʤ��Ȥ򤷤Ƥ��ޤäƤޤ���");
#else
		buf_sprintf(dumpbuf, "\n You have done something illegal.");
#endif


	if (stupid_monsters)
#ifdef JP
buf_sprintf(dumpbuf, "\n Ũ�϶򤫤ʹ�ư����ޤ���");
#else
		buf_sprintf(dumpbuf, "\n Your opponents are behaving stupidly.");
#endif


	if (munchkin_death)
#ifdef JP
buf_sprintf(dumpbuf, "\n ���ʤ��ϻ����򤹤륤��������Ϥ���äƤ��ޤ���");
#else
		buf_sprintf(dumpbuf, "\n You possess munchkinish power over death.");
#endif

	buf_sprintf(dumpbuf,"\n");

	/* Monsters slain */
	{
		int k;
		s32b Total = 0;

		for (k = 1; k < max_r_idx; k++)
		{
			monster_race *r_ptr = &r_info[k];

			if (r_ptr->flags1 & RF1_UNIQUE)
			{
				bool dead = (r_ptr->max_num == 0);
				if (dead)
				{
					Total++;
				}
			}
			else
			{
				s16b This = r_ptr->r_pkills;
				if (This > 0)
				{
					Total += This;
				}
			}
		}

		if (Total < 1)
#ifdef JP
buf_sprintf(dumpbuf,"\n �ޤ�Ũ���ݤ��Ƥ��ޤ���\n");
#else
			buf_sprintf(dumpbuf,"\n You have defeated no enemies yet.\n");
#endif

		else if (Total == 1)
#ifdef JP
buf_sprintf(dumpbuf,"\n ���Τ�Ũ���ݤ��Ƥ��ޤ���\n");
#else
			buf_sprintf(dumpbuf,"\n You have defeated one enemy.\n");
#endif

		else
#ifdef JP
buf_sprintf(dumpbuf,"\n %lu �Τ�Ũ���ݤ��Ƥ��ޤ���\n", Total);
#else
			buf_sprintf(dumpbuf,"\n You have defeated %lu enemies.\n", Total);
#endif

	}


#ifdef JP
buf_sprintf(dumpbuf, "\n\n  [�ץ쥤�䡼����]\n\n");
#else
	buf_sprintf(dumpbuf, "\n\n  [Virtues]\n\n");
#endif

#ifdef JP
	if (p_ptr->align > 150) disp_align = "����";
	else if (p_ptr->align > 50) disp_align = "����";
	else if (p_ptr->align > 10) disp_align = "����";
	else if (p_ptr->align > -11) disp_align = "��Ω";
	else if (p_ptr->align > -51) disp_align = "����";
	else if (p_ptr->align > -151) disp_align = "�氭";
	else disp_align = "�簭";
	buf_sprintf(dumpbuf, "°�� : %s\n", disp_align);
#else
	if (p_ptr->align > 150) disp_align = "lawful";
	else if (p_ptr->align > 50) disp_align = "good";
	else if (p_ptr->align > 10) disp_align = "nutral good";
	else if (p_ptr->align > -11) disp_align = "nutral";
	else if (p_ptr->align > -51) disp_align = "nutral evil";
	else if (p_ptr->align > -151) disp_align = "evil";
	else disp_align = "chaotic";
	buf_sprintf(dumpbuf, "Your alighnment : %s\n", disp_align);
#endif
	buf_sprintf(dumpbuf, "\n");
	dumpbuf_virtues(dumpbuf);

	if (p_ptr->muta1 || p_ptr->muta2 || p_ptr->muta3)
	{
#ifdef JP
buf_sprintf(dumpbuf, "\n\n  [�����Ѱ�]\n\n");
#else
		buf_sprintf(dumpbuf, "\n\n  [Mutations]\n\n");
#endif

		dumpbuf_mutations(dumpbuf);
	}


	/* Skip some lines */
	buf_sprintf(dumpbuf, "\n\n");


	/* Dump the equipment */
	if (equip_cnt)
	{
#ifdef JP
buf_sprintf(dumpbuf, "  [ ����饯�������� ]\n\n");
#else
		buf_sprintf(dumpbuf, "  [Character Equipment]\n\n");
#endif

		for (i = INVEN_RARM; i < INVEN_TOTAL; i++)
		{
			object_desc(o_name, &inventory[i], TRUE, 3);
			if ((i == INVEN_LARM) && p_ptr->ryoute)
#ifdef JP
				strcpy(o_name, "(����ξ�����)");
#else
				strcpy(o_name, "(wielding a weapon with two-handed.)");
#endif
			buf_sprintf(dumpbuf, "%c%s %s\n",
			        index_to_label(i), paren, o_name);
		}
		buf_sprintf(dumpbuf, "\n\n");
	}

	/* Dump the inventory */
#ifdef JP
buf_sprintf(dumpbuf, "  [ ����饯���λ���ʪ ]\n\n");
#else
	buf_sprintf(dumpbuf, "  [Character Inventory]\n\n");
#endif

	for (i = 0; i < INVEN_PACK; i++)
	{
		/* Don't dump the empty slots */
		if (!inventory[i].k_idx) break;

		/* Dump the inventory slots */
		object_desc(o_name, &inventory[i], TRUE, 3);
		buf_sprintf(dumpbuf, "%c%s %s\n", index_to_label(i), paren, o_name);
	}

	/* Add an empty line */
	buf_sprintf(dumpbuf, "\n\n");

	process_dungeon_file("w_info_j.txt", 0, 0, max_wild_y, max_wild_x);

	st_ptr = &town[1].store[STORE_HOME];

	/* Home -- if anything there */
	if (st_ptr->stock_num)
	{
		/* Header with name of the town */
#ifdef JP
		buf_sprintf(dumpbuf, "  [ �椬�ȤΥ����ƥ� ]\n");
		x=1;
#else
		buf_sprintf(dumpbuf, "  [Home Inventory]\n");
#endif


		/* Dump all available items */
		for (i = 0; i < st_ptr->stock_num; i++)
		{
#ifdef JP
                if ((i % 12) == 0) buf_sprintf(dumpbuf, "\n ( %d �ڡ��� )\n", x++);
#else
                if ((i % 12) == 0) buf_sprintf(dumpbuf, "\n ( Page %d )\n", x++);
#endif
		object_desc(o_name, &st_ptr->stock[i], TRUE, 3);
		buf_sprintf(dumpbuf, "%c%s %s\n", I2A(i%12), paren, o_name);

		}

		/* Add an empty line */
		buf_sprintf(dumpbuf, "\n\n");
	}

	st_ptr = &town[1].store[STORE_MUSEUM];

	/* Home -- if anything there */
	if (st_ptr->stock_num)
	{
		/* Header with name of the town */
#ifdef JP
		buf_sprintf(dumpbuf, "  [ ��ʪ�ۤΥ����ƥ� ]\n");
		x=1;
#else
		buf_sprintf(dumpbuf, "  [Museum]\n");
#endif


		/* Dump all available items */
		for (i = 0; i < st_ptr->stock_num; i++)
		{
#ifdef JP
                if ((i % 12) == 0) buf_sprintf(dumpbuf, "\n ( %d �ڡ��� )\n", x++);
			object_desc(o_name, &st_ptr->stock[i], TRUE, 3);
			buf_sprintf(dumpbuf, "%c%s %s\n", I2A(i%12), paren, o_name);
#else
			object_desc(o_name, &st_ptr->stock[i], TRUE, 3);
			buf_sprintf(dumpbuf, "%c%s %s\n", I2A(i), paren, o_name);
#endif

		}

		/* Add an empty line */
		buf_sprintf(dumpbuf, "\n\n");
	}

	/* Success */
	return (0);
}

errr report_score(void)
{
#ifdef MACINTOSH
	OSStatus err;
#else
	errr err = 0;
#endif

#ifdef WINDOWS
    WSADATA wsaData;
    WORD wVersionRequested =(WORD) (( 1) |  ( 1 << 8));
#endif


  BUF *score;
  int sd;
  char date[10];
  char seikakutmp[128];
  time_t ct = time(NULL);

  score = buf_new();

#ifdef JP
  sprintf(seikakutmp, "%s%s", ap_ptr->title, (ap_ptr->no ? "��" : ""));
#else
  sprintf(seikakutmp, "%s ", ap_ptr->title);
#endif

  /* HiperMegaHack -- ʸ�������ɤ����� */
#if defined(EUC)
  buf_sprintf(score, "code: 0\n");
#elif defined(SJIS)
  buf_sprintf(score, "code: 1\n");
#elif defined(JIS)
  buf_sprintf(score, "code: 2\n");
#endif
  buf_sprintf(score, "name: %s\n", player_name);
#ifdef JP
  buf_sprintf(score, "version: �Ѷ����� %d.%d.%d\n",
	      FAKE_VER_MAJOR-10, FAKE_VER_MINOR, FAKE_VER_PATCH);
#else
  buf_sprintf(score, "version: Hengband %d.%d.%d\n",
	      FAKE_VER_MAJOR-10, FAKE_VER_MINOR, FAKE_VER_PATCH);
#endif
  buf_sprintf(score, "score: %d\n", total_points());
  buf_sprintf(score, "level: %d\n", p_ptr->lev);
  buf_sprintf(score, "depth: %d\n", dun_level);
  buf_sprintf(score, "maxlv: %d\n", p_ptr->max_plv);
  buf_sprintf(score, "maxdp: %d\n", max_dlv[DUNGEON_ANGBAND]);
  buf_sprintf(score, "au: %d\n", p_ptr->au);
  buf_sprintf(score, "turns: %d\n", turn_real(turn));
  buf_sprintf(score, "sex: %d\n", p_ptr->psex);
  buf_sprintf(score, "race: %s\n", rp_ptr->title);
  buf_sprintf(score, "class: %s\n", cp_ptr->title);
  buf_sprintf(score, "seikaku: %s\n", seikakutmp);
  buf_sprintf(score, "realm1: %s\n", realm_names[p_ptr->realm1]);
  buf_sprintf(score, "realm2: %s\n", realm_names[p_ptr->realm2]);
  buf_sprintf(score, "killer: %s\n", died_from);
  strftime(date, 9, "%y/%m/%d", localtime(&ct));
  buf_sprintf(score, "date: %s\n", date);
  buf_sprintf(score, "-----charcter dump-----\n");

  make_dump(score);

#ifdef WINDOWS
    if(WSAStartup(wVersionRequested, &wsaData)){
	msg_print("Report: WSAStartup failed.");
	goto report_end;
    }
#endif

#ifdef MACINTOSH
#if TARGET_API_MAC_CARBON
	err = InitOpenTransportInContext(kInitOTForApplicationMask, NULL);
#else
	err = InitOpenTransport();
#endif
	if (err != noErr){
		msg_print("Report: OpenTransport failed.");
		return 1;
	}
#endif

	Term_clear();

	while(1)
	{
		char buff[160];
#ifdef JP
		prt("��³��...", 0, 0);
#else
		prt("connecting...", 0, 0);
#endif
		Term_fresh();
		
		sd = connect_scoreserver();
		if (!(sd < 0)) break;
#ifdef JP
		sprintf(buff, "�������������Фؤ���³�˼��Ԥ��ޤ�����(%s)", soc_err());
#else
		sprintf(buff, "Failed to connect to the score server.(%s)", soc_err());
#endif
		prt(buff, 0, 0);
		(void)inkey();
		
#ifdef JP
		if(!get_check("�⤦������³���ߤޤ���? "))
#else
		if(!get_check("Try again? "))
#endif
		{
			err = 1;
			goto report_end;
		}
	}
#ifdef JP
	prt("������������...", 0, 0);
#else
	prt("Sending the score...", 0, 0);
#endif
	Term_fresh();
	http_post(sd, SCORE_PATH, score);
	
	disconnect_server(sd);
 report_end:
#ifdef WINDOWS
	WSACleanup();
#endif

#ifdef MACINTOSH
#if TARGET_API_MAC_CARBON
	CloseOpenTransportInContext(NULL);
#else
	CloseOpenTransport();
#endif
#endif

	return err;
}

#endif /* WORLD_SCORE */




