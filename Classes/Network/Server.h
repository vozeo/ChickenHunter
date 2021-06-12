#ifndef SERVER_H
#define SERVER_H

#include "yasio/yasio.hpp"
#include "TransStructure.h"
#include <map>
#include <string>

struct SPlayer
{
	bool alive;
	float position_x, position_y;
	int hp;
};

struct SMap
{
	SPlayer player[MAX_CONNECTIONS];
};

using namespace std;
using namespace yasio;
using namespace yasio::inet;

class CHServer
{
	io_service* server;
	bool uid_usage[MAX_CONNECTIONS] = { 0 };
	map<transport_handle_t, int> uid;
	map<int, string> player_name;
protected:
	int get_unused_uid();//注意 调用会自动分配uid
	bool delete_uid(int id);
	
public:
	PlayerAction paction[MAX_CONNECTIONS];
	RoomInformation room;
	MapInformation map_trans;
	SMap map;
public:
	CHServer(const char* ip, unsigned short port = 25595);
	~CHServer();
	void listen();
	void update();
};

#endif 
