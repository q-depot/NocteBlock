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

#include "cinder/App/App.h"
#include "cinder/MayaCamUI.h"
#include "cinder/gl/Vbo.h"
#include "cinder/ObjLoader.h"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <fstream>


namespace nocte {
	
    namespace scene {    

        class Fixture {
            
            friend class Scene;
            friend class Group;
            
        public: 
            
            Fixture( ci::Vec3f pos, int dmxChannel ) : mPos(pos), mDMXChannel(dmxChannel), mVal(0.0f) {}
            
        protected:
            
            ci::Vec3f   mPos;
            int         mDMXChannel;
            float       mVal;             // val normalised
            
        };
        
        /*
        class Group {
            
            friend class Scene;
            
        public:
            
            Group( int groupID, std::vector<Fixture*> fixtures ) : mGroupID(groupID), mFixtures(fixtures)
            {
                // calc bounding box
                if ( fixtures.empty() )
                    return;
                
//                mValues = new float[mFixtures.size()];
//                for(size_t k=0; k < fixtures.size(); k++)
//                    mValues[k] = 0.0f;
                
                
                ci::Vec3f lowerFrontRight   = fixtures[0]->mPos;
                ci::Vec3f upperBackLeft     = fixtures[0]->mPos;
                
                Fixture *fixture;
                
                for( size_t k=1; k < fixtures.size(); k++ )
                {
                    fixture = fixtures[k];
                    
                    // X
                    if ( fixture->mPos.x < lowerFrontRight.x )
                        lowerFrontRight.x = fixture->mPos.x;
                    
                    if ( fixture->mPos.x > upperBackLeft.x )
                        upperBackLeft.x = fixture->mPos.x;
                    
                    // Y
                    if ( fixture->mPos.y < lowerFrontRight.y )
                        lowerFrontRight.y = fixture->mPos.y;
                    
                    if ( fixture->mPos.y > upperBackLeft.y )
                        upperBackLeft.y = fixture->mPos.y;
                    
                    // Z
                    if ( fixture->mPos.z < lowerFrontRight.z )
                        lowerFrontRight.z = fixture->mPos.z;
                    
                    if ( fixture->mPos.z > upperBackLeft.z )
                        upperBackLeft.z = fixture->mPos.z;
                }
                
                mBoundingBoxCenter  = upperBackLeft + ( lowerFrontRight - upperBackLeft  ) / 2.0f;
                mBoundingBoxSize    = lowerFrontRight - upperBackLeft;
            }
            
            void renderBoundingBox()
            {
                ci::gl::enableWireframe();
                ci::gl::drawCube(mBoundingBoxCenter, mBoundingBoxSize );
                ci::gl::disableWireframe();
            }
            
            int getFixturesN() 
            {
                return mFixtures.size();
            }
            
            void updateValues( float *values )
            {
                for( int k=0; k < mFixtures.size(); k++ )
                    mFixtures[k]->mVal = values[k];
            }
            
            int getID() { return mGroupID; }
            
        protected:
            
            int                                 mGroupID;
            std::vector<Fixture*>               mFixtures;
            ci::Vec3f                           mBoundingBoxCenter;
            ci::Vec3f                           mBoundingBoxSize;
        };
        
        */
        class Scene {
        
        public:

            Scene( ci::app::App *app = NULL) : mApp(app), mDrawGrid(true), mVenueMesh(NULL), mFixtureMesh(NULL)
            {
                if ( mApp != NULL )
                {
                    mCbMouseDown	= mApp->registerMouseDown( this, &Scene::mouseDown );
                    mCbMouseDrag	= mApp->registerMouseDrag( this, &Scene::mouseDrag );
                }
                
                ci::CameraPersp initialCam;
                initialCam.setPerspective( 45.0f, ci::app::getWindowAspectRatio(), 0.1, 3000 );
                mMayaCam	= new ci::MayaCamUI( initialCam );
            }

            void begin() 
            {
                ci::gl::pushMatrices();
                
                ci::gl::setMatrices( mMayaCam->getCamera() );
                
                if ( mDrawGrid )
                    drawGrid();
                
                // render venue
                if ( mVenueMesh )
                {
                    ci::gl::color( ci::ColorA( 1.0f, 1.0f, 1.0f, 0.2f ) );
                    ci::gl::draw( *mVenueMesh );
                    ci::gl::color( ci::Color::white() );
                }
                
                // render fixtures
                if ( mFixtureMesh )
                {
                    
                    Group   *group;
                    Fixture *fixture;
                    
                    for( size_t k=0; k < mGroups.size(); k++ )
                    {
                        group = mGroups[k];
                            
                        ci::gl::color( ci::ColorA::white() );
                        
                        group->renderBoundingBox();
                        
                        // optimise this shit, draw as vbo!!!     <<<<<<<<<<<<<<<<<<<<<<<<<<<<<
                        for( size_t i=0; i < group->mFixtures.size(); i++ )
                        {
                            fixture = group->mFixtures[i];
                            ci::gl::color( ci::ColorA( 0.9f, 0.21f, 0.34f, fixture->mVal ) );
                            ci::gl::pushMatrices();
                            ci::gl::translate(fixture->mPos);
                            ci::gl::draw( *mFixtureMesh );
                            ci::gl::popMatrices();
                        } 
                    }
                }
                
            }
            
