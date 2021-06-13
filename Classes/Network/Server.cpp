#include "Server.h"
#include <cstdlib>
#include <iostream>
using namespace std;

//创建一个服务端
CHServer* chserver = nullptr;

int CHServer::get_unused_uid()
{
    for (int i = 1; i < MAX_CONNECTIONS; i++)
        if (uid_usage[i] == false)
        {
            uid_usage[i] = true;
            connection_num++;
            return i;
        }
    return -1;
}

bool CHServer::delete_uid(int id)
{
    if (uid_usage[id] == false)
        return false;
    uid_usage[id] = false;
    connection_num--;
    return true;
}

CHServer::CHServer(const char* ip, unsigned short port)
{
    server = new io_service({ ip,port });//配置信道
    server->set_option(YOPT_S_DEFERRED_EVENT, 0);//在网络线程中调度网络事件
    server->start([&](event_ptr&& ev) { //添加网络线程函数
        switch (ev->kind())
        {
        case YEK_PACKET: {//消息包事件
            auto packet = std::move(ev->packet());
            char header[HEAD_LENGTH + 1] = { 0 };
            memcpy(header, packet.data(), HEAD_LENGTH);
            auto thandle = ev->transport();
            if (strstr(header, "GU"))
            {
                if(debug_mode) cout << "uid:" << uid[thandle] << " DEBUG#:GU" << endl;
                int id = uid[thandle];
                char buf[HEAD_LENGTH + 8] = "SU\0";
                memcpy(buf + HEAD_LENGTH, &id, 4);
                server->write(thandle, buf, HEAD_LENGTH + 4);
            }
            else if (strstr(header, "SN"))
            {
                if (debug_mode) cout << "uid:" <<uid[thandle] << " DEBUG#:SN" << endl;
                char buf[20] = { 0 };
                memcpy(buf, packet.data() + HEAD_LENGTH, packet.size() - HEAD_LENGTH);
                string s = buf;
                int id = uid[thandle];
                player_name[id] = s;
                strcpy(room.player_name[id], buf);
                cout << "uid:" << id << " set name:" << buf << " len:" << s.length() << endl;
            }
            else if (strstr(header, "PA"))
            {
                //if (debug_mode) cout << "uid:" << uid[thandle] << " DEBUG#:PA" << endl;
                memcpy(&paction[uid[thandle]], packet.data() + HEAD_LENGTH, sizeof(MapInformation));
            }
            else if (strstr(header, "ST"))//游戏开始
            {

            }
            fflush(stdout);
            break;
        }
        case YEK_CONNECT_RESPONSE://连接响应事件
            if (ev->status() == 0)//status为0正常 1非正常
            {
                auto thandle = ev->transport();
                int id = get_unused_uid();
                uid[thandle] = id;
                uid_to_handle[id] = thandle;
                if (debug_mode) cout << "Client#" << thandle << "#comes in uid:" << id << endl;
                if (started)
                {
                    smap.player[id].alive = true;
                    smap.player[id].position_x = 100, smap.player[id].position_y = 100, smap.player[id].hp = 100;
                }
            }
            break;
        case YEK_CONNECTION_LOST://连接丢失事件
            auto thandle = ev->transport();
            int id = uid[thandle];
            if (debug_mode) cout << "Client#" << thandle << "#lost connection! uid:" << id << endl;
            delete_uid(uid[thandle]);
            uid.erase(thandle);
            uid_to_handle.erase(id);
            player_name.erase(id);
            memset(room.player_name[id], 0, MAX_NAME_LENGTH + 1);
            break;
        }
        });
}

CHServer::~CHServer()
{
    if (server != nullptr)
        delete server;
}

void CHServer::listen()
{
    if (server != nullptr)
        server->open(0, YCK_TCP_SERVER);
}

#define CHRAND() ((float)rand()/RAND_MAX)
void CHServer::map_init(int seed)
{
    srand(seed);
    started = true;
    for(int i = 1; i < MAX_CONNECTIONS;i++)
        if (uid_usage[i] == true)
        {
            smap.player[i].alive = true;
            smap.player[i].position_x = CHRAND() * 200;
            smap.player[i].position_y = CHRAND() * 200;
            smap.player[i].hp = 100;
        }
    
}

void CHServer::map_update()
{
    //客户端上传的数据读取
    if (connection_num == 0)
        return;
    //if (debug_mode)cout << "DEBUG#MAP_UPDATE #START#" << endl;
    for (int i = 1; i < MAX_CONNECTIONS; i++)
    {
        if (uid_usage[i] && smap.player[i].alive)
        {
            switch (paction[i].keyboard_action)
            {
            case PA_LEFT: smap.player[i].position_x += -4; break;
            case PA_RIGHT: smap.player[i].position_x += 4; break;
            case PA_UP: smap.player[i].position_y += 4; break;
            case PA_DOWN: smap.player[i].position_y += -4; break;
            }
            if (paction[i].is_shoot)
            {
                ;
            }
            memset(&paction[i], 0, sizeof(PlayerAction));
        }
    }
    //更新地图
    memset(&map_trans, 0, sizeof(MapInformation));
    map_trans.player_num = connection_num;
    for (int i = 1; i < MAX_CONNECTIONS; i++)
        if (uid_usage[i] && smap.player[i].alive)
        {
            map_trans.player[i].alive = true;
            map_trans.player[i].position_x = smap.player[i].position_x;
            map_trans.player[i].position_y = smap.player[i].position_y;
            map_trans.player[i].hp = map_trans.player[i].hp;
        }
    for (int i = 1; i < MAX_CONNECTIONS; i++)
        if (uid_usage[i])
        {
            char buf[sizeof(MapInformation) + HEAD_LENGTH + 2] = "MP\0";
            memcpy(buf + HEAD_LENGTH, &map_trans, sizeof(MapInformation));
            //if (debug_mode)cout << "Send Map to #" << uid_to_handle[i] << "# size:" << HEAD_LENGTH + sizeof(MapInformation) << endl;
            server->write(uid_to_handle[i], buf,HEAD_LENGTH + sizeof(MapInformation));
        }
    //if (debug_mode)cout << "DEBUG#MAP_UPDATE #OVER#" << endl;
}

bool CHServer::startGame()
{
    if (debug_mode)cout << "#DEBUG#START: " << (started ? "YES" : "NO") << endl;
    if(started == true)
        return false;
    started = true;
    for (int i = 1; i < MAX_CONNECTIONS; i++)
        if (uid_usage[i])
        {
            if (debug_mode)cout << "TELL START: " << i << endl;
            server->write(uid_to_handle[i], "ST\0", HEAD_LENGTH);
        }
    this->map_init();
    return true;
}

bool CHServer::GameIsStarted()
{
    return started;
}

int CHServer::getConnectionNum()
{
    return connection_num;
}

void CHServer::open_debug_mode()
{
    debug_mode = !debug_mode;
    cout << "DEBUG MODE IS " << (debug_mode ? "OPEN" : "CLOSED") << endl;
}

void CHServer::room_update()
{
    room.player_num = connection_num;
    if (room.player_num == 0)
        return;
    for (int i = 0; i < MAX_CONNECTIONS; i++)
        room.player_alive[i] = uid_usage[i];
    char buff[HEAD_LENGTH + sizeof(RoomInformation) + 20] = "RO\0";
    memcpy(buff + HEAD_LENGTH, &room, HEAD_LENGTH + sizeof(RoomInformation));
    for(int i = 0; i < MAX_CONNECTIONS;i++)
        if (uid_usage[i])
        {
            server->write(uid_to_handle[i], buff, HEAD_LENGTH + sizeof(RoomInformation)); 
        }
}
