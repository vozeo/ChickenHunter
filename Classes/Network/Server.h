#ifndef SERVER_H
#define SERVER_H

#include "yasio/yasio.hpp"
#include "TransStructure.h"
#include <map>
#include <string>
#include "Network/Client.h"

struct SPlayer {
    bool alive = false;
    float position_x, position_y;
    int hp;
};

using namespace std;
using namespace yasio;
using namespace yasio::inet;

class CHServer {
    bool debug_mode = false;
    io_service *m_server;
    int m_connection_num = 0;
    bool m_started = false;
    bool m_client_get_started[MAX_CONNECTIONS] = {0};
    bool m_uid_usage[MAX_CONNECTIONS] = {0};
    map<transport_handle_t, int> m_handle_to_uid;
    map<int, transport_handle_t> m_uid_to_handle;
    map<int, string> m_player_name;
protected:
    int getUnusedUid();//ע�� ���û��Զ�����uid
    bool deleteUid(int id);

protected:
    CHClient *ai_client[MAX_CONNECTIONS];

public:
    PlayerAction paction[MAX_CONNECTIONS];
    RoomInformation m_room;
    MapInformation m_map_trans;
    //bool ai_player[MAX_CONNECTIONS] = 0;
    int ai_player_num = 0;

public:
    CHServer(const char *ip, unsigned short port = 25595);

    ~CHServer();

    void listen();

    void mapUploadInit();

    void mapUpload();

    bool UidIsUsed(int uid);

    bool startGame();

    void closeGame();

    bool GameIsStarted();

    int getConnectionNum();

    void openDebugMode();

    void roomUpdate();

    bool addAi();

    bool deleteAi();

    void mapInformationInit(MapInformationInit mii);
};

extern CHServer *chserver;


#endif 
