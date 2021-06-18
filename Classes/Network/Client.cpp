#include "Client.h"
#include <cstring>
#include <iostream>

using namespace std;
using namespace yasio;
using namespace yasio::inet;

CHClient *chclient = nullptr;

CHClient::CHClient(const char *ip, unsigned short port) {
    m_client = new io_service({ip, port});
    m_client->set_option(YOPT_S_DEFERRED_EVENT, 0);
    m_client->start([&](event_ptr &&ev) { 
        switch (ev->kind()) {
            case YEK_PACKET: {
                auto packet = std::move(ev->packet());
                char header[HEAD_LENGTH + 1] = {0};
                memcpy(header, packet.data(), HEAD_LENGTH);
                if (strstr(header, "SU")) {
                    //cout << "DEBUG#:SU" << endl;
                    memcpy(&m_uid, packet.data() + HEAD_LENGTH, 4);

                } else if (strstr(header, "RO")) {
                    //cout << "DEBUG#:RO" << endl;
                    memcpy(&m_room, packet.data() + HEAD_LENGTH, sizeof(RoomInformation));
                } else if (strstr(header, "MP")) {
                    memcpy(&m_map, packet.data() + HEAD_LENGTH, sizeof(MapInformation));
                } else if (strstr(header, "GO"))
                {
                    m_started = false;
                } else if (strstr(header, "ST"))
                {
                    m_started = true;
                    m_client->write(m_thandle, "GS\0", 4);
                } else if (strstr(header, "MI"))
                {
                    memcpy(&m_map_information_init, packet.data() + HEAD_LENGTH,
                           sizeof(MapInformationInit));
                    m_map_information_init.is_updated = true;
                }
                fflush(stdout);
                break;
            }
            case YEK_CONNECT_RESPONSE:
                if (ev->status() == 0)
                {
                    m_thandle = ev->transport();
                    m_client->write(m_thandle, "GU\0", 4);
                }
                break;
            case YEK_CONNECTION_LOST:
                if (ev->status() == 0)
                {
                    
                    m_thandle = 0;
                    m_uid = 0;
                }
                break;
        }
    });
}

CHClient::~CHClient() {

    if (m_client != nullptr)
    {
        if (m_thandle != 0)
            m_client->close(m_thandle);
        m_client->stop();
        delete m_client;
    }
}

void CHClient::link() {
    m_client->open(0, YCK_TCP_CLIENT);
}

int CHClient::getUid() {
    return m_uid;
}

void CHClient::setName(const char *name) {
    char buf[128] = "SN\0";
    strcpy(buf + HEAD_LENGTH, name);
    while (m_uid == 0);
    m_client->write(m_thandle, buf, HEAD_LENGTH + 10);
}

bool CHClient::upload() {
    char str[sizeof(PlayerAction) + 5] = "PA\0";
    memcpy(str + HEAD_LENGTH, &m_localaction, sizeof(PlayerAction));
    memset(&m_localaction, 0, sizeof(m_localaction));
    m_client->write(m_thandle, str, HEAD_LENGTH + sizeof(PlayerAction));
    return true;
}

bool CHClient::isStarted() {
    return m_started;
}

bool CHClient::getMapInitState() {
    return m_map_init_state;
}

void CHClient::setMapInited() {
    m_map_init_state = true;
}

bool CHClient::isUnconnected()
{
    if(m_uid != 0)
        return false;
    return true;
}

bool isMultipleGame() {
    if (chclient != nullptr)
        return true;
    return false;
}
