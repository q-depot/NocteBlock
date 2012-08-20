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

//#include "cinder/App/App.h"
#include "cinder/MayaCamUI.h"
#include "cinder/gl/Vbo.h"
//#include "cinder/ObjLoader.h"
//
//#include <boost/algorithm/string.hpp>
//#include <boost/lexical_cast.hpp>
//#include <fstream>


namespace nocte {

        class Fixture {
            
            friend class Scene;
            friend class Stage;
            
            
        public: 
            
            Fixture( ci::Vec3f pos, int dmxChannel ) : mPos(pos), mDMXChannel(dmxChannel), mValue(1.0f), mTargetValue(1.0f) {}
            
            void update( float val, float fadeInSpeed, float fadeOutSpeed )
            {
                mTargetValue = ci::math<float>::clamp(val, 0.0f, 1.0f);
                
                if ( mValue < mTargetValue ) 
                    mValue = std::min(mValue + fadeInSpeed, mTargetValue);
                else 
                    mValue = std::max(mValue - fadeOutSpeed , mTargetValue);
                
          //      mBrightness = math<float>::clamp( mValue * gLiveMasterBrightness + gLiveBaseBrightness, 0.0f, 1.0f );
                //	console() << "fixture " << mBrightness << " " <<  mValue << " " << gLiveMasterBrightness << " " << gLiveBaseBrightness << endl;
            }
            
            
            ci::Vec3f   getPos() { return mPos; }
            
            ci::Vec2f	getPos2f() { return ci::Vec2f(mPos.x, mPos.z); }
            
            float getValue() { return mValue; }
            
        protected:
            
            ci::Vec3f   mPos;
            int         mDMXChannel;
            float       mValue;             // val normalised
            float       mTargetValue;
            
        };
        
        
        class Scene {
        
            friend class Stage;
            
        public:
            
            Scene(  int sceneID, const std::string &fixturesCoords, bool flipZ, const std::string &fixtureMesh, const std::string &venuMesh );
            
            void render();
            
            void update( float *values, int buffSize, float fadeIn, float fadeOut )
            {	
                if ( buffSize != mFixtures.size() )
                    return;
                
                for(int k=0; k < buffSize; k++) 
                    mFixtures[k]->update( values[k], fadeIn, fadeOut );
            }
            
            int getFixturesN() { return mFixtures.size(); }
            
            Fixture* getFixture( int n ) { return mFixtures[n]; }
            
            int  getId() { return mSceneID; };
            
        protected:
            
            void importFixturesFile( const std::string &filename, bool flipZ );
            
            void loadFixtureMesh( const std::string &filename );
            
            void loadVenueMesh( const std::string &filename );
            
        private:
            
            std::vector<Fixture*>       mFixtures;
            ci::gl::VboMesh             *mFixtureMesh;
            ci::gl::VboMesh             *mVenueMesh;
            int                         mSceneID;
            
        };

}