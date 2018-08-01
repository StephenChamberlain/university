// Tree.h: interface for the CTree class.
//
//////////////////////////////////////////////////////////////////////

// Tree data, including orientation. This ensures that the trees dont all
// line up and makes it feel a bit more realistic.

class CTree  
{
private:
	float x;
	float y;
	float z;

	float orientation;

public:
	CTree();
	~CTree();

};

