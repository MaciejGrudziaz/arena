#include "Gun.h"

//KONSTRUKTOR
Gun::Gun(const std::string& _name, const std::string& _parentName, const Vector& _parentMidPoint, const Vector& parentPos, const Vector& parentOrient):
Weapon(_name,_parentName,_parentMidPoint,parentPos,parentOrient){
	WeaponDatabaseRecord record = WeaponDatabase::GetWeaponStats(name);															//wczytanie odpowiedniego rekordu z bazy danych broni

	ROF = record.ROF;																											//przypisanie wartoœci statystyk broni pobranych z rekordu
	damage = record.damage;
	bulletSpeed = record.bulletSpeed;
	magazineCapacity = record.magazineCapacity;
	reloadTime = record.reloadTime * 1000;

	bulletsInMagLeft =magazineCapacity;
	bulletsCount = 90;

	reloadState = 100;
	actReloadTime = 0;

	shotTimeDif = 1000000.0 / (static_cast<double>(ROF) / 60.0);																//obliczenie sta³ej wartoœci czasu pomiêdzy wystrza³ami
	timeFromLastShot = 0;																										//inicjalizacja  zmiennej okreœlaj¹cej ile czasu mine³o od strza³u

	shotTimerName = parentName + "_WEAPON_SHOOT";																				//inicjalizacja nazwy TIMERA broni
	reloadTimerName = parentName + "_WEAPON_RELOAD";
	TIMER::AddTimeMarker(shotTimerName);																						//inicjalizacja TIMERA dla broni
	TIMER::AddTimeMarker(reloadTimerName);
}

void Gun::PerformAdditionalAction(ActionPtr Action) {
	switch (Action->name) {
		case RELOAD: {
			if (reloadState == 100 && bulletsCount > 0 && bulletsInMagLeft != magazineCapacity) {
				reloadState = 0;
				actReloadTime = 0;
				TIMER::StartTimeMeasurment(reloadTimerName);
			}
			break;
		}

		default:
			break;
	}
}

//funkcja wykonuj¹ca strza³ z broni
void Gun::Shoot() {
	if (bulletsInMagLeft > 0 && reloadState==100) {
		TIMER::StopTimeMeasurment(shotTimerName);																				//zatrzymanie TIMERA odliczaj¹cego czas pomiêdzy strza³ami												
		timeFromLastShot -= TIMER::GetTime(shotTimerName);																		//aktualizacja licznika czasu pomiêdzy strza³ami
		if (timeFromLastShot <= 0) {																							//jeœli min¹³ odpowiedni czas pomiêdzy strza³ami
			Vector angle = orientation / 360.0 * 2.0 * 3.1415;																	//przeliczenie orientacji broni na radiany
			Vector direction(cos(-angle.x)*sin(angle.y), sin(-angle.x), cos(-angle.x)*cos(angle.y));							//obliczenie wektora kierunkowego dla pocisku
			Matrix transform(4, 4);																								//macierz przekszta³cenia jednorodnego
			Matrix rotX(4, 4), rotY(4, 4), rotZ(4, 4);																			//macierze obrotu wokó³ osi X, Y i Z
			Vector startPoint = Vector(collisionBox.midPoint.x, collisionBox.midPoint.y, collisionBox.maxPoint.z);				//punkt startowy dla pozycji pocisku
			//stworzenie macierzy obrotu
			rotX[0][0] = 1; rotX[0][1] = rotX[0][2] = rotX[0][3] = 0;
			rotX[1][0] = 0; rotX[1][1] = cos(angle.x); rotX[1][2] = -sin(angle.x); rotX[1][3] = 0;
			rotX[2][0] = 0; rotX[2][1] = sin(angle.x); rotX[2][2] = cos(angle.x); rotX[2][3] = 0;
			rotX[3][0] = rotX[3][1] = rotX[3][2] = 0; rotX[3][3] = 1;

			rotY[0][0] = cos(angle.y); rotY[0][1] = 0; rotY[0][2] = sin(angle.y); rotY[0][3] = 0;
			rotY[1][0] = 0; rotY[1][1] = 1; rotY[1][2] = 0; rotY[1][3] = 0;
			rotY[2][0] = -sin(angle.y); rotY[2][1] = 0; rotY[2][2] = cos(angle.y); rotY[2][3] = 0;
			rotY[3][0] = rotY[3][1] = rotY[3][2] = 0; rotY[3][3] = 1;

			rotZ[0][0] = cos(angle.z); rotZ[0][1] = -sin(angle.z); rotZ[0][2] = 0; rotZ[0][3] = 0;
			rotZ[1][0] = sin(angle.z); rotZ[1][1] = cos(angle.z); rotZ[1][2] = 0; rotZ[1][3] = 0;
			rotZ[2][0] = 0; rotZ[2][1] = 0; rotZ[2][2] = 1; rotZ[2][3] = 0;
			rotZ[3][0] = rotZ[3][1] = rotZ[3][2] = 0; rotZ[3][3] = 1;

			transform = rotZ*rotY*rotX;																							//obliczenie macierzy przekszta³cenia jednorodnego		
			transform[0][3] = position.x; transform[1][3] = position.y; transform[2][3] = position.z;							//dodanie wektora przesuniêcia do macierzy

			Vector bulletPos = transform*startPoint;																			//obliczenie pozycji startowej dla pocisku po przekszta³ceniu

			shotsFired.push_back(Bullet(name, damage, bulletSpeed, direction, bulletPos));										//dodanie nowego pocisku do wektora pocisków
			timeFromLastShot = shotTimeDif;																						//uaktualnienie licznika czasu pomiêdzy strza³ami

			bulletsInMagLeft--;
		}
		TIMER::StartTimeMeasurment(shotTimerName);																				//uruchomienie TIMERA odmierzaj¹cego czas pomiêdzy strza³ami
	}
}

