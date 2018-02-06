#include "OBJmodel.h"

//konstruktor domyœlny
OBJmodel::OBJmodel() {
	position = { 0,0,0 };										//ustawienie domyœlnych wartoœci pól klasy
	orientation = { 0,0,0 };
	transform.Initilize(4, 4);
}

//funkcja ³aduj¹ca model z pliku
int OBJmodel::LoadModel(/*const char**/const std::string& filename) {
	std::string _filename = "MODELS/" + filename;
	char charFilename[50];
	unsigned idx = 0;
	for (idx = 0; idx < _filename.size(); idx++)
		charFilename[idx] = _filename[idx];
	charFilename[idx] = NULL;

	//unsigned int faceStartIdx;								//indeks okreœlaj¹cy ostatni element w wektorze 'faces' przed wczytaniem kolejengo obiektu
	bool end = false;											//znacznik pomocniczy u¿ywany do wczytywania modelu
	std::fstream file;											//klasa odpowiadaj¹ca za wczytywanie strumienia z pliku
	std::string objectName;
	file.open(charFilename, std::ios::in);							//otworzenie pliku [tylko do odczytu]

	if (!file.is_open())										//sprawdzenie czy plik zosta³ poprawnie otworzony
		return 0;

	skipFileBegin(file);										//ominiêcie znaków na pocz¹tku pliku a¿ do wspó³rzêdnych wierzcho³ków

	while (!end) {
		objectName=readObjectName(file);						//wczytanie nazwy obiektu

		if (objectName.compare(0, 6, "HITBOX")==0)				//jeœli nazwa obiektu zaczyna siê od s³owa "HITBOX"
			end=loadCollisionBox(file,objectName);				//wczytaj dane bry³y kolizji
		else end=loadObject(file,objectName);					//w przeciwnym wypadku wczytaj dane obiektu

	}

	file.close();												//zamkniêcie pliku

	return 1;
}

//funkcja rysuj¹ca wczytany model
void OBJmodel::DrawModel() {
	Vector normal;												//aktualna normalna do rysowanej powierzchni
	Vector tmpVertices[3];										//wierzcho³ki aktualnie rysowanego trójk¹ta

	glPushMatrix();												//umieeszczenie macierzy widoku OpenGL na stosie

	ModelTransformation();										//dokonanie odpowiedniej transformacji modelu [opisanej za pomoc¹ zmiennych 'position' i 'orientation'

	glBegin(GL_TRIANGLES);										//rozpoczêcie rysowania modelu

	for (unsigned int i = 0; i < objects.size(); i++) {
		for (unsigned int j = 0; j < objects[i].facesIdx.size(); j++) {
			//wczytanie danych dla powierzchni okreœlonej indeksem 'objects[i].facesIdx[j]'
			DRAW_getFaceData(tmpVertices, normal, objects[i].facesIdx[j]);
			for (int j = 0; j < 3; j++) {
				//za³adowanie normalnej do modleu widoku OpenGL
				glNormal3d(normal.x, normal.y, normal.z);		
				//za³adowanie wierzcho³ków do modelu widoku OpenGL
				glVertex3d(tmpVertices[j].x, tmpVertices[j].y, tmpVertices[j].z);
			}
		}
	}

	glEnd();													//koniec rysowania modelu

	glPopMatrix();												//wczytanie macierzy widoku OpenGL ze stosu
}

