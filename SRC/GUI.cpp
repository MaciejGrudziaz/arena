#include "GUI.h"

unsigned GUI::normalFontSize = 28;
unsigned GUI::smallFontSize = 16;
unsigned GUI::bigFontSize = 36;
double GUI::screenNormalFontHeight = 0;
double GUI::screenSmallFontHeight = 0;
double GUI::screenBigFontHeight = 0;
FONT GUI::guiNormalFont;
FONT GUI::guiSmallFont;
FONT GUI::guiBigFont;
double GUI::screenDepth = 1;
double GUI::screenWidth = 0;
double GUI::screenHeight = 0;
double GUI::screenRatio = 0;

void GUI::Initialize() {
	guiNormalFont.Initilize("Arial", normalFontSize, WinAPIwindow::hDC);
	guiSmallFont.Initilize("Arial", smallFontSize, WinAPIwindow::hDC);
	guiBigFont.Initilize("Arial", bigFontSize, WinAPIwindow::hDC);

	screenHeight = screenDepth*tan((WinAPIwindow::FOV / 2.0) / 360.0 * 2 * 3.1415);
	screenRatio = static_cast<double>(WinAPIwindow::screenWidth) / static_cast<double>(WinAPIwindow::screenHeight);
	screenWidth = screenRatio*screenHeight;

	screenNormalFontHeight = (static_cast<double>(normalFontSize) / (static_cast<double>(WinAPIwindow::screenHeight / 2.0) ))*screenHeight;
	screenSmallFontHeight = (static_cast<double>(smallFontSize) / (static_cast<double>(WinAPIwindow::screenHeight / 2.0)))*screenHeight;
	screenBigFontHeight = (static_cast<double>(bigFontSize) / (static_cast<double>(WinAPIwindow::screenHeight / 2.0)))*screenHeight;
}

void GUI::Show() {
	DrawAmmoCount();
	DrawReloadInfo();
	DrawPlayerHealthInfo();
	DrawBotsHealthInfo();
}

void GUI::DrawAmmoCount() {
	if (CharacterRegister::GetPlayer()->GetWeapon() != nullptr) {
		Gun& playerGun = static_cast<Gun&>(*(CharacterRegister::GetPlayer()->GetWeapon()));
		unsigned bulletMagCount = playerGun.GetBulletsLeftInMag();
		unsigned bulletCount = playerGun.GetBulletsCount();
		unsigned magCapacity = playerGun.GetMagazineCapacity();
		Vector bulletMagColor, bulletCountColor;

		if (bulletMagCount <= 0.3*magCapacity)
			bulletMagColor = Vector(1.0, 0.0, 0.0);
		else
			bulletMagColor = Vector(0, 0, 0);

		if (bulletCount <= magCapacity)
			bulletCountColor = Vector(1.0, 0.0, 0.0);
		else
			bulletCountColor = Vector(0.3, 0.3, 0.3);

		char bulletMagChar[10], bulletCountChar[10];
		_itoa_s(bulletMagCount, bulletMagChar, 10);
		_itoa_s(bulletCount, bulletCountChar, 10);

		WriteTextOnScreen(Vector(screenWidth - 6 * screenNormalFontHeight, -(screenHeight - screenNormalFontHeight), -screenDepth), bulletMagChar, bulletMagColor);
		WriteTextOnScreen(Vector(screenWidth - 4 * screenNormalFontHeight, -(screenHeight - screenNormalFontHeight), -screenDepth), "|", Vector(0.3, 0.3, 0.3));
		WriteTextOnScreen(Vector(screenWidth - 3 * screenNormalFontHeight, -(screenHeight - screenNormalFontHeight), -screenDepth), bulletCountChar, bulletCountColor);
	}
}

void GUI::DrawReloadInfo() {
	if (CharacterRegister::GetPlayer()->GetWeapon() != nullptr) {
		Gun& playerGun = static_cast<Gun&>(*(CharacterRegister::GetPlayer()->GetWeapon()));
		unsigned bulletMagCount = playerGun.GetBulletsLeftInMag();
		unsigned reloadState = playerGun.GetReloadState();

		if (bulletMagCount == 0 && reloadState == 100)
			WriteTextOnScreen(Vector(-1.5*screenSmallFontHeight, -0.3*screenHeight, -screenDepth), "RELOAD", Vector(0.5, 0, 0), GUICONST::SMALL);

		if (reloadState != 100) {
			double val = static_cast<double>(reloadState) / 100.0;
			Vector beginPos(-screenWidth*0.1, -0.25*screenHeight, -screenDepth);
			Vector endPos(-screenWidth*0.1 + val*0.2*screenWidth, -0.25*screenHeight, -screenDepth);
			Vector color;

			if (val < 0.4)
				color = Vector(1.0, 0.0, 0.0);
			else if (val < 0.9)
				color = Vector(1.0, 1.0, 0.0);
			else
				color = Vector(0.0, 1.0, 0.0);

			DrawLine(beginPos, endPos, 3.0, color);
		}
	}
}

