/* File: z-util.c */

/*
 * Copyright (c) 1997 Ben Harrison
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.
 */

/* Purpose: Low level utilities -BEN- */

#include "z-util.h"
#include "angband.h"




/*
 * Convenient storage of the program name
 */
cptr argv0 = NULL;


/*
 * Case insensitive comparison between two strings
 */
int my_stricmp(const char *s1, const char *s2)
{
	char ch1 = 0;
	char ch2 = 0;

	/* Just loop */
	while (TRUE)
	{
		/* We've reached the end of both strings simultaneously */
		if ((*s1 == 0) && (*s2 == 0))
		{
			/* We're still here, so s1 and s2 are equal */
			return (0);
		}

		ch1 = toupper(*s1);
		ch2 = toupper(*s2);

		/* If the characters don't match */
		if (ch1 != ch2)
		{
			/* return the difference between them */
			return ((int)(ch1 - ch2));
		}

		/* Step on through both strings */
		s1++;
		s2++;
	}

	return (0);
}

/*
 * Case insensitive comparison between the first n characters of two strings
 */
int my_strnicmp(cptr a, cptr b, int n)
{
	cptr s1, s2;
	char z1, z2;

	/* Scan the strings */
	for (s1 = a, s2 = b; n > 0; s1++, s2++, n--)
	{
		z1 = toupper((unsigned char)*s1);
		z2 = toupper((unsigned char)*s2);
		if (z1 < z2) return (-1);
		if (z1 > z2) return (1);
		if (!z1) return (0);
	}

	return 0;
}

/*
 * The my_strcpy() function copies up to 'bufsize'-1 characters from 'src'
 * to 'buf' and NUL-terminates the result.  The 'buf' and 'src' strings may
 * not overlap.
 *
 * my_strcpy() returns strlen(src).  This makes checking for truncation
 * easy.  Example: if (my_strcpy(buf, src, sizeof(buf)) >= sizeof(buf)) ...;
 *
 * This function should be equivalent to the strlcpy() function in BSD.
 */
size_t my_strcpy(char *buf, const char *src, size_t bufsize)
{
	size_t len = strlen(src);
	size_t ret = len;

	/* Paranoia */
	if (bufsize == 0) return ret;

	/* Truncate */
	if (len >= bufsize) len = bufsize - 1;

	/* Copy the string and terminate it */
	(void)memcpy(buf, src, len);
	buf[len] = '\0';

	/* Return strlen(src) */
	return ret;
}


/*
 * The my_strcat() tries to append a string to an existing NUL-terminated string.
 * It never writes more characters into the buffer than indicated by 'bufsize' and
 * NUL-terminates the buffer.  The 'buf' and 'src' strings may not overlap.
 *
 * my_strcat() returns strlen(buf) + strlen(src).  This makes checking for
 * truncation easy.  Example:
 * if (my_strcat(buf, src, sizeof(buf)) >= sizeof(buf)) ...;
 *
 * This function should be equivalent to the strlcat() function in BSD.
 */
size_t my_strcat(char *buf, const char *src, size_t bufsize)
{
	size_t dlen = strlen(buf);

	/* Is there room left in the buffer? */
	if (dlen < bufsize - 1)
	{
		/* Append as much as possible  */
		return (dlen + my_strcpy(buf + dlen, src, bufsize - dlen));
	}
	else
	{
		/* Return without appending */
		return (dlen + strlen(src));
	}
}


/*
 * Determine if string "a" is equal to string "b"
 */
bool streq(cptr a, cptr b)
{
	return (!strcmp(a, b));
}


/*
 * Determine if string "t" is a suffix of string "s"
 */
bool suffix(cptr s, cptr t)
{
	size_t tlen = strlen(t);
	size_t slen = strlen(s);

	/* Check for incompatible lengths */
	if (tlen > slen) return (FALSE);

	/* Compare "t" to the end of "s" */
	return (!strcmp(s + slen - tlen, t));
}


/*
 * Determine if string "t" is a prefix of string "s"
 */
bool prefix(cptr s, cptr t)
{
	/* Scan "t" */
	while (*t)
	{
		/* Compare content and length */
		if (*t++ != *s++) return (FALSE);
	}

	/* Matched, we have a prefix */
	return (TRUE);
}



/*
 * Redefinable "plog" action
 */
void (*plog_aux)(cptr) = NULL;

/*
 * Print (or log) a "warning" message (ala "perror()")
 * Note the use of the (optional) "plog_aux" hook.
 */
void plog(cptr str)
{
	/* Use the "alternative" function if possible */
	if (plog_aux) (*plog_aux)(str);

	/* Just do a labeled fprintf to stderr */
	else (void)(fprintf(stderr, "%s: %s\n", argv0 ? argv0 : "?", str));
}



/*
 * Redefinable "quit" action
 */
void (*quit_aux)(cptr) = NULL;

/*
 * Exit (ala "exit()").  If 'str' is NULL, do "exit(EXIT_SUCCESS)".
 * Otherwise, plog() 'str' and exit with an error code of -1.
 * But always use 'quit_aux', if set, before anything else.
 */
void quit(cptr str)
{
	/* Attempt to use the aux function */
	if (quit_aux) (*quit_aux)(str);

	/* Cleanup the generic angband stuff */
	cleanup_angband();

	/* Success */
	if (!str) (void)(exit(EXIT_SUCCESS));

	/* Send the string to plog() */
	plog(str);

	/* Failure */
	(void)(exit(EXIT_FAILURE));
}


/*
 * Fast string concatenation.
 * Append the "src" string to "buf" given the address of the trailing null
 * character of "buf" in "end". "end" can be NULL, in which the trailing null
 * character is fetched from the beginning of "buf".
 * "bufsize" is the maximum size of "buf" (including the trailing null character).
 * It returns the -new- address of the trailing null character of "buf".
 *
 * Example of usage:
 *
 * char buf[100] = "", *end;
 * int i;
 *
 * end = my_fast_strcat(buf, NULL, "START", sizeof(buf));
 *
 * for (i = 0; i < 5; i++)
 * {
 * 	end = my_fast_strcat(buf, end, "_", sizeof(buf));
 * }
 *
 * end = my_fast_strcat(buf, end, "END", sizeof(buf));
 *
 * buf ==> "START_____END"
 */
char *my_fast_strcat(char *buf, char *end, const char *src, size_t bufsize)
{
	/* No end, go to the beginning of "buf" */
	if (end == NULL) end = buf;

	/* Find the trailing null character, if necessary */
	while (*end) ++end;

	/* Make room for the trailing null character, if possible */
	if (bufsize > 0) --bufsize;

	/* Append "str" to "buf", if possible */
	while (*src && ((size_t)(end - buf) < bufsize)) *end++ = *src++;

	/* Terminate the string */
	*end = '\0';

	/* Return the new end of "buf" */
	return end;
}