//funkcja rysuj¹ca bry³y kolizji dla ca³ego modelu
void OBJmodel::DrawCollisionBox() {
	Vector normal, tmpVertices[3];														//aktualna normalna oraz wierzcho³ki powierzchni	

	glPushMatrix();																		//umieszczenie macierzy widoku OpenGL na stosie
	glBegin(GL_LINE_LOOP);																//rozpoczêcie rysowania bry³ kolizji
	//rysowanie bry³ kolizji dla ka¿dego obiektu modelu
	for (unsigned int i = 0; i < objects.size(); i++) {
		for (unsigned int j = 0; j < objects[i].collisionBox.size(); j++) {
			for (unsigned int k = 0; k < objects[i].collisionBox[j].facesIdx.size(); k++) {
				//wczytanie danych dla powierzchni okreœlonej indeksem 'objects[i].collisionBox[j].facesIdx[k]'
				//parametr 'transformedVertices_VEC' okreœla wczytywanie wspó³rzêdnych z wektora przekszta³conych wierzcho³ków
				DRAW_getFaceData(tmpVertices, normal, objects[i].collisionBox[j].facesIdx[k],transformedVertices_VEC);
				for (unsigned int l = 0; l < 3; l++) {									//za³adowanie normalnej do modleu widoku OpenGL
					glNormal3d(normal.x, normal.y, normal.z);
					glVertex3d(tmpVertices[l].x, tmpVertices[l].y, tmpVertices[l].z);	//za³adowanie wierzcho³ków do modelu widoku OpenGL
				}
			}
		}
		//narysowanie g³ównej bry³y kolizji obiektu
		for (unsigned int k = 0; k < objects[i].mainColBox.facesIdx.size(); k++) {
			//wczytanie danych dla powierzchni okreœlonej indeksem 'objects[i].collisionBox[j].facesIdx[k]'
			//parametr 'transformedVertices_VEC' okreœla wczytywanie wspó³rzêdnych z wektora przekszta³conych wierzcho³ków
			DRAW_getFaceData(tmpVertices, normal, objects[i].mainColBox.facesIdx[k], transformedVertices_VEC);
			for (unsigned int l = 0; l < 3; l++) {
				glNormal3d(normal.x, normal.y, normal.z);							//za³adowanie normalnej do modleu widoku OpenGL
				glVertex3d(tmpVertices[l].x, tmpVertices[l].y, tmpVertices[l].z);	//za³adowanie wierzcho³ków do modelu widoku OpenGL
			}
		}
	}
	//rysowanie bry³ kolizji dla ca³ego modelu
	for (unsigned int i = 0; i < collisionBox.facesIdx.size(); i++) {
		//wczytanie danych dla powierzchni okreœlonej indeksem 'objects[i].collisionBox[j].facesIdx[k]'
		//parametr 'transformedVertices_VEC' okreœla wczytywanie wspó³rzêdnych z wektora przekszta³conych wierzcho³ków
		DRAW_getFaceData(tmpVertices, normal, collisionBox.facesIdx[i], transformedVertices_VEC);
		for (unsigned int j = 0; j < 3; j++) {
			glNormal3d(normal.x, normal.y, normal.z);							//za³adowanie normalnej do modleu widoku OpenGL
			glVertex3d(tmpVertices[j].x, tmpVertices[j].y, tmpVertices[j].z);	//za³adowanie wierzcho³ków do modelu widoku OpenGL
		}
	}
	glEnd();																	//koniec rysowania bry³ kolizji
	glPopMatrix();																//wczytanie amcierzy widoku OpenGL ze stosu
}

//transformacja macierzy przekszta³ceñ modelu
void OBJmodel::ModelTransformation() {
	//przesuniêcie modelu do odpowiedniej pozycji [opisanej przy pomocy zmiennej 'position']
	glTranslated(position.x, position.y, position.z);

	//ustawienie odpowiedniej orientacji modelu [opisanej przy pomocy zmiennej 'orientation' - ka¿da sk³adowa zmiennej opisuje k¹t obrotu wokó³ odpwoiedniej osi]
	glRotated(orientation.z, 0.0, 0.0, 1.0);
	glRotated(orientation.y, 0.0, 1.0, 0.0);
	glRotated(orientation.x, 1.0, 0.0, 0.0);
}

