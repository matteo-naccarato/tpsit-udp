#include "includes/Address.hpp"
#include "includes/utilities.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_MSG 1024


int main(int argc, char* argv[]) {

	// controllo parametri
	if (argc != 4) {
		printf("USAGE: %s IP PORT MSG\n", argv[0]);
		return -1;
	}

	// assegnazione variabili di supporto
	char* ip = argv[1];
	int port = atoi(argv[2]);
	char* msg = argv[3];

	// creazione Address del server
	Address server(ip, port);

	// creazione socket
	int socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	if (socket_id == -1) 
		errore((char*)"socket()", -2);

	// preparazione invio messaggio
	struct sockaddr_in* add_server = server.getAddress();
	// invio messaggio con la sendto()
	int rc = sendto(socket_id,
					msg,
					strlen(msg)+1,
					0,
					(struct sockaddr*) add_server,
					(socklen_t) sizeof(sockaddr_in));
	if (rc != strlen(msg)+1) 
		errore((char*)"sendto()", -3);
	printf("sent to %s '%s'\n", server.toString(), msg);

	// preparazione ricezione messaggio
	char buffer[MAX_MSG + 1];
	Address* mittente = new Address();
	struct sockaddr_in mitt;
	int len_addr = sizeof(struct sockaddr_in);
	// ricezione messaggio con la recvfrom()
	rc = recvfrom(socket_id,
					buffer,
					MAX_MSG,
					0,
					(struct sockaddr*) &mitt,
					(socklen_t*) &len_addr);
	if (rc <= 0)
		errore((char*)"recvfrom()", -4);
	buffer[rc] = '\0';

	mittente->setAddress(&mitt);
	printf("received from %s '%s'\n", mittente->toString(), buffer);

	delete mittente;

	// chiusura socket
	close(socket_id);

	return 0;
}