//                      
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                      
//                 ghv : Garrett Vance : 20180528 
//  
//                     file  gv_MiniSphere.cpp   
//      
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  

#include "pch.h"
#include "gv_SCENE_3D.h"
#include "..\Common\DirectXHelper.h"
#include "..\gv_ADJUSTABLE.h"
#include "gv_Hopfbrau.h"



using namespace VHG;
using namespace Windows::Foundation;
using namespace DirectX;
using namespace DirectX::SimpleMath;




void VHG_Scene3D1::S2sphere_INodes_Animator()
{
    static uint16_t     delay_idx = 0;
    uint16_t const      delay_speed = 16;   //  Use 12 or e.g. 22; 

    (delay_idx + 1) > UINT16_MAX ? delay_idx = 0 : delay_idx++;

    if (delay_idx % delay_speed == 0)
    {
        uint32_t u_instance_idx = (1 + e_INodes_MVP_data.animator_count.x) % e_HopfSystem->e_INodes_instance_card;

        e_INodes_MVP_data.animator_count = XMUINT4(u_instance_idx, 0, 0, 0);
    }
}
//  Closes VHG_Scene3D1::S2sphere_INodes_Animator();






void VHG_HopfSystem::mesh_S2sphere_INodes(void)
{
    std::vector<VHG_Vertex_PosTex> inode_per_cube_vertices =
    {
        //     the +y face: 
        // 
        { XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT2(1.f, 0.00f) },
        { XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT2(0.f, 0.00f) },
        { XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT2(0.f, 1.f) },
        { XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT2(1.f, 1.f) },

        //  
        //     the -y face:  
        //  
        { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.f, 0.00f) },
        { XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.00f) },
        { XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT2(1.0f,  1.f) },
        { XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT2(0.f, 1.f) },

        // 
        //    the  -x face: 
        //   
        { XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT2(0.00f, 1.f) },
        { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(1.f,   1.f) },
        { XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT2(1.f,   0.f) },
        { XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT2(0.00f, 0.f) },

        // 
        //    the +x face: 
        //   
        { XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT2(1.f,    1.f) },
        { XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT2(0.00f,  1.f) },
        { XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT2(0.00f,  0.00f) },
        { XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT2(1.f,    0.00f) },

        //  
        //   the front face is the -z face: 
        //  
        { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.f,     1.f) },   //  16
        { XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT2(1.00f,   1.f) },   //  17 
        { XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT2(1.00f,   0.f) },   //  18 
        { XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT2(0.f,     0.f) },   //  19  

        //  
        //      the back face is the +z face: 
        //  
        { XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT2(1.f, 1.f) },
        { XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT2(0.f, 1.f) },
        { XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT2(0.f, 0.f) },
        { XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT2(1.f, 0.f) }
    };

    VHG_Scale   gv_scaleObj(0.05f);   //  Either  0.05 or 0.15f; 
    gv_scaleObj.posApply(inode_per_cube_vertices);


    D3D11_SUBRESOURCE_DATA Instance_Nodes_vbsubdata = { 0 };
    ZeroMemory(&Instance_Nodes_vbsubdata, sizeof(Instance_Nodes_vbsubdata));
    Instance_Nodes_vbsubdata.pSysMem = &(inode_per_cube_vertices[0]);
    Instance_Nodes_vbsubdata.SysMemPitch = 0;
    Instance_Nodes_vbsubdata.SysMemSlicePitch = 0;


    size_t nodes_required_allocation = sizeof(VHG_Vertex_PosTex) * (uint32)inode_per_cube_vertices.size();
    CD3D11_BUFFER_DESC Instance_Nodes_vb_desc( (UINT)nodes_required_allocation, D3D11_BIND_VERTEX_BUFFER );

    DX::ThrowIfFailed( 
        e_deviceResources->GetD3DDevice()->CreateBuffer( 
            &Instance_Nodes_vb_desc,
            &Instance_Nodes_vbsubdata,
            &e_INodes_vertex_buffer
        )
    );


    //              setup the instance buffer  
    //              =========================
    // 

    std::vector<VHG_Instance>* tmpInstances = new std::vector<VHG_Instance>;

	this->AddTrajectories(tmpInstances); 

    e_INodes_instance_card = (uint32_t)tmpInstances->size();

    D3D11_BUFFER_DESC ii_buffer_description;
    ZeroMemory(&ii_buffer_description, sizeof(ii_buffer_description));
    ii_buffer_description.ByteWidth = (UINT)tmpInstances->size() * sizeof(VHG_Instance); 
    ii_buffer_description.Usage = D3D11_USAGE_DYNAMIC;
    ii_buffer_description.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    ii_buffer_description.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    ii_buffer_description.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA INodes_instance_buffer_subdata = { 0 };
    ZeroMemory(&INodes_instance_buffer_subdata, sizeof(INodes_instance_buffer_subdata));
    INodes_instance_buffer_subdata.pSysMem = &(*tmpInstances)[0];
    INodes_instance_buffer_subdata.SysMemPitch = 0;
    INodes_instance_buffer_subdata.SysMemSlicePitch = 0;

    DX::ThrowIfFailed(
        e_deviceResources->GetD3DDevice()->CreateBuffer( // ensure_this_buffer_is_DYNAMIC
            &ii_buffer_description, 
            &INodes_instance_buffer_subdata, 
            &e_INodes_instance_buffer
        )
    );

    //   
    //     Don't try this without the Index Buffer
    // 

    static const unsigned short ib_indices_arrary[] =
    {
        3,1,0,  		2,1,3,
        6,4,5,  		7,4,6,
        11,9,8,  		10,9,11,
        14,12,13,  		15,12,14,
        19,17,16,  		18,17,19,
        22,20,21,  		23,20,22
    };

        
    e_INodes_index_buffer_card = ARRAYSIZE(ib_indices_arrary);


    D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
    ZeroMemory(&indexBufferData, sizeof(indexBufferData));
    indexBufferData.pSysMem = ib_indices_arrary;
    indexBufferData.SysMemPitch = 0;
    indexBufferData.SysMemSlicePitch = 0;

    CD3D11_BUFFER_DESC indexBufferDesc(sizeof(ib_indices_arrary), D3D11_BIND_INDEX_BUFFER);

    DX::ThrowIfFailed(
        e_deviceResources->GetD3DDevice()->CreateBuffer(
            &indexBufferDesc, 
			&indexBufferData, 
			&e_INodes_index_buffer
		)
	);
}
//  Closes VHG_HopfSystem::mesh_S2sphere_INodes();  











