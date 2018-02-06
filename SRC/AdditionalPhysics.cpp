#include <AdditionalPhysics.h>

std::map<int, PhysicsModel*> AdditionalPhysics::models;

//przetworzenie wszystkich zarejestrowanych modeli
void AdditionalPhysics::ProcessAllModels() {
	for (std::pair<int, PhysicsModel*> modelPtr : models)
		modelPtr.second->Process();																		//wywo³anie funkcji 'Process' dla ka¿dego modelu fizyki
}

//rejestracja nowego modelu fizyki
void AdditionalPhysics::RegisterModel(PhysicsModel* newModel,int priority) {
	models.insert(std::pair<int,PhysicsModel*>(priority,newModel));										//dodanie nowego modelu fizyki do kontenera 
}																										//[priority - priorytet wykonania modelu, im ni¿sza wart. tym wy¿szy priorytet]