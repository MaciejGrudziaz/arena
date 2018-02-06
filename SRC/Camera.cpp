#include "Camera.h"

//inicjalizacja sk³adowych staycznych klasy
Vector Camera::position = { 0.0, 0.0, 0.0 };
Vector Camera::orientation = { 0.0,0.0,0.0 };
Vector Camera::center = { 0.0, 0.0, -10.0 };
Vector Camera::upvector = { 0.0, 0.0, 0.0 };
Vector Camera::relativePos = { 0.0,0.0,0.0 };
Vector Camera::relativeCenter = { 0.0,0.0,0.0 };

//inicjalizacja kamery przy pomocy wektorów
void Camera::Initialize(Vector Position, Vector Center, Vector UpVector)
{
	position = Position;
	center = Center;
	upvector = UpVector;
}

//inicjalizacja kamery przy pomocy zmiennych opisuj¹cych dane wspó³rzêdne
void Camera::Initialize(double xPos, double yPos, double zPos, double xCen, double yCen, double zCen, double xUpVec, double yUpVec, double zUpVec)
{
	position.x = xPos;
	position.y = yPos;
	position.z = zPos;

	center.x = xCen;
	center.y = yCen;
	center.z = zCen;

	upvector.x = xUpVec;
	upvector.y = yUpVec;
	upvector.z = zUpVec;
}

//ustawienie kamery w scenie OpenGL
void Camera::SetCamera()
{
	//funkcja ustwaij¹ca kamerê z biblioteki GLU
	gluLookAt(position.x, position.y, position.z, center.x, center.y, center.z, upvector.x, upvector.y, upvector.z);
}