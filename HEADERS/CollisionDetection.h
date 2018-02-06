#pragma once
#include "Player.h"
#include "Map.h"
#include <time.h>

//------------------------------------------------------------------------------------------------------
//-------------------------------------------DETEKCJA KOLIZJI-------------------------------------------
//--------------------klasa odpowiadaj¹ca ze detekcjê kolizji obiektów w œwiecie gry--------------------
//------------------------------------------------------------------------------------------------------

const unsigned int RayCastLinesNum = 128;																					//liczba prostych u¿ywanych przy algorytmie 'Ray Cast'

struct ModelCollision {
	bool status;																											//status kolizji: TRUE - kolizja zasz³a, FALSE - nie by³o kolizji
	//sk³adowe klasy okreœlaj¹ce detekcjê kolizji tylko z g³ównym hitboxem modelu/obiektu
	//u¿ywane jeœli dany model/obiekt posiada TYLKO g³ównego hitboxa
	//jeœli zaszla kolizja z g³ównym hitboxem modelu ['Character']:						'mainColBox = TRUE', 'mainColBoxObjIdx = -1'
	//jeœli zasz³a kolizja z g³ównym hitboxem obiektu ['Object' dla klasy 'Character']: 'mainColBox = TRUE', 'mainColBoxObjIdx = indeks_obiektu'
	bool mainColBox;																										//status kolizji z g³ównym hitboxem modelu/obiektu
	int mainColBoxObjIdx;																									//indeks obiektu dla którego zasz³a kolizja z g³ównym hitboxem
	//wektor przechowuj¹cy pary (objIdx, colBoxIdx)
	//'objIdx' - indeks obiektu z któym zasz³a kolizja dla aktualnie sprawdzanego modelu
	//'colBoxIdx' - indeks dla bry³y kolizji dla obiektu opisanego poprzez 'indeks obiektu' z któr¹ zasz³a kolizja dla aktualnie sprawdzanego modelu
	std::vector<std::pair<unsigned int, unsigned int>> colIdx;

	ModelCollision() :status(false), mainColBox(false), mainColBoxObjIdx(-1) {}
};

//wektor przechowuj¹cy pary (normal, linesCrossed) dla danej pary (objIdx,colBoxIdx)
//'normal' - wektor normalny do œciany badanej bry³y kolizji ('colBoxIdx')
//'linesCrossed' - liczba prostych przechodz¹cych przez œcianê opisan¹ przez normaln¹ 'normal' podczas testu 'Ray Cast' dla aktualnie badanej bry³y kolizji
typedef std::vector<std::pair<Vector, unsigned int>> CollisionNormals;

class CollisionDetection {
	//-----------------------------------------------------------------------------------------------------------------------------
	//--------------------------------------------------------ALGORYTM GJK---------------------------------------------------------
	//------------------------------------------------funckje tworz¹ce algorytm GJK------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------------------------
	static bool GJK(std::vector<Vector>&model1, std::vector<Vector>&model2);												//g³ówna funkcja algorytmu GJK

	static Vector GJK_StartVector(std::vector<Vector>&model1, std::vector<Vector>&model2);									//wyliczanie pocz¹tkowego wektora kierunkowego

	static Vector GJK_SupportFunction(std::vector<Vector>&model1, std::vector<Vector>&model2,Vector d);						//'Support function' dla GJK

	static bool GJK_SimplexContainsORIGIN(std::vector<Vector>& Simplex);													//sprawdzanie czy Sympleks zawiera punkt (0,0,0)
	static void GJK_SimplexContainsORIGIN_CreateNormals(std::vector<Vector>& Simplex, Vector n[]);							//funkcja pomocnicza - budowanie normalnych dla Sympleksa
	static void GJK_SimplexContainsORIGIN_CreateSurfaces(std::vector<Vector>& Simplex, Vector n[], double Surface[][4]);	//funkcja pomocnicza - budowanie powierzchni dla Sympleksa

	static Vector GJK_GetDirection(std::vector<Vector>&Simplex);															//funkcja wyznaczaj¹ca wektor kierunkowy w kolejnych iteracjach algorytmu

	//Iteracja 1 [2 punkty w Sympleksie]
	static Vector GJK_GetDirection_Iteration_1(std::vector<Vector>&Simplex);
	//Iteracja 2 [3 punkty w Sympleksie]
	static Vector GJK_GetDirection_Iteration_2(std::vector<Vector>&Simplex);												
	static Vector GJK_GetDirection_Iteration_2_AB(std::vector<Vector>&Simplex, Vector AB, Vector AC, Vector A0);
	static Vector GJK_GetDirection_Iteration_2_AC(std::vector<Vector>&Simplex, Vector AB, Vector AC, Vector A0);
	static Vector GJK_GetDirection_Iteration_2_CBA(Vector AB, Vector AC, Vector A0);
	static Vector GJK_GetDirection_Iteration_2_ABC(Vector AB, Vector AC, Vector A0);
	//Iteracja 3 [4 punkty w Sympleksie]
	static Vector GJK_GetDirection_Iteration_3(std::vector<Vector>&Simplex);
	static Vector GJK_GetDirection_Iteration_3_AB(std::vector<Vector>&Simplex, Vector AB, Vector AC, Vector A0);
	static Vector GJK_GetDirection_Iteration_3_AC(std::vector<Vector>&Simplex, Vector AB, Vector AC, Vector A0);
	static Vector GJK_GetDirection_Iteration_3_AD(std::vector<Vector>&Simplex, Vector AC, Vector AD, Vector A0);
	static Vector GJK_GetDirection_Iteration_3_ABC(std::vector<Vector>&Simplex, Vector AC, Vector AB, Vector A0);
	static Vector GJK_GetDirection_Iteration_3_ABD(std::vector<Vector>&Simplex, Vector AB, Vector AD, Vector A0);
	static Vector GJK_GetDirection_Iteration_3_ACD(std::vector<Vector>&Simplex, Vector AD, Vector AC, Vector A0);

