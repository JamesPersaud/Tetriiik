//Flying fragment
class D3DTetrisFragment
{
public:
		int Birthday;
        float Lifespan;
        float X;
        float Y;
        float Z;
		float StartX;
        float StartY;
        float StartZ;
        float XVelocity;
        float YVelocity;
        float ZVelocity;
        float Yaw;
        float Pitch;
        float Roll;
        int Colour;
        bool falling;

		D3DTetrisFragment(float x, float y, float z, float xv, float yv, float zv, float yaw, float pitch, float roll, float lifespan, int col, int birthday);
		
		void Update(int now, float gravity);
			
};