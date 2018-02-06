#include "CollisionDetection.h"

//sprawdzanie kolizji pomiêdzy dwoma modelami
//sprawdzenie nastêpuje pomiêdzy g³ówn¹ bry³¹ kolizji modelu pierwszego ['model1'] ,a wszystkimi mo¿liwymi bry³ami kolizji modelu drugiego ['model2']
ModelCollision CollisionDetection::CheckCollision(Character& model1, Character& model2) {
	if (model1.IsEnabled() && model2.IsEnabled()) {																	//sprawdzanie kolizji tylko dla 'odblokowanych' postaci
		CollisionBox colBoxPlayer = model1.getCollisionBox(), colBoxChar = model2.getCollisionBox();				//pobranie bry³ kolizji dla obu modeli
		Vector shift((colBoxPlayer.midPoint + model1.getPosition()) - (colBoxChar.midPoint + model2.getPosition()));//obliczenie przesuniêcia miêdzy œrodkami bry³ kolizji
		double distance = shift.MOD();																				//obliczenie odleg³oœci pomiêdzy œrodkami bry³ kolizji
		std::vector<Vector> colBoxPlayerVertices;																	//wektor przechowuj¹cy punkty dla bry³y kolizji modelu Player
		ModelCollision newCol;																						//obiekt przechowuj¹cy informacje zebrane z detekcji kolizji

		if (distance > (colBoxPlayer.radius + colBoxChar.radius)) {													//sprawdzenie czy sfery kolizji modeli siê przecinaj¹ 
			newCol.status = false;																					//status kolizji = FALSE
			return newCol;
		}

		for (unsigned int i = 0; i < colBoxPlayer.verticesIdx.size(); i++)											//uzupe³nienie wektora przechowuj¹cego punkty dla bry³y kolizji modelu Player
			colBoxPlayerVertices.push_back(model1.getTransformedVertice(colBoxPlayer.verticesIdx[i]));

		if (CollisionDetection_CharChar_CheckMainColBoxCollision(model2, colBoxPlayerVertices)) {					//sprawdzenie czy zachodzi kolizja pomiêdzy g³ównymi hitboxami modeli
			return (CollisionDetection_CharChar_LoadNewCol(model2, colBoxPlayerVertices));							//jeœli tak sprawdŸ z którymi dok³adnie hitboxami modelu 2 zachodzi kolizja
		}
		else {																										//w przeciwnym wypadku
			newCol.status = false;																					//status kolizji = FALSE
			return newCol;
		}
	}
	else return ModelCollision();																					//jeœli postacie nie s¹ odblokowane - zwróæ wart. domyœln¹ kolizji
}

//sprawdzenie kolizji pomiêdzy modelem, a wybranym obiektem z drugiego modelu
//sprawdzenie nastêpuje pomiêdzy g³ówn¹ bry³¹ kolizji modelu pierwszego ['model1'] ,a wszystkimi mo¿liwymi bry³ami kolizji wybranego obiektu modelu drugiego ['model2']
ModelCollision CollisionDetection::CheckCollision(Character& model1, Character& model2, unsigned model2ObjIdx) {
	CollisionBox colBoxMod1=model1.getCollisionBox(), colBoxObj=model2.getObjectMainCollisionBox(model2ObjIdx);	//pobranie bry³ kolizji dla modelu 1 oraz danego obiektu modelu 2
	Vector shift((colBoxMod1.midPoint+model1.getPosition()) - colBoxObj.midPoint);								//obliczenie przesuniêcia miêdzy œrodkami bry³ kolizji
	double distance = shift.MOD();																				//obliczenie odleg³oœci pomiêdzy œrodkami bry³ kolizji
	std::vector<Vector> colBoxMod1Vertices;																		//wektor przechowuj¹cy punkty dla bry³y kolizji modelu Player
	ModelCollision newCol;																						//obiekt przechowuj¹cy informacje zebrane z detekcji kolizji

	if (distance > (colBoxMod1.radius + colBoxObj.radius)) {													//sprawdzenie czy sfery kolizji modeli siê przecinaj¹ 
		newCol.status = false;																					//jeœli tak, status kolizji = FALSE
		return newCol;
	}

	for (unsigned int i = 0; i < colBoxMod1.verticesIdx.size(); i++)
		colBoxMod1Vertices.push_back(model1.getTransformedVertice(colBoxMod1.verticesIdx[i]));					//uzupe³nienie wektora przechowuj¹cego punkty dla bry³y kolizji modelu Player

	if(CollisionDetection_CharObj_CheckMainColBoxCollision(model2,model2ObjIdx,colBoxMod1Vertices)){			//sprawdzenie czy zachodzi kolizja pomiêdzy g³ównymi hitboxami modelu i obiektu
		return ( CollisionDetection_CharObj_LoadNewCol(model2, model2ObjIdx, colBoxMod1Vertices) );				//jeœli tak sprawdŸ z którymi dok³adnie hitboxami obiektu zachodzi kolizja
	}
	else {																										//w przeciwnym wypdaku
		newCol.status = false;																					//status kolizji = FALSE
		return newCol;
	}
}

//pobranie normalnej do œciany obiektu z którym zasz³a kolizja
CollisionNormals CollisionDetection::GetCollisonFaceNormals(Character& model1, Character& model2,ModelCollision col) {
	if (col.status) {																							//jeœli zasz³a kolizja z jakimkolwiek obiektem
		if (col.mainColBox) {																					//jeœli zasz³a kolizja z g³ównym hitboxem modelu/obiektu
			return GetCollisonFaceNormal_MainHitboxCol(model1, model2, col);									//oblicz normaln¹ do œciany kolizji dla g³ównego hitboxa modelu/obiektu
		}
		else {																									//w przeciwnym wypadku
			return GetCollisonFaceNormal_StandardHitboxCol(model1, model2, col);								//oblicz normaln¹ do œciany kolizji dla standardowego hitboxa modelu/obiektu
		}
	}
	else
	{
		CollisionNormals normals;
		return normals;																				//jeœli nie zasz³a kolizja z ¿adnym obiektem zwróæ wektor (0,0,0)
	}
}

//sprawdzenie czy model postaci 'wychodzi' ze stanu kolizji
bool CollisionDetection::GettingOutOfCollision(Character& model, Vector n) {
	Vector p = model.GetNextPosition() - model.getPosition();													//wektor opisuj¹cy 'przysz³e' przesuniêcie postaci gracza

	if (n == Vector(0, 0, 0)) return false;																		//jeœli ¿adne przemiszczenie nie zachodzi zwróæ FALSE

	if (n.DotProduct(p) >= 0.0)																					//jeœli iloczyn skalrny normalnej i wektora przesuniêcia jest wiêkszy od zera
		return true;																							//oznacza to ¿e wektor przesuniêcia jest skierowany w tym samym kierunku co normalna
																												// - postaæ wychodzi z kolizji - zwróæ TRUE 
	else return false;																							//w przeciwnym wypadku zwróæ FALSE
}

