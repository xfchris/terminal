// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once

#include "TabWinRT.g.h"
#include "Pane.h"
#include "cppwinrt_utils.h"

namespace winrt::TerminalApp::implementation
{
    class TabWinRT
    {
        TabWinRT(const winrt::Guid& profile, const winrt::Microsoft::Terminal::TerminalControl::TermControl& control);
    };
}

namespace winrt::Microsoft::Terminal::TerminalPage::factory_implementation
{
    struct TabWinRT : TabWinRTT<TabWinRT, implementation::TabWinRT>
    {
    };
}
