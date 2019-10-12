//                      
//             
//           ghv : Garrett Vance : 2018_01_14
//  
//       Implementation of Parallel Transport Frames
//      based on "Parallel Transport Approach to Curve Framing",  
//      1995 by Andrew J. Hanson. 
//
//      DirectX 11 App (Universal Windows), Windows 10
// 
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  



#include "pch.h"
#include "gv_SCENE_3D.h"
#include "..\Common\DirectXHelper.h"



using namespace VHG;
using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace Windows::Foundation;






void VHG_Fiber::HansonParallelTransportFrame(UINT p_torus_guid, UINT p_flatOffset)
{
    UINT const ArcDen = this->e_trajectory->GetAxonArcDensity();

    //  Special case when accum_nodes == 0: 
    //  ========================================
    //  Construct what Andrew J. Hanson terms the "initial normal vector V0": 

    XMVECTOR axon_r_position_zero = e_axonodromal_vertices->at(0).axon_position_V;
    XMVECTOR axon_r_position_one = e_axonodromal_vertices->at(1).axon_position_V;
    XMVECTOR axon_delta_r = axon_r_position_one - axon_r_position_zero;

    XMVECTOR g_zero = XMVector3Cross(axon_r_position_zero, axon_delta_r);  

    //  assert that g_zero is  perpendicular to the tangent at axon_r_position_zero;

    XMVECTOR h_normalized_normal = XMVector3Normalize( XMVector3Cross(g_zero, axon_delta_r) );

    e_axonodromal_vertices->at(0).curve_unit_normal = h_normalized_normal; // aka transported_unit_normal[0]; 

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    for (UINT idx_frame = 0; idx_frame < -1 + ArcDen; idx_frame++)
    {
        XMVECTOR B_vector = XMVector3Cross(
            e_axonodromal_vertices->at(idx_frame).curve_unit_tangent, 
            e_axonodromal_vertices->at(1 + idx_frame).curve_unit_tangent 
        );

        float B_length = XMVectorGetX(XMVector3Length(B_vector));
        const float epsilon_length = 0.0001f;
        XMVECTOR next_transported_normal;

        if (B_length < epsilon_length)
        {
            next_transported_normal = e_axonodromal_vertices->at(idx_frame).curve_unit_normal;
        }
        else
        {
            XMVECTOR B_unit_vector = XMVector3Normalize(B_vector);

            XMVECTOR angle_theta_vector = XMVector3AngleBetweenNormals(
                e_axonodromal_vertices->at(idx_frame).curve_unit_tangent, 
                e_axonodromal_vertices->at(1 + idx_frame).curve_unit_tangent 
            );
            
            float angle_theta = XMVectorGetX(angle_theta_vector);

            XMMATRIX gv_rotation_matrix = XMMatrixRotationNormal(
                    B_unit_vector, 
                    angle_theta
            ); 

            next_transported_normal = XMVector3Transform(
                e_axonodromal_vertices->at(idx_frame).curve_unit_normal, 
                gv_rotation_matrix
            );
        }

        XMVECTOR transported_unit_normal = XMVector3Normalize(next_transported_normal); 

        e_axonodromal_vertices->at(idx_frame + 1).curve_unit_normal = transported_unit_normal; 
    }

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //  
    //  This concludes Hanson's Parallel Transport Frames algorithm;
    //  
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


    for (UINT idx_section = 0; idx_section < ArcDen; idx_section++)
    {
        // 
        //     In order to orient and position the cross-section, 
        //     need another normal vector in addition to the transported_normal.
        //     I have chosen to use vector B = T cross N.
        //      
        //  I expect that binormal doesn't need explicit normalization;
        //            

        XMVECTOR binormal = XMVector3Cross(
            e_axonodromal_vertices->at(idx_section).curve_unit_tangent,  
            e_axonodromal_vertices->at(idx_section).curve_unit_normal   
        );

        float loft_cross_section_edge = e_trajectory->GetHopfSystem()->GetLoftCrossSectionEdge();

        for (uint32_t k = 0; k < VHG_Scene3D1::tubeFacets; k++)
        {
            //     Poloidal loop: 

            float angle_phi = k * 2 * XM_PI / (float)VHG_Scene3D1::tubeFacets;

            float C_x = loft_cross_section_edge * cosf(angle_phi);
            float C_y = loft_cross_section_edge * sinf(angle_phi);

            XMVECTOR vectorP = 
                e_axonodromal_vertices->at(idx_section).axon_position_V + 
                C_x * e_axonodromal_vertices->at(idx_section).curve_unit_normal + 
                C_y * binormal; 

            VHG_Loft_Vertex tmp_surface_point; 
            if (idx_section + 1 == ArcDen)
            {
                tmp_surface_point.e_loft_pos = e_surface_points->at(k).e_loft_pos;
            }
            else
            {
                XMFLOAT3 tmp_f3; 
                XMStoreFloat3(&tmp_f3, vectorP); 
                tmp_surface_point.e_loft_pos = tmp_f3;
            }

            tmp_surface_point.e_guids = XMFLOAT3((float)e_fiber_guid, (float)p_torus_guid, (float)idx_section);
            tmp_surface_point.e_texco = XMFLOAT2(0.f, 0.f);                                                   
            e_surface_points->at(k + idx_section * VHG_Scene3D1::tubeFacets) = tmp_surface_point;
        }
        // for each cross-section facet;
    }

    //  
    //  Assert cardinality of std::vector e_surface_points = e_axon_arc_density * p_tube_facets.
    //  


    //          
    //          Texture Coordinates 
    //==================================================================
    //          texco_u  is poloidal;
    //          texco_v is axial (aka toroidal);
    //==================================================================
    //          



    VHG_Loft_Vertex quad_top_left;
    VHG_Loft_Vertex quad_top_right;
    VHG_Loft_Vertex quad_bottom_right;
    VHG_Loft_Vertex quad_bottom_left;

    uint32_t Fac = VHG_Scene3D1::tubeFacets; 

    for (uint32_t i_axial = 0; i_axial < ArcDen; i_axial++)
    {
        DirectX::XMFLOAT3 the_guids = DirectX::XMFLOAT3((float)e_fiber_guid, (float)p_torus_guid, (float)i_axial); 
        for (uint32_t i_poloidal = 0; i_poloidal < VHG_Scene3D1::tubeFacets; i_poloidal++)
        {
            uint32_t eltLL = (i_axial * Fac) + i_poloidal;
            uint32_t eltUL = (i_axial * Fac) + ((i_poloidal + 1) % Fac);
               
            uint32_t eltLR = (((i_axial + 1) % ArcDen) * Fac) + i_poloidal;  
            uint32_t eltUR = (((i_axial + 1) % ArcDen) * Fac) + ((i_poloidal + 1) % Fac);
            
            if (i_axial + 1 == ArcDen)
            {
                eltLR =  i_poloidal; 
            
                eltUR =  (i_poloidal + 1) % Fac;
            }

            quad_top_left.e_loft_pos = (XMFLOAT3)e_surface_points->at(eltUL).e_loft_pos; 
            quad_top_left.e_guids = (XMFLOAT3)the_guids;
            quad_top_left.e_texco = XMFLOAT2(0.f, 1.f); 

            quad_top_right.e_loft_pos = (XMFLOAT3)e_surface_points->at(eltUR).e_loft_pos; 
            quad_top_right.e_guids = (XMFLOAT3)the_guids;
            quad_top_right.e_texco = XMFLOAT2(0.f, 0.f); 
            
            quad_bottom_right.e_loft_pos = (XMFLOAT3)e_surface_points->at(eltLR).e_loft_pos; 
            quad_bottom_right.e_guids = (XMFLOAT3)the_guids;
            quad_bottom_right.e_texco =  XMFLOAT2(1.f, 0.f);

            quad_bottom_left.e_loft_pos = (XMFLOAT3)e_surface_points->at(eltLL).e_loft_pos;
            quad_bottom_left.e_guids = (XMFLOAT3)the_guids;
            quad_bottom_left.e_texco =  XMFLOAT2(1.f, 1.f) ;


            uint32_t flat_idx = p_flatOffset + 
                VHG_Scene3D1::e_verts_per_quad * 
                (i_axial * VHG_Scene3D1::tubeFacets + i_poloidal); 


            //      Synthesize the 1st of the 2 triangles:
            e_trajectory->GetHopfSystem()->e_master_lofts->at(flat_idx + 0) = quad_bottom_left;
            e_trajectory->GetHopfSystem()->e_master_lofts->at(flat_idx + 1) = quad_bottom_right;
            e_trajectory->GetHopfSystem()->e_master_lofts->at(flat_idx + 2) = quad_top_left;

#ifdef GHV_OPTION_INDEX_BUFFER
            e_trajectory->GetHopfSystem()->e_master_indices->at(flat_idx + 0) = flat_idx + 0;  //  vertex a in triangle 1;
            e_trajectory->GetHopfSystem()->e_master_indices->at(flat_idx + 1) = flat_idx + 1;  //  vertex b in triangle 1;
            e_trajectory->GetHopfSystem()->e_master_indices->at(flat_idx + 2) = flat_idx + 2;  //  vertex c in triangle 1;
#endif

            //      Synthesize the 2nd triangle:
            e_trajectory->GetHopfSystem()->e_master_lofts->at(flat_idx + 3) = quad_top_right;
            e_trajectory->GetHopfSystem()->e_master_lofts->at(flat_idx + 4) = quad_top_left;
            e_trajectory->GetHopfSystem()->e_master_lofts->at(flat_idx + 5) = quad_bottom_right;

#ifdef GHV_OPTION_INDEX_BUFFER
            e_trajectory->GetHopfSystem()->e_master_indices->at(flat_idx + 3) = flat_idx + 3;  //  vertex b in triangle 2;
            e_trajectory->GetHopfSystem()->e_master_indices->at(flat_idx + 4) = flat_idx + 4;  //  vertex d in triangle 2;
            e_trajectory->GetHopfSystem()->e_master_indices->at(flat_idx + 5) = flat_idx + 5;  //  vertex c in triangle 2;
#endif

        }
        //  Closes "for" loop over i_poloidal;
    }
    //  Closes "for" loop over i_axial;
}  
//  Closes VHG_Fiber::HansonParallelTransportFrame(); 



