//                      
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//             
//      
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  

#include "pch.h"
#include "GVMain.h"
#include "Common\DirectXHelper.h"
#include "..\gv_ADJUSTABLE.h"

using namespace VHG;
using namespace Windows::Foundation;
using namespace Windows::System::Threading;
using namespace Concurrency;


std::unique_ptr<VHG_D2D1>         GVMain::m_singleton_d2d1;






GVMain::GVMain(
    const std::shared_ptr<DX::DeviceResources>& formal_deviceResources
) : m_deviceResources(formal_deviceResources)
{

	// Register to be notified if the Device is lost or recreated

	m_deviceResources->RegisterDeviceNotify(this);

    m_sceneRenderer = std::make_unique<VHG_Scene3D1>(formal_deviceResources);

    GVMain::m_singleton_d2d1 = std::unique_ptr<VHG_D2D1>(
                                new VHG_D2D1(formal_deviceResources));


	m_overlay_upper1 = std::unique_ptr<VHG_HUD1>(new VHG_HUD1(
			m_deviceResources,
			0.05f,    //  upper 
			0.05f,   //  LEFT !!! 
			//   Classic : D2D1::ColorF(D2D1::ColorF::Violet)
			D2D1::ColorF(D2D1::ColorF::Crimson)
    ));

	m_overlay_upper2 = std::unique_ptr<VHG_HUD1>(new VHG_HUD1(
			m_deviceResources,
			0.05f,    //  upper 
			0.999f,   //  right 
			D2D1::ColorF(D2D1::ColorF::Yellow)));

	m_overlay_lower_right = std::unique_ptr<VHG_HUD1>(new VHG_HUD1(
			m_deviceResources,
			0.999f,    //  lower  
			0.999f,    //  right  
			D2D1::ColorF(D2D1::ColorF::Red)));


    GVMain::m_singleton_d2d1->CreateDeviceResources();

    GVMain::m_singleton_d2d1->Invalidate();
    GVMain::m_singleton_d2d1->CreateSizeDependentResources();
}
//  Closes class ctor;







GVMain::~GVMain()
{
	// Deregister device notification
	m_deviceResources->RegisterDeviceNotify(nullptr);
}







void GVMain::CreateWindowSizeDependentResources() 
{
    //      Updates application state 
    //      when the window size changes 
    //      (e.g. device orientation change)
   
	m_sceneRenderer->CreateWindowSizeDependentResources();

    GVMain::m_singleton_d2d1->Invalidate();
    GVMain::m_singleton_d2d1->CreateSizeDependentResources();
}  
//  Closes GVMain::CreateWindowSizeDependentResources(); 









void GVMain::Update() 
{
	//      Update the scene objects once per frame:


	m_timer.Tick([&]()
	{
		m_sceneRenderer->Update3DScene(m_timer);  

		{
            m_overlay_upper1->Update(m_timer, std::wstring(L"Hopf Fibration"));
		}



		{
            uint32_t u_triangles_per_frame = m_sceneRenderer->CountRenderedTriangles();  
            uint32_t u_frames_per_sec = max(m_timer.GetFramesPerSecond(), 1);
            uint32_t u_triangles_per_second = u_triangles_per_frame * u_frames_per_sec; 

            wchar_t   buffer2[24];
            const wchar_t  fmt2[] = L"%d\n"; 
			std::swprintf(buffer2, 17, fmt2, u_triangles_per_second); 


            int num_digits = -1 + (int)wcslen(buffer2); 
            std::wstring wstr_int_with_commas(L"");
            int idx_comma_string = 0; 
            int distance_comma_string = 0;

            for (idx_comma_string = 0; buffer2[idx_comma_string] != '\0'; idx_comma_string++)
            {
                wstr_int_with_commas += buffer2[idx_comma_string]; 

                distance_comma_string = num_digits - idx_comma_string - 1; 

                if (distance_comma_string > 0 && distance_comma_string % 3 == 0) wstr_int_with_commas += ',';
            }

            std::wstring wstr_upper2(L"Triangles per Second: "); 
            wstr_upper2 += wstr_int_with_commas; 
			m_overlay_upper2->Update(m_timer, wstr_upper2);
		}


		{
			std::wstring wstr_lower = L"";

			if (m_sceneRenderer->gv_GetAdvertCount() > m_sceneRenderer->gv_GetAdvertModulus())
			{
				wstr_lower = L"The Fractal Method";
			}
			else
			{
				uint32 v_fps = m_timer.GetFramesPerSecond();
				wstr_lower = (v_fps > 0) ? std::to_wstring(v_fps) + L" Frames per second" : L" - FPS";
			}
			m_overlay_lower_right->Update(m_timer, wstr_lower);
		}
	});

}  
//  Closes GVMain::Update(); 






bool GVMain::Render() 
{
	// Don't try to render anything before the first Update.

	if (m_timer.GetFrameCount() == 0)
	{
		return false;
	}

	auto context = m_deviceResources->GetD3DDeviceContext();


    //  =================================================================
    //  =================================================================
    //                
    //  ghv : 20180528 : regarding RSSetViewports() 
    //  =================================================================
    //  The code to reset the viewport (or array of viewports plural)
    //  has been migrated into the VHG_Scene3D1::Render3DScene() method.
    //        
    //  =================================================================
    //  =================================================================



	// Reset render targets to the screen.

	ID3D11RenderTargetView *const targets[1] = { m_deviceResources->GetBackBufferRenderTargetView() };
	context->OMSetRenderTargets(1, targets, m_deviceResources->GetDepthStencilView());



	// Clear the back buffer and depth stencil view.  




	context->ClearRenderTargetView(
		m_deviceResources->GetBackBufferRenderTargetView(),
		DirectX::Colors::Black  // GHV_COLOR_OF_WORLD
		// DirectX::Colors::CadetBlue
	);




	context->ClearDepthStencilView(
		m_deviceResources->GetDepthStencilView(), 
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 
		1.0f, 
		0);






	m_sceneRenderer->Render3DScene();






#ifdef GHV_OPTION_SHOW_OSCILLOSCOPE_GRID_LINES
    GVMain::m_singleton_d2d1->Render_d2d1(); // Oscilloscope-style grid lines;
#endif 




#ifdef GHV_OPTION_SHOW_OVERLAY_HUD
    m_overlay_upper1->Render();
	m_overlay_upper2->Render();
	m_overlay_lower_right->Render();
#endif 


	return true; 
}  
//  Closes  GVMain::Render;  












// Notifies renderers that device resources need to be released.

void GVMain::OnDeviceLost()
{

	m_sceneRenderer->ReleaseDeviceDependentResources(); 

	m_overlay_upper1->ReleaseDeviceDependentResources();
	m_overlay_upper2->ReleaseDeviceDependentResources();
	m_overlay_lower_right->ReleaseDeviceDependentResources(); 

}








void GVMain::OnDeviceRestored()
{ 
    // Notifies renderers that device resources may now be recreated.
	
    
    m_sceneRenderer->CreateDeviceDependentResources();

    GVMain::m_singleton_d2d1->CreateDeviceResources();

	m_overlay_upper1->CreateDeviceDependentResources();
	m_overlay_upper2->CreateDeviceDependentResources();
	m_overlay_lower_right->CreateDeviceDependentResources();

	CreateWindowSizeDependentResources();

}




//                   ...file ends....


