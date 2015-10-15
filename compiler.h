#ifndef __COMPILER_H__
#define __COMPILER_H__

#define ALPHALEN 127
#define TOKEN_END 0
#define MULTI_LINE	100
#define SINGLE_LINE 101
#define COMMENT_END1 102
#define IGNORE 103
#define QOUTE 104
#define COMMENT_END2 105
#define SEMI_COLON 106
#define EQL 115
#define LT 131
#define GT 132
#define NOT 135
#define NUM_OF_STATES 150
#define MAX_TOKEN_LEN 1024

#define ID 75
#define KEYWORDS 95
#define OTHERS 100
#define PUNCTUATORS 120
#define ARITHEMATIC 130
#define RELATIONAL	140

typedef struct dfas {
	int state;
	unsigned short *next;
}dfa;

typedef struct tokens {
	char *type;
	char* val;
	struct tokens* next;
}token;

typedef struct name_linkedlist{
	char *name;
	int pos;
	struct name_linkedlist *next;
}names;

dfa *states;
char *curr_token;
dfa curr_state;
int prev_state=-1, prev_prac=0, prac_state=0;

void build_dfa();
token* getNextToken(FILE* fp);
char* token_type();
void write_dfa();
FILE* pre_processing(char *filename);

#endif