//zwrócenie wartoœci wektora dla postaci która znajduje siê w stanie kolizji [wekotr ten jest proponowan¹ œcie¿k¹ dla postaci która wesz³a w kolizjê]
Vector CollisionDetection::GettingIntoCollision(Character& model,Vector n) {
	Vector playerVec = model.GetNextPosition() - model.getPosition();											//wektor opisuj¹cy 'przysz³e' przesuniêcie postaci gracza
	Vector crossVec = playerVec.CrossProduct(n);																//iloczyn wektorowy: crossVec = playerVec x n
	Vector moveVec = n.CrossProduct(crossVec);																	//iloczyn wektorowy: moveVec = n x crossVec
																												//opisuje on wektor wzd³u¿ któego postaæ ma siê przesuwaæ bêd¹c w kolizji z dan¹ œcian¹
	if (moveVec.MOD() == 0)																						//jeœli wektor ten jest zerowy - postaæ jest skierowana prosto w œcianê 
		return Vector(0, 0, 0);
	moveVec /= moveVec.MOD();																					//normalizacja wektora przesuniêcia
	double d = playerVec.MOD();																					//d³ugoœæ wektora 'przysz³ego' przesuniêcia postaci
	if (d == 0)																									//jeœli postaæ nie przemieœci siê - zwróæ wektor zerowy
		return Vector(0, 0, 0);																							
	playerVec /= d;																								//normalizacja wektora przysz³ego przesuniêcia gracza
	double a = playerVec.DotProduct(moveVec);																	//iloczyn skalarny opisuj¹cy w jakim stopniu wektor 'playerVec' jest odchylony
																												//od wektora 'moveVec' - im wartoœæ bli¿sza '1' tym mniejsze odchylenie
	moveVec = moveVec*a*d;																						//obliczenie wektora przesuniêcia dla gracza który wszed³ w kolizjê z dan¹ œcian¹

	return moveVec;
}

//g³ówna funkcja algorytmu GJK
bool CollisionDetection::GJK(std::vector<Vector>&model1, std::vector<Vector>&model2) {
	unsigned GJK_it_limit = static_cast<unsigned>((model1.size()*model2.size())/2.0);												//górna granica iloœci iteracji dla algorytmu GJK
	unsigned int iterations = 0;																				//zmienna przechowuj¹ca aktualn¹ wartoœæ iteracji algorytmu
																												//zabezpiecza przed apêtleniem siê algorytmu
	std::vector<Vector> Simplex;																				//vector opisuj¹cy 'Sympleks' algorytmu GJK - punkty opisujace czworoœcian

	Vector d = GJK_StartVector(model1, model2);																	//obliczenie startowego wektora kierunkowego dla algorytmu GJK

	Simplex.push_back(GJK_SupportFunction(model1, model2, d));													//dodanie pierwszego punktu do Sympleksu przy u¿yciu 'Support function' dla GJK

	d = d.Negate();																								//zmienienie zwrotu wektora kierunkowego

	while (true) {
		if (iterations > GJK_it_limit)																			//jeœli algorytm siê zapêtli³
			return true;																						//przerwij i zwróæ wartoœæ TRUE - kolizja zachodzi
																												//[jeœli kolizja by nie zachodzi³a, zosta³oby to du¿o wczeœniej wykryte]
			
			Simplex.push_back(GJK_SupportFunction(model1, model2, d));											//dodanie kolejnego punktu do Sympleksu

			if (Simplex[Simplex.size() - 1].DotProduct(d) < 0) {												//jeœli dodany punkt nie przeszed³ przez œrodek uk³adu
				return false;																					//kolizja nie zachodzi - zwróæ FALSE
			}
			else {																								//w przeciwnym wypadku
				if (GJK_SimplexContainsORIGIN(Simplex)) {														//sprawdŸ czy punkt (0,0,0) jest zawartty w Sympleksie
					return true;																				//jeœli tak - kolizja zachodzi, zwróæ TRUE
				}
				else 																							//w przeciwnym wypadku
					d = GJK_GetDirection(Simplex);																//oblicz now¹ wartoœæ wektora kierunkowego
			}
		iterations++;																							//zwiêksz licznik iteracji algorytmu
	}
}

//wyliczanie pocz¹tkowego wektora kierunkowego
Vector CollisionDetection::GJK_StartVector(std::vector<Vector>&model1, std::vector<Vector>&model2) {
	//obliczenie punktów œrodkowych dla obu modeli przekazanych jako wektory punktów
	Vector midPoint1(0, 0, 0);
	Vector midPoint2(0, 0, 0);	
	if (model1.size() >= model2.size()) {											
		for (unsigned int i = 0; i < model1.size(); i++) {
			if (i < model2.size())
				midPoint2 += model2[i];
			midPoint1 += model1[i];
		}
	}
	else {
		for (unsigned int i = 0; i < model2.size(); i++) {
			if (i < model1.size())
				midPoint1 += model1[i];
			midPoint2 += model2[i];
		}
	}

	midPoint1 /= model1.size();
	midPoint2 /= model2.size();

	return (midPoint2 - midPoint1);																				//zwrócenie wektora przesuniêcia miedzy œrodkami modelu 1 ,a modelu 2
}

//'Support function' dla GJK zwracaj¹ca kolejny punkt do Sympleksu ze wzglêdu na wektor 'd'
Vector CollisionDetection::GJK_SupportFunction(std::vector<Vector>&model1, std::vector<Vector>&model2, Vector d) {
	//-------------------------------------------------------------------------------------------------OPIS----------------------------------------------------------------------------------------------
	//algorytm 'Support function' znajduje punkt w bryle 'model1' najbardziej oddalony w kierunku 'd' oraz punkt w bryle 'model2' najbardziej oddalony w kierunku '-d'
	//nastêpnie wyznacza punkt do Sympleksu poprzez ró¿nicê miêdzy tymi dwoma punktami [wyznaczony punkt jest najbardziej oddalonym punktem w bryle ró¿nicy Minkowskiego dla modeli 1 i 2 w kierunku 'd']
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	Vector _d = d.Negate();																						//stworzenie wektora o przeciwnym zwrocie do wektora kierunkowego d
	double dotProductResult1 = model1[0].DotProduct(d), dotProductResult2 = model2[0].DotProduct(_d);			//zainicjalizowanie wartoœci dla przeszukiwania zbioru punktów
	unsigned int idx1 = 0, idx2 = 0;

	for (unsigned int i = 1; i < model1.size(); i++) {
		if (model1[i].DotProduct(d) > dotProductResult1) {														//jeœli iloczyn skalarny danego punktu z wek. 'd' jest wiêkszy od wczeœniejszego max
			idx1 = i;																							//to znaczy ¿e dany punkt jest bardziej oddalony w kierunku 'd' ni¿ wczeœniejsz
			dotProductResult1 = model1[i].DotProduct(d);														//zaktualizuj wart. dla najbardziej oddalonego punktu
		}
	}

	for (unsigned int i = 1; i < model2.size(); i++) {															
		if (model2[i].DotProduct(_d) > dotProductResult2) {														//jeœli iloczyn skalarny danego punktu z wek. '-d' jest wiêkszy od wczeœniejszego max
			idx2 = i;																							//to znaczy ¿e dany punkt jest bardziej oddalony w kierunku '-d' ni¿ wczeœniejszy				
			dotProductResult2 = model2[i].DotProduct(_d);														//zaktualizuj wart. dla najbardziej oddalonego punktu
		}
	}

	return (model1[idx1] - model2[idx2]);																		//wyznacz punkt bêd¹cy ró¿nic¹ Minkowskiego dla znalezionych punktów dla modeli 1 i 2
}

