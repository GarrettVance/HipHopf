//                      
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
//        
//               gv_Hopfbrau.cpp: Hopf Fibration methods 
//                       Garrett Vance 20180509
//                =====================================
//  The deep mathematics of the Hopf Fibration embodied in the following 
//  C++ methods are based on works published by Niles Johnson (see github.com) 
//  and David W. Lyons ("An Elementary Introduction to the Hopf Fibration").  
//    
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
//  

#include "pch.h"
#include "gv_SCENE_3D.h"
#include "..\Common\DirectXHelper.h"
#include "gv_Hopfbrau.h"
#include <string>

using namespace VHG;
using namespace Windows::Foundation;
using namespace DirectX;
using namespace DirectX::SimpleMath;




VHG_Fiber::VHG_Fiber(
    VHG_Trajectory *const  p_trajectory, 
    UINT p_torus_guid, 
    UINT p_fiber_guid, 
    float p_sS2_x, 
    float p_sS2_y, 
    float p_sS2_z
) : e_trajectory(p_trajectory), e_fiber_guid(p_fiber_guid)
{
    UINT ArcDen = p_trajectory->GetAxonArcDensity();

    //  pre-Allocate all elements of vectors: 
    e_axonodromal_vertices = new std::vector<VHG::VHG_Axonodromal_Vertex>(ArcDen);
    e_surface_points = new std::vector<VHG_Loft_Vertex>(VHG_Scene3D1::tubeFacets * ArcDen);

    //   Use the full 2 pi radians of psi to obtain closed (ouroboros) circles: 
    float                       psi_begin = 0.f; 
    float                       psi_end = 2.f * XM_PI; // need psi_end = 2pi for closed circles;

    VHG_Axonodromal_Vertex      axonal_fiber_vertex;
    for (uint32_t idx_psi = 0; idx_psi < ArcDen; idx_psi++)
    {
        float angle_psi = psi_begin + idx_psi * (psi_end - psi_begin) / (-1 + ArcDen); 

        //   To simplifiy rendering the INode at the north pole, add pi/2: 
        //  undo angle_psi += XM_PIDIV2;

        if (this->e_trajectory->GetHopfSystem()->e_TorusLinkTorus)
        {
            axonal_fiber_vertex.axon_position_V = hopf_fiberize(angle_psi, p_sS2_x, p_sS2_y, p_sS2_z); // experimental
        }
        else
        {
            axonal_fiber_vertex.axon_position_V = hopf_fiberize(angle_psi, p_sS2_x, p_sS2_z, p_sS2_y); // swizzled to <x,z,y>!!!
        }

        axonal_fiber_vertex.axon_fiber_torus_guid = XMUINT2(p_fiber_guid, p_torus_guid);
        axonal_fiber_vertex.axon_psi = angle_psi;
        e_axonodromal_vertices->at(idx_psi) = axonal_fiber_vertex;
    }

    finite_differences(); 
}
//  Closes VHG_Fiber::VHG_Fiber() class ctor;





DirectX::XMVECTOR VHG_Fiber::hopf_fiberize(
    float p_angle_psi,
    float p_S2_u,   //   Use <u,v,w> rather than <x,y,z> because 
    float p_S2_v,   //   the arguments were probably swizzled...
    float p_S2_w
)
{
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //  Given a point <x,y,z> on the S2 sphere and a value of p_angle_psi
    //  calculate the single corresponding point in the fiber; 
    //  Each fiber is thus parameterized by p_angle_psi. 
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    float self_al = sqrt(0.5f * (1.f + p_S2_w));
    float self_be = sqrt(0.5f * (1.f - p_S2_w));
    float angle_theta = atan2(-p_S2_u, p_S2_v) - p_angle_psi;

    float s_w = self_al * cos(angle_theta);

    float s_x = -1.f * self_be * cos(p_angle_psi);
    float s_y = -1.f * self_be * sin(p_angle_psi);
    float s_z = self_al * sin(angle_theta);

    float rr = acos(s_w) * DirectX::XM_PI * (1.f / sqrt(1.f - s_w * s_w));

    rr *= 0.22f;  //  scaling;

    XMVECTOR V13 = XMVectorSet(s_x * rr, s_y * rr, s_z * rr, 0.f);
    return V13; 
}
//  Closes VHG_Fiber::hopf_fiberize();






