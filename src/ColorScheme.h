/*
 *  ColorScheme.h
 *  NocteBlock
 *
 *  Created by Q on 30/01/2012.
 *  Copyright 2012 . All rights reserved.
 *
 */

#pragma once

#include "cinder/Xml.h"

namespace nocte {
		
	class ColorScheme {

	public:
		
		ColorScheme() {};
		
		ci::ColorA	getColor( const std::string &name )
		{
			return mColorsMap[name];
		};
		
		void addColor( const std::string &name, ci::ColorA col )
		{
			mColorsMap[name] = col;
		};
		
		void open( const std::string &filename = "" )
		{
			ci::fs::path fsPath(filename);
			
			if ( fsPath == "" )
			{
				std::vector<std::string> extensions; extensions.push_back("xcs");
				fsPath = ci::app::getOpenFilePath( "", extensions );
				
				if ( fsPath == "" )
					return;
			}
			
			mFilename = fsPath.generic_string();
			mColorsMap.clear();
			
			try {
				mXmlAsset = ci::XmlTree( ci::loadFile(mFilename) );
			}
			catch ( ... ) {
				ci::app::console() << "failed to load XML settings file: " << mFilename << std::endl;
				return;
			}
			
			std::list<ci::XmlTree> colorNodes = mXmlAsset.getChild("/ColorScheme").getChildren();
			
			for(std::list<ci::XmlTree>::iterator nodeIt = colorNodes.begin(); nodeIt != colorNodes.end(); ++nodeIt)
			{
				ci::ColorA col( nodeIt->getAttributeValue<float>("r"), 
								nodeIt->getAttributeValue<float>("g"), 
								nodeIt->getAttributeValue<float>("b"), 
								nodeIt->getAttributeValue<float>("a") );

				mColorsMap[ nodeIt->getAttributeValue<std::string>("name") ] = col;
			}
			
			ci::app::console() << "Color scheme loaded: " << mFilename << std::endl;
			
			list();
		};
		
		void saveAs( const std::string &filename = "" )
		{
			ci::fs::path fsPath(filename);
			
			if ( fsPath == "" )	
			{	
				std::vector<std::string> extensions; extensions.push_back("xcs");
				fsPath = ci::app::getSaveFilePath( "", extensions );
				
				if ( fsPath == "" )
					return;
			}
			
			mFilename = fsPath.generic_string();
			
			mXmlAsset = ci::XmlTree("ColorScheme", "");
			
			for( std::map<std::string, ci::ColorA>::iterator it = mColorsMap.begin(); it != mColorsMap.end(); it++ )
			{
				ci::XmlTree node = ci::XmlTree( "ColorA", "").setAttribute("name", it->first);
				node.setAttribute("r", it->second.r).setAttribute("g", it->second.g).setAttribute("b", it->second.b).setAttribute("a", it->second.a);
				mXmlAsset.push_back( node );
			}
			
			mXmlAsset.write( ci::writeFile( mFilename ) );
			
			ci::app::console() << "Color scheme saved: " << mFilename << std::endl;

			list();
		};
		
		void list()
		{
			for( std::map<std::string, ci::ColorA>::iterator it = mColorsMap.begin(); it != mColorsMap.end(); it++ )
				ci::app::console() << it->first << "\t" << it->second << std::endl;	
		};
		
	private:
		
		std::map<std::string, ci::ColorA>	mColorsMap;
		std::string							mFilename;
		ci::XmlTree							mXmlAsset;
		
	};

}
