#include "CubeBuilder.h"

#include <cassert>
#include <type_traits>
#include <windows.h>
#include <iostream>

#include "Mesh.h"

#define START_PROFILLING(section) { \
						ULONGLONG startTime = GetTickCount64();

#define END_PROFILLING(section) ULONGLONG endTime = GetTickCount64(); \
						ULONGLONG elapsedTime = endTime - startTime; \
						std::cout << "Elapsed time for section: " << #section << " is " << elapsedTime / 1000 << " sec and " <<  elapsedTime % 1000 << " ms" << std::endl; \
					}

//courtesy of StackOverflow
template<class C>
typename std::underlying_type<C>::type underlying_cast(C o)
{
	return std::underlying_type<C>::type(o);
}

///////////////////////////////////////////////////
//CubeShell
///////////////////////////////////////////////////
CubeShell::CubeShell()
{
	_Indexes.reserve(4);
}

void CubeShell::AddNodeIndex(int index)
{
	assert((_Indexes.size() <= 4) && "Shell already has 4 indexes added");
	assert((std::find(_Indexes.begin(), _Indexes.end(), index) == _Indexes.end()) && "Index is already added to the shell");

	_Indexes.push_back(index);
}

///////////////////////////////////////////////////
//CubeFace
///////////////////////////////////////////////////
CubeFace::CubeFace()
	: _Type(FaceType::Count)
{
	
}

CubeFace::CubeFace(int wDivisions, int hDivisions, FaceType type)
	: _WidthDivs(wDivisions)
	, _HeightDivs(hDivisions)
	, _Type(type)
{
	_Shells.resize(_WidthDivs * _HeightDivs);
}

void CubeFace::AddNodeIndexToShellSafe(int index, int x, int y, int z)
{
	auto components = GetRelevantComponents(x, y, z);
	auto addIndex = [&](int w, int h)
	{
		if (w >= 0 && w < _WidthDivs && h >= 0 && h < _HeightDivs)
			AddIndexToShell(index, w, h);
	};

	addIndex(components.first, components.second); //add to right top shell
	addIndex(components.first - 1, components.second); //add to left top shell
	addIndex(components.first, components.second - 1); //add to right bot shell
	addIndex(components.first - 1, components.second - 1); //add to left bot shell
}

void  CubeFace::AddNodeIndexToShellFast(int index, int x, int y, int z)
{
	//in this method we dont check if x,y,z are out of bounds
	auto components = GetRelevantComponents(x, y, z);
	AddIndexToShell(index, components.first, components.second); //add to right top shell
	AddIndexToShell(index, components.first - 1, components.second); //add to left top shell
	AddIndexToShell(index, components.first, components.second - 1); //add to right bot shell
	AddIndexToShell(index, components.first - 1, components.second - 1); //add to left bot shell
}

void CubeFace::AddIndexToShell(int index, int w, int h)
{
	CubeShell& shell = GetShell(w, h);
	shell.AddNodeIndex(index);
}

CubeShell& CubeFace::GetShell(int w, int h) //a face is a 2D structure so we need only 2 components 
{
	assert(w < _WidthDivs);
	assert(h < _HeightDivs);
	//the shell is identified by the position in the grid. (w, h) is the bottom-right corner of the shell
	unsigned int index = h * _WidthDivs + w; //the index for shell list
	
	return _Shells[index];
}

std::pair<int, int> CubeFace::GetRelevantComponents(int x, int y, int z) const
{
	switch (_Type)
	{
	case FaceType::Left:
	case FaceType::Right:
		return std::pair<int, int>(z, y);
	case FaceType::Top:
	case FaceType::Bottom:
		return std::pair<int, int>(x, z);
	case FaceType::Front:
	case FaceType::Back:
		return std::pair<int, int>(x, y);
	default:
		assert(false);
	}
	return std::pair<int, int>(-1, -1);
}

///////////////////////////////////////////////////
//CubeBuilder
///////////////////////////////////////////////////

CubeBuilder::CubeBuilder(const Cube& cube)
	: _Cube(cube)
{

}

