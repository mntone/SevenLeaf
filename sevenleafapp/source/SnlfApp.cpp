#include <wx/splitter.h>
#include <wx/wx.h>

#include "SnlfApp.hpp"

#include <SnlfCore.h>
#include <SnlfModule.h>

#include "SnlfPreviewWindow.hpp"

using namespace sevenleaf;

wxIMPLEMENT_APP(SnlfApp);

class MyFrame: public wxFrame {
public:
  MyFrame();

  static void RootSourceChangedCallbackStatic(SnlfCoreRef sender, SnlfSourceChangedArgs args, intptr_t param) noexcept {
    reinterpret_cast<MyFrame *>(param)->RootSourceChangedCallback(sender, args);
  }

  static void RootSourceArrayChangedCallbackStatic(intptr_t sender, SnlfArrayChangedArgs args, intptr_t param) noexcept {
    reinterpret_cast<MyFrame *>(param)->RootSourceArrayChangedCallback(reinterpret_cast<SnlfCoreRef>(sender), args);
  }
  
protected:
  void AddSource(SnlfSourceRef source) noexcept;
  void SelectSource(SnlfSourceRef source) noexcept;
  
  void RootSourceChangedCallback(SnlfCoreRef sender, SnlfSourceChangedArgs args) noexcept;
  void RootSourceArrayChangedCallback(SnlfCoreRef sender, SnlfArrayChangedArgs args) noexcept;

private:
  void OnHello(wxCommandEvent const &event);
  void OnExit(wxCommandEvent const &event);
  void OnAbout(wxCommandEvent const &event);

  void OnShow(wxShowEvent &event);
  
  void OnRootSourceSelected(wxCommandEvent& event);
  void OnRootSourceAdd(wxCommandEvent& event);
  void OnRootSourceRemove(wxCommandEvent& event);
  
  void OnSourceSelected(wxCommandEvent& event);
  void OnSourceAddMenuSelected(wxCommandEvent& event);
  void OnSourceAdd(wxCommandEvent &event);
  void OnSourceRemove(wxCommandEvent &event);

private:
  wxDisplayPreviewWindow *preview_;
  wxListBox *rootSourceList_;
  wxButton *sourceGroupListBoxAddButton_;
  wxCheckListBox *sourceList_;
  wxButton *sourceListAddButton_;

  wxDECLARE_EVENT_TABLE();
};

static SnlfCoreRef gSnlfCore = nullptr;

#include <osutil.h>

bool SnlfApp::OnInit() {
  SnlfCoreStartupArgs args = {0};
  args.appPreferenceName = "org.mntone.SevenLeaf";  // TODO: SnlfAppConfig.h
  gSnlfCore = SnlfCoreInit(args);

  SnlfModuleLoadAll(gSnlfCore);
  
  MyFrame *frame = new MyFrame();
  frame->Show(true);
  return true;
}

int SnlfApp::OnExit() {
  assert(gSnlfCore);
  SnlfCoreUninit(gSnlfCore);
  gSnlfCore = nullptr;
  return 0;
}

enum {
  CONTROL_Display,

  CONTROL_SourceGroupList,
  BUTTON_SourceGroupAdd,
  BUTTON_SourceGroupRemove,

  CONTROL_SourceList,
  BUTTON_SourceAdd,
  BUTTON_SourceRemove,
  
  ID_SourceGroupListBegin = 0x100,
};

// clang-format on
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_SHOW(MyFrame::OnShow)

  EVT_LISTBOX(CONTROL_SourceGroupList, MyFrame::OnRootSourceSelected)
  EVT_BUTTON(BUTTON_SourceGroupAdd, MyFrame::OnRootSourceAdd)
  EVT_BUTTON(BUTTON_SourceGroupRemove, MyFrame::OnRootSourceRemove)

  EVT_LISTBOX(CONTROL_SourceList, MyFrame::OnSourceSelected)
  EVT_BUTTON(BUTTON_SourceAdd, MyFrame::OnSourceAdd)
  EVT_BUTTON(BUTTON_SourceRemove, MyFrame::OnSourceRemove)
wxEND_EVENT_TABLE()
// clang-format off

            enum {
              ID_Hello = 1
            };

#include <wx/toolbar.h>

