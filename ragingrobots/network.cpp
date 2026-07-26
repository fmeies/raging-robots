#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include "network.h"
#include "gamemodel.h"
#include "gamecontroller.h"

Network::Network(GameController* controller){
	c = controller;
	sock = -1;
	totalread = 0;
}

int Network::create(const char *server, int port)
{
	int s;
	struct sockaddr_in addr;
	struct hostent *hp;

	printf("Calling %s on port %d.\n", server, port);
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("I can't create a socket\n");
		return 0;
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);

	if ((addr.sin_addr.s_addr = inet_addr(server)) == INADDR_NONE)
	{
		if ((hp = gethostbyname(server)) == NULL)
		{
			printf("Who is %s?\n", server);
			return 0;
		}else
		{
			// h_addr points at 4 bytes; reading it as long would read 8 on LP64
			addr.sin_addr = *(struct in_addr *) hp->h_addr;
		}
	}

	if (connect(s, (struct sockaddr *) & addr, sizeof(addr)) < 0)
	{
		printf("Server not listening!\n");
		return 0;
	}
	printf("Got connection.\n");

	sock = s;
	return 1;
}

void Network::doServer()
{
	fd_set read_fds;
	int s;
	struct timeval tv;

	FD_ZERO(&read_fds);
	FD_SET(sock, &read_fds);
	tv.tv_sec = 0;
	tv.tv_usec = 50000;

	if((s=select(32, &read_fds, 0, 0, &tv)) > 0)
	{
		if(FD_ISSET(sock, &read_fds))
			doRead(sock);
	}
}

int packetLength(char *packet)
{
	switch (packet[0])
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

void Network::doRead(int asock)
{
	int in, pos, r;

	in = read(asock, buf, 1024);
	if(in > 0)
	{
		totalread += in;
		pos = 0;
		while(pos<in)
		{
			while(in - pos < packetLength(buf+pos))
			{
				r = read(asock, buf+in, packetLength(buf+pos) - (in - pos));
				if (r<=0)
				{
					printf("Read error from server, exiting\n");
					return;
				}
				in += r;
				totalread += r;
			}

			switch(buf[pos])
			{
				case S_POSITION:
					handleShotPosition(&buf[pos]);
					break;
				case C_POSITION:
					handlePlayerPosition(&buf[pos]);
					break;
				case H_PLAYER:
					handlePlayerHit(&buf[pos]);
					break;
				case C_REMOVE:
					handlePlayerRemove(&buf[pos]);
					break;
				default:
					break;
			}
			pos += packetLength(buf+pos);
		}
	}
}

void Network::sendPacket(char *packet)
{
	int len;

	switch(packet[0])
	{
		case C_POSITION:
			len = sizeof(c_position);
			break;
		case S_POSITION:
		    len = sizeof(s_position);
		    break;
		case H_PLAYER:
		    len = sizeof(h_player);
		    break;
		default:
			len = 4;
			break;
	}

	if (write(sock, packet, len) != len)
	{
		printf("Connection to server lost\n");
	}
}

void Network::sendShotPosition(double x, double y, double angle, bool valid)
{
	s_position pack;

	pack.type = S_POSITION;
	pack.x = x;
	pack.y = y;
	pack.angle = angle;
	pack.valid = valid;
	pack.player = PLAYER_UNKNOWN;
	sendPacket((char *)&pack);
}

void Network::sendPlayerPosition(double x, double y, double angle, double legangle)
{
	c_position pack;

	pack.type = C_POSITION;
	pack.x = x;
	pack.y = y;
	pack.angle = angle;
	pack.legangle = legangle;
	pack.player = PLAYER_UNKNOWN;
	sendPacket((char *)&pack);
}

void Network::sendPlayerHit(int p)
{
	h_player pack;

	pack.type = H_PLAYER;
	// player who has been hit
	pack.who = p;
	pack.player = PLAYER_UNKNOWN;
	sendPacket((char *)&pack);
}

void Network::handleShotPosition(char *packet)
{
    const s_position *pack = (s_position *) packet;
	if ((pack->player>=0) && (pack->player<MAXPLAYERS)) {
		c->handleShotPosition(pack->player, pack->valid, pack->x, pack->y, pack->angle);
    }
}

void Network::handlePlayerPosition(char *packet)
{
	const c_position *pack = (c_position *) packet;
	if ((pack->player>=0) && (pack->player<MAXPLAYERS)) {
		c->handlePlayerPosition(pack->player, pack->x, pack->y, pack->angle, pack->legangle);
    }
}

void Network::handlePlayerHit(char *packet)
{
    const h_player *pack = (h_player *) packet;
	if ((pack->who>=0) && (pack->who<MAXPLAYERS)) {
        // who is the player beeing hit
        c->handlePlayerHit(pack->who);
    }
}

void Network::handlePlayerRemove(char *packet)
{
	const c_remove *pack = (c_remove *) packet;
	if ((pack->player>=0) && (pack->player<MAXPLAYERS)) {
		c->handlePlayerRemove(pack->player);
	}
}