//aktualizacja stanu pocisków
void Gun::UpdateBullets() {
	std::vector<Bullet> validBullets;																							//wektor pocisków które nie uleg³y kolizji
	for (Bullet& bullet : shotsFired) {
		bullet.lifetime += static_cast<unsigned long>(PHYSICSMODULE::targetFrameTime);											//aktualizacja czasu ¿ycia pocisku
		double shift = bullet.speed*(bullet.lifetime / 1000000.0);																//droga przebyta przez pocisk od czasu wystrzelenia
		Vector newPos = bullet.startPos + bullet.direction*shift;																//nowa pozycja pocisku

		bullet.previousPos = bullet.actualPos;																					//aktualizacja pozycji
		bullet.actualPos = newPos;

		if (!bullet.hit)																										//jeœli pocisk nie uleg³ kolizji
			validBullets.push_back(bullet);																						//dodaj go do wektora 
	}
	shotsFired = validBullets;																									//aktualizacja wektora wystrzelonych pocisków
}

//narysowanie modelu
void Gun::DrawModel() {
	double bulletLength=5.0;																									//d³ugoœæ pocisku dla wizualizacji				

	OBJmodel::DrawModel();																										//narysowanie modelu broni

	DrawCrossHair();																											//narysowanie celownika

	//rysowanie pocisków
	glPushMatrix();																																															
	glDisable(GL_LIGHTING);
	glColor3f(1, 0, 0);																											//wybranie koloru czerwonego dla pocisków
	glLineWidth(3.0);	
	for (Bullet& bullet : shotsFired) {															
			Vector bulletEndPos;																								//pozycja krañca rysowanego pocisku
			Vector shiftVec = bullet.actualPos - bullet.startPos;																//wektor przesuniêcia dla aktualnej pozycji od pozycji startowej								
			if (shiftVec.MOD() < bulletLength)																					//jesli d³. tego wek. jest mniejsza od d³. pocisku
				bulletEndPos = bullet.startPos;																					//pozycj¹ krañcow¹ pocisku jest pozycja pocz¹tkowa
			else bulletEndPos = bullet.actualPos - bullet.direction*bulletLength;												//w przeciwnym wypadku jest to pozycja wzglêdna do aktualnej

			glBegin(GL_LINES);																									//narysowanie pocisku
			glVertex3d(bullet.actualPos.x, bullet.actualPos.y, bullet.actualPos.z);
			glVertex3d(bulletEndPos.x, bulletEndPos.y, bulletEndPos.z);
			glEnd();
	}
	glLineWidth(1.0);																											//przywrócenie poprzednich w³aœciwoœci masznie stanów OpenGL
	glColor3d(0.8, 0.8, 0.8);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

//narysowanie celownika
void Gun::DrawCrossHair() {
	Vector midPt = collisionBox.midPoint, size = collisionBox.size;																//pobranie punktu œrodkowego dla broni oraz jej wielkoœci 
	if (size.x > size.y) size.y = size.x;																						//wyrównanie wielkoœci broni w osiach X i Y [do max wart.]														
	else size.x = size.y;

	glPushMatrix();

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glColor3d(1.0, 1.0, 1.0);

	glTranslated(position.x, position.y, position.z);																			//przekszta³cenie macierzy stanów OpenGL																	
	glRotated(orientation.z, 0, 0, 1);
	glRotated(orientation.y, 0, 1, 0);
	glRotated(orientation.x, 1, 0, 0);

	glLineWidth(3.0);																											//gruboœæ linii
	glBegin(GL_LINES);
	glVertex3d(midPt.x, midPt.y + 20 * size.y, midPt.z + 20 * size.z);															//górna linia
	glVertex3d(midPt.x, midPt.y + 5*size.y, midPt.z + 20 * size.z);

	glVertex3d(midPt.x - 20* size.x, midPt.y, midPt.z + 20 * size.z);															//lewa linia
	glVertex3d(midPt.x - 5 * size.x, midPt.y, midPt.z + 20 * size.z);

	glVertex3d(midPt.x, midPt.y - 20 * size.y, midPt.z + 20 * size.z);															//dolna linia			
	glVertex3d(midPt.x, midPt.y - 5 * size.y, midPt.z + 20 * size.z);

	glVertex3d(midPt.x + 20 * size.x, midPt.y, midPt.z + 20 * size.z);															//prawa linia
	glVertex3d(midPt.x + 5 * size.x, midPt.y, midPt.z + 20 * size.z);
	glEnd();

	glLineWidth(1.0);																											//przywrócenie 	poprzednich w³aœciwoœci maszyny stanów OpenGL											
	glColor3d(0.8, 0.8, 0.8);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);

	glPopMatrix();
}

void Gun::Update() {
	PerformAllActions();
	DynamicCharacter::Update();

	if (reloadState != 100) {
		TIMER::StopTimeMeasurment(reloadTimerName);
		unsigned long timeVal = TIMER::GetTime(reloadTimerName);
		actReloadTime += timeVal;
		reloadState = static_cast<unsigned>((static_cast<double>(actReloadTime) / static_cast<double>(reloadTime)) * 100);
		if (reloadState > 100)
			reloadState = 100;

		if (reloadState < 100)
			TIMER::StartTimeMeasurment(reloadTimerName);
		else {
			if (bulletsCount >= (magazineCapacity - bulletsInMagLeft)) {
				bulletsCount -= (magazineCapacity - bulletsInMagLeft);
				bulletsInMagLeft = magazineCapacity;
			}
			else {
				bulletsInMagLeft += bulletsCount;
				bulletsCount = 0;
			}
		}
	}
}