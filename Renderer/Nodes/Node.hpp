//
//  Node.hpp
//  SnowSoup
//
//  Created by Guilherme de Souza Barci on 03/09/24.
//

#ifndef Node_hpp
#define Node_hpp

#include <stdio.h>
#include <Metal/Metal.hpp>

class Node {
public:
    char name[1024];
    Node* parent;
    Node* children[256];
    int childrenCount;
    bool isTransform = false;
    
    Node();
    
    void AddChild(Node* child);
    virtual void Draw( MTL::RenderCommandEncoder* pEnc);
    virtual void Update();
    
    virtual void DrawDeferred (MTL::RenderCommandEncoder* pEnc);
};

#endif /* Node_hpp */
