#pragma once
#include <vector>
#include "Light.h"
#include "Player.h"
#include "Map.h"
#include "Bot.h"

//--------------------------------------------------------------------------------------------------
//----------------------------------------OBIEKTY ŒWIATA GRY----------------------------------------
//----------klasa przechowuj¹ca obiekty znajduj¹ce siê w œwiecie gry (postacie, mapy itp.)----------
//--------------------------------------------------------------------------------------------------

class Map;

class GameObjects {
	GameObjects();											//klasa singletonowa [zablokowany konstruktor domyœlny]

public:
	static std::vector <Player> players;					//wekotr przechowuj¹cy wszytkie obiekty postaci sterowanych przez gracza

	static std::vector <Character> characters;				//wektor przechowuj¹cy wszystkie pozosta³ê obiekty œwiata gry

	static std::vector <Light> lights;						//wektor przechowuj¹cy wszystkie Ÿród³a œwiat³a w œwiecie gry

	static std::vector <Map> maps;

	static std::vector<Bot> bots;
};