#include "Client.h"

using namespace yasio;
using namespace yasio::inet;

int makeConnection()
{
	io_service client(io_hostent{ "127.0.0.1", 25564 });
	client.open(0, YCK_TCP_CLIENT);
	io_event_cb_t cb;
	client.start(cb);
	return 0;
}

int sendStateUpdate()
{
	return 0;
}