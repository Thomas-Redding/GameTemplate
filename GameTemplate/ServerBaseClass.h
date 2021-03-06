//
//  ServerBaseClass.h
//  GameTemplate
//
//  Created by Thomas Redding on 5/19/15.
//  Copyright (c) 2015 Thomas Redding. All rights reserved.
//

#ifndef __GameTemplate__ServerBaseClass__
#define __GameTemplate__ServerBaseClass__

#include <stdio.h>
#include "ClientServerCommunicator.h"
#include "TcpHandlerCommunicator.h"

class ServerBaseClass {
public:
    ServerBaseClass(TcpHandlerCommunicator &tcpHandlerCommunicator, ClientServerCommunicator &clientServerCommunicator);
    ~ServerBaseClass();
    void checkNetworkForMessages();
protected:
    void sendTcp(std::string message, sf::TcpSocket *socket);
    void sendUdp(std::string message, sf::IpAddress ipAddressOfClient, unsigned short portOfClient);
    virtual void receivedTcpMessage(std::string message, sf::TcpSocket *socket) = 0;
    virtual void receivedUdpMessage(std::string message, sf::IpAddress ipAddressOfClient, unsigned short portOfClient) = 0;
private:
    TcpHandlerCommunicator *tcpHandler;
    ClientServerCommunicator *client;
    sf::UdpSocket udpSocket;
};

#endif /* defined(__GameTemplate__ServerBaseClass__) */