//funckaj pomocnicza dla metody opisuj¹cej rysowanie modelu
void OBJmodel::DRAW_getFaceData(Vector tmpVertices[], Vector& normal,unsigned int faceIdx, int verticesVector) {
	unsigned int vertexIdx;										//zmienna pomocnicza okreœlaj¹ca indeks aktualnie wczytywanego wierzcho³ka
	unsigned int normalIdx;										//zmienna pomocnicza okreœlaj¹ca indeks aktualnie wczytywanej normalnej

	for (int j = 0; j < 3; j++) {								//wczytanie wierzcho³ków dla aktualnej poweirzchni opisanej indeksem 'faceIdx'
		vertexIdx = faces[faceIdx].PointsIdx[j];				//pobranie indeksu wierzcho³ka

		if(verticesVector==0)	
			tmpVertices[j] = vertices[vertexIdx];					//wczytanie wspó³rzêdnych wierzcho³ka
		if (verticesVector == 1)
			tmpVertices[j] = transformedVertices[vertexIdx];
	}

	normalIdx = faces[faceIdx].NormalIdx;						//wczytanie indeksu normalnej do poweirzchni	
	if(verticesVector==0)
		normal = normals[normalIdx];								//pobranie wspó³rzêdnych normalnej
	if (verticesVector == 1)
		normal = transformedNormals[normalIdx];
}

//funkcja zamieniaj¹ca ci¹g znaków 'string' o danej d³ugoœci 'n' na liczbê ca³kowit¹
int OBJmodel::char2int(char* string, int n) {
	int retVal = 0;												//wartoœæ zwracana
	int tmp;													//aktualnie wczytany znak z tablicy 'string'
	for (int i = 0; i < n; i++) {
		tmp = string[i] - 48;									//pobranie znaku i zmienienie go na odpowiedni¹ wartoœæ ca³kowitoliczbow¹
		for (int j = 0; j < n - i - 1; j++) {
			tmp *= 10;											//okreœlenie prawid³owej 'dziesiêtnej' wartoœci danej cyfry
		}
		retVal += tmp;											//dodanie aktualnie pobranej cyfry do wartoœci zwracanej
	}

	return retVal;
}

//opuszczeni znaków na pocz¹tku pliku a¿ do nazwy pierwszego obiektu
void OBJmodel::skipFileBegin(std::fstream& file) {
	bool data = false;
	char tmp[1] = { NULL };

	while (!data) {
		file.read(tmp, 1);
		if (tmp[0] != 'o') {									//jeœli pierwszym znakiem w nowej linii nie jest 'v'
			while (tmp[0] != '\n') {							//pobieraj kolejne znaki a¿ do koñca linii
				file.read(tmp, 1);
			}
		}
		else data = true;
	}
}

//wczytanie nazwy obiektu z pliku
std::string OBJmodel::readObjectName(std::fstream& file) {
	bool data = true;
	char tmp[1] = { NULL };
	std::string name;

	while (data) {											
		file.read(tmp, 1);
		if (tmp[0] != '\n') {									//dopóki nie wczytano znaku koñca linii
			name.push_back(tmp[0]);								//wczytaj znak do stringa nazwy obiektu
		}
		else {													//w przeciwnym wypadku
			file.read(tmp, 1);									//wczytaj kolejny znak ['v']
			data = false;										//ustaw znacznik koñca pêtli
		}
	}

	//wyczyszczenie zawartoœci stringa nazwy
	name.erase(0, 1);											//wykasuj pierwszy znak z nazwy [' ']
	unsigned int startIdx = 0;									//indeks startowy dla stringa nazwy
	if (name.compare(0, 6, "HITBOX")==0)						//jeœli nazwa obiektu zaczyna siê od "HITBOX"
		startIdx = 7;											//startIdx=7
	else startIdx = 0;											//w przeciwnym wypadku startIdx=0
	for (unsigned int i = startIdx; i < name.size(); i++) {
		/*if (name[i] == '_')										//jeœli wczytano znak '_'
			name.erase(i, std::string::npos);*/					//usuñ pozosta³e znaki do koñca stringa
		if (name[i] == '_')										//jeœli wczytano znak '_'
			if (name.compare(i + 1, 4, "MAIN")==0)				//jeœli nastêpnym s³owem jest 'MAIN'
				i += 4;											//skocz iteratorem do nastêpnego s³owa
			else name.erase(i, std::string::npos);				//w przeciwnym wypadku usuñ pozosta³e znaki do koñca stringa
	}

	return name;
}

