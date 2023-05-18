#include "collider.hpp"
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

	bool Collider::colliding(Collider other) {
		return false;
	}

	bool BoxCollider::colliding(Collider other) {
		if (typeid(other) == typeid(RightTriCollider)) return other.colliding(*this);
		BoxCollider otherBox = *((BoxCollider*)&other);
		return ((std::abs((x + w/2) - (otherBox.x + otherBox.w/2)) * 2 < (w + otherBox.w)) &&
         		(std::abs((y + h/2) - (otherBox.y + otherBox.h/2)) * 2 < (h + otherBox.h)));
	}

	bool BoxCollider::pointInBox(double x, double y) {
		return (x > this->x && x < this->x + this->w) && (y > this->y && y < this->y + this->h);
	}

	bool RightTriCollider::colliding(Collider other) {
		if (typeid(other) == typeid(RightTriCollider)) {
			RightTriCollider otherTri = *((RightTriCollider*)&other);
			if ((std::abs((x + x2)/2 - (otherTri.x + otherTri.x2)/2) * 2 < (std::abs(x2-x) + std::abs(otherTri.x2-otherTri.x))) &&
	         	(std::abs((y + y2)/2 - (otherTri.y + otherTri.y2)/2) * 2 < (std::abs(y2-y) + std::abs(otherTri.y2-otherTri.y)))) {
				// Too lazy rn
			}
			return false;
		}
		BoxCollider otherBox = *((BoxCollider*)&other);
		if ((std::abs((x + x2)/2 - (otherBox.x + otherBox.w/2)) * 2 < (std::abs(x2-x) + otherBox.w)) &&
         	(std::abs((y + y2)/2 - (otherBox.y + otherBox.h/2)) * 2 < (std::abs(y2-y) + otherBox.h))) {
			double cX = otherBox.x;
			double cY = otherBox.y;
			if (useTopside) {
				if (cY > lerp(min(this->y2, this->y), max(this->y2, this->y), (cX - this->x) / std::abs(this->x - this->x2))) return true;
				cX += otherBox.w;
				if (cY > lerp(min(this->y2, this->y), max(this->y2, this->y), (cX - this->x) / std::abs(this->x - this->x2))) return true;
				cY += otherBox.h;
				if (cY > lerp(min(this->y2, this->y), max(this->y2, this->y), (cX - this->x) / std::abs(this->x - this->x2))) return true;
				cX -= otherBox.w;
				if (cY > lerp(min(this->y2, this->y), max(this->y2, this->y), (cX - this->x) / std::abs(this->x - this->x2))) return true;
			} else {
				if (cY < lerp(min(this->y2, this->y), max(this->y2, this->y), (cX - this->x) / std::abs(this->x - this->x2))) return true;
				cX += otherBox.w;
				if (cY < lerp(min(this->y2, this->y), max(this->y2, this->y), (cX - this->x) / std::abs(this->x - this->x2))) return true;
				cY += otherBox.h;
				if (cY < lerp(min(this->y2, this->y), max(this->y2, this->y), (cX - this->x) / std::abs(this->x - this->x2))) return true;
				cX -= otherBox.w;
				if (cY < lerp(min(this->y2, this->y), max(this->y2, this->y), (cX - this->x) / std::abs(this->x - this->x2))) return true;
			}
		}
		return false;
	}
}