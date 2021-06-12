#include "Server.h"
#include <cstdlib>

int CHServer::get_unused_uid()
{
    for (int i = 1; i < MAX_CONNECTIONS; i++)
        if (uid_usage[i] == false)
        {
            uid_usage[i] = true;
            player_num++;
            return i;
        }
    return -1;
}

bool CHServer::delete_uid(int id)
{
    if(uid_usage[id] == false)
        return false;
    uid_usage[id] = false;
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
                //cout << "DEBUG#:GU" << endl;
                int id = uid[thandle];
                char buf[HEAD_LENGTH + 8] = "SU\0";
                memcpy(buf + HEAD_LENGTH, &id, 4);
                server->write(thandle, buf, HEAD_LENGTH + 4);
            }
            else if (strstr(header, "SN"))
            {
                //cout << "DEBUG#:SN" << endl;
                char buf[20] = { 0 };
                memcpy(buf, packet.data() + HEAD_LENGTH, packet.size() - HEAD_LENGTH);
                string s = buf;
                player_name[uid[thandle]] = s;
            }
            else if (strstr(header, "PA"))
            {
                //cout << "DEBUG#:PA" << endl;
                memcpy(&paction[uid[thandle]], packet.data() + HEAD_LENGTH, sizeof(MapInformation));
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
                if (started)
                {
                    map.player[id].alive = true;
                    map.player[id].position_x = 100, map.player[id].position_y = 100, map.player[id].hp = 100;
                }
            }
            break;
        case YEK_CONNECTION_LOST://连接丢失事件
            auto thandle = ev->transport();
            int id = uid[thandle];
            delete_uid(uid[thandle]);
            uid.erase(thandle);
            uid_to_handle.erase(id);
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
            map.player[i].alive = true;
            map.player[i].position_x = CHRAND() * 200;
            map.player[i].position_y = CHRAND() * 200;
            map.player[i].hp = 100;
        }
    
}

void CHServer::map_update()
{
    for (int i = 1; i < MAX_CONNECTIONS; i++)
    {
        if (uid_usage[i])
        {
            switch (paction[i].keyboard_action)
            {
            case PA_LEFT: map.player[i].position_x += -4; break;
            case PA_RIGHT: map.player[i].position_x += 4; break;
            case PA_UP: map.player[i].position_y += 4; break;
            case PA_DOWN: map.player[i].position_y += -4; break;
            }
            if (paction[i].is_shoot)
            {
                ;
            }
            memset(&paction[i], 0, sizeof(PlayerAction));
        }
    }
    memset(&map_trans, 0, sizeof(MapInformation));
    map_trans.player_num = player_num;
    for (int i = 1; i < MAX_CONNECTIONS; i++)
        if (uid_usage[i])
        {
            map_trans.player[i].position_x = map.player[i].position_x;
            map_trans.player[i].position_y = map.player[i].position_y;
            map_trans.player[i].hp = map_trans.player[i].hp;
        }
    for (int i = 1; i < MAX_CONNECTIONS; i++)
        if (uid_usage[i])
        {
            char buf[sizeof(PlayerInformation) + HEAD_LENGTH + 1] = "MP\0";
            memcpy(buf + HEAD_LENGTH, &map_trans, sizeof(MapInformation));
            server->write(uid_to_handle[i], buf, sizeof(PlayerInformation) + HEAD_LENGTH);
        }

}