void VHG_Fiber::finite_differences(void)
{
    UINT const u_axon_arc_density = this->e_trajectory->GetAxonArcDensity();
    double const e_underflow = 0.000001;  // undo;

    for (UINT idx_nodes = 0; idx_nodes < u_axon_arc_density; idx_nodes++)
    {
        //    Obtain an approximate first derivative dr/dt 
        //    (aka tangent vector) via finite difference techniques:

        UINT idx_neung = idx_nodes;
        UINT idx_song = (idx_nodes == (-1 + u_axon_arc_density)) ? 0 : 1 + idx_nodes;

        //  assert the space curve is a closed loop (ouroboros); 

        double dt =
            e_axonodromal_vertices->at(idx_song).axon_psi -
            e_axonodromal_vertices->at(idx_neung).axon_psi;

        if (abs(dt) < e_underflow)
        {
            if (dt < 0.00) dt = -1.00 * e_underflow;
            else if (dt > 0.00) dt = e_underflow;
        }

        XMVECTOR drdt = 
            (e_axonodromal_vertices->at(idx_song).axon_position_V - 
             e_axonodromal_vertices->at(idx_neung).axon_position_V) / (float)dt;

        //   normalize the drdt vector to obtain unit tangent vector;

        e_axonodromal_vertices->at(idx_neung).curve_unit_tangent = XMVector3Normalize(drdt); 
    }
}
//  Closes VHG_Fiber::gv_finite_differences();  




//              Trajectory Class,   GreatCircle and CircleOfLatitude: 


VHG_Trajectory::VHG_Trajectory(VHG_HopfSystem* p_hopf_s, UINT p_arc_density) 
    : 
    e_systeme_hopf(p_hopf_s), 
    e_axon_arc_density(p_arc_density)
{
} 






VHG_GreatCircle::VHG_GreatCircle( 
    VHG_HopfSystem  *p_hopf_s, 
    float p_planar_polar_inclination, 
    UINT  p_number_of_fibers, 
    float p_tau_begin, 
    float p_tau_end, 
    UINT  p_axon_arc_density
) : 
    VHG_Trajectory(p_hopf_s, p_axon_arc_density), 
    e_planar_polar_inclination(p_planar_polar_inclination),
    e_num_requested_fibers(p_number_of_fibers), 
    e_tau_begin(p_tau_begin), 
    e_tau_end(p_tau_end)
{
    if (p_number_of_fibers > 63)
    {
        e_inode_ratio = 4;
    }
    else if (p_number_of_fibers > 31)
    {
        e_inode_ratio = 2;
    }
    else
    {
        e_inode_ratio = 1;
    }

    assert(p_hopf_s != nullptr);

    e_fibers = new std::vector<VHG::VHG_Fiber*>();  //  e_fibers is "protected" in Base Class;

    this->e_systeme_hopf->GetAllTrajectories()->push_back(this);

    e_torus_guid = (UINT)(-1 + p_hopf_s->GetAllTrajectories()->size());
}
//  Closes VHG_GreatCircle::VHG_GreatCircle(); 




