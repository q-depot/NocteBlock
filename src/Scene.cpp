/*
 *  Scene.h
 *	NocteBlock
 *
 *  Created by Andrea Cuius on 25/01/2011.
 *  Nocte Studio Copyright 2011 . All rights reserved.
 *	www.nocte.co.uk
 *
 */

#include "cinder/App/App.h"
#include "cinder/ObjLoader.h"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <fstream>

#include "Scene.h"

#include "Resources.h"


using namespace ci;
using namespace ci::app;
using namespace std;


namespace nocte {
	
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
        
        
        Scene::Scene( int sceneID, const string &fixturesCoords, bool flipZ, const string &fixtureMesh, const string &venueMesh ) : mSceneID( sceneID ), mFixtureMesh(NULL), mVenueMesh(NULL)
        {
            loadFixtureMesh( fixtureMesh );
            
            loadVenueMesh( venueMesh );
            
            importFixturesFile( fixturesCoords, flipZ );    
        }
        
        
        void Scene::render()
        {
            // render venue
            if ( mVenueMesh )
            {
                gl::color( ColorA( 1.0f, 1.0f, 1.0f, 0.2f ) );
                gl::draw( *mVenueMesh );
                gl::color( Color::white() );
            }
            
            // render fixtures
            if ( mFixtureMesh )
            {                
                // optimise this shit, draw as vbo!!!     <<<<<<<<<<<<<<<<<<<<<<<<<<<<<
                for( size_t i=0; i < mFixtures.size(); i++ )
                {
                    gl::color( ColorA( 0.9f, 0.21f, 0.34f, mFixtures[i]->mValue ) );
                    gl::pushMatrices();
                    gl::translate(mFixtures[i]->mPos);
                    gl::draw( *mFixtureMesh );
                    gl::popMatrices();
                }
            }
        
        }
        
        
        void Scene::loadFixtureMesh( const std::string &filename )
        {
            if ( mFixtureMesh )
                delete mFixtureMesh;
            
            mFixtureMesh = NULL;
            
            ObjLoader loader( (DataSourceRef)loadResource(filename) );
            TriMesh	mesh; 
            loader.load( &mesh );
            mFixtureMesh = new gl::VboMesh( mesh );   
        }
        
        
        void Scene::loadVenueMesh( const std::string &filename )
        {
            if ( mVenueMesh )
                delete mVenueMesh;
            
            mVenueMesh = NULL;
            
            ObjLoader loader( (DataSourceRef)loadResource(filename) );
            TriMesh	mesh; 
            loader.load( &mesh );
            mVenueMesh = new gl::VboMesh( mesh );   
        }
        
        
        void Scene::importFixturesFile( const string &filename, bool flipZ )
        {            
            ifstream openFile( filename.c_str() );
            
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
                    
                    mFixtures.push_back( new Fixture(pos, dmxChannel) );
                    
                    //                    ci::app::console() << "Scene > Import fixture - DMX_" << dmxChannel << " " << pos << std::endl;
                }
                
                //                ci::app::console() << "Scene > Imported " + ci::toString(mFixtures.size()) + " fixtures" << std::endl;
                openFile.close();
            } 
            else 
                ci::app::console() << "Scene > Failed to load fixtures coordinates: " << filename << std::endl;
            
        }
    
}