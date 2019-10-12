//                      
//             
//                     ghv : Garrett Vance : 20170407 
//  
//                           file  gv_HUD.cpp    
//        
//      Synthesized from MSVC template Visual C++ >> Windows >> Universal 
//
//                      DirectX 11 App (Universal Windows)  
// 
//                                 Windows 10 
//      
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
//  

#include "pch.h"

#include "gv_HUD.h"

#include "Common/DirectXHelper.h"

using namespace VHG;

using namespace Microsoft::WRL;







VHG_HUD1::VHG_HUD1( 
        const std::shared_ptr<DX::DeviceResources>& p_deviceResources, 
        const float&                                p_vertical_percent, 
        const float&                                p_horizontal_percent, 
        D2D1_COLOR_F                                p_my_color
) 
    : m_deviceResources(p_deviceResources), 
    m_screen_pos_vert(p_vertical_percent), 
    m_screen_pos_horz(p_horizontal_percent), 
    m_special_color(p_my_color), 
    m_text(L"") 
{

    //      
    //      Class ctor for the FpsTextRenderer class 
    //  
    //    Initialize the D2D resources needed to render text 
    //  

    ZeroMemory(&m_textMetrics, sizeof(DWRITE_TEXT_METRICS));



    // 
    //   First part:  Create device independent resources
    // 

    ComPtr<IDWriteTextFormat> textFormat;


    DX::ThrowIfFailed(
        m_deviceResources->GetDWriteFactory()->CreateTextFormat(
            L"Mametosca 026",   //   L"Segoe UI",
            nullptr,
            DWRITE_FONT_WEIGHT_BOLD,   //   DWRITE_FONT_WEIGHT_LIGHT,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            32.0f,
            L"en-US", &textFormat));



    DX::ThrowIfFailed(textFormat.As(&m_textFormat));

    DX::ThrowIfFailed(m_textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR));

    DX::ThrowIfFailed(m_deviceResources->GetD2DFactory()->CreateDrawingStateBlock(&m_stateBlock));



    //   
    //      Next,  create the device DEPENDENT resources, 
    //  
    //      for example, must create a colored brush.... 
    //  

    CreateDeviceDependentResources();  

}  //  Closes class ctor VHG_HUD1::VHG_HUD1(); 












 

void VHG_HUD1::Update(
    DX::StepTimer const& timer, 
    std::wstring const& p_wchar_message)
{
    //     
    //       Update the display text.
    // 

    m_text = p_wchar_message;

    ComPtr<IDWriteTextLayout> textLayout;  


    DX::ThrowIfFailed(
        m_deviceResources->GetDWriteFactory()->CreateTextLayout(
            m_text.c_str(),
            (uint32) m_text.length(),
            m_textFormat.Get(),
            240.0f, // Max width of the input text.
            50.0f, // Max height of the input text.
            &textLayout
            ));


    DX::ThrowIfFailed(textLayout.As(&m_textLayout));


    DX::ThrowIfFailed(m_textLayout->GetMetrics(&m_textMetrics)); 


}  //  Closes VHG_HUD1::Update;  












void VHG_HUD1::Render()
{

    //          Render a frame to the screen.

    //  
    //      m_screen_pos_horz == 0.000 implies "position at left edge of screen";       
    //      m_screen_pos_horz == 0.999 implies "position at right edge of screen";       
    //
    //      m_screen_pos_vert == 0.000 implies "position along the top edge of screen";       
    //      m_screen_pos_vert == 0.999 implies "position along the bottom edge of screen";    
    //

    ID2D1DeviceContext* context = m_deviceResources->GetD2DDeviceContext(); 

    Windows::Foundation::Size logicalSize = m_deviceResources->GetLogicalSize();

    
    float location_x = (m_screen_pos_horz < 0.5f) ? 
        30.f : (logicalSize.Width - m_textMetrics.layoutWidth - 30);

    float location_y = (m_screen_pos_vert < 0.5f) ? 
        30.f : (logicalSize.Height - m_textMetrics.height - 30);


    context->SaveDrawingState(m_stateBlock.Get());  


    context->BeginDraw();


    D2D1::Matrix3x2F screenTranslation = D2D1::Matrix3x2F::Translation(location_x, location_y);

    context->SetTransform(screenTranslation * m_deviceResources->GetOrientationTransform2D());


    DWRITE_TEXT_ALIGNMENT   v_lateral = (m_screen_pos_horz < 0.5f) ? 
        DWRITE_TEXT_ALIGNMENT_LEADING : DWRITE_TEXT_ALIGNMENT_TRAILING;


    DX::ThrowIfFailed(m_textFormat->SetTextAlignment(v_lateral));


    context->DrawTextLayout(D2D1::Point2F(0.f, 0.f), m_textLayout.Get(), m_brush.Get());


    // Ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
    // is lost. It will be handled during the next call to Present.   


    HRESULT hr = context->EndDraw();


    if (hr != D2DERR_RECREATE_TARGET)
    {
        DX::ThrowIfFailed(hr);
    }

    context->RestoreDrawingState(m_stateBlock.Get());


}  //  Closes VHG_HUD1::Render; 













void VHG_HUD1::CreateDeviceDependentResources()
{
    DX::ThrowIfFailed(
        m_deviceResources->GetD2DDeviceContext()->CreateSolidColorBrush(
            m_special_color,
            &m_brush
        )
    );
}









void VHG_HUD1::ReleaseDeviceDependentResources()
{
    m_brush.Reset();
}




//                 ...file ends...