//sprawdzanie czy Sympleks zawiera punkt (0,0,0)
bool CollisionDetection::GJK_SimplexContainsORIGIN(std::vector<Vector>& Simplex) {
	if (Simplex.size() < 4)																						//jeœli wielkoœæ Sympleksu jest mniejsz ni¿ 4
		return false;																							//Sympleks nie jest bry³a zwróæ FALSE
	else {
		Vector n[4];																							//normalne dla œcian Symplesku - skierowane na zewn¹trz bry³y

		GJK_SimplexContainsORIGIN_CreateNormals(Simplex, n);													//za³adowanie normalnych 

		double Surface[4][4];																					//tablica opisuj¹ca powierzchnie Sympleksu 

		GJK_SimplexContainsORIGIN_CreateSurfaces(Simplex, n, Surface);											//za³adowanie wartoœci tablicy 'Surface' - obliczenie wspó³æzynników powierzchni
		//------------------------------------------------------------------------ALGORYTM-------------------------------------------------------------------------
		//jeœli parametr 't' dla prostej parametrycznej zaczynaj¹cej siê w punkcie (0,0,0) i skierowanje w kierunku normalnej dla danej œciany jest mniejszy od '0'
		//punkt (0,0,0) nie znajduje siê wewn¹trz Sympleksu - zwróæ FALSE
		//jeœli dla ka¿dej normalnej paramtr 't' prostej jest wiêkszy od '0' - punkt (0,0,0)  znajduje siê wewn¹trz Sympleksu - zwróæ TRUE
		//---------------------------------------------------------------------------------------------------------------------------------------------------------
		//zapis skrócony powy¿szego algorytmu po wyprowadzeniu konkretnych warunków dla znaku parametru 't'
		
		double t_mian[4], t_licz[4];
		for (unsigned i = 0; i < 4; i++) {																		//obliczenie wart. dla licznika i mianownika param. 't' dla ka¿dej œciany
			t_licz[i] = Surface[i][3];
			t_mian[i] = Surface[i][0] * n[i].x + Surface[i][1] * n[i].y + Surface[i][2] * n[i].z;
		}
		
		if ((t_licz[0] < 0 && t_mian[0] < 0) || (t_licz[0] > 0 && t_mian[0] > 0))								//sprawdzenie warunków czy 't < 0' dla ka¿dej œciany
			return false;
		if ((t_licz[1] < 0 && t_mian[1] < 0) || (t_licz[1] > 0 && t_mian[1] > 0))
			return false;
		if ((t_licz[2] < 0 && t_mian[2] < 0) || (t_licz[2] > 0 && t_mian[2] > 0))
			return false;
		if ((t_licz[3] < 0 && t_mian[3] < 0) || (t_licz[3] > 0 && t_mian[3] > 0))
			return false;

		return true;																							//jeœli dla ka¿dej œciany parametr 't' ma wartoœæ dodatni¹
																												//punkt (0,0,0) jest wewn¹trz Sympleksu - zwróæ 'TRUE'
	}
}

//funkcja pomocnicza - budowanie normalnych dla Sympleksa
void CollisionDetection::GJK_SimplexContainsORIGIN_CreateNormals(std::vector<Vector>& Simplex,Vector n[]) {
	//obliczenie normalnych do ka¿dej œciany Sympleksu
	Vector tmp1(Simplex[3] - Simplex[2]), tmp2(Simplex[1] - Simplex[2]);
	n[0] = tmp1.CrossProduct(tmp2);
	tmp2 = Simplex[0] - Simplex[2];
	n[1] = tmp1.CrossProduct(tmp2);
	tmp1 = Simplex[3] - Simplex[1]; tmp2 = Simplex[0] - Simplex[1];
	n[2] = tmp1.CrossProduct(tmp2);
	tmp1 = Simplex[2] - Simplex[1];
	n[3] = tmp1.CrossProduct(tmp2);

	//sprawdzenie czy ka¿da normalna wskazuje na zewn¹trz bry³y
	if (n[0].DotProduct(Simplex[0]) > n[0].DotProduct(Simplex[3]))
		n[0] = n[0].Negate();
	if (n[1].DotProduct(Simplex[1]) > n[1].DotProduct(Simplex[3]))
		n[1] = n[1].Negate();
	if (n[2].DotProduct(Simplex[2]) > n[2].DotProduct(Simplex[3]))
		n[2] = n[2].Negate();
	if (n[3].DotProduct(Simplex[3]) > n[3].DotProduct(Simplex[2]))
		n[3] = n[3].Negate();
}

//funkcja pomocnicza - budowanie powierzchni dla Sympleksa
void CollisionDetection::GJK_SimplexContainsORIGIN_CreateSurfaces(std::vector<Vector>& Simplex, Vector n[],double Surface[][4]) {
	//przypisanie wspó³rzêdnych normalnych jako pierwszych 3 parametrów dla ka¿dej powierzchni (A, B, C)
	for (unsigned int i = 0; i < 3; i++) {
		Surface[0][i] = n[0][i];
		Surface[1][i] = n[1][i];
		Surface[2][i] = n[2][i];
		Surface[3][i] = n[3][i];
	}

	//obliczenie parametru D dla ka¿dej powierzchni
	Surface[0][3] = -Surface[0][0] * Simplex[3].x - Surface[0][1] * Simplex[3].y - Surface[0][2] * Simplex[3].z;
	Surface[1][3] = -Surface[1][0] * Simplex[3].x - Surface[1][1] * Simplex[3].y - Surface[1][2] * Simplex[3].z;
	Surface[2][3] = -Surface[2][0] * Simplex[3].x - Surface[2][1] * Simplex[3].y - Surface[2][2] * Simplex[3].z;
	Surface[3][3] = -Surface[3][0] * Simplex[2].x - Surface[3][1] * Simplex[2].y - Surface[3][2] * Simplex[2].z;
}

