/*
 *  Fixture.h
 *	NocteBlock
 *
 *  Created by Andrea Cuius on 25/01/2011.
 *  Nocte Studio Copyright 2011 . All rights reserved.
 *	www.nocte.co.uk
 *
 */

#ifndef NOCTE_FIXTURE
#define NOCTE_FIXTURE

#pragma once
#include "cinder/gl/Vbo.h"


namespace nocte {

        class Fixture {
            
            friend class Scene;
            friend class Stage;
            
            
        public: 
            
            Fixture( ci::Vec3f pos, int dmxChannel, ci::gl::VboMesh *mesh = NULL ) 
            : mPos(pos), mDMXChannel(dmxChannel), mValue(0.0f), mTargetValue(0.0f), mMesh(mesh) {}
            
            ~Fixture() {}
            
            virtual void update( float val, float fadeInSpeed, float fadeOutSpeed )
            {
                mTargetValue = ci::math<float>::clamp(val, 0.0f, 1.0f);
                
                if ( mValue < mTargetValue ) 
                    mValue = std::min(mValue + fadeInSpeed, mTargetValue);
                else 
                    mValue = std::max(mValue - fadeOutSpeed , mTargetValue);
                
          //      mBrightness = math<float>::clamp( mValue * gLiveMasterBrightness + gLiveBaseBrightness, 0.0f, 1.0f );
                //	console() << "fixture " << mBrightness << " " <<  mValue << " " << gLiveMasterBrightness << " " << gLiveBaseBrightness << endl;
            }
            
            virtual void render() 
            {
                if ( !mMesh )
                    return;
                
                ci::gl::color( ci::ColorA( 0.9f, 0.21f, 0.34f, mValue ) );
                ci::gl::pushMatrices();
                ci::gl::translate(mPos);
                ci::gl::draw( *mMesh );
                ci::gl::popMatrices();
            }
            
            ci::Vec3f   getPos() { return mPos; }
            
            ci::Vec2f	getPos2f() { return mPos.xz(); }
            
            float getValue() { return mValue; }
            
//            void addValue( float val )
//            {
//                mTargetValue = ci::math<float>::clamp( mTargetValue + val, 0.0f, 1.0f );
//            }
//            
            void setMesh( ci::gl::VboMesh *mesh ) { mMesh = mesh; }
            
        protected:
            
            ci::Vec3f           mPos;
            int                 mDMXChannel;
            float               mValue;             // val normalised
            float               mTargetValue;
            ci::gl::VboMesh     *mMesh;
            
        private:
            // disallow
            Fixture(const Fixture&);
            Fixture& operator=(const Fixture&);
        
        };
}

#endif