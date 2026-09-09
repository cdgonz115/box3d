//
//  gravity.cpp
//  box3d
//
//  Created by Christian Gonzalez on 9/8/26.
//


#include "sample.h"
#include "box3d/box3d.h"
#include "gfx/debug_adapter.h"
#include "box3d/math_functions.h"

class SingleObject : public Sample
{
    b3BodyId m_topBodyId;
    b3Pos m_base; // world position of the offset content, the frame the height readout uses
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
        
    }
};

static int sampleSingleObject = RegisterSample( "Gravity", "Single Object", SingleObject::Create );

