#include "Server.h"
#include <cstdlib>
#include <iostream>
using namespace std;

//创建一个服务端
CHServer* chserver = nullptr;

int CHServer::getUnusedUid()
{
    for (int i = 1; i < MAX_CONNECTIONS; i++)
        if (m_uid_usage[i] == false)
        {
            m_uid_usage[i] = true;
            m_connection_num++;
            return i;
        }
    return -1;
}

bool CHServer::deleteUid(int id)
{
    if (m_uid_usage[id] == false)
        return false;
    m_uid_usage[id] = false;
    m_connection_num--;
    return true;
}

CHServer::CHServer(const char* ip, unsigned short port)
{
    m_server = new io_service({ ip,port });//配置信道
    m_server->set_option(YOPT_S_DEFERRED_EVENT, 0);//在网络线程中调度网络事件
    m_server->start([&](event_ptr&& ev) { //添加网络线程函数
        switch (ev->kind())
        {
        case YEK_PACKET: {//消息包事件
            auto packet = std::move(ev->packet());
            char header[HEAD_LENGTH + 1] = { 0 };
            memcpy(header, packet.data(), HEAD_LENGTH);
            auto thandle = ev->transport();
            if (strstr(header, "GU"))
            {
                if(debug_mode) cout << "uid:" << m_handle_to_uid[thandle] << " DEBUG#:GU" << endl;
                int id = m_handle_to_uid[thandle];
                char buf[HEAD_LENGTH + 8] = "SU\0";
                memcpy(buf + HEAD_LENGTH, &id, 4);
                m_server->write(thandle, buf, HEAD_LENGTH + 4);
            }
            else if (strstr(header, "SN"))
            {
                if (debug_mode) cout << "uid:" <<m_handle_to_uid[thandle] << " DEBUG#:SN" << endl;
                char buf[20] = { 0 };
                memcpy(buf, packet.data() + HEAD_LENGTH, packet.size() - HEAD_LENGTH);
                string s = buf;
                int id = m_handle_to_uid[thandle];
                m_player_name[id] = s;
                strcpy(m_room.player_name[id], buf);
                cout << "uid:" << id << " set name:" << buf << " len:" << s.length() << endl;
            }
            else if (strstr(header, "PA"))
            {
                //if (debug_mode) cout << "uid:" << uid[thandle] << " DEBUG#:PA" << endl;
                memcpy(&paction[m_handle_to_uid[thandle]], packet.data() + HEAD_LENGTH, sizeof(PlayerAction));
            }
            else if (strstr(header, "GS"))//游戏开始
            {
                m_client_get_started[m_handle_to_uid[thandle]] = true;
            }
            break;
        }
        case YEK_CONNECT_RESPONSE://连接响应事件
            if (ev->status() == 0)//status为0正常 1非正常
            {
                auto thandle = ev->transport();
                int id = getUnusedUid();
                m_handle_to_uid[thandle] = id;
                m_uid_to_handle[id] = thandle;
                if (debug_mode) cout << "Client#" << thandle << "#comes in uid:" << id << endl;
            }
            break;
        case YEK_CONNECTION_LOST://连接丢失事件
            auto thandle = ev->transport();
            int id = m_handle_to_uid[thandle];
            if (debug_mode) cout << "Client#" << thandle << "#lost connection! uid:" << id << endl;
            deleteUid(m_handle_to_uid[thandle]);
            m_handle_to_uid.erase(thandle);
            m_uid_to_handle.erase(id);
            m_player_name.erase(id);
            memset(m_room.player_name[id], 0, MAX_NAME_LENGTH + 1);
            break;
        }
        });
}

CHServer::~CHServer()
{
    for (int i = 0; i < ai_player_num; i++)
        delete ai_client[i];
    if (m_server != nullptr)
    {
        m_server->stop();
        delete m_server;
    }
}

void CHServer::listen()
{
    if (m_server != nullptr)
        m_server->open(0, YCK_TCP_SERVER);
} 

