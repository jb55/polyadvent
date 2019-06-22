#include <sys/types.h>

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "xml.h"

static void
xml_parseattrs(struct xmlparser *x)
{
	size_t namelen = 0, valuelen;
	int c, endsep, endname = 0, valuestart = 0;

	while ((c = GETNEXT) != EOF) {
		if (isspace(c)) {
			if (namelen)
				endname = 1;
			continue;
		} else if (c == '?')
			; /* ignore */
		else if (c == '=') {
			x->name[namelen] = '\0';
			valuestart = 1;
			endname = 1;
		} else if (namelen && ((endname && !valuestart && isalpha(c)) || (c == '>' || c == '/'))) {
			/* attribute without value */
			x->name[namelen] = '\0';
			if (x->xmlattrstart)
				x->xmlattrstart(x, x->tag, x->taglen, x->name, namelen);
			if (x->xmlattr)
				x->xmlattr(x, x->tag, x->taglen, x->name, namelen, "", 0);
			if (x->xmlattrend)
				x->xmlattrend(x, x->tag, x->taglen, x->name, namelen);
			endname = 0;
			x->name[0] = c;
			namelen = 1;
		} else if (namelen && valuestart) {
			/* attribute with value */
			if (x->xmlattrstart)
				x->xmlattrstart(x, x->tag, x->taglen, x->name, namelen);

			valuelen = 0;
			if (c == '\'' || c == '"') {
				endsep = c;
			} else {
				endsep = ' '; /* isspace() */
				goto startvalue;
			}

			while ((c = GETNEXT) != EOF) {
startvalue:
				if (c == '&') { /* entities */
					x->data[valuelen] = '\0';
					/* call data function with data before entity if there is data */
					if (valuelen && x->xmlattr)
						x->xmlattr(x, x->tag, x->taglen, x->name, namelen, x->data, valuelen);
					x->data[0] = c;
					valuelen = 1;
					while ((c = GETNEXT) != EOF) {
						if (c == endsep || (endsep == ' ' && (c == '>' || isspace(c))))
							break;
						if (valuelen < sizeof(x->data) - 1)
							x->data[valuelen++] = c;
						else {
							/* entity too long for buffer, handle as normal data */
							x->data[valuelen] = '\0';
							if (x->xmlattr)
								x->xmlattr(x, x->tag, x->taglen, x->name, namelen, x->data, valuelen);
							x->data[0] = c;
							valuelen = 1;
							break;
						}
						if (c == ';') {
							x->data[valuelen] = '\0';
							if (x->xmlattrentity)
								x->xmlattrentity(x, x->tag, x->taglen, x->name, namelen, x->data, valuelen);
							valuelen = 0;
							break;
						}
					}
				} else if (c != endsep && !(endsep == ' ' && (c == '>' || isspace(c)))) {
					if (valuelen < sizeof(x->data) - 1) {
						x->data[valuelen++] = c;
					} else {
						x->data[valuelen] = '\0';
						if (x->xmlattr)
							x->xmlattr(x, x->tag, x->taglen, x->name, namelen, x->data, valuelen);
						x->data[0] = c;
						valuelen = 1;
					}
				}
				if (c == endsep || (endsep == ' ' && (c == '>' || isspace(c)))) {
					x->data[valuelen] = '\0';
					if (x->xmlattr)
						x->xmlattr(x, x->tag, x->taglen, x->name, namelen, x->data, valuelen);
					if (x->xmlattrend)
						x->xmlattrend(x, x->tag, x->taglen, x->name, namelen);
					break;
				}
			}
			namelen = endname = valuestart = 0;
		} else if (namelen < sizeof(x->name) - 1) {
			x->name[namelen++] = c;
		}
		if (c == '>') {
			break;
		} else if (c == '/') {
			x->isshorttag = 1;
			x->name[0] = '\0';
			namelen = 0;
		}
	}
}

static void
xml_parsecomment(struct xmlparser *x)
{
	size_t datalen = 0, i = 0;
	int c;

	if (x->xmlcommentstart)
		x->xmlcommentstart(x);
	while ((c = GETNEXT) != EOF) {
		if (c == '-' || c == '>') {
			if (x->xmlcomment && datalen) {
				x->data[datalen] = '\0';
				x->xmlcomment(x, x->data, datalen);
				datalen = 0;
			}
		}

		if (c == '-') {
			if (++i > 2) {
				if (x->xmlcomment)
					for (; i > 2; i--)
						x->xmlcomment(x, "-", 1);
				i = 2;
			}
			continue;
		} else if (c == '>' && i == 2) {
			if (x->xmlcommentend)
				x->xmlcommentend(x);
			return;
		} else if (i) {
			if (x->xmlcomment) {
				for (; i > 0; i--)
					x->xmlcomment(x, "-", 1);
			}
			i = 0;
		}

		if (datalen < sizeof(x->data) - 1) {
			x->data[datalen++] = c;
		} else {
			x->data[datalen] = '\0';
			if (x->xmlcomment)
				x->xmlcomment(x, x->data, datalen);
			x->data[0] = c;
			datalen = 1;
		}
	}
}

