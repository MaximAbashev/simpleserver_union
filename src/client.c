# include <stdio.h>
# include <stdlib.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <netinet/in.h>
# include <time.h>

# define SRV_PORT 5555
# define CLNT_PORT 4444
# define VERSION 2

int main ()
{
	short port;
	int sock, new_sock;
	int from_len, len;
	char buf[30];
	struct sockaddr_in clnt_addr, s_addr, new_s_addr;
#if VERSION == 1 /* TCP version = 1 */
	sock = socket (AF_INET, SOCK_STREAM, 0);
#else /* UDP version != 1 */
	sock = socket (AF_INET, SOCK_DGRAM, 0);
#endif
/* Socket options init */
	s_addr.sin_family = AF_INET;
	s_addr.sin_addr.s_addr = htonl (INADDR_LOOPBACK);
	s_addr.sin_port = htons (SRV_PORT);
#if VERSION == 1
	printf ("Connecting to first socket...\n");
	connect (sock, &s_addr, sizeof (s_addr));
	send (sock, "Can you give me the new port?\n", 30, 0);
	from_len = recv (sock, &port, sizeof (short), 0);
	printf ("I take port: %d\n", port);
	close (sock);
	sleep (3);
/*
 * Reconfigure and connect to new socket.
 */
	new_sock = socket (AF_INET, SOCK_STREAM, 0);
	new_s_addr.sin_family = AF_INET;
	new_s_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	new_s_addr.sin_port = htons(port);
	
	printf("Connecting to second socket...\n");
	connect (new_sock, &new_s_addr, sizeof(new_s_addr));
	send (new_sock, "How much o'clock?\n", 19, 0);
	from_len = recv (new_sock, buf, 30, 0);
	write (1, buf, from_len);
	close(new_sock);
#else /* UDP part */
	len = sizeof(struct sockaddr_in);
	printf("Sending message...\n");
	sendto (sock, "Can you give me new port?\n", 27, 0, (struct sockaddr *)&s_addr, sizeof(s_addr));
	printf("Waiting answer from server...\n");
	while (1)
    {
		from_len = recvfrom (sock, &port, sizeof(port), 0, (struct sockaddr *)&s_addr, &len);
		printf ("I take port: %d\n", port);
		if (from_len > 0)
        {
			close(sock);
			break;
		}
	}
	sleep(3);
	printf("Connecting to new socket...\n");
	new_sock = socket (AF_INET, SOCK_DGRAM, 0);
	new_s_addr.sin_family = AF_INET;
	new_s_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	new_s_addr.sin_port = htons(port);
	printf("Sending message...\n");
	sendto (new_sock, "How much o'clock?\n", 19, 0, (struct sockaddr *)&new_s_addr, sizeof(new_s_addr));
	printf("Waiting answer from new server...\n");
	while (2)
    {
		from_len = recvfrom (sock, buf, 30, 0, (struct sockaddr *)&new_s_addr, &len);
		if (from_len > 0)
        {
			break;
		}
	}
	write (1, buf, from_len);
	close(new_sock);
#endif	
	return 1;
}
