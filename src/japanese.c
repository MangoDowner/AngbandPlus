/* File: japanese.c */


#include "angband.h"

#ifdef JP

/*���ܸ�ư����� (�Ǥġ��Ǥä�,�Ǥ� etc) */

#define CMPTAIL(y) strncmp(&in[l-strlen(y)],y,strlen(y))

/* ����,����䲥��,���� */
void jverb1( const char *in , char *out){
int l=strlen(in);
strcpy(out,in);

if( CMPTAIL("����")==0) sprintf(&out[l-4],"��");else
if( CMPTAIL("����")==0) sprintf(&out[l-4],"����");else

if( CMPTAIL("����")==0) sprintf(&out[l-4],"��");else
if( CMPTAIL("����")==0) sprintf(&out[l-4],"��");else
if( CMPTAIL("����")==0) sprintf(&out[l-4],"��");else
if( CMPTAIL("����")==0) sprintf(&out[l-4],"��");else
if( CMPTAIL("����")==0) sprintf(&out[l-4],"��");else
if( CMPTAIL("�Ƥ�")==0) sprintf(&out[l-4],"��");else
if( CMPTAIL("�Ǥ�")==0) sprintf(&out[l-4],"��");else
if( CMPTAIL("�ͤ�")==0) sprintf(&out[l-4],"��");else
if( CMPTAIL("�ؤ�")==0) sprintf(&out[l-4],"��");else
if( CMPTAIL("�٤�")==0) sprintf(&out[l-4],"��");else
if( CMPTAIL("���")==0) sprintf(&out[l-4],"��");else
if( CMPTAIL("���")==0) sprintf(&out[l-4],"��");else

if( CMPTAIL("��")==0) sprintf(&out[l-2],"��");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"��");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"��");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"��");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"��");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"��");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"��");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"��");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"��");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"��");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"��");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"��");else

  sprintf(&out[l],"������");}

/* ����,����> ���äƽ��� */
void jverb2( const char *in , char *out){
int l=strlen(in);
strcpy(out,in);

if( CMPTAIL("����")==0) sprintf(&out[l-4],"����");else
if( CMPTAIL("����")==0) sprintf(&out[l-4],"����");else

if( CMPTAIL("����")==0) sprintf(&out[l-4],"����");else
if( CMPTAIL("����")==0) sprintf(&out[l-4],"����");else
if( CMPTAIL("����")==0) sprintf(&out[l-4],"����");else
if( CMPTAIL("����")==0) sprintf(&out[l-4],"����");else
if( CMPTAIL("����")==0) sprintf(&out[l-4],"����");else
if( CMPTAIL("�Ƥ�")==0) sprintf(&out[l-4],"�Ƥä�");else
if( CMPTAIL("�Ǥ�")==0) sprintf(&out[l-4],"�Ǥ�");else
if( CMPTAIL("�ͤ�")==0) sprintf(&out[l-4],"�ͤ�");else
if( CMPTAIL("�ؤ�")==0) sprintf(&out[l-4],"�ؤ�");else
if( CMPTAIL("�٤�")==0) sprintf(&out[l-4],"�٤�");else
if( CMPTAIL("���")==0) sprintf(&out[l-4],"���");else
if( CMPTAIL("���")==0) sprintf(&out[l-4],"���");else

if( CMPTAIL("��")==0) sprintf(&out[l-2],"�ä�");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"����");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"����");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"����");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"����");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"�ä�");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"�ä�");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"�ͤ�");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"�ؤ�");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"���");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"���");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"�ä�");else
  sprintf(&out[l],"���Ȥˤ��");}

/* ����,���� > ���ä��꽳�ä��� */
void jverb3( const char *in , char *out){
int l=strlen(in);
strcpy(out,in);

if( CMPTAIL("����")==0) sprintf(&out[l-4],"����");else
if( CMPTAIL("����")==0) sprintf(&out[l-4],"����");else

if( CMPTAIL("����")==0) sprintf(&out[l-4],"����");else
if( CMPTAIL("����")==0) sprintf(&out[l-4],"����");else
if( CMPTAIL("����")==0) sprintf(&out[l-4],"����");else
if( CMPTAIL("����")==0) sprintf(&out[l-4],"����");else
if( CMPTAIL("����")==0) sprintf(&out[l-4],"����");else
if( CMPTAIL("�Ƥ�")==0) sprintf(&out[l-4],"�Ƥä�");else
if( CMPTAIL("�Ǥ�")==0) sprintf(&out[l-4],"�Ǥ�");else
if( CMPTAIL("�ͤ�")==0) sprintf(&out[l-4],"�ͤ�");else
if( CMPTAIL("�ؤ�")==0) sprintf(&out[l-4],"�ؤ�");else
if( CMPTAIL("�٤�")==0) sprintf(&out[l-4],"�٤�");else
if( CMPTAIL("���")==0) sprintf(&out[l-4],"�᤿");else
if( CMPTAIL("���")==0) sprintf(&out[l-4],"�줿");else

if( CMPTAIL("��")==0) sprintf(&out[l-2],"�ä�");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"����");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"����");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"����");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"����");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"�ä�");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"�ä�");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"�ͤ�");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"�ؤ�");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"���");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"���");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"�ä�");else
  sprintf(&out[l],"���Ȥ�");}


