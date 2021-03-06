//
//  MainMenu.h
//  TacticalWarfare
//
//  Created by Thomas Redding on 5/5/15.
//  Copyright (c) 2015 Thomas Redding. All rights reserved.
//

#ifndef __TacticalWarfare__MainMenu__
#define __TacticalWarfare__MainMenu__

#include <stdio.h>
#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include "ResourcePath.hpp"
#include "MenuPage.h"
#include "TextBox.h"
#include "AppBaseClass.h"

enum MainMenuPageName {
    MainMenuPageName_Home = 0,
    MainMenuPageName_MultiplayerClient = 1,
    MainMenuPageName_SinglePlayer = 2,
    MainMenuPageName_MultiplayerServer = 3,
};

class MainMenu {
public:
    MainMenu(sf::RenderWindow &w, AppBaseClass &app);
    void think();
    void draw();
    void mouseMove(sf::Event::MouseMoveEvent event);
    void mouseDown(sf::Event::MouseButtonEvent event);
    void mouseUp(sf::Event::MouseButtonEvent event);
    void keyDown(sf::Event::KeyEvent event);
    void keyUp(sf::Event::KeyEvent event);
    void textEntered(sf::Event::TextEvent event);
    void sendMessage(std::string message);
private:
    std::vector<MenuPage> pages;
    sf::Texture buttonTexture;
    sf::Font font;
    AppBaseClass *parentApp;
    
    sf::RenderWindow *window;
    
    MainMenuPageName pageNum = MainMenuPageName_Home;
    
    void buttonClicked(int index);
    int frameCounter = 0;
    
    TextBox usernameTextBox;
    TextBox serverIpTextBox;
    TextBox portTextBox;
    TextBox udpPortTextBox;
};


#endif /* defined(__TacticalWarfare__MainMenu__) */