void VHG_Scene3D1::mesh_MQuad(void)
{
    float scm = 1.2f;

    std::vector<VHG_MQuad_Vertex>  model_vertices_MQuad =
    {
        VHG_MQuad_Vertex(XMFLOAT3(-1.f * scm, -1.f * scm, +1.f), XMFLOAT3(0.75f, 0.75f, 0.75f)),  //  0; grey;
        VHG_MQuad_Vertex(XMFLOAT3(+1.f * scm, -1.f * scm, +1.f), XMFLOAT3(0.75f, 0.75f, 0.75f)),  //  1; grey;
        VHG_MQuad_Vertex(XMFLOAT3(-1.f * scm, +1.f * scm, +1.f), XMFLOAT3(0.75f, 0.75f, 0.75f)),  //  2; grey;

        VHG_MQuad_Vertex(XMFLOAT3(+1.f * scm, +1.f * scm, +1.f), XMFLOAT3(0.75f, 0.75f, 0.75f)),  //  3; grey;
        VHG_MQuad_Vertex(XMFLOAT3(-1.f * scm, +1.f * scm, +1.f), XMFLOAT3(0.75f, 0.75f, 0.75f)),  //  4; grey;
        VHG_MQuad_Vertex(XMFLOAT3(+1.f * scm, -1.f * scm, +1.f), XMFLOAT3(0.75f, 0.75f, 0.75f)),  //  5; grey;
    };

    D3D11_BUFFER_DESC MQuad_vbdesc;
    ZeroMemory(&MQuad_vbdesc, sizeof(MQuad_vbdesc));
    MQuad_vbdesc.ByteWidth = sizeof(VHG_MQuad_Vertex) * (UINT)model_vertices_MQuad.size();
    MQuad_vbdesc.Usage = D3D11_USAGE_DEFAULT;
    MQuad_vbdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    MQuad_vbdesc.CPUAccessFlags = 0;
    MQuad_vbdesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA MQuad_vbsubdata = { 0 };
    MQuad_vbsubdata.pSysMem = &(model_vertices_MQuad[0]);
    MQuad_vbsubdata.SysMemPitch = 0;
    MQuad_vbsubdata.SysMemSlicePitch = 0;

    DX::ThrowIfFailed(
        m_deviceResources->GetD3DDevice()->CreateBuffer(
            &MQuad_vbdesc,
            &MQuad_vbsubdata,
            &e_vertex_buffer_MQuad
        )
    );
		
    static const unsigned short model_ib_indices_MQuad [] = 
    {
        // Each trio of indices represents a triangle with FRONT FACE CLOCKWISE. 

        1, 0, 2,    //  clockwise triangle; for camera_position < 0, e.g. -3.f;
        4, 3, 5,    //  clockwise triangle; for camera_position < 0, e.g. -3.f;
	};
    e_ib_indices_card_MQuad = ARRAYSIZE(model_ib_indices_MQuad);

	D3D11_SUBRESOURCE_DATA indexBufferSubData_MQuad = {0};
	indexBufferSubData_MQuad.pSysMem = model_ib_indices_MQuad;
	indexBufferSubData_MQuad.SysMemPitch = 0;
	indexBufferSubData_MQuad.SysMemSlicePitch = 0;

	CD3D11_BUFFER_DESC indexBufferDesc_MQuad(sizeof(model_ib_indices_MQuad), D3D11_BIND_INDEX_BUFFER);

	DX::ThrowIfFailed( 
        m_deviceResources->GetD3DDevice()->CreateBuffer( 
            &indexBufferDesc_MQuad, 
            &indexBufferSubData_MQuad, 
            &e_index_buffer_MQuad 
        ) 
    );
}
//  Closes VHG_Scene3D1::mesh_MQuad(); 


//                 ...file ends... 

