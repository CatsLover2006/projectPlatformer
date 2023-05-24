#include "collider.hpp"
#include "hailLib/basemath.hpp"
#include <cmath>
#include <typeinfo>

double max (double a, double b) {
	if (a>b) return a;
	return b;
}
double min (double a, double b) {
	if (a<b) return a;
	return b;
}
double lerp(double a, double b, double t) {
	return a*t + (1-t)*b;
}

namespace CollisionHandler {
	Collider::Collider() {
		Collider(0,0);
	}


	Collider::Collider(double x, double y) {
		setLocation(x, y);
	}

	RightTriCollider::RightTriCollider(double x, double y, double x2, double y2, bool topTriangle): Collider(x, y) {
		useTopside = topTriangle;
		this->x2 = x2;
		this->y2 = y2;
	}

	BoxCollider::BoxCollider(double x, double y, double w, double h): Collider(x, y) {
		this->w = w;
		this->h = h;
	}

	void Collider::setLocation(double x, double y) {
		this->x = x;
		this->y = y;
	}

	void RightTriCollider::setLocation(double x, double y) {
		x2 -= this->x;
		x2 += x;
		y2 -= this->y;
		y2 += y;
		this->x = x;
		this->y = y;
	}

	double Collider::getBound_t() {return 0;} // Stub
	double Collider::getBound_b() {return 0;} // Stub
	double Collider::getBound_l() {return 0;} // Stub
	double Collider::getBound_r() {return 0;} // Stub

	double BoxCollider::getBound_t() {return y;}
	double BoxCollider::getBound_b() {return y+h;}
	double BoxCollider::getBound_l() {return x;}
	double BoxCollider::getBound_r() {return x+w;}

	double RightTriCollider::getBound_t() {return min(y, y2);}
	double RightTriCollider::getBound_b() {return max(y, y2);}
	double RightTriCollider::getBound_l() {return min(x, x2);}
	double RightTriCollider::getBound_r() {return max(x, x2);}

	double Collider::getMid_y() {
		return (this->getBound_t() + this->getBound_b()) / 2;
	}

	double Collider::getMid_x() {
		return (this->getBound_l() + this->getBound_r()) / 2;
	}

	bool Collider::colliding(Collider * other) {
		return false;
	}

	bool BoxCollider::colliding(Collider * other) {
		if (typeid(other) == typeid(RightTriCollider)) return other->colliding(this);
		BoxCollider * otherBox = (BoxCollider*)other;
		return ((hailMath::abs((x + w/2) - (otherBox->x + otherBox->w/2)) * 2 < (w + otherBox->w)) &&
         		(hailMath::abs((y + h/2) - (otherBox->y + otherBox->h/2)) * 2 < (h + otherBox->h)));
	}

	bool Collider::pointInCollider(double x, double y) {
		return false;
	}

	bool BoxCollider::pointInCollider(double x, double y) {
		return (y > getBound_t() && y < getBound_b()) && (x > getBound_l() && x < getBound_r());
	}

	bool RightTriCollider::pointInCollider(double x, double y) {
		if ((y > getBound_t() && y < getBound_b()) && (x > getBound_l() && x < getBound_r())) {
			if (useTopside) {
				return (y > lerp(min(this->y2, this->y), max(this->y2, this->y), (x - this->x) / hailMath::abs(this->x - this->x2)));
			} else {
				return (y < lerp(min(this->y2, this->y), max(this->y2, this->y), (x - this->x) / hailMath::abs(this->x - this->x2)));
			}
		}
		return false;
	}

	bool RightTriCollider::colliding(Collider * other) {
		if (typeid(other) == typeid(RightTriCollider)) {
			RightTriCollider * otherTri = (RightTriCollider*)other;
			if ((hailMath::abs((x + x2)/2 - (otherTri->x + otherTri->x2)/2) * 2 < (hailMath::abs(x2-x) + hailMath::abs(otherTri->x2-otherTri->x))) &&
	         	(hailMath::abs((y + y2)/2 - (otherTri->y + otherTri->y2)/2) * 2 < (hailMath::abs(y2-y) + hailMath::abs(otherTri->y2-otherTri->y)))) {
				// Too lazy rn
			}
			return false;
		}
		BoxCollider * otherBox = (BoxCollider*)other;
		if ((hailMath::abs((x + x2)/2 - (otherBox->x + otherBox->w/2)) * 2 < (hailMath::abs(x2-x) + otherBox->w)) &&
         	(hailMath::abs((y + y2)/2 - (otherBox->y + otherBox->h/2)) * 2 < (hailMath::abs(y2-y) + otherBox->h))) {
			double cX = otherBox->x;
			double cY = otherBox->y;
			if (useTopside) {
				if (cY > lerp(min(this->y2, this->y), max(this->y2, this->y), (cX - this->x) / hailMath::abs(this->x - this->x2))) return true;
				cX += otherBox->w;
				if (cY > lerp(min(this->y2, this->y), max(this->y2, this->y), (cX - this->x) / hailMath::abs(this->x - this->x2))) return true;
				cY += otherBox->h;
				if (cY > lerp(min(this->y2, this->y), max(this->y2, this->y), (cX - this->x) / hailMath::abs(this->x - this->x2))) return true;
				cX -= otherBox->w;
				if (cY > lerp(min(this->y2, this->y), max(this->y2, this->y), (cX - this->x) / hailMath::abs(this->x - this->x2))) return true;
			} else {
				if (cY < lerp(min(this->y2, this->y), max(this->y2, this->y), (cX - this->x) / hailMath::abs(this->x - this->x2))) return true;
				cX += otherBox->w;
				if (cY < lerp(min(this->y2, this->y), max(this->y2, this->y), (cX - this->x) / hailMath::abs(this->x - this->x2))) return true;
				cY += otherBox->h;
				if (cY < lerp(min(this->y2, this->y), max(this->y2, this->y), (cX - this->x) / hailMath::abs(this->x - this->x2))) return true;
				cX -= otherBox->w;
				if (cY < lerp(min(this->y2, this->y), max(this->y2, this->y), (cX - this->x) / hailMath::abs(this->x - this->x2))) return true;
			}
		}
		return false;
	}
}