static const char *const htmpage_xpm[] = {
    "16 16 4 1",        "   c None",        ".  c #808080",     "X  c Gray100",     "o  c Black",
    "                ", "  ..........    ", "  .XXXXXXXX..   ", "  .XXXXXXXXooo  ", "  .X......XXXo  ",
    "  .XXXXXXXXXXo  ", "  .X........Xo  ", "  .XXXXXXXXXXo  ", "  .X........Xo  ", "  .XXXXXXXXXXo  ",
    "  .X........Xo  ", "  .XXXXXXXXXXo  ", "  .X........Xo  ", "  .XXXXXXXXXXo  ", "  .XXXXXXXXXXo  ",
    "  oooooooooooo  "};

#include "wx/renderer.h"

MyFrame::MyFrame(): wxFrame(NULL, wxID_ANY, "SevenLeaf") {
  wxMenu *menuFile = new wxMenu;
  menuFile->Append(ID_Hello, "&Hello...\tCtrl-H", "Help string shown in status bar for this menu item");
  menuFile->AppendSeparator();
  menuFile->Append(wxID_EXIT);
  wxMenu *menuHelp = new wxMenu;
  menuHelp->Append(wxID_ABOUT);
  wxMenuBar *menuBar = new wxMenuBar;
  menuBar->Append(menuFile, "&File");
  menuBar->Append(menuHelp, "&Help");
  SetMenuBar(menuBar);
  CreateStatusBar();
  SetStatusText("Welcome to wxWidgets!");
  Bind(wxEVT_MENU, &MyFrame::OnHello, this, ID_Hello);
  Bind(wxEVT_MENU, &MyFrame::OnAbout, this, wxID_ABOUT);
  Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);

  wxSplitterWindow *splitterWindow =
      new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3DSASH);
  splitterWindow->SetSashGravity(0.9);
  splitterWindow->SetMinimumPaneSize(60);

  preview_ = new wxDisplayPreviewWindow(splitterWindow, CONTROL_Display, gSnlfCore);
  preview_->SetActive(true);

  wxPanel *toolPanel = new wxPanel(splitterWindow);

  // Source List begin
  rootSourceList_ =
      new wxListBox(toolPanel, CONTROL_SourceGroupList, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_SINGLE);
  wxSize minSize = rootSourceList_->GetMinSize();
  minSize.SetWidth(80);
  minSize.SetHeight(64);
  rootSourceList_->SetMinSize(minSize);

  sourceGroupListBoxAddButton_ = new wxButton(toolPanel, BUTTON_SourceGroupAdd, "Add");
  wxButton *sourceGroupListBoxRemoveButton = new wxButton(toolPanel, BUTTON_SourceGroupRemove, "Remove");
  wxBoxSizer *sourceGroupListBoxButtonGroup = new wxBoxSizer(wxHORIZONTAL);
  sourceGroupListBoxButtonGroup->Add(sourceGroupListBoxAddButton_, 1);
  sourceGroupListBoxButtonGroup->Add(sourceGroupListBoxRemoveButton, 1);

  wxBoxSizer *sourceGroupListBoxGroup = new wxBoxSizer(wxVERTICAL);
  sourceGroupListBoxGroup->Add(rootSourceList_, 1, wxEXPAND);
  sourceGroupListBoxGroup->Add(sourceGroupListBoxButtonGroup, 0);
  // Source List Buttons end

  // Source List begin
  sourceList_ = new wxCheckListBox(toolPanel, CONTROL_SourceList);
  minSize = sourceList_->GetMinSize();
  minSize.SetWidth(80);
  minSize.SetHeight(64);
  sourceList_->SetMinSize(minSize);

  sourceListAddButton_ = new wxButton(toolPanel, BUTTON_SourceAdd, "Add");
  wxButton *sourceListBoxRemoveButton = new wxButton(toolPanel, BUTTON_SourceRemove, "Remove");
  wxBoxSizer *sourceListBoxButtonGroup = new wxBoxSizer(wxHORIZONTAL);
  sourceListBoxButtonGroup->Add(sourceListAddButton_, 1);
  sourceListBoxButtonGroup->Add(sourceListBoxRemoveButton, 1);

  wxBoxSizer *sourceListBoxGroup = new wxBoxSizer(wxVERTICAL);
  sourceListBoxGroup->Add(sourceList_, 1, wxEXPAND);
  sourceListBoxGroup->Add(sourceListBoxButtonGroup, 0);
  // Source List Buttons end

  wxBoxSizer *horizontalBox = new wxBoxSizer(wxHORIZONTAL);
  horizontalBox->Add(sourceGroupListBoxGroup, 1, wxEXPAND);
  horizontalBox->Add(sourceListBoxGroup, 1, wxEXPAND);

  toolPanel->SetSizer(horizontalBox);
  horizontalBox->SetSizeHints(toolPanel);

  splitterWindow->SplitHorizontally(preview_, toolPanel);

  // Bind Root Frame <-> SplitterWindow
  wxBoxSizer *splitterSizer = new wxBoxSizer(wxVERTICAL);
  splitterSizer->Add(splitterWindow, 1, wxEXPAND);

  SetSizer(splitterSizer);
  splitterSizer->SetSizeHints(this);

  /*wxBitmap icon(htmpage_xpm, wxBITMAP_TYPE_XPM);
  wxToolBar *toolbar = CreateToolBar();
  toolbar->AddTool(wxID_ANY, wxT("New"), icon, icon);
  toolbar->Realize();*/
  
  SnlfTransitionAddNotificationHandler(
      gSnlfCore, MyFrame::RootSourceChangedCallbackStatic, reinterpret_cast<intptr_t>(this));
  SnlfCoreRootSourceAddNotificationHandler(
      gSnlfCore, MyFrame::RootSourceArrayChangedCallbackStatic, reinterpret_cast<intptr_t>(this));

  // Add Test source
  SnlfGraphicsGeneratorRef generator = nullptr;
  SnlfEnumGraphicsGenerators(gSnlfCore, [](SnlfGraphicsGeneratorRef generator, intptr_t param) {
    *reinterpret_cast<SnlfGraphicsGeneratorRef *>(param) = generator;
  }, reinterpret_cast<intptr_t>(&generator));
  SnlfSourceRef source = SnlfSourceCreateFromGraphicsGenerator(gSnlfCore, generator);
  
  matrix4x4_t transform = matrix4x4_mul(matrix4x4_sx(.5F), matrix4x4_tx(.5F));
  SnlfSourceSetTransform(source, transform);
  
  SnlfSourceRef rootSource = SnlfSourceCreate(gSnlfCore);
  SnlfSourceAppend(source, rootSource);
  
  SnlfRootSourceAppend(rootSource);
}

