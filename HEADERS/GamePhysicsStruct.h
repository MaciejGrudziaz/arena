#pragma once
#include <vector>
#include "Vector.h"

//---------------------------------------------------------------------------------------------------------------------------
//------------------------------STRUKTURY U¯YWANE PRZEZ PODSTAWOWY MODEL FIZYKI ['GamePhysics']------------------------------
//---------------------------------------------------------------------------------------------------------------------------

namespace GAMEPHYSICS {
	//struktura opisuj¹ca nastêpn¹ pozycjê postaci po danej kolizji
	struct CollisionVectorCheck {
		Vector nextPos;																									//pozycja postaci po kolizji
		bool collisionOccured;																							//zmienna okreœlaj¹ca czy kolizja zasz³a

		int charIdx;																									//indeks postaci z mapy z któr¹ zasz³a kolizja
		int objIdx;																										//indeks obiektu podstawowego z mapy z którym zasz³a kolizja

		CollisionVectorCheck():nextPos(Vector(0,0,0)),collisionOccured(false),charIdx(-1),objIdx(-1){}
	};

	//struktura 'specjalnej' normalnej [normalna œciany z która zasz³a kolizja + dodatkowe parametry; podstawowa struktura detekcji kolizji]
	struct SpecialNormalStruct {
		Vector normal;																									//normalna œciany z któr¹ zasz³a kolizja
		unsigned linesCrossed;																							//liczba prostych które przeciê³y œcianê podczas testu RayCast
		double dotY;																									//iloczyn skalarny normalnej z wektorem [0, 1, 0] - okreœla typ kolizji

		int charIdx;																									//indeks postaci z mapy z któr¹ zasz³a kolizja
		int objIdx;																										//indeks obiektu podstawowego z mapy z którym zasz³a kolizja

		SpecialNormalStruct() :normal(Vector(0, 0, 0)), linesCrossed(0), dotY(0), charIdx(-1), objIdx(-1) {}
	};

	typedef std::vector<SpecialNormalStruct> specialNormals;															//wektor dla struktur 'specjalnych' normalnych

	//struktura grupuj¹ca wektory 'CollisionVectorCheck' dla ka¿dego typu kolizji
	struct AllCollisionsTypesVec {
		CollisionVectorCheck standardCol;																				//kolizja standardowa [ze œcian¹]
		CollisionVectorCheck negativeCol;																				//kolizja negatywna [ze skoœnym/zwyk³ym sufitem]
		CollisionVectorCheck groundCol;																					//kolizja z obiektem pod³o¿a [kolizja z podstawy postaci z dowolnym obiektem]
		CollisionVectorCheck mapBoundriesCol;																			//kolizja z granicami mapy
	};

	//struktura zwracaj¹ca normaln¹ do œcainy z któr¹ zasz³a kolizja
	struct CollisionRetVal {
		Vector collisionNormal;																							//normalna do œciany kolizji
		bool collisionOccured;																							//zmienna okreœlaj¹ca czy kolizja zasz³a

		CollisionRetVal() :collisionOccured(false), collisionNormal(Vector(0, 0, 0)) {}
	};

	//struktura grupuj¹ca struktury 'CollisionRetVal' dla ka¿dego typu kolizji
	struct CollisionStatus {
		CollisionRetVal standardCol;																					//kolizja standardowa [œciany]
		CollisionRetVal negativeCol;																					//kolizja 'negatywna' [sufit/skos]
		CollisionRetVal groundCol;																						//kolizja z obiektem pod³o¿a [kolizja dla podstawy obiektu postaci]
		CollisionRetVal mapBoundriesCol;																				//kolizja z granicami mapy
	};

	//struktura grupuj¹ca wektory przechowuj¹ce 'specjalne' struktury normalnych kolizji
	struct CollisionVectors {
		specialNormals standardCol;																					
		specialNormals negativeCol;
		specialNormals groundCol;
	};

	//struktura grupuj¹ca struktury 'AllCollisionVectorCheck' [obliczone nowe pozycje postaci dla ka¿dgo typu kolizji] oraz 'CollisionVectors' [czyste dane prosto z detekcji kolizji] 
	struct ColContain_AllColTypeVec {
		AllCollisionsTypesVec allColType;
		CollisionVectors allColVec;
	};
}