VHG_CircleOfLatitude::VHG_CircleOfLatitude(
    VHG_HopfSystem*     p_hopf_s, 
    float p_polar_phi_angle,
    UINT  p_number_of_fibers,
    float p_azimuth_angle_begin,
    float p_azimuth_angle_end, 
    UINT  p_axon_arc_density
) : 
    VHG_Trajectory(p_hopf_s, p_axon_arc_density), 
    e_polar_phi_angle(p_polar_phi_angle),
    e_num_requested_fibers(p_number_of_fibers),
    e_azimuth_begin(p_azimuth_angle_begin),
    e_azimuth_end(p_azimuth_angle_end)
{
    if (p_number_of_fibers > 63) 
    {
        e_inode_ratio = 4;
    }
    else if (p_number_of_fibers > 31) 
    {
        e_inode_ratio = 2;
    }
    else 
    {
        e_inode_ratio = 1;
    }

    assert(p_hopf_s != nullptr);

    e_fibers = new std::vector<VHG::VHG_Fiber*>();  //  e_fibers is "protected" in Base Class;

    this->e_systeme_hopf->GetAllTrajectories()->push_back(this);

    e_torus_guid = (UINT)(-1 + p_hopf_s->GetAllTrajectories()->size());
}
//  Closes VHG_CircleOfLatitude::VHG_CircleOfLatitude();  



void VHG_Trajectory::hopf_inverse_map(UINT p_torus_guid, float p_S2_x, float p_S2_y, float p_S2_z)
{
    //          Create a new Fiber in the given Torus  // TODO: align terminology to either torus or trajectory.
    //          =====================================
    //  This method "hopf_inverse_map()" is the entry point to the 
    //  inverse Hopf map. For the purpose of this application, 
    //  Fibers are 1-1 with invocations of hopf_inverse_map(), 
    //  so each invocation of hopf_inverse_map() will increment FiberGuid. 
    //      

    //  It can be seen below that each Fiber (the rendered space curve
    //  in R3) is parameterized by angle_psi.
    //  
    //  ??? Is angle_psi somehow formally related to arc length ???
    //   
    //  Perhaps there is a formal relation, but empirically 
    //  equal steps in "angle_psi" will render as widely varying 
    //  increments of Fiber arc length. This is well demonstrated 
    //  using a GreatCircle Trajectory whose angle of polar inclination
    //  is near zero: many of the resulting R3 Fibers are nice circles, 
    //  but there are also pathological Fibers which render like 
    //  a majuscule English letter "D" with a nearly straight line segment,
    //  two well-behaved corners at each end of that vertical segment, 
    //  and then an arc of huge radius. Using the optional WIREFRAME view
    //  of this R3 Fiber, that big-radius arc segment takes huge steps 
    //  along its arc length, although the behind-the-scenes increments 
    //  of angle_psi are all of them uniform. 
    //  
    //  This motivates finding an adaptive method to improve rendering 
    //  of that awful big-radius arc...
    //          

    UINT fiber_guid = 0; 
    
    if (e_fibers != nullptr)
    {
        fiber_guid = (UINT)this->e_fibers->size();
    }

    VHG_Fiber    *the_new_fiber = new VHG_Fiber(this, p_torus_guid, fiber_guid, p_S2_x, p_S2_y, p_S2_z); 

    this->e_fibers->push_back(the_new_fiber);
}
//  Closes VHG_Trajectory::hopf_inverse_map();










void VHG_CircleOfLatitude::Resolve(std::vector<VHG_Instance>* p_InstanceVector)
{
    //  undo  UINT number_of_fibers = this->GetNumberOfFibers();

    VHG_Instance            current_instance;


    for (UINT idx_theta = 0; idx_theta < e_num_requested_fibers; idx_theta++)
    {
        float azimuth_angle_theta = e_azimuth_begin + idx_theta * (e_azimuth_end - e_azimuth_begin) / e_num_requested_fibers;

        float const whence_two = 1.f;
        float x_coord = sinf(whence_two * e_polar_phi_angle) * cosf(azimuth_angle_theta);
        float y_coord = sinf(whence_two * e_polar_phi_angle) * sinf(azimuth_angle_theta);
        float z_coord = cosf(whence_two * e_polar_phi_angle);

        if ( idx_theta % this->e_inode_ratio == 0 )  
        {
            current_instance.inst_pos = XMFLOAT3(
                x_coord,
                z_coord,  // swizzled;
                y_coord   // swizzled;
            );

            p_InstanceVector->push_back(current_instance);
        }

        this->hopf_inverse_map( 
            e_systeme_hopf->GetTopTorusGuid(),
            x_coord, 
            z_coord,  //  swizzled;
            y_coord   //  swizzled;
        );
    }
}
//  Closes VHG_CircleOfLatitude::Resolve();  




