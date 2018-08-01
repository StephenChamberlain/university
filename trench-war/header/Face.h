// Face.h: interface for the CFace class.
//
//////////////////////////////////////////////////////////////////////

// Face data, contains a reference to three points which make up a polygon

class CFace  
{
private:
	int vertex_one;
	int vertex_two;
	int vertex_three;

public:
	void set_vertex_one(int);
	void set_vertex_two(int);
	void set_vertex_three(int);

	int get_vertex_one(void);
	int get_vertex_two(void);
	int get_vertex_three(void);

	CFace();
	~CFace();

};