static void
xml_parsecdata(struct xmlparser *x)
{
	size_t datalen = 0, i = 0;
	int c;

	if (x->xmlcdatastart)
		x->xmlcdatastart(x);
	while ((c = GETNEXT) != EOF) {
		if (c == ']' || c == '>') {
			if (x->xmlcdata && datalen) {
				x->data[datalen] = '\0';
				x->xmlcdata(x, x->data, datalen);
				datalen = 0;
			}
		}

		if (c == ']') {
			if (++i > 2) {
				if (x->xmlcdata)
					for (; i > 2; i--)
						x->xmlcdata(x, "]", 1);
				i = 2;
			}
			continue;
		} else if (c == '>' && i == 2) {
			if (x->xmlcdataend)
				x->xmlcdataend(x);
			return;
		} else if (i) {
			if (x->xmlcdata)
				for (; i > 0; i--)
					x->xmlcdata(x, "]", 1);
			i = 0;
		}

		if (datalen < sizeof(x->data) - 1) {
			x->data[datalen++] = c;
		} else {
			x->data[datalen] = '\0';
			if (x->xmlcdata)
				x->xmlcdata(x, x->data, datalen);
			x->data[0] = c;
			datalen = 1;
		}
	}
}

static int
codepointtoutf8(long r, char *s)
{
	if (r == 0) {
		return 0; /* NUL byte */
	} else if (r <= 0x7F) {
		/* 1 byte: 0aaaaaaa */
		s[0] = r;
		return 1;
	} else if (r <= 0x07FF) {
		/* 2 bytes: 00000aaa aabbbbbb */
		s[0] = 0xC0 | ((r & 0x0007C0) >>  6); /* 110aaaaa */
		s[1] = 0x80 |  (r & 0x00003F);        /* 10bbbbbb */
		return 2;
	} else if (r <= 0xFFFF) {
		/* 3 bytes: aaaabbbb bbcccccc */
		s[0] = 0xE0 | ((r & 0x00F000) >> 12); /* 1110aaaa */
		s[1] = 0x80 | ((r & 0x000FC0) >>  6); /* 10bbbbbb */
		s[2] = 0x80 |  (r & 0x00003F);        /* 10cccccc */
		return 3;
	} else {
		/* 4 bytes: 000aaabb bbbbcccc ccdddddd */
		s[0] = 0xF0 | ((r & 0x1C0000) >> 18); /* 11110aaa */
		s[1] = 0x80 | ((r & 0x03F000) >> 12); /* 10bbbbbb */
		s[2] = 0x80 | ((r & 0x000FC0) >>  6); /* 10cccccc */
		s[3] = 0x80 |  (r & 0x00003F);        /* 10dddddd */
		return 4;
	}
}

static int
namedentitytostr(const char *e, char *buf, size_t bufsiz)
{
	static const struct {
		const char *entity;
		int c;
	} entities[] = {
		{ "amp;",  '&'  },
		{ "lt;",   '<'  },
		{ "gt;",   '>'  },
		{ "apos;", '\'' },
		{ "quot;", '"'  },
		{ "AMP;",  '&'  },
		{ "LT;",   '<'  },
		{ "GT;",   '>'  },
		{ "APOS;", '\'' },
		{ "QUOT;", '"'  }
	};
	size_t i;

	/* buffer is too small */
	if (bufsiz < 2)
		return -1;

	for (i = 0; i < sizeof(entities) / sizeof(*entities); i++) {
		if (!strcmp(e, entities[i].entity)) {
			buf[0] = entities[i].c;
			buf[1] = '\0';
			return 1;
		}
	}
	return 0;
}

static int
numericentitytostr(const char *e, char *buf, size_t bufsiz)
{
	long l;
	int len;
	char *end;

	/* buffer is too small */
	if (bufsiz < 5)
		return -1;

	errno = 0;
	/* hex (16) or decimal (10) */
	if (*e == 'x')
		l = strtoul(e + 1, &end, 16);
	else
		l = strtoul(e, &end, 10);
	/* invalid value or not a well-formed entity or too high codepoint */
	if (errno || *end != ';' || l > 0x10FFFF)
		return 0;
	len = codepointtoutf8(l, buf);
	buf[len] = '\0';

	return len;
}

/* convert named- or numeric entity string to buffer string
 * returns byte-length of string. */
