#pragma once

#include <vector>

/*
DO NOT MODIFY THIS FILE !
Representation of a mesh
*/
class Mesh
{
public:
    Mesh() = default;
    ~Mesh() = default;

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    // This method sets the number of nodes that are needed in the mesh.
    // You can only call this method once and you have to call it before adding nodes
    void SetNumberNodes(unsigned int iNbrNodes) { _Nodes.resize(iNbrNodes); };
    unsigned int GetNumberNodes() {return _Nodes.size();}

    // This method sets the number of QUADs that are needed in the mesh.
    // You can only call this once and you have to call it before adding QUADs
    void SetNumberQUADs(unsigned int iNbrQUADs) { _QUADs.resize(iNbrQUADs);  };
    unsigned int GetNumberQUADs() {return _QUADs.size();}

    // Set a node on index iIndex. 
    // The range is [0, numbernodes-1]
    void SetNode(unsigned int iIndex, double iX, double iY, double iZ);

    // Set a QUAD on indec iIndex
    // The range is [0, numberquads-1]
    void SetQUAD(unsigned int iIndex, int iNode1, int iNode2, int iNode3, int iNode4);

   // Output the first 100 elements and nodes to the console
    void Print();
private:

    struct Node {
        double _X;
        double _Y;
        double _Z;
    };
    struct QUAD {
        int _Node1;
        int _Node2;
        int _Node3;
        int _Node4;
    };

    std::vector<Node> _Nodes;
    std::vector<QUAD> _QUADs;
};

