#include "includes/utilities.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PROMPT "$>"
#define EXIT_CMD "quit"
#define CHILD "./serverRecSend"


int main(int argc, char* argv[]) {

	// controllo parametri
	if (argc != 3) {
		printf("USAGE: %s PORT MESSAGE\n", argv[0]);
		return -1;
	}

	char* port = argv[1];
	char* msg = argv[2];

	char* server[] = {(char*)CHILD, port, msg, (char*) NULL};

	int pid = fork();

	if (!pid) {
		// eseguo il processo figlio
		execvp(CHILD, server);
		errore((char*)"execvp(server)", -5);
	}

	/* PROCESSO PADRE */
	printf("[%d] Figlio creato\n", getpid());

	// gestione input da tastiera
	printf("[%d] %s ", getpid(), PROMPT);
	char* command = inputStr();
	while(strcmp(command, EXIT_CMD)) {
		free(command);
		command = inputStr();
	}
	free(command);

	printf("[%d] Arrivederci!\n", getpid());

	return 0;
}