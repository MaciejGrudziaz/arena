#include "SpecialMath.h"
#include "LOGFILE.h"

//sprawdzenie czy pkt. 'point' znajduje siê wewn¹trz trójk¹ta 'triangle' ['normal' - normalna do trójk¹ta]
bool CheckPointInTriangle(Vector point, Vector triangle[], Vector normal) {	//trójk¹t opisany jest przy pomocy 3 kolejno podanych punktów ['triangle'] oraz normalnej
	Vector n[3], AB, BC, CA;
	AB = triangle[1] - triangle[0]; BC = triangle[2] - triangle[1]; CA = triangle[0] - triangle[2];				//inicjalizacja wektorów opisuj¹cych kolejne ramiona trójk¹ta
	n[0] = AB.CrossProduct(normal);																				//obliczenie normalnych do ka¿dego boku trójk¹ta (wskazuj¹ one na zewn¹trz trójk¹ta)
	n[1] = BC.CrossProduct(normal);
	n[2] = CA.CrossProduct(normal);

	if (n[0].DotProduct(BC) > 0)																				//sprawdzenie czy wektory normalne do ka¿dego boku sa skierowane na zewn¹trz trójk¹ta
		n[0]=n[0].Negate();
	if (n[1].DotProduct(CA) > 0)
		n[1] = n[1].Negate();
	if (n[2].DotProduct(AB) > 0)
		n[2] = n[2].Negate();

	for (unsigned int i = 0; i < 3; i++) {																		//dla ka¿dego boku
		double A, B, C, D;
		A = n[i].x; B = n[i].y; C = n[i].z;																		//wyzanczamy parametry powierzchni opisuj¹cej dany bok	
		D = -A*triangle[i].x - B*triangle[i].y - C*triangle[i].z;												//[przy pomocy normalnej do danego boku oraz punktów wchodz¹cych w jego sk³ad]

		double t = -D - A*point.x - B*point.y - C*point.z;														//obliczenie parametru 't' dla parametrycznego równania prostej	
																												//zaczynaj¹cej siê w punkcie 'point' i skierowanej zgodnie z wektorem normalnym
		if (t < 0) return false;																				//jeœli wartoœc ta jest ujemna - punkt nie znajduje siê wewn¹trz trójk¹ta - zwróæ FALSE																			
	}

	return true;																								//jeœli wszystkie wartoœci parametru 't' by³y dodatnie - punkt znajduje siê wewn¹trz trójk¹ta
}

//rzut pkt. 'point' na powierzchniê 'surface' wzd³u¿ wek. 'projectionVec' ['normal' - normalna do powierzchni 'surface']
Vector PointSurfaceProjection(Vector surface[], Vector normal, Vector point, Vector projectionVec) {
	double A, B, C, D;																							//parametry równania powierzchni [Ax+By+Cz+D=0]
	double t=0,t_licz,t_mian;																					//parametr 't' równania parametrycznego prostej
	Vector pointProjection;																						//rzut puntku 'point' na powierzchniê 'surface'
	
	A = normal.x;																								//obliczenie paramterów równania powierzchni						
	B = normal.y;
	C = normal.z;
	D = -A*surface[0].x - B*surface[0].y - C*surface[0].z;

	t_licz = -D - A*point.x - B*point.y - C*point.z;															//obliczenie parametru 't' równania prostej [p'=projectionVec*t+p]
	t_mian = A*projectionVec.x + B*projectionVec.y + C*projectionVec.z;
	if (t_mian != 0) t = t_licz / t_mian;

	for (unsigned i = 0; i < 3; i++)
		pointProjection[i] = point[i] + projectionVec[i] * t;													//obliczenie rzutu pkt. 'point' na powierzchniê 'surface'

	return pointProjection;
}