//funkcja wyznaczaj¹ca wektor kierunkowy w kolejnych iteracjach algorytmu
Vector CollisionDetection::GJK_GetDirection(std::vector<Vector>&Simplex) {
	switch (Simplex.size()) {									
	case 2:																										//dla Sympleksu o wielkoœci 2
		return GJK_GetDirection_Iteration_1(Simplex);															//pierwsza iteracja algorytmu GJK
		break;
	case 3:																										//dla Sympleksu o weilkoœci 3
		return GJK_GetDirection_Iteration_2(Simplex);															//druga iteracja algorytmu GJK
		break;
	case 4:																										//dla Sympleksu o wielkoœci 4
		return GJK_GetDirection_Iteration_3(Simplex);															//trzecia iteracja algorytmu GJK
		break;
	default:
		return Vector(0.0, 0.0, 0.0);																			//dla ka¿dej innej wartoœci Sympleksu zwróæ wektor zerowy - ochrona przed b³êdami
		break;
	}
}

//Iteracja 1 [2 punkty w Sympleksie]
Vector CollisionDetection::GJK_GetDirection_Iteration_1(std::vector<Vector>&Simplex) {
	//punkt A - ostatni punkt dodany do Sympleksu, B - pierwszy punkt dodany do Sympleksu,  d - wyznaczany wektor kierunkowy
	Vector d, ORIGIN(0, 0, 0), AB, A0;

	//obliczenie wektora 'd' na podstawie danych punktów w Sympleksie, oraz wiedzy ¿e punkt A znajduje siê po 'drugiej' stronie uk³adu wspó³rzêdnych ni¿ punkt B
	AB = Simplex[0] - Simplex[1];																				
	A0 = ORIGIN - Simplex[1];
	d = AB.CrossProduct(A0);
	d = d.CrossProduct(AB);																						
	return d;
}

//Iteracja 2 [3 punkty w Sympleksie]
Vector CollisionDetection::GJK_GetDirection_Iteration_2(std::vector<Vector>&Simplex) {
	//punkt A - ostatni dodany punkt do Sympleksa, B - punkt dodany przed A, C - piewrszy punkt w Sympleksie
	Vector d, ORIGIN(0,0,0), AB, AC, A0;
	AB = Simplex[1] - Simplex[2]; AC = Simplex[0] - Simplex[2];													//inicjalizacja wektorów
	A0 = ORIGIN - Simplex[2];

	//ustalenie nowej wartoœci wektora kierunkowego
	d = GJK_GetDirection_Iteration_2_AB(Simplex, AB, AC, A0);													//normlana do odcinka AB
	if (d!=ORIGIN) 
		return d;
	else {
		d = GJK_GetDirection_Iteration_2_AC(Simplex, AB, AC, A0);												//normalna do odcinka AC
		if (d != ORIGIN)
			return d;
		else {
			d = GJK_GetDirection_Iteration_2_CBA(AB, AC, A0);													//normalna do powierzchni CBA
			if (d!=ORIGIN)
				return d;
			else {
				d = GJK_GetDirection_Iteration_2_ABC(AB, AC,A0);												//normalna do poweirzchni ABC
				return d;
			}
		}
	}
}

//sprawdzenie czy wektor normalny do odcinka AB jest skierowany do punktu (0,0,0)
Vector CollisionDetection::GJK_GetDirection_Iteration_2_AB(std::vector<Vector>&Simplex,Vector AB, Vector AC, Vector A0) {
	Vector d;
	d = AC.CrossProduct(AB);
	d = d.CrossProduct(AB);																						//wyznaczenie wektora normalnego do odcinka AB

	if (d.DotProduct(A0) > 0) {																					//sprawdzenie czy wektor 'd' jest skierowany do œrodka uk³adu
		Simplex.erase(Simplex.begin());																			//jesli tak usuñ pierwszy punkt z Sympleksu [C]
		return d;																								//zwróæ now¹ wartoœæ wektora kierunkowego
	}

	return Vector(0, 0, 0);																						//w przeciwnym wypadku zwróæ wektor zerowy
}

//sprawdzenie czy wektor normalny do odcinka AC jest skierowany do punktu (0,0,0)
Vector CollisionDetection::GJK_GetDirection_Iteration_2_AC(std::vector<Vector>&Simplex,Vector AB, Vector AC, Vector A0) {
	Vector d;
	d = AB.CrossProduct(AC);
	d = d.CrossProduct(AC);																						//wyznaczenie wektora normlanego do odcinka AC

	if (d.DotProduct(A0) > 0) {																					//sprawdzenie czy wektor 'd' jest skierowany do œrodka uk³adu
		Simplex.erase(Simplex.begin() + 1);																		//jesli tak usuñ drugi element z Sympleksu [B]
		return d;																								//zwróæ now¹ wartoœæ wektora kierunkowego
	}

	return Vector(0, 0, 0);																						//w przeciwnym wypadku zwróæ wektor zerowy
}

//sprawdzenie czy wektor normalny do powierzchni CBA jest skierowany do punktu (0,0,0)
Vector CollisionDetection::GJK_GetDirection_Iteration_2_CBA(Vector AB,Vector AC,Vector A0) {
	Vector d;
	d = AC.CrossProduct(AB);																					//wyznaczenie wektora normalnego do poweirzchni CBA

	if (d.DotProduct(A0) > 0)																					//jeœli wektor 'd' jest skierowny do œrodka uk³adu
		return d;																								//zachowaj wszystkie elementy Sympleksu i zwróæ now¹ wartoœæ wektora kierunkowego

	return Vector(0, 0, 0);																						//w przeciwnym wypadku zwróæ wektor zerowy
}

//sprawdzenie czy wektor normalny do powierzchni ABC jest skierowany do punktu (0,0,0)
Vector CollisionDetection::GJK_GetDirection_Iteration_2_ABC(Vector AB,Vector AC,Vector A0) {
	Vector d;																									
	d = AB.CrossProduct(AC);																					//wyznaczenie wektora normalnego do powierzchni ABC

	return d;																									//zwróæ wektor 'd' jako now¹ wartoœæ wektora kierunkowego
																												//ostatni warunek w 2 iteracji algorytmu - nie przeprowadza siê ¿adnych dodatkowych testów
}

