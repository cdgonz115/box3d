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
#include <iostream>

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
        if ( context->restart == false )
        {
            m_camera->SetView( 0.0f, 0.0f, 35.0f, { 0.0f, 0.0f, 0.0f });
        }
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

        b3Transform wallTransforms[] = {{{0.0f,-11.0f,0.0f}, b3Quat_identity}, {{0.0f,11.0f,0.0f}, b3MakeQuatFromAxisAngle( b3Vec3_axisZ, B3_DEG_TO_RAD *  180)}, {{11.0f,0.0f,0.0f}, b3MakeQuatFromAxisAngle( b3Vec3_axisZ, B3_DEG_TO_RAD *  90)}, {{-11.0f,0.0f,0.0f}, b3MakeQuatFromAxisAngle( b3Vec3_axisZ, -B3_DEG_TO_RAD *  90)},
            {{0.0f,0.0f,-11.0f}, b3MakeQuatFromAxisAngle( b3Vec3_axisX, -B3_DEG_TO_RAD *  90)}
        };
        
        for (b3Transform transform : wallTransforms)
        {
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
            
            
            
            b3GravitySource source = {result.point,{0},{0},10,true,false};
            
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

class MultipleObjects : public Sample
{
public:
    explicit MultipleObjects( SampleContext* context )
        : Sample( context )
    {
        if ( context->restart == false )
        {
            m_camera->SetView( 0.0f, 0.0f, 35.0f, { 0.0f, 0.0f, 0.0f });
        }
        BuildScene();
    }
    
    static Sample* Create( SampleContext* context )
    {
        return new MultipleObjects( context );
    }
    void BuildScene()
    {
        b3Pos base = { 0.0f, 0.0f, 0.0f };

        b3Transform wallTransforms[] = {{{0.0f,-11.0f,0.0f}, b3Quat_identity}, {{0.0f,11.0f,0.0f}, b3MakeQuatFromAxisAngle( b3Vec3_axisZ, B3_DEG_TO_RAD *  180)}, {{11.0f,0.0f,0.0f}, b3MakeQuatFromAxisAngle( b3Vec3_axisZ, B3_DEG_TO_RAD *  90)}, {{-11.0f,0.0f,0.0f}, b3MakeQuatFromAxisAngle( b3Vec3_axisZ, -B3_DEG_TO_RAD *  90)},
            {{0.0f,0.0f,-11.0f}, b3MakeQuatFromAxisAngle( b3Vec3_axisX, -B3_DEG_TO_RAD *  90)}
        };
        
        for (b3Transform transform : wallTransforms)
        {
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
    
        b3GravitySource sourceGravity = {{0},{0},{0},10,true,false};
        
        int numberOfBalls = 64;
        float radius = 5;
        
        float step = 360 / numberOfBalls;
        
        b3Vec3 center = { 0.0f, 4.0f };
        
        for (int i = 0 ; i < numberOfBalls; i++)
        {
            b3BodyDef bodyDef = b3DefaultBodyDef();
            bodyDef.type = b3_dynamicBody;
            bodyDef.isEnabled = true;
            
            float angle =step * i;
            
            float x = cos(angle);
            float y = sin(angle);
            
            bodyDef.position = center + ((b3Vec3){x, y, 0}) * radius;
            bodyDef.name = "object";
            b3BodyId m_sphereBodyId = b3CreateBody( m_worldId, &bodyDef );
            b3Sphere sphere = { { 0.0f, 0.5f, 0.0f }, .5 };

            b3ShapeDef shapeDef = b3DefaultShapeDef();
            shapeDef.density = 2.0f;
            
            b3Body_AddGravitySource(m_sphereBodyId,sourceGravity);

            b3CreateSphereShape( m_sphereBodyId, &shapeDef, &sphere );
        }
    }
};

static int sampleMultipleObjects = RegisterSample( "Gravity", "Multiple Objects", MultipleObjects::Create );

class SourceBody : public Sample
{
public:
    explicit SourceBody( SampleContext* context )
        : Sample( context )
    {
        if ( context->restart == false )
        {
            m_camera->SetView( 0.0f, 0.0f, 35.0f, { 0.0f, 0.0f, 0.0f });
        }
        BuildScene();
    }
    
    static Sample* Create( SampleContext* context )
    {
        return new SourceBody( context );
    }
    void BuildScene()
    {
        b3Pos base = { 0.0f, 0.0f, 0.0f };
    
        b3Transform wallTransforms[] = {{{0.0f,-11.0f,0.0f}, b3Quat_identity}, {{0.0f,11.0f,0.0f}, b3MakeQuatFromAxisAngle( b3Vec3_axisZ, B3_DEG_TO_RAD *  180)}, {{11.0f,0.0f,0.0f}, b3MakeQuatFromAxisAngle( b3Vec3_axisZ, B3_DEG_TO_RAD *  90)}, {{-11.0f,0.0f,0.0f}, b3MakeQuatFromAxisAngle( b3Vec3_axisZ, -B3_DEG_TO_RAD *  90)},
            {{0.0f,0.0f,-11.0f}, b3MakeQuatFromAxisAngle( b3Vec3_axisX, -B3_DEG_TO_RAD *  90)}
        };
        
        for (b3Transform transform : wallTransforms)
        {
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
        b3BodyDef gravitySourceBodyDef = b3DefaultBodyDef();
        gravitySourceBodyDef.type = b3_dynamicBody;
        gravitySourceBodyDef.gravityScale = 0;
        gravitySourceBodyDef.isEnabled = true;
        gravitySourceBodyDef.position = { 0.0f, 5.0f };
        gravitySourceBodyDef.name = "floater";
        b3BodyId gravitySourceBodyId = b3CreateBody( m_worldId, &gravitySourceBodyDef );

        b3Sphere gravitySourceSphere = { { 0.0f, 0.5f, 0.0f }, 1 };

        b3ShapeDef gravitySourceShapeDef = b3DefaultShapeDef();
        gravitySourceShapeDef.density = 2.0f;
        
        b3ShapeId gravitySourceShapeId = b3CreateSphereShape( gravitySourceBodyId, &gravitySourceShapeDef, &gravitySourceSphere );
        
        b3SurfaceMaterial gravitySourceMaterial = b3DefaultSurfaceMaterial();
        gravitySourceMaterial.customColor = b3_colorCyan;
        
        b3Shape_SetSurfaceMaterial(gravitySourceShapeId, gravitySourceMaterial);
        
        b3GravitySource sourceGravity = {{0},{0},gravitySourceBodyId,10,true,true};
        
        int numberOfBalls = 12;
        float radius = 5;
        
        float step = 360 / numberOfBalls;
        
        b3Vec3 center = { 0.0f, 4.0f };
        
        for (int i = 0 ; i < numberOfBalls; i++)
        {
            b3BodyDef bodyDef = b3DefaultBodyDef();
            bodyDef.type = b3_dynamicBody;
            bodyDef.isEnabled = true;
            
            float angle =step * i;
            
            float x = cos(angle);
            float y = sin(angle);
            
            bodyDef.position = center + ((b3Vec3){x, y, 0}) * radius;
            bodyDef.name = "object";
            b3BodyId m_sphereBodyId = b3CreateBody( m_worldId, &bodyDef );
            b3Sphere sphere = { { 0.0f, 0.5f, 0.0f }, .5 };

            b3ShapeDef shapeDef = b3DefaultShapeDef();
            shapeDef.density = 2.0f;
            
            b3Body_AddGravitySource(m_sphereBodyId,sourceGravity);

            b3CreateSphereShape( m_sphereBodyId, &shapeDef, &sphere );
        }
    }
};

static int sampleSourceBody = RegisterSample( "Gravity", "Source Body", SourceBody::Create );


class AllSourceBodies : public Sample
{
    std::vector<b3GravitySource> m_gravitySources;
public:
    explicit AllSourceBodies( SampleContext* context )
        : Sample( context )
    {
        if ( context->restart == false )
        {
            m_camera->SetView( 0.0f, 90.0f, 35.0f, { 0.0f, 0.0f, 0.0f });
        }
        BuildScene();
    }
    
    static Sample* Create( SampleContext* context )
    {
        return new AllSourceBodies( context );
    }
    void BuildScene()
    {
        //Base large platform
        {
            b3BodyDef bodyDef = b3DefaultBodyDef();
            bodyDef.position = { 0.0f, -1.0f, 0.0f };
            b3BodyId groundId = b3CreateBody( m_worldId, &bodyDef );

            b3ShapeDef shapeDef = b3DefaultShapeDef();
            b3BoxHull groundHull = b3MakeBoxHull( 400.0f, 1.0f, 400.0f );
            b3ShapeId groundShapeId = b3CreateHullShape( groundId, &shapeDef, &groundHull.base );
            SetGroundShape( groundShapeId );
        }
    
        int numberOfBalls = 3200;
        int numberOfRings = 500;
        float radiusOfBalls = 0.5;
        
        int ballsPerRing = numberOfBalls/numberOfRings;
        int ballsOverflow = numberOfBalls%numberOfRings;
        
        float minRadius = radiusOfBalls / sin(B3_PI / ballsPerRing);
        float maxRadius = 5;
        
        b3Pos centerOfSpiral = {0,1,0};
        
        for(int i = 0; i < numberOfRings; i++)
        {
            int inRing = ballsPerRing + ((i == numberOfRings-1)? ballsOverflow : 0);
            
            double step = 2 * B3_PI / inRing;
            
            for(int j = 0 ; j < inRing ; j++)
            {
                b3BodyDef bodyDef = b3DefaultBodyDef();
                bodyDef.type = b3_dynamicBody;
                bodyDef.isEnabled = true;
                
                float angle = step * j + i;
                
                float x = cos(angle);
                float z = sin(angle);
                
                bodyDef.position = centerOfSpiral + ((b3Vec3){x,0,z}) * (i * radiusOfBalls * 2.01 + minRadius + 5);
                bodyDef.name = "object";
                b3BodyId m_sphereBodyId = b3CreateBody( m_worldId, &bodyDef );
                m_gravitySources.push_back({{0},{0},m_sphereBodyId,1,true,true});
                b3Sphere sphere = { { 0.0f, 0.5f, 0.0f }, radiusOfBalls};

                b3ShapeDef shapeDef = b3DefaultShapeDef();
                shapeDef.density = 2.0f;
                
                b3CreateSphereShape( m_sphereBodyId, &shapeDef, &sphere );
            }
        }
        
        for (int i = m_gravitySources.size() -1; i >=0; i--)
        {
            for(int j = m_gravitySources.size()-1 ; j >= 0 ; j--)
            {
                if(i!=j)
                {
                    b3Body_AddGravitySource(m_gravitySources[i].sourceBodyId, m_gravitySources[j]);
                }
            }
        }
    }
};

static int sampleAllSourceBodies = RegisterSample( "Gravity", "All source Bodies", AllSourceBodies::Create );


class StressTest : public Sample
{
public:
    explicit StressTest(SampleContext * context) : Sample(context)
    {
        if ( context->restart == false )
        {
            m_camera->SetView( 0.0f, 0.0f, 100, { 0.0f, 50, 0.0f });
        }
        BuildScene();
    }
    static Sample* Create( SampleContext* context )
    {
        return new StressTest( context );
    }
    void BuildScene()
    {
        //Base large platform
        {
            b3BodyDef bodyDef = b3DefaultBodyDef();
            bodyDef.position = { 0.0f, -1.0f, 0.0f };
            b3BodyId groundId = b3CreateBody( m_worldId, &bodyDef );

            b3ShapeDef shapeDef = b3DefaultShapeDef();
            b3BoxHull groundHull = b3MakeBoxHull( 400.0f, 1.0f, 400.0f );
            b3ShapeId groundShapeId = b3CreateHullShape( groundId, &shapeDef, &groundHull.base );
            SetGroundShape( groundShapeId );
        }
        //Spawn balls
        {
            int numberOfBalls = 4000;
            int numberOfRings = 400;
            float radiusOfBalls = 0.5;
            
            int ballsPerRing = numberOfBalls/numberOfRings;
            int ballsOverflow = numberOfBalls%numberOfRings;
            
            float minRadius = radiusOfBalls / sin(B3_PI / ballsPerRing);
            float maxRadius = 50;
            
            b3Pos centerOfGravity = {0,50,0};
            
            double radiusStep = B3_PI / numberOfRings;
            double heightStep = B3_PI / numberOfRings;
            
            for(int i = 0; i < numberOfRings; i++)
            {
                int inRing = ballsPerRing + ((i == numberOfRings-1)? ballsOverflow : 0);
                
                double step = 2 * B3_PI / inRing;
                
                b3GravitySource sourceGravity = {centerOfGravity,{0},{0},10,true,false};
                
                float radius = sin(radiusStep * i) * maxRadius;
                float height = cos(heightStep * i) * maxRadius;
                                
                for(int j = 0 ; j < inRing ; j++)
                {
                    b3BodyDef bodyDef = b3DefaultBodyDef();
                    bodyDef.type = b3_dynamicBody;
                    bodyDef.isEnabled = true;
                    
                    float angle = step * j + i;
                    
                    float x = cos(angle);
                    float z = sin(angle);
                    
                    bodyDef.position = centerOfGravity + ((b3Vec3){x,0,z}) * (radius+ minRadius) + (b3Vec3){0,height,0};
                    bodyDef.name = "object";
                    b3BodyId m_sphereBodyId = b3CreateBody( m_worldId, &bodyDef );
                    b3Sphere sphere = { { 0.0f, 0.5f, 0.0f }, radiusOfBalls};

                    b3ShapeDef shapeDef = b3DefaultShapeDef();
                    shapeDef.density = 2.0f;
                    
                    b3Body_AddGravitySource(m_sphereBodyId,sourceGravity);

                    b3CreateSphereShape( m_sphereBodyId, &shapeDef, &sphere );
                }
            }
        }
    }
};

static int sampleStressTest = RegisterSample( "Gravity", "Stress Test", StressTest::Create );


