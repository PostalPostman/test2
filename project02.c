#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "sha256.h"

#define DIG_BIN_LEN 32
#define DIG_STR_LEN (DIG_BIN_LEN * 2)
#define MAX_PASS_LEN 64
 /* (e is shorthand for entry) 
 */
 struct entry {
     char password[MAX_PASS_LEN + 1];
     char hash[DIG_STR_LEN + 1];
     struct entry *next;
 } entry;
 

void sha256(char *dest, char *src) {
    // zero out the sha256 context
    struct sha256_ctx ctx;
    memset(&ctx, 0, sizeof(ctx));

    // zero out the binary version of the hash digest
    unsigned char dig_bin[DIG_BIN_LEN];
    memset(dig_bin, 0, DIG_BIN_LEN);

    // zero out the string version of the hash digest
    memset(dest, 0, DIG_STR_LEN);

    // compute the binary hash digest
    __sha256_init_ctx(&ctx);
    __sha256_process_bytes(src, strlen(src), &ctx);
    __sha256_finish_ctx(&ctx, dig_bin);

    // convert it into a string of hexadecimal digits
    for (int i = 0; i < DIG_BIN_LEN; i++) {
        snprintf(dest, 3, "%02x", dig_bin[i]);
        dest += 2;
    }
} 

struct entry *createStandard(char *password){ //creates an entry for a copy of the buffer passed through to it
	//malloc start
	struct entry *e = malloc(sizeof(struct entry));
	if (!e) {
		printf("bad malloc\n");
		exit(-1);
	}
	memset(e, 0, sizeof(entry));
	//malloc end
	//variable assignment start
	strncpy(e->password, password, MAX_PASS_LEN);
	sha256(e->hash, password);
	// variable assignment end
	return e;
}
struct entry *createplus(char *password){ // creates a plus one entry and assigns buffer concatenated with 1 to entry
	//malloc start
	char* plus_one = malloc(sizeof(char)*MAX_PASS_LEN+1);
	if (!plus_one) {
		printf("bad malloc\n");
		exit(-1);
	}
	memset(plus_one, 0, sizeof(char)*MAX_PASS_LEN+1);
	struct entry *e = malloc(sizeof(struct entry));
	if (!e) {
		printf("bad malloc\n");
		exit(-1);
	}
	memset(e, 0, sizeof(entry));
	// malloc end
	//variable assignment start
	strncpy(plus_one,password,MAX_PASS_LEN);
	strcat(plus_one, "1");
	strncpy(e->password, plus_one, MAX_PASS_LEN);
	sha256(e->hash, plus_one);
	free(plus_one);
	//variable assignment end
	return e;
}
struct entry *createleet(char *password){ //creates a "leet" entry and assigns a leetified buffer 
	//malloc start
	char* leet = malloc(sizeof(char)*MAX_PASS_LEN);
	if (!leet) {
		printf("bad malloc\n");
		exit(-1);
	}
	struct entry *e = malloc(sizeof(struct entry));
	if (!e) {
		printf("bad malloc\n");
		exit(-1);
	}
	memset(e, 0, sizeof(entry));
	//malloc end
	//variable assignment start
	strncpy(leet, password,MAX_PASS_LEN);
	for (int b=0; b<strlen(leet);b++) { //leet relpacement loop
			if (leet[b]=='o') {
				leet[b]='0';
			}
			if (leet[b]=='e') {
				leet[b]='3';
			}
			if (leet[b]=='i') {
				leet[b]='!';
			}
			if (leet[b]=='a') {
				leet[b]='@';
			}
			if (leet[b]=='h') {
				leet[b]='#';
			}
			if (leet[b]=='s') {
				leet[b]='$';
			}
			if (leet[b]=='t') {
				leet[b]='+';
			}
		}
	strncpy(e->password, leet, MAX_PASS_LEN);
	sha256(e->hash, leet);
	free(leet);
	//variable assignment end
	return e;
}

void printList(struct entry *head){//writes out the list to the terminal
	while (head) {
		printf("%s %.5s...\n", head->password, head->hash);
		head = head->next;
	}
	printf("\n");
}

void insert(struct entry **head, struct entry *w_entry){ //(w_entry stands for wanted entry) links list and does an insertion sort
	if (*head == NULL) {// head empty
		*head = w_entry;
		return;
	}
	struct entry *current= *head;
	while(current->next!=NULL){
		if(!strcmp(current->hash,w_entry->hash)){ // if w_entry is duplicate then free it
			free(w_entry);
			return;
		}
		current=current->next;
	}
	if (strcmp(w_entry->hash, (*head)->hash) < 0) { // less than head
		w_entry->next = *head;
		*head = w_entry;
		return;
	}
	current=*head;
	while(current->next!=NULL){
		if (strcmp(current->next->hash, w_entry->hash) > 0) { //regular insertion case for ascending order
			w_entry->next=current->next;
			current->next= w_entry;
			return;
		}
		current=current->next;
	}
	if(strcmp(current->hash,w_entry->hash)<0&&current->next==NULL){ //insertion for end of the list
			current->next= w_entry;
			return;
		}
}
void writeOut(struct entry *head, FILE* fp){ //writes the list out to the file 
	//get length of list
	struct entry *current= head;
	int len=0;
	while(current){
		len++;
		current=current->next;
	}
	//done
	current=head;
	//start print
	fprintf(fp, "%d\n", len);
	while (current) {
		fprintf(fp, "%s,%s\n", current->hash, current->password);
		current = current->next;
	}
	//end print
}
int main(int argc, char** argv){
	struct entry *head=NULL;
	char buffer[MAX_PASS_LEN+1];
	memset(buffer, '\000', MAX_PASS_LEN+1); //initializes buffer to null
	if (argc<3||argc>4){
		exit(-1);
	}
	// open files
	FILE* fp_one= fopen(argv[1], "r");
	if(!fp_one){
		exit(-1);
	}
	FILE* fp_two= fopen(argv[2], "w");
	while(fscanf(fp_one, "%s", buffer)==1) { //loop for each entry in the read file
		struct entry *standard = createStandard(buffer);
		standard->next = NULL;
		insert(&head, standard);
		if (argc==4&&!strcmp(argv[argc-1],"-v")) {// verbose case goes for all the rest of the if statements that have this expression
			printf("inserting: %s\n", standard->password);
			printList(head);
		}
		struct entry *leet = createleet(buffer);
		leet->next = NULL;
		insert(&head, leet);
		if (argc==4&&!strcmp(argv[argc-1],"-v")) {
			printf("inserting: %s\n", leet->password);
			printList(head);
		}
		struct entry *plus_one = createplus(buffer);
		plus_one->next = NULL;
		insert(&head, plus_one);
		if (argc==4&&!strcmp(argv[argc-1],"-v")) {
			printf("inserting: %s\n", plus_one->password);
			printList(head);
		}
	}
	writeOut(head, fp_two);
	//close files
	fclose(fp_one);
	fclose(fp_two);
}
