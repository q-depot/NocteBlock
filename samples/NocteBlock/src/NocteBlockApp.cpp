/*
 *  Nocte.h
 *	NocteBlockApp
 *
 *  Created by Andrea Cuius on 25/01/2011.
 *  Nocte Studio Copyright 2011 . All rights reserved.
 *	www.nocte.co.uk
 *
 * - Scene
 * - Dmx
 * - XmlSettings
 * - Logger (doesn't delete old files yet, need to fix it)
 * - Osc
 
 * - Midi
 * - Params?
 * - Parallel machines
 * - XmlColorScheme
 * - Timeline
 * - Dynamic loading modules
 *
 */


#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/params/Params.h"

#include "Nocte.h"

#include "OscListener.h"
#include "OscSender.h"

using namespace ci;
using namespace ci::app;
using namespace std;

#define OSC_HOST		"localhost"
#define OSC_OUT_PORT	8000
#define OSC_IN_PORT		9000


class NocteBlockApp : public AppBasic {
  public:
	void setup();
	void mouseDown( MouseEvent event );
	void keyDown( KeyEvent event );
	void update();
	void draw();
	void initParams();
	void openHandler( const string &name );
	void saveAsHandler( const string &name );
	
	nocte::Scene		*mScene;
	nocte::XmlSettings	*mSettings;
	nocte::ColorScheme	*mColorScheme;
	
	osc::Listener		*mOscListener;
	osc::Sender			*mOscSender;
	
	params::InterfaceGl	*mParams;
	
};

void NocteBlockApp::setup()
{
	mScene			= new nocte::Scene(this);					// Init Scene
	mSettings		= new nocte::XmlSettings();					// Init Settings
	mColorScheme	= new nocte::ColorScheme();					// Init ColorScheme
	
	nocte::Logger::init();										// Init Logger (static class)
	
//	mOscListener	= new osc::Listener(OSC_IN_PORT);			// Init Osc Listener
//	mOscSender		= new osc::Listener(OSC_HOST, OSC_IN_PORT);	// Init Osc Listener
	
	mColorScheme->addColor("red", ColorA(1.0f, 0.0f, 0.0f, 1.0f) );
	
	initParams();
}

void NocteBlockApp::mouseDown( MouseEvent event )
{
}

void NocteBlockApp::keyDown( KeyEvent event )
{
	char c = event.getChar();
	
	if ( event.isMetaDown() )		
	{
		
	}
}

void NocteBlockApp::update()
{
	mScene->update();
	
	/*
	
	// Listener
	
	while (mOscListener->hasWaitingMessages()) {
		osc::Message message;
		mOscListener->.getNextMessage(&message);
		
		console() << "New message received" << std::endl;
		console() << "Address: " << message.getAddress() << std::endl;
		console() << "Num Arg: " << message.getNumArgs() << std::endl;
		for (int i = 0; i < message.getNumArgs(); i++) {
			console() << "-- Argument " << i << std::endl;
			console() << "---- type: " << message.getArgTypeName(i) << std::endl;
			if (message.getArgType(i) == osc::TYPE_INT32){
				try {
					console() << "------ value: "<< message.getArgAsInt32(i) << std::endl;
				}
				catch (...) {
					console() << "Exception reading argument as int32" << std::endl;
				}
				
			}else if (message.getArgType(i) == osc::TYPE_FLOAT){
				try {
					console() << "------ value: " << message.getArgAsFloat(i) << std::endl;
				}
				catch (...) {
					console() << "Exception reading argument as float" << std::endl;
				}
			}else if (message.getArgType(i) == osc::TYPE_STRING){
				try {
					console() << "------ value: " << message.getArgAsString(i).c_str() << std::endl;
				}
				catch (...) {
					console() << "Exception reading argument as string" << std::endl;
				}
				
			}
		}
        
        if( message.getNumArgs() != 0 && message.getArgType( 0 ) == osc::TYPE_FLOAT )
        {
            positionX = message.getArgAsFloat(0);
        }
		
	}
	 
	 
	 // Sender
	 osc::Message message;
	 message.addFloatArg(positionX);
	 message.setAddress("/cinder/osc/1");
	 message.setRemoteEndpoint(host, port);
	 mOscSender->sendMessage(message);
*/	
}

void NocteBlockApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) ); 
	
	mScene->draw();
	
	gl::setMatricesWindow( getWindowSize() );
	
	mSettings->drawDebug();
	
	mParams->draw();
	
}


void NocteBlockApp::initParams() 
{
	mParams	= new params::InterfaceGl( "Params", Vec2i(200, 300) );
	
	mParams->addButton( "Open scene", std::bind( &NocteBlockApp::openHandler, this, "Scene" ) );
	mParams->addButton( "Save scene", std::bind( &NocteBlockApp::saveAsHandler, this, "Scene" ) );
	
	mParams->addButton( "Open settings", std::bind( &NocteBlockApp::openHandler, this, "Settings" ) );
	mParams->addButton( "Save settings", std::bind( &NocteBlockApp::saveAsHandler, this, "Settings" ) );

	mParams->addButton( "Open colors", std::bind( &NocteBlockApp::openHandler, this, "Colors" ) );
	mParams->addButton( "Save colors", std::bind( &NocteBlockApp::saveAsHandler, this, "Colors" ) );
	
}

void NocteBlockApp::openHandler( const string &name )
{
	if ( name == "Scene" )
		mScene->open();
	else if ( name == "Settings" )
		mSettings->open();
	else if ( name == "Colors" )
		mColorScheme->open();
}

void NocteBlockApp::saveAsHandler( const string &name )
{
	if ( name == "Scene" )
		mScene->saveAs();
	else if ( name == "Settings" )
		mSettings->saveAs();
	else if ( name == "Colors" )
		mColorScheme->saveAs();
}

CINDER_APP_BASIC( NocteBlockApp, RendererGl(0) )
