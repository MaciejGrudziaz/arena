#include "Quaternion.h"

Quaternion& Quaternion::operator=(const Quaternion q) {
	w = q.w;
	v = q.v;
	return *this;
}

Quaternion Quaternion::operator+(const Quaternion& q) const {
	Quaternion q_out;
	q_out.w = w + q.w;
	q_out.v = v + q.v;
	return q_out;
}

Quaternion& Quaternion::operator+=(const Quaternion& q) {
	w = w + q.w;
	v = v + q.v;
	return *this;
}

Quaternion Quaternion::operator-(const Quaternion& q) const {
	Quaternion q_out;
	q_out.w = w - q.w;
	q_out.v = v - q.v;
	return q_out;
}

Quaternion& Quaternion::operator-=(const Quaternion& q) {
	w = w - q.w;
	v = v - q.v;
	return *this;
}

Quaternion Quaternion::operator*(const double& c) const {
	Quaternion q_out;
	q_out.w = c*w;
	q_out.v = v*c;
	return q_out;
}

Quaternion& Quaternion::operator*=(const double& c) {
	w = w*c;
	v = v*c;
	return *this;
}

Quaternion Quaternion::operator/(const double& c) const {
	Quaternion q_out;
	q_out.w = w / c;
	q_out.v = v / c;
	return q_out;
}

Quaternion& Quaternion::operator/= (const double& c) {
	w = w / c;
	v = v / c;
	return *this;
}

//obliczenie normy kwaternionu	
double Quaternion::MOD() const {
	double mod;
	mod = w*w + v.x*v.x + v.y*v.y + v.z*v.z;
	mod = sqrt(mod);
	return mod;
}

//normalizacja kwaternionu
void Quaternion::NORM() {
	double d = this->MOD();
	if (d != 0) *this / d;
}

//obliczenie kwaternionu przeciwnego
Quaternion Quaternion::INV() const{
	Quaternion q_out;
	q_out.w = w;
	q_out.v = v.Negate();
	return q_out;
}

//iloczyn skalarny
double Quaternion::DotProduct(const Quaternion& q) const{
	double dotProduct;
	dotProduct = w*q.w;
	dotProduct += v.DotProduct(q.v);
	return dotProduct;
}

//iloczyn wektorowy
Quaternion Quaternion::CrossProduct(const Quaternion& q) const{
	Quaternion q_out;
	q_out.w = w*q.w - v.DotProduct(q.v);
	q_out.v = q.v*w + v*q.w + v.CrossProduct(q.v);
	return q_out;
}

//stworzenie macierzy jednorodnej z kwaternionu
Matrix Quaternion::CreateMatrix() const {
	Matrix mat(4, 4);
	double a, b, c, d, e, f, x2, y2, z2;
	a = v.x*v.y; b = w*v.z; c = v.x*v.z;
	d = w*v.y; e = v.y*v.z; f = w*v.x;
	x2 = v.x*v.x; y2 = v.y*v.y; z2 = v.z*v.z;

	mat[0][0] = 1 - 2 * (y2 + z2); mat[0][1] = 2 * (a - b); mat[0][2] = 2 * (d + c); mat[0][3] = 0;
	mat[1][0] = 2 * (a + b); mat[1][1] = 1 - 2 * (x2 + z2); mat[1][2] = 2 * (e - f); mat[1][3] = 0;
	mat[2][0] = 2 * (c - d); mat[2][1] = 2 * (e + f); mat[2][2] = 1 - 2 * (x2 + y2); mat[2][3] = 0;
	mat[3][0] = 0; mat[3][1] = 0; mat[3][2] = 0; mat[3][3] = 1;

	return mat;
}