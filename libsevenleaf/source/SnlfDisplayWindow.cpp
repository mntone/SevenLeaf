#include <wx/wx.h>

#include "SnlfDisplayWindow.hpp"

#include <compositor/CpsrGraphics.hpp>
#include <compositor/vector/matrix4x4_t.h>

#include <geometry/SnlfPlaneGeometry.hpp>

namespace sevenleaf {

struct DisplayWindowGraphicsResources {
  compositor::GraphicsPipelineState pipelineState;
  compositor::UploadBuffer<matrix4x4_t, CPSR_CONSTANT_BUFFER> projectionTransform;
  sevenleaf::geometry::PlaneGeometry<1, 1, true> planeGeometry;
};

}

using namespace sevenleaf;

wxBEGIN_EVENT_TABLE(wxDisplayWindow, wxWindow)
  EVT_SIZE(wxDisplayWindow::OnSize)
  EVT_DPI_CHANGED(wxDisplayWindow::OnDPIChanged)
wxEND_EVENT_TABLE()

static inline CpsrSizeU32 GetSizeFromWxSize(wxSize const& wxSize) {
  CpsrSizeU32 size;
  size.width  = (uint32_t)wxSize.GetWidth();
  size.height = (uint32_t)wxSize.GetHeight();
  return size;
}

wxDisplayWindow::wxDisplayWindow(
  wxWindow *parent,
  wxWindowID id,
  SnlfCoreRef core,
  const wxPoint& pos,
  const wxSize& size,
  long style,
  const wxString& name)
  : wxWindow(parent, id, pos, size, style, name)
  , display_(nullptr)
  , resizeMode_(DisplayWindowResizeMode::AspectFit)
  , currentResizeMode_(DisplayWindowResizeMode::AspectFit)
  , padding_(SNLF_DISPLAY_DEFAULT_PADDING)
{
  CpsrViewHost viewHost;
  viewHost.handle      = (CpsrViewHandle)GetHandle();
  viewHost.size        = GetSizeFromWxSize(ConvertDialogToPixels(GetSize()));
  viewHost.pixelFormat = CPSR_PIXELFORMAT_RGB10A2_UNORM;
  
  SnlfDisplay *display = SnlfDisplayCreate(core, viewHost, wxDisplayWindow::DrawStatic, reinterpret_cast<intptr_t>(this));
  display_ = display;
}

wxDisplayWindow::~wxDisplayWindow() {
  if (display_) {
    SnlfDisplayDestroy(display_);
  }
}

void wxDisplayWindow::OnSize(wxSizeEvent& event) {
  if (display_) {
    SnlfDisplayChangeSize(display_, GetSizeFromWxSize(ConvertDialogToPixels(event.GetSize())));
  }
}

void wxDisplayWindow::OnDPIChanged(wxDPIChangedEvent& event) {
  if (display_) {
    SnlfDisplayChangeSize(display_, GetSizeFromWxSize(ConvertDialogToPixels(GetSize())));
  }
}

void wxDisplayWindow::_InitResources(SnlfGraphicsContext const* snlfGraphicsContext, CpsrDevice const* device) noexcept {
  resources_ = std::make_unique<DisplayWindowGraphicsResources>();
  
  auto& pipelineState = resources_->pipelineState;
  pipelineState.Init(device);
  pipelineState.SetVertexFunction(SnlfGraphicsContextGetShaderFunction(snlfGraphicsContext, SNLF_SHADER_VERTEX_DRAW));
  pipelineState.SetPixelFunction(SnlfGraphicsContextGetShaderFunction(snlfGraphicsContext, SNLF_SHADER_PIXEL_DRAW_HALF));
  pipelineState.SetPrimitiveTopologyType(compositor::PrimitiveTopologyType::Triangle);
  
  CpsrRasterizerDescriptor rasterizerDesc = kCpsrRasterizerDefault;
  rasterizerDesc.cullMode = CPSR_CULL_FRONT;
  pipelineState.SetRasterizerState(rasterizerDesc);
  
  resources_->projectionTransform.Init(device, "Projection Transform");
  resources_->planeGeometry.Init(device, 1.F, 1.F);
}

