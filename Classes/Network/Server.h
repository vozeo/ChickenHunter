#ifndef SERVER_H
#define SERVER_H

#include "yasio/yasio.hpp"
#include "TransStructure.h"
#include <map>
#include <string>

struct SPlayer
{
	bool alive = false;
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
	bool debug_mode = false;
	io_service* server;
	int connection_num = 0;
	bool started = false;
	bool uid_usage[MAX_CONNECTIONS] = { 0 };
	map<transport_handle_t, int> uid;
	map<int, transport_handle_t> uid_to_handle;
	map<int, string> player_name;
protected:
	int get_unused_uid();//注意 调用会自动分配uid
	bool delete_uid(int id);
	
public:
	PlayerAction paction[MAX_CONNECTIONS];
	RoomInformation room;
	MapInformation map_trans;
	SMap smap;
public:
	CHServer(const char* ip, unsigned short port = 25595);
	~CHServer();
	void listen();
	void map_init(int seed = 19260817);
	void map_update();
	bool startGame();
	bool GameIsStarted();
	int getConnectionNum();
	void open_debug_mode();
	void room_update();
};

extern CHServer* chserver;


#endif 
