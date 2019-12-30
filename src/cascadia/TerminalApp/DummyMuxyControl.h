// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.
//
// Declaration of the MainUserControl class.
//

#pragma once

#include "winrt/Windows.UI.Xaml.h"
#include "winrt/Windows.UI.Xaml.Markup.h"
#include "winrt/Windows.UI.Xaml.Interop.h"
#include "DummyMuxyControl.g.h"
#include "../../cascadia/inc/cppwinrt_utils.h"

namespace winrt::TerminalApp::implementation
{
    struct DummyMuxyControl : DummyMuxyControlT<DummyMuxyControl>
    {
        DummyMuxyControl();
    };
}

namespace winrt::TerminalApp::factory_implementation
{
    BASIC_FACTORY(DummyMuxyControl);
}
