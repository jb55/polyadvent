#ifndef _XML_H
#define _XML_H

#define XML_BUFSIZ 1024

#include <stdlib.h>

struct xmlparser {
	/* handlers */
	void (*xmlattr)(struct xmlparser *, const char *, size_t,
	      const char *, size_t, const char *, size_t);
	void (*xmlattrend)(struct xmlparser *, const char *, size_t,
	      const char *, size_t);
	void (*xmlattrstart)(struct xmlparser *, const char *, size_t,
	      const char *, size_t);
	void (*xmlattrentity)(struct xmlparser *, const char *, size_t,
	      const char *, size_t, const char *, size_t);
	void (*xmlcdatastart)(struct xmlparser *);
	void (*xmlcdata)(struct xmlparser *, const char *, size_t);
	void (*xmlcdataend)(struct xmlparser *);
	void (*xmlcommentstart)(struct xmlparser *);
	void (*xmlcomment)(struct xmlparser *, const char *, size_t);
	void (*xmlcommentend)(struct xmlparser *);
	void (*xmldata)(struct xmlparser *, const char *, size_t);
	void (*xmldataend)(struct xmlparser *);
	void (*xmldataentity)(struct xmlparser *, const char *, size_t);
	void (*xmldatastart)(struct xmlparser *);
	void (*xmltagend)(struct xmlparser *, const char *, size_t, int);
	void (*xmltagstart)(struct xmlparser *, const char *, size_t);
	void (*xmltagstartparsed)(struct xmlparser *, const char *,
	      size_t, int);

#ifndef GETNEXT
	#define GETNEXT (x)->getnext(x)
	int (*getnext)(struct xmlparser *);
#endif

	/* current tag */
	char tag[1024];
	size_t taglen;
    void *user_data;
	/* current tag is in short form ? <tag /> */
	int isshorttag;
	/* current attribute name */
	char name[1024];
	/* data buffer used for tag data, cdata and attribute data */
	char data[XML_BUFSIZ];
};

int xml_entitytostr(const char *, char *, size_t);
void xml_parse(struct xmlparser *);
#endif

/*
void xmlattr(struct xmlparser *x, const char *t, size_t tl, const char *a, size_t al, const char *v, size_t vl)
void xmlattrentity(struct xmlparser *x, const char *t, size_t tl, const char *a, size_t al, const char *v, size_t vl)
void xmlattrend(struct xmlparser *x, const char *t, size_t tl, const char *a, size_t al)
void xmlattrstart(struct xmlparser *x, const char *t, size_t tl, const char *a, size_t al)
void xmlcdatastart(struct xmlparser *x)
void xmlcdata(struct xmlparser *x, const char *d, size_t dl)
void xmlcdataend(struct xmlparser *x)
void xmlcommentstart(struct xmlparser *x)
void xmlcomment(struct xmlparser *x, const char *c, size_t cl)
void xmlcommentend(struct xmlparser *x)
void xmldata(struct xmlparser *x, const char *d, size_t dl)
void xmldataend(struct xmlparser *x)
void xmldataentity(struct xmlparser *x, const char *d, size_t dl)
void xmldatastart(struct xmlparser *x)
void xmltagend(struct xmlparser *x, const char *t, size_t tl, int isshort)
void xmltagstart(struct xmlparser *x, const char *t, size_t tl)
void xmltagstartparsed(struct xmlparser *x, const char *t, size_t tl, int isshort)

*/
