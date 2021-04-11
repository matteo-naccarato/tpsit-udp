#include "includes/Address.hpp"


int main(int argc, char* argv[]) {

	Address a((char*)IP_LO, 5000);
	printf("A > %s\n", a.toString());

	if (5000 != a.getPort())
		return -1;

	if (strcmp(a.getIp(), IP_LO) != 0)
		return -2; 

	Address b(a);
	if (a.getPort() != b.getPort())
		return -3;

	if (strcmp(a.getIp(), b.getIp()) != 0)
		return -4;

	printf("B > %s\n", b.toString());

	printf("Cambio IP e Porta di A\n");
	a.setIp((char*)"192.168.10.1");
	a.setPort(8045);
	printf("IP A > %s\n", a.getIp());
	printf("Porta A > %d\n", a.getPort());

	printf("B > %s\n", b.toString());

	Address c(a.getAddress());

	printf("C > %s\n", c.toString());
}