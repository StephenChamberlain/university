// Fractal.h: interface for the CFractal class.
//
//////////////////////////////////////////////////////////////////////

#define XPIXELS				768		// Number of pixels in X direction
#define YPIXELS				768

typedef struct {
	float red;
	float green;
	float blue;
} SFractalMem;	// Tank structure

class CFractal  
{
private:
	SFractalMem fractal[XPIXELS][YPIXELS];

public:
	void draw_fractal (void);
	void set_2d_display (void);

	CFractal();
	~CFractal();

};

