#ifndef __SOCKET_UDP_
#define __SOCKET_UDP_

#include "Address.hpp"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_MSG 1024

class SocketUDP {
	private: int sock_id;

	public: SocketUDP(); // socket() :: CLIENT
			SocketUDP(Address); // bind() :: SERVER
			~SocketUDP(); // close()

			int 	invia(Address, char*);
			int 	invia(Address, void*, int);
			char*	ricevi(Address*);
			void* 	ricevi(Address*, int*);
};

SocketUDP::SocketUDP() {
	this->sock_id = socket(AF_INET, SOCK_DGRAM, 0);
}

SocketUDP::SocketUDP(Address address) : SocketUDP() {
	struct sockaddr_in* addr = address.getAddress();
	bind(this->sock_id,
		(struct sockaddr*) addr,
		(socklen_t) sizeof(struct sockaddr_in));
}

SocketUDP::~SocketUDP() { 
	close(this->sock_id);
	delete(&this->sock_id);
}

// invio di una stringa
int SocketUDP::invia(Address dest, char* msg) {
	int msg_len = strlen(msg) + 1;
	return invia(dest, (void*) msg, msg_len);
}

// invio generale
int SocketUDP::invia(Address dest, void* buffer, int len) {
	struct sockaddr_in* dest_addr = dest.getAddress();
	int rc = sendto(this->sock_id,
					buffer,
					len,
					0,
					(struct sockaddr*) dest_addr,
					(socklen_t) sizeof(struct sockaddr_in));
	return rc;
}

// ricezione di una stringa
char* SocketUDP::ricevi(Address* mitt) {
	int len;
	char* buffer = (char*) ricevi(mitt, &len);
	if (len > 0) buffer[len] = '\0';
	return buffer;
}

// ricezione generale
void* SocketUDP::ricevi(Address* mitt, int* len) {
	char* buffer = (char*) malloc(sizeof(char*) * (MAX_MSG+1));
	struct sockaddr_in addr;
	int len_addr = sizeof(struct sockaddr_in);

	int rc = recvfrom(this->sock_id,
						buffer,
						MAX_MSG,
						0,
						(struct sockaddr*) &addr,
						(socklen_t*) &len_addr);

	// assegnazione dell'indirizzo del mittente
	mitt->setAddress(&addr);
	*len = rc;

	if (rc > 0) return (void*) buffer;

	free(buffer);

	return NULL;
}

#endif //__SOCKET_UDP_