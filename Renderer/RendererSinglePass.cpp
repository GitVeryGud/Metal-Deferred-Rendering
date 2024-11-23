
#include "RendererSinglePass.h"

// Include header shared between C code here, which executes Metal API commands, and .metal files
#include "Shaders/ShaderTypes.h"

#include "Utilities.h"

RendererSinglePass::RendererSinglePass( MTL::Device* pDevice )
: Renderer( pDevice )
{
    m_singlePassDeferred = true;

    m_GBufferStorageMode = MTL::StorageModeMemoryless;

    loadMetalInternal();
    loadScene();
}

RendererSinglePass::~RendererSinglePass()
{
    m_pLightPipelineState->release();
    m_pViewRenderPassDescriptor->release();
}

void RendererSinglePass::loadMetalInternal()
{
    Renderer::loadMetal();
    NS::Error* pError = nullptr;

    #pragma mark Point light render pipeline setup
    {
        MTL::RenderPipelineDescriptor* pRenderPipelineDescriptor = MTL::RenderPipelineDescriptor::alloc()->init();

        pRenderPipelineDescriptor->colorAttachments()->object(RenderTargetLighting)->setPixelFormat(colorTargetPixelFormat());
        pRenderPipelineDescriptor->colorAttachments()->object(RenderTargetAlbedo)->setPixelFormat(m_albedo_specular_GBufferFormat);
        pRenderPipelineDescriptor->colorAttachments()->object(RenderTargetNormal)->setPixelFormat(m_normal_shadow_GBufferFormat);
        pRenderPipelineDescriptor->colorAttachments()->object(RenderTargetDepth)->setPixelFormat(m_depth_GBufferFormat);
        pRenderPipelineDescriptor->setDepthAttachmentPixelFormat(depthStencilTargetPixelFormat());
        pRenderPipelineDescriptor->setStencilAttachmentPixelFormat(depthStencilTargetPixelFormat());

        MTL::Library* pShaderLibrary = m_pDevice->newDefaultLibrary();

        AAPL_ASSERT( pShaderLibrary, "Failed to create default shader library" );

        MTL::Function* pLightVertexFunction = pShaderLibrary->newFunction( AAPLSTR( "deferred_point_lighting_vertex" ) );
        MTL::Function* pLightFragmentFunction = pShaderLibrary->newFunction( AAPLSTR( "deferred_point_lighting_fragment_single_pass" ) );

        AAPL_ASSERT( pLightVertexFunction, "Failed to load deferred_point_lighting_vertex" );
        AAPL_ASSERT( pLightFragmentFunction, "Failed to load deferred_point_lighting_fragment_single_pass" );

        pRenderPipelineDescriptor->setVertexFunction( pLightVertexFunction );
        pRenderPipelineDescriptor->setFragmentFunction( pLightFragmentFunction );

        m_pLightPipelineState = m_pDevice->newRenderPipelineState( pRenderPipelineDescriptor, &pError );

        AAPL_ASSERT_NULL_ERROR( pError, "Failed to create lighting render pipeline state" );
        
        pLightVertexFunction->release();
        pLightFragmentFunction->release();
        pRenderPipelineDescriptor->release();
        pShaderLibrary->release();
    }

    #pragma mark GBuffer + View render pass descriptor setup
    m_pViewRenderPassDescriptor = MTL::RenderPassDescriptor::alloc()->init();
    m_pViewRenderPassDescriptor->colorAttachments()->object(RenderTargetAlbedo)->setLoadAction(MTL::LoadActionDontCare);
    m_pViewRenderPassDescriptor->colorAttachments()->object(RenderTargetAlbedo)->setStoreAction(MTL::StoreActionDontCare);
    m_pViewRenderPassDescriptor->colorAttachments()->object(RenderTargetNormal)->setLoadAction(MTL::LoadActionDontCare);
    m_pViewRenderPassDescriptor->colorAttachments()->object(RenderTargetNormal)->setStoreAction(MTL::StoreActionDontCare);
    m_pViewRenderPassDescriptor->colorAttachments()->object(RenderTargetDepth)->setLoadAction(MTL::LoadActionDontCare);
    m_pViewRenderPassDescriptor->colorAttachments()->object(RenderTargetDepth)->setStoreAction(MTL::StoreActionDontCare);
    m_pViewRenderPassDescriptor->depthAttachment()->setLoadAction( MTL::LoadActionClear );
    m_pViewRenderPassDescriptor->depthAttachment()->setStoreAction( MTL::StoreActionDontCare) ;
    m_pViewRenderPassDescriptor->stencilAttachment()->setLoadAction( MTL::LoadActionClear );
    m_pViewRenderPassDescriptor->stencilAttachment()->setStoreAction( MTL::StoreActionDontCare );
    m_pViewRenderPassDescriptor->depthAttachment()->setClearDepth( 1.0 );
    m_pViewRenderPassDescriptor->stencilAttachment()->setClearStencil( 0 );

}