//opuszczenie znaków w œrodku pliku od zakoñczenai wspó³rzêdnych normalnych do indeksów wierzcho³ków pwoeirzchni
void OBJmodel::skipMidFile(std::fstream& file) {
	char tmp[1] = { NULL };
	bool data = false;

	while (!data) {
		file.read(tmp, 1);
		if (tmp[0] != 'f') {									//jeœli pierwszym znakiem w nowej linii nie jest 'f'
			while (tmp[0] != '\n') {							//pobieraj kolejne znaki a¿ do koñca linii
				file.read(tmp, 1);
			}
		}
		else data = true;
	}
}

//za³adowanie danych dla obiektu
bool OBJmodel::loadObject(std::fstream& file, std::string objectName) {
	bool endFile = false;										//znacznik koñca pliku
	unsigned int faceStartIdx = faces.size();

	VERTEX_LoadVertices(file);									//pobranie wspó³rzêdnych wierzcho³ków

	NORMAL_LoadNormals(file);									//pobranie wspó³rzêdnych normalnych

	skipMidFile(file);											//ominiêcie znaków od zakoñczenia wspó³rzêdnych normalnych a¿ do pocz¹tku danych opisuj¹cych powierzchnie

	if (!FACE_LoadFaces(file))									//pobranie danych poweirzchni
		endFile = true;											//jeœli dotarto do koñca pliku zakoñcz pobieranie danych
	//wczytanie danych dla obiektu
	OBJECT_LoadObject(faceStartIdx, faces.size() - 1,objectName);

	return endFile;												//zwrócenie znacznika koñca pliku
}

//za³adowanie danych dla bry³y kolizji
bool OBJmodel::loadCollisionBox(std::fstream& file, std::string objectName) {
	objectName.erase(0, 7);										//usuniêcie czêœci "HITBOX_" ze stringa nazwy obiektu [pozstawienie czystej nazwy obiektu]
	bool endFile = false;										//znacznik koñca pliku
	int objIdx = OBJECT_findObject(objectName);					//sprawdzenie czy dany obiekt ju¿ istnieje [jeœli tak -> objIdx - indeks obiektu, w przeciwnym wypadku -> objIdx = -1]
	//indeksy pocz¹tkowe dla wierzcho³ków, powierzchni i normalnych wchodz¹cych w sk³¹d bry³y kolizji
	unsigned int verticesStartIdx = vertices.size(), facesStartIdx = faces.size(), normalsStartIdx = normals.size();
	CollisionBox newCollision;								

	VERTEX_LoadVertices(file);									//pobranie wspó³rzêdnych wierzcho³ków

	NORMAL_LoadNormals(file);									//pobranie wspó³rzêdnych normalnych

	skipMidFile(file);											//ominiêcie znaków od zakoñczenia wspó³rzêdnych normalnych a¿ do pocz¹tku danych opisuj¹cych powierzchnie

	if (!FACE_LoadFaces(file))									//pobranie danych poweirzchni
		endFile = true;											//jeœli dotarto do koñca pliku zakoñcz pobieranie danych
	//wczytanie danych dla bry³y kolizji
	COLLISIONBOX_LoadData(newCollision, verticesStartIdx, facesStartIdx, normalsStartIdx);
	//za³adowanie danych obiektu bry³y kolizji do odpowiedniego obiektu
	COLLISIONBOX_LoadCollisionToObject(newCollision, objectName, objIdx);

	return endFile;												//zwróæ znacznik koñca pliku
}

//pobranie wspó³rzêdnych wierzcho³ka
Vector OBJmodel::VERTEX_getVertex(std::fstream& file) {
	Vector point;
	double coord;

	file >> coord;
	point.x = coord;
	file >> coord;
	point.y = coord;
	file >> coord;
	point.z = coord;

	return point;
}

