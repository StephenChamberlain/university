// OpenGL.h: interface for the COpenGL class.
//
//////////////////////////////////////////////////////////////////////



class COpenGL  
{
public:
	COpenGL();
	~COpenGL();

	/// SETUP FUNCTIONS ///
	void GLSetup(void);
	void InitPerspective(void);
	void PerspectiveViewingSystem(float, float, float,
							      float, float);

	/// DRAW FUNCTIONS ///
	void Point(float, float, float, int);

	void Line(float, float, float,
			  float, float, float);

	void Triangle(float, float, float,
				  float, float, float,
				  float, float, float);

	void Sphere(float, float, float, int);

	/// GL CALLBACK FUNCTIONS ///
	void Reshape(int, int);

	// VARIABLES ///
};

