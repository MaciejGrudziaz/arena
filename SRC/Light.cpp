#include "Light.h"

//konstruktor domyœlny
Light::Light() {
	lightIdx = GL_LIGHT0;																//pierwsze Ÿród³o œwiat³a
	lightParams = 0;																	//brak parametrów
	enable = false;																		//Ÿród³o wy³¹czone

	//zainicjalizowanie wszystkich parametrów Ÿród³a œwiat³a wartoœciami domyœlnymi
	position[4] = { 0 };
	direction[3] = { 0 };
	ambientParams[4] = { 0 };
	diffuseParams[4] = { 0 };
	specularParams[4] = { 0 };
	spotExponentParam = 0.0;
	spotCutoffParam = 0.0;
	constantAttenuationParam = 0.0;
	linearAttenuationParam = 0.0;
	quadraticAttenuationParam = 0.0;
}

//konstruktor rozszerzony [wczytuje podstawowe parametry obiektu od razu przy tworzeniu]
Light::Light(GLenum LightIdx, unsigned int params) {
	lightIdx = LightIdx;																//indeks Ÿród³a œwiat³a [LIGHT0, ... , LIGHT7]
	lightParams = params;																//parametry Ÿród³a œwiat³a
	enable = false;																		//Ÿród³o wy³¹czone

	//zainicjalizowanie wszystkich parametrów Ÿród³a œwiat³a wartoœciami domyœlnymi
	position[4] = { 0 };
	direction[3] = { 0 };
	ambientParams[4] = { 0 };
	diffuseParams[4] = { 0 };
	specularParams[4] = { 0 };
	spotExponentParam = 0.0;
	spotCutoffParam = 0.0;
	constantAttenuationParam = 0.0;
	linearAttenuationParam = 0.0;
	quadraticAttenuationParam = 0.0;
}

//zainicjalizowanie Ÿród³a œwiat³a w OpenGL
void Light::Initialize() {
	//w zale¿noœci od wartoœci konkretnych bitów w zmiennej lightParams inicjalizujemy konkretne parametry Ÿród³a œwiat³a
	if (lightParams & ambientLight)
		glLightfv(lightIdx, GL_AMBIENT, ambientParams);									//intensywnoœæ œwiat³a otoczenia
	if (lightParams & diffuseLight)
		glLightfv(lightIdx, GL_DIFFUSE, diffuseParams);									//intensywnoœæ œwiat³a rozproszonego
	if (lightParams & specularLight)			
		glLightfv(lightIdx, GL_SPECULAR, specularParams);								//intensywnoœæ œwait³a odbicia
	if (lightParams & lightPosition)
		glLightfv(lightIdx, GL_POSITION, position);										//pozycja Ÿród³a œwiat³a
	if (lightParams & lightDirection)
		glLightfv(lightIdx, GL_SPOT_DIRECTION, direction);								//wektor kierunku Ÿród³a œwiat³a
	if (lightParams & lightSpotExponent)
		glLightf(lightIdx, GL_SPOT_EXPONENT, spotExponentParam);						//koncentracja strumienia œwiat³a
	if (lightParams & lightSpotCutoff)
		glLightf(lightIdx, GL_SPOT_CUTOFF, spotCutoffParam);							//k¹t rozwarcia strumienia œwiat³a
	if (lightParams & lightConstantAttenuation)
		glLightf(lightIdx, GL_CONSTANT_ATTENUATION, constantAttenuationParam);			//sta³a wartoœæ t³umienia
	if (lightParams & lightLinearAttenuation)
		glLightf(lightIdx, GL_LINEAR_ATTENUATION, linearAttenuationParam);				//liniowa wartoœæ t³umienia
	if (lightParams & lightQuadraticAttenuation)
		glLightf(lightIdx, GL_QUADRATIC_ATTENUATION, quadraticAttenuationParam);		//kwadratowa wartoœæ t³umienia
}

//ustawienie pozycji Ÿród³a œwiat³a
void Light::SetPosition(float X, float Y, float Z) {
	position[0] = X;
	position[1] = Y;
	position[2] = Z;
	position[3] = 1.0;
}

//ustawienie kierunku strumienia œwiat³a
void Light::SetDirection(float X, float Y, float Z) {
	direction[0] = X;
	direction[1] = Y;
	direction[2] = Z;
}

//ustaweinie intensywnoœci œwiat³a otoczenia
void Light::SetAmbientParams(float R, float G, float B, float a) {
	ambientParams[0] = R;
	ambientParams[1] = G;
	ambientParams[2] = B;
	ambientParams[3] = a;
}

//ustawienie intensywnoœci œwiat³a rozproszonego
void Light::SetDiffuseParams(float R, float G, float B, float a) {
	diffuseParams[0] = R;
	diffuseParams[1] = G;
	diffuseParams[2] = B;
	diffuseParams[3] = a;
}

//ustwienie intesywnoœci œwiat³a odbicia
void Light::SetSpecularParams(float R, float G, float B, float a) {
	specularParams[0] = R;
	specularParams[1] = G;
	specularParams[2] = B;
	specularParams[3] = a;
}