//za³adowanie wszystkich wspó³rzêdnych wierzcho³ków
void OBJmodel::VERTEX_LoadVertices(std::fstream& file) {
	bool data = true;
	char tmp[2] = { 'v',NULL };									//ostatnio pobrane znaki
	while (data) {
		if (tmp[0] == 'v' && tmp[1] == NULL) {					//jeœli w nowym wierszu pobrano znak 'v '
			vertices.push_back(VERTEX_getVertex(file));			//pobierz wspó³rzêdne wierzcho³ka

			file.read(tmp, 1);									//pobierz 2 znaki z nowej linii
			file.read(tmp, 2);
			if (tmp[1] != 'n') tmp[1] = NULL;					//jeœli nie pobrano kombinacji 'vn' to usatw znak tmp[1] na NULL i pobierz nastêpne wspó³rzêdne
		}
		else
			data = false;										//jeœli w nowym wierszu pobrano 'vn' zakoñcz pêtlê
	}
}

//pobranie wspó³rzêdnych normalnej
Vector OBJmodel::NORMAL_getNormal(std::fstream& file) {
	Vector normal(FREE);
	double coord;

	file >> coord;
	normal.x = coord;
	file >> coord;
	normal.y = coord;
	file >> coord;
	normal.z = coord;

	return normal;
}

//pobranie wszystkich normalnych
void OBJmodel::NORMAL_LoadNormals(std::fstream& file) {
	bool data = true;
	char tmp[2] = { 'v','n' };									//ostatnio pobrane znaki

	while (data) {
		if (tmp[0] == 'v' && tmp[1] == 'n') {					//jesli w nowym wierszu pobrano 'vn'
			normals.push_back(NORMAL_getNormal(file));			//wczytaj wspó³rzêdne normalnej

			file.read(tmp, 1);									//wczytaj 2 znaki z nowej linii
			file.read(tmp, 2);
		}
		else data = false;										//jeœli w nowej linii nie pobrano znaków 'vn' zakoñcz pêtlê
	}
}

//odczytywanie indeksu wierzcho³ka dla danej powierzchni [wczytuje indeks wierzcho³ka wraz ze znakiem '/' wystêpuj¹cym bezpoœrednio po nim]
int OBJmodel::FACE_readPtIdx(std::fstream& file) {
	unsigned int idx = 0;										//iloœæ wczytanych znaków
	unsigned int PtIdx;											//wczytany indeks wierzcho³ka
	char facetmp[256];											//tymczasowa tablica wczytanych znaków
	char tmp[1];
	bool data = true;

	while (data) {
		file.read(tmp, 1);										//pobranie kolejnego znaku w linii
		if (tmp[0] == '/') data = false;						//jeœli znakiem tym jest '/' zakoñcz pêtlê
		else {
			facetmp[idx] = tmp[0];								//jeœli pobranym znakiem nie jest '/' wczytaj go do tablicy
			idx++;												//zwiêksz iloœæ wczytanych znaków
		}
	}
	PtIdx = char2int(facetmp, idx);								//zmieñ tablicê znaków na liczbê int

	return PtIdx - 1;
}

//odczytywanie indeksu normalnej do poweirzchni [wczytuje indeks normalnej wraz ze znakiem ' ' wystêpuj¹cym bezpoœrednio po nim]
int OBJmodel::FACE_readNormIdx(std::fstream& file) {
	unsigned int idx = 0;										//iloœæ wczytanych znaków
	unsigned int NormIdx;										//wczytany indeks normalnej
	char facetmp[256];											//tymczasowa tablica wczytanych znaków
	char tmp[1];
	bool data = true;

	while (data) {
		file.read(tmp, 1);										//pobranie kolejnego znaku w linii
		if (tmp[0] == ' ') data = false;						//jeœli znakiem tym jest ' ' zakoñcz pêtlê
		else {
			facetmp[idx] = tmp[0];								//jeœli pobranym znakiem nie jest ' ' wczytaj go do tablicy
			idx++;												//zwiêksz iloœæ wczytanych znaków
		}
	}
	NormIdx = char2int(facetmp, idx);							//zmieñ tablicê znaków na liczbê int

	return NormIdx - 1;
}

