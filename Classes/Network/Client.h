#ifndef CLIENT_H
#define CLIENT_H

//#include <string>
#include "yasio/yasio.hpp"
#include "TransStructure.h"

using namespace yasio;
using namespace yasio::inet;

class CHClient
{
	io_service* client;
	transport_handle_t thandle;
	int uid = 0;
	bool started = false;
public:
	RoomInformation room;
	MapInformation map;
public:
	CHClient(const char* ip, unsigned short port = 25595);
	~CHClient();
	void link();
	int getuid();
	void setName(const char* name);
	bool upload(PlayerAction action);
	bool isStarted();
};
extern CHClient* hunter_client;

#endif 