class wxSnlfSourceData final: public wxClientData {
public:
  inline wxSnlfSourceData(SnlfSourceRef source, bool noInc = false) noexcept: source_(source) {
    if (!noInc) {
      SnlfSourceAddRef(source);
    }
  }
  
  virtual ~wxSnlfSourceData() {
    SnlfSourceRelease(source_);
  }
  
  constexpr operator SnlfSourceRef() const noexcept { return source_; }
  
  wxSnlfSourceData(wxSnlfSourceData const& other) = delete;
  wxSnlfSourceData& operator=(wxSnlfSourceData const& other) = delete;
  
private:
  SnlfSourceRef source_;
};

void MyFrame::RootSourceChangedCallback(SnlfCoreRef sender, SnlfSourceChangedArgs args) noexcept {
  sourceList_->Clear();
  
  SnlfSourceEnumChildSources(args.currentSource, [](SnlfSourceRef source, intptr_t param) {
    MyFrame& that = *reinterpret_cast<MyFrame *>(param);
    that.AddSource(source);
  }, reinterpret_cast<intptr_t>(this));
}

void MyFrame::RootSourceArrayChangedCallback(SnlfCoreRef sender, SnlfArrayChangedArgs args) noexcept {
  switch (args.operation) {
    case SNLF_OPERATION_ADD: {
      uint32_t endIndex = args.newIndex + args.length;
      for (uint32_t index = args.newIndex; index != endIndex; ++index) {
        SnlfSourceRef source = SnlfRootSourceGetAt(gSnlfCore, index);
        wxSnlfSourceData *data = new wxSnlfSourceData(source, true);
        rootSourceList_->Insert("t", args.newIndex, data);
      }
      break;
    }

    case SNLF_OPERATION_REMOVE: {
      uint32_t endIndex = args.oldIndex + args.length;
      for (uint32_t index = args.oldIndex; index != endIndex; ++index) {
        rootSourceList_->Delete(index);
      }
      break;
    }

    case SNLF_OPERATION_MOVE:
      rootSourceList_->Move(args.oldIndex, args.newIndex);
      break;

    case SNLF_OPERATION_REPLACE:
      break;
  }
}

