//
//  gravity.cpp
//  box3d
//
//  Created by Christian Gonzalez on 9/8/26.
//


#include "box3d/box3d.h"
#include "box3d/math_functions.h"
#include "gfx/debug_adapter.h"
#include "gfx/keycodes.h"
#include "gfx/draw.h"
#include "sample.h"
#include <vector>

struct SampleSceneGravityMarker
{
    b3Vec3 position;
};

class SingleObject : public Sample
{
    b3BodyId m_topBodyId;
    b3Pos m_base; // world position of the offset content, the frame the height readout uses
    
    b3BodyId m_sphereBodyId;
    b3BodyType m_type;
    bool m_isEnabled;
    std::vector<SampleSceneGravityMarker> m_markers;
    static constexpr float PICK_RADIUS = 2;
public:
    explicit SingleObject( SampleContext* context )
        : Sample( context )
    {
        BuildScene();
    }
    
    static Sample* Create( SampleContext* context )
    {
        return new SingleObject( context );
    }
    void BuildScene()
    {
        m_type = b3_dynamicBody;
        m_isEnabled = true;
        
        b3Pos base = { 0.0f, 0.0f, 0.0f };
        m_base = base;
        m_camera->m_pivot = b3OffsetPos( base, { 0.0f, 2.0f, 0.0f } );
        m_camera->UpdateTransform();

        b3Transform transforms[] = {{{0.0f,-11.0f,0.0f}, b3Quat_identity}, {{0.0f,11.0f,0.0f}, b3MakeQuatFromAxisAngle( b3Vec3_axisZ, B3_DEG_TO_RAD *  180)}, {{11.0f,0.0f,0.0f}, b3MakeQuatFromAxisAngle( b3Vec3_axisZ, B3_DEG_TO_RAD *  90)}, {{-11.0f,0.0f,0.0f}, b3MakeQuatFromAxisAngle( b3Vec3_axisZ, -B3_DEG_TO_RAD *  90)},
            {{0.0f,0.0f,-11.0f}, b3MakeQuatFromAxisAngle( b3Vec3_axisX, -B3_DEG_TO_RAD *  90)}
        };
        
        for (b3Transform transform : transforms) {
            b3BodyDef bodyDef = b3DefaultBodyDef();
            bodyDef.name = "ground";
            bodyDef.position = b3OffsetPos( base, transform.p );
            bodyDef.rotation = transform.q;
            b3BodyId groundId = b3CreateBody( m_worldId, &bodyDef );
            
            b3ShapeDef shapeDef = b3DefaultShapeDef();
            b3BoxHull groundHull = b3MakeBoxHull( 12.0f, 1.0f, 12.0f );
            b3ShapeId groundShapeId = b3CreateHullShape( groundId, &shapeDef, &groundHull.base );
            SetGroundShape( groundShapeId );
        }
        
        {
            b3BodyDef bodyDef = b3DefaultBodyDef();
            bodyDef.type = m_type;
            bodyDef.isEnabled = m_isEnabled;
            bodyDef.position = { 0.0f, 5.0f };
            bodyDef.name = "floater";
            m_sphereBodyId = b3CreateBody( m_worldId, &bodyDef );

            b3Sphere sphere = { { 0.0f, 0.5f, 0.0f }, 1 };

            b3ShapeDef shapeDef = b3DefaultShapeDef();
            shapeDef.density = 2.0f;
            
            b3CreateSphereShape( m_sphereBodyId, &shapeDef, &sphere );
        }
        
    }
    
    void MouseDown( b3Vec2 p, int button, int modifiers ) override
    {
        if ( button == 0 && modifiers == MOD_SHIFT)
        {
            PickRay pickRay = m_camera->BuildPickRay( p.x, p.y );
            b3RayResult result = b3World_CastRayClosest(m_worldId, pickRay.origin, pickRay.translation, b3DefaultQueryFilter());
            for(int i = 0; i < m_markers.size(); i++)
            {
                if(b3Distance(m_markers[i].position, result.point) < PICK_RADIUS)
                {
                    b3Body_RemoveGravitySourceAt(m_sphereBodyId, i);
                    m_markers.erase(m_markers.begin()+i);
                    return;
                }
            }
            
            b3GravitySource source = {result.point,{0},10,true};
            
            if(m_markers.size() >= B3_MAX_GRAVITY_SOURCES){
                m_markers.erase(m_markers.begin());
            }
            b3Body_AddGravitySource(m_sphereBodyId,source);
            m_markers.push_back({result.point});
        }
        else
        {
            Sample::MouseDown(p,button,modifiers);
        }
    }
    void Step() override
    {
        for(const SampleSceneGravityMarker& marker: m_markers)
        {
            b3Sphere sphere = { marker.position, 0.2f};
            DrawSolidSphere(b3Transform_identity, sphere, MakeColor(b3_colorGreen));
        }

        Sample::Step();
    }
};

static int sampleSingleObject = RegisterSample( "Gravity", "Single Object", SingleObject::Create );