//Iteracja 3 [4 punkty w Sympleksie]
Vector CollisionDetection::GJK_GetDirection_Iteration_3(std::vector<Vector>&Simplex) {
	Vector d, ORIGIN(0,0,0), AB, AC, AD, A0;
	AB = Simplex[2] - Simplex[3]; AC = Simplex[1] - Simplex[3];
	AD = Simplex[0] - Simplex[3]; A0 = ORIGIN - Simplex[3];

	//ustalenie nowej wartoœci wektora kierunkowego
	d = GJK_GetDirection_Iteration_3_AB(Simplex, AB, AC, A0);													//normalna do odcinka AB									
	if (d != ORIGIN)
		return d;
	else {
		d = GJK_GetDirection_Iteration_3_AC(Simplex, AB, AC, A0);												//normalna do odcinka AC
		if (d != ORIGIN)
			return d;
		else {
			d = GJK_GetDirection_Iteration_3_AD(Simplex, AC, AD, A0);											//normalna do odcinka AD
			if (d != ORIGIN)
				return d;
			else {
				d = GJK_GetDirection_Iteration_3_ABC(Simplex, AB, AC, A0);										//normalna do p³aszczyzny ABC
				if (d != ORIGIN)
					return d;
				else {
					d = GJK_GetDirection_Iteration_3_ABD(Simplex, AB, AD, A0);									//normalna do p³aszczyzny ABD
					if (d != ORIGIN)
						return d;
					else {
						d = GJK_GetDirection_Iteration_3_ACD(Simplex, AD, AC,A0);									//normalna do p³aszczyzny ACD
							return d;
					}
				}
			}
		}
	}
}

//sprawdzenie czy wektor normalny do odcinka AB jest skierowany do punktu (0,0,0)
Vector CollisionDetection::GJK_GetDirection_Iteration_3_AB(std::vector<Vector>&Simplex,Vector AB,Vector AC,Vector A0) {
	Vector d;
	d = AC.CrossProduct(AB);
	d = d.CrossProduct(AB);																						//wyznaczenie wektora normlanego do odcinka AB

	if (d.DotProduct(A0) > 0) {																					//jeœli wektor 'd' jest skierowany do punktu (0,0,0)
		Simplex.erase(Simplex.begin());																			//usuñ dwa pierwsze elementy z Sympleksu (C i D)
		Simplex.erase(Simplex.begin());
		return d;																								//zwróæ now¹ wartoœæ wektora kierunkowego 
	}

	return Vector(0, 0, 0);																						//w przeciwnym wypadku zwróæ wektor zerowy
}

//sprawdzenie czy wektor normalny do odcinka AC jest skierowany do punktu (0,0,0)
Vector CollisionDetection::GJK_GetDirection_Iteration_3_AC(std::vector<Vector>&Simplex,Vector AB,Vector AC,Vector A0) {
	Vector d;
	d = AB.CrossProduct(AC);
	d = d.CrossProduct(AC);																						//wyznaczenie wektora normalnego do odcinka AC

	if (d.DotProduct(A0) > 0) {																					//jeœli wektor 'd' jest skierowany do punktu (0,0,0)
		Simplex.erase(Simplex.begin());																			//usuñ pierwszy i trzeci punkt z Sympleksu (D i B)
		Simplex.erase(Simplex.begin()+1);
		return d;																								//zwróæ now¹ wartoœæ wektora kierunkowego
	}

	return Vector(0, 0, 0);																						//w przeciwnym wypadku zwróæ wektor zerowy
}

//sprawdzenie czy wektor normalny do odcinka AD jest skierowany do punktu (0,0,0)
Vector CollisionDetection::GJK_GetDirection_Iteration_3_AD(std::vector<Vector>&Simplex,Vector AC,Vector AD, Vector A0) {
	Vector d;
	d = AC.CrossProduct(AD);
	d = d.CrossProduct(AD);																						//wyznaczenie wektora normalnego do odcinka AD

	if (d.DotProduct(A0) > 0) {																					//jeœli wektor 'd' jest skierowany do punktu (0,0,0)
		Simplex.erase(Simplex.begin() + 1);																		//usuñ drugi i trzeci punkt z Sympleksu (B i C)
		Simplex.erase(Simplex.begin() + 1);
		return d;																								//zwróæ now¹ wartoœæ wektora kierunkowego
	}

	return Vector(0, 0, 0);																						//w przeciwnym wypadku zwróæ wektor zerowy
}

//sprawdzenie czy wektor normalny do poweirzchni ABC jest skierowany do punktu (0,0,0)
Vector CollisionDetection::GJK_GetDirection_Iteration_3_ABC(std::vector<Vector>&Simplex,Vector AB,Vector AC,Vector A0) {
	Vector d;
	d = AC.CrossProduct(AB);																					//wyznaczenie wektora normalnego do powierzchni  ABC 

	if (d.DotProduct(Simplex[0]) > d.DotProduct(Simplex[3]))													//sprawdzenie czy normalna jest skierowana na zewn¹trz Sympleksu
		d = d.Negate();																							

	if (d.DotProduct(A0) > 0) {																					//jeœli wektor 'd' jest skierowany do punktu (0,0,0)
		Simplex.erase(Simplex.begin());																			//usuñ pierwszy punkt z Sympleksu (D)
		return d;																								//zwróæ now¹ wartoœæ wektora kierunkowego
	}

	return Vector(0, 0, 0);																						//w przeciwnym wypadku zwróæ wektor zerowy
}

//sprawdzenie czy wektor normalny do powierzchni ABD jest skierowany do (0,0,0)
Vector CollisionDetection::GJK_GetDirection_Iteration_3_ABD(std::vector<Vector>&Simplex,Vector AB,Vector AD,Vector A0) {
	Vector d;
	d = AD.CrossProduct(AB);																					//wyznaczenie wektora normalnego do poweirzchni ABD
	
	if (d.DotProduct(Simplex[1]) > d.DotProduct(Simplex[3]))													//sprawdzenie czy normalna jest skierowana na zewn¹trz Sympleksu
		d = d.Negate();

	if (d.DotProduct(A0) > 0) {																					//jeœli wektor 'd' jest skierowany do punktu (0,0,0) 
		Simplex.erase(Simplex.begin() + 1);																		//usuñ drugi element z Sympleksu (C)
		return d;																								//zwróæ now¹ wartoœæ wektora kierunkowego
	}

	return Vector(0, 0, 0);																						//w przeciwnym wypadku zwróæ wektor zerowy
}

