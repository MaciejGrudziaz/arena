#pragma once

//---------------------------------------------------------------------------------------------------------------------
//------------------------------KLASA SINGLETONOWA OPISUJ¥CA DODATKOWE ZMIENNE ŒWIATA GRY------------------------------
//---------------------------------------------------[klasa testowa]---------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

class GamePhysicsConst {
	GamePhysicsConst();
	GamePhysicsConst(const GamePhysicsConst& obj) {}
	GamePhysicsConst& operator=(const GamePhysicsConst& obj) {}

	static GamePhysicsConst* instance;																	//instancja klasy

	double G;																							//przyspieszenie grawitacyjne [si³a ci¹¿enia]

public:

	static GamePhysicsConst* GetInstance();																//pobranie instancji klasy

	//funkcje pobieraj¹ce sk³adowe klasy
	double GetG() { return G; }																			

	//funkcje ustawiaj¹ce wartoœci dla sk³adowych klasy
	void SetG(double _G) { G = _G; }
};