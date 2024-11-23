

#include "ViewAdapter.h"
#import <MetalKit/MetalKit.h>
#include <QuartzCore/CAMetalDrawable.hpp>

ViewAdapter::ViewAdapter( void* pMTKView )
: m_pMTKView( pMTKView )
{
    
}

ViewAdapter::ViewAdapter( const ViewAdapter& rhs )
: m_pMTKView( rhs.m_pMTKView )
{
    
}

ViewAdapter& ViewAdapter::operator=( const ViewAdapter& rhs )
{
    m_pMTKView = rhs.m_pMTKView;
    return *this;
}

CA::MetalDrawable* ViewAdapter::currentDrawable() const
{
    return (__bridge CA::MetalDrawable*)[(__bridge MTKView *)m_pMTKView currentDrawable];
}

MTL::Texture* ViewAdapter::depthStencilTexture() const
{
    return (__bridge MTL::Texture*)[(__bridge MTKView *)m_pMTKView depthStencilTexture];
}

MTL::RenderPassDescriptor* ViewAdapter::currentRenderPassDescriptor() const
{
    return (__bridge MTL::RenderPassDescriptor*)[(__bridge MTKView *)m_pMTKView currentRenderPassDescriptor];
}

std::tuple<CGFloat, CGFloat> ViewAdapter::drawableSize() const
{
    CGSize size = [(__bridge MTKView *)m_pMTKView drawableSize];
    return std::make_tuple( size.width, size.height );
}

void ViewAdapter::draw()
{
    [(__bridge MTKView*)m_pMTKView draw];
}

void ViewAdapter::setHidden( bool hidden )
{
    [(__bridge MTKView*)m_pMTKView setHidden:hidden];
}

void ViewAdapter::setPaused( bool paused )
{
    [(__bridge MTKView*)m_pMTKView setPaused:paused];
}

void ViewAdapter::setColorPixelFormat( MTL::PixelFormat colorPixelFormat )
{
    [(__bridge MTKView*)m_pMTKView setColorPixelFormat:(MTLPixelFormat)colorPixelFormat];
}