//sprawdzenie czy wektor normalny do powierzchni ACD jest skierowany do (0,0,0)
Vector CollisionDetection::GJK_GetDirection_Iteration_3_ACD(std::vector<Vector>&Simplex,Vector AD,Vector AC, Vector A0) {
	Vector d;
	d = AD.CrossProduct(AC);																					//wyznaczenie wektora normalnego do poweirzchni ACD

	if (d.DotProduct(Simplex[2]) > d.DotProduct(Simplex[3]))													//sprawdzenie czy normalna jest skierowana na zewn¹trz Sympleksu
		d = d.Negate();

	Simplex.erase(Simplex.begin() + 2);																			//usuñ 3 element z Sympleksu
	return d;																									//zwróæ now¹ wartoœæ wektora kierunkowego
																												//ostatni warunek 3 iteracji algorytmu - nie jest sprawdzany ¿aden dodatkowy warunek
}

//sprawdzenie czy zasz³a kolizja z g³ównym hitboxem badanego modelu
bool CollisionDetection::CollisionDetection_CharChar_CheckMainColBoxCollision(Character& model2, std::vector<Vector>& colBoxPlayerVertices) {
	CollisionBox colBoxChar = model2.getCollisionBox();															//za³adowanie g³ównego hitboxa dla modelu 2											
	std::vector<Vector> colBoxCharVertices;																		//wektora przechowuj¹cy wierzcho³ki wchodz¹ce w sk³ad hitboxa modelu 2
	for (unsigned int i = 0; i < colBoxChar.verticesIdx.size(); i++)
		colBoxCharVertices.push_back(model2.getTransformedVertice(colBoxChar.verticesIdx[i]));					//za³adowanie wierzcho³ków hitboxa do wektora

	if (GJK(colBoxPlayerVertices, colBoxCharVertices)) return true;												//sprawdzenie algorytmem GJK czy zachodzi kolizja pomiêdzy g³ównymi hitboxami
	else return false;
}

//za³adowanie wartoœci dla obiektu detekcji kolizji 'ModelCollision'
ModelCollision CollisionDetection::CollisionDetection_CharChar_LoadNewCol(Character& model2, std::vector<Vector>& colBoxPlayerVertices) {
	CollisionBox colBoxChar;																					//hitbox dla modelu 2
	std::vector<Vector> colBoxCharVertices;																		//wektora przechowuj¹cy wierzcho³ki dla hitboxa modelu 2
	ModelCollision newCol;																						//obiekt przechowuj¹cy informacje zebrane z detekcji kolizji
	unsigned sum = 0;																							//suma pomocnicza, okreœlaj¹ca ile hitboxów wchodzi w sk³ad modelu [oprócz g³ównego]

	for(unsigned int i=0;i<model2.getObjectsCount();i++)
		sum += model2.getObjectCollisionBoxCount(i);															//obliczenie sumy hitboxów modelu 2
	if (sum == 0) {																								//jeœli w sk³¹d modelu 2 nie wchodzi ¿aden inny hitbox poza g³ównym
		newCol.status = true; newCol.mainColBox = true; newCol.mainColBoxObjIdx = -1;							//ustaw odpowiednie statusy kolizji
		return newCol;
	}

	for (unsigned int i = 0; i < model2.getObjectsCount(); i++) {												//dla ka¿dego obiektu z modelu Character
		for (unsigned int j = 0; j < model2.getObjectCollisionBoxCount(i); j++) {
			colBoxChar = model2.getObjectCollisionBox(i, j);													//pobranie bry³y kolizji 'j' dla obiektu 'i'
			colBoxCharVertices.clear();
			for (unsigned int k = 0; k < colBoxChar.verticesIdx.size(); k++)									//uzupe³nienie wektora przechowuj¹cego punkty dla bry³y kolizji modelu Character
				colBoxCharVertices.push_back(model2.getTransformedVertice(colBoxChar.verticesIdx[k]));

			if (GJK(colBoxPlayerVertices, colBoxCharVertices)) {												//sprawdzenie czy zachodzi kolizja
				std::pair<unsigned int, unsigned int> objIdx_ColboxIdx;											//jeœli tak, dodaj indeks obiektu oraz danej bry³y kolizji do wektorów
				objIdx_ColboxIdx.first = i;
				objIdx_ColboxIdx.second = j;
				newCol.colIdx.push_back(objIdx_ColboxIdx);														//dodaj indeks obiektu oraz indeks bry³y kolizji obektu 'ModelCollision'
			}
		}
	}
	if (newCol.colIdx.size() > 0) {																				//jeœli zasz³a kolizja z conajmniej jednym hitboxem modelu
		newCol.status = true; newCol.mainColBox = false;														//status kolizji = TRUE, status kolizji z g³ówn¹ym hitboxem = FALSE
	}
	else newCol.status = false;																					//w przeciwnym wypadku, statu kolizji = FALSE
	return newCol;
}

//sprawdzenie czy zasz³a kolizja z g³ównym hitboxem badanego obiektu
bool CollisionDetection::CollisionDetection_CharObj_CheckMainColBoxCollision(Character& model2, unsigned model2ObjIdx, std::vector<Vector>& colBoxMod1Vertices) {
	CollisionBox colBoxObj=model2.getObjectMainCollisionBox(model2ObjIdx);										//za³adowanie g³ównego hitboxa dla obiektu z modelu 2
	std::vector<Vector> colBoxObjVertices;																		//wektor przechowuj¹cy wierzcho³ki wchodz¹ce w sk³ad hitboxa obiektu

	for (unsigned int i = 0; i < colBoxObj.verticesIdx.size(); i++)
		colBoxObjVertices.push_back(model2.getTransformedVertice(colBoxObj.verticesIdx[i]));					//za³¹dowanie wierzcho³ków hitboxa do wektora

	if (GJK(colBoxMod1Vertices, colBoxObjVertices)) return true;												//sprawdzenie algorytmem GJK czy zachodzi kolizja pomiêdzy g³ównymi hitboxami
	else return false;
}

