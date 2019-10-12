//                      
//             
//                      ghv : Garrett Vance : 20180520
//  
//                           file  gv_D3D11_KMI.cpp      
//     
//                DirectX 11 App (Universal Windows), Windows 10
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  






#include "pch.h"
#include "gv_SCENE_3D.h"
#include "..\Common\DirectXHelper.h"
#include "..\gv_ADJUSTABLE.h"




using namespace VHG;
using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace Windows::Foundation;








static const DirectX::XMVECTORF32 START_POSITION = { 0.0f, 0.0f, -13.5f, 0.0f };

bool g0_Button_Processed = false; 

static const float risk_speed = 40.f; //  80.f;   //  Speed for mouse-look mode; 

static const float ROTATION_GAIN = 0.004f;

double                   g0_total_seconds = 0.0;
double                   g0_total_sec_stop = 0.0;
double                   g0_total_sec_resume = 0.0;
double                   g0_eff_total_seconds = 0.0; 


double                   g0_S2sphere_total_seconds = 0.0;
double                   g0_S2sphere_total_sec_stop = 0.0;
double                   g0_S2sphere_total_sec_resume = 0.0;
double                   g0_S2sphere_eff_total_seconds = 0.0; 


void VHG_Scene3D1::debouncer(void)
{
    static uint32_t idx_waiting = 0;
    if (g0_Button_Processed)
    {
        //  keyboard debouncer: Wait some duration before resetting g0_Button_Processed: 
        idx_waiting++;
        if (idx_waiting > 90)
        {
            idx_waiting = 0;
            g0_Button_Processed = false;
        }
    }
}










DirectX::SimpleMath::Vector3 gv_Calc_kminput_x_y_z_From_Pitch_Yaw(float p_pitch, float p_yaw)
{
    float   kminput_f_y = sinf(p_pitch);
    float   kminput_f_r = cosf(p_pitch);

    float   kminput_f_z = kminput_f_r * cosf(p_yaw);
    float   kminput_f_x = kminput_f_r * sinf(p_yaw);

    DirectX::SimpleMath::Vector3 kmi_input_as_vector = DirectX::SimpleMath::Vector3(
        kminput_f_x, kminput_f_y, kminput_f_z);

    return kmi_input_as_vector;
}  
//  Closes gv_Calc_kminput_x_y_z_From_Pitch_Yaw();
















void VHG_Scene3D1::GV_Camera_Init(void)
{
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
    //           Update the View Matrix and Camera Position
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 

    e_camera_position = START_POSITION.v;
    DirectX::SimpleMath::Vector3 tmp_kminput = gv_Calc_kminput_x_y_z_From_Pitch_Yaw(e_pitch, e_yaw);
    DirectX::SimpleMath::Vector3 camera_target = e_camera_position + tmp_kminput;
    DirectX::SimpleMath::Vector3 camera_up_vector = DirectX::SimpleMath::Vector3::Up;


    DirectX::SimpleMath::Matrix viewMatrixLeftHanded = XMMatrixLookAtLH(e_camera_position, camera_target, camera_up_vector);

    /*
    XMStoreFloat4x4(
        &e_conbuf_Loft_ModelViewProjectionTransform_data.view_transform, 
        XMMatrixTranspose(
            viewMatrixLeftHanded
        )
    );
    */



    XMStoreFloat4x4(
        &e_conbuf_MQuad_ViewProjectionTransform_data.view_transform,
        XMMatrixTranspose(
            viewMatrixLeftHanded // CHIRALITY SINISTRAL;
        )
    );

    XMStoreFloat4x4(
        &e_INodes_MVP_data.view_transform,
        XMMatrixTranspose(
            viewMatrixLeftHanded // CHIRALITY SINISTRAL;
        )
    );
}  
//  Closes  VHG_Scene3D1:GV_Camera_Init();



























