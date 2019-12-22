// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#include "ConvertedTab.g.h"
#include "Pane.h"

namespace winrt::TerminalApp::implementation
{
    struct ConvertedTab : public ConvertedTabT<ConvertedTab>
    {
    public:
        ConvertedTab() = default;
        ConvertedTab(const GUID& profile, const winrt::Microsoft::Terminal::TerminalControl::TermControl& control);

        winrt::Microsoft::Terminal::TerminalControl::TermControl GetActiveTerminalControl() const;
        winrt::Windows::UI::Xaml::UIElement GetRootElement();

        void BindEventHandlers(const winrt::Microsoft::Terminal::TerminalControl::TermControl& control) noexcept;
        bool IsFocused() const noexcept;
        void SetFocused(const bool focused);
        std::optional<GUID> GetFocusedProfile() const noexcept;
        void UpdateSettings(const winrt::Microsoft::Terminal::Settings::TerminalSettings& settings, const GUID& profile);
        void Scroll(const int delta);
        winrt::hstring GetActiveTitle() const;
        void UpdateIcon(const winrt::hstring iconPath);
        bool CanSplitPane(winrt::TerminalApp::SplitState splitType);
        void ResizeContent(const winrt::Windows::Foundation::Size& newSize);
        void ResizePane(const winrt::TerminalApp::Direction& direction);
        void NavigateFocus(const winrt::TerminalApp::Direction& direction);
        void ClosePane();

        WINRT_CALLBACK(Closed, winrt::Windows::Foundation::EventHandler<winrt::Windows::Foundation::IInspectable>);
        DECLARE_EVENT(PropertyChanged, _PropertyChanged, Windows::UI::Xaml::Data::PropertyChangedEventHandler);
        DECLARE_EVENT(ActivePaneChanged, _ActivePaneChangedHandlers, winrt::delegate<>);
        DEFINE_OBSERVABLE_GETSET_PROPERTY(hstring, Title, _PropertyChanged);
        DEFINE_OBSERVABLE_GETSET_PROPERTY(hstring, IconPath, _PropertyChanged);

    private:
        std::shared_ptr<Pane> _rootPane{ nullptr };
        std::shared_ptr<Pane> _activePane{ nullptr };
        winrt::hstring _lastIconPath{};

        bool _focused{ false };

        void _Focus();
        void _AttachEventHandlersToControl(const winrt::Microsoft::Terminal::TerminalControl::TermControl& control);
        void _AttachEventHandlersToPane(std::shared_ptr<Pane> pane);
    };
}
