

#include "RendererTraditional.h"

// Include header shared between C code here, which executes Metal API commands, and .metal files.
#include "ShaderTypes.h"

#include "Utilities.h"


RendererTraditional::RendererTraditional(MTL::Device* pDevice)
: Renderer( pDevice )
{
    m_singlePassDeferred = false;
    loadMetalInternal();
    loadScene();
}

RendererTraditional::~RendererTraditional()
{
    m_pLightPipelineState->release();
    m_pGBufferRenderPassDescriptor->release();
    m_pFinalRenderPassDescriptor->release();
}

/// Create traditional deferred renderer specific Metal state objects
void RendererTraditional::loadMetalInternal()
{
    Renderer::loadMetal();

    NS::Error* pError = nullptr;


    #pragma mark Point light render pipeline setup
    {
        MTL::RenderPipelineDescriptor* pRenderPipelineDescriptor = MTL::RenderPipelineDescriptor::alloc()->init();

        pRenderPipelineDescriptor->colorAttachments()->object(RenderTargetLighting)->setPixelFormat( colorTargetPixelFormat() );

        // Enable additive blending
        pRenderPipelineDescriptor->colorAttachments()->object(RenderTargetLighting)->setBlendingEnabled( true );
        pRenderPipelineDescriptor->colorAttachments()->object(RenderTargetLighting)->setRgbBlendOperation( MTL::BlendOperationAdd );
        pRenderPipelineDescriptor->colorAttachments()->object(RenderTargetLighting)->setAlphaBlendOperation( MTL::BlendOperationAdd );
        pRenderPipelineDescriptor->colorAttachments()->object(RenderTargetLighting)->setDestinationRGBBlendFactor( MTL::BlendFactorOne );
        pRenderPipelineDescriptor->colorAttachments()->object(RenderTargetLighting)->setDestinationAlphaBlendFactor( MTL::BlendFactorOne );
        pRenderPipelineDescriptor->colorAttachments()->object(RenderTargetLighting)->setSourceRGBBlendFactor( MTL::BlendFactorOne );
        pRenderPipelineDescriptor->colorAttachments()->object(RenderTargetLighting)->setSourceAlphaBlendFactor( MTL::BlendFactorOne );

        pRenderPipelineDescriptor->setDepthAttachmentPixelFormat( depthStencilTargetPixelFormat() );
        pRenderPipelineDescriptor->setStencilAttachmentPixelFormat( depthStencilTargetPixelFormat() );

        MTL::Library* pShaderLibrary = m_pDevice->newDefaultLibrary();

        AAPL_ASSERT( pShaderLibrary, "Failed to create default shader library" );

        MTL::Function* pLightVertexFunction = pShaderLibrary->newFunction( AAPLSTR( "deferred_point_lighting_vertex" ) );
        MTL::Function* pLightFragmentFunction = pShaderLibrary->newFunction( AAPLSTR( "deferred_point_lighting_fragment_traditional" ) );

        AAPL_ASSERT( pLightVertexFunction, "Failed to load deferred_point_lighting_vertex" );
        AAPL_ASSERT( pLightFragmentFunction, "Failed to load deferred_point_lighting_fragment_traditional" );

        pRenderPipelineDescriptor->setVertexFunction( pLightVertexFunction );
        pRenderPipelineDescriptor->setFragmentFunction( pLightFragmentFunction );

        m_pLightPipelineState = m_pDevice->newRenderPipelineState( pRenderPipelineDescriptor, &pError );

        AAPL_ASSERT_NULL_ERROR( pError, "Failed to create lighting render pipeline state" );
        
        pLightVertexFunction->release();
        pLightFragmentFunction->release();
        pRenderPipelineDescriptor->release();
        pShaderLibrary->release();
    }

    #pragma mark GBuffer render pass descriptor setup
    // Create a render pass descriptor to create an encoder for rendering to the GBuffers.
    // The encoder stores rendered data of each attachment when encoding ends.
    m_pGBufferRenderPassDescriptor = MTL::RenderPassDescriptor::alloc()->init();
    
    m_pGBufferRenderPassDescriptor->colorAttachments()->object(RenderTargetLighting)->setLoadAction( MTL::LoadActionDontCare );
    m_pGBufferRenderPassDescriptor->colorAttachments()->object(RenderTargetLighting)->setStoreAction( MTL::StoreActionDontCare );
    m_pGBufferRenderPassDescriptor->colorAttachments()->object(RenderTargetAlbedo)->setLoadAction( MTL::LoadActionDontCare );
    m_pGBufferRenderPassDescriptor->colorAttachments()->object(RenderTargetAlbedo)->setStoreAction( MTL::StoreActionStore );
    m_pGBufferRenderPassDescriptor->colorAttachments()->object(RenderTargetNormal)->setLoadAction( MTL::LoadActionDontCare );
    m_pGBufferRenderPassDescriptor->colorAttachments()->object(RenderTargetNormal)->setStoreAction( MTL::StoreActionStore );
    m_pGBufferRenderPassDescriptor->colorAttachments()->object(RenderTargetDepth)->setLoadAction( MTL::LoadActionDontCare );
    m_pGBufferRenderPassDescriptor->colorAttachments()->object(RenderTargetDepth)->setStoreAction( MTL::StoreActionStore );
    m_pGBufferRenderPassDescriptor->depthAttachment()->setClearDepth( 1.0 );
    m_pGBufferRenderPassDescriptor->depthAttachment()->setLoadAction( MTL::LoadActionClear );
    m_pGBufferRenderPassDescriptor->depthAttachment()->setStoreAction( MTL::StoreActionStore );

    m_pGBufferRenderPassDescriptor->stencilAttachment()->setClearStencil( 0 );
    m_pGBufferRenderPassDescriptor->stencilAttachment()->setLoadAction( MTL::LoadActionClear );
    m_pGBufferRenderPassDescriptor->stencilAttachment()->setStoreAction( MTL::StoreActionStore );

    // Create a render pass descriptor for the lighting and composition pass

    // Whatever rendered in the final pass needs to be stored so it can be displayed
    m_pFinalRenderPassDescriptor = MTL::RenderPassDescriptor::alloc()->init();
    m_pFinalRenderPassDescriptor->colorAttachments()->object(0)->setStoreAction( MTL::StoreActionStore );
    m_pFinalRenderPassDescriptor->depthAttachment()->setLoadAction( MTL::LoadActionLoad );
    m_pFinalRenderPassDescriptor->stencilAttachment()->setLoadAction( MTL::LoadActionLoad );
}

