// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "pch.h"

#include "../TerminalApp/TerminalPage.h"
#include "../TerminalApp/ShortcutActionDispatch.h"
#include "../TerminalApp/Tab.h"
#include "../CppWinrtTailored.h"
#include "JsonTestClass.h"

using namespace Microsoft::Console;
using namespace TerminalApp;
using namespace WEX::Logging;
using namespace WEX::TestExecution;

namespace TerminalAppLocalTests
{
    // TODO:microsoft/terminal#3838:
    // Unfortunately, these tests _WILL NOT_ work in our CI. We're waiting for
    // an updated TAEF that will let us install framework packages when the test
    // package is deployed. Until then, these tests won't deploy in CI.

    class TabTests : public JsonTestClass
    {
        // For this set of tests, we need to activate some XAML content. For
        // release builds, the application runs as a centennial application,
        // which lets us run full trust, and means that we need to use XAML
        // Islands to host our UI. However, in these tests, we don't really need
        // to run full trust - we just need to get some UI elements created. So
        // we can just rely on the normal UWP activation to create us.
        //
        // IMPORTANTLY! When tests need to make XAML objects, or do XAML things,
        // make sure to use RunOnUIThread. This helper will dispatch a lambda to
        // be run on the UI thread.

        BEGIN_TEST_CLASS(TabTests)
            TEST_CLASS_PROPERTY(L"RunAs", L"UAP")
            TEST_CLASS_PROPERTY(L"UAP:AppXManifest", L"TerminalApp.LocalTests.AppxManifest.xml")
            TEST_CLASS_PROPERTY(L"UAP:Host", L"Xaml")
            TEST_CLASS_PROPERTY(L"UAP:WaitForXamlWindowActivation", L"true")
        END_TEST_CLASS()

        // These four tests act as canary tests. If one of them fails, then they
        // can help you identify if something much lower in the stack has
        // failed.
        TEST_METHOD(EnsureTestsActivate);
        TEST_METHOD(TryCreateLocalWinRTType);
        TEST_METHOD(TryCreateXamlObjects);
        TEST_METHOD(TryCreateTab);

        TEST_METHOD(TryCreateTerminalPage);

        TEST_CLASS_SETUP(ClassSetup)
        {
            InitializeJsonReader();
            return true;
        }
    };

    void TabTests::EnsureTestsActivate()
    {
        // This test was originally used to ensure that XAML Islands was
        // initialized correctly. Now, it's used to ensure that the tests
        // actually deployed and activated. This test _should_ always pass.
        VERIFY_IS_TRUE(true);
    }

    void TabTests::TryCreateLocalWinRTType()
    {
        // Verify we can create a WinRT type we authored
        // Just creating it is enough to know that everything is working.
        winrt::Microsoft::Terminal::Settings::TerminalSettings settings;
        VERIFY_IS_NOT_NULL(settings);
        auto oldFontSize = settings.FontSize();
        settings.FontSize(oldFontSize + 5);
        auto newFontSize = settings.FontSize();
        VERIFY_ARE_NOT_EQUAL(oldFontSize, newFontSize);
    }

    void TabTests::TryCreateXamlObjects()
    {
        auto result = RunOnUIThread([]() {
            VERIFY_IS_TRUE(true, L"Congrats! We're running on the UI thread!");

            auto v = winrt::Windows::ApplicationModel::Core::CoreApplication::GetCurrentView();
            VERIFY_IS_NOT_NULL(v, L"Ensure we have a current view");
            // Verify we can create a some XAML objects
            // Just creating all of them is enough to know that everything is working.
            winrt::Windows::UI::Xaml::Controls::UserControl controlRoot;
            VERIFY_IS_NOT_NULL(controlRoot, L"Try making a UserControl");
            winrt::Windows::UI::Xaml::Controls::Grid root;
            VERIFY_IS_NOT_NULL(root, L"Try making a Grid");
            winrt::Windows::UI::Xaml::Controls::SwapChainPanel swapChainPanel;
            VERIFY_IS_NOT_NULL(swapChainPanel, L"Try making a SwapChainPanel");
            winrt::Windows::UI::Xaml::Controls::Primitives::ScrollBar scrollBar;
            VERIFY_IS_NOT_NULL(scrollBar, L"Try making a ScrollBar");
        });

        VERIFY_SUCCEEDED(result);
    }

