//                      
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                      
//                 ghv : Garrett Vance : 20180520 
//  
//                     file  gv_D3D11_Resources.cpp   
//      
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  

#include "pch.h"
#include "gv_SCENE_3D.h"
#include "..\Common\DirectXHelper.h"
#include "..\gv_ADJUSTABLE.h"

#include <DDSTextureLoader.h>


using namespace VHG;
using namespace Windows::Foundation;
using namespace DirectX;






void VHG_Scene3D1::loft_vertex_buffer_create(void)
{

#ifdef GHV_OPTION_INDEX_BUFFER
    //      Create Index Buffer for Loft   (recall that sizeof(DWORD) = 4 bytes );

    UINT debug_card_loft_ib_indices = (UINT)e_HopfSystem->e_master_indices->size();

    D3D11_BUFFER_DESC           index_buffer_description;
    ZeroMemory(&index_buffer_description, sizeof(index_buffer_description));
    index_buffer_description.ByteWidth = debug_card_loft_ib_indices * sizeof(DWORD); 
    index_buffer_description.Usage = D3D11_USAGE_DYNAMIC;
    index_buffer_description.BindFlags = D3D11_BIND_INDEX_BUFFER;
    index_buffer_description.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    index_buffer_description.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA  index_subres_data;
    ZeroMemory(&index_subres_data, sizeof(index_subres_data));
    index_subres_data.SysMemPitch = 0;
    index_subres_data.SysMemSlicePitch = 0;
    index_subres_data.pSysMem = &(e_HopfSystem->e_master_indices->at(0)); 

    DX::ThrowIfFailed( 
        m_deviceResources->GetD3DDevice()->CreateBuffer( 
            &index_buffer_description, 
            &index_subres_data, 
            e_index_buffer_loft.ReleaseAndGetAddressOf() 
        )
    );
#endif


    //      Create Vertex Buffer for Lofting Tube (aka extrusion): 

    UINT debug_card_loft_vertices = (UINT)e_HopfSystem->e_master_lofts->size();

    D3D11_BUFFER_DESC vertex_buffer_description;
    ZeroMemory(&vertex_buffer_description, sizeof(vertex_buffer_description));
    vertex_buffer_description.ByteWidth = debug_card_loft_vertices * sizeof(VHG_Loft_Vertex); 
    vertex_buffer_description.Usage = D3D11_USAGE_DYNAMIC;
    vertex_buffer_description.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertex_buffer_description.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vertex_buffer_description.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA vertex_subresource_data;
    ZeroMemory(&vertex_subresource_data, sizeof(vertex_subresource_data));
    vertex_subresource_data.SysMemPitch = 0;
    vertex_subresource_data.SysMemSlicePitch = 0;
    vertex_subresource_data.pSysMem = &(e_HopfSystem->e_master_lofts->at(0));
        
    DX::ThrowIfFailed(
        m_deviceResources->GetD3DDevice()->CreateBuffer(
            &vertex_buffer_description,
            &vertex_subresource_data,
            e_vertex_buffer_loft.ReleaseAndGetAddressOf()
        )
    );
}
//  Closes VHG_Scene3D1::loft_vertex_buffer_create();






void VHG_Scene3D1::Create_Loft_Input_Layout(const std::vector<byte>& p_byte_vector)
{
    static const D3D11_INPUT_ELEMENT_DESC vertexDescLoft[] =
    {
        //    
        // was { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  ... wasn't using Input Slots > 0.
        // was { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  ... wasn't using Input Slots > 0.
        //    

        //  
        //  Formerly, this Input Layout was set to draw from TWO distinct vertex buffers, one dedicated to Normal values, 
        //  the other vertex buffer supplying Position, Guids and Texture Coordinates. 
        //      
        //  But now I'm going to deploy only a single vertex buffer, 
        //  and for that reason the value of "1" for Input Slot will be reverted to zero. 
        //      

        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },  //  COLOR0 for e_fiber_guid;
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,          0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    DX::ThrowIfFailed(
        m_deviceResources->GetD3DDevice()->CreateInputLayout(
            vertexDescLoft,
            ARRAYSIZE(vertexDescLoft),
            &p_byte_vector[0], 
            p_byte_vector.size(), 
            e_input_layout_loft.GetAddressOf()
        )
    );
}  
// Closes VHG_Scene3D1::Create_Loft_Input_Layout(); 










