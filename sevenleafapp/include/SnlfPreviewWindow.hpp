#ifndef _SNLF_PREVIEW_WINDOW_HPP
#define _SNLF_PREVIEW_WINDOW_HPP

#include <SnlfDisplayWindow.hpp>

namespace sevenleaf {

struct PreviewWindowGraphicsResources;

class wxDisplayPreviewWindow: public wxDisplayWindow {
public:
  wxDisplayPreviewWindow(wxWindow* parent,
                         wxWindowID id,
                         SnlfCoreRef core,
                         const wxPoint& pos = wxDefaultPosition,
                         const wxSize& size = wxDefaultSize,
                         long style = 0,
                         const wxString& name = wxPanelNameStr);
  
  virtual void DrawOverride(SnlfDisplayContext const *context) noexcept;
  
  void SetSource(SnlfSourceRef source) noexcept;
  
  void OnContextMenuSelected(wxCommandEvent& event);
  void OnContextMenu(wxContextMenuEvent& event);
  
private:
  void InitResources(SnlfGraphicsContext const* snlfGraphicsContext, CpsrDevice const* device) noexcept;
  void Update() noexcept;
  
private:
  std::unique_ptr<PreviewWindowGraphicsResources> resources_;
  CpsrSizeU32 savedDestinationSize_;
  
  SnlfSourceRef selectedSource_;

  wxDECLARE_EVENT_TABLE();
};

}  // namespace snlf

#endif // _SNLF_PREVIEW_WINDOW_HPP
