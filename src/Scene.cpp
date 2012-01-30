/*
 *  Scene.cpp
 *	NocteBlock
 *
 *  Created by Andrea Cuius on 25/01/2011.
 *  Nocte Studio Copyright 2011 . All rights reserved.
 *	www.nocte.co.uk
 *
 */

#include "Scene.h"

#include "cinder/app/App.h"
#include "cinder/ObjLoader.h"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <fstream>


using namespace ci;
using namespace ci::app;
using namespace std;

namespace nocte {
	
	class Fixture {
		
	public:
		
		Fixture(Vec3f pos) : mPos(pos) 
		{
		};
		
		void update() 
		{
		};

		void draw() 
		{
			gl::color( Color::white() );
			gl::drawSphere( mPos, 0.5f );
		};
		
		Vec3f getPos() { return mPos; };
		
	private:
		
		Vec3f mPos;
		
	};
	
	Scene::Scene( App *app, string filename ) : mApp(app), mFilename(filename)
	{
		if ( mApp != NULL )
		{
			mCbMouseDown	= mApp->registerMouseDown( this, &Scene::mouseDown );
			mCbMouseUp		= mApp->registerMouseUp( this, &Scene::mouseUp );
			mCbMouseDrag	= mApp->registerMouseDrag( this, &Scene::mouseDrag );	
			mCbMouseMove	= mApp->registerMouseMove( this, &Scene::mouseMove );
//			mCbKeyDown		= mApp->registerKeyDown( this, &Scene::keyDown );
		}
		
		if ( mFilename != "" )
			loadCSVFile( filename );
		
		CameraPersp initialCam;
		initialCam.setPerspective( 45.0f, getWindowAspectRatio(), 0.1, 1000 );
		mMayaCam	= new MayaCamUI( initialCam );
		mIsEditable = true;
		mVboMesh	= NULL;
	}


	void Scene::update()
	{
		for(int k=0; k < mFixtures.size(); k++)
			mFixtures[k]->update();
	}


	void Scene::draw()
	{
		gl::pushMatrices();
		gl::setMatrices( mMayaCam->getCamera() );
		
		drawGrid();
		
		for(int k=0; k < mFixtures.size(); k++)
			mFixtures[k]->draw();
		
		gl::popMatrices();
		
		gl::color( Color::white() );
	}


	bool Scene::mouseDown( MouseEvent event )
	{
		if ( !mIsEditable )
			return false;
		
		if( event.isAltDown() )
			mMayaCam->mouseDown( event.getPos() );

		return false;
	}

	bool Scene::mouseUp( MouseEvent event )
	{
		if ( !mIsEditable )
			return false;
		
		return false;
	}


	bool Scene::mouseDrag( MouseEvent event )
	{
		if ( !mIsEditable )
			return false;
		
		if( event.isAltDown() )
			mMayaCam->mouseDrag( event.getPos(), event.isLeftDown(), event.isMiddleDown(), event.isRightDown() );
		
		return false;
	}

	bool Scene::mouseMove( MouseEvent event )
	{
		if ( !mIsEditable )
			return false;
		
		return false;
	}
/*
	bool Scene::keyDown( KeyEvent event )
	{
		char c		= event.getChar();
		int	 code	= event.getCode();
			
		if ( event.isMetaDown() )
		{
			if ( c == 'o' )
				open();
			
			else if ( c == 's' )
				saveAs();
		}

		return false;
	}
*/
	void Scene::open( string filename )
	{
		fs::path fsPath(filename);
		
		if ( fsPath == "" )
		{
			std::vector<std::string> extensions; extensions.push_back("xml");
			fsPath = ci::app::getOpenFilePath( "", extensions );
			
			if ( fsPath == "" )
				return;
		}
		loadCSVFile( fsPath.generic_string() );
	}
	
	void Scene::saveAs( string filename )
	{
		fs::path fsPath(filename);
		
		if ( fsPath == "" )
		{
			std::vector<std::string> extensions; extensions.push_back("xml");
			fsPath = ci::app::getSaveFilePath( "", extensions );
			
			if ( fsPath == "" )
				return;
		}
		saveCSVFile( fsPath.generic_string() );
	}
	
	/*
	void Scene::loadMesh( string filename )
	{
		fs::path fsPath(filename);
		
		if ( fsPath == "" )
		{
			fsPath = getOpenFilePath();
			
			if ( fsPath == "" )
				return;
		}
		TriMesh mesh;
		ObjLoader loader( loadFile( fsPath ) );
		loader.load( &mesh );
		mVboMesh = new gl::VboMesh( mesh );
	}
	*/

	void Scene::loadCSVFile( const string &filename )
	{
		clear();
		
		string line;
		ifstream openFile( filename.c_str() );
		
		if ( openFile.is_open() )
		{
			while ( openFile.good() )
			{
				getline(openFile,line);
				vector<string> splitValues;
				boost::split( splitValues, line, boost::is_any_of(",") );
				
				Vec3f pos;
				pos.x = boost::lexical_cast<float>(splitValues.at(0));
				pos.y = boost::lexical_cast<float>(splitValues.at(1));
				pos.z = -boost::lexical_cast<float>(splitValues.at(2));
				pos /= 100;
				
				mFixtures.push_back( new Fixture(pos) );
			}
			openFile.close();
		} 
		else
			console() << "Scene > Failed to load fixtures coordinates!" << endl;
	}
	
	void Scene::saveCSVFile( const string &filename )
	{		
		Vec3f pos;
		std::ofstream openFile( filename.c_str() );
		for(int k=0; k < mFixtures.size(); k++)
		{
			pos = mFixtures[k]->getPos();
			openFile << pos.x << "," << pos.y << "," << pos.z; 
			if ( k < mFixtures.size() - 1 )
				openFile << endl;
		}
		openFile.close();
	}
	
	void Scene::clear()
	{
		for(int k=0; k < mFixtures.size(); k++)
			delete mFixtures[k];
		mFixtures.clear();
	}
	
	void Scene::drawGrid(float size, float step)
	{
		gl::color( Colorf(0.2f, 0.2f, 0.2f) );
		for(float i=-size;i<=size;i+=step) {
			gl::drawLine( Vec3f(i, 0.0f, -size), Vec3f(i, 0.0f, size) );
			gl::drawLine( Vec3f(-size, 0.0f, i), Vec3f(size, 0.0f, i) );
		}
	}
	
}