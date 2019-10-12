//                      
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
//        
//                gv_Hopfbrau.h: Hopf Fibration methods 
//                       Garrett Vance 20180509
//                =====================================
//  The deep mathematics of the Hopf Fibration embodied in the following 
//  C++ methods are based on works published by Niles Johnson (see github.com) 
//  and David W. Lyons ("An Elementary Introduction to the Hopf Fibration").  
//    
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
//  
 
#pragma once

#include "..\Common\DeviceResources.h"
#include "..\Common\StepTimer.h"
#include "..\gv_ADJUSTABLE.h"



namespace VHG
{
    struct VHG_Axonodromal_Vertex
    {
        DirectX::XMVECTOR   axon_position_V;
        DirectX::XMUINT2    axon_fiber_torus_guid;
        float               axon_psi; 
        DirectX::XMVECTOR   curve_unit_tangent;
        DirectX::XMVECTOR   curve_unit_normal; // aka transported normal;
    };



    struct VHG_Instance
    {
        DirectX::XMFLOAT3    inst_pos;
    }; 


    struct VHG_Loft_Vertex
    {
        VHG_Loft_Vertex(void) :
            e_loft_pos(  DirectX::XMFLOAT3(0.f, 0.f, 0.f)),
            e_guids(DirectX::XMFLOAT3(0.f, 0.f, 0.f)),
            e_texco(DirectX::XMFLOAT2(0.f, 0.f)) {}

        // copy ctor
        VHG_Loft_Vertex(const  VHG_Loft_Vertex& a) : e_loft_pos(a.e_loft_pos), e_guids(a.e_guids), e_texco(a.e_texco) {}


        DirectX::XMFLOAT3   e_loft_pos;
        DirectX::XMFLOAT3   e_normal;
        DirectX::XMFLOAT3   e_guids;
        DirectX::XMFLOAT2   e_texco;
    }; 


    class VHG_Trajectory;  //  forward declaration; 

    class VHG_HopfSystem;  //  forward declaration; 




    class VHG_Fiber
    {
    public: 
        VHG_Fiber::VHG_Fiber(VHG_Trajectory *const  p_trajectory, UINT p_torus_guid, UINT p_fiber_guid, float p_sS2_x, float p_sS2_y, float p_sS2_z);

        std::vector<VHG::VHG_Axonodromal_Vertex>*   GetAxonodromalVertices(void)            { return e_axonodromal_vertices; }

        UINT                GetFiberGuid(void)          const   { return e_fiber_guid;  }

        void VHG_Fiber::HansonParallelTransportFrame(UINT p_torus_guid, UINT p_flatOffset);

        void VHG_Fiber::ReleaseHeapAllocatedMemory(void);

    private: 
        
        DirectX::XMVECTOR VHG_Fiber::hopf_fiberize(
            float p_angle_phi,
            float p_S2_u,   //   Use <u,v,w> rather than <x,y,z> because 
            float p_S2_v,   //   the arguments were probably swizzled...
            float p_S2_w
        );

        void VHG_Fiber::finite_differences(void);

    public:
        VHG_Trajectory *                                        e_trajectory;
    private:
        UINT                                                    e_fiber_guid;
        std::vector<VHG::VHG_Axonodromal_Vertex>                *e_axonodromal_vertices;
        std::vector<VHG_Loft_Vertex>                            *e_surface_points; 
    };
    //  Closes class VHG_Fiber;  






    class VHG_Trajectory
    {
    public: 
        VHG_Trajectory(VHG_HopfSystem* p_hopf_s, UINT p_arc_density); 

        UINT GetTopFiberGuid(void) { return (UINT)e_fibers->size(); }

        virtual void    Resolve(std::vector<VHG_Instance>* p_InstanceVector) = 0;

        void hopf_inverse_map(UINT p_torus_guid, float p_S2_x, float p_S2_y, float p_S2_z);
        
        virtual VHG_HopfSystem*     GetHopfSystem(void) { return e_systeme_hopf; }

        virtual UINT                GetAxonArcDensity(void)     const   { return e_axon_arc_density; } 

        virtual UINT                GetTorusGuid(void)          const   { return e_torus_guid;  }

        std::vector<VHG_Fiber*>   *GetAllFibers(void)  const { return e_fibers; }

    protected: 
        VHG_HopfSystem*                                 e_systeme_hopf;
        UINT                                            e_torus_guid;
        UINT                                            e_axon_arc_density;
        std::vector<VHG::VHG_Fiber*>                   *e_fibers;
    };
    //  Closes class VHG_Trajectory;  




    class VHG_HopfSystem
    {
    public:

        VHG_HopfSystem( 
            const std::shared_ptr<DX::DeviceResources>& p_deviceResources
        );

        std::vector<VHG_Trajectory*>   *GetAllTrajectories(void)  const { return e_trajectories; }

        float GetLoftCrossSectionEdge(void) { return e_loft_cross_section_edge; }

        UINT GetTopTorusGuid(void) { return (UINT)e_trajectories->size(); }

        ID3D11Buffer*   GetINodesVertexBuffer()     const   { return e_INodes_vertex_buffer.Get(); }
        ID3D11Buffer*   GetINodesInstanceBuffer()   const   { return e_INodes_instance_buffer.Get(); }
        ID3D11Buffer*   GetINodesIndexBuffer()      const   { return e_INodes_index_buffer.Get(); }

        void VHG_HopfSystem::mesh_S2sphere_INodes(void);

        void VHG_HopfSystem::CreateLoft(void);

        void VHG_HopfSystem::AddTrajectories(std::vector<VHG_Instance>* p_InstanceVector, float p_StepSize = 0.f);

    private:
        std::shared_ptr<DX::DeviceResources>            e_deviceResources;

        float                                           e_loft_cross_section_edge;

        Microsoft::WRL::ComPtr<ID3D11Buffer>            e_INodes_vertex_buffer;  // TODO: migrate to Scene class;
        Microsoft::WRL::ComPtr<ID3D11Buffer>            e_INodes_index_buffer;  // TODO: migrate to Scene class;

    public:
        UINT                                            e_INodes_instance_card;
        UINT                                            e_INodes_index_buffer_card;
        bool                                                    e_TorusLinkTorus;


    public:
        Microsoft::WRL::ComPtr<ID3D11Buffer>            e_INodes_instance_buffer;  // TODO: migrate to Scene class;
        std::vector<VHG_Trajectory*>                   *e_trajectories;
        std::vector<VHG_Loft_Vertex>                   *e_master_lofts; 

#ifdef GHV_OPTION_INDEX_BUFFER
        std::vector<DWORD>                             *e_master_indices; 
#endif

    };
    //  Closes class VHG_HopfSystem; 



    class VHG_GreatCircle : public VHG_Trajectory
    {
    public:
        VHG_GreatCircle(
            VHG_HopfSystem  *p_hopf_s,
            float p_planar_polar_inclination,
            UINT  p_number_of_fibers,
            float p_tau_begin,
            float p_tau_end, 
            UINT  p_axon_arc_density = 128
        ); 
        
        void                Resolve(std::vector<VHG_Instance>* p_InstanceVector);
        float               GetPlanarPolarInclination(void)     const   { return e_planar_polar_inclination; }
    private:
        UINT                                        e_num_requested_fibers;
        UINT                                        e_inode_ratio; 
        float                                       e_planar_polar_inclination;  // radians (Use 90 degrees, 88 GOLD,  66 off-center); 
        float                                       e_tau_begin;  //  GOLD : 0.f;
        float                                       e_tau_end;
    };
    //  Closes class VHG_GreatCircle;  



    class VHG_CircleOfLatitude : public VHG_Trajectory
    {
    public:
        VHG_CircleOfLatitude(
            VHG_HopfSystem  *p_hopf_s,
            float p_polar_phi_angle,
            UINT  p_number_of_fibers,
            float p_azimuth_angle_begin,
            float p_azimuth_angle_end, 
            UINT  p_axon_arc_density = 128
        );

        void                Resolve(std::vector<VHG_Instance>* p_InstanceVector);
    private: 
        UINT                                        e_num_requested_fibers;
        UINT                                        e_inode_ratio;
        float                                       e_polar_phi_angle;
        float                                       e_azimuth_begin;
        float                                       e_azimuth_end;
    };
    //  Closes class VHG_CircleOfLatitude; 


}  //  Closes namespace VHG; 



                                  