void VHG_GreatCircle::Resolve(std::vector<VHG_Instance>* p_InstanceVector)
{
    //  undo   UINT number_of_fibers = this->GetNumberOfFibers();


    VHG_Instance            current_instance;

    float k_sin = sinf(this->GetPlanarPolarInclination());
    float k_cos = cosf(this->GetPlanarPolarInclination());

    XMFLOAT3 Pu_f3 = XMFLOAT3((float)k_sin, 0.f, (float)k_cos);
    XMVECTOR Pu_xmv = XMLoadFloat3(&Pu_f3);

    XMFLOAT3 Pv_f3 = XMFLOAT3(0.f, 1.f, 0.f);
    XMVECTOR Pv_xmv = XMLoadFloat3(&Pv_f3);

    XMFLOAT3 gc_pos_fl3; 
    XMVECTOR gc_pos_xmv;

    for (UINT idx_tau = 0; idx_tau < e_num_requested_fibers; idx_tau++)
    {
        float param_tau = e_tau_begin + idx_tau * (e_tau_end - e_tau_begin) / e_num_requested_fibers;

        gc_pos_xmv = cos(param_tau) * Pu_xmv + sin(param_tau) * Pv_xmv;

        XMStoreFloat3(&gc_pos_fl3, gc_pos_xmv);

        if ((idx_tau % this->e_inode_ratio) == 0)
        {
            current_instance.inst_pos = XMFLOAT3(
                gc_pos_fl3.x,
                gc_pos_fl3.z,  // swizzled;
                gc_pos_fl3.y   // swizzled;
            );

            p_InstanceVector->push_back(current_instance);
        }

        this->hopf_inverse_map(
            e_systeme_hopf->GetTopTorusGuid(),
            gc_pos_fl3.x,
            gc_pos_fl3.z,  // swizzled;
            gc_pos_fl3.y   // swizzled;
        );
    }
}
//  Closes VHG_GreatCircle::Resolve();  





VHG_HopfSystem::VHG_HopfSystem( 
    const std::shared_ptr<DX::DeviceResources>& p_deviceResources
) : 
    e_deviceResources(p_deviceResources),  
    e_loft_cross_section_edge(0.02f),  // e_loft_cross_section_edge between 0.01f and 0.03f; aka tube_radius;
    e_TorusLinkTorus(false)
{
    e_trajectories = new std::vector<VHG::VHG_Trajectory*>();
}
//  Closes VHG_HopfSystem::VHG_HopfSystem() ctor;