DirectX::XMMATRIX VHG_Scene3D1::do_S2sphere_rotation(DX::StepTimer const& p_scene_timer)
{
    g0_S2sphere_total_seconds = p_scene_timer.GetTotalSeconds();
    float radiansPerSecond = XMConvertToRadians(m_degreesPerSecond);
    double total_sec = 0.f;

    if (e_option_S2sphere_rotating) 
    {
        total_sec = g0_S2sphere_total_seconds - (g0_S2sphere_total_sec_resume - g0_S2sphere_total_sec_stop);  // perfect!!!
        g0_S2sphere_eff_total_seconds = total_sec;
    }
    else 
    {
        total_sec = g0_S2sphere_total_sec_stop;
    }
    
    double total_radians = total_sec * radiansPerSecond; 
    float proper_radians = static_cast<float>(fmod(total_radians, XM_2PI));
 

	//   
	//  Suppose that the application is being driven by 
	//  one or more CircleOfLatitude. 
	//  If the S2sphere is perfectly orthogonal when rendered, 
	//  then the user won't really be able to see the points 
	//  which comprise a circle of latitude. 
	//   
	//  So the S2sphere's axis of rotation 
	//  will be given some artificial tilt (e.g. ~20 degree) 
	//	solely to facilitate user visualization. 
	//      



	//   
	//	Animated rotation about the vertical y-axis: 
	//   
	DirectX::XMMATRIX mx_spin = XMMatrixRotationY(-1.f * proper_radians);


	//   
	//  The desired tilt of the y-axis: 
	//  
	float r_roll    = -18.f * XM_PI / 180.f;
	float r_pitch   = -60.f * XM_PI / 180.f;  //  or maybe use r_pitch = -70.f;
	float r_yaw = 0.f; 
	DirectX::XMMATRIX mx_tilt = XMMatrixRotationRollPitchYaw(r_pitch, r_yaw, r_roll); 


	//  
	//  Composition: spin about the y-axis then tilt the result:
	//  
	DirectX::XMMATRIX p_rot_mx = mx_spin * mx_tilt;  //  GOLD : spin * tilt; 


    return p_rot_mx;

	//    other ideas :  
	//    other ideas :  DirectX::XMMATRIX mx_inverse_tilt = XMMatrixInverse(nullptr, mx_tilt); 
	//    other ideas :  
}
//  Closes VHG_Scene3D1::do_S2sphere_rotation(); 






DirectX::XMMATRIX VHG_Scene3D1::do_Hopf_rotation(DX::StepTimer const& p_scene_timer)
{
    g0_total_seconds = p_scene_timer.GetTotalSeconds();
    float radiansPerSecond = XMConvertToRadians(m_degreesPerSecond);
    double total_sec = 0.f;

    if (e_option_Hopf_rotating) 
    {
        total_sec = g0_total_seconds - (g0_total_sec_resume - g0_total_sec_stop);  // perfect!!!
        g0_eff_total_seconds = total_sec;
    }
    else 
    {
        total_sec = g0_total_sec_stop;
    }

    double total_radians = total_sec * radiansPerSecond; 
    float proper_radians = static_cast<float>(fmod(total_radians, XM_2PI));

    DirectX::XMMATRIX x_rotation = XMMatrixRotationX(DirectX::XM_PI / 2.f); 
    DirectX::XMMATRIX p_rot_mx = XMMatrixIdentity();


    if (e_option_Hopf_x_axis_ortho)
    {
        //   The spiral's axis is vertical:
        x_rotation = XMMatrixRotationX(DirectX::XM_PI / 2.f); 
        p_rot_mx = x_rotation * XMMatrixRotationY(-1.f * proper_radians);
    }
    else
    {
        //    Looking into the corkscrew straight-on: 
        x_rotation = XMMatrixIdentity();
        p_rot_mx = x_rotation * XMMatrixRotationZ(-1.f * proper_radians);
    }

    return p_rot_mx;
}  
//  Closes VHG_Scene3D1::do_Hopf_rotation();













