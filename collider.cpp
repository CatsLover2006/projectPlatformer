#include "collider.hpp"
#include "hailLib/basemath.hpp"
#include <cmath>
#include <typeinfo>
#include <iostream>

namespace CollisionHandler {
	bool lineOnLineCol(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4) {
		double uA = ((x4-x3)*(y1-y3) - (y4-y3)*(x1-x3)) / ((y4-y3)*(x2-x1) - (x4-x3)*(y2-y1));
		double uB = ((x2-x1)*(y1-y3) - (y2-y1)*(x1-x3)) / ((y4-y3)*(x2-x1) - (x4-x3)*(y2-y1));
		return (uA >= 0 && uA <= 1 && uB >= 0 && uB <= 1);
	}

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
		return (getBound_l() < otherBox->getBound_r()) && (getBound_r() > otherBox->getBound_l()) &&
			(getBound_b() > otherBox->getBound_t()) && (getBound_t() < otherBox->getBound_b());
	}

	bool Collider::pointInCollider(double x, double y) {
		return false;
	}

	bool Collider::lineColliding (double x1, double y1, double x2, double y2) {
		return false;
	}

	bool BoxCollider::pointInCollider(double x, double y) {
		return (y >= getBound_t() && y <= getBound_b()) && (x >= getBound_l() && x <= getBound_r());
	}

	bool BoxCollider::lineColliding (double x1, double y1, double x2, double y2) {
		if (lineOnLineCol(x1, y1, x2, y2, getBound_l(), getBound_t(), getBound_l(), getBound_b())) return true;
		if (lineOnLineCol(x1, y1, x2, y2, getBound_r(), getBound_t(), getBound_r(), getBound_b())) return true;
		if (lineOnLineCol(x1, y1, x2, y2, getBound_l(), getBound_t(), getBound_r(), getBound_t())) return true;
		if (lineOnLineCol(x1, y1, x2, y2, getBound_l(), getBound_b(), getBound_r(), getBound_b())) return true;
		return false;
	}

	bool RightTriCollider::lineColliding (double x1, double y1, double x2, double y2) {
		if (lineOnLineCol(x1, y1, x2, y2, this->x, this->y, this->x2, this->y2)) return true;
		if (useTopside) {
			if (lineOnLineCol(x1, y1, x2, y2, this->x, getBound_b(), this->x2, getBound_b())) return true;
			if (lineOnLineCol(x1, y1, x2, y2, hailMath::min<double>(this->y2, this->y)==this->y?this->x:this->x2, this->y, hailMath::min<double>(this->y2, this->y)==this->y?this->x:this->x2, this->y2)) return true;
		} else {
			if (lineOnLineCol(x1, y1, x2, y2, this->x, getBound_t(), this->x2, getBound_t())) return true;
			if (lineOnLineCol(x1, y1, x2, y2, hailMath::max<double>(this->y2, this->y)==this->y?this->x:this->x2, this->y, hailMath::max<double>(this->y2, this->y)==this->y?this->x:this->x2, this->y2)) return true;
		}
		return false;
	}

	bool RightTriCollider::pointInCollider(double x, double y) {
		if ((x < getBound_r()) && (x > getBound_l()) && (y > getBound_t()) && (y < getBound_b())) {
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