            void end()
            {
                ci::gl::popMatrices();
            }
                    
            ci::MayaCamUI*  getMayaCam() { return mMayaCam; }
            
            ci::CameraPersp getCamera() { return mMayaCam->getCamera(); }
            
            void            drawFrustum( ci::ColorA col = ci::ColorA::white() )
            {
                ci::gl::color( col );
                ci::gl::drawFrustum( mMayaCam->getCamera() );
                ci::gl::color( ci::Color::white() );
            }
            
            void toggleGrid() { mDrawGrid = !mDrawGrid; };
            
            
            void addGroup( int groupID, const std::string &filename, bool flipZ = true )
            {
                std::vector<Fixture*> fixtures = loadFixtures( filename, flipZ );
                mGroups.push_back( new Group( groupID, fixtures ) );
            }
            
            Group* getGroup( int groupID )
            {
                Group *group = NULL;
                
                for( size_t k=0; k < mGroups.size(); k++ )
                    if ( groupID == mGroups[k]->mGroupID )
                    {
                        group = mGroups[k];
                    }
                
                return group;
            }
            
            void loadFixtureMesh( const std::string &filename )
            {
                if ( mFixtureMesh )
                    delete mFixtureMesh;
                
                mFixtureMesh = NULL;
                
                ci::ObjLoader loader( (ci::DataSourceRef)ci::app::loadResource(filename) );
                ci::TriMesh	mesh; 
                loader.load( &mesh );
                mFixtureMesh = new ci::gl::VboMesh( mesh );   
            }
            
            void loadVenueMesh( const std::string &filename )
            {
                if ( mVenueMesh )
                    delete mVenueMesh;
                
                mVenueMesh = NULL;
                
                ci::ObjLoader loader( (ci::DataSourceRef)ci::app::loadResource(filename) );
                ci::TriMesh	mesh; 
                loader.load( &mesh );
                mVenueMesh = new ci::gl::VboMesh( mesh );   
            }
            
            
        private:
            
            std::vector<Fixture*> loadFixtures( const std::string &filename, bool flipZ )
            {
                std::vector<Fixture*> fixtures;
                
                std::ifstream openFile( filename.c_str() );
                
                float       scale       = 1000.0f;
                int         dmxChannel  = 0;
                
                ci::Vec3f   pos;
                std::string line;
                
                if (openFile.is_open())
                {
    //                ci::app::console() << "Scene > loadFixtures " << filename << std::endl;
                    
                    while ( openFile.good() )
                    {
                        getline(openFile,line);
                        std::vector<std::string> splitValues;
                        boost::split(splitValues, line, boost::is_any_of(","));						// get comma separated values
                        
                        if ( splitValues.size() != 4 )
                            continue;
                        
                        pos.x = boost::lexical_cast<float>(splitValues.at(0));
                        pos.y = boost::lexical_cast<float>(splitValues.at(1));
                        pos.z = boost::lexical_cast<float>(splitValues.at(2));

                        if ( flipZ ) pos.z *= -1;
                            
                        pos /= scale;
                        
                        dmxChannel = boost::lexical_cast<int>(splitValues.at(3));
                        
                        fixtures.push_back( new Fixture(pos, dmxChannel) );
                        
    //                    ci::app::console() << "Scene > Import fixture - DMX_" << dmxChannel << " " << pos << std::endl;
                    }
                    
    //                ci::app::console() << "Scene > Imported " + ci::toString(mFixtures.size()) + " fixtures" << std::endl;
                    openFile.close();
                } 
                else 
                {
                    ci::app::console() << "Scene > Failed to load fixtures coordinates!" << std::endl;
                }
                
                return fixtures;
            }
            
            void drawGrid( int steps=10, float size=1.0f )      // size in meters
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

            
        private:
            
            ci::app::App                *mApp;
            ci::CallbackId              mCbMouseDown, mCbMouseDrag;
            ci::MayaCamUI               *mMayaCam;
            bool                        mDrawGrid;
            std::vector<Group*>         mGroups;
            ci::gl::VboMesh             *mFixtureMesh;
            ci::gl::VboMesh             *mVenueMesh;
            
        };

    }
}