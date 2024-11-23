#import "ViewController.h"

#include "RendererTraditional.h"
#include "RendererSinglePass.h"

#include "ViewAdapter.h"
#include "SnowView.hpp"


@implementation ViewController
{
    __weak IBOutlet SnowView* _view;

    Renderer* _pRenderer;

    MTL::Device* _pDevice;
    
    Input* input;
}

- (void)viewDidLoad
{
    [super viewDidLoad];

    _pDevice = MTL::CreateSystemDefaultDevice();
    
    input = new Input;

    NSAssert(_pDevice, @"Metal is not supported on this device");

    // Set the view to use the default device
    SnowView *mtkView = (SnowView *)self.view;
    
    [mtkView setInputManager:input];

    mtkView.device = (__bridge id<MTLDevice>)_pDevice;
    mtkView.delegate = self;

    bool useSinglePassDeferred = false;

    if(_pDevice->supportsFamily(MTL::GPUFamilyApple1))
    {
        useSinglePassDeferred = false;
    }

    if(useSinglePassDeferred)
    {
        _pRenderer = new RendererSinglePass( _pDevice );
    }
    else
    {
        _pRenderer = new RendererTraditional( _pDevice );
    }
    
    _pRenderer->input = input;
    _view.depthStencilPixelFormat = (MTLPixelFormat)_pRenderer->depthStencilTargetPixelFormat();
    _view.colorPixelFormat = (MTLPixelFormat)_pRenderer->colorTargetPixelFormat();

    NSAssert(_pRenderer, @"Renderer failed initialization");

    CGSize size = _view.drawableSize;
    _pRenderer->drawableSizeWillChange( MTL::Size::Make(size.width, size.height, 0), MTL::StorageModePrivate);
}

- (void)_setupView:(MTKView *)mtkView withDevice:(id<MTLDevice>)device
{
    mtkView.device = device;
    
    // Other setup steps can be implemented here
}

- (void)dealloc
{
    delete _pRenderer;
    _pDevice->release();
}

- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size
{
    MTL::Size newSize = MTL::Size::Make(size.width, size.height, 0);
    _pRenderer->drawableSizeWillChange(newSize, MTL::StorageModePrivate);
    
    if(_view.paused)
    {
        [_view draw];
    }
}

- (void)drawInMTKView:(nonnull MTKView *)view
{
    _pRenderer->drawInView(_view.paused,
                           (__bridge MTL::Drawable *)_view.currentDrawable,
                           (__bridge MTL::Texture *)_view.depthStencilTexture);
}

- (BOOL)prefersHomeIndicatorAutoHidden
{
    return YES;
}

- (void)viewDidAppear
{
    // Make the view controller the window's first responder so that it can handle the Key events
    [_view.window makeFirstResponder:self];
}

@end
