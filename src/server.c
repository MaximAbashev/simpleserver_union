# include <stdio.h>
# include <stdlib.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <netinet/in.h>
# include <time.h>

# define SRV_PORT 5555
# define CLNT_PORT 4444
# define VERSION 2

int main () {
	short port;
	int sock, new_sock, last_sock, from_len, fd_fork, contact, contact2, len, 
        len2;
	long int ttime;
	char buf[30];
	struct sockaddr_in s_addr, clnt_addr, new_s_addr;
#if VERSION == 1
	sock = socket (AF_INET, SOCK_STREAM, 0); /* TCP socket */
#else 
	sock = socket (AF_INET, SOCK_DGRAM, 0); /* UDP socket */
#endif
/*
 * Initiate socket options.
 */
	s_addr.sin_family = AF_INET;
	s_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	s_addr.sin_port = htons(SRV_PORT);
	if(bind (sock, (struct sockaddr *)&s_addr, sizeof(s_addr)) < 0)
    {
		perror ("First bind error!\n");
		exit(1);
	}
#if VERSION == 1	
	listen(sock, 2);
#endif	
	port = 4449;	
	len = sizeof(s_addr);
	while(1)
    {
/* TCP version = 1 */
#if VERSION == 1        
		contact = accept (sock, (struct sockaddr *)&s_addr, &len);
		if(contact == (-1))
        {
			perror ("Connect error!\n");
			exit(1);
		}
		from_len = recv (contact, buf, 30, 0);
		write (1, buf, from_len);
		port++;
		send (contact, &port, sizeof (short), 0);
		if (from_len > 0)
        {
		    fd_fork = fork();
		}
		if (fd_fork == (-1))
        {
			perror ("Fork create error!\n");
			exit (1);
		}
        else if (fd_fork == 0) /* doughter part */
        {
			close (sock);
			new_sock = socket (AF_INET, SOCK_STREAM, 0);					
			new_s_addr.sin_family = AF_INET;
			new_s_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
			new_s_addr.sin_port = htons(port);
            bind (new_sock, (struct sockaddr *)&new_s_addr, 
                    sizeof (new_s_addr));

			listen (new_sock, 2);
			len2 = sizeof (new_s_addr);
			while (2)
            {
			    contact2 = 
                    accept (new_sock, (struct sockaddr *)&new_s_addr, &len2);
				if (contact2 == (-1))
                {
				    perror ("Connect error!\n");
					exit (1);
				}
				    from_len = recv (contact2, buf, 19, 0);
					if (from_len > 0) break;
			}
			write (1, buf, from_len);
			ttime = time (NULL);
			send(contact2, ctime(&ttime), 30, 0);
		}
#else /* UDP */
		while (2) 
		{
			from_len = 
                recvfrom (sock, buf, 27, 0, (struct sockaddr *)&s_addr, &len);
			if(from_len > 0) {
				write (1, buf, from_len);
				port++;
				break;
			}
		}
		printf ("Sending message to client...\n");
		sendto (sock, &port, sizeof(port), 0,
                (struct sockaddr *)&s_addr, sizeof(s_addr));	
		fd_fork = fork();
		if (fd_fork == (-1))
        {
			perror ("Fork create error!\n");
			exit(1);
		}
        else if (fd_fork == 0)
        {
/* Reconfigure socket. */
			close (sock);
			new_sock = socket (AF_INET, SOCK_DGRAM, 0);
			new_s_addr.sin_family = AF_INET;
			new_s_addr.sin_addr.s_addr = htonl (INADDR_LOOPBACK);
			new_s_addr.sin_port = htons (port);
			if (bind (new_sock, (struct sockaddr *)&new_s_addr, 
                sizeof (new_s_addr)) < 0)
            {
				perror("Second bind error!\n");
				exit(1);
			}
			len = sizeof (struct sockaddr_in); /* for recvfrom */
			while (3)
            {
				from_len = recvfrom (new_sock, buf, 19, 0,
                        (struct sockaddr *)&new_s_addr, &len);
				if(from_len > 0)
                {
					write (1, buf, from_len);
					ttime = time (NULL);
					break;
				}
			}
		printf ("Sending message to client... again...\n");
		sendto (new_sock, ctime(&ttime), 30, 0,
                (struct sockaddr *)&new_s_addr, sizeof(new_s_addr));
		}
#endif		
	}
	close (new_sock);				
	close (sock);				
	return 1;
}
