#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compiler.h"

int main(int argc, char *argv[]) {
	FILE *fp = fopen(argv[1], "r");
	curr_token = malloc(sizeof(char)*MAX_TOKEN_LEN);
	build_dfa();
	curr_state=states[0];
	token *newtok;
	while((newtok=getNextToken(fp))!=NULL)printf("%s %s\n", newtok->type, newtok->val);
}

void build_dfa(){
	int j;
	states = (dfa*) malloc(sizeof(dfa)*NUM_OF_STATES);
	for(j=0;j<NUM_OF_STATES;j++){					//buidling main dfa
		states[j].state=j;
		states[j].next = (unsigned short*) calloc(ALPHALEN,sizeof(int));
		states[j].next[10]=IGNORE;states[j].next[32]=IGNORE;states[j].next[9]=IGNORE;states[j].next[34]=QOUTE;states[j].next[59]=SEMI_COLON;
		states[j].next[123]=109;states[j].next[125]=110; //braces
		states[j].next[91]=111;states[j].next[93]=112; //square brackets
		states[j].next[40]=113;states[j].next[41]=114; //round brackets
		states[j].next[61]=115;states[j].next[46]=116;states[j].next[44]=117;states[j].next[58]=118; //dot,colon,equal,comma
		states[j].next[47]=121;states[j].next[42]=122;states[j].next[37]=123;states[j].next[43]=124;states[j].next[45]=125;	//arithematic operators
		states[j].next[98]=1;states[j].next[99]=5;states[j].next[100]=13;states[j].next[101]=25;states[j].next[105]=32;states[j].next[114]=34;states[j].next[115]=39;states[j].next[102]=54;states[j].next[119]=59;states[j].next[118]=63; //keywords
		states[j].next[48]=66;states[j].next[49]=67;states[j].next[50]=68;states[j].next[51]=69;states[j].next[52]=70;states[j].next[53]=71;states[j].next[54]=72;states[j].next[55]=73;states[j].next[56]=74;states[j].next[57]=75; //numbers
		states[j].next[60]=131;states[j].next[62]=132;states[j].next[33]=135;
	}
	for(j=0;j<ALPHALEN;j++){						//handling comments and space removal states
		states[MULTI_LINE].next[j]=MULTI_LINE;
		states[SINGLE_LINE].next[j]=SINGLE_LINE;
		states[COMMENT_END1].next[j]=MULTI_LINE;
		states[QOUTE].next[j]=QOUTE;
	}
	for(j=0;j<10;j++) states[66+j].next[46]=66+j; //handling the float datatype
	states[MULTI_LINE].next[42]=COMMENT_END1;
	states[SINGLE_LINE].next[10]=IGNORE;
	states[121].next[47]=SINGLE_LINE;states[121].next[42]=MULTI_LINE;
	states[COMMENT_END1].next[47]=COMMENT_END2;
	states[QOUTE].next[34]=TOKEN_END;
	states[1].next[114]=2;states[2].next[101]=3;states[3].next[97]=4;states[4].next[107]=76; //break
	states[5].next[97]=6;states[6].next[115]=7;states[7].next[101]=77;states[5].next[104]=8;states[5].next[111]=10;	//case,char,const
	states[8].next[97]=9; states[9].next[114]=78;
	states[10].next[110]=11;states[11].next[115]=12;states[11].next[116]=79;
	states[13].next[101]=14;states[14].next[102]=15;states[16].next[97]=17;states[17].next[117]=18;states[19].next[108]=20;states[20].next[116]=80;states[13].next[111]=21;	//double, default
	states[21].next[111]=22;states[22].next[111]=23;states[23].next[111]=24;states[24].next[111]=81;
	states[25].next[108]=26;states[26].next[115]=27;states[27].next[101]=82;states[25].next[120]=28;	//else, extern
	states[28].next[116]=29;states[29].next[101]=30;states[30].next[114]=31;states[31].next[110]=83;
	states[32].next[102]=84;states[32].next[110]=33;states[33].next[116]=85;	//if, int
	states[34].next[101]=35;states[35].next[116]=36;states[36].next[117]=37;states[37].next[114]=38;states[38].next[110]=86; //return
	states[39].next[104]=40;states[40].next[111]=41;states[41].next[114]=42;states[42].next[116]=87;	//short,static,struct,switch
	states[39].next[116]=43;states[43].next[97]=44;states[44].next[116]=45;states[45].next[105]=46;states[46].next[99]=88;
	states[43].next[114]=47;states[47].next[117]=48;states[48].next[99]=49;states[49].next[116]=89;
	states[39].next[119]=50;states[50].next[105]=51;states[51].next[116]=52;states[52].next[99]=53;states[53].next[104]=90;
	states[54].next[111]=55;states[55].next[114]=91;	//float,for
	states[54].next[108]=56;states[56].next[111]=57;states[57].next[97]=58;states[58].next[116]=92;
	states[59].next[104]=60;states[60].next[105]=61;states[61].next[108]=62;states[62].next[101]=93;	//while
	states[63].next[111]=64;states[64].next[105]=65;states[65].next[100]=94;	//void
	states[131].next[61]=133;states[132].next[61]=134;states[135].next[61]=136;states[115].next[61]=137;	//relational operators
	
}

