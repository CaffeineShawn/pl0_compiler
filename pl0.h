#include <cstdio>

#define NRW        18     // number of reserved words
#define TXMAX      500    // length of identifier table
#define MAXNUMLEN  14     // maximum number of digits in numbers
#define NSYM       13     // maximum number of symbols in array ssym and csym
#define MAXIDLEN   10     // length of identifiers

#define MAXADDRESS 32767  // maximum address
#define MAXLEVEL   32     // maximum depth of nesting block
#define CXMAX      500    // size of code array

#define MAXSYM     30     // maximum number of symbols  

#define STACKSIZE  1000   // maximum storage

enum symtype
{
	SYM_NULL,           // 0
	SYM_IDENTIFIER,     // 1
	SYM_NUMBER,         // 2
	SYM_PLUS,           // 3
	SYM_MINUS,          // 4
	SYM_TIMES,          // 5
	SYM_SLASH,          // 6
	SYM_ODD,            // 7
	SYM_EQU,            // 8
	SYM_NEQ,            // 9
	SYM_LES,            // 10
	SYM_LEQ,            // 11
	SYM_GTR,            // 12
	SYM_GEQ,            // 13
	SYM_LPAREN,         // 14
	SYM_RPAREN,         // 15
	SYM_COMMA,          // 16
	SYM_SEMICOLON,      // 17
	SYM_PERIOD,         // 18
	SYM_BECOMES,        // 19
    SYM_BEGIN,          // 20
	SYM_END,            // 21
	SYM_IF,             // 22
	SYM_THEN,           // 23
	SYM_WHILE,          // 24
	SYM_DO,             // 25
	SYM_CALL,           // 26
	SYM_CONST,          // 27
	SYM_VAR,            // 28
	SYM_PROCEDURE,      // 29

    /***新增部分***/
    SYM_ELSE,           // 30
    SYM_FOR,            // 31
    SYM_STEP,           // 32
    SYM_UNTIL,          // 33
    SYM_RETURN,         // 34
    SYM_EXCLAMATION,    // 35
    SYM_PLUSBY,         // 36
    SYM_MINUSBY,        // 37
    SYM_MULTIPLYBY,     // 38
    SYM_DIVIDEBY,       // 39
    SYM_AND,            // 40
    SYM_OR,             // 41
    SYM_NOTE            // 42
    /***新增部分***/
};

const char* symtypeDescription[] = {
        "SYM_NULL",
        "SYM_IDENTIFIER",
        "SYM_NUMBER",
        "SYM_PLUS",
        "SYM_MINUS",
        "SYM_TIMES",
        "SYM_SLASH",
        "SYM_ODD",
        " SYM_EQU",
        "SYM_NEQ",
        "SYM_LES",
        "SYM_LEQ",
        "SYM_GTR",
        "SYM_GEQ",
        "SYM_LPAREN",
        "SYM_RPAREN",
        "SYM_COMMA",
        "SYM_SEMICOLON",
        "SYM_PERIOD",
        "SYM_BECOMES",
        "SYM_BEGIN",
        "SYM_END",
        "SYM_IF",
        "SYM_THEN",
        "SYM_WHILE",
        "SYM_DO",
        "SYM_CALL",
        "SYM_CONST",
        "SYM_VAR",
        "SYM_PROCEDURE",

        /***新增部分***/
        "SYM_ELSE",
        "SYM_FOR",
        "SYM_STEP",
        "SYM_UNTIL",
        "SYM_RETURN",
        "SYM_EXCLAMATION",
        "SYM_PLUSBY",
        "SYM_MINUSBY",
        "SYM_MULTIPLYBY",
        "SYM_DIVIDEBY",
        "SYM_AND",
        "SYM_OR",
        "SYM_NOTE"
        /***新增部分***/
};

enum idtype
{
	ID_CONSTANT, ID_VARIABLE, ID_PROCEDURE
};

enum opcode
{
	LIT, OPR, LOD, STO, CAL, INT, JMP, JPC
};

