#include <wx/wx.h>

#include "SnlfPreviewWindow.hpp"

#include <array>
#include <memory>

#include <compositor/vector/matrix3x2_t.h>
#include <compositor/CpsrGraphics.hpp>

#include <geometry/SnlfCircleGeometry.hpp>
#include <geometry/SnlfRectangleGeometry.hpp>

namespace sevenleaf {

struct Uniforms {
  matrix4x4_t transform;
  SnlfColor color;
};

struct PreviewWindowGraphicsResources {
  compositor::GraphicsPipelineState pipelineState;
  compositor::UploadBuffer<Uniforms, CPSR_CONSTANT_BUFFER> uniforms;
  compositor::UploadBuffer<uint16x2_t[4], CPSR_CONSTANT_BUFFER> instanceUniforms;
  geometry::RectangleGeometry<1, 1> rectangleGeometry;
  geometry::CircleGeometry<12> circleGeometry;
};

enum {
  ID_RESIZE_MODE_ASPECT_FIT_SCALE_DOWN_ONLY = 1,
  ID_RESIZE_MODE_ASPECT_FIT,
  ID_RESIZE_MODE_FIT,
};

}

using namespace sevenleaf;

wxBEGIN_EVENT_TABLE(wxDisplayPreviewWindow, wxDisplayWindow)
  EVT_CONTEXT_MENU(wxDisplayPreviewWindow::OnContextMenu)
wxEND_EVENT_TABLE()

wxDisplayPreviewWindow::wxDisplayPreviewWindow(
  wxWindow *parent,
  wxWindowID id,
  SnlfCoreRef core,
  const wxPoint& pos,
  const wxSize& size,
  long style,
  const wxString& name)
  : wxDisplayWindow(parent, id, core, pos, size, style, name)
  , savedDestinationSize_()
  , selectedSource_(nullptr)
{ }

void wxDisplayPreviewWindow::InitResources(SnlfGraphicsContext const* snlfGraphicsContext, CpsrDevice const* device) noexcept {
  resources_ = std::make_unique<PreviewWindowGraphicsResources>();
  
  auto& pipelineState = resources_->pipelineState;
  pipelineState.Init(device);
  pipelineState.SetVertexFunction(SnlfGraphicsContextGetShaderFunction(snlfGraphicsContext, SNLF_SHADER_VERTEX_DRAW_COLOR_INSTANCED));
  pipelineState.SetPixelFunction(SnlfGraphicsContextGetShaderFunction(snlfGraphicsContext, SNLF_SHADER_PIXEL_DRAW_COLOR));
  pipelineState.SetPrimitiveTopologyType(compositor::PrimitiveTopologyType::Triangle);
  
  CpsrRasterizerDescriptor rasterizerDesc = kCpsrRasterizerDefault;
  rasterizerDesc.cullMode = CPSR_CULL_FRONT;
  pipelineState.SetRasterizerState(rasterizerDesc);
  
  resources_->uniforms.Init(device, "Uniforms (Transform | Color)");
  resources_->uniforms.Data().color = SnlfColor::RedColor();
  
  resources_->instanceUniforms.Init(device, "Instance Uniforms");
  
  float scale = static_cast<float>(GetContentScaleFactor());
  resources_->rectangleGeometry.Init(device, 100.F, 100.F, floorf(1.5F * scale));
  resources_->circleGeometry.Init(device, floorf(2.F * scale));
}

void wxDisplayPreviewWindow::Update() noexcept {
  auto destinationSize = DestinationSize();
  if (destinationSize.width != savedDestinationSize_.width
      || savedDestinationSize_.height != savedDestinationSize_.height) {
    resources_->uniforms.data.transform = matrix4x4_ortho_offcenter(0.F, destinationSize.width, destinationSize.height, 0.F, 0.F, 1.F);
    resources_->uniforms.Sync();
    savedDestinationSize_ = destinationSize;
  }
  
  CpsrSizeU32 sourceSize = SourceSize();
  SnlfPointF origin = Origin();
  
  float scaleX = static_cast<float>(destinationSize.width - 2.F * origin.x) / sourceSize.width;
  float scaleY = static_cast<float>(destinationSize.height - 2.F * origin.y) / sourceSize.height;
  matrix3x2_t matrix = matrix3x2_mul(matrix3x2_scale(scaleX, scaleY), matrix3x2_trans(origin.x, origin.y));
  
  auto& instanceUniforms = resources_->instanceUniforms.Data();
  instanceUniforms[0].x = 0;
  instanceUniforms[0].y = 0;
  instanceUniforms[0] = matrix3x2_transform_uint16(instanceUniforms[0], matrix);
  
  instanceUniforms[1].x = 1280;
  instanceUniforms[1].y = 0;
  instanceUniforms[1] = matrix3x2_transform_uint16(instanceUniforms[1], matrix);
  
  instanceUniforms[2].x = 10;
  instanceUniforms[2].y = 41;
  instanceUniforms[2] = matrix3x2_transform_uint16(instanceUniforms[2], matrix);
  
  instanceUniforms[3].x = 0;
  instanceUniforms[3].y = 41;
  instanceUniforms[3] = matrix3x2_transform_uint16(instanceUniforms[3], matrix);
  resources_->instanceUniforms.Sync();
}