/// Respond to view size change
void RendererSinglePass::drawableSizeWillChange(const MTL::Size& size, MTL::StorageMode GBufferStorageMode)
{
    m_drawableSize = size;
    
    // The renderer base class allocates all GBuffers except lighting GBuffer, because with the
    // single-pass deferred renderer, the lighting buffer is the same as the drawable.
    Renderer::drawableSizeWillChange(size, m_GBufferStorageMode);

    // Re-set GBuffer textures in the GBuffer render pass descriptor after they have been
    // reallocated by a resize
    m_pViewRenderPassDescriptor->colorAttachments()->object(RenderTargetAlbedo)->setTexture(m_albedo_specular_GBuffer);
    m_pViewRenderPassDescriptor->colorAttachments()->object(RenderTargetNormal)->setTexture(m_normal_shadow_GBuffer);
    m_pViewRenderPassDescriptor->colorAttachments()->object(RenderTargetDepth)->setTexture(m_depth_GBuffer);
}

/// Frame drawing routine
void RendererSinglePass::drawInView( bool isPaused, MTL::Drawable* pCurrentDrawable, MTL::Texture* pDepthStencilTexture )
{
    MTL::CommandBuffer* pCommandBuffer = beginFrame( isPaused );

    drawShadow( pCommandBuffer );
    pCommandBuffer->commit();

    pCommandBuffer = beginDrawableCommands();

    MTL::Texture* pDrawableTexture = currentDrawableTexture( pCurrentDrawable );
    if ( pDrawableTexture )
    {
        m_pViewRenderPassDescriptor->colorAttachments()->object(RenderTargetLighting)->setTexture( pDrawableTexture );
        m_pViewRenderPassDescriptor->depthAttachment()->setTexture( pDepthStencilTexture );
        m_pViewRenderPassDescriptor->stencilAttachment()->setTexture( pDepthStencilTexture );

        MTL::RenderCommandEncoder* pRenderEncoder = pCommandBuffer->renderCommandEncoder(m_pViewRenderPassDescriptor);

        Renderer::drawGBuffer( pRenderEncoder );

        drawDirectionalLight( pRenderEncoder );

        Renderer::drawPointLightMask( pRenderEncoder );

        drawPointLights( pRenderEncoder );

        Renderer::drawSky( pRenderEncoder );

        Renderer::drawFairies( pRenderEncoder );

        pRenderEncoder->endEncoding();
    }

    endFrame( pCommandBuffer, pCurrentDrawable );
}

void RendererSinglePass::drawDirectionalLight(MTL::RenderCommandEncoder* pRenderEncoder)
{

    Renderer::drawDirectionalLightCommon( pRenderEncoder );

}

void RendererSinglePass::drawPointLights(MTL::RenderCommandEncoder* pRenderEncoder)
{

    pRenderEncoder->setRenderPipelineState( m_pLightPipelineState );

    // Call the common base class method after setting the state in the renderEncoder specific to the
    // single-pass deferred renderer
    Renderer::drawPointLightsCommon( pRenderEncoder );

}