//za³adownie wartoœci dla obiektu detekcji kolizji 'ModelCollision'
ModelCollision CollisionDetection::CollisionDetection_CharObj_LoadNewCol(Character& model2, unsigned model2ObjIdx, std::vector<Vector>& colBoxMod1Vertices) {
	CollisionBox colBoxObj;																						//hitbox dla obiektu
	std::vector<Vector> colBoxObjVertices;																		//wektor przechowuj¹cy wierzcho³ki wchodz¹ce w sk³ad hitboxa obiektu
	ModelCollision newCol;																						//obiekt przechowuj¹cy informacje zebrane z detekcji kolizji

	if (model2.getObjectCollisionBoxCount(model2ObjIdx) == 0) {													//jeœli obiekt nie posiada ¿adnego hitboxa [oprócz g³ównego]
		newCol.status = true; newCol.mainColBox = true; newCol.mainColBoxObjIdx = model2ObjIdx;					//ustaw odpowiednie statusy kolizji
		return newCol;
	}
																	
	for (unsigned int i = 0; i < model2.getObjectCollisionBoxCount(model2ObjIdx); i++) {						//dla ka¿dego hitboxa obiektu
		colBoxObj = model2.getObjectCollisionBox(model2ObjIdx, i);												//za³aduj nowego hitboxa obiektu
		colBoxObjVertices.clear();																				//wyczyœæ wektor wierzcho³ków hitboxa
		for (unsigned int j = 0; j < colBoxObj.verticesIdx.size(); j++)
			colBoxObjVertices.push_back(model2.getTransformedVertice(colBoxObj.verticesIdx[j]));				//za³aduj nowe wartoœci wierzcho³ków hitboxa do wektora

		if (GJK(colBoxMod1Vertices, colBoxObjVertices)) {														//sprawdzenie czy zachodzi kolizja
			std::pair<unsigned int, unsigned int> objIdx_ColboxIdx;												//jeœli tak, dodaj indeks obiektu oraz danej bry³y kolizji do wektorów
			objIdx_ColboxIdx.first = model2ObjIdx;
			objIdx_ColboxIdx.second = i;
			newCol.colIdx.push_back(objIdx_ColboxIdx);															
		}
	}

	if (newCol.colIdx.size() > 0) {																				//jeœli zasz³a kolizja z conajmniej jednym hitboxem obiektu
		newCol.status = true; newCol.mainColBox = false;														//ustaw status kolizji = TRUE, a status kolizji z g³ównym hitboxem = FALSE
	}
	else newCol.status = false;																					//w przeciwnym wypadku status kolizji = FALSE
	return newCol;
}

//pobranie normalnej do powierzchni kolizji dla standardowego hitboxa
CollisionNormals CollisionDetection::GetCollisonFaceNormal_StandardHitboxCol(Character& model1,Character& model2,ModelCollision& col) {
	CollisionNormals normals;																					//wektor przechowuj¹cy inf. z algorytmu RayCast dla danych wektorów normalnych
	std::pair<Vector,unsigned> normal;

	for (unsigned int i = 0; i < col.colIdx.size(); i++) {														//dla wszystkich obiektów z któymi zasz³a kolizja												
		CollisionBox colBoxChar = model2.getObjectCollisionBox(col.colIdx[i].first,
			col.colIdx[i].second);																				//pobierz bry³ê kolizji dla obiektu z którym zasz³a kolizja

		normal = GetCollisionFaceNormal_GetNormal(model1, model2, colBoxChar);
		if(normal.first!=Vector(0,0,0))
			normals.push_back(normal);																			//dodanie nowej wartoœci do wektora przechowuj¹cego inf. z alg. RayCast
	
	}

	return normals;
}

//pobranie normalnej do powierzchni kolizji dla g³ównego hitboxa
CollisionNormals CollisionDetection::GetCollisonFaceNormal_MainHitboxCol(Character& model1, Character& model2, ModelCollision& col) {
	CollisionBox colBoxChar;																					//g³ówny hitbox dla modelu/obiektu
	std::pair<Vector, unsigned int> normal;																		//obiekt przechowuj¹cy inf. z algorytmu RayCast dla danego wektora normalnego
	CollisionNormals normalOut;

	if (col.mainColBoxObjIdx == -1)																				//jeœli badanym g³ównym hitboxem jest hitbox modelu
		colBoxChar = model2.getCollisionBox();																	//za³aduj g³ówny hitbox modelu
	else colBoxChar = model2.getObjectMainCollisionBox(col.mainColBoxObjIdx);									//w przeciwnym wypadku za³aduj g³ówny hitbox obiektu danego indeksem 'col.mainColBoxObjIdx'

	normal=GetCollisionFaceNormal_GetNormal(model1, model2, colBoxChar);										//pobranie wartoœci noramlnej przez algorytm 'RayCast'

	if (normal.second > 0 && normal.first != Vector(0, 0, 0))
		normalOut.push_back(normal);

	return normalOut;
}

//funkcja pomocnicza sprawdzaj¹ca czy dany punkt znajduje siê wewn¹trz trójk¹ta
bool CollisionDetection::CheckPointInTriangle(Vector point, Vector triangle[], Vector normal) {					//trójk¹t opisany jest przy pomocy 3 kolejno podanych punktów ['triangle'] oraz normalnej
	Vector n[3], AB, BC, CA;																					
	AB = triangle[1] - triangle[0]; BC = triangle[2] - triangle[1]; CA = triangle[0] - triangle[2];				//inicjalizacja wektorów opisuj¹cych kolejne ramiona trójk¹ta
	n[0] = AB.CrossProduct(normal);																				//obliczenie normalnych do ka¿dego boku trójk¹ta (wskazuj¹ one na zewn¹trz trójk¹ta)
	n[1] = BC.CrossProduct(normal);																			
	n[2] = CA.CrossProduct(normal);

	for (unsigned int i = 0; i < 3; i++) {																		//dla ka¿dego boku
		double A, B, C, D;															
		A = n[i].x; B = n[i].y; C = n[i].z;																		//wyzanczamy parametry powierzchni opisuj¹cej dany bok	
		D = -A*triangle[i].x - B*triangle[i].y - C*triangle[i].z;												//[przy pomocy normalnej do danego boku oraz punktów wchodz¹cych w jego sk³ad]

		double t = -D - A*point.x - B*point.y - C*point.z;														//obliczenie parametru 't' dla parametrycznego równania prostej	
		t /= A*n[i].x + B*n[i].y + C*n[i].z;																	//zaczynaj¹cej siê w punkcie 'point' i skierowanej zgodnie z wektorem normalnym

		if (t < 0) return false;																				//jeœli wartoœc ta jest ujemna - punkt nie zanjduje siê wewn¹trz trójk¹ta - zwróæ FALSE
	}

	return true;																								//jeœli wszystkie wartoœci parametru 't' by³y dodatnie - punkt znajduje siê wewn¹trz trójk¹ta
}

