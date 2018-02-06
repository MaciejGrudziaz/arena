#pragma once
#include <windows.h>
#include <gl/GL.h>

//----------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------œWIAT£O--------------------------------------------------------																												
//------------------------------klasa definiuj¹ca i inicjalizuj¹ca Ÿród³o œwiat³a w OpenGL------------------------------
//----------------------------------------------------------------------------------------------------------------------

//definicja parametrów u¿ywanych do okreœlenia Ÿród³a œwiat³a
//kombinacja tych parametrów okreslana jest do zdefiniowania zmiennej lightParams
//ka¿demu parametrowi przypisany jest konkretny bit w s³owie lightParams
#define ambientLight					0x001
#define diffuseLight					0x002
#define specularLight					0x004
#define lightPosition					0x008
#define lightDirection					0x010
#define lightSpotExponent				0x020
#define lightSpotCutoff					0x040
#define lightConstantAttenuation		0x080
#define lightLinearAttenuation			0x100
#define lightQuadraticAttenuation		0x200

class Light {
	GLenum lightIdx;									//ozanczenie Ÿród³a œwiat³a [GL_LIGHT0, ... , GL_LIGHT7] 
	unsigned int lightParams;							//parametry Ÿród³a œwiat³a
	bool enable;										//uruchomienie Ÿród³a œwiat³a

	float position[4];									//pozycja Ÿród³a œwiat³a
	float direction[3];									//kierunek wektora Ÿród³a œwiat³a
	float ambientParams[4];								//parametry œwiat³a otoczenia
	float diffuseParams[4];								//parametry œwiat³a rozproszonego
	float specularParams[4];							//parametry œwiat³a odbicia
	float spotExponentParam;							//parametr koncentracji strumienia œwiat³a
	float spotCutoffParam;								//parametr k¹ta rozwarcia strumienia œwiat³a
	float constantAttenuationParam;						//parametr sta³ej wartoœci t³umienia
	float linearAttenuationParam;						//parametr liniowej wartoœci t³umienia
	float quadraticAttenuationParam;					//parametr kwadratowej wartoœci t³umienia

public:
	Light();											//konstruktor domyœlny
	Light(GLenum LightIdx, unsigned int params);		//konstruktor rozszerzony [wczytuje od razu podstawowe parametry obiektu]

	//inicjalizacja w³aœciwoœci Ÿród³a œwiat³a
	void Initialize();

	//usatwienie odpowiednich parametrów Ÿród³a œwiat³a
	void SetLightIdx(GLenum idx) { lightIdx = idx; }
	void SetLightParams(unsigned int params) { lightParams = params; }
	void SetPosition(float X, float Y, float Z);
	void SetDirection(float X, float Y, float Z);
	void SetAmbientParams(float R, float G, float B, float a);
	void SetDiffuseParams(float R,float G,float B,float a);
	void SetSpecularParams(float R, float G, float B, float a);
	void SetSpotExponentParam(float param) { spotExponentParam = param; }
	void SetSpotCutoffParam(float param) { spotCutoffParam = param; }
	void SetConstantAttenuationParam(float param) { constantAttenuationParam = param; }
	void SetLinearAttenuationParam(float param) { linearAttenuationParam = param; }
	void SetQuadraticAttenuationParam(float param) { quadraticAttenuationParam = param; }
	
	//w³¹czenie Ÿród³a œwiat³a
	void EnableLight() { glEnable(lightIdx); enable = true; }
	//wy³¹czenie Ÿród³a œwiat³a
	void DisableLight() { glDisable(lightIdx); enable = false; }

	bool IsEnabled() { return enable; }					//sprawdzenie uruchomienia Ÿród³a œwiat³a
};