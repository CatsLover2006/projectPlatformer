namespace CollisionHandler {
	class Collider {
	public:
		double x, y;
		virtual void setLocation(double x, double y);
		virtual bool colliding (Collider other);
		virtual double getBound_t();
		virtual double getBound_b();
		virtual double getBound_l();
		virtual double getBound_r();
		Collider(double x, double y);
		Collider();
	};
	class BoxCollider: public Collider {
	public:
		double w, h;
		bool colliding (Collider other);
		bool pointInBox (double x, double y);
		BoxCollider(double x, double y, double w, double h);
		double getBound_t();
		double getBound_b();
		double getBound_l();
		double getBound_r();
	};
	class RightTriCollider: public Collider {
	public:
		double x2, y2;
		bool useTopside;
		bool colliding (Collider other);
		void setLocation(double x, double y);
		RightTriCollider(double x, double y, double x2, double y2, bool topTriangle);
		double getBound_t();
		double getBound_b();
		double getBound_l();
		double getBound_r();
	};
}