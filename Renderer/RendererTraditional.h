

#ifndef RendererTraditional_h
#define RendererTraditional_h

#include "Renderer.h"

#include <Metal/Metal.hpp>

#include "Input.hpp"

class RendererTraditional : public Renderer
{
public:

    explicit RendererTraditional( MTL::Device* pDevice);

    virtual ~RendererTraditional() override;

    virtual void drawInView( bool isPaused, MTL::Drawable* pCurrentDrawable, MTL::Texture* pDepthStencilTexture ) override;

    virtual void drawableSizeWillChange(const MTL::Size& size, MTL::StorageMode GBufferStorageMode) override;

private:

    MTL::RenderPipelineState* m_pLightPipelineState;

    MTL::RenderPassDescriptor* m_pGBufferRenderPassDescriptor;
    MTL::RenderPassDescriptor* m_pFinalRenderPassDescriptor;

    void loadMetalInternal();

    void drawDirectionalLight(MTL::RenderCommandEncoder* renderEncoder);

    void drawPointLights(MTL::RenderCommandEncoder* renderEncoder);

};
#endif // RendererTraditional_h