void VHG_HopfSystem::CreateLoft(void)
{
    if (e_master_lofts != nullptr)
    {
        delete e_master_lofts;
        e_master_lofts = nullptr;
    }


#ifdef GHV_OPTION_INDEX_BUFFER
    if (e_master_indices != nullptr)
    {
        delete e_master_indices;
        e_master_indices = nullptr;
    }
#endif 



    UINT grandTotalVertices = 0;
    std::vector<VHG_Trajectory*> *all_traj = this->GetAllTrajectories();
    std::vector<VHG_Trajectory*>::iterator it_traj;
    for (it_traj = all_traj->begin(); it_traj != all_traj->end(); it_traj++)
    {
        std::vector<VHG_Fiber*>               *all_fib = (*it_traj)->GetAllFibers();
        std::vector<VHG_Fiber*>::iterator     it_fib;
        for (it_fib = all_fib->begin(); it_fib != all_fib->end(); it_fib++)
        {
            grandTotalVertices += 
                VHG_Scene3D1::e_verts_per_quad *
                VHG_Scene3D1::tubeFacets *
                (*it_traj)->GetAxonArcDensity();
        }
    }

    e_master_lofts = new std::vector<VHG_Loft_Vertex>(grandTotalVertices); 


#ifdef GHV_INDEX_BUFFER
    e_master_indices = new std::vector<DWORD>(grandTotalVertices); 
#endif 


    std::vector<VHG_Trajectory*> *all_trajectories = this->GetAllTrajectories();
    std::vector<VHG_Trajectory*>::iterator iter_all_trajects;

    UINT flatOffset = 0;  

    // std::vector<VHG_VectorMetadata> vectorMetadata;
    Windows::Globalization::Calendar^ calendar = ref new Windows::Globalization::Calendar(); 

    for (
        iter_all_trajects = all_trajectories->begin(); 
        iter_all_trajects != all_trajectories->end();
        iter_all_trajects++
        )
    {
        std::vector<VHG_Fiber*>               *all_fibers = (*iter_all_trajects)->GetAllFibers();
        std::vector<VHG_Fiber*>::iterator     iter_all_fibers; 

        for (
            iter_all_fibers = all_fibers->begin();
            iter_all_fibers != all_fibers->end();
            iter_all_fibers++
            )
        {
            calendar->SetToNow();
            Windows::Foundation::DateTime beforeTime = calendar->GetDateTime();
            VHG::VHG_Scene3D1::e_ghv_loggingChannel->LogMessage("CreateLoft: Hanson_pre;"); 

            (*iter_all_fibers)->HansonParallelTransportFrame(
                (*iter_all_trajects)->GetTorusGuid(),
                flatOffset
            );

            flatOffset += 
                VHG_Scene3D1::e_verts_per_quad *
                VHG_Scene3D1::tubeFacets *
                (*iter_all_trajects)->GetAxonArcDensity();


            calendar->SetToNow();
            Windows::Foundation::DateTime  afterTime = calendar->GetDateTime();
            Windows::Foundation::TimeSpan duration;
            duration.Duration = afterTime.UniversalTime - beforeTime.UniversalTime;
            std::wstring ghvStr = L"CreateLoft: Hanson_post "; 
            ghvStr += std::to_wstring(duration.Duration);
            Platform::String^ xxx = ref new Platform::String(ghvStr.c_str()); 
            VHG::VHG_Scene3D1::e_ghv_loggingChannel->LogMessage( xxx ); 
        }
    }







    XMFLOAT2 nul2 = XMFLOAT2(0.f, 0.f); 
    XMFLOAT3 nul3 = XMFLOAT3(0.f, 0.f, 0.f); 
    XMVECTOR nulXMV = XMVectorSet(0.f, 0.f, 0.f, 0.f); 







    int32_t countOfVertices = (int32_t)e_master_lofts->size(); 


#ifdef GHV_OPTION_INDEX_BUFFER

    int32_t countOfIndexEntries = (int32_t)e_master_indices->size(); 
    int32_t countOfTriangles = countOfIndexEntries / 3; 
    DWORD *arrTriangles = new DWORD[countOfIndexEntries]{ 0 }; 

#else

    int32_t countOfTriangles = countOfVertices / 3; 
    DWORD *arrTriangles = new DWORD[countOfVertices]{ 0 }; 

#endif



    int32_t idxCWS = 0; 

#ifdef GHV_OPTION_INDEX_BUFFER
    std::vector<DWORD>::iterator itCWS; 
    for (itCWS = e_master_indices->begin(); itCWS != e_master_indices->end(); itCWS++)
    {
        DWORD x1 = *(itCWS + 0);
        arrTriangles[idxCWS] = x1;
        idxCWS++;
    }
#else
    
    for (idxCWS = 0; idxCWS < countOfVertices; idxCWS++)
    {
        arrTriangles[idxCWS] = idxCWS;
    }
    
    
#endif 






    XMFLOAT3 *arrVertices = new XMFLOAT3[countOfVertices]{ nul3 }; 
    XMFLOAT2 *arrTexco = new XMFLOAT2[countOfVertices]{ nul2 }; 

    int32_t idxVertx = 0; 
    std::vector<VHG_Loft_Vertex>::iterator itVertx; 
    for (itVertx = e_master_lofts->begin(); itVertx != e_master_lofts->end(); ++itVertx) 
    {
        arrVertices[idxVertx] = (*itVertx).e_loft_pos; 

        //  later:  
        arrTexco[idxVertx] = (*itVertx).e_texco;

        idxVertx++;
    }


    XMFLOAT3 *normals = new XMFLOAT3[countOfVertices]{nul3}; 

    DirectX::ComputeNormals(
        (uint32_t*)arrTriangles, 
        countOfTriangles, 
        arrVertices, 
        countOfVertices, 
        CNORM_DEFAULT,
        normals
    ); 

    //  Move the newly generated normals data into the e_master_lofts vector: 

    idxVertx = 0; 
    for (itVertx = e_master_lofts->begin(); itVertx != e_master_lofts->end(); ++itVertx)
    {
        (*itVertx).e_normal = normals[idxVertx]; 
        idxVertx++;
    }

    UINT debugCardMeta = 505;
}
//  Closes VHG_HopfSystem::CreateLoft(); 






