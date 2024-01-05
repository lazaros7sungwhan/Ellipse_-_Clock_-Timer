#pragma once

#include "MainPage.g.h"
#include <winrt/Windows.UI.Xaml.Shapes.h>

namespace winrt::BlankApp7::implementation
{
    struct MainPage : MainPageT<MainPage>
    {

        MainPage();
        

        int32_t MyProperty();
        void MyProperty(int32_t value);

        void ClickHandler(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
        void testCircle_ContextCanceled(winrt::Windows::UI::Xaml::UIElement const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& args);

        Windows::UI::Composition::ContainerVisual MainPage::GetVisual(Windows::UI::Xaml::UIElement element);
        void startTimerAndRegistHandler(void);
        void setHandsCurrnetTime(void);
        void OnTick(Windows::Foundation::IInspectable const& sender, Windows::Foundation::IInspectable const& event);

    };
}

namespace winrt::BlankApp7::factory_implementation
{
    struct MainPage : MainPageT<MainPage, implementation::MainPage>
    {
    };
}
