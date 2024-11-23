

#ifndef RendererSinglePass_h
#define RendererSinglePass_h

#include "Renderer.h"

#include "Input.hpp"

class RendererSinglePass : public Renderer
{
public:

    explicit RendererSinglePass( MTL::Device* pDevice );
    
    virtual ~RendererSinglePass() override;

    virtual void drawInView( bool isPaused, MTL::Drawable* pCurrentDrawable, MTL::Texture* pDepthStencilTexture ) override;
    
    virtual void drawableSizeWillChange(const MTL::Size& size, MTL::StorageMode GBufferStorageMode) override;

private:

    void loadMetalInternal();

    void drawDirectionalLight(MTL::RenderCommandEncoder* pRenderEncoder);

    void drawPointLights(MTL::RenderCommandEncoder* pRenderEncoder);

    MTL::RenderPipelineState* m_pLightPipelineState;

    MTL::RenderPassDescriptor* m_pViewRenderPassDescriptor;

    MTL::StorageMode m_GBufferStorageMode;
    
    MTL::Size m_drawableSize;
};

#endif // RendererSinglePass_h
