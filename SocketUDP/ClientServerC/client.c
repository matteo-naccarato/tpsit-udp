#include "includes/utilities.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAX_MSG 1024

int main(int argc, char* argv[]) {

	// controllo parametri
	if (argc != 4) {
		printf("USAGE: %s IP PORT MESSAGE\n", argv[0]);
		return -1;
	}

	// assegnazione variabili di supporto
	char* ip = argv[1];
	int port = atoi(argv[2]);
	char* msg = argv[3];

	// creazione socket
	int socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	if (socket_id == -1) errore("socket()", -2);

	// creazione struttura server
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	inet_aton(ip, &server.sin_addr);
	server.sin_port = htons(port);
	for (int i=0; i<8; i++) server.sin_zero[i] = 0;

	// invio messaggio con la sendto()
	int rc = sendto(socket_id,
					msg,
					strlen(msg)+1,
					0,
					(struct sockaddr*) &server,
					(socklen_t) sizeof(struct sockaddr_in));
	if (rc != strlen(msg)+1)
		errore("sendto()", -3);
	printf("sent to [%s:%d] '%s'\n", ip, port, msg);

	// preparo ricezione messaggio
	char* buffer = (char*) malloc(sizeof(char) * (MAX_MSG+1));
	struct sockaddr_in mitt;
	int len_addr = sizeof(struct sockaddr_in);
	// ricezione messaggio con la recvfrom()
	rc = recvfrom(socket_id,
					buffer,
					MAX_MSG,
					0,
					(struct sockaddr*) &mitt,
					(socklen_t*) &len_addr);
	if (rc <= 0) errore("recvfrom()", -4);
	buffer[rc] = '\0';

	char* ip_mitt = strdup(inet_ntoa(mitt.sin_addr));
	int port_mitt = ntohs(mitt.sin_port);

	printf("received from [%s:%d] '%s'\n", ip_mitt, port_mitt, buffer);
	free(ip_mitt);

	// chiusura socket
	close(socket_id);

	free(buffer);

	return 0;
}
