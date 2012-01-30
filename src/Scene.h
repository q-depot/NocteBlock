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
#include <vector>


namespace nocte {
	
	class Fixture;

	class Scene {

	public:

		Scene( ci::app::App *app = NULL, std::string filename = "" );
			
		void update();
		
		void draw();
		
		bool	mouseDown( ci::app::MouseEvent event );
		bool	mouseUp( ci::app::MouseEvent event );
		bool	mouseDrag( ci::app::MouseEvent event );
		bool	mouseMove( ci::app::MouseEvent event );
		bool	keyDown( ci::app::KeyEvent event );	
		
		bool	isEditable() { return mIsEditable; };
		void	setEditable( bool edit = true ) { mIsEditable = edit; };
		
		void	open(std::string filename = "");
		void	saveAs(std::string filename = "");
		
		void	loadMesh(std::string filename = "");
		
	private:
		
		void	loadCSVFile( const std::string &filename );
		void	saveCSVFile( const std::string &filename );
		void	clear();
		void	drawGrid(float size=100.0f, float step=10.0f);
		
		ci::app::App			*mApp;
		ci::CallbackId			mCbMouseDown, mCbMouseUp, mCbMouseDrag, mCbMouseMove, mCbKeyDown;
		bool					mIsEditable;
		ci::MayaCamUI			*mMayaCam;
		std::string				mFilename;
		std::vector<Fixture*>	mFixtures;
		ci::gl::VboMesh			*mVboMesh;
	};

}