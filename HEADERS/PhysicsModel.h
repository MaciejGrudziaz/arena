#pragma once
#include <string>

//--------------------------------------------------------------------------------------------
//------------------------------KLASA WZORCOWA DLA MODELU FIZYKI------------------------------
//-------------------------------------[klasa wirtualna]--------------------------------------
//--------------------------------------------------------------------------------------------

class PhysicsModel {
	std::string name;															//nazwa modelu

public:

	virtual void Process() = 0;													//funkcja przetwarzaj¹ca model [czysto wirtualna - musi byæ zdefiniowana w ka¿dym modelu]
};