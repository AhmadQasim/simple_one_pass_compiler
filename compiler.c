#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compiler.h"

int main(int argc, char *argv[]) {
	//FILE *fp = fopen(argv[1], "r");
	curr_token = malloc(sizeof(char)*MAX_TOKEN_LEN);
	FILE* fp=pre_processing(argv[1]);
	build_dfa();
	curr_state=states[0];
	token *head, *newtok, *curr_tok;
	curr_tok=malloc(sizeof(token));
	head=curr_tok;
	while((newtok=getNextToken(fp))!=NULL){curr_tok->next=newtok;curr_tok=newtok;}
	while(head->next!=NULL){head=head->next;printf("%s %s\n", head->val, head->type);}
}

void build_dfa(){
	int j=0;
	states = (dfa*) malloc(sizeof(dfa)*NUM_OF_STATES);
	FILE *fp_dfa=fopen("dfa.csv", "rb");
	int read = fread(states, sizeof(dfa), NUM_OF_STATES, fp_dfa);
	for(j=0;j<NUM_OF_STATES;j++) {states[j].next = (unsigned short*) calloc(ALPHALEN,sizeof(int));read = fread(states[j].next, sizeof(unsigned short), ALPHALEN, fp_dfa);}
	fclose(fp_dfa);
}

FILE* pre_processing(char *filename){
	FILE *fp = fopen(filename, "r");
	char chr;int state=1;
	char *buffer;buffer=malloc(sizeof(char)*100);
	int buffer_len=0,file_len=0;
	names *file_head, *curr_file;
	curr_file=malloc(sizeof(names));
	file_head=curr_file;
	while ((chr=fgetc(fp))!=EOF) {
		file_len++;
		if(chr!='"'&&state==11){buffer[buffer_len]=chr;buffer_len++;continue;} else if (chr=='"'&&state==11){state=1;buffer[buffer_len]='\0';buffer_len=0;names *new_file=malloc(sizeof(names));new_file->name=malloc(strlen(buffer));strcpy(new_file->name,buffer);new_file->pos=file_len-11-strlen(buffer);file_len=0;curr_file->next=new_file;curr_file=new_file;continue;}
		if(chr=='#'&&state==1){state++;continue;}else if(chr=='i'&&state==2){state++;continue;}else if(chr=='n'&&state==3){state++;continue;}else if(chr=='c'&&state==4){state++;continue;}else if(chr=='l'&&state==5){state++;continue;}else if(chr=='u'&&state==6){state++;continue;}else if(chr=='d'&&state==7){state++;continue;}else if(chr=='e'&&state==8){state++;continue;}else if(chr==' '&&state==9){state++;continue;}if(chr=='"'&&state==10){state++;continue;}else{state=1;}
	}
	fseek(fp,0,SEEK_SET);
	FILE* fp_dest=fopen(strcat(filename,".c"), "w+");
	while(file_head->next!=NULL){
		file_head=file_head->next;
		buffer=malloc(file_head->pos+1);
		file_len=fread(buffer,1,file_head->pos,fp);
		fwrite(buffer,1,file_head->pos,fp_dest);
		FILE *fp_temp=fopen(file_head->name,"r");
		fseek(fp_temp,0,SEEK_END);buffer_len=ftell(fp_temp);fseek(fp_temp,0,SEEK_SET);
		fseek(fp,11+strlen(file_head->name),SEEK_CUR);
		buffer=malloc(buffer_len+1);
		file_len=fread(buffer,1,buffer_len,fp_temp);
		fwrite(buffer,1,buffer_len,fp_dest);
		fclose(fp_temp);
	}
	buffer_len=ftell(fp);
	fseek(fp,0,SEEK_END);file_len=ftell(fp);fseek(fp,buffer_len,SEEK_SET);
	buffer_len=file_len-buffer_len;
	buffer=malloc(buffer_len+1);
	file_len=fread(buffer,1,buffer_len,fp);
	fwrite(buffer,1,buffer_len,fp_dest);
	fclose(fp);fseek(fp_dest,0,SEEK_SET);
	fflush(fp_dest);
	return fp_dest;
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