void CubeBuilder::BuildMesh(Mesh& outMesh, int xNodes, int yNodes, int zNodes)
{
	START_PROFILLING("CreateMesh")
	
	START_PROFILLING("Allocate memory")
	StartBuild(outMesh, xNodes, yNodes, zNodes);
	END_PROFILLING("Allocate memory");

	START_PROFILLING("Corner and Edge Nodes")
	CreateCornerNodes();
	CreateEdgeNodes();
	END_PROFILLING("Corner and Edge Nodes");
	
	START_PROFILLING("Inner Face Nodes")
	CreateInnerFaceNodes();
	END_PROFILLING("Inner Face Nodes");

	assert(outMesh.GetNumberNodes() == _CurrentIndex && "Not all nodes have been created");

	START_PROFILLING("Copy quads")
	CopyShells();
	END_PROFILLING("Copy quads");

	//clear data
	_Faces.clear();
	END_PROFILLING("CreateMesh");
}

void CubeBuilder::StartBuild(Mesh& outMesh, int xNodes, int yNodes, int zNodes)
{
	_BuildData = { &outMesh, xNodes, yNodes, zNodes };
	_CurrentIndex = 0;
	unsigned int totalNodes = (xNodes * yNodes * zNodes) - (xNodes - 2) * (yNodes - 2) * (zNodes - 2);
	outMesh.SetNumberNodes(totalNodes);

	_XStride = _Cube.GetX() / (double)(xNodes - 1);
	_YStride = _Cube.GetY() / (double)(yNodes - 1);
	_ZStride = _Cube.GetZ() / (double)(zNodes - 1);

	InitFaces();
}

void CubeBuilder::InitFaces()
{
	auto addFace = [&](int wNodes, int hNodes, FaceType type) 
	{
		_Faces[underlying_cast(type)] = CubeFace(wNodes - 1, hNodes - 1, type); //there are - 1 divisions per row than nodes 
	};

	_Faces.resize(underlying_cast(FaceType::Count));
	addFace(_BuildData.ZNodes, _BuildData.YNodes, FaceType::Left);
	addFace(_BuildData.ZNodes, _BuildData.YNodes, FaceType::Right);
	addFace(_BuildData.XNodes, _BuildData.ZNodes, FaceType::Top);
	addFace(_BuildData.XNodes, _BuildData.ZNodes, FaceType::Bottom);
	addFace(_BuildData.XNodes, _BuildData.YNodes, FaceType::Front);
	addFace(_BuildData.XNodes, _BuildData.YNodes, FaceType::Back);
}

void CubeBuilder::CreateCornerNodes()
{
	AddEdgeNode(0, 0, 0);
	AddEdgeNode(_BuildData.XNodes - 1, 0, 0);
	AddEdgeNode(_BuildData.XNodes - 1, 0, _BuildData.ZNodes - 1);
	AddEdgeNode(0, 0, _BuildData.ZNodes - 1);
	AddEdgeNode(0, _BuildData.YNodes - 1, 0);
	AddEdgeNode(_BuildData.XNodes - 1, _BuildData.YNodes - 1, 0);
	AddEdgeNode(_BuildData.XNodes - 1, _BuildData.YNodes - 1, _BuildData.ZNodes - 1);
	AddEdgeNode(0, _BuildData.YNodes - 1, _BuildData.ZNodes - 1);
}

void CubeBuilder::CreateEdgeNodes()
{
	//we create the nodes on the edges parallel with X axis
	for (int x = 1; x < _BuildData.XNodes - 1; ++x) //exclude the corner nodes
	{
		AddEdgeNode(x, 0, 0); //bottom - front face edge
		AddEdgeNode(x, _BuildData.YNodes - 1, 0); //top - front face edge
		AddEdgeNode(x, 0, _BuildData.ZNodes - 1); //bottom - back face edge
		AddEdgeNode(x, _BuildData.YNodes - 1, _BuildData.ZNodes - 1); //top - back face edge
	}

	//we create the nodes on the edges parallel with Y axis
	for (int y = 1; y < _BuildData.YNodes - 1; ++y)
	{
		AddEdgeNode(0, y, 0); //right - left face edge
		AddEdgeNode(0, y, _BuildData.ZNodes - 1); //left - left face edge
		AddEdgeNode(_BuildData.XNodes - 1, y, 0); //left - right face edge
		AddEdgeNode(_BuildData.XNodes - 1, y, _BuildData.ZNodes - 1); //right - right face edge
	}

	//we create the nodes on edges parallel with Z axis
	for (int z = 1; z < _BuildData.ZNodes - 1; ++z)
	{
		AddEdgeNode(0, 0, z); //right bottom face edge
		AddEdgeNode(_BuildData.XNodes - 1, 0, z); //left bottom face edge
		AddEdgeNode(0, _BuildData.YNodes - 1, z);// left top face edge
		AddEdgeNode(_BuildData.XNodes - 1, _BuildData.YNodes - 1, z); //right top face edge
	}
}