enum oprcode
{
	OPR_RET, OPR_NEG, OPR_ADD, OPR_MIN,
	OPR_MUL, OPR_DIV, OPR_ODD, OPR_EQU,
	OPR_NEQ, OPR_LES, OPR_LEQ, OPR_GTR,
	OPR_GEQ
};


typedef struct
{
	int f; // function code
	int l; // level
	int a; // displacement address
} instruction;

//////////////////////////////////////////////////////////////////////
const char* err_msg[] =
{
/*  0 */    "",
/*  1 */    "Found ':=' when expecting '='.",
/*  2 */    "There must be a number to follow '='.",
/*  3 */    "There must be an '=' to follow the identifier.",
/*  4 */    "There must be an identifier to follow 'const', 'var', or 'procedure'.",
/*  5 */    "Missing ',' or ';'.",
/*  6 */    "Incorrect procedure name.",
/*  7 */    "Statement expected.",
/*  8 */    "Follow the statement is an incorrect symbol.",
/*  9 */    "'.' expected.",
/* 10 */    "';' expected.",
/* 11 */    "Undeclared identifier.",
/* 12 */    "Illegal assignment.",
/* 13 */    "':=' expected.",
/* 14 */    "There must be an identifier to follow the 'call'.",
/* 15 */    "A constant or variable can not be called.",
/* 16 */    "'then' expected.",
/* 17 */    "';' or 'end' expected.",
/* 18 */    "'do' expected.",
/* 19 */    "Incorrect symbol.",
/* 20 */    "Relative operators expected.",
/* 21 */    "Procedure identifier can not be in an expression.",
/* 22 */    "Missing ')'.",
/* 23 */    "The symbol can not be followed by a factor.",
/* 24 */    "The symbol can not be as the beginning of an expression.",
/* 25 */    "The number is too great.",
/* 26 */    "A single / comment is not allowed.",
/* 27 */    "Step length is ambiguous.",
/* 28 */    "Until condition is ambiguous.",
/* 29 */    "",
/* 30 */    "",
/* 31 */    "",
/* 32 */    "There are too many levels."
};

//////////////////////////////////////////////////////////////////////
char ch;         // last character read
int  sym;        // last symbol read
char id[MAXIDLEN + 1]; // last identifier read
int  num;        // last number read
int  cc;         // character count
int  ll;         // le length
int  kk;
int  err;
int  cx;         // index of current instruction to be generated.
int  level = 0;
int  tx = 0;

char line[80];

instruction code[CXMAX];

const char* word[NRW + 1] =
{
	"", /* place holder */
	"begin", "call", "const", "do", "end","if",
	"odd", "procedure", "then", "var", "while", "else", "for", "step", "until", "do", "return", "//"
};

int wsym[NRW + 1] =
{
	SYM_NULL, SYM_BEGIN, SYM_CALL, SYM_CONST, SYM_DO, SYM_END,
	SYM_IF, SYM_ODD, SYM_PROCEDURE, SYM_THEN, SYM_VAR, SYM_WHILE, SYM_ELSE, SYM_FOR, SYM_STEP, SYM_UNTIL, SYM_DO, SYM_RETURN, SYM_NOTE
};

int ssym[NSYM + 1] =
{
	SYM_NULL, SYM_PLUS, SYM_MINUS, SYM_TIMES, SYM_SLASH,
	SYM_LPAREN, SYM_RPAREN, SYM_EQU, SYM_COMMA, SYM_PERIOD, SYM_SEMICOLON, SYM_EXCLAMATION, SYM_AND, SYM_OR
};

char csym[NSYM + 1] =
{
	' ', '+', '-', '*', '/', '(', ')', '=', ',', '.', ';', '!', '&', '|'
};

//////////////////////////////////////////////////////////////////////
int dx;  // data allocation index

#define MAXINS   8
const char* mnemonic[MAXINS] =
{
	"LIT", "OPR", "LOD", "STO", "CAL", "INT", "JMP", "JPC"
};

typedef struct
{
	char name[MAXIDLEN + 1];
	int  kind;
	int  value;
} comtab;

comtab table[TXMAX];

typedef struct
{
	char  name[MAXIDLEN + 1];
	int   kind;
	short level;
	short address;
} mask;

FILE* infile;

// EOF pl0.h
