#include "Vector.h"

Vector& Vector::ABS() {
	if (x < 0) x = -x;
	if (y < 0)y = -y;
	if (z < 0) z = -z;

	return *this;
}

double Vector::MOD() const {
	double R = sqrt(x*x + y*y + z*z);
	return R;
}

double Vector::DotProduct(Vector v) const {
	return (x*v.x + y*v.y + z*v.z);
}

Vector Vector::CrossProduct(Vector v)const {
	Vector result(0, 0, 0);

	result.x = v.z*y - v.y*z;
	result.y = v.x*z - v.z*x;
	result.z = v.y*x - v.x*y;

	return result;
}

Vector Vector::Negate() const {
	Vector result;
	result.x = -x;
	result.y = -y;
	result.z = -z;

	return result;
}

void Vector::NORM() {
	double d = this->MOD();
	if (d != 0)
		*this /= d;
}

Vector& Vector::operator= (const Vector& a) {				//przeci¹¿enie operatora przyrównania '='
	x = a.x;
	y = a.y;
	z = a.z;
	return *this;
}

Vector Vector::operator+(const Vector& a) const{
	Vector b;
	b.x = x + a.x;
	b.y = y + a.y;
	b.z = z + a.z;
	return b;
}

Vector& Vector::operator+= (const Vector& a) {
	x += a.x;
	y += a.y;
	z += a.z;
	return *this;
}

Vector Vector::operator-(const Vector& a) const {
	Vector b;
	b.x = x - a.x;
	b.y = y - a.y;
	b.z = z - a.z;
	return b;
}

Vector& Vector::operator-=(const Vector& a) {
	x -= a.x;
	y -= a.y;
	z -= a.z;
	return *this;
}

Vector Vector::operator*(double a) const{
	Vector b;
	b.x = x*a;
	b.y = y*a;
	b.z = z*a;
	return b;
}

Vector& Vector::operator*=(double a) {
	x *= a;
	y *= a;
	z *= a;
	return *this;
}

Vector Vector::operator/(double a) const {
	Vector b;
	b.x = x / a;
	b.y = y / a;
	b.z = z / a;
	return b;
}

Vector& Vector::operator/=(double a) {
	x /= a;
	y /= a;
	z /= a;
	return *this;
}

double& Vector::operator[](unsigned int a) {
	switch (a) {
	case 0:
		return x;
		break;
	case 1:
		return y;
		break;
	case 2:
		return z;
		break;
	default:
		return x;
		break;
	}
}