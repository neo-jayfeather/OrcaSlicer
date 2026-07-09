#ifndef slic3r_GUI_StaticGroup_hpp_
#define slic3r_GUI_StaticGroup_hpp_

#include "../wxExtensions.hpp"

#include "LabeledStaticBox.hpp"

class StaticGroup : public LabeledStaticBox
{
public:
    StaticGroup(wxWindow *parent, wxWindowID id);
    void ShowBadge(bool show);
    bool Show(bool show=true) override;
private:
    void DrawBorderAndLabel(wxDC& dc) override;
    ScalableBitmap badge;
};

#endif // !slic3r_GUI_StaticGroup_hpp_
