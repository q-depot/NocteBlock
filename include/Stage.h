/*
 *  Scene.h
 *	NocteBlock
 *
 *  Created by Andrea Cuius on 25/01/2011.
 *  Nocte Studio Copyright 2011 . All rights reserved.
 *	www.nocte.co.uk
 *
 */

#pragma once

#include "Scene.h"

namespace nocte {

    class Stage {
    
    public:
        
        Stage( ci::app::App *app ) : mApp(app), mRenderGrid(true)
        {
            if ( mApp != NULL )
            {
                mCbMouseDown	= mApp->registerMouseDown(  this, &Stage::mouseDown );
                mCbMouseDrag	= mApp->registerMouseDrag(  this, &Stage::mouseDrag );
                mCbResizeWindow	= mApp->registerResize(     this, &Stage::resize );
            }
            
            ci::CameraPersp initialCam;
            initialCam.setPerspective( 45.0f, ci::app::getWindowAspectRatio(), 0.1, 3000 );
            mMayaCam	= new ci::MayaCamUI( initialCam );
        }
        
        void begin()
        {
            ci::gl::pushMatrices();
            
            ci::gl::setMatrices( mMayaCam->getCamera() );
            
            if ( mRenderGrid )
                renderGrid();
            
            for(size_t k=0; k < mScenes.size(); k++)
                mScenes[k]->render();
            
        }
        
        void end()
        {
            ci::gl::popMatrices();
        }
        
        ci::MayaCamUI*  getMayaCam() { return mMayaCam; }
        
        ci::CameraPersp getCamera() { return mMayaCam->getCamera(); }
        
        void renderFrustum( ci::ColorA col = ci::ColorA::white() )
        {
            ci::gl::color( col );
            ci::gl::drawFrustum( mMayaCam->getCamera() );
            ci::gl::color( ci::Color::white() );
        }   
        
        void toggleGrid() { mRenderGrid = !mRenderGrid; }
            
        void loadScene( Scene *scene ) { mScenes.push_back( scene ); }
        
        Scene* getScene( int sceneId )
        {
            Scene *scene = NULL;
            
            for(size_t k=0; k < mScenes.size(); k++)
                if ( mScenes[k]->mSceneID == sceneId )
                    return mScenes[k];
            
            return scene;
        }
        
    private:
        
        void renderGrid( int steps=10, float size=1.0f )      // size in meters
        {
            ci::gl::color( ci::Color(0.2f, 0.2f, 0.2f) );
            
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
        
        ci::app::App                *mApp;
        ci::CallbackId              mCbMouseDown, mCbMouseDrag, mCbResizeWindow;
        ci::MayaCamUI               *mMayaCam;
        bool                        mRenderGrid;
        std::vector<Scene*>         mScenes;
    };

}