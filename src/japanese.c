/* File: japanese.c */


/*
 * Copyright (c) 1997 Ben Harrison
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

#include "angband.h"

#ifdef JP

typedef struct sindarin2j sindarin2j;

struct sindarin2j {
  char *sindarin;
  char *kana;
};

static const sindarin2j s2j_table[]= {
{"mb","nb"},{"mp","np"},{"mv","nv"},{"mm","nm"},
{"x","ks"},
{"ar$","a-ru$"},{"ir$","ia$"},{"or$","o-ru$"},
{"ra","��"},{"ri","��"},{"ru","��"},{"re","��"},{"ro","��"},
{"ir","ia"},{"ur","ua"},{"er","ea"},
{"ar","a��"},
{"sha","����"},{"shi","��"},{"shu","����"},{"she","����"},{"sho","����"},
{"tha","��"},{"thi","��"},{"thu","��"},{"the","��"},{"tho","��"},
{"cha","��"},{"chi","��"},{"chu","��"},{"che","��"},{"cho","��"},
{"dha","��"},{"dhi","��"},{"dhu","��"},{"dhe","��"},{"dho","��"},
{"ba","��"},{"bi","��"},{"bu","��"},{"be","��"},{"bo","��"},
{"ca","��"},{"ci","��"},{"cu","��"},{"ce","��"},{"co","��"},
{"da","��"},{"di","�ǥ�"},{"du","�ɥ�"},{"de","��"},{"do","��"},
{"fa","�ե�"},{"fi","�ե�"},{"fu","��"},{"fe","�ե�"},{"fo","�ե�"},
{"ga","��"},{"gi","��"},{"gu","��"},{"ge","��"},{"go","��"},
{"ha","��"},{"hi","��"},{"hu","��"},{"he","��"},{"ho","��"},
{"ja","����"},{"ji","��"},{"ju","����"},{"je","����"},{"jo","����"},
{"ka","��"},{"ki","��"},{"ku","��"},{"ke","��"},{"ko","��"},
{"la","��"},{"li","��"},{"lu","��"},{"le","��"},{"lo","��"},
{"ma","��"},{"mi","��"},{"mu","��"},{"me","��"},{"mo","��"},
{"na","��"},{"ni","��"},{"nu","��"},{"ne","��"},{"no","��"},
{"pa","��"},{"pi","��"},{"pu","��"},{"pe","��"},{"po","��"},
{"qu","��"},
{"sa","��"},{"si","��"},{"su","��"},{"se","��"},{"so","��"},
{"ta","��"},{"ti","�ƥ�"},{"tu","�ȥ�"},{"te","��"},{"to","��"},
{"va","����"},{"vi","����"},{"vu","��"},{"ve","����"},{"vo","����"},
{"wa","��"},{"wi","����"},{"wu","��"},{"we","����"},{"wo","����"},
{"ya","��"},{"yu","��"},{"yo","��"},
{"za","��"},{"zi","��"},{"zu","��"},{"ze","��"},{"zo","��"},
{"dh","��"},
{"ch","��"},
{"th","��"},
{"b","��"},
{"c","��"},
{"d","��"},
{"f","��"},
{"g","��"},
{"h","��"},
{"j","����"},
{"k","��"},
{"l","��"},
{"m","��"},
{"n","��"},
{"p","��"},
{"q","��"},
{"r","��"},
{"s","��"},
{"t","��"},
{"v","��"},
{"w","��"},
{"y","��"},
{"a","��"},
{"i","��"},
{"u","��"},
{"e","��"},
{"o","��"},
{"-","��"},
{NULL,NULL}
};

unsigned char *sindarin_to_kana( unsigned char *sindarin ){
static unsigned char buf1[256], buf2[256];
int idx;

sprintf(buf1,"%s$",sindarin);
for(idx=0;buf1[idx];idx++) if( isupper(buf1[idx]))buf1[idx]=tolower(buf1[idx]);

for(idx=0; s2j_table[idx].sindarin != NULL;idx++){
  unsigned char *pat1 = s2j_table[idx].sindarin;
  unsigned char *pat2 = s2j_table[idx].kana;
  int len=strlen(pat1);
  unsigned char *dest = buf2;
  unsigned char *src = buf1;
 
  for( ; *src ; ){
    if( strncmp( src, pat1, len)==0 ){
     strcpy( dest, pat2 );
     src+=len;
     dest+= strlen(pat2);
    }
    else
      if( iskanji(*src) ){
	*dest = *src;
	dest++; src++;
        *dest=*src;
	dest++; src++;
      }	
      else{
        *dest=*src;
	dest++; src++;
      }    
  }  
  *dest=0;
  strcpy(buf1,buf2);
}
idx=0;
while( buf1[idx] != '$' ) idx++;
buf1[idx]=0;
return(buf1);
}

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

#if 0
void roff_to_buf(char * str,int wlen,char *tbuf)
{
   int wpos=-1;
   int dpos=0;
   int len=0;
   int wrap;
   char ch[3];
   ch[2]=0;
	/* Scan the given string, character at a time */
	for (; *str; str++)
	{
                int k_flag = iskanji((unsigned char)(*str));
		ch[0] = *str;ch[1]=0;
                if( k_flag ) {str++; ch[1]=*str;}
                wrap = (ch[0] == '\n');
                if (!isprint(ch[0]) && !k_flag ) ch[0] = ' ';
                if ( len >=  wlen )  wrap=1;

		/* Handle line-wrap */
		if (wrap)
		{
			if (wpos!=-1) {
			  if(tbuf[wpos]!=' ') { 
			    int i;
                            len=dpos-wpos;
			    for( i=dpos;i>=wpos+1;i--)tbuf[i]=tbuf[i-1];
			    dpos++;
			  }
			  else
			  {
				  len = 1;
			  }
			  tbuf[wpos]=0;wpos=-1; 
			}else {tbuf[dpos]=0;dpos++;wpos=-1;len=0;}
		}
		if( strcmp(ch," ")==0 ||
		    (k_flag && strcmp(ch, "��") != 0 && 
		     strcmp(ch, "��") != 0 &&
		     strcmp(ch, "��") != 0 &&
		     strcmp(ch, "��") != 0)) wpos=dpos;
                if ( len>0 || strcmp(ch," ")) {
		  strcpy( &tbuf[dpos],ch); dpos+= k_flag? 2:1;
                  len+=k_flag? 2:1;
		}
	}
/***************/
	if(dpos!=0) dpos--;
	while (tbuf[dpos] == ' ' && dpos>0) dpos--;
	if ( dpos!=0) dpos++;
	tbuf[dpos]=0;
	tbuf[dpos+1]=0;
}
#endif

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
		if (iskanji(*d)) {
			if(n + 2 >= size || !*(d+1)) break;
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

#endif /* JP */