//przesuniecie znacznika w tekœcie [pominiêcie znaków] dla odczytu powierzchni w zale¿noœci od obecnej pozycji w wierszu
void OBJmodel::FACE_skipSpace(std::fstream& file, int idx) {
	char tmp[1];
	bool data;

	switch (idx) {
	case 0:														//usuniêcie pojedynczej spacji po znaku 'f' oraz znaku '/' po odczytaniu pierwszego indeksu wierzcho³ka
		file.read(tmp, 1);
		break;
	case 1:														//pominiecie wszystkich znaków pomiêdzy drugim, a trzecim indeksem wierzcho³ka
		data = false;
		file.read(tmp, 1);
		while (!data) {											//wczytuj kolejne znaki a¿ do momentu wczytania ' '
			file.read(tmp, 1);
			if (tmp[0] == ' ') data = true;
		}
		break;
	case 2:														//pominiêcie wszystkich znaków po trzecim ideksie wierzcho³ka a¿ do koñca linii
		data = false;
		while (!data) {
			file.read(tmp, 1);									//wczytuj kolejne znaki a¿ do momentu wczytania '\n'
			if (tmp[0] == '\n') data = true;
		}
		break;
	}
}

//odczytywanie indeksu wierzcho³ka poweirzchni w zale¿noœci od jego po³o¿enia w wierszu
int OBJmodel::FACE_readFaceVertexIdx(std::fstream& file, int idx) {
	unsigned int ptIdx;

	//u¿ywanie funkcji pominiecia znaków w zale¿noœci od po³o¿enia indeksu w wierszu
	switch (idx) {
	case 0:
		FACE_skipSpace(file, 0);								//pominiêcie psutego znaku po pierwszym symbolu 'f' w wierszu
		break;
	case 2:
		FACE_skipSpace(file, 1);								//pominiêcie wszystkich znaków pomiêdzy indeksem 2 i 3 wierzcho³ka
		break;
	}

	ptIdx = FACE_readPtIdx(file);								//pobranie indeksu wierzcho³ka

	return ptIdx;
}

//odczytywanie indeksu normalnej dla poweirzchni
int OBJmodel::FACE_readFaceNormalIdx(std::fstream& file) {
	unsigned int normIdx;

	FACE_skipSpace(file, 0);									//pominiêcie znaku '/' po indeksie pierwszego wierzcho³ka

	normIdx = FACE_readNormIdx(file);							//odczytanie indeksu normalnej

	return normIdx;
}

//pobranie indeksów wierzcho³ków oraz indeksu normalnej dla danej powierzchni
Face OBJmodel::FACE_getFace(std::fstream& file) {
	Face face;

	for (int i = 0; i < 3; i++) {
		face.PointsIdx[i] = FACE_readFaceVertexIdx(file, i);		//pobranie indeksu wierzcho³ka i
		if (i == 0) face.NormalIdx = FACE_readFaceNormalIdx(file);	//po pobraniu indeksu wierzcho³ka 0 nale¿y pobraæ indeks normalnej do powierzchni 
	}

	return face;
}

//pobranie wszystkich danych powierzchni
int OBJmodel::FACE_LoadFaces(std::fstream& file) {
	bool data = true;
	char tmp[1] = { 'f' };										//ostatnio pobrany znak

	while (data) {
		if (tmp[0] == 'f') {									//dopóki pierwszym znakiem wczytanym w linii jest 'f'
			tmp[0] = NULL;

			faces.push_back(FACE_getFace(file));				//wczytaj kolejne dane powierzchni

			FACE_skipSpace(file, 2);							//pomiñ wszystkie znaki po trzecim indeksie wierzcho³ka a¿ do koñca linii

			file.read(tmp, 1);									//wczytaj pierwszy znak z nowej linii
		}
		else data = false;
	}

	if (tmp[0] == '\0') return 0;								//jeœli wczytano znak koñca pliku zwróæ 0
	else return 1;												//w przeciwnym przypadku zwróæ 1						
}

