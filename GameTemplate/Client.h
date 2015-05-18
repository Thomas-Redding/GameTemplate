//
//  Client.h
//  GameTemplate
//
//  Created by Thomas Redding on 5/15/15.
//  Copyright (c) 2015 Thomas Redding. All rights reserved.
//

#ifndef __GameTemplate__Client__
#define __GameTemplate__Client__

#include <stdio.h>
#include <sstream>
#include <SFML/Graphics.hpp>
#include "ClientBaseClass.h"
#include "MainMenu.h"
#include "TcpMessageContainer.h"

class Client : public ClientBaseClass {
public:
    Client(sf::RenderWindow &w, TcpMessageContainer &tcpMessageContainer);
    void think();
    void draw();
    void mouseMove(sf::Event::MouseMoveEvent event);
    void mouseDown(sf::Event::MouseButtonEvent event);
    void mouseUp(sf::Event::MouseButtonEvent event);
    void keyDown(sf::Event::KeyEvent event);
    void keyUp(sf::Event::KeyEvent event);
    void textEntered(sf::Event::TextEvent event);
    void receivedUdpMessage(std::string message);
    void receivedTcpMessage(std::string message);
    std::string sendMeMessage(std::string message);
private:
    sf::RenderWindow *window;
    MainMenu mainMenu;
    bool inMainMenu = true;
    
    std::vector<std::string> split(const std::string s, char delim);
};

#endif /* defined(__GameTemplate__Client__) */