void GUI::DrawPlayerHealthInfo() {
	if (CharacterRegister::GetPlayer() != nullptr) {
		unsigned playerHealth = CharacterRegister::GetPlayer()->GetHealth();
		double val = static_cast<double>(playerHealth) / 100.0;
		Vector beginPos, endPos, color;
		beginPos = Vector(-0.3*screenWidth, -(0.9*screenHeight - screenNormalFontHeight*0.5), -screenDepth);
		endPos = Vector(-0.3*screenWidth + val*0.6*screenWidth, -(0.9*screenHeight-screenNormalFontHeight*0.5), -screenDepth);

		if (playerHealth < 30)
			color = Vector(1.0, 0.0, 0.0);
		else if (playerHealth < 80)
			color = Vector(1.0, 1.0, 0.0);
		else color = Vector(0.0, 1.0, 0.0);

		char playerHealthChar[10];
		_itoa_s(playerHealth, playerHealthChar, 10);

		WriteTextOnScreen(Vector(-(0.4*screenWidth + 1.75*screenNormalFontHeight), -0.9*screenHeight, -screenDepth), playerHealthChar, Vector(1.0, 1.0, 1.0));
		WriteTextOnScreen(Vector(-0.4*screenWidth, -0.9*screenHeight, -screenDepth), "HP", Vector(1.0, 1.0, 1.0));
		
		DrawBar(beginPos, endPos, 0.5*screenNormalFontHeight, color);
	}
}

void GUI::DrawBotsHealthInfo() {
	if (CharacterRegister::GetPlayer()->IsEnabled()) {
		Vector playerOrient = CharacterRegister::GetPlayer()->getOrientation();
		playerOrient = playerOrient / 360.0 * 2.0 *3.1415;
		Vector playerLookVec(sin(playerOrient.y), 0, cos(playerOrient.y));
		Vector healthBarOrient = playerLookVec.CrossProduct(Vector(0, 1, 0));

		for (unsigned i = 0; i < CharacterRegister::GetBotCount(); i++) {
			if (CharacterRegister::GetBot(i)->IsEnabled()) {
				Bot* botPtr = CharacterRegister::GetBot(i);
				unsigned botHealth = botPtr->GetHealth();
				double botHealthRatio = static_cast<double>(botHealth) / 100.0;
				Vector botPos = botPtr->getPosition();
				Vector botMaxPt = botPtr->getCollisionBox().maxPoint;
				Vector botSize = botPtr->getCollisionBox().size;
				Vector labelMidPos = botPos;
				labelMidPos.y += botMaxPt.y+0.25*botSize.y;
				Vector beginPos, endPos, color;

				if (botHealth < 30)
					color = Vector(1.0, 0.0, 0.0);
				else if (botHealth < 80)
					color = Vector(1.0, 1.0, 0.0);
				else color = Vector(0.0, 1.0, 0.0);

				beginPos = Vector(labelMidPos.x - healthBarOrient.x, labelMidPos.y, labelMidPos.z - healthBarOrient.z);
				endPos = Vector(labelMidPos.x - healthBarOrient.x + botHealthRatio*2.0*healthBarOrient.x, labelMidPos.y, labelMidPos.z - healthBarOrient.z + botHealthRatio*2.0*healthBarOrient.z);

				DrawBar(beginPos, endPos, 0.05*botSize.y, color, false);
			}
		}
	}
}

void GUI::WriteTextOnScreen(Vector position, char buffer[], Vector color, GUICONST::FONTSIZE size) {
	glPushMatrix();															//zapisanie macierzy widoku OepnGL ['MODELVIEW'] na stosie 

	glLoadIdentity();

	glDisable(GL_LIGHTING);													//wy³¹czenie oœwietlenia sceny					
	glDisable(GL_DEPTH_TEST);

	glColor3f(static_cast<GLfloat>(color.x), static_cast<GLfloat>(color.y),
		static_cast<GLfloat>(color.z));										//wybranie koloru wyœwietlanego napisu [bia³y]
	glTranslated(position.x, position.y, position.z);						//ustawienie odpowiedniej pozycji								
	glRasterPos2d(0.0, 0.0);
	
	//wyœwietlenie napisu
	switch (size) {
	case GUICONST::NORMAL:
		guiNormalFont.PrintString(buffer);									
		break;
	case GUICONST::SMALL:
		guiSmallFont.PrintString(buffer);
		break;
	case GUICONST::BIG:
		guiBigFont.PrintString(buffer);
		break;
	default:
		break;
	}
	glColor3d(0.8, 0.8, 0.8);												//przywrócenie podstawowych kolorów dla wyœwietlania modeli

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);													//w³¹czenie oœwieltenia sceny

	glPopMatrix();															//przywrócenie macierzy widoku OpenGL ze stosu
}

void GUI::DrawLine(Vector begin, Vector end, double thickness, Vector color, bool onScreen) {
	glPushMatrix();

	if(onScreen)
		glLoadIdentity();

	glDisable(GL_LIGHTING);													//wy³¹czenie oœwietlenia sceny					
	glDisable(GL_DEPTH_TEST);

	glColor3d(color.x, color.y, color.z);
	glLineWidth((GLfloat)thickness);

	glBegin(GL_LINES);
	glVertex3d(begin.x, begin.y, begin.z);
	glVertex3d(end.x, end.y, end.z);
	glEnd();

	glLineWidth(1.0);
	glColor3d(0.8, 0.8, 0.8);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);													//w³¹czenie oœwieltenia sceny

	glPopMatrix();															//przywrócenie macierzy widoku OpenGL ze stosu
}

void GUI::DrawBar(Vector begin, Vector end, double thickness, Vector color, bool onScreen) {
	glPushMatrix();

	if (onScreen)
		glLoadIdentity();

	glDisable(GL_LIGHTING);													//wy³¹czenie oœwietlenia sceny					

	glColor3d(color.x, color.y, color.z);

	glBegin(GL_POLYGON);
	glVertex3d(begin.x, begin.y, begin.z);
	glVertex3d(begin.x, begin.y - thickness, begin.z);
	glVertex3d(end.x, end.y - thickness, end.z);
	glVertex3d(end.x, end.y, end.z);
	glEnd();

	glColor3d(0.8, 0.8, 0.8);

	glEnable(GL_LIGHTING);													//w³¹czenie oœwieltenia sceny

	glPopMatrix();															//przywrócenie macierzy widoku OpenGL ze stosu
}