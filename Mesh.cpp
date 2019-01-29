#include "Mesh.h"
#include <algorithm>
#include <iostream>

void Mesh::SetNode(unsigned int iIndex, double iX, double iY, double iZ)
{
    int lResult = -1;
    if (iIndex < _Nodes.size() && iIndex >=0 ) 
    {
        _Nodes[iIndex] = { iX, iY, iZ };
    }
    else 
    {
        std::cout << "Wrong index" << std::endl;
    }
}

void Mesh::SetQUAD(unsigned int iIndex, int iNode1, int iNode2, int iNode3, int iNode4)
{
    if (iIndex < _QUADs.size() && iIndex>=0) 
    {
        _QUADs[iIndex] = { iNode1, iNode2, iNode3, iNode4 };
    }
    else 
    {
        std::cout << "The table to store the QUADs is already full" << std::endl;
    }
}

void Mesh::Print()
{
    // Print number nodes & quads
    std::cout << "Number nodes: " << _Nodes.size() << ", number QUADs: " << _QUADs.size() << std::endl;

    const unsigned int maxItems = 100;
    for (size_t idxNode = 0; idxNode < std::min(maxItems, _Nodes.size()); idxNode++)
    {
        std::cout << "Node " << idxNode << " = (" << _Nodes[idxNode]._X << ", " << _Nodes[idxNode]._Y << ", " << _Nodes[idxNode]._Z << ")" << std::endl;
    }

    for (size_t idxElem = 0; idxElem < std::min(maxItems, _QUADs.size()); idxElem++)
    {
        std::cout << "Element " << idxElem << " = (" 
            << _QUADs[idxElem]._Node1 << ", " 
            << _QUADs[idxElem]._Node2 << ", " 
            << _QUADs[idxElem]._Node3 << ", " 
            << _QUADs[idxElem]._Node4 << ")" 
            << std::endl;

        std::cout << "    Node " << _QUADs[idxElem]._Node1 << " = (" << _Nodes[_QUADs[idxElem]._Node1]._X << ", " << _Nodes[_QUADs[idxElem]._Node1]._Y << ", " << _Nodes[_QUADs[idxElem]._Node1]._Z << ")" << std::endl;
        std::cout << "    Node " << _QUADs[idxElem]._Node2 << " = (" << _Nodes[_QUADs[idxElem]._Node2]._X << ", " << _Nodes[_QUADs[idxElem]._Node2]._Y << ", " << _Nodes[_QUADs[idxElem]._Node2]._Z << ")" << std::endl;
        std::cout << "    Node " << _QUADs[idxElem]._Node3 << " = (" << _Nodes[_QUADs[idxElem]._Node3]._X << ", " << _Nodes[_QUADs[idxElem]._Node3]._Y << ", " << _Nodes[_QUADs[idxElem]._Node3]._Z << ")" << std::endl;
        std::cout << "    Node " << _QUADs[idxElem]._Node4 << " = (" << _Nodes[_QUADs[idxElem]._Node4]._X << ", " << _Nodes[_QUADs[idxElem]._Node4]._Y << ", " << _Nodes[_QUADs[idxElem]._Node4]._Z << ")" << std::endl;
    }
}
