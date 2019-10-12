//                      
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
//        
//    
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
//  
 
#pragma once


#include "..\Common\DeviceResources.h"
#include "..\Common\StepTimer.h"
#include "GeometricPrimitive.h"

#include "gv_Hopfbrau.h"








namespace VHG
{


    struct VHG_Vertex_PosTex  // Used  for the INodes on sphere S2; 
    {
        DirectX::XMFLOAT3       e_pos;
        DirectX::XMFLOAT2       e_texco;
    };





    class VHG_Scale
    {
    public:
        VHG_Scale(float p_scale_factor) : e_scale_factor(p_scale_factor) {}


        void posApply(std::vector<VHG_Vertex_PosTex> & p_vectpostex)
        {
            for_each(
                p_vectpostex.begin(),
                p_vectpostex.end(),

                //      Important : pass p_postex BY REFERENCE 
                //      otherwise won't be able to alter it!!! 

                [this](VHG_Vertex_PosTex & p_postex) {
                p_postex.e_pos.x *= this->e_scale_factor;
                p_postex.e_pos.y *= this->e_scale_factor;
                p_postex.e_pos.z *= this->e_scale_factor;
            }
            );
        }

    private:
        float e_scale_factor;
    };




    struct VHG_conbuf_MVP_struct  // Used by the INodes on sphere S2; 
    {
        DirectX::XMFLOAT4X4     model_transform;
        DirectX::XMFLOAT4X4     view_transform;
        DirectX::XMFLOAT4X4     projection_transform; 
        DirectX::XMUINT4        animator_count; 
    };


    struct VHG_conbuf_ViewProjectionTransform_struct  // Used by MQuad; 
    {
        DirectX::XMFLOAT4X4 view_transform;
        DirectX::XMFLOAT4X4 projection_transform;
    };


    struct VHG_conbuf_ModelTransform_struct // Used by MQuad; 
    {
        DirectX::XMFLOAT4X4 model_transform;
    };



    // Constant buffer used to send hologram position and normal transforms to the shader pipeline.
    struct ModelNormalConstantBuffer
    {
        DirectX::XMFLOAT4X4 modelToWorld;
        DirectX::XMFLOAT4X4 normalToWorld;
        DirectX::XMFLOAT4   colorFadeFactor;
    };

    // Constant buffer used to send the view-projection matrices to the shader pipeline.
    struct ViewProjectionConstantBuffer
    {
        DirectX::XMFLOAT4   cameraPosition;
        DirectX::XMFLOAT4   lightPosition;
        DirectX::XMFLOAT4X4 viewProjection;
    };

















    struct VHG_VectorMetadata
    {
        UINT            idxTrajectory;
        UINT            idxFiber; 
        UINT            cardVBVertices;
        UINT            cardIBIndices;
    };





    struct VHG_MQuad_Vertex
    {
        VHG_MQuad_Vertex(DirectX::XMFLOAT3 p_pos, DirectX::XMFLOAT3 p_color) : e_pos(p_pos), e_color(p_color) {}

        DirectX::XMFLOAT3       e_pos;
        DirectX::XMFLOAT3       e_color;
    };







    class VHG_Scene3D1
    {
    public:
        VHG_Scene3D1(const std::shared_ptr<DX::DeviceResources>& deviceResources);



        static Windows::Foundation::Diagnostics::LoggingChannel^   e_ghv_loggingChannel; // 20191005;



        static UINT const e_verts_per_quad;
        static UINT const tubeFacets;


        void VHG_Scene3D1::mesh_MQuad(void);  //  MiniSphere;


        uint32_t VHG_Scene3D1::CountRenderedTriangles(void) const 
        {
            uint32_t nTris = 0; 
            if (e_HopfSystem->e_master_lofts != nullptr) // fixed bug, fixed crash;
            {
                nTris = (uint32_t)this->e_HopfSystem->e_master_lofts->size() / 3; 
            }
            return nTris;
        }


        uint32_t     VHG_Scene3D1::gv_GetAdvertCount(void)
        {
            return e_advert_count;
        }


        uint32_t     VHG_Scene3D1::gv_GetAdvertModulus(void)
        {
            return e_advert_modulus;
        }

        void VHG_Scene3D1::AdvertUpdate(); 



        float VHG_Scene3D1::GV_Get_Yaw(void) const { return e_yaw; }
        float VHG_Scene3D1::GV_Get_Pitch(void) const { return e_pitch; }

        void CreateDeviceDependentResources();
        void CreateWindowSizeDependentResources(void);
        void ReleaseDeviceDependentResources();




        void gv_map_unmap_vertex_buffer(float p_StepSize = 0.f);

        void VHG_Scene3D1::UpdateDynamic_ColoringMode(void);

        void Update3DScene(DX::StepTimer const& timer);





        void VHG_Scene3D1::Render3DScene_PTF_Loft_2(void);
        void Render3DScene_PTF();   //  brand-new Parallel Transport Frames;
        void Render3DScene();       //  < called from main > 
        void VHG_Scene3D1::RenderMQuad(void); //  MiniSphere;
        void VHG_Scene3D1::RenderS2sphere(void);  //  MiniSphere;


    private:
            
        DirectX::SimpleMath::Vector3  VHG_Scene3D1::gv_Handle_KMI(float t_elapsed_time);

        void VHG_Scene3D1::debouncer(void);

        void VHG_Scene3D1::gv_load_hopf_system(void); 

        void VHG_Scene3D1::loft_vertex_buffer_create(void);

        void VHG_Scene3D1::Create_Loft_Input_Layout( const std::vector<byte>& p_byte_vector );

        void VHG_Scene3D1::Load_Texture_Triax(void); 
        void VHG_Scene3D1::Create_Sampler_State_Loft(void);