//pobranie normalnej do powierzchni obiektu z któr¹ zasz³a kolizja
//[argumenty: 'model2' - obiekt z którym zasz³a kolizja, 'move' - wekotr przesuniêcia postaci, 'point' - wektor wyznaczonych punktów z=startowych dla 'Ray cast', 'colBoxChar' - bry³a kolziji obiektu 'model2']
std::pair<Vector, unsigned> CollisionDetection::GetCollisionFaceNormal_GetNormal(Character &model1, Character& model2, CollisionBox &colBoxChar) {
	Vector nRet,point[RayCastLinesNum];
	unsigned int linesCrossed = 0, checkCrossed = 0;
	double R= model1.getCollisionBox().radius, surface[4];
																		
	GetCollisionFaceNormal_LoadPoints(point, model1);															//za³adowanie tablicy 'point' losowymi punktami startowymi dla prostych w algorytmie 'RayCast'

	for (unsigned int i = 0; i < colBoxChar.facesIdx.size(); i++) {
		Vector newN = model2.getTransformedNormal(model2.getFace(colBoxChar.facesIdx[i]).NormalIdx);			//pobranie aktulanej wartoœci normalnej do danej œciany																						//zresetowanie wartoœci zmiennej pomocniczej
		Vector faceP[3];
		for (unsigned int j = 0; j < 3; j++)
			faceP[j] = model2.getTransformedVertice(model2.getFace(colBoxChar.facesIdx[i]).PointsIdx[j]);		//pobranie punktów wchodz¹cych w sk³ad danej œciany

		surface[0] = newN.x; surface[1] = newN.y; surface[2] = newN.z;
		surface[3] = -surface[0]*faceP[0].x - surface[1]*faceP[0].y - surface[2]*faceP[0].z;					//wyznaczenie parametrów powierzchni opisanej przez dan¹ œcianê
				
		checkCrossed=GetCollisionFaceNormal_GetNormal_RayCast(faceP, point, newN, surface,R);					//algorytm 'RayCast' - wyznaczenie ile losowo wybranych prostych przechodzi przez dan¹ pow.

		if(checkCrossed!=-1)																					//jeœli algorytm zakoñczy³ siê sukcesem
			if (checkCrossed > linesCrossed) {																	//jeœli przez aktualn¹ poweirzchniê przesz³o wiêcej prostych ni¿ poprzednie maksimum
				linesCrossed = checkCrossed;																	//zaktualizuj nowe maksimum
				nRet = newN;																					//zaktualizuj wartoœc zwracanaego wektora normalnego
			}
	}
	std::pair<Vector, unsigned> normal_linesCrossed;															//utwórz parê ('normal', 'linesCrossed')											
	normal_linesCrossed.first = nRet;																			//[normaln¹ œciany z któr¹ zasz³a kolizja oraz liczbê prostych które przez ni¹ przesz³y]
	normal_linesCrossed.second = linesCrossed;
	return normal_linesCrossed;
}

//za³adowanie punktów pocz¹tkowych dla prostych do algorytmu 'Ray Casting'
//[argumety: 'point' - tablica przechowuj¹ca wyzanczone punkty, 'R' - promieñ dla sfery kolizji postaci, 'playerPos' - aktualna pozycja postaci, 'playerMidPoint' - œrodek sfery kolizji postaci]
void CollisionDetection::GetCollisionFaceNormal_LoadPoints(Vector point[], Character& model1) {
	srand(static_cast<unsigned int>(time(0)));
	Vector shift(model1.getCollisionBox().maxPoint - model1.getCollisionBox().midPoint);						//wektor przesuniêcia ze œrodka bry³y kolizji do najbardziej wysuniêtego punktu
	Vector midPt = model1.getCollisionBox().midPoint, playerPos = model1.getPosition();

	for (unsigned int i = 0; i < RayCastLinesNum / 8; i++) {													//wyznaczenie punktów startowych dla 'RayCast'
		for (unsigned int j = 0; j < 3; j++) {																	//wylosowaniew wartoœci wspó³rzêdnych punktów ograniczonych przez wektor 'shift' 
			int tmpR = static_cast<int>(shift[j] * 100);
			point[i][j] = (rand() % tmpR)/100.0;																//wylosowanie wspó³rzêdnych punktu dla 'RayCast' przy ograniczeniach nak³adanych przez 'dist'
		}

		unsigned int j = i + RayCastLinesNum / 8;																//wyznaczenie wartoœci punktów dla ka¿dej pozostalych 7 czêœci sfery jako odbiæ punktu bazowego
		point[j] = point[i]; point[j].y = -point[j].y;															//[punkt bazowy: x > 0, y > 0, z > 0]
		j = j + RayCastLinesNum / 8;
		point[j] = point[i]; point[j].z = -point[j].z;
		j = j + RayCastLinesNum / 8;
		point[j] = point[i]; point[j].y = -point[j].y; point[j].z = -point[j].z;
		j = j + RayCastLinesNum / 8;
		point[j] = point[i]; point[j].x = -point[j].x;
		j = j + RayCastLinesNum / 8;
		point[j] = point[i]; point[j].x = -point[j].x; point[j].y = -point[j].y;
		j = j + RayCastLinesNum / 8;
		point[j] = point[i]; point[j].x = -point[j].x; point[j].z = -point[j].z;
		j = j + RayCastLinesNum / 8;
		point[j] = point[i].Negate();
	}

	for (unsigned int i = 0; i < RayCastLinesNum; i++)
		point[i] = model1.getTransformMatrix()*point[i];														//przekszta³cenie wylosowanych punktów zgodnie z macierz¹ przekszta³ceñ modelu
}

//wykonanie algorytmu 'RayCast' dla funkcji pobierania normalnej
//[argumenty: 'faceP' - punkty opisuj¹ce dan¹ powierzchniê (trójk¹t), 'point' - losowe punkty startowe dla prostych w algorytmie 'RayCast', 'normal' - normalna do pow., 'surface' - param. powierzchni]
int CollisionDetection::GetCollisionFaceNormal_GetNormal_RayCast(Vector faceP[],Vector point[],Vector normal,double surface[],double R) {
	bool tError = false;
	unsigned int checkCrossed = 0;

	for (unsigned int j = 0; j < RayCastLinesNum; j++) {													//rozpoczêcie algorytmu 'RayCast'
		double t;																							//parametr 't' dla równania parametrycznego prostej
		double licz = -surface[3] - surface[0] * point[j].x - surface[1] * point[j].y - 
			surface[2] * point[j].z;																		//licznik parametru 't'
		double mian = surface[0] * normal.x + surface[1] * normal.y + surface[2] * normal.z;				//mianownik parametru 't'
		if (mian == 0.0) t = 0.0;																			//zabezpieczenie przed dzieleniem przez '0'			
		else t = licz / mian;																				//wyznacz parametr 't'

		Vector newPoint = normal*t + point[j];																//wyznaczenie rzutu punktu 'point[j]' na opisan¹ wczeœniej powierzchniê wzd³u¿ jej normalnej
		if (t <=0 ) {																						//jeœli parametr 't' jest dodatni
			if (CheckPointInTriangle(newPoint, faceP, normal))												//jeœli punkt znajduje siê wewn¹trz powierzchni					
				checkCrossed++;																				//zwiêksz licznik prostych dla testu raycast
		}																									//[oznacza to ¿e œciana ta nie znajduje siê naprzeciw postaci - wtedy 't' < 0]
	}
	return checkCrossed;
}