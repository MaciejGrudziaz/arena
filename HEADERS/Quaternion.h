#pragma once
#include "Vector.h"
#include "Matrix.h"

//-------------------------------------------------------------------------------------------
//------------------------------KLASA REPREZENTUJ¥CA KWATERNION------------------------------
//-------------------------------------------------------------------------------------------

struct Quaternion {
	double w;															//skalar kwaternionu
	Vector v;															//wektor kwaternionu

	Quaternion():w(0),v(Vector(0,0,0)){}
	Quaternion(Vector _v):w(0),v(_v){}
	Quaternion(double _w):w(_w),v(Vector(0,0,0)){}
	Quaternion(double _w, Vector _v) :w(_w), v(_v) {}

	Quaternion& operator=(const Quaternion q);
	Quaternion operator+(const Quaternion& q) const;
	Quaternion& operator+=(const Quaternion& q);
	Quaternion operator-(const Quaternion& q) const;
	Quaternion& operator-=(const Quaternion& q);
	Quaternion operator*(const double& c) const;
	Quaternion& operator*=(const double& c);
	Quaternion operator/(const double& c) const;
	Quaternion& operator/= (const double& c);

	double MOD() const;													//obliczenie normy kwaternionu		
	void NORM();														//przekszta³cenie kwaternionu do kwaternionu jednostkowego
	Quaternion INV() const;												//obliczenie kwaternionu przeciwnego
	double DotProduct(const Quaternion& q) const;						//iloczyn skalarny
	Quaternion CrossProduct(const Quaternion& q) const;					//iloczyn wektorowy

	Matrix CreateMatrix() const;										//stworzenie macierzy jednorodnej z kwaternionu
};