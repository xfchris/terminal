// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once

#include "TabTest.g.h"
#include "Pane.h"

namespace winrt::TerminalApp::implementation
{
    struct TabTest : TabTestT<TabTest>
    {
        TabTest() = default;
    };
}

namespace winrt::TerminalApp::factory_implementation
{
    struct TabTest : TabTestT<TabTest, implementation::TabTest>
    {
    };
}
