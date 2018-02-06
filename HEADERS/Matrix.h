#pragma once
#include "Vector.h"

//-----------------------------------------------------------------------------------------
//---------------------------------IMPLEMENTACJA MACIERZY----------------------------------
//------------------------------klasa implementuj¹ca macierze------------------------------
//-----------------------------------------------------------------------------------------

class Matrix {
	unsigned int rows;												//liczba wierszy								
	unsigned int columns;											//liczba kolumn

	double** matrix;												//tablica opisuj¹ca macierz										

public:

	//konstruktory
	Matrix();														

	Matrix(unsigned int Rows, unsigned int Columns);	

	Matrix(const Matrix& m);

	~Matrix();														//destruktor

	void Initilize(unsigned int Rows, unsigned int Columns);		//inicjalziacja macierzy

	void LoadIdentity();											//za³adowanie macierzy jednostkowej

	void LoadZeros();												//za³adowanie macierzy zerowej

	//przeci¹¿enie operatorów
	Matrix& operator=(const Matrix& m);

	Matrix operator*(const Matrix& m);

	Vector operator*(const Vector& v);

	Matrix operator+(const Matrix& m);

	Matrix operator-(const Matrix& m);

	double* operator[](const unsigned int a);
};