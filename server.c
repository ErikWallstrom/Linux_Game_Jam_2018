#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "API2/log.h"

void onerror(void* userdata)
{
	(void)userdata;
	abort();
}

int main(int argc, char* argv[])
{
	if(argc < 2)
	{
		printf("Usage: %s [PORT]\n", argv[0]);
		return EXIT_FAILURE;
	}

	log_seterrorhandler(onerror, NULL);
	int serversocket = socket(AF_INET, SOCK_STREAM, 0);
	if(serversocket < 0)
	{
		log_error("%s", strerror(errno));
	}

	struct sockaddr_in serveraddr = {
		.sin_family = AF_INET,
		.sin_addr.s_addr = htonl(INADDR_ANY),
		.sin_port = htons(atoi(argv[1])),
		.sin_zero = {0} //Padding
	};

	int status = bind(
		serversocket, 
		(struct sockaddr*)&serveraddr, 
		sizeof(serveraddr)
	);
	if(status < 0)
	{
		log_error("%s", strerror(errno));
	}

#define MAX_QUEUE 5
	status = listen(serversocket, 5);
	if(status < 0)
	{
		log_error("%s", strerror(errno));
	}

	log_info("Listening on port %i", atoi(argv[1]));

	struct sockaddr_in clientaddr;
	int clientsocket = accept(
		serversocket, 
		(struct sockaddr*)&clientaddr, 
		&(socklen_t){sizeof(clientaddr)}
	);
	if(clientsocket < 0)
	{
		log_error("%s", strerror(errno));
	}

	char clientip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &clientaddr.sin_addr.s_addr, clientip, sizeof clientip);
	log_info("%s connected!", clientip);

	while(1)
	{
		char buffer[12];
		int bytes = read(clientsocket, buffer, sizeof buffer);
		if(bytes < 0)
		{
			log_warning("%s", strerror(errno));
			break;
		}
		else
		{
			log_info("Got: %s", buffer);
		}
	}

	close(clientsocket);
	close(serversocket);
}