void wxDisplayWindow::_Update() noexcept {
  SnlfSizeF const sourceSize = sourceSize_;
  SnlfSizeF avaiableSize = destinationSize_;
  
  // Remove padding from avaiable size
  float scale = static_cast<float>(GetContentScaleFactor());
  SnlfThickness scaledPadding = scale * padding_;
  avaiableSize -= scaledPadding;
  
  SnlfSizeF previewSize;
  switch (currentResizeMode_) {
  case DisplayWindowResizeMode::DotByDot:
    previewSize = sourceSize;
    break;
      
  case DisplayWindowResizeMode::AspectFitScaleDownOnly:
    previewSize = sourceSize.AspectFit(avaiableSize, true);
    break;
        
  case DisplayWindowResizeMode::Fit:
    previewSize = avaiableSize;
    break;
          
  case DisplayWindowResizeMode::AspectFit:
  default:
    previewSize = sourceSize.AspectFit(avaiableSize);
    break;
  }
  previewSize_ = previewSize;
  
  SnlfPointF origin(scaledPadding.left, scaledPadding.top);
  origin += .5F * (avaiableSize - previewSize);
  origin_ = origin;
  centerPoint_ = origin + .5F * previewSize;
  
  // Update resources
  resources_->projectionTransform.data = matrix4x4_ortho_offcenter(0.F, sourceSize_.width, 0.F, sourceSize_.height, 0.F, 1.F);
  resources_->projectionTransform.Sync();
  resources_->planeGeometry.Update(sourceSize_.width, sourceSize_.height);
}

void wxDisplayWindow::DrawOverride(SnlfDisplayContext const *context) noexcept {
  SnlfGraphicsContext const *snlfGraphicsContext = SnlfDisplayContextGetGraphicsContext(context);
  CpsrDevice const *device = SnlfGraphicsContextGetDevice(snlfGraphicsContext);
  
  // Init resources if needed
  if (!resources_.get()) {
    _InitResources(snlfGraphicsContext, device);
  }
  
  bool needsUpdate = false;
  
  // Resize Mode
  if (resizeMode_ != currentResizeMode_) {
    currentResizeMode_ = resizeMode_;
    needsUpdate = true;
  }
  
  // Source
  CpsrTexture2D const *sourceTexture = SnlfDisplayContextGetSourceTexture(context);
  CpsrSizeU32 sourceSize = CpsrTexture2DGetSize(sourceTexture);
  if (sourceSize.width != sourceSize_.width || sourceSize.height != sourceSize_.height) {
    sourceSize_ = sourceSize;
    needsUpdate = true;
  }
  
  // Destination
  CpsrSwapChain *destination = SnlfDisplayContextGetSwapChain(context);
  CpsrSizeU32 destinationSize = CpsrSwapChainGetSize(destination);
  if (destinationSize.width != destinationSize_.width || destinationSize.height != destinationSize_.height) {
    destinationSize_ = destinationSize;
    needsUpdate = true;
  }
  
  // Update
  if (needsUpdate) {
    _Update();
  }
  
  // Set debug marker
  compositor::DebugMarker marker(SnlfGraphicsContextGetCommandBuffer(snlfGraphicsContext), "wxDisplayWindow::DrawOverride");

  CpsrSetRenderTargetPixelFormatFromSwapChain(resources_->pipelineState, 0, destination);
  
  compositor::GraphicsContext graphicsContext = SnlfGraphicsContextCreateGraphicsContext(snlfGraphicsContext);
  graphicsContext.ClearRenderTarget(0, kCpsrClearBlack);
  graphicsContext.SetRenderTarget(0, destination);
  if (!graphicsContext.SetPipelineState(resources_->pipelineState)) {
    CpsrViewport viewport;
    viewport.originX = origin_.x;
    viewport.originY = origin_.y;
    viewport.width = previewSize_.width;
    viewport.height = previewSize_.height;
    viewport.znear = 0.F;
    viewport.zfar = 1.F;
    graphicsContext.SetViewport(viewport);
    
    CpsrScissorRect scissorRect;
    scissorRect.x = floorf(origin_.x);
    scissorRect.y = floorf(origin_.y);
    scissorRect.width = ceilf(previewSize_.width);
    scissorRect.height = ceilf(previewSize_.height);
    graphicsContext.SetScissorRect(scissorRect);
    
    graphicsContext.SetVertexBuffer(0, resources_->planeGeometry.VertexBuffer());
    graphicsContext.SetConstantBuffer(0, resources_->projectionTransform);
    graphicsContext.SetTexture(0, sourceTexture);
    graphicsContext.SetPrimitiveTopology(compositor::PrimitiveTopology::Triangle);
    graphicsContext.SetIndexBuffer(resources_->planeGeometry.IndexBuffer(), compositor::IndexType::UInt16);
    graphicsContext.DrawIndexed(6);
    graphicsContext.Close();
  }
}

void wxDisplayWindow::DrawStatic(intptr_t param, SnlfDisplayContext const *context) noexcept {
  reinterpret_cast<wxDisplayWindow *>(param)->DrawOverride(context);
  SnlfDisplayContextPresent(context);
}

void wxDisplayWindow::SetResizeMode(DisplayWindowResizeMode resizeMode) noexcept {
  resizeMode_ = resizeMode;
}

void wxDisplayWindow::SetPadding(SnlfThickness padding) noexcept {
  padding_ = padding;
}

void wxDisplayWindow::SetActive(bool active) {
  active_ = active;
  
  if (display_) {
    if (active_) {
      SnlfDisplayBeginDraw(display_);
    } else {
      SnlfDisplayEndDraw(display_);
    }
  }
}
