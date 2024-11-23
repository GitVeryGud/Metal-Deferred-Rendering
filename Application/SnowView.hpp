//
//  SnowView.hpp
//  MetalDeferredLighting
//
//  Created by Guilherme de Souza Barci on 26/08/24.
//

#ifndef SnowView_hpp
#define SnowView_hpp

#import <MetalKit/MetalKit.h>

#include "Input.hpp"

@interface SnowView : MTKView{
    Input* input; 
}

- (void)setInputManager : (Input*)input;

@end

#endif /* SnowView_hpp */