//wczytanie indeksów powierzchni oraz wierzcho³kó dla danego obiektu
void OBJmodel::OBJECT_LoadObject(unsigned int startIdx, unsigned int endIdx,std::string objectName) {
	//Object newObj;											//obiekt do wczytania
	int objIdx = OBJECT_findObject(objectName);

	Object* obj;
	if (objIdx != -1) obj = &objects[objIdx];
	else obj = new Object;

	obj->name = objectName;

	for (unsigned int i = startIdx; i <= endIdx; i++) {
		obj->facesIdx.push_back(i);								//wczytanie do wektora indeksów powierzchni kolejnych powierzchni wchodz¹cych w jego sk³ad

		for (unsigned int j = 0; j < 3; j++) {
			unsigned int idx = faces[i].PointsIdx[j];
			if (!OBJECT_CheckVertexInVector(*obj, idx))			//jeœli dany wierzcho³ek nie zosta³ jeszcze wczytany do obiektu
				obj->verticesIdx.push_back(idx);				//dodaj go do wekotra wierzcho³ków obiektu
		}
	}

	if (objIdx == -1) {
		objects.push_back(*obj);								//wczytanie obiektu do wektora obiektów
		delete obj;
	}
}

//sprawdzenie czy dany wierzcho³ek znajduje siê w wekotrze wierzcho³ków danego obiektu
bool OBJmodel::OBJECT_CheckVertexInVector(Object obj,unsigned int vertexIdx) {
	unsigned int n = obj.verticesIdx.size();
	for (unsigned int i = 0; i < n; i++) {
		unsigned int idx = obj.verticesIdx[i];
		if (idx == vertexIdx) return true;						//jeœli tak zwróæ TRUE
	}

	return false;												//w przeciwnym wypadku zwróæ FALSE
}

//funkcja zwracaj¹ca indeks obiektu o danej nazwie w wektorze obiektów modelu ['objects']
//jeœli dany obiekt nie istnieje - funkcja zwraca wartoœæ -1
int OBJmodel::OBJECT_findObject(std::string objectName) {
	unsigned int n = objects.size();

	for (unsigned int i = 0; i < n; i++)
		if (objects[i].name.compare(objectName)==0)				//jeœli nazwa obiektu zgadza siê z dan¹ nazw¹ 'objectName'
			return i;											//zwróæ indeks obiektu

	return -1;													//jeœli nie znaleziono obiektu o danej nazwie zwróæ -1
}

//za³adowanie danych do obiektu bry³y kolizji
void OBJmodel::COLLISIONBOX_LoadData(CollisionBox &colBox, unsigned int verticesStartIdx, unsigned int facesStartIdx, unsigned int normalsStartIdx){
	Vector midPt(0,0,0), maxPt(vertices[verticesStartIdx]),minPt(vertices[verticesStartIdx]);		//inicajlizacja zmiennych opisuj¹cych œrodek bry³y kolizji [midPt] i punktu o maksymalnych wspó³rzêdnych dla ka¿dej osi [maxPt]

	for (unsigned int i = verticesStartIdx; i < vertices.size(); i++) {
		colBox.verticesIdx.push_back(i);						//wczytanie indeksu wierzcho³ka do wektora indeksów bry³y kolizji
		/*if (vertices[i].x > maxPt.x) maxPt.x = vertices[i].x;	//sprawdzenie czy dany punkt nie ma wspó³rzêdnych wiêkszych ni¿ odpowiednie wspó³rzêdne 'maxPt'
		if (vertices[i].y > maxPt.y) maxPt.y = vertices[i].y;
		if (vertices[i].z > maxPt.z) maxPt.z = vertices[i].z;*/
		for (unsigned j = 0; j < 3; j++) {
			if (vertices[i][j] > maxPt[j]) maxPt[j] = vertices[i][j];
			if (vertices[i][j] < minPt[j]) minPt[j] = vertices[i][j];
		}

		//midPt += vertices[i];									//dodanie danego wierzcho³ka do sumy wspó³rzednych w celu obliczenia punktu 'midPt'
	}
	for (unsigned int i = facesStartIdx; i < faces.size(); i++)	
		colBox.facesIdx.push_back(i);							//wczytanie indeksów powierzchni do wekotra indeksów bry³y kolizji
	for (unsigned int i = normalsStartIdx; i < normals.size(); i++)
		colBox.normalsIdx.push_back(i);							//wczytanie indesków normalnych do wektora indeksów bry³y kolizji

	//midPt /= (vertices.size() - verticesStartIdx);				//podzielenie sumy wszystkich wierzcho³ków bry³y kolizji przez ich liczbê
	midPt = (maxPt + minPt) / 2.0;
	colBox.midPoint = midPt;									//dodanie punktu œrodkowego do bry³y kolizji

	Vector shift(maxPt - midPt);								//obliczenie wektora przesuniecia od punktu œrodkowego do punktu o maksymlanych wspólrzêdnych ['maxPt']
	colBox.radius = shift.MOD();								//wczytanie promienia bry³y kolizji jako d³ugoœci wektora przesuniêcia

	colBox.maxPoint = maxPt;									//dodanie wspó³rzêdnych punktu najbardziej wysuniêtego do bry³y kolizji
	colBox.minPoint = minPt;

	colBox.size = (maxPt - midPt) * 2.0;
}

