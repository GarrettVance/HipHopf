//                      
//             
//                     ghv : Garrett Vance : 20170407 
//  
//                           file  gv_HUD.h   
//        
//      Synthesized from MSVC template Visual C++ >> Windows >> Universal 
//
//                      DirectX 11 App (Universal Windows)  
// 
//                                 Windows 10 
//      
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
//  




#pragma once

#include <string>

#include "..\Common\DeviceResources.h"

#include "..\Common\StepTimer.h"




namespace VHG
{

	// Renders the current FPS value using Direct2D and DirectWrite.


	class VHG_HUD1
	{
	public:

		VHG_HUD1(
			const std::shared_ptr<DX::DeviceResources>& p_deviceResources, 
			const float&                                p_vertical_percent,
			const float&                                p_horizontal_percent,
			D2D1_COLOR_F                                p_my_color
		);

			
		


		void CreateDeviceDependentResources();  

		void ReleaseDeviceDependentResources(); 




		void Update(DX::StepTimer const& timer,  	
			std::wstring const& p_wchar_message
		);

		


		void Render();



	private:

		std::shared_ptr<DX::DeviceResources>                      m_deviceResources;

		std::wstring                                              m_text; 

		float                                                     m_screen_pos_vert;
		float                                                     m_screen_pos_horz;

		D2D1_COLOR_F                                              m_special_color;

		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>              m_brush;

		DWRITE_TEXT_METRICS	                                      m_textMetrics;

		Microsoft::WRL::ComPtr<ID2D1DrawingStateBlock1>           m_stateBlock;  

		Microsoft::WRL::ComPtr<IDWriteTextLayout3>                m_textLayout;
		Microsoft::WRL::ComPtr<IDWriteTextFormat2>                m_textFormat;

	};



}  //  Closes namespace VHG; 



//                          ...file ends... 

                                       
