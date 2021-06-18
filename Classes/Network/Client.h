#ifndef CLIENT_H
#define CLIENT_H

//#include <string>
#include "yasio/yasio.hpp"
#include "TransStructure.h"

using namespace yasio;
using namespace yasio::inet;

class CHClient {
    io_service *m_client;
    transport_handle_t m_thandle = 0;
    int m_uid = 0;
    bool m_started = false;
    bool m_map_init_state = false;
public:
    RoomInformation m_room;
    MapInformation m_map;
    PlayerAction m_localaction;
    MapInformationInit m_map_information_init;
public:
    CHClient(const char *ip, unsigned short port = 25595);

    ~CHClient();

    void link();

    int getUid();

    void setName(const char *name);

    bool upload();

    bool isStarted();

    bool getMapInitState();

    void setMapInited();
};

extern CHClient *chclient;

bool isMultipleGame();

#endif 