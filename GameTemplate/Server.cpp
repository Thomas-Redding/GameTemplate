//
//  Server.cpp
//  GameTemplate
//
//  Created by Thomas Redding on 5/19/15.
//  Copyright (c) 2015 Thomas Redding. All rights reserved.
//

#include "Server.h"

Server::Server(TcpHandlerCommunicator &tcpHandlerCommunicator, ClientServerCommunicator &clientServerCommunicator) : ServerBaseClass(tcpHandlerCommunicator, clientServerCommunicator), game(*this) {
    teams.push_back(std::vector<std::string>());
    teams.push_back(std::vector<std::string>());
    teams.push_back(std::vector<std::string>());
    tableOfClients.push_back(ClientOfServer());
    tableOfClients[0].username = "SERVER";
    tableOfClients[0].udpPort = 0;
    tableOfClients[0].tcpSocket = nullptr;
}

Server::~Server() {
}

void Server::run() {
    long long currentTime = std::chrono::duration_cast< std::chrono::milliseconds >(std::chrono::system_clock::now().time_since_epoch()).count();
    typedef std::map<sf::TcpSocket*, long long>::iterator it_type;
    
    if(isGameOccuring) {
        game.think();
    }
    
    // sleep for 10 milliseconds
    struct timespec tim, tim2;
    tim.tv_sec = 0;
    tim.tv_nsec = 10;
    tim.tv_nsec *= 1000000;
    nanosleep(&tim , &tim2);
}

void Server::receivedTcpMessage(std::string message, sf::TcpSocket *socket) {
    if(isGameOccuring) {
        std::string name = "";
        for(int i=0; i<tableOfClients.size(); i++) {
            if(tableOfClients[i].tcpSocket == socket) {
                name = tableOfClients[i].username;
                break;
            }
        }
        if(name == "") {
            game.receivedTcpMessage(message, "SERVER");
        }
        else {
            game.receivedTcpMessage(message, name);
        }
        return;
    }
    
    std::vector<std::string> arr = split(message, '\n');
    if(arr.size() == 0) {
        return;
    }
    if(socket == nullptr) {
        // local client sent the message
        if(arr[0] == "Chat Message" && arr.size() == 2) {
            chatMessages.push_back("SERVER: " + arr[1]);
            sendTcpMessageToAllClients("CHAT MESSAGE\n"+chatMessages[chatMessages.size()-1]);
        }
        else if(arr[0] == "Join No Team") {
            changeTeamOfClient("SERVER", 0);
            sendListOfTeamsToAllClients();
        }
        else if(arr[0] == "Join Team 1") {
            changeTeamOfClient("SERVER", 1);
            sendListOfTeamsToAllClients();
        }
        else if(arr[0] == "Join Team 2") {
            changeTeamOfClient("SERVER", 2);
            sendListOfTeamsToAllClients();
        }
        else if(arr[0] == "LAUNCH GAME") {
            game.startGame("MyMap", teams);
            sendTcpMessageToAllClients("LAUNCHING GAME");
            isGameOccuring = true;
        }
    }
    else {
        // remote client sent the message
        if(arr[0] == "Give UDP Socket" && arr.size() >= 3) {
            // give new client UDP port number
            arr[1] = makeAlphaNumeric(arr[1]);
            if(isUsernameUnique(arr[1])) {
                // username is unique; send UDP port back
                tableOfClients.push_back(ClientOfServer());
                tableOfClients[tableOfClients.size()-1].username = arr[1];
                tableOfClients[tableOfClients.size()-1].udpPort = stoi(arr[2]);
                tableOfClients[tableOfClients.size()-1].tcpSocket = socket;
                std::string newMessage = "Server UDP Port\n"+std::to_string(getMyUdpPort());
                sendTcp(newMessage, socket);
            }
            else {
                // username is a duplicate; send this message back and refuse to give the client the UDP port #
                std::string newMessage = "USERNAME NOT UNIQUE";
                sendTcp(newMessage, socket);
            }
        }
        else {
            // its a normal TCP message
            if(arr[0] == "Chat Message" && arr.size() == 2) {
                for(int i=0; i<tableOfClients.size(); i++) {
                    if(tableOfClients[i].tcpSocket == socket) {
                        chatMessages.push_back(tableOfClients[i].username + ": " + arr[1]);
                        sendTcpMessageToAllClients("CHAT MESSAGE\n"+chatMessages[chatMessages.size()-1]);
                        break;
                    }
                }
            }
            else if(arr[0] == "Join No Team") {
                for(int i=0; i<tableOfClients.size(); i++) {
                    if(tableOfClients[i].tcpSocket == socket) {
                        sendTcpMessageToAllClients("CHAT MESSAGE\nGAME: " + tableOfClients[i].username + " joined No Team");
                        changeTeamOfClient(tableOfClients[i].username, 0);
                        sendListOfTeamsToAllClients();
                        break;
                    }
                }
            }
            else if(arr[0] == "Join Team 1") {
                for(int i=0; i<tableOfClients.size(); i++) {
                    if(tableOfClients[i].tcpSocket == socket) {
                        sendTcpMessageToAllClients("CHAT MESSAGE\nGAME: " + tableOfClients[i].username + " joined Team 1");
                        changeTeamOfClient(tableOfClients[i].username, 1);
                        sendListOfTeamsToAllClients();
                        break;
                    }
                }
            }
            else if(arr[0] == "Join Team 2") {
                for(int i=0; i<tableOfClients.size(); i++) {
                    if(tableOfClients[i].tcpSocket == socket) {
                        sendTcpMessageToAllClients("CHAT MESSAGE\nGAME: " + tableOfClients[i].username + " joined Team 2");
                        changeTeamOfClient(tableOfClients[i].username, 2);
                        sendListOfTeamsToAllClients();
                        break;
                    }
                }
            }
            else if(arr[0] == "LAUNCH GAME") {
                // comment out the contents of this "else if" statement when the game is launched
                game.startGame("MyMap", teams);
                sendTcpMessageToAllClients("LAUNCHING GAME");
                isGameOccuring = true;
            }
        }
    }
}