void wxDisplayPreviewWindow::DrawOverride(SnlfDisplayContext const *context) noexcept {
  wxDisplayWindow::DrawOverride(context);
  
  if (!selectedSource_) {
    return;
  }
  
  SnlfGraphicsContext const *snlfGraphicsContext = SnlfDisplayContextGetGraphicsContext(context);
  CpsrDevice const *device = SnlfGraphicsContextGetDevice(snlfGraphicsContext);
  
  if (!resources_.get()) {
    InitResources(snlfGraphicsContext, device);
  }
  
  Update();
  
  // Set debug marker
  compositor::DebugMarker marker(SnlfGraphicsContextGetCommandBuffer(snlfGraphicsContext), "wxDisplayPreviewWindow::DrawOverride");
  
  // Get destination texture
  CpsrSwapChain *destinationTexture = SnlfDisplayContextGetSwapChain(context);
  
  // Setup pipeline
  CpsrSetRenderTargetPixelFormatFromSwapChain(resources_->pipelineState, 0, destinationTexture);
  
  compositor::GraphicsContext graphicsContext = SnlfGraphicsContextCreateGraphicsContext(snlfGraphicsContext);
  graphicsContext.SetRenderTarget(0, destinationTexture);
  if (!graphicsContext.SetPipelineState(resources_->pipelineState)) {
    graphicsContext.SetConstantBuffer(0, resources_->uniforms);
    graphicsContext.SetConstantBuffer(1, resources_->instanceUniforms);
    
    resources_->rectangleGeometry.Draw(graphicsContext);
    resources_->circleGeometry.DrawInstanced(graphicsContext, 4);
    graphicsContext.Close();
  }
}

void wxDisplayPreviewWindow::SetSource(SnlfSourceRef source) noexcept {
  SnlfSourceRef prevSource = selectedSource_;
  if (prevSource) {
    SnlfSourceRelease(prevSource);
    selectedSource_ = nullptr;
  }
  
  if (source) {
    SnlfSourceAddRef(source);
    selectedSource_ = source;
  }
}

void wxDisplayPreviewWindow::OnContextMenuSelected(wxCommandEvent& event) {
  switch (event.GetId()) {
  case ID_RESIZE_MODE_ASPECT_FIT_SCALE_DOWN_ONLY:
    SetResizeMode(DisplayWindowResizeMode::AspectFitScaleDownOnly);
    break;

  case ID_RESIZE_MODE_ASPECT_FIT:
    SetResizeMode(DisplayWindowResizeMode::AspectFit);
    break;
      
  case ID_RESIZE_MODE_FIT:
    SetResizeMode(DisplayWindowResizeMode::Fit);
    break;

  default: break;
  }
}

void wxDisplayPreviewWindow::OnContextMenu(wxContextMenuEvent& event) {
  wxMenu menu;
  
  auto resizeMode = ResizeMode();
  menu.Append(ID_RESIZE_MODE_ASPECT_FIT_SCALE_DOWN_ONLY,
              "Aspect fit (Scale down only)",
              wxEmptyString,
              resizeMode == DisplayWindowResizeMode::AspectFitScaleDownOnly ? wxITEM_RADIO : wxITEM_NORMAL);
  menu.Append(ID_RESIZE_MODE_ASPECT_FIT,
              "Aspect fit",
              wxEmptyString,
              resizeMode == DisplayWindowResizeMode::AspectFit ? wxITEM_RADIO : wxITEM_NORMAL);
  menu.Append(ID_RESIZE_MODE_FIT,
              "Fit",
              wxEmptyString,
              resizeMode == DisplayWindowResizeMode::Fit ? wxITEM_RADIO : wxITEM_NORMAL);
  menu.Connect(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(wxDisplayPreviewWindow::OnContextMenuSelected), nullptr, this);
  PopupMenu(&menu);
}