void VHG_Scene3D1::Load_Texture_Triax(void)
{
    Microsoft::WRL::ComPtr<ID3D11Resource>   temp_resource;

    //          
    //      Texture SRVs for the Loft: 
    //          

    DX::ThrowIfFailed(
        CreateDDSTextureFromFile(
            m_deviceResources->GetD3DDevice(),
            L"Assets\\Loft_DDS_Pure_Fail.dds",
            temp_resource.ReleaseAndGetAddressOf(),
            e_texture_srv_Loft_Pure_Fail.GetAddressOf(),
            0,
            nullptr
        ) 
    );

    DX::ThrowIfFailed( 
        CreateWICTextureFromFile( 
            m_deviceResources->GetD3DDevice(), 
            L"Assets\\Loft_png_ferrero_flat.png", 
            temp_resource.ReleaseAndGetAddressOf(), 
            e_texture_srv_Loft_Ferrero_Rocher.GetAddressOf(), 
            0 
        ) 
    );


    //          
    //      Texture SRVs for S2sphere: 
    //          

    DX::ThrowIfFailed( 
        CreateWICTextureFromFile( 
            m_deviceResources->GetD3DDevice(), 
            L"Assets\\S2sphere_png_mandelbrot_extrusion.png", 
            temp_resource.ReleaseAndGetAddressOf(), 
            e_texture_srv_S2sphere_instance.GetAddressOf(), 
            0 
        ) 
    );

    DX::ThrowIfFailed( 
        CreateWICTextureFromFile( 
            m_deviceResources->GetD3DDevice(), 
            L"Assets\\S2sphere_bmp_mandelbrot_hue_252.bmp", 
            temp_resource.ReleaseAndGetAddressOf(), 
            e_texture_srv_S2sphere_sphere.GetAddressOf(), 
            0 
        ) 
    );
}
//  Closes VHG_Scene3D1::Load_Texture_Triax(); 






void VHG_Scene3D1::Create_Rasterizer_State_Loft(void)
{
    D3D11_RASTERIZER_DESC raster_desc;
    ZeroMemory(&raster_desc, sizeof(raster_desc));

    raster_desc.FillMode = this->e_rasterizer_fill_mode_loft;



    raster_desc.CullMode = D3D11_CULL_NONE;
    raster_desc.FrontCounterClockwise = FALSE;


    raster_desc.CullMode = D3D11_CULL_BACK;



    raster_desc.DepthBias = 0;
    raster_desc.SlopeScaledDepthBias = 0.0f;
    raster_desc.DepthBiasClamp = 0.0f;
    raster_desc.DepthClipEnable = TRUE;
    raster_desc.ScissorEnable = FALSE;
    raster_desc.MultisampleEnable = FALSE;
    raster_desc.AntialiasedLineEnable = FALSE;

    DX::ThrowIfFailed(
        this->m_deviceResources->GetD3DDevice()->CreateRasterizerState(
            &raster_desc,
            e_rasterizer_state_loft.ReleaseAndGetAddressOf()
        )
    );
}
//  Closes VHG_Scene3D1::Create_Rasterizer_State_Loft(); 






void VHG_Scene3D1::Create_Rasterizer_State_Wireframe(void)
{
    D3D11_RASTERIZER_DESC wireframe_descr;
    ZeroMemory(&wireframe_descr, sizeof(wireframe_descr));

    wireframe_descr.FillMode = D3D11_FILL_WIREFRAME;
    wireframe_descr.CullMode = D3D11_CULL_NONE;
    wireframe_descr.FrontCounterClockwise = FALSE;
    wireframe_descr.DepthBias = 0;
    wireframe_descr.SlopeScaledDepthBias = 0.0f;
    wireframe_descr.DepthBiasClamp = 0.0f;
    wireframe_descr.DepthClipEnable = TRUE;
    wireframe_descr.ScissorEnable = FALSE;
    wireframe_descr.MultisampleEnable = FALSE;
    wireframe_descr.AntialiasedLineEnable = FALSE;

    DX::ThrowIfFailed(
        this->m_deviceResources->GetD3DDevice()->CreateRasterizerState(
            &wireframe_descr,
            e_rasterizer_state_wireframe.ReleaseAndGetAddressOf()
        )
    );
}
//  Closes VHG_Scene3D1::Create_Rasterizer_State_Wireframe(); 





void VHG_Scene3D1::Create_Rasterizer_State_Solid(void)
{
    D3D11_RASTERIZER_DESC wireframe_descr;
    ZeroMemory(&wireframe_descr, sizeof(wireframe_descr));

    wireframe_descr.FillMode = D3D11_FILL_SOLID;
    wireframe_descr.CullMode = D3D11_CULL_NONE;
    wireframe_descr.FrontCounterClockwise = FALSE;
    wireframe_descr.DepthBias = 0;
    wireframe_descr.SlopeScaledDepthBias = 0.0f;
    wireframe_descr.DepthBiasClamp = 0.0f;
    wireframe_descr.DepthClipEnable = TRUE;
    wireframe_descr.ScissorEnable = FALSE;
    wireframe_descr.MultisampleEnable = FALSE;
    wireframe_descr.AntialiasedLineEnable = FALSE;

    DX::ThrowIfFailed(
        this->m_deviceResources->GetD3DDevice()->CreateRasterizerState(
            &wireframe_descr,
            e_rasterizer_state_solid.ReleaseAndGetAddressOf()
        )
    );
}
//  Closes VHG_Scene3D1::Create_Rasterizer_State_Solid(); 












void VHG_Scene3D1::Create_Sampler_State_Loft(void)
{
    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

    DX::ThrowIfFailed(
        m_deviceResources->GetD3DDevice()->CreateSamplerState(
            &sampDesc,
            e_texture_sampler_state_loft.ReleaseAndGetAddressOf()
        )
    );
}
//  Closes VHG_Scene3D1::Create_Sampler_State_Loft();



//                 ...file ends... 

