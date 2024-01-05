#include "pch.h"
#include "MainPage.h"
#include "MainPage.g.cpp"
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <ctime>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Composition.h>

using namespace winrt;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Composition;
using namespace Windows::UI::Xaml::Shapes;

using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;

using namespace Windows::UI;

using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Xaml::Hosting;
using namespace Windows::UI::Composition;

const float clockCenterPoint = 200.0f;       // center of the clock face, a circle, is from left margin and down from top.
const float tickHeight = 20.0f;              // the height of tick marks drawn to indicate hours of day.
const float handCenterOffset = 20.0f;        // number of units of stub of the hand for center of rotation.
const float hourHandDifference = 40.0f;      // number of units difference in length between hour hand and minute hand.
const float degreesInClockFace = 360.0f;     // number of degrees in a circle. clock face is a circle.
const float hoursOnClock = 12.0f;            // number of hours on a clock face, 12 hours counted 1 through 12.

float timer_Var = 0.0;

namespace winrt::BlankApp7::implementation
{

    Windows::UI::Composition::Compositor m_compositor = nullptr;
    Windows::UI::Composition::ContainerVisual m_root= nullptr;
    Windows::UI::Composition::SpriteVisual m_hourhand=nullptr;
    Windows::UI::Composition::SpriteVisual m_minutehand = nullptr;

    Windows::UI::Composition::ContainerVisual MainPage::GetVisual(Windows::UI::Xaml::UIElement element)
    {
        auto hostVisual =  ElementCompositionPreview::GetElementVisual(element);
        auto root = hostVisual.Compositor().CreateContainerVisual();
        ElementCompositionPreview::SetElementChildVisual(element, root);
        return root;
    }

    MainPage::MainPage()
    {
        InitializeComponent();

        m_root = GetVisual(SP1());
        m_compositor = m_root.Compositor();

        //winrt::Windows::UI::Color color = Windows::UI::Colors::White();
        Shapes::Ellipse mEll = Shapes::Ellipse();
        mEll.Stroke(SolidColorBrush::SolidColorBrush(Windows::UI::Colors::WhiteSmoke()));//Windows::UI::Colors::White
        mEll.Fill(SolidColorBrush::SolidColorBrush(Windows::UI::Colors::WhiteSmoke()));
        mEll.StrokeThickness(4);
        
        mEll.Height(clockCenterPoint * 2.0f);
        mEll.Width(clockCenterPoint * 2.0f);
        SP1().Children().Append(mEll);
        SP1().SetLeft(mEll, 2);
        SP1().SetTop(mEll, 0);

        SpriteVisual tick=nullptr;

        for (int i = 0; i < 12; i++)
        {
            tick = m_compositor.CreateSpriteVisual();
            if (i % 3 != 0) {
                // for tick marks other than 3, 6, 9, and 12 make them less prominent.
                tick.Brush(m_compositor.CreateColorBrush(Windows::UI::Colors::Silver()));
                tick.Size(Windows::Foundation::Numerics::float2(4.0f, tickHeight));                      // width and height of sprite
            }
            else {
                // for tick marks for 3, 6, 9, and 12 make them more prominent.
                tick.Brush(m_compositor.CreateColorBrush(Windows::UI::Colors::DarkRed()));
                tick.Size(Windows::Foundation::Numerics::float2(6.0f, tickHeight));                      // width and height of sprite
            }
            tick.CenterPoint(Windows::Foundation::Numerics::float3((tick.Size().x / 2.0f), clockCenterPoint, 0.0f));   // center point for rotations
            tick.Offset(Windows::Foundation::Numerics::float3(clockCenterPoint, 0.0f, 0.0f));                       // offset from the left only.
            
            tick.RotationAngleInDegrees(i * (degreesInClockFace / hoursOnClock));  // degrees divided by number of hour ticks on clock face.
            m_root.Children().InsertAtTop(tick);
        }

        // Create the sprite for the minute hand of the clock.
        // The minute hand is a green rectangle 2.0 wide by 100.0 high

        m_minutehand = m_compositor.CreateSpriteVisual();
        m_minutehand.Brush(m_compositor.CreateColorBrush(Windows::UI::Colors::Green()));
        m_minutehand.Size(Windows::Foundation::Numerics::float2(2.0f, clockCenterPoint - handCenterOffset));
        m_minutehand.CenterPoint(Windows::Foundation::Numerics::float3(m_minutehand.Size().x / 2.0f, m_minutehand.Size().y - handCenterOffset, 0.0f));
        m_minutehand.Offset(Windows::Foundation::Numerics::float3(clockCenterPoint, clockCenterPoint - m_minutehand.CenterPoint().y, 0.0f));

        // Create the sprite for the hour hand of the clock.
        // The hour hand is a gray rectangle 4.0 wide. It is shorter and wider than the minute hand.
        m_hourhand = m_compositor.CreateSpriteVisual();
        m_hourhand.Brush(m_compositor.CreateColorBrush(Windows::UI::Colors::Gray()));
        m_hourhand.Size(Windows::Foundation::Numerics::float2(4.0f, m_minutehand.Size().y - hourHandDifference));
        m_hourhand.CenterPoint(Windows::Foundation::Numerics::float3(m_hourhand.Size().x / 2.0f, m_hourhand.Size().y - handCenterOffset, 0.0f));
        m_hourhand.Offset(Windows::Foundation::Numerics::float3(clockCenterPoint, clockCenterPoint - m_hourhand.CenterPoint().y, 0.0f));

        m_root.Children().InsertAtTop(m_hourhand);      // add hour hand first so that it is beneath the minute hand
        m_root.Children().InsertAtTop(m_minutehand);    // add the minute hand after the hour hand so it is on top.

        // Set the hands of the clock to the current time and then start our timer.
        // The timer will update the position of the clock hands once a minute.
        setHandsCurrnetTime();
        startTimerAndRegistHandler();
    }

    int32_t MainPage::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void MainPage::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

    void MainPage::ClickHandler(IInspectable const&, RoutedEventArgs const&)
    {
        myButton().Content(box_value(L"Clicked"));
    }

    void MainPage::startTimerAndRegistHandler(void)
    {
        auto timer = Windows::UI::Xaml::DispatcherTimer();

        auto registrationtoken = timer.Tick({ this , &MainPage::OnTick });
        TimeSpan ts = TimeSpan::duration(500);
        timer.Interval(ts);
        timer.Start();
        //auto registration = timer.Tick({ this, &MainPage::OnTick }); //https://stackoverflow.com/questions/49959889/example-code-for-use-of-dispatchertime-in-c-winrt
    }

    void MainPage::setHandsCurrnetTime(void)
    {
        std::time_t result = std::time(nullptr);
        std::tm localTime;
        localtime_s(&localTime, &result);
        
        m_hourhand.RotationAngleInDegrees((float)localTime.tm_hour * (degreesInClockFace / hoursOnClock));  // degrees divided by number of hour ticks on clock face.
        m_minutehand.RotationAngleInDegrees((float)localTime.tm_sec * (degreesInClockFace / 60.0f));
    }
    
    void MainPage::OnTick(IInspectable const& sender , IInspectable const& event)
    {
        tsetbox1().Text(to_hstring(timer_Var++));
        setHandsCurrnetTime();
    }
}

void winrt::BlankApp7::implementation::MainPage::testCircle_ContextCanceled(winrt::Windows::UI::Xaml::UIElement const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& args)
{

}
