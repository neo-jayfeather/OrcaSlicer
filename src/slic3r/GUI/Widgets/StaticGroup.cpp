#include "StaticGroup.hpp"
#include "Label.hpp"

StaticGroup::StaticGroup(wxWindow *parent, wxWindowID id)
#ifdef __WXOSX__
    : LabeledStaticBox(parent, id, ".")
#elses
    : LabeledStaticBox(parent, id, "")
#endif
{
    SetBackgroundColour(*wxWHITE);
    SetForegroundColour("#CECECE");
}

bool StaticGroup::Show(bool show)
{
    bool ret = wxStaticBox::Show(show);
    return ret;
}

void StaticGroup::ShowBadge(bool show)
{
    if (show && badge.name() != "badge") {
        badge = ScalableBitmap(this, "badge", 18);
        Refresh();
    } else if (!show && !badge.name().empty()) {
        badge = ScalableBitmap{};
        Refresh();
    }
}

void StaticGroup::DrawBorderAndLabel(wxDC& dc)
{
    LabeledStaticBox::DrawBorderAndLabel(dc);
    if (badge.bmp().IsOk()) {
        auto s = badge.bmp().GetScaledSize();
        dc.DrawBitmap(badge.bmp(), GetSize().x - s.x, std::max(0, m_pos.y) + m_label_height / 2);
    }
}
