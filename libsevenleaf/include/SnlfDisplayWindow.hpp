#ifndef _SNLF_DISPLAY_WINDOW_HPP
#define _SNLF_DISPLAY_WINDOW_HPP

#include "SnlfDisplay.h"

#ifndef __cplusplus
#error This header requires C++
#endif

namespace sevenleaf {

enum class DisplayWindowResizeMode {
  DotByDot,
  AspectFitScaleDownOnly,
  AspectFit,
  Fit,
};

struct DisplayWindowGraphicsResources;

class wxDisplayWindow: public wxWindow {
public:
  wxDisplayWindow(wxWindow* parent,
                  wxWindowID id,
                  SnlfCoreRef core,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = 0,
                  const wxString& name = wxPanelNameStr);
  virtual ~wxDisplayWindow();
  
  virtual void OnSize(wxSizeEvent& event);
  virtual void OnDPIChanged(wxDPIChangedEvent& event);

  virtual void DrawOverride(SnlfDisplayContext const *context) noexcept;
  static void DrawStatic(intptr_t, SnlfDisplayContext const *context) noexcept;
  
  DisplayWindowResizeMode ResizeMode() const noexcept { return resizeMode_; }
  void SetResizeMode(DisplayWindowResizeMode resizeMode) noexcept;
  
  SnlfThickness Padding() const noexcept { return padding_; }
  void SetPadding(SnlfThickness padding) noexcept;
  
  bool IsActive() const noexcept { return active_; }
  void SetActive(bool active);
  
  CpsrSizeU32 SourceSize() const noexcept { return sourceSize_; }
  CpsrSizeU32 DestinationSize() const noexcept { return destinationSize_; }
  SnlfPointF Origin() const noexcept { return origin_; }
  
private:
  void _InitResources(SnlfGraphicsContext const* snlfGraphicsContext, CpsrDevice const* device) noexcept;
  void _Update() noexcept;

private:
  bool active_;
  SnlfDisplay *display_;
  DisplayWindowResizeMode resizeMode_;
  
  DisplayWindowResizeMode currentResizeMode_;
  CpsrSizeU32 sourceSize_, destinationSize_;
  SnlfPointF origin_, centerPoint_;
  SnlfSizeF previewSize_;
  SnlfThickness padding_;
  
  std::unique_ptr<DisplayWindowGraphicsResources> resources_;

  wxDECLARE_EVENT_TABLE();
};

}  // namespace snlf

#endif // _SNLF_DISPLAY_WINDOW_HPP
