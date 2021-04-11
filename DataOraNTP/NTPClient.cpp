// RFC - NTP
// https://tools.ietf.org/html/rfc1361#:~:text=Leap%20Indicator%20(LI)%3A%20This,minute%20has%2061%20seconds%2010

// https://lettier.github.io/posts/2016-04-26-lets-make-a-ntp-client-in-c.html

#include <netdb.h>
#include "includes/SocketUDP.hpp"

#define NTP_TIMESTAMP_DELTA 2208988800ull // NTP time-stamp of 1 Jan 1970 or put another way 2,208,988,800 unsigned long long seconds.
#define IP "216.239.35.0" // ip of time.google.com
#define PORT_NTP 123

// uint8_t -> unsigned 8-bit integer

typedef struct {
	uint8_t li_vn_mode; // 8 bits. li, vn, and mode
						/* 
							Leap Indicator (LI): This is a two-bit code warning of an impending
	   							leap second to be inserted/deleted in the last minute of the current
	   							day, with bit 0 and bit 1, respectively, coded as follows:

							      LI       Value     Meaning
							      -------------------------------------------------------
							      00       0         no warning
							      01       1         last minute has 61 seconds
							      10       2         last minute has 59 seconds)
							      11       3         alarm condition (clock not synchronized)
					    */ 
						/*
							Version Number (VN): This is a three-bit integer indicating the NTP
   								version number, currently 3.
						*/
						/*
							Mode: This is a three-bit integer indicating the mode, with values
							   defined as follows:

							      Mode     Meaning
							      ------------------------------------
							      0        reserved
							      1        symmetric active
							      2        symmetric passive
							      3        client
							      4        server
							      5        broadcast
							      6        reserved for NTP control message
							      7        reserved for private use
						*/

	uint8_t stratum; // 8 bits. Stratum level of the local clock
						/*
							Stratum: This is a eight-bit integer indicating the stratum level of
							   the local clock, with values defined as follows:

							      Stratum  Meaning
							      ----------------------------------------------
							      0        unspecified or unavailable
							      1        primary reference (e.g., radio clock)
							      2-15     secondary reference (via NTP or SNTP)
							      16-255   reserved
						*/ 
	uint8_t poll;	// 8 bits. Maximum interval between successive messages
	uint8_t precision; // 8 bits. Precision of the local clock

	uint32_t rootDelay; 		// 32 bits. Total round trip delay time
	uint32_t rootDispersion; 	// 32 bits. Max error aloud from primary clock source
	uint32_t refId; 			// 32 bits. Reference clock identifier

	uint32_t refTm_s;			// 32 bits. Reference time-stamp second
	uint32_t refTm_f;			// 32 bits. Reference time-stamp fraction of a second

	uint32_t origTm_s;			// 32 bits. Originate time-stamp seconds
		uint32_t origTm_f;			// 32 bits. Originate time-stamp fraction of a second

	uint32_t rxTm_s;			// 32 bits. Received time-stamp seconds
	uint32_t rxTm_f;			// 32 bits. Received time-stamp fraction of a second

	uint32_t txTm_s;			// 32 bits and the most important field the client cares about.
								//	Transmit time-stamp second
	uint32_t txTm_f;			// 32 bits. Transmit time-stamp fraction of a second

} ntp_packet;	// Total: 384 bits or 48 Bytes





int main(int argc, char* argv[]) {

	// create and zero out the packet (all 48 Bytes worth)
	ntp_packet packet = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	// memset > converts the value of a character to unsigned character 
	// 				and copies it into each of first n character of the 
	//				object pointed by the given str[]
	memset( &packet, 0, sizeof( ntp_packet) );

	// set the first Byte's bits to 00,011,011 for li = 0, vn = 3, and mode = 3. The rest will be left set to zero
	*( ( char * ) &packet + 0) = 0X1b; // represents 27 in base 10 or 00011011 in base 2


	SocketUDP* myself = new SocketUDP();
	Address server((char*) IP, PORT_NTP);


	int len = sizeof(ntp_packet);

	int rc = myself->invia(server, (void*) &packet, len);
	if (rc != sizeof(ntp_packet)) errore((char*) "invia()", -2);

	ntp_packet* resp = (ntp_packet*) myself->ricevi(&server, &len);
	if (resp == NULL) errore((char*) "ricevi()", -3);

	// These two fields contain the time-stamp seconds as the packet left the NTP server.
	// The number of seconds correspond to the seconds passed since 1900.
	// ntohl() converts the bit/byte order from the network's to host's "endianness".	
	resp->txTm_s = ntohl(resp->txTm_s);
	resp->txTm_f = ntohl(resp->txTm_f);

	// Extract the 32 bits that represent the time-stamp seconds (since NTP epoch) from when the packet left the server.
	// Subtract 70 years worth of seconds from the seconds since 1900.
	// This leaves the seconds since the UNIX epoch of 1970.
	// (1900)------------------(1970)**************************************(Time Packet Left the Server)
	time_t txTm = (time_t) (resp->txTm_s - NTP_TIMESTAMP_DELTA);

	printf("Time: %s\n", ctime((const time_t*) &txTm));

	return 0;
}