void jverb( const char *in , char *out , int flag){
  switch (flag){
  case JVERB_AND:jverb1(in , out);break;
  case JVERB_TO :jverb2(in , out);break;
  case JVERB_OR :jverb3(in , out);break;
  }
}

char* strstr_j(const char* s, const char* t)
{
	int i, l1, l2;

	l1 = strlen(s);
	l2 = strlen(t);
	if (l1 >= l2) {
		for(i = 0; i <= l1 - l2; i++) {
			if(!strncmp(s + i, t, l2))
				return (char *)s + i;
			if (iskanji(*(s + i)))
				i++;
		}
	}
	return NULL;
}

/* 2�Х���ʸ�����θ���ʤ������ n �Х���ʸ����򥳥ԡ����� */
size_t mb_strlcpy(char *dst, const char *src, size_t size)
{
	unsigned char *d = (unsigned char*)dst;
	const unsigned char *s = (unsigned char*)src;
	size_t n = 0;

	/* reserve for NUL termination */
	size--;

	/* Copy as many bytes as will fit */
	while(n < size) {
		if (iskanji(*s)) {
			if(n + 2 >= size || !*(s+1)) break;
			*d++ = *s++;
			*d++ = *s++;
			n += 2;
		} else {
			*d++ = *s++;
			n++;
		}
	}
	*d = '\0';
	while(*s++) n++;
	return n;
}

/*
 * str��Ķ��˹�ä�ʸ�������ɤ��Ѵ����롣
 * str��Ĺ�������¤Ϥʤ���
 */
void codeconv(char *str)
{
	int i;
	int kanji = 0, iseuc = 1;
	int len;
	unsigned char c1, c2;
	unsigned char *tmp;

	/* ������¸�ߤ������δ��������ɤ�EUC���ɤ���Ĵ�٤롣*/
	for (i = 0; str[i]; i++)
	{
		c1 = str[i];
		if (c1 & 0x80)  kanji = 1;
		if ( c1>=0x80 && (c1 < 0xa1 || c1 > 0xfe)) iseuc = 0;
	}

	/* str��Ĺ��+1(tmp������) */
	len = i + 1;

#ifdef EUC
	if (kanji && !iseuc)	 /* SJIS -> EUC */
	{
		C_MAKE(tmp, len, byte);

		for (i = 0; str[i]; i++)
		{
			c1 = str[i];
			if (c1 & 0x80)
			{
				i++;
				c2 = str[i];
				if (c2 >= 0x9f)
				{
					c1 = c1 * 2 - (c1 >= 0xe0 ? 0xe0 : 0x60);
					c2 += 2;
				}
				else
				{
					c1 = c1 * 2 - (c1 >= 0xe0 ? 0xe1 : 0x61);
					c2 += 0x60 + (c2 < 0x7f);
				}
				tmp[i - 1] = c1;
				tmp[i] = c2;
			}
			else
				tmp[i] = c1;
		}
		tmp[i] = '\0';
		strcpy(str, tmp);

		C_KILL(tmp, len, byte);
	}
#endif

#ifdef SJIS
	if (kanji && iseuc)	/* EUC -> SJIS */
	{
		C_MAKE(tmp, len, byte);

		for (i = 0; str[i]; i++)
		{
			c1 = str[i];
			if (c1 & 0x80)
			{
				i++;
				c2 = str[i];
				if (c1 % 2)
				{
					c1 = (c1 >> 1) + (c1 < 0xdf ? 0x31 : 0x71);
					c2 -= 0x60 + (c2 < 0xe0);
				}
				else
				{
					c1 = (c1 >> 1) + (c1 < 0xdf ? 0x30 : 0x70);
					c2 -= 2;
				}
				tmp[i - 1] = c1;
				tmp[i] = c2;
			}
			else
				tmp[i] = c1;
		}
		tmp[i] = '\0';
		strcpy(str, tmp);

		C_KILL(tmp, len, byte);
	}
#endif
}

#endif /* JP */