void MyFrame::OnExit(wxCommandEvent const &event) { Close(true); }

void MyFrame::OnAbout(wxCommandEvent const &event) {
  wxMessageBox("This is a wxWidgets' Hello world sample", "About Hello World", wxOK | wxICON_INFORMATION);
}

void MyFrame::OnHello(wxCommandEvent const &event) { wxLogMessage("Hello world from wxWidgets!"); }

void MyFrame::OnShow(wxShowEvent &event) { preview_->SetActive(event.IsShown()); }

void MyFrame::OnRootSourceSelected(wxCommandEvent& event) {
  wxSnlfSourceData *data = static_cast<wxSnlfSourceData *>(event.GetClientData());
  SnlfTransitionDispatch(gSnlfCore, *data);
  preview_->SetSource(nullptr);
}

void MyFrame::OnRootSourceAdd(wxCommandEvent& event) {
  SnlfSourceRef source = SnlfSourceCreate(gSnlfCore);
  if (!source) {
    wxString error("Out of memory");
    wxMessageBox(error);
  }

  if (SnlfRootSourceAppend(source)) {
    wxString error("Out of memory");
    wxMessageBox(error);
  }
}

void MyFrame::OnRootSourceRemove(wxCommandEvent &event) {
  unsigned int selectedIndex = rootSourceList_->GetSelection();
  if (selectedIndex == wxNOT_FOUND) {
    return;
  }

  wxSnlfSourceData *data = static_cast<wxSnlfSourceData *>(rootSourceList_->GetClientData(selectedIndex));
  SnlfSourceRemoveFromParent(*data);
}


// ---
// Source-related
// ---
void MyFrame::AddSource(SnlfSourceRef source) noexcept {
  wxSnlfSourceData *data = new wxSnlfSourceData(source, true);
  sourceList_->Append("D", data);
}

void MyFrame::SelectSource(SnlfSourceRef source) noexcept {
  preview_->SetSource(source);
}

void MyFrame::OnSourceSelected(wxCommandEvent& event) {
  auto source = static_cast<wxSnlfSourceData *>(event.GetClientData());
  SelectSource(*source);
}

void MyFrame::OnSourceAddMenuSelected(wxCommandEvent& event) {
  //event.GetId()
}

void MyFrame::OnSourceAdd(wxCommandEvent &event) {
  wxMenu menu;
  SnlfEnumInputs(gSnlfCore, [](SnlfInputRef input, intptr_t param) {
    auto& menu = *reinterpret_cast<wxMenu *>(param);
    auto id = SnlfInputGetIdentifier(input);
    auto name = SnlfInputGetFriendlyName(input);
    menu.Append(id, wxString::FromUTF8(name, strnlen(name, 64)));
    SnlfInputRelease(input);
  }, reinterpret_cast<intptr_t>(&menu));
  
  wxPoint offset = sourceListAddButton_->GetClientAreaOrigin();
  wxPoint position = ScreenToClient(sourceListAddButton_->GetScreenPosition());
  wxSize size = sourceListAddButton_->GetClientSize();
  menu.Connect(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyFrame::OnSourceAddMenuSelected), nullptr, this);
  PopupMenu(&menu, position.x - offset.x, position.y + size.GetHeight());
}

void MyFrame::OnSourceRemove(wxCommandEvent &event) {
  unsigned int selectedIndex = sourceList_->GetSelection();
  if (selectedIndex == wxNOT_FOUND) {
    return;
  }

  SnlfSourceRef source = static_cast<SnlfSourceRef>(sourceList_->GetClientData(selectedIndex));
  if (source) {
    SnlfSourceRemoveFromParent(source);

    sourceList_->DeselectAll();
  }
}
