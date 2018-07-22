// Frustum.h: interface for the CFrustum class.
//
//////////////////////////////////////////////////////////////////////

// Controls frustum culling by calculating it and providing collision
// response to point, sphere and cube in frustum.

class CFrustum 
{
private:

	float m_Frustum[6][4];

public:

	CFrustum();
	~CFrustum();

	void CalculateFrustum();

	bool PointInFrustum (float x, float y, float z);
	bool SphereInFrustum (float x, float y, float z, float radius);
	bool CubeInFrustum (float x, float y, float z, float size );
};