void write_dfa(){
	FILE* fp_dfa=fopen("dfa.csv","wb");
	fwrite (states , sizeof(dfa), NUM_OF_STATES, fp_dfa);
	for(j=0;j<NUM_OF_STATES;j++) fwrite (states[j].next , sizeof(unsigned short), ALPHALEN, fp_dfa);
	fclose(fp_dfa);
}

token* getNextToken(FILE *fp)  {	
	int i;
	int curr_token_len=1;
	char chr;
	token *new_tok;

	if (prev_state==-1) curr_token_len=0;

	while ((chr=fgetc(fp))!=EOF) {
		prev_state=curr_state.state;
		curr_state = states[curr_state.next[(int)chr]];
		if (curr_state.state==MULTI_LINE||curr_state.state==SINGLE_LINE||curr_state.state==COMMENT_END1||curr_state.state==COMMENT_END2){
			curr_token_len=0;continue;
		}
		else if (curr_state.state==IGNORE)
			continue;
		else{
			prev_prac=prac_state;
			prac_state=curr_state.state;
			curr_token[curr_token_len]=chr;
			curr_token_len++;
		}
		if (prev_state==curr_state.state||(curr_state.state<=KEYWORDS&&prev_state<=OTHERS)||((prev_state==EQL||prev_state==GT||prev_state==LT||prev_state==NOT)&&curr_state.state>ARITHEMATIC))
			continue;
		else {
			curr_token_len--;
			new_tok=malloc(sizeof(token));
			new_tok->type=token_type();
			new_tok->val=malloc(curr_token_len);
			curr_token[curr_token_len]='\0';
			strcpy(new_tok->val, curr_token);
			prev_state=curr_state.state;
			curr_token[0]=chr;
			return new_tok;
		}
	}
	if (curr_token!=NULL){
		new_tok=malloc(sizeof(token));
		new_tok->type=token_type();
		new_tok->val = malloc(curr_token_len);
		curr_token[curr_token_len]='\0';
		strcpy(new_tok->val, curr_token);
		curr_token=NULL;
	}
	return new_tok;
}

char* token_type(){
	printf("%d %d ", prev_prac, prac_state);
	if (prev_prac==QOUTE)
		return "literal";
	else if (prev_prac<=ID)
		return "ID";
	else if (prev_prac>ID&&prev_prac<KEYWORDS)
		return "keyword";
	else if (prev_prac>OTHERS&&prev_prac<PUNCTUATORS)
		return "punctuator";
	else if (prev_prac>PUNCTUATORS&&prev_prac<ARITHEMATIC)
		return "arithematic_op";
	else if (prev_prac>ARITHEMATIC&&prev_prac<RELATIONAL)
		return "relational_op";
	return "unknown";
}
