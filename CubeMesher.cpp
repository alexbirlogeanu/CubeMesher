// CubeMesher.cpp : Defines the entry point for the console application.
//

#include <cstdio>
#include <tchar.h>

#include "Mesh.h"
#include "Cube.h"
#include "CubeBuilder.h"

// Exercise: fill in the method CreateMesh.
// This method must mesh a cube. There are a couple rules underlying the meshing:
// - You may only use shell elements with a connectivity of 4 (QUAD element)
// - Only the outside of the cube has to be meshed. There may not be any interior nodes or elements
// - Of course, nodes have to be shared when needed, also on the corners and edges of the cube
// - Specifying 1000 nodes in x,y and z direction, should execute within a 10 seconds on a 2GHZ computer in optimized compilation
// Additionally, there are some rules to follow:
// - You may not modify the code in Mesh.h , Mesh.cpp, Cube.h and Cube.cpp
// - It is allowed to add new files
// - The _tmain method should stay like it is, except that you may change the parameters for the cube and the mesh for experimentation purposes
// This method takes a input:
//  - a Cube with certain dimensions
//  - the number of nodes to create in x/y/z direction
//  - An object that represents the mesh
// You have to call methods on the ioMesh object, such that the ioMesh is a correct representation
// of the meshed cube. Please see the header file of the mesh to see which methods it supports and how 
// the nodes and elements are represented in the mesh.
void CreateMesh(const Cube& iCube, int iNbrNodesX, int iNbrNodesY, int iNbrNodesZ, Mesh& ioMesh)
{
	CubeBuilder builder(iCube);

	builder.BuildMesh(ioMesh, iNbrNodesX, iNbrNodesY, iNbrNodesZ);
}

int _tmain(int argc, _TCHAR* argv[])
{
    // This represents the design of a cube with dimensions 1x1x1 meter
    Cube lCube (1, 1, 1); 

    // This object will hold the mesh
    Mesh lMesh; 

    // Create the mesh
    CreateMesh(lCube, 4, 4, 4, lMesh);

    // Print the Mesh
    // The output of the Print method will be limited to 100 elements and 100 nodes, because
    // otherwise it might take an hour before the output is printed
    lMesh.Print();

	return 0;
}


