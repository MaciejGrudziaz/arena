#pragma once
#include <windows.h>
#include <gl/glu.h>
#include "Vector.h"

//------------------------------------------------------------------------------------------------------
//------------------------------------------------KAMERA------------------------------------------------
//--------------------klasa opisuj¹ca obecne po³o¿enie i orientacjê kamery dla sceny--------------------
//------------------------------------------------------------------------------------------------------

class Camera {
	static Vector position;									//pozycja kamery
	static Vector orientation;								//orientacja kamery
	static Vector center;									//pozycja punktu w kierunku którym 'patrzy' kamera
	static Vector upvector;									//wektor wskazuj¹cy 'górê' kamery

	static Vector relativePos;								//pozycja kamery wzglêdem postaci gracza
	static Vector relativeCenter;							//pozycja punktu w kierunku którym 'patrzy' kamera wzglêdem postaci gracza

public:
	//inicjalizacja kamery przy pomocy wektorów
	static void Initialize(Vector Position, Vector Center, Vector upVector);
	//inicjalizacja kamery przy pomocy zmiennych opisuj¹cych ka¿d¹ wspó³rzêdn¹
	static void Initialize(double xPos, double yPos, double zPos, double xCen, double yCen, double zCen, double xUpVec, double yUpVec, double zUpVec);
	//ustawienie kamery
	static void SetCamera();

	//funkcje ustawiaj¹ce odpowiednie wartoœci dla wpsó³rzêdnych kamery 
	static void SetPosition(double x, double y, double z) { position.x = x; position.y = y; position.z = z; }
	static void SetOrientation(double x, double y, double z) { orientation.x = x; orientation.y = y; orientation.z = z; }
	static void SetCenter(double x, double y, double z) { center.x = x; center.y = y; center.z = z; }
	static void SetUpVector(double x, double y, double z) { upvector.x = x; upvector.y = y; upvector.z = z; }
	static void SetRelativePosition(double x, double y, double z) { relativePos.x = x; relativePos.y = y; relativePos.z = z; }
	static void SetRelativeCenter(double x, double y, double z) { relativeCenter.x = x; relativeCenter.y = y; relativeCenter.z = z; }

	//funkcje pobieraj¹ce odpowiednie wartoœci wspó³rzêdnych kamery
	static Vector GetPosition() { return position; }
	static Vector GetOrientation() { return orientation; }
	static Vector GetCenter() { return center; }
	static Vector GetUpVector() { return upvector; }
	static Vector GetRelativePosition() { return relativePos; }
	static Vector GetRelativeCenter() { return relativeCenter; }
};