    void TabTests::TryCreateTab()
    {
        // If you leave the Tab shared_ptr owned by the RunOnUIThread lambda, it
        // will crash when the test tears down. Not totally clear why, but make
        // sure it's owned outside the lambda
        std::shared_ptr<Tab> newTab{ nullptr };

        auto result = RunOnUIThread([&newTab]() {
            // Try creating all of:
            // 1. one of our pure c++ types (Profile)
            // 2. one of our c++winrt types (TerminalSettings, EchoConnection)
            // 3. one of our types that uses MUX/Xaml (TermControl).
            // 4. one of our types that uses MUX/Xaml in this dll (Tab).
            // Just creating all of them is enough to know that everything is working.
            const auto profileGuid{ Utils::CreateGuid() };
            winrt::Microsoft::Terminal::Settings::TerminalSettings settings{};
            VERIFY_IS_NOT_NULL(settings);
            winrt::Microsoft::Terminal::TerminalConnection::EchoConnection conn{};
            VERIFY_IS_NOT_NULL(conn);
            winrt::Microsoft::Terminal::TerminalControl::TermControl term{ settings, conn };
            VERIFY_IS_NOT_NULL(term);

            newTab = std::make_shared<Tab>(profileGuid, term);
            VERIFY_IS_NOT_NULL(newTab);
        });

        VERIFY_SUCCEEDED(result);
    }

    void TabTests::TryCreateTerminalPage()
    {
        const std::string settingsJson{ R"(
        {
            "defaultProfile": "{6239a42c-0000-49a3-80bd-e8fdd045185c}",
            "profiles": [
                {
                    "name": "profile0",
                    "guid": "{6239a42c-0000-49a3-80bd-e8fdd045185c}",
                    "historySize": 1,
                    "commandline": "cmd.exe"
                },
                {
                    "name": "profile1",
                    "guid": "{6239a42c-1111-49a3-80bd-e8fdd045185c}",
                    "historySize": 2,
                    "commandline": "pwsh.exe"
                },
                {
                    "name": "profile2",
                    "historySize": 3,
                    "commandline": "wsl.exe"
                }
            ],
            "keybindings": [
                { "keys": ["ctrl+a"], "command": { "action": "splitPane", "split": "vertical" } },
                { "keys": ["ctrl+b"], "command": { "action": "splitPane", "split": "vertical", "profile": "{6239a42c-1111-49a3-80bd-e8fdd045185c}" } },
                { "keys": ["ctrl+c"], "command": { "action": "splitPane", "split": "vertical", "profile": "profile1" } },
                { "keys": ["ctrl+d"], "command": { "action": "splitPane", "split": "vertical", "profile": "profile2" } },
                { "keys": ["ctrl+e"], "command": { "action": "splitPane", "split": "horizontal", "commandline": "foo.exe" } },
                { "keys": ["ctrl+f"], "command": { "action": "splitPane", "split": "horizontal", "profile": "profile1", "commandline": "foo.exe" } },
                { "keys": ["ctrl+g"], "command": { "action": "newTab" } },
                { "keys": ["ctrl+h"], "command": { "action": "newTab", "startingDirectory": "c:\\foo" } },
                { "keys": ["ctrl+i"], "command": { "action": "newTab", "profile": "profile2", "startingDirectory": "c:\\foo" } },
                { "keys": ["ctrl+j"], "command": { "action": "newTab", "tabTitle": "bar" } },
                { "keys": ["ctrl+k"], "command": { "action": "newTab", "profile": "profile2", "tabTitle": "bar" } },
                { "keys": ["ctrl+l"], "command": { "action": "newTab", "profile": "profile1", "tabTitle": "bar", "startingDirectory": "c:\\foo", "commandline":"foo.exe" } }
            ]
        })" };

        auto settings = std::make_shared<CascadiaSettings>();
        winrt::com_ptr<winrt::TerminalApp::implementation::TerminalPage> root{ nullptr };

        // If you inline RunOnUIThread inside the VERIFY_SUCCEEDED call, the log
        // message will contain the entire lambda. That's annoying, so don't do
        // that.
        auto result = RunOnUIThread([&]() {
            VerifyParseSucceeded(settingsJson);
            settings->_ParseJsonString(settingsJson, false);
            settings->LayerJson(settings->_userSettings);
            settings->_ValidateSettings();
            VERIFY_ARE_EQUAL(3u, settings->GetProfiles().size());
        });
        VERIFY_SUCCEEDED(result);

        result = RunOnUIThread([&root, &settings]() {
            DebugBreak();
            root = winrt::make_self<winrt::TerminalApp::implementation::TerminalPage>();
            root->SetSettings(settings, false);
            VERIFY_ARE_EQUAL(0u, root->_tabs.size());
            root->Create();
            VERIFY_ARE_EQUAL(1u, root->_tabs.size());
            root->_OpenNewTab(nullptr);
            VERIFY_ARE_EQUAL(2u, root->_tabs.size());
        });
        VERIFY_SUCCEEDED(result);
    }

}
