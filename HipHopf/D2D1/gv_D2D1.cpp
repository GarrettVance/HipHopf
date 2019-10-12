//                        
//                
//                              

#include "pch.h"
#include <D2d1helper.h>
#include "..\Common\DirectXHelper.h"   //  for DX::ThrowIfFailed(); 
#include "gv_D2D1.h"

using namespace VHG;    
using Microsoft::WRL::ComPtr;




#undef  GHV_WM_SIZING_SQUARE






VHG_D2D1::VHG_D2D1(
    const std::shared_ptr<DX::DeviceResources>& q_deviceResources
) 
  : d_deviceResources( q_deviceResources ), 
    m_d2d1_invalid(true)
{
    m_vector_of_wstrings = new std::vector<std::wstring>(); 
}
//  Closes class ctor VHG_D2D1::VHG_D2D1(); 







VHG_D2D1::~VHG_D2D1(void)
{
    if (m_vector_of_wstrings)
    {
        delete m_vector_of_wstrings; 

        m_vector_of_wstrings = nullptr; 
    }
}
//  Class destructor; 







void VHG_D2D1::CreateSizeDependentResources(void)
{
    //  Invoked whenever the window size changes.
    //      
    //  fitToWindow(-1.0, +1.0, -1.0, +1.0);
    //      

    ;

}
//  Closes VHG_D2D1::CreateSizeDependentResources(); 






void VHG_D2D1::CreateDeviceResources(void)
{
    ID2D1DeviceContext2   *d2d1_ctxt = 
        this->d_deviceResources->GetD2DDeviceContext();


    DX::ThrowIfFailed(d2d1_ctxt->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::BlueViolet),
        &m_Brush1)
    ); 
    
    DX::ThrowIfFailed(d2d1_ctxt->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::DarkGreen),
        &m_Fill_Interior_Brush)
    );

    DX::ThrowIfFailed(d2d1_ctxt->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::LightBlue),
        &m_Draw_Boundary_Brush)
    );
}
//  Closes VHG_D2D1::CreateDeviceResources(); 






void VHG_D2D1::basic_fit(
    double q_x0,
    double q_x1,
    double q_y0,
    double q_y1,
    double q_left,
    double q_right,
    double q_bottom,
    double q_top
)
{
    //
    //          Set translate and scale
    //          so that x0,x1,y0,y0
    //          map to left,right,bottom,top.
    //

    m_scaleX = (q_right - q_left) / (q_x1 - q_x0);

    m_scaleY = (q_top - q_bottom) / (q_y1 - q_y0);

    m_translateX = q_left - m_scaleX * q_x0;

    m_translateY = q_bottom - m_scaleY * q_y0;
}
//  Closes VHG_D2D1::basic_fit;








