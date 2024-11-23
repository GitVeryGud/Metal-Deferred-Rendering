
#ifndef VIEWADAPTER_H
#define VIEWADAPTER_H

#include <Metal/MTLPixelFormat.hpp>
#include <tuple>
#include <CoreGraphics/CGBase.h>

namespace CA
{
    class MetalDrawable;
}

namespace MTL
{
    class Texture;
    class RenderPassDescriptor;
}

class ViewAdapter
{
public:
    explicit ViewAdapter( void* pMTKView );
    
    ViewAdapter( const ViewAdapter& );
    ViewAdapter& operator=( const ViewAdapter& );
    
    ViewAdapter( ViewAdapter&& ) = delete;
    ViewAdapter& operator=( ViewAdapter&& ) = delete;
    
    CA::MetalDrawable* currentDrawable() const;
    MTL::Texture* depthStencilTexture() const;
    MTL::RenderPassDescriptor* currentRenderPassDescriptor() const;
    std::tuple<CGFloat, CGFloat> drawableSize() const;
    
    void draw();
    
    void setHidden( bool hidden );
    void setPaused( bool paused );
    void setColorPixelFormat( MTL::PixelFormat );
    
private:
    void *m_pMTKView;
};

#endif //VIEWADAPTER_H