DirectX::SimpleMath::Vector3  VHG_Scene3D1::gv_Handle_KMI(float t_elapsed_time)
{
    DirectX::SimpleMath::Vector3       kminput_move = DirectX::SimpleMath::Vector3::Zero;


    //  
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //   
    //                        Mouse Handling: 
    //   
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //   
    //      
    DirectX::Mouse::State    maus_state = e_mouse->GetState();

    if (maus_state.positionMode == Mouse::MODE_RELATIVE)
    {
        Vector3 delta_mouse = ROTATION_GAIN * Vector3(float(maus_state.x), float(maus_state.y), 0.f);

        e_pitch -= delta_mouse.y;

        e_yaw -= delta_mouse.x;



        //    limit pitch to straight up or straight down
        //    with a small deviation to avoid gimbal lock  

#undef max
#undef min

        float gimbal_limit = XM_PI / 2.0f - 0.01f;
        e_pitch = std::max(-gimbal_limit, e_pitch);
        e_pitch = std::min(+gimbal_limit, e_pitch);

        // keep longitude within a sane range by wrapping  

        if (e_yaw > XM_PI)
        {
            e_yaw -= XM_PI * 2.0f;
        }
        else if (e_yaw < -XM_PI)
        {
            e_yaw += XM_PI * 2.0f;
        }


    }  //  Closes "if" the mouse mode is relative; 


    e_mouse->SetMode(maus_state.leftButton ? Mouse::MODE_RELATIVE : Mouse::MODE_ABSOLUTE);


    //  
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //   
    //                        Keyboard Handling: 
    //   
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //   
    //      
    DirectX::Keyboard::State           kb = e_keyboard->GetState();




    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    if (kb.Escape)
    {
        //   When using MODE_RELATIVE, the system cursor is hidden 
        //   so a user can't navigate away to another monitor or app or even exit. 
        //
        //   If your game makes use of 'mouse-look' controls, 
        //   you should ensure that a simple key (like the ESC key) 
        //   returns to the game's menu/pause screen 
        //   
        //   Moreover, that key press to pause must restore MODE_ABSOLUTE behavior.
        // 
        //   https://github.com/Microsoft/DirectXTK/wiki/Mouse 
        //   


        // ghv : todo :  assign some key to restore absolute behavior:    m_mouse->SetMode(Mouse::MODE_ABSOLUTE);


        Windows::ApplicationModel::Core::CoreApplication::Exit(); 

    }  //  Closes "Escape"; 


    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    if (kb.P)
    {
        auto av = Windows::UI::ViewManagement::ApplicationView::GetForCurrentView();
        if (!av->IsFullScreenMode)
        {
            if (av->TryEnterFullScreenMode())
            {
                av->FullScreenSystemOverlayMode = Windows::UI::ViewManagement::FullScreenSystemOverlayMode::Minimal;
            }
        }
    }  //  Closes "P"; 


    if (kb.L)
    {
        auto av = Windows::UI::ViewManagement::ApplicationView::GetForCurrentView();
        if (av->IsFullScreenMode) 
        {
            av->ExitFullScreenMode();
            av->FullScreenSystemOverlayMode = Windows::UI::ViewManagement::FullScreenSystemOverlayMode::Standard;
        }
    }  //  Closes "L"; 


    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



    if (kb.O)
    {       //   A request to resume the rotation: 
        if (!e_option_Hopf_rotating) {
            g0_total_sec_resume = g0_total_seconds;
            e_option_Hopf_rotating = true; 
        }
    }  //  Closes "O"; 

    if (kb.K)
    { //    Request to stop the rotation. 
        if (e_option_Hopf_rotating) {
            g0_total_sec_stop = g0_eff_total_seconds;   
            e_option_Hopf_rotating = false;
        }
    }  //  Closes "K"; 



    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++




    if (kb.I)
    {       //   A request to resume the rotation: 
        if (!e_option_S2sphere_rotating) {
            g0_S2sphere_total_sec_resume = g0_S2sphere_total_seconds;
            e_option_S2sphere_rotating = true; 
        }
    }  //  Closes "I"; 

    if (kb.J)
    { //    Request to stop the rotation. 
        if (e_option_S2sphere_rotating) {
            g0_S2sphere_total_sec_stop = g0_S2sphere_eff_total_seconds;   
            e_option_S2sphere_rotating = false;
        }
    }  //  Closes "J"; 



    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    if (kb.F2)
    {
        //   A request to re-mesh: Torus-Link-Torus or Standard view of fibration: 
        if (!g0_Button_Processed)
        {
            g0_Button_Processed = true;
            if (e_HopfSystem->e_TorusLinkTorus)
            {
                e_HopfSystem->e_TorusLinkTorus = false;
            }
            else
            {
                e_HopfSystem->e_TorusLinkTorus = true;
            }
            gv_map_unmap_vertex_buffer(0.f); 
        }
    }


    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


    if (kb.F3)
    { //   A request to show wireframe: 
        if (e_rasterizer_fill_mode_loft == D3D11_FILL_SOLID)
        {
            e_rasterizer_fill_mode_loft = D3D11_FILL_WIREFRAME;
        }
    }  //  Closes "F3"; 

    if (kb.F4)
    { //   A request to de-activate wireframe and return to "solid" rendering: 
        if (e_rasterizer_fill_mode_loft == D3D11_FILL_WIREFRAME)
        {
            e_rasterizer_fill_mode_loft = D3D11_FILL_SOLID;
        }
    }  //  Closes "F4"; 




    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    if (kb.F5)
    {
        if (e_conbuf_Loft_fiberColoringMode_data == 2)
        {
            e_conbuf_Loft_fiberColoringMode_data = 1;  //  Color per Fiber;
        }
    }  //  Closes "F5"; 

    if (kb.F6)
    {
        if (e_conbuf_Loft_fiberColoringMode_data == 1)
        {
            e_conbuf_Loft_fiberColoringMode_data = 2;  //  Color per Torus;
        }
    }  //  Closes "F6"; 

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifdef GHV_OPTION_ANIMATE_EXVERSION
    //  
    //  When animation is active, need to 
    //  disable the F8 keyboard control: 
    //   

    ;  // no-op;
#else
    if (kb.F8)
    {
        //   A request to re-mesh: 
        if (!g0_Button_Processed)
        {
            g0_Button_Processed = true;
            gv_map_unmap_vertex_buffer(8.f); 
        }
    }
#endif

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


#if 7 == 8
    if (kb.F11)
    {
        //   A request to change MeshObjectShape: 

        if (!g0_Button_Processed)
        {
            g0_Button_Processed = true;

            gv_next_mesh_object_shape(); 

            //  obfuscate : this->gv_
        }
    }  //  Closes "F11"; 
#endif 




    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    if (kb.F12)
    {
        //   A request to toggle the pi/2 rotation about x-axis:

        if (!g0_Button_Processed)
        {
            g0_Button_Processed = true; 

            if (e_option_Hopf_x_axis_ortho)
            {
                e_option_Hopf_x_axis_ortho = false;
            }
            else
            {
                e_option_Hopf_x_axis_ortho = true;
            }
        }
    }  //  Closes "F12"; 

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


    if (kb.Home)
    {
        e_camera_position = START_POSITION.v;
        e_pitch = e_yaw = 0;
    }


    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


    if (kb.Up || kb.W)
    {
        kminput_move.y += risk_speed * t_elapsed_time;  
    }

    if (kb.Down || kb.S)
    {
        kminput_move.y -= risk_speed * t_elapsed_time;   
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    if (kb.Left || kb.A)
    {
        kminput_move.x -= risk_speed * t_elapsed_time;
    }

    if (kb.Right || kb.D)
    {
        kminput_move.x  += risk_speed * t_elapsed_time;
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    if (kb.PageUp || kb.Space)
    {
        kminput_move.z += risk_speed * t_elapsed_time; 
    }

    if (kb.PageDown || kb.X)
    {
        kminput_move.z -= risk_speed * t_elapsed_time;
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

   
    return kminput_move;
}  
//  Closes VHG_Scene3D1::gv_Handle_KMI(); 







