//
//  SnowView.m
//  MetalDeferredLighting-macOS
//
//  Created by Guilherme de Souza Barci on 26/08/24.
//

#import <Foundation/Foundation.h>
#include "SnowView.hpp"

@implementation SnowView

- (void)setInputManager:(Input *)input {
    self->input = input;
}

- (void)keyDown:(NSEvent *)event {
    int key;
    
    if (event.type == NSEventTypeKeyDown) {
        key = (int)event.keyCode;
        
        if (self->input->pressedKeys[key]) {
            if (self->input->justPressedKeys[key] == true) {
                self->input->justPressedKeys[key] = false;
            }
        } else {
            self->input->pressedKeys[key] = true;
            self->input->justPressedKeys[key] = true;
            self->input->justReleasedKeys[key] = false;
        }
    }
}

- (void)keyUp:(NSEvent *)event {
    int key;
    
    if (event.type == NSEventTypeKeyUp) {
        key = (int)event.keyCode;
        if (self->input->justReleasedKeys[key]) {
            self->input->justReleasedKeys[key] = false;
        } else {
            self->input->pressedKeys[key] = false;
            self->input->justPressedKeys[key] = false;
            self->input->justReleasedKeys[key] = true;
        }
    }
}

- (void)mouseMoved:(NSEvent *)event {
    if (event.type == NSEventTypeMouseMoved) {
        
        input->mousePosition.x = NSEvent.mouseLocation.x;
        input->mousePosition.y = NSEvent.mouseLocation.y;
        
        input->mouseDelta.x = event.deltaX;
        input->mouseDelta.y = event.deltaY;
    }
}

- (void)mouseUp:(NSEvent *)event {
    input->leftMouseState = false;
}

- (void)mouseDown:(NSEvent *)event {
    input->leftMouseState = true;
}

- (void)rightMouseUp:(NSEvent *)event {
    input->rightMouseState = false;
}

- (void)rightMouseDown:(NSEvent *)event {
    input->rightMouseState = true;
}

- (BOOL)acceptsFirstResponder
{
    return YES;
}

@end