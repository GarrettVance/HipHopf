//                      
//        
//                           gv_D2D1.h
//
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
//  


#pragma once


#include "..\Common\DeviceResources.h"



template<class Interface>
inline void SafeRelease(
    Interface **ppInterfaceToRelease
)
{
    if (*ppInterfaceToRelease != NULL)
    {
        (*ppInterfaceToRelease)->Release();

        (*ppInterfaceToRelease) = NULL;
    }
}






namespace VHG
{

    class VHG_D2D1
    {
    public:

        VHG_D2D1(const std::shared_ptr<DX::DeviceResources>& deviceResources);

        VHG_D2D1::~VHG_D2D1(void); 


        void VHG_D2D1::CreateSizeDependentResources(void); 

        void VHG_D2D1::CreateDeviceResources(void); 



        void VHG_D2D1::Invalidate(void)
        {
            m_d2d1_invalid = true;
        }


        std::vector<std::wstring>       * VHG_D2D1::GetVectorWStr(void)
        {
            return m_vector_of_wstrings;
        }



        void VHG_D2D1::basic_fit(   //  for Oscilloscope lines grid;
            double q_x0,
            double q_x1,
            double q_y0,
            double q_y1,
            double q_left,
            double q_right,
            double q_bottom,
            double q_top
        ); 




        void VHG_D2D1::Render_d2d1(void); 

    private: 
        std::shared_ptr<DX::DeviceResources>        d_deviceResources;

        bool                                        m_d2d1_invalid; 

        Microsoft::WRL::ComPtr<ID2D1PathGeometry>   m_d2d1_path_geometry; 

        ID2D1SolidColorBrush*                       m_Brush1;

        ID2D1SolidColorBrush*                       m_Fill_Interior_Brush;
        ID2D1SolidColorBrush*                       m_Draw_Boundary_Brush;

        double                                      m_scaleX;
        double                                      m_scaleY;

        double                                      m_translateX;
        double                                      m_translateY; 

        std::vector<std::wstring>                  *m_vector_of_wstrings;
    };  //  Closes class VHG_D2D1; 

}  //  Closes namespace VHG; 



                                                                