/// Respond to view size change
void RendererTraditional::drawableSizeWillChange(const MTL::Size& size, MTL::StorageMode GBufferStorageMode)
{
    // The renderer base class allocates all GBuffers >except< lighting GBuffer (since with the
    // single-pass deferred renderer the lighting buffer is the same as the drawable)
    Renderer::drawableSizeWillChange( size, GBufferStorageMode );

    // Re-set GBuffer textures in the GBuffer render pass descriptor after they have been
    // reallocated by a resize
    m_pGBufferRenderPassDescriptor->colorAttachments()->object(RenderTargetAlbedo)->setTexture( m_albedo_specular_GBuffer );
    m_pGBufferRenderPassDescriptor->colorAttachments()->object(RenderTargetNormal)->setTexture( m_normal_shadow_GBuffer );
    m_pGBufferRenderPassDescriptor->colorAttachments()->object(RenderTargetDepth)->setTexture( m_depth_GBuffer );
}

/// Draw directional lighting, which, with a tradition deferred renderer needs to set GBuffers as
/// textures before executing common rendering code to draw the light
void RendererTraditional::drawDirectionalLight(MTL::RenderCommandEncoder* pRenderEncoder)
{
    pRenderEncoder->setFragmentTexture( m_albedo_specular_GBuffer, RenderTargetAlbedo );
    pRenderEncoder->setFragmentTexture( m_normal_shadow_GBuffer, RenderTargetNormal );
    pRenderEncoder->setFragmentTexture( m_depth_GBuffer, RenderTargetDepth );

    Renderer::drawDirectionalLightCommon( pRenderEncoder );
}

/// Setup traditional deferred rendering specific pipeline and set GBuffer textures.  Then call
/// common renderer code to apply the point lights
void RendererTraditional::drawPointLights(MTL::RenderCommandEncoder* pRenderEncoder)
{

    pRenderEncoder->setRenderPipelineState( m_pLightPipelineState );

    pRenderEncoder->setFragmentTexture( m_albedo_specular_GBuffer, RenderTargetAlbedo );
    pRenderEncoder->setFragmentTexture( m_normal_shadow_GBuffer, RenderTargetNormal );
    pRenderEncoder->setFragmentTexture( m_depth_GBuffer, RenderTargetDepth );

    // Call common base class method after setting state in the renderEncoder specific to the
    // traditional deferred renderer
    Renderer::drawPointLightsCommon( pRenderEncoder );

}

/// Frame drawing routine
void RendererTraditional::drawInView( bool isPaused, MTL::Drawable* pCurrentDrawable, MTL::Texture* pDepthStencilTexture )
{
    {
        MTL::CommandBuffer* pCommandBuffer = Renderer::beginFrame( isPaused );

        Renderer::drawShadow( pCommandBuffer );

        m_pGBufferRenderPassDescriptor->depthAttachment()->setTexture( pDepthStencilTexture );
        m_pGBufferRenderPassDescriptor->stencilAttachment()->setTexture( pDepthStencilTexture );

        MTL::RenderCommandEncoder* pRenderEncoder = pCommandBuffer->renderCommandEncoder( m_pGBufferRenderPassDescriptor );

        Renderer::drawGBuffer( pRenderEncoder );

        pRenderEncoder->endEncoding();

        pCommandBuffer->commit();
    }

    {
        MTL::CommandBuffer* pCommandBuffer = Renderer::beginDrawableCommands();

        MTL::Texture* pDrawableTexture = Renderer::currentDrawableTexture( pCurrentDrawable );

        // The final pass can only render if a drawable is available, otherwise it needs to skip
        // rendering this frame.
        if( pDrawableTexture )
        {
            // Render the lighting and composition pass

            m_pFinalRenderPassDescriptor->colorAttachments()->object(0)->setTexture( pDrawableTexture );
            m_pFinalRenderPassDescriptor->depthAttachment()->setTexture( pDepthStencilTexture );
            m_pFinalRenderPassDescriptor->stencilAttachment()->setTexture( pDepthStencilTexture );

            MTL::RenderCommandEncoder* pRenderEncoder = pCommandBuffer->renderCommandEncoder( m_pFinalRenderPassDescriptor );

            drawDirectionalLight( pRenderEncoder );

            Renderer::drawPointLightMask( pRenderEncoder );

            drawPointLights( pRenderEncoder );

            Renderer::drawSky( pRenderEncoder );

            Renderer::drawFairies( pRenderEncoder );

            pRenderEncoder->endEncoding();
        }

        Renderer::endFrame( pCommandBuffer, pCurrentDrawable );
    }
}