	//-----------------------------------------------------------------------------------------------------------------------------
	//--------------------------FUNKCJE POMOCNICZE DLA METOD DETECKCJI KOLIZJI ['CollisionDetection(..)']--------------------------
	//-----------------------------------------------------------------------------------------------------------------------------

	//'CollisionDetection(..)' dla dwóch modeli 'Character'
	static bool CollisionDetection_CharChar_CheckMainColBoxCollision(Character& model2, 
		std::vector<Vector>& colBoxPlayerVertices);																			//sprawdzenie czy zasz³a kolizja z g³ównym hitboxem badanego modelu
	static ModelCollision CollisionDetection_CharChar_LoadNewCol(Character& model2, 
		std::vector<Vector>& colBoxPlayerVertices);																			//za³adowanie wartoœci dla obiektu detekcji kolizji 'ModelCollision'

	//'CollisionDetection(..)' dla modelu 'Character' z obiektem 'Object' wchodz¹cym w sk³ad modelu 'Character'
	static bool CollisionDetection_CharObj_CheckMainColBoxCollision(Character& model2, 
		unsigned model2ObjIdx, std::vector<Vector>& colBoxMod1Vertices);													//sprawdzenie czy zasz³a kolizja z g³ównym hitboxem badanego obiektu
	static ModelCollision CollisionDetection_CharObj_LoadNewCol(Character& model2, 
		unsigned model2ObjIdx, std::vector<Vector>& colBoxMod1Vertices);													//za³adownie wartoœci dla obiektu detekcji kolizji 'ModelCollision'

	//-----------------------------------------------------------------------------------------------------------------------------
	//-----------FUNKCJE POMOCNICZE DLA METODY ODNAJDYWANIA NORMALNEJ DO POWIERZCHNI KOLIZJI ['GetCollisionFaceNormal']------------
	//-----------------------------------------------------------------------------------------------------------------------------

	static CollisionNormals GetCollisonFaceNormal_StandardHitboxCol(Character& model1, Character& model2, ModelCollision& col);		//pobranie normalnej do powierzchni kolizji dla standardowego hitboxa
	static CollisionNormals GetCollisonFaceNormal_MainHitboxCol(Character& model1, Character& model2, ModelCollision& col);			//pobranie normalnej do powierzchni kolizji dla g³ównego hitboxa

	//-----------------------------------------------------------------------------------------------------------------------------

	static bool CheckPointInTriangle(Vector point, Vector triangle[], Vector normal);										//funkcja pomocnicza sprawdzaj¹ca czy dany punkt znajduje siê wewn¹trz trójk¹ta

	static std::pair<Vector, unsigned> GetCollisionFaceNormal_GetNormal(Character &model1,
		Character& model2,CollisionBox &colBoxChar);																		//pobranie normalnej do powierzchni obiektu z któr¹ zasz³a kolizja
	static void GetCollisionFaceNormal_LoadPoints(Vector points[], Character& model1);										//za³adowanie punktów pocz¹tkowych dla prostych do algorytmu 'Ray Casting'
	static int GetCollisionFaceNormal_GetNormal_RayCast(Vector faceP[], Vector point[], 
		Vector normal, double surface[],double R);																			//wykonanie algorytmu 'RayCast' dla funkcji pobierania normalnej

public:
	static unsigned GJK_IT;

	static ModelCollision CheckCollision(Character& model1, Character& model2);												//sprawdzanie kolizji pomiêdzy dwoma modelami

	static ModelCollision CheckCollision(Character& model1, Character& model2, unsigned model2ObjIdx);						//sprawdzenie kolizji pomiêdzy modelem, a wybranym obiektem z drugiego modelu	

	static CollisionNormals GetCollisonFaceNormals(Character& model1,Character& model2, ModelCollision col);							//pobranie normalnej do œciany obiektu z którym zasz³a kolizja
	
	static bool GettingOutOfCollision(Character& model, Vector n);															//sprawdzenie czy model postaci 'wychodzi' ze stanu kolizji

	static Vector GettingIntoCollision(Character& model, Vector n);										//zwrócenie wartoœci wektora dla postaci która znajduje siê w stanie kolizji
																															//[wekotr ten jest proponowan¹ œcie¿k¹ dla postaci która wesz³a w kolizjê]						
};