//za³¹dowanie danej bry³y kolizji 'newCollision' do obiektu o danym indeksie 'objIdx'
void OBJmodel::COLLISIONBOX_LoadCollisionToObject(CollisionBox& newCollision,std::string objectName,unsigned int objIdx) {
	bool mainColBox = false;
	if (objIdx == -1) {											//jeœli obiekt o danej anzwie nie istnieje
		if (objectName == "MODEL")								//jeœli nazwa obiektu == 'MODEL"
			collisionBox = newCollision;						//oznacza to ¿e wczytana bry³a kolizji jest bry³¹ kolizji dla ca³ego modelu
		else {
																//jeœli nie jest to g³ówna bry³a kolizji obiektu
			if (!COLLISIONBOX_LoadCollisionToObject_CheckIfMainHitbox(objectName)) {										
				Object obj;										//stwórz nowy obiekt
				obj.name = objectName;							//wczytaj jego nazwê
				obj.collisionBox.push_back(newCollision);		//dodaj dla niego wczytan¹ bry³ê kolizji
				objects.push_back(obj);							//dodaj obiekt do wekotra obiketów modelu
			}
																//w przeciwnym wypadku za³aduj do obiektu g³ówn¹ bry³e kolizji
			else COLLISIONBOX_LoadCollisionToObject_LoadObjectMainHitbox(objectName, newCollision);
		}

	}
	else objects[objIdx].collisionBox.push_back(newCollision);		//jeœli istnieje - dodaj bry³e kolizji do odpowiedniego wektora obiektu danego indeksem objIdx
}

bool OBJmodel::COLLISIONBOX_LoadCollisionToObject_CheckIfMainHitbox(std::string& objectName) {
	for (unsigned int i = 0; i < objectName.size(); i++) {			//sprawdzenie nazwy obiektu
		if (objectName[i] == '_') {
			if (objectName.compare(i + 1, 4, "MAIN") == 0) {		//jeœli nazwa zakoñczona jest na "MAIN"
				objectName.erase(i, std::string::npos);				//usuniêcie s³owa "MAIN" z nazwy
				return true;
			}
		}
	}

	return false;
}

void OBJmodel::COLLISIONBOX_LoadCollisionToObject_LoadObjectMainHitbox(std::string objectName, CollisionBox& newCollision) {
	unsigned int objIdx = OBJECT_findObject(objectName);			//sprawdŸ czy istnieje obiekt o danej nazwie
	if (objIdx == -1) {												//jeœli nie
		Object obj;													//stwórz go
		obj.name = objectName;
		obj.mainColBox = newCollision;								//za³aduj g³ówn¹ bry³ê kolizji obiektu
		objects.push_back(obj);										//dodaj obiekt do wektora wszystkich obiektów dla modelu
	}
	else objects[objIdx].mainColBox = newCollision;					//jeœli istnieje to dodaj do za³aduj do niego g³ówn¹ bry³ê kolizji
}