void VHG_HopfSystem::AddTrajectories(std::vector<VHG_Instance>* p_InstanceVector, float p_StepSize)
{
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //      Good pair for visualization: use two VHG_CircleOfLatitude
    //      one having polar angle 6pi/32, the other with 25pi/32. 
    // 
    //      The INodes in S2sphere's southern hemisphere become visible 
    //      when WIREFRAME mode is used...
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


    static int number_of_calls = 0; 

    if (p_StepSize > 0.5f)
    {
        number_of_calls++; 
    }
    else
    {
        number_of_calls = 0;
    }


    //      If render a CircleOfLatitude Trajectory with e.g. polar angle 3 degrees, 
    //      then that's a small circle drawn about the north pole of the S2sphere. 
    //      And when INodes are near the north pole of S2sphere, 
    //      the pathologically wide arcs appear in the R3 Fibers. 
    //   

    VHG_CircleOfLatitude  *isolatitudinal_1 = new VHG_CircleOfLatitude(
        this, 
        (42.f + p_StepSize * number_of_calls) * XM_PI / 180.f, //  polar_angle_phi = 52 pi / 180 (1-1 with values of latitude);
        32,      //  number of fibers = 32;
        XM_PI / 4.f,   //  0.f,    //  azimuth starting angle;
        2.f * XM_PI,  //  azimuth final angle;
        512       // axon_arc_density = 256; 
    );


    VHG_CircleOfLatitude  *isolatitudinal_2 = new VHG_CircleOfLatitude(
        this, 
        137.f * XM_PI / 180.f,  //  polar_angle_phi = 127 pi / 180; 
        32,         //  number of fibers;
        XM_PI / 4.f,   //  0.f,  //  azimuth starting angle;
        2.f * XM_PI,   //  azimuth final angle;
        256       // axon_arc_density = 256; 
    );


#if 5 == 6
    //          Important Case : the Nearly Vertical Great Circle 
    //          =================================================
    //          Use polar inclination = 1 degree;
    //          Number of Fibers: 32; 
    //          tau_begin:  zero   (the INode at the north pole of S2sphere); 
    //          tau_end:    pi     (the INode at the south pole of S2sphere); 
    //          Arc density:   2048 or 1024; 
    //          

    VHG_GreatCircle  *gc_1 = new VHG_GreatCircle(
        this,
        28.f * XM_PI / 180.f,   //  inclination close to zero, e.g. 4.f degrees...
        32,         //  number_of_fibers, e.g. four (4) fibers is a good look for the near-vertical GreatCircle...
        0.5f * XM_PI,        //  tau_begin;
        2.f * XM_PI,      //  tau_end;
        2048   //  Great Circle requires axon arc density >= 1024 when inclination near zero; 
    );
#endif 

    std::vector<VHG_Trajectory*>::iterator traj_it;
    for (traj_it = e_trajectories->begin(); traj_it != e_trajectories->end(); traj_it++)
    {
        (*traj_it)->Resolve(p_InstanceVector);
    }
}
//  Closes VHG_HopfSystem::AddTrajectories();



 





    //  Define the great circle by two points on the sphere S2, Pu, Pv:
    //  
    //  Utilize a plane which makes a polar angle of 30 degrees with the vertical
    //  z-axis to slice the S2 sphere along a great circle. 
    //  Furthermore assume that the axis of rotation of this 30-degree polar angle 
    //  is y-axis   (x-axis left-to-right, y-axis into-and-outof the paper, 
    //  z-axis vertical). 
    // 
    //  
    //  Point Pu is the intersection of the sphere S2 with this plane. 
    //  By choice of the axis of rotation, Pu.y = 0. 
    //  Pu.x = r * sin30,   Pu.z = r * cos30;
    //
    //  S2 is the unit sphere, so r = 1.
    //
    //  Pu = (sin30, 0, cos30);
    //  
    //  Choose point Pv to be the intersection of the plane, 
    //  the sphere S2 and the positive y-axis, so
    //  
    //  Pv = (0, 1, 0);
    //                    
    //  The vector dot product Pu dot Pv = 0, so Pu and Pv are perpendicular vectors;
    // 
    //  The vectors Pu and Pv are also unit vectors by inspection. 
    //  Consequently n_hat = Pu cross Pv is a unit normal to the plane of 
    //  the great circle. 
    //   
    //  n_hat = Pu cross  Pv;
    //  
    //  Let w = n_hat cross Pu. 
    //  
    //  Then unit vectors <Pu, w> are an orthonormal basis for the great circle.
    //  
    //  ??? But isn't  <Pu, Pv> as defined above already an orthonormal basis for the great circle??? 
    //  
    //  pos(t) = Pu cos(t) + Pv sin(t);
    //  










void VHG_Fiber::ReleaseHeapAllocatedMemory(void)
{
    if (e_surface_points)
    {
        delete e_surface_points;
        e_surface_points = nullptr;
    }

    if (e_axonodromal_vertices)
    {
        delete e_axonodromal_vertices;
        e_axonodromal_vertices = nullptr;
    }
}
//  Closes VHG_HopfSystem::ReleaseHeapAllocatedMemory(); 







void VHG_Scene3D1::gv_load_hopf_system(void)
{
    //      must call ~VHG_HopfSystem destructor to free the heap-allocated vectors, 
    //      then must call  the VHG_HopfSystem(.....params.....) constructor again to re-create the geometry; 
    //  Must reset e_fiber_guid back to zero every time geometry is created...
    //  Must reset e_torus_guid back to zero every time geometry is created...
    //    

    mesh_MQuad();  //  can be called early since has no dependencies;

    e_HopfSystem->mesh_S2sphere_INodes(); //  calls AddTrajectories() which calls Resolve() which makes all calls to hopf_inverse_map(); 


    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //     all invocations of hopf_inverse_map() have completed; 
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


    e_HopfSystem->CreateLoft();




    if (e_vertex_buffer_loft.Get() == nullptr)
    {       
        loft_vertex_buffer_create();
    }


    //  TODO:   memory leak :  e_HopfSystem->ReleaseHeapAllocatedMemory();


}  
//  Closes VHG_Scene3D1::gv_load_hopf_system(); 





//                 ...file ends... 

