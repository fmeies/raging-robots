#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <netdb.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>

#include "rwserver.h"

Network::Network()
{
	int i;

	signal(SIGPIPE, SIG_IGN);

	sock = -1;
	for (i=0; i<MAXPLAYERS; i++)
		player[i] = -1;
}

int Network::create(int port)
{
	struct sockaddr_in addr;

	// create the socket, exit if we can't get one
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket");
		return 0;
	}

	// bind to the specified port.  Allow connections from any address
	bzero((char*)&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);
	if (bind(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0)
	{
		sleep(10);
		if (bind(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0)
		{
			perror("bind");
			close(sock);
			return 0;
		}
	}

	// listen for new connections, only allow 1 pending
	if (listen(sock, 1) < 0)
	{
		perror("listen");
		return 0;
	}

	// everything should be non-blocking so server never waits on a connection
	fcntl(sock, F_SETFL, O_NONBLOCK);

	printf("Listening on %d\n", port);
	return 1;
}

void newplayer_find_host(int sock)
{
	struct sockaddr_in sin;
	struct hostent *hp;
	int len;
	char buf[80];

	len = sizeof(sin);
	if (!(getpeername(sock, (struct sockaddr *) &sin, (socklen_t *) &len)))
	{
		hp = gethostbyaddr((char*) &sin.sin_addr.s_addr,
			sizeof(sin.sin_addr.s_addr), AF_INET);
		if (hp != NULL)
			(void) strcpy(buf, hp->h_name);
		else
			strcpy(buf, inet_ntoa(sin.sin_addr));
		printf("New connection from host %s\n", buf);
	}
}
void Network::check()
{
	fd_set read_fds;
	int i, newsock, len;
	struct timeval tv;
	unsigned long usecs;
	struct sockaddr_in naddr;

	FD_ZERO(&read_fds);
	FD_SET(sock, &read_fds);
	for (i=0; i<MAXPLAYERS; i++)
	{
		if (player[i] >= 0)
			FD_SET(player[i], &read_fds);
	}

	usecs = 100000;
	tv.tv_sec = usecs / 1000000L;
	tv.tv_usec = usecs % 1000000L;
	select(70, &read_fds, 0, 0, &tv);

	if (FD_ISSET(sock, &read_fds))
	{
		len = sizeof(naddr);
		newsock = accept(sock, (struct sockaddr *) &naddr, (socklen_t *) &len);
		if (newsock < 0)
		{
			perror("accept");
		}else
		{
			for (i=0; i<MAXPLAYERS; i++)
			{
				if (player[i]<0)
				{
					fcntl(newsock, F_SETFL, O_NONBLOCK);
					printf("Adding player %d\n", i);
					player[i] = newsock;
					newplayer_find_host(newsock);
					break;
				}
			}
		}
	}
	for (i=0; i<MAXPLAYERS; i++)
	{
		if (player[i] >= 0 && FD_ISSET(player[i], &read_fds))
		{
			do_read(player[i], i);
		}
	}
}

int c_packetLength(char *buf)
{
	switch (buf[0])
	{
		case C_POSITION:
			return sizeof(c_position);
            break;
		case S_POSITION:
			return sizeof(s_position);
            break;
		case C_REMOVE:
			return sizeof(c_remove);
            break;
		case H_PLAYER:
			return sizeof(h_player);
            break;
		default:
			return 4;
	}
}

// A packet split across two reads is completed by appending to the buffer, so
// it needs room for one whole packet beyond what a single read can deliver.
#define READ_CHUNK      1024
#define MAX_PACKET_SIZE sizeof(c_position)

void Network::do_read(int clientSock, int pl)
{
	char buf[READ_CHUNK + MAX_PACKET_SIZE];
	int i, in, pos, r;             // in: how many bytes have yet to be processed
                    //   pos: position currently being processed in the buffer
                     //  r: how much is read by subsequent reads (to finish
                      //    reading packets the initial read only got part of)

//	printf("GOT SOMETHING ");

	// read everything pending, up to 1k
	in = read(clientSock, buf, READ_CHUNK);

	if (in <= 0)
	{
		if (errno == EAGAIN)
		{
			// the kernel's telling us it couldn't read for some reason, but that
			// we shouldn't give up
			return;
		}
		// other errors mean we should bail on this socket.
		close(clientSock);
		player[pl] = -1;
		// newplayer_remove_player(pl);
		printf("Removing player %d\n", pl);
		for(i=0; i<MAXPLAYERS; i++) {
			if (player[i]>0) {
                sendPlayerRemove(player[i], pl);
            }
        }
		return;
	}
	pos = 0;
	while(pos<in)
	{
		while(in - pos < c_packetLength(buf + pos))
		{
			// we don't have a complete packet in the buffer yet, read more until
			// we do.
			r = read(clientSock, buf + in, c_packetLength(buf + pos) - (in - pos));
			if (r <= 0)
			{
				// do the same error checking as above, with the exception
				// that EAGAIN is probably fatal here too, so kill the
				// connection no matter what.
				close(clientSock);
				player[pl] = -1;
				// newplayer_remove_player(pl);
				for(i=0; i<MAXPLAYERS; i++) {
					if (player[i]>0) {
                        sendPlayerRemove(player[i], pl);
                    }
                }
				printf("Removing player %d\n", pl);
				return;
			}
			// we just added r bytes to the buffer, update our count of
			// unprocessed data
			in += r;
		}

		// pos is pointing to the first byte of the next packet to be
		// processed, figure out what type it is and call an appropriate
		// handler

		switch (buf[pos])
		{
			case S_POSITION:
				handleShotPosition(&buf[pos], pl);
				break;
			case C_POSITION:
				handlePlayerPosition(&buf[pos], pl);
				break;
			case H_PLAYER:
				handlePlayerHit(&buf[pos], pl);
				break;
			default:
				// oops!  Got a bad packet!
				printf("Bad packet type %d from player %d\n", buf[pos], pl);
 				break;
		}
		// we've now processed a packet, point to the byte immediately following
		// it and continue processing
		pos += c_packetLength(buf + pos);
	}
}

void Network::handleShotPosition(char *buf, int pl)
{
	int i;
	const s_position *pack = (s_position *) buf;
	//printf("Receiving position %f,%f from %d\n", pack->x, pack->y, pl);
	for (i=0; i<MAXPLAYERS; i++)
	{
		if (player[i]>=0 && pl != i) {
			sendShotPosition(player[i], pl, pack->x, pack->y, pack->angle, pack->valid);
		} 
	}
}

void Network::handlePlayerPosition(char *buf, int pl)
{
	int i;
	const c_position *pack = (c_position *) buf;
	//printf("Receiving position %f,%f from %d\n", pack->x, pack->y, pl);
	for (i=0; i<MAXPLAYERS; i++)
	{
		if (player[i]>=0 && pl != i) {
			sendPlayerPosition(player[i], pl, pack->x, pack->y, pack->angle, pack->legangle);
		} 
	}
}

void Network::handlePlayerHit(char *buf, int pl)
{
	int i;
	const h_player *pack = (h_player *) buf;
	//printf("Receiving hit player %d from %d\n", pack->who, pl);
	for (i=0; i<MAXPLAYERS; i++)
	{
		if (player[i]>=0 && i != pl) {
			sendPlayerHit(player[i], pl, pack->who);
		} 
	}
}

void Network::sendPacket(int outsock, int pl, char *buf)
{
	int i, len;

	switch(buf[0])
	{
		case C_POSITION:
			len = sizeof(c_position);
			break;
		case S_POSITION:
			len = sizeof(s_position);
			break;
		case C_REMOVE:
			len = sizeof(c_remove);
			break;
		case H_PLAYER:
			len = sizeof(h_player);
			break;
		default:
			len = 4;
			break;
	}

	if (write(outsock, buf, len) != len)
	{
		printf("Connection to client lost\n");
		//	newplayer_remove_player(pl);
		printf("Removing player %d\n", pl);
		close(player[pl]);
	    player[pl] = -1;
		for(i=0; i<MAXPLAYERS; i++) {
			if (player[i]>0) {
                sendPlayerRemove(player[i], pl);
            }
        }
	}
}

void Network::sendShotPosition(int outsock, int pl, double x, double y, double angle, bool valid)
{
	s_position pack;

	pack.type = S_POSITION;
	pack.x = x;
	pack.y = y;
    pack.angle = angle;
	pack.player = pl;
    pack.valid = valid;
	//printf("Sending position %f,%f to %d\n", x, y, pl);
	sendPacket(outsock, pl, (char *)&pack);
}

void Network::sendPlayerPosition(int outsock, int pl, double x, double y, double angle, double legangle)
{
	c_position pack;

	pack.type = C_POSITION;
	pack.x = x;
	pack.y = y;
    pack.angle = angle;
	pack.legangle = legangle;
	pack.player = pl;
	//printf("Sending position %f,%f to %d\n", x, y, pl);
	sendPacket(outsock, pl, (char *)&pack);
}

void Network::sendPlayerHit(int outsock, int pl, int who)
{
	h_player pack;

	pack.type = H_PLAYER;
	pack.who = who;
	pack.player = pl;
	//printf("Sending position %f,%f to %d\n", x, y, pl);
	sendPacket(outsock, pl, (char *)&pack);
}

void Network::sendPlayerRemove(int outsock, int pl)
{
	c_remove pack;

	pack.type = C_REMOVE;
	pack.player = pl;
	sendPacket(outsock, pl, (char *)&pack);
}

int main()
{
	Network n;

	n.create(7986);

	while(1)
	{
// printf("Update game loop here\n");
		n.check();
	}
	return 1;
}

