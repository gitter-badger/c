#include <u.h>
#include <gc/gc.h>
#include <ds/ds.h>
#include "c.h"

#define MAXINCLUDE 128

int    nlexers;
Lexer *lexers[MAXINCLUDE];

static Tok *ppnoexpand();
static int64 ifexpr();

static void
pushlex(char *path)
{
	Lexer *l;

	if(nlexers == MAXINCLUDE)
		panic("include depth limit reached!");
	l = gcmalloc(sizeof(Lexer));
	l->pos.file = path;
	l->prevpos.file = path;
	l->markpos.file = path;
	l->pos.line = 1;
	l->pos.col = 1;
	l->f = fopen(path, "r");
	if (!l->f)
		errorf("error opening file %s\n", path);
	lexers[nlexers] = l;
	nlexers += 1;
}

static void
poplex()
{
	nlexers--;
	fclose(lexers[nlexers]->f);
}

static void
include()
{
	panic("unimplemented");
}

static void
define()
{
	panic("unimplemented");
}

static void
pif()
{
	ifexpr();
	panic("unimplemented");
}

static void
elseif()
{
	panic("unimplemented");
}


static void
pelse()
{
	panic("unimplemented");
}

static void
endif()
{
	panic("unimplemented");
}

static int64
ifexpr()
{
	return 0;
}

static void
directive()
{
	Tok  *t;
	char *dir;

	t = ppnoexpand();
	dir = t->v;
	if(strcmp(dir, "include") == 0)
		include();
	else if(strcmp(dir, "define")  == 0)
		define();
	else if(strcmp(dir, "if")  == 0)
		pif();
	else if(strcmp(dir, "elseif")  == 0)
		elseif();
	else if(strcmp(dir, "else")  == 0)
		pelse();
	else if(strcmp(dir, "endif")  == 0)
		endif();
	else
		errorposf(&t->pos, "invalid directive %s", dir);
}

static struct {char *kw; int t;} keywordlut[] = {
	{"auto", TOKAUTO},
	{"break", TOKBREAK},
	{"case", TOKCASE},
	{"char", TOKCHAR},
	{"const", TOKCONST},
	{"continue", TOKCONTINUE},
	{"default", TOKDEFAULT},
	{"do", TOKDO},
	{"double", TOKDOUBLE},
	{"else", TOKELSE},
	{"enum", TOKENUM},
	{"extern", TOKEXTERN},
	{"float", TOKFLOAT},
	{"for", TOKFOR},
	{"goto", TOKGOTO},
	{"if", TOKIF},
	{"int", TOKINT},
	{"long", TOKLONG},
	{"register", TOKREGISTER},
	{"return", TOKRETURN},
	{"short", TOKSHORT},
	{"signed", TOKSIGNED},
	{"sizeof", TOKSIZEOF},
	{"static", TOKSTATIC},
	{"struct", TOKSTRUCT},
	{"switch", TOKSWITCH},
	{"typedef", TOKTYPEDEF},
	{"union", TOKUNION},
	{"unsigned", TOKUNSIGNED},
	{"void", TOKVOID},
	{"volatile", TOKVOLATILE},
	{"while", TOKWHILE},
	{0, 0}
};

static int
identkind(char *s) {
	int i;

	i = 0;
	while(keywordlut[i].kw) {
		if(strcmp(keywordlut[i].kw, s) == 0)
			return keywordlut[i].t;
		i++;
	}
	return TOKIDENT;
}

static Tok *
ppnoexpand()
{
	Tok *t;

	t = lex(lexers[nlexers - 1]);
	if(t->k == TOKEOF && nlexers == 1)
		return t;
	if(t->k == TOKEOF && nlexers > 1) {
		poplex();
		return ppnoexpand();
	}
	if(t->k == TOKIDENT)
		t->k = identkind(t->v);
	return t;
}

Tok *
pp()
{
	Tok *t;

	t = ppnoexpand();
	if(t->k == '#' && t->nl) {
		directive();
		return pp();
	}
	return t;
}

void
cppinit(char *path)
{
	nlexers = 0;
	pushlex(path);
}

