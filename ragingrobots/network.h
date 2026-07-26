#ifndef NETWORK_H
#define NETWORK_H 1

typedef int INT;
typedef unsigned char UCHAR;

// The server assigns player numbers and overwrites this field when it relays a
// packet, so an outgoing packet has no meaningful number of its own to send.
#define PLAYER_UNKNOWN (-1)

// Values are fixed explicitly: they travel on the wire and must stay
// identical to the server's enum in rwserver.h.
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

class GameController;

class Network {
public:
  	Network(GameController* c);

  	int create(const char *server, int port);
  	void doServer();
  	void doRead(int asock);
  	void sendPacket(char *packet);
	
  	void sendShotPosition(double x, double y, double angle, bool valid);
  	void sendPlayerPosition(double x, double y, double angle, double legangle);
  	void sendPlayerHit(int p);
	
	void handleShotPosition(char *packet);
	void handlePlayerPosition(char *packet);
	void handlePlayerHit(char *packet);
	void handlePlayerRemove(char *packet);
	
  	GameController* c;
	
  	int sock;
  	char buf[100000];
  	int totalread;
};

#endif
