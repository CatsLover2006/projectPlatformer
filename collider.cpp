#include "collider.hpp"
#include "hailLib/basemath.hpp"
#include <cmath>
#include <typeinfo>
#include <iostream>

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

	double RightTriCollider::getBound_t() {return hailMath::min<double>(y, y2);}
	double RightTriCollider::getBound_b() {return hailMath::max<double>(y, y2);}
	double RightTriCollider::getBound_l() {return hailMath::min<double>(x, x2);}
	double RightTriCollider::getBound_r() {return hailMath::max<double>(x, x2);}

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
		if (dynamic_cast<RightTriCollider*>(other)) return other->colliding(this);
		BoxCollider * otherBox = (BoxCollider*)other;
		return (getBound_l() <= otherBox->getBound_r()) && (getBound_r() >= otherBox->getBound_l()) &&
			(getBound_b() >= otherBox->getBound_t()) && (getBound_t() <= otherBox->getBound_b());
	}

	bool Collider::pointInCollider(double x, double y) {
		return false;
	}

	bool BoxCollider::pointInCollider(double x, double y) {
		return (y >= getBound_t() && y <= getBound_b()) && (x >= getBound_l() && x <= getBound_r());
	}

	bool RightTriCollider::pointInCollider(double x, double y) {
		if ((x <= getBound_r()) && (x >= getBound_l()) && (y >= getBound_t()) && (y <= getBound_b())) {
			double hi = hailMath::min<double>(this->x2, this->x)==this->x?this->y:this->y2;
			double lo = hailMath::max<double>(this->x2, this->x)==this->x?this->y:this->y2;
			if (useTopside) {
				return (y > hailMath::lerp<double>(lo, hi, (x - hailMath::min<double>(this->x2, this->x)) / hailMath::abs(this->x - this->x2)));
			} else {
				return (y < hailMath::lerp<double>(lo, hi, (x - hailMath::min<double>(this->x2, this->x)) / hailMath::abs(this->x - this->x2)));
			}
		}
		return false;
	}

	bool RightTriCollider::colliding(Collider * other) {
		if (dynamic_cast<RightTriCollider*>(other)) {
			RightTriCollider * otherTri = (RightTriCollider*)other;
			if ((hailMath::abs((x + x2)/2 - (otherTri->x + otherTri->x2)/2) * 2 < (hailMath::abs(x2-x) + hailMath::abs(otherTri->x2-otherTri->x))) &&
	         	(hailMath::abs((y + y2)/2 - (otherTri->y + otherTri->y2)/2) * 2 < (hailMath::abs(y2-y) + hailMath::abs(otherTri->y2-otherTri->y)))) {
				// Too lazy rn
			}
			return false;
		}
		if(pointInCollider(other->getBound_l(), other->getBound_t())) return true;
		if(pointInCollider(other->getBound_r(), other->getBound_t())) return true;
		if(pointInCollider(other->getBound_l(), other->getBound_b())) return true;
		if(pointInCollider(other->getBound_r(), other->getBound_b())) return true;
		if(pointInCollider(other->getBound_l(), other->getMid_y())) return true;
		if(pointInCollider(other->getBound_r(), other->getMid_y())) return true;
		if(pointInCollider(other->getMid_x(), other->getBound_t())) return true;
		if(pointInCollider(other->getMid_x(), other->getBound_b())) return true;
		if(pointInCollider(other->getMid_x(), other->getMid_y())) return true;
		return false;
	}
}