        void VHG_Scene3D1::Create_Rasterizer_State_Loft(void); 
        void VHG_Scene3D1::Create_Rasterizer_State_Wireframe(void);
        void VHG_Scene3D1::Create_Rasterizer_State_Solid(void);



        void VHG_Scene3D1::S2sphere_INodes_Animator(void);
        void VHG_Scene3D1::GV_Camera_Init(void);




        DirectX::XMMATRIX VHG_Scene3D1::do_Hopf_rotation(DX::StepTimer const& p_scene_timer );

        DirectX::XMMATRIX VHG_Scene3D1::do_S2sphere_rotation(DX::StepTimer const& p_scene_timer);

    private: 
        std::shared_ptr<DX::DeviceResources>                m_deviceResources;

        std::unique_ptr<VHG::VHG_HopfSystem>                e_HopfSystem;

        Microsoft::WRL::ComPtr<ID3D11Buffer>                e_vertex_buffer_loft;
        Microsoft::WRL::ComPtr<ID3D11Buffer>                e_index_buffer_loft;
        Microsoft::WRL::ComPtr<ID3D11InputLayout>           e_input_layout_loft;
        Microsoft::WRL::ComPtr<ID3D11VertexShader>          e_vertex_shader_loft;
        Microsoft::WRL::ComPtr<ID3D11PixelShader>           e_pixel_shader_loft;


        Microsoft::WRL::ComPtr<ID3D11Buffer>                e_conbuf_ModelNormal_buffer;
        ModelNormalConstantBuffer                           e_conbuf_ModelNormal_data;

        Microsoft::WRL::ComPtr<ID3D11Buffer>                e_conbuf_ViewProjection_buffer;
        ViewProjectionConstantBuffer                        e_conbuf_ViewProjection_data;

        Microsoft::WRL::ComPtr<ID3D11Buffer>                e_conbuf_Loft_fiberColoringMode_buffer;
        int                                                 e_conbuf_Loft_fiberColoringMode_data;


        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    e_texture_srv_Loft_Pure_Fail;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    e_texture_srv_Loft_Ferrero_Rocher;


        bool                                                e_option_Hopf_rotating;
        bool                                                e_option_Hopf_x_axis_ortho;



        //  MiniSphere and MQuad: 

        Microsoft::WRL::ComPtr<ID3D11Buffer>                e_vertex_buffer_MQuad;  //  MiniSphere;
        UINT                                                e_ib_indices_card_MQuad;
        Microsoft::WRL::ComPtr<ID3D11Buffer>                e_index_buffer_MQuad;  //  MiniSphere;
        Microsoft::WRL::ComPtr<ID3D11InputLayout>           e_input_layout_MQuad;
        Microsoft::WRL::ComPtr<ID3D11VertexShader>          e_vertex_shader_MQuad;
        Microsoft::WRL::ComPtr<ID3D11PixelShader>           e_pixel_shader_MQuad;

        Microsoft::WRL::ComPtr<ID3D11InputLayout>           e_input_layout_INodes;
        Microsoft::WRL::ComPtr<ID3D11VertexShader>          e_vertex_shader_INodes;
        Microsoft::WRL::ComPtr<ID3D11PixelShader>           e_pixel_shader_INodes;

        Microsoft::WRL::ComPtr<ID3D11Buffer>		        e_conbuf_MQuad_ViewProjectionTransform_buffer;
        VHG_conbuf_ViewProjectionTransform_struct           e_conbuf_MQuad_ViewProjectionTransform_data;
        Microsoft::WRL::ComPtr<ID3D11Buffer>		        e_conbuf_MQuad_ModelTransform_buffer;
        VHG_conbuf_ModelTransform_struct                    e_conbuf_MQuad_ModelTransform_data; // MiniSphere;
        Microsoft::WRL::ComPtr<ID3D11Buffer>		        e_conbuf_S2sphere_ModelTransform_buffer;
        VHG_conbuf_ModelTransform_struct                    e_conbuf_S2sphere_ModelTransform_data; // MiniSphere;



        Microsoft::WRL::ComPtr<ID3D11Buffer>                e_INodes_MVP_buffer;
        VHG_conbuf_MVP_struct                               e_INodes_MVP_data;


        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    e_texture_srv_S2sphere_sphere;  // MiniSphere;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    e_texture_srv_S2sphere_instance;  // MiniSphere;
        bool                                                e_option_S2sphere_rotating;  //  MiniSphere;
        std::unique_ptr<DirectX::GeometricPrimitive>        e_primitive_sphere;  //  MiniSphere;

        Microsoft::WRL::ComPtr<ID3D11SamplerState>          e_texture_sampler_state_loft;
        Microsoft::WRL::ComPtr<ID3D11RasterizerState>       e_rasterizer_state_wireframe;
        Microsoft::WRL::ComPtr<ID3D11RasterizerState>       e_rasterizer_state_solid;
        Microsoft::WRL::ComPtr<ID3D11RasterizerState>       e_rasterizer_state_loft;
        D3D11_FILL_MODE                                     e_rasterizer_fill_mode_loft;




        bool                                            m_loadingComplete;
        float                                           m_degreesPerSecond;




        uint32_t                                        e_advert_modulus;
        uint32_t                                        e_advert_count;



        std::unique_ptr<DirectX::Keyboard>              e_keyboard;
        std::unique_ptr<DirectX::Mouse>                 e_mouse; 
        float                                           e_pitch;
        float                                           e_yaw; 
        DirectX::SimpleMath::Vector3                    e_camera_position;
};  //  Closes class VHG_Scene3D1; 


}  //  Closes namespace VHG; 



                                                                