int
xml_entitytostr(const char *e, char *buf, size_t bufsiz)
{
	/* doesn't start with & */
	if (e[0] != '&')
		return 0;
	/* numeric entity */
	if (e[1] == '#')
		return numericentitytostr(e + 2, buf, bufsiz);
	else /* named entity */
		return namedentitytostr(e + 1, buf, bufsiz);
}

void
xml_parse(struct xmlparser *x)
{
	size_t datalen, tagdatalen;
	int c, isend;

	while ((c = GETNEXT) != EOF && c != '<')
		; /* skip until < */

	while (c != EOF) {
		if (c == '<') { /* parse tag */
			if ((c = GETNEXT) == EOF)
				return;

			if (c == '!') { /* cdata and comments */
				for (tagdatalen = 0; (c = GETNEXT) != EOF;) {
					/* NOTE: sizeof(x->data) must be atleast sizeof("[CDATA[") */
					if (tagdatalen <= sizeof("[CDATA[") - 1)
						x->data[tagdatalen++] = c;
					if (c == '>')
						break;
					else if (c == '-' && tagdatalen == sizeof("--") - 1 &&
							(x->data[0] == '-')) {
						xml_parsecomment(x);
						break;
					} else if (c == '[') {
						if (tagdatalen == sizeof("[CDATA[") - 1 &&
						    !strncmp(x->data, "[CDATA[", tagdatalen)) {
							xml_parsecdata(x);
							break;
						}
					}
				}
			} else {
				/* normal tag (open, short open, close), processing instruction. */
				x->tag[0] = c;
				x->taglen = 1;
				x->isshorttag = isend = 0;

				/* treat processing instruction as shorttag, don't strip "?" prefix. */
				if (c == '?') {
					x->isshorttag = 1;
				} else if (c == '/') {
					if ((c = GETNEXT) == EOF)
						return;
					x->tag[0] = c;
					isend = 1;
				}

				while ((c = GETNEXT) != EOF) {
					if (c == '/')
						x->isshorttag = 1; /* short tag */
					else if (c == '>' || isspace(c)) {
						x->tag[x->taglen] = '\0';
						if (isend) { /* end tag, starts with </ */
							if (x->xmltagend)
								x->xmltagend(x, x->tag, x->taglen, x->isshorttag);
							x->tag[0] = '\0';
							x->taglen = 0;
						} else {
							/* start tag */
							if (x->xmltagstart)
								x->xmltagstart(x, x->tag, x->taglen);
							if (isspace(c))
								xml_parseattrs(x);
							if (x->xmltagstartparsed)
								x->xmltagstartparsed(x, x->tag, x->taglen, x->isshorttag);
						}
						/* call tagend for shortform or processing instruction */
						if (x->isshorttag) {
							if (x->xmltagend)
								x->xmltagend(x, x->tag, x->taglen, x->isshorttag);
							x->tag[0] = '\0';
							x->taglen = 0;
						}
						break;
					} else if (x->taglen < sizeof(x->tag) - 1)
						x->tag[x->taglen++] = c; /* NOTE: tag name truncation */
				}
			}
		} else {
			/* parse tag data */
			datalen = 0;
			if (x->xmldatastart)
				x->xmldatastart(x);
			while ((c = GETNEXT) != EOF) {
				if (c == '&') {
					if (datalen) {
						x->data[datalen] = '\0';
						if (x->xmldata)
							x->xmldata(x, x->data, datalen);
					}
					x->data[0] = c;
					datalen = 1;
					while ((c = GETNEXT) != EOF) {
						if (c == '<')
							break;
						if (datalen < sizeof(x->data) - 1)
							x->data[datalen++] = c;
						else {
							/* entity too long for buffer, handle as normal data */
							x->data[datalen] = '\0';
							if (x->xmldata)
								x->xmldata(x, x->data, datalen);
							x->data[0] = c;
							datalen = 1;
							break;
						}
						if (c == ';') {
							x->data[datalen] = '\0';
							if (x->xmldataentity)
								x->xmldataentity(x, x->data, datalen);
							datalen = 0;
							break;
						}
					}
				} else if (c != '<') {
					if (datalen < sizeof(x->data) - 1) {
						x->data[datalen++] = c;
					} else {
						x->data[datalen] = '\0';
						if (x->xmldata)
							x->xmldata(x, x->data, datalen);
						x->data[0] = c;
						datalen = 1;
					}
				}
				if (c == '<') {
					x->data[datalen] = '\0';
					if (x->xmldata && datalen)
						x->xmldata(x, x->data, datalen);
					if (x->xmldataend)
						x->xmldataend(x);
					break;
				}
			}
		}
	}
}
