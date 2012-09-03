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

    
    Scene::Scene( App *app, const string &fixtureMesh, const string &venueMesh ) : mApp(app)
    {
        init( app, fixtureMesh, venueMesh ); 
    }


    Scene::Scene( App *app, const string &fixturesCoords, bool flipZ, const string &fixtureMesh, const string &venueMesh )
    {                   
        init( app, fixtureMesh, venueMesh ); 
        
        importFixturesFile( fixturesCoords, flipZ );
    }
     
    void Scene::init( App *app, const string &fixtureMesh, const string &venueMesh )
    {
        mApp            = app;
        mFixtureMesh    = NULL;
        mVenueMesh      = NULL;
        mRenderGrid     = true;
        mMayaCam        = NULL;
        
        loadFixtureMesh( fixtureMesh );
        
        loadVenueMesh( venueMesh );
        
        mCbMouseDown	= mApp->registerMouseDown(  this, &Scene::mouseDown );
        mCbMouseDrag	= mApp->registerMouseDrag(  this, &Scene::mouseDrag );
        mCbResizeWindow	= mApp->registerResize(     this, &Scene::resize );
        
        ci::CameraPersp initialCam;
        initialCam.setPerspective( 45.0f, ci::app::getWindowAspectRatio(), 0.1, 3000 );
        mMayaCam        = new ci::MayaCamUI( initialCam );
        
        mGridColor      = ci::ColorA(0.2f, 0.2f, 0.2f, 1.0f);
        
        console() << "init scene" << endl;

    }
    
    
    Scene::~Scene() 
    {
        for( size_t k=0; k < mFixtures.size(); k++ )
            delete mFixtures[k];
        mFixtures.clear();
        
        delete mFixtureMesh;
        delete mVenueMesh;
        delete mMayaCam;
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
        
        for( size_t i=0; i < mFixtures.size(); i++ )
            mFixtures[i]->setMesh(mFixtureMesh); 
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
        string fname = loadResource(filename)->getFilePath().generic_string();
        ifstream openFile( fname.c_str() );
        
        float       scale       = 1000.0f;
        int         dmxChannel  = 0;
        
        ci::Vec3f   pos;
        std::string line;
        
        if (openFile.is_open())
        {                
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
                
                mFixtures.push_back( new Fixture(pos, dmxChannel, mFixtureMesh) );
                
                //                    ci::app::console() << "Scene > Import fixture - DMX_" << dmxChannel << " " << pos << std::endl;
            }
            
            //                ci::app::console() << "Scene > Imported " + ci::toString(mFixtures.size()) + " fixtures" << std::endl;
            openFile.close();
        } 
        else 
            ci::app::console() << "Scene > Failed to load fixtures coordinates: " << filename << std::endl;
        
    }
        
    
    void Scene::begin()
    {
        gl::pushMatrices();
        
        gl::setMatrices( mMayaCam->getCamera() );
        
        if ( mRenderGrid )
            renderGrid();
        
        // render venue
        if ( mVenueMesh )
        {
            gl::color( ColorA( 1.0f, 1.0f, 1.0f, 0.2f ) );
            gl::draw( *mVenueMesh );
            gl::color( Color::white() );
        }
        
        // render fixtures
        for( size_t i=0; i < mFixtures.size(); i++ )
            mFixtures[i]->render();
        
//        if ( mFixtureMesh )
//        {                
//            // optimise this shit, draw as vbo!!!     <<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//            for( size_t i=0; i < mFixtures.size(); i++ )
//            {
//                gl::color( ColorA( 0.9f, 0.21f, 0.34f, mFixtures[i]->mValue ) );
//                gl::pushMatrices();
//                gl::translate(mFixtures[i]->mPos);
//                gl::draw( *mFixtureMesh );
//                gl::popMatrices();
//            }
//        }
        
    }
    
    void Scene::end()
    {
        gl::popMatrices();
    }
    

}