void CubeBuilder::CreateInnerFaceNodes()
{
	//front and back face
	for (int i = 1; i < _BuildData.XNodes - 1; ++i)
		for (int j = 1; j < _BuildData.YNodes - 1; ++j)
		{
			AddInnerFaceNode(i, j, 0, FaceType::Front);
			AddInnerFaceNode(i, j, _BuildData.ZNodes - 1, FaceType::Back);
		}

	//left and right face
	for (int i = 1; i < _BuildData.YNodes - 1; ++i)
		for (int j = 1; j < _BuildData.ZNodes - 1; ++j)
		{
			AddInnerFaceNode(0, i, j, FaceType::Left);
			AddInnerFaceNode(_BuildData.XNodes - 1, i, j, FaceType::Right);
		}
	//bottom and top face
	for (int i = 1; i < _BuildData.XNodes - 1; ++i)
		for (int j = 1; j < _BuildData.ZNodes - 1; ++j)
		{
			AddInnerFaceNode(i, 0, j, FaceType::Bottom);
			AddInnerFaceNode(i, _BuildData.YNodes - 1, j, FaceType::Top);
		}
}

void CubeBuilder::AddEdgeNode(int x, int y, int z)
{
	Mesh* mesh = _BuildData.OutputMesh;
	mesh->SetNode(_CurrentIndex, x * _XStride, y * _YStride, z * _ZStride);
	
	std::vector<FaceType> faceTypes;
	GetAdjacentFaces(x, y, z, faceTypes);
	
	for (unsigned int i = 0; i < faceTypes.size(); ++i)
	{
		CubeFace& face = _Faces[underlying_cast(faceTypes[i])];
		face.AddNodeIndexToShellSafe(_CurrentIndex, x, y, z);
	}

	++_CurrentIndex;
}

void CubeBuilder::AddInnerFaceNode(int x, int y, int z, FaceType fType)
{
	Mesh* mesh = _BuildData.OutputMesh;
	mesh->SetNode(_CurrentIndex, x * _XStride, y * _YStride, z * _ZStride);

	CubeFace& face = _Faces[underlying_cast(fType)];
	face.AddNodeIndexToShellFast(_CurrentIndex, x, y, z);
	++_CurrentIndex;
}

void CubeBuilder::CopyShells()
{
	Mesh* mesh = _BuildData.OutputMesh;

	int numberOfQuads = 0;
	for (const auto& face : _Faces)
		numberOfQuads += face.GetNumberOfShells();

	mesh->SetNumberQUADs(numberOfQuads);

	int quadIndex = 0;
	for (const auto& face : _Faces)
	{
		const auto& shells = face.GetShells();
		for (const auto& shell : shells)
		{
			const auto& indexes = shell.GetIndexes();
			assert(indexes.size() == 4);
			//indexes are kinda chaotic. i should change put them in an order (CCW or CC)
			mesh->SetQUAD(quadIndex++, indexes[0], indexes[1], indexes[2], indexes[3]);
		}
	}
}

void CubeBuilder::GetAdjacentFaces(int x, int y, int z, std::vector<FaceType>& faceTypes) const
{
	assert(x >= 0 && x < _BuildData.XNodes);
	assert(y >= 0 && y < _BuildData.YNodes);
	assert(z >= 0 && z < _BuildData.ZNodes);
	
	if (x == 0)
		faceTypes.push_back(FaceType::Left);
	if (x == _BuildData.XNodes - 1)
		faceTypes.push_back(FaceType::Right);
	if (y == 0)
		faceTypes.push_back(FaceType::Bottom);
	if (y == _BuildData.YNodes - 1)
		faceTypes.push_back(FaceType::Top);
	if (z == 0)
		faceTypes.push_back(FaceType::Front);
	if (z == _BuildData.ZNodes - 1)
		faceTypes.push_back(FaceType::Back);
}
