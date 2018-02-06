#include "GameGraphics.h"

//przetwarzanie grafiki
void GameGraphics::ProccessGraphics() {
	TIMER::StartTimeMeasurment("Graphics");

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);							//wyczyszcenie buforów koloru i g³êbi
	glLoadIdentity();															//zresetowanie macierzy widoku modelu

	SetCamera();																//ustawienie pozycji i oreintacji kamery

	DrawMaps();																	//narysowanie map [klasa Map]

	DrawCharacters();															//narysowanie postaci statycznych [klasa Character]

	DrawBots();

	DrawPlayers();																//narysowanie postaci sterowanych przez gracza [klasa Player]

	ShowGUI();

	ShowConsoleOutput();														//wyœwietlenie danych wyjœciowych z 'konsoli'

	glFlush();																	//opró¿nienie buforów grafiki OpenGL

	SwapBuffers(WinAPIwindow::hDC);												//zamiana buforów [p³ynniejsze wykonywanie grafiki]

	TIMER::StopTimeMeasurment("Graphics");
}

//rysowanie wszystkich obiektów klasy Map
void GameGraphics::DrawMaps() {
	for (unsigned int i = 0; i < GameObjects::maps.size(); i++) {
		if (GameObjects::maps[i].IsEnabled())									//sprawdzenie czy mapa jest 'w³¹czona'
			GameObjects::maps[i].Draw();									//narysowanie modelu
	}
}

//rysowanie wszystkich obiektów klasy Character
void GameGraphics::DrawCharacters() {
	for (unsigned int i = 0; i < GameObjects::characters.size(); i++) {
		if (GameObjects::characters[i].IsEnabled())								//sprawdzenie czy postaæ jest 'w³¹czona'
			GameObjects::characters[i].DrawModel();								//narysowanie modelu
	}
}

void GameGraphics::DrawBots() {
	for (Bot& bot : GameObjects::bots) {
		if (bot.IsEnabled())
			bot.DrawModel();
	}
}

//rysowanie wszystkich obiektów klasy Player
void GameGraphics::DrawPlayers() {
	for (unsigned int i = 0; i < GameObjects::players.size(); i++) {
		if (GameObjects::players[i].IsEnabled())								//sprawdzenie czy postaæ jest 'w³¹czona'
			GameObjects::players[i].DrawModel();								//narysowanie modelu
	}
}

//ustalenie odpowiedniej pozycji
void GameGraphics::SetCameraPosition() {
	for (unsigned int i = 0; i < GameObjects::players.size(); i++) {
		if (GameObjects::players[i].IsEnabled()) {								//wybranie pierwszej 'odblokowanej' postaci gracza z wektora postaci
			Vector position = GameObjects::players[i].getPosition();			//pobranie pozycji postaci
			Vector orientation = GameObjects::players[i].getOrientation();		//pobranie orienatcji postaci
			Vector relativePos = Camera::GetRelativePosition();					//pobranie pozycji kamery relatywnej do postaci

			double angle = orientation.y / 360.0 * 2.0 * 3.1415;				//k¹t obrotu postaci wokó³ osi y [w radianach; 2.0 * 3.1415 -> 2 * PI]
			position.x -= relativePos.z*sin(-angle);							//wartoœæ pozycji kamery w osi x w stosunku do pozycji postaci
			position.y += relativePos.y;										//wartoœæ pozycji kamery w osi y w stosunku do pozycji postaci
			position.z += relativePos.z*cos(-angle);							//wartoœæ pozycji kamery w osi z w stosunku do pozycji postaci

			Camera::SetPosition(position.x, position.y, position.z);			//ustawienie nowej pozycji kamery
			break;																//zakoñczenie przeszukiwania wekotra postaci po pierwszej 'odblokowanej' postaci
		}
	}
}

//ustalenie odpowiedniej orientacji kamery
void GameGraphics::SetCameraOrientation() {
		Vector camCenter;														//pozycja punktu 'wycentrowania' kamery
		Vector camPos = Camera::GetPosition();									//pozycja kamery
		Vector relativePos = Camera::GetRelativePosition();						//pozycja 'wzglêdna' kamery
		Vector relativeCenter = Camera::GetRelativeCenter();					//relatywna pozycja punktu 'wycentrowania' kamery
		Vector camOrientation = Camera::GetOrientation();						//orientacja kamery

		for (Player& player : GameObjects::players) {
			if (player.IsEnabled()) {
				Vector playerOrient = player.getOrientation();
				camOrientation.y = playerOrient.y;
				camOrientation.x = player.GetLookAngle();
				break;
			}
		}

		//przeliczenie orientacji kamery na radiany
		camOrientation.x = camOrientation.x / 360.0 * 2.0 *3.1415;
		camOrientation.y = camOrientation.y / 360.0 * 2.0 *3.1415;
		camOrientation.z = camOrientation.z / 360.0 * 2.0 *3.1415;
		
		//wyliczenie wspó³rzêdnych punktu 'wycentrowania' kamery ze wzoru:
		//y = R * sin( -camOrientation.x ), x = r * sin( camOrientation.y ), z = r * cos( camOrientation.y )
		//gdzie: R - odleg³oœæ od œrodka kamery do punktu 'wycentrowania', r - rzutowanie odleg³oœci R na p³aszczyznê (X,Z), obliczona zgodnie ze wzorem: r = sqrt( R^2 + y^2 )
		double R = sqrt((-relativePos.z + relativeCenter.z)*(-relativePos.z + relativeCenter.z) + relativePos.y*relativePos.y);

		camCenter.y = R*sin(-camOrientation.x);
			
		double r = sqrt(R*R + camCenter.y*camCenter.y);

		camCenter.x = camPos.x+r*sin(camOrientation.y);
		camCenter.y = camPos.y - camCenter.y;
		camCenter.z = camPos.z+r*cos(camOrientation.y);

		Camera::SetCenter(camCenter.x, camCenter.y, camCenter.z);				//ustawienie punktu 'wycentrowania' kamery
}

//wszystkie ustawienia kamery
void GameGraphics::SetCamera() {
	SetCameraPosition();														//ustawienie odpwoiedniej pozycji kamery
	SetCameraOrientation();														//ustawienie odpwoiedniej orientacji kamery
	Camera::SetCamera();														//wprowadzenie nowych ustawieñ kamery do maszyny stanów OpenGL
}

//wyœwietlenie danych wyjœciowych z 'konsoli'
void GameGraphics::ShowConsoleOutput() {
	CONSOLE::ShowConsoleOutput();
}

void GameGraphics::ShowGUI() {
	GUI::Show();
}