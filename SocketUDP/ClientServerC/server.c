#include "includes/utilities.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PROMPT "$>"
#define EXIT_CMD "quit"
#define IP_DHCP "0.0.0.0"
#define MAX_STR 1024

void* my_thread(void*);

// struttura parametri thread
typedef struct {
	char* msg;
	int sock_id;
	struct sockaddr_in dest;
	pthread_mutex_t mtx;
} Params;


int main(int argc, char* argv[]) {

	// controllo parametri
	if (argc != 3) {
		printf("USAGE: %s PORT MESSAGE\n", argv[0]);
		return -1;
	}


	// assegnazione variabili di supporto
	int port = atoi(argv[1]);
	char* msg = argv[2];

	// creazione socket
	int socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	if (socket_id == -1) errore("socket()", -2);

	// struttra per l'indirizzo del server
	struct sockaddr_in myself;
	myself.sin_family = AF_INET;
	inet_aton(IP_DHCP, &myself.sin_addr);
	myself.sin_port = htons(port);
	for (int i=0; i<8; i++) myself.sin_zero[i] = 0;

	// bind server
	int rc = bind(socket_id,
				(struct sockaddr*) &myself,
				(socklen_t) sizeof(struct sockaddr_in));
	if (rc != 0)
		errore("bind()", -3);

	// creazione thread
	pthread_t thread_id;
	// semaforo
	pthread_mutex_t mutex;
	rc = pthread_mutex_init(&mutex, NULL);
	if (rc != 0) errore("pthread_mutex_init()", -4);
	Params p = {msg, socket_id, myself, mutex};
	rc = pthread_create(&thread_id, NULL, my_thread, (void*) &p);
	if (rc != 0) errore("pthread_create()", -5);

	// gestione input da tastiera
	printf("%s ", PROMPT);
	char* command = inputStr();
	while(strcmp(command, EXIT_CMD)) {
		free(command);
		command = inputStr();
	}
	free(command);

	// rimozione semaforo
	rc = pthread_mutex_destroy(&mutex);
	if (rc != 0) errore("pthread_mutex_destroy()", -6);

	// chiusura socket
	close(socket_id);

	return 0;	
}


void* my_thread(void* param) {

	// ciclo per la ricezione e invio dei messaggi
	while(1) {

		Params* p = (Params*) param;

		// struttura per l'indirizzo del destinatario
		struct sockaddr_in dest;
		int addr_len = sizeof(struct sockaddr);
		char buffer[MAX_STR + 1];
		// ricezione messaggio con la recvfrom()
		int rc = recvfrom(p->sock_id, 
							buffer, 
							MAX_STR, 
							0, 
							(struct sockaddr*)&dest, 
							(socklen_t*)&addr_len);
		if (rc <= 0) errore("recvfrom()", -6);

		// regione critica
		pthread_mutex_lock(&p->mtx);
		char* ip = strdup(inet_ntoa(dest.sin_addr));
		pthread_mutex_unlock(&p->mtx);
		// fine regione critica
		int port = ntohs(dest.sin_port);	
		buffer[rc] = '\0';
		printf("received from [%s:%d] '%s'\n", ip, port, buffer);

		// invio risposta con la sendto()
		rc = sendto(p->sock_id,
					p->msg,
					strlen(p->msg)+1,
					0,
					(struct sockaddr*) &dest,
					(socklen_t) sizeof(struct sockaddr_in));
		if (rc != strlen(p->msg)+1)
			errore("sendto()", -7);
		printf("%s sent to [%s:%d] '%s'\n", PROMPT, ip, port, p->msg);
		printf("%s ", PROMPT);
		fflush(stdout);
		free(ip);
	}
	
	return NULL;
}