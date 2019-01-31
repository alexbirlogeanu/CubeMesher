#pragma once

#include <vector>
#include <utility>

#include "Cube.h"

class Mesh;

//utility for cube builder

enum class FaceType
{
	Left,
	Right,
	Top,
	Bottom,
	Front,
	Back,
	Count
};

/*
	equivalent of the Mesh::QUAD that keeps a list of indexes
*/
class CubeShell
{
public:
	CubeShell();
	~CubeShell() = default;

	void					AddNodeIndex(int index);
	const std::vector<int>&	GetIndexes() const { return _Indexes; }
private:
	std::vector<int> _Indexes;
};

/*
	A cube face keeps only a list of shells and a shell keeps info about the nodes
*/
class CubeFace
{
public:
	CubeFace();
	CubeFace(int wDivisions, int hDivisions, FaceType type);
	~CubeFace() = default;

	const std::vector<CubeShell>& GetShells() const { return _Shells; }

	//we do a bunch of checks before adding not to go outside of the grid
	void AddNodeIndexToShell(int index, int x, int y, int z);
	void AddIndexToShell(int index, int w, int h);

	int	GetNumberOfShells() const { return _Shells.size(); };

private:
	//get relevant components based on face type. Every face has a component that is constant. Ex: for left face all the nodes will have the component x = 0
	//pair.first - width, pair.second - height
	std::pair<int, int>	GetRelevantComponents(int x, int y, int z) const;
	CubeShell&			GetShell(int w, int h); //a face is a 2D structure so we need only 2 components

private:
	std::vector<CubeShell>		_Shells;
	FaceType					_Type;
	int							_WidthDivs;
	int							_HeightDivs;
};

class CubeBuilder
{
public:
	CubeBuilder(const Cube& cube);
	~CubeBuilder() = default;

	void BuildMesh(Mesh& outMesh, int xNodes, int yNodes, int zNodes);
private:
	struct BuildData
	{
		Mesh* OutputMesh = nullptr;
		int XNodes = 0;
		int YNodes = 0;
		int ZNodes = 0;
	};

	void StartBuild(Mesh& outMesh, int xNodes, int yNodes, int zNodes);
	void InitFaces();
	void GetAdjacentFaces(int x, int y, int z, std::vector<FaceType>& faceTypes) const;

	//we separate the nodes int 3 categories: corners (only 8 of them), edge (the nodes that are common for 2 faces) and inner face(nodes that are inside the face)
	//the reason is that the input data can create a considerable number of nodes (eg: x, y, z > 1000) and try to remove some if-s inside the for loops and increase performance

	void CreateCornerNodes();
	void CreateEdgeNodes();
	void CreateInnerFaceNodes();

	/*a node is indentify by the divisions of the XYZ axes, with x E [0, xDivisions - 1], y E [0, yDivisions - 1], etc
	the nodes are "numbered" in this order:
	
				(0, y, z)			(x, y, z)
				   ____________________
				  /|				  /|
				(0, y, 0) 		(x, y, 0)
				/__|________________/  |
				|  |				|  |
	^  ^		|  |				|  |
	| /Z		|(0, 0, z)			|  |
   Y|/			|  |_ _ _ _ _ _ _ _ |_ |
	|----->		|  /				|  / (x, 0, z)
		X		| /					| /
				|/__________________|/
			 (0, 0, 0)          (x, 0, 0)
	
		
	*/
	//TODO Maybe create a struct that encapsulates x, y, z data
	//this method creates a node that is found in multiples faces
	void AddEdgeNode(int x, int y, int z);
	//this method creates a node that is found in one face only
	void AddInnerFaceNode(int x, int y, int z, FaceType fType);

	void CopyShells();
private:
	Cube						_Cube;
	
	//following variables are initialized when we start to build the mesh

	//this list will be filled when the building of the mesh starts
	std::vector<CubeFace>		_Faces;
	// keep current index. When we create a node, immediately add to the mesh and avoid a latter copy operation
	uint32_t					_CurrentIndex;
	BuildData					_BuildData;
	//stride on X,Y,Z axis
	double						_XStride;
	double						_YStride;
	double						_ZStride;
};
