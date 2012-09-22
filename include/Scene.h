/*
 *  Scene.h
 *	NocteBlock
 *
 *  Created by Andrea Cuius on 25/01/2011.
 *  Nocte Studio Copyright 2012 . All rights reserved.
 *	www.nocte.co.uk
 *
 */

#ifndef NOCTE_SCENE
#define NOCTE_SCENE

#pragma once

#include "cinder/MayaCamUI.h"
#include "cinder/gl/Vbo.h"
#include "Fixture.h"

namespace nocte {        
        

    class Scene {
    
        friend class Stage;
        
    public:
        
        Scene( ci::app::App *app, const std::string &fixtureMesh, const std::string &venueMesh );
        
        Scene(  ci::app::App *app, const std::string &fixturesCoords, bool flipZ, const std::string &fixtureMesh, const std::string &venuMesh );
        
        ~Scene();
        
        void init( ci::app::App *app, const std::string &fixtureMesh, const std::string &venueMesh );
        
        void render();
        
        void update( float* values, float fadeIn = 1.0f, float fadeOut = 1.0f )
        {	
            for(int k=0; k < mFixtures.size(); k++) 
                mFixtures[k]->update( values[k], fadeIn, fadeOut );
        }
        
        int getFixturesN() { return mFixtures.size(); }
        
        Fixture* getFixture( int n ) { return mFixtures[n]; }
        
        std::vector<Fixture*> getFixtures() { return mFixtures; }
        
        void begin();
        
        void end();
        
        void renderLabels();
        
        void renderValues();
        
        ci::MayaCamUI*  getMayaCam() { return mMayaCam; }
        
        ci::CameraPersp getCamera() { return mMayaCam->getCamera(); }
        
        void setCamera( ci::CameraPersp cam ) { mMayaCam->setCurrentCam( cam ); }
        
        void renderFrustum( ci::ColorA col = ci::ColorA::white() )
        {
            ci::gl::color( col );
            ci::gl::drawFrustum( mMayaCam->getCamera() );
            ci::gl::color( ci::Color::white() );
        }   
        
        void toggleGrid() { mRenderGrid = !mRenderGrid; }

        void setGridColor( ci::ColorA col ) { mGridColor = col; }
        
        void addFixture( Fixture *fixture, bool setMesh = true )         // temp method, this is to load subclass of Fixture
        { 
            if ( setMesh )
                fixture->setMesh(mFixtureMesh);

            mFixtures.push_back( fixture );
        } 
        
        void setFixtureValue( int idx, float val )
        {
            if ( idx < mFixtures.size() )
                mFixtures[idx]->mValue = ci::math<float>::clamp( val, 0.0f, 1.0f );
        }
        
    protected:
                
        void importFixturesFile( const std::string &filename, bool flipZ );
        
        void loadFixtureMesh( const std::string &filename );
        
        void loadVenueMesh( const std::string &filename );
        
        void renderGrid( int steps=10, float size=1.0f )      // size in meters
        {
            ci::gl::color( mGridColor );
            
            float halfLineLength = size * steps * 0.5f;     // half line length
            
            for(float i=-halfLineLength; i<=halfLineLength; i+=size) {
                ci::gl::drawLine( ci::Vec3f(i, 0.0f, -halfLineLength), ci::Vec3f(i, 0.0f, halfLineLength) );
                ci::gl::drawLine( ci::Vec3f(-halfLineLength, 0.0f, i), ci::Vec3f(halfLineLength, 0.0f, i) );
            }
            
            ci::gl::drawCoordinateFrame();
        }
        
        bool mouseDown( ci::app::MouseEvent event )
        {
            if( event.isAltDown() )
            {
                mMayaCam->mouseDown( event.getPos() );
                return true;
            }
            
            return false;
        }
        
        bool mouseDrag( ci::app::MouseEvent event )
        {
            if( event.isAltDown() )
            {
                mMayaCam->mouseDrag( event.getPos(), event.isLeftDown(), event.isMiddleDown(), event.isRightDown() );
                return true;
            }
            return false;            
        }
        
        bool resize( ci::app::ResizeEvent event )
        {
            ci::CameraPersp initialCam;
            initialCam.setPerspective( 45.0f, ci::app::getWindowAspectRatio(), 0.1, 3000 );
            mMayaCam->setCurrentCam( initialCam );
            return true;
        }

    private:
        
        std::vector<Fixture*>       mFixtures;
        ci::gl::VboMesh             *mFixtureMesh;
        ci::gl::VboMesh             *mVenueMesh;
        
        ci::app::App                *mApp;
        ci::CallbackId              mCbMouseDown, mCbMouseDrag, mCbResizeWindow;
        ci::MayaCamUI               *mMayaCam;
        bool                        mRenderGrid;
        ci::ColorA                  mGridColor;

    private:
        // disallow
        Scene(const Scene&);
        Scene& operator=(const Scene&);
        
    };

}

#endif