void VHG_D2D1::Render_d2d1(void)
{
    if (!m_d2d1_invalid)
    {
        return;
    }

    ID2D1DeviceContext2       *gv_d2d1_context 
        = this->d_deviceResources->GetD2DDeviceContext();

    
    gv_d2d1_context->BeginDraw();

    gv_d2d1_context->SetTransform(D2D1::Matrix3x2F::Identity());
    
    m_Brush1->SetColor(D2D1::ColorF(D2D1::ColorF::GreenYellow));

   
    Windows::Foundation::Size   trial_size 
        = this->d_deviceResources->GetOutputSize();
  

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //          Draw a grid background
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    int rt_width = static_cast<int>(trial_size.Width);
    int rt_height = static_cast<int>(trial_size.Height);

    float const f_delta_grid = 80.f;

    int const limit_idx_x = lrint(floor(trial_size.Width / f_delta_grid)); 
    for (int idx_x = 1; idx_x < limit_idx_x; idx_x++)
    {
        float x_tick = idx_x * f_delta_grid;

        gv_d2d1_context->DrawLine(
            D2D1::Point2F( -1.f * x_tick + trial_size.Width / 2.f, 0.0f),
            D2D1::Point2F( -1.f * x_tick + trial_size.Width / 2.f, trial_size.Height),
            m_Brush1, 0.5f );

        gv_d2d1_context->DrawLine(
            D2D1::Point2F( +1.f * x_tick + trial_size.Width / 2.f, 0.0f),
            D2D1::Point2F( +1.f * x_tick + trial_size.Width / 2.f, trial_size.Height),
            m_Brush1, 0.5f );
    }


    int const limit_idx_y = lrint(floor(trial_size.Height / f_delta_grid)); 
    for (int idx_y = 1; idx_y < limit_idx_y; idx_y++)
    {
        float y_tick = idx_y * f_delta_grid;

        gv_d2d1_context->DrawLine(
            D2D1::Point2F(0.f,              -1.f * y_tick + trial_size.Height / 2.f),
            D2D1::Point2F(trial_size.Width, -1.f * y_tick + trial_size.Height / 2.f),
            m_Brush1, 0.5f );

        gv_d2d1_context->DrawLine(
            D2D1::Point2F(0.f,              +1.f * y_tick + trial_size.Height / 2.f),
            D2D1::Point2F(trial_size.Width, +1.f * y_tick + trial_size.Height / 2.f),
            m_Brush1, 0.5f );
    }

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    float translateX = trial_size.Width / (+1.f - -1.f);   // typically 600.f;
    float translateY = trial_size.Height / 2.f;   // typically 450.f; 
    float scaleX = trial_size.Width / 2.f;
    float scaleY = trial_size.Height / 2.f;
    float pixel_line_width = 1;  // Probably bold enough...

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    float x_left_end = -0.95f * scaleX + translateX;  //  -(0.5f * pixel_line_width);
    float y_left_end = 0.00f * scaleY + translateY;   //   -(0.5f * pixel_line_width);

    float x_right_end = +0.95f * scaleX + translateX;  //   -(0.5f * pixel_line_width);
    float y_right_end = 0.00f * scaleY + translateY;   //   -(0.5f * pixel_line_width);

    gv_d2d1_context->DrawLine(
        D2D1::Point2F(x_left_end, y_left_end),
        D2D1::Point2F(x_right_end, y_right_end),
        m_Brush1,
        pixel_line_width /* line width */
    );

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    float x_upper_end =  0.f * scaleX + translateX;  //  -(0.5f * pixel_line_width);
    float y_upper_end = -0.95f * scaleY + translateY;   //   -(0.5f * pixel_line_width);

    float x_lower_end =  0.f * scaleX + translateX;  //   -(0.5f * pixel_line_width);
    float y_lower_end = +0.95f * scaleY + translateY;   //   -(0.5f * pixel_line_width);

    gv_d2d1_context->DrawLine(
        D2D1::Point2F(x_upper_end, y_upper_end),
        D2D1::Point2F(x_lower_end, y_lower_end),
        m_Brush1,
        pixel_line_width /* line width */
    );

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


    
/*********************************************************************************    
    //         
    //      ghv : for the D2D1 Transforms (translate and scale)
    //            the grid "domain" is assumed to be [-1.0, +1.0]
    //            in both the x- and y-directions...

    
    float translateX = trial_size.Width / (+1.f - -1.f);   // typically 600.f;
    float translateY = trial_size.Height / 2.f;   // typically 450.f; 
    const D2D1::Matrix3x2F  translate_matrix = D2D1::Matrix3x2F::Translation(translateX, translateY);
    float scaleX = 600.0f; //  trial_size.Width / 2.f;
    float scaleY = 5.0f; // trial_size.Height / 2.f;
    D2D1_SIZE_F gv_scale_factor = { scaleX, scaleY };
    D2D1_POINT_2F gv_center = { 0.f, 0.f }; 
    const D2D1::Matrix3x2F scale_matrix = D2D1::Matrix3x2F::Scale(gv_scale_factor, gv_center); 
    gv_d2d1_context->SetTransform(scale_matrix * translate_matrix); 
*********************************************************************************/    

    

    //  not required gv_d2d1_context->SetTransform(D2D1::Matrix3x2F::Identity());


    DX::ThrowIfFailed(
        gv_d2d1_context->EndDraw()
    );


    //    nice try, but needs work... m_d2d1_invalid = false; 


}
//  Closes VHG_D2D1::Render_d2d1(); 