void Server::receivedUdpMessage(std::string message, sf::IpAddress ipAddressOfClient, unsigned short portOfClient) {
    if(isGameOccuring) {
        std::string name = "SERVER";
        for(int i=0; i<tableOfClients.size(); i++) {
            if(tableOfClients[i].tcpSocket != nullptr && tableOfClients[i].tcpSocket->getRemoteAddress() == ipAddressOfClient) {
                name = tableOfClients[i].username;
                break;
            }
        }
        game.receivedUdpMessage(message, name);
        return;
    }
}

std::vector<std::string> Server::split(const std::string s, char delim) {
    std::vector<std::string> elems;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    if(s[s.length()-1] == delim) {
        elems.push_back("");
    }
    return elems;
}

bool Server::isUsernameUnique(std::string name) {
    for(int i=0; i<tableOfClients.size(); i++) {
        if(tableOfClients[i].username == name) {
            return false;
        }
    }
    if(name == "SERVER" || name == "") {
        return false;
    }
    return true;
}

void Server::sendTcpMessageToAllClients(std::string message) {
    for(int i=0; i<tableOfClients.size(); i++) {
        sendTcp(message, tableOfClients[i].tcpSocket);
    }
}

void Server::changeTeamOfClient(std::string username, int newTeam) {
    int i, j;
    
    // remove the client from their current team
    for(i=0; i<teams.size(); i++) {
        for(j=0; j<teams[i].size(); j++) {
            if(teams[i][j] == username) {
                teams[i].erase(teams[i].begin()+j, teams[i].begin()+j+1);
                break;
            }
        }
        if(j != teams[i].size()) {
            break;
        }
    }
    
    // add the client to their new team
    if(newTeam >= 0 && newTeam < teams.size()) {
        teams[newTeam].push_back(username);
    }
}

void Server::sendListOfTeamsToAllClients() {
    std::string message = "TEAM LIST";
    for(int i=0; i<teams.size(); i++) {
        message += "\n";
        for(int j=0; j<teams[i].size(); j++) {
            if(j != 0) {
                message += "\t";
            }
            message += teams[i][j];
        }
    }
    sendTcpMessageToAllClients(message);
}

void Server::logOutUser(sf::TcpSocket *socket) {
    for(int i=0; i<tableOfClients.size(); i++) {
        if(tableOfClients[i].tcpSocket == socket) {
            std::string username = tableOfClients[i].username;
            changeTeamOfClient(username, -1);
            tableOfClients.erase(tableOfClients.begin()+i, tableOfClients.begin()+i+1);
        }
    }
    removeTcpSocket(socket);
}

void Server::removeTcpSocket(sf::TcpSocket *socketToRemove) {
    for(int i=0; i<tableOfClients.size(); i++) {
        if(tableOfClients[i].tcpSocket == socketToRemove) {
            tableOfClients.erase(tableOfClients.begin()+i, tableOfClients.begin()+i+1);
        }
    }
}

void Server::sendTcpMessage(std::string message, std::string username) {
    for(int i=0; i<tableOfClients.size(); i++) {
        if(tableOfClients[i].username == username) {
            sendTcp(message, tableOfClients[i].tcpSocket);
        }
    }
}

void Server::sendUdpMessage(std::string message, std::string username) {
    for(int i=0; i<tableOfClients.size(); i++) {
        if(tableOfClients[i].username == username) {
            if(username == "SERVER") {
                sendUdp(message, "0.0.0.0", 0);
            }
            else {
                sendUdp(message, tableOfClients[i].tcpSocket->getRemoteAddress(), tableOfClients[i].udpPort);
            }
        }
    }
}

bool isNotAlphaNumeric(char c) {
    if(isalnum(c) == 0) {
        return true;
    }
    return false;
}

std::string Server::makeAlphaNumeric(std::string str) {
    str.erase(std::remove_if(str.begin(), str.end(), isNotAlphaNumeric), str.end());
    return str;
}