void CHServer::mapUploadInit()
{
    //客户端上传的数据读取
    if (m_connection_num == 0)
    {
        m_started = false;
        memset(&m_room, 0, sizeof(RoomInformation));
        closeGame();
    }
    memset(&m_map_trans, 0, sizeof(MapInformation));
    m_map_trans.is_updated = true;
    
}

void CHServer::mapUpload()
{
    m_map_trans.is_updated = true;
    for (int i = 1; i < MAX_CONNECTIONS; i++)
        if (m_uid_usage[i])
        {
            char buf[sizeof(MapInformation) + HEAD_LENGTH + 2] = "MP\0";
            memcpy(buf + HEAD_LENGTH, &m_map_trans, sizeof(MapInformation));
            m_server->write(m_uid_to_handle[i], buf, HEAD_LENGTH + sizeof(MapInformation));
        }
}

bool CHServer::UidIsUsed(int uid)
{
    return m_uid_usage[uid];
}

bool CHServer::startGame()
{
    if (debug_mode)cout << "#DEBUG#GAME START " << endl;
    if(m_started == true)
        return false;
    m_started = true;
    for (int i = 1; i < MAX_CONNECTIONS; i++)
        if (m_uid_usage[i])
        {
            if (debug_mode)cout << "TELL START: " << i << endl;
            m_server->write(m_uid_to_handle[i], "ST\0", HEAD_LENGTH);
        }
    while (1)
    {
        bool all_started = true;
        for (int i = 1; i < MAX_CONNECTIONS; i++)
            if (m_uid_usage[i])
                if (m_client_get_started[i] == false)
                {
                    m_server->write(m_uid_to_handle[i], "ST\0", HEAD_LENGTH);
                    all_started = false;
                }
        if (all_started)
            break;
    }
    return true;
}

void CHServer::closeGame()
{
    if (debug_mode)cout << "#DEBUG#GAME CLOSE" << endl;
    if (m_started == true)
        return;
    m_started = false;
    for (int i = 1; i < MAX_CONNECTIONS; i++)
        if (m_uid_usage[i])
        {
            if (debug_mode)cout << "TELL CLOSED: " << i << endl;
            m_server->write(m_uid_to_handle[i], "GO\0", HEAD_LENGTH);
        }
}

bool CHServer::GameIsStarted()
{
    return m_started;
}

int CHServer::getConnectionNum()
{
    return m_connection_num;
}

void CHServer::openDebugMode()
{
    debug_mode = !debug_mode;
    cout << "DEBUG MODE IS " << (debug_mode ? "OPEN" : "CLOSED") << endl;
}

void CHServer::roomUpdate()
{
    m_room.player_num = m_connection_num;
    if (m_room.player_num == 0)
        return;
    for (int i = 0; i < MAX_CONNECTIONS; i++)
        m_room.player_alive[i] = m_uid_usage[i];
    char buff[HEAD_LENGTH + sizeof(RoomInformation) + 2] = "RO\0";
    memcpy(buff + HEAD_LENGTH, &m_room, sizeof(RoomInformation));
    for(int i = 0; i < MAX_CONNECTIONS;i++)
        if (m_uid_usage[i])
        {
            m_server->write(m_uid_to_handle[i], buff, HEAD_LENGTH + sizeof(RoomInformation)); 
        }
}

bool CHServer::addAi()
{
    if(m_connection_num >= MAX_CONNECTIONS - 1)
        return false;
    string str = "AIPlayer"+ std::to_string(m_connection_num);
    int p = ai_player_num++;
    ai_client[p] = new CHClient("127.0.0.1", 25595);
    ai_client[p]->setName(str.c_str());
    return true;
}

void CHServer::mapInformationInit(MapInformationInit mii)
{
    mii.player_num_all = m_connection_num;
    char buff[HEAD_LENGTH + sizeof(MapInformationInit) + 20] = "MI\0";
    memcpy(buff + HEAD_LENGTH, &mii, sizeof(MapInformationInit));
    for (int i = 1; i < MAX_CONNECTIONS; i++)
        if (m_uid_usage[i])
        {
            m_server->write(m_uid_to_handle[i], buff, HEAD_LENGTH + sizeof(MapInformationInit));
        }
}
