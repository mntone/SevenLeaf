#pragma once

namespace sevenleaf {

class SnlfApp: public wxApp
{
public:
  virtual bool OnInit();
  virtual int OnExit();
};

}
