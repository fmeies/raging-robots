#ifndef NETWORK_H
#define NETWORK_H 1

#define MAXPLAYERS 3

typedef int INT;
typedef unsigned char UCHAR;

// Values are fixed explicitly: they travel on the wire and must stay
// identical to the client's enum in ragingrobots/network.h.
enum c_packet_types {
	C_POSITION = 1,
	C_REMOVE   = 2,
	S_POSITION = 4,
	H_PLAYER   = 5
};

typedef struct {
	UCHAR type;
   	double x;
   	double y;
    double angle;
	double legangle;
	INT player;
} c_position;

typedef struct {
	UCHAR type;
	INT player;
} c_remove;

typedef struct {
	UCHAR type;
    double x;
    double y;
    double angle;
	INT player;
    bool valid;
} s_position;

typedef struct {
	UCHAR type;
    INT who;
	INT player;
} h_player;

class Network
{
public:
	Network();

	int create(int port);
	void check();
	void do_read(int clientSock, int pl);
	void sendPacket(int outsock, int pl, char *buf);

	void sendShotPosition(int outsock, int pl, double x, double y, double angle, bool valid);
	void sendPlayerPosition(int outsock, int pl, double x, double y, double angle, double legangle);
	void sendPlayerHit(int outsock, int pl, int who);
	void sendPlayerRemove(int outsock, int pl);

	void handleShotPosition(char *buf, int pl);
	void handlePlayerPosition(char *buf, int pl);
	void handlePlayerHit(char *buf, int pl);

	int sock;
	int player[MAXPLAYERS];
};

#endif

