/*
 *  Logger.h
 *  MoodLight
 *
 *  Created by Andrea Cuius on 07/04/2011.
 *  Copyright 2011 . All rights reserved.
 *
 */

#pragma once

#include "cinder/app/AppBasic.h"
#include <iostream>
#include <fstream>
#include <time.h>
#include "cinder/Utilities.h"
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

#define LOGS_FOLDER_NAME		"/Logs/"

#define	KEEP_LOGS_FOR_N_DAYS	15


namespace nocte {
	
	
	class Logger {
		
	public:
		
		static void init() 
		{
			ci::createDirectories( ci::app::getAppPath().generic_string() + LOGS_FOLDER_NAME );

			log( "App started :]" );
	/*
			// delete all the files older than KEEP_FOR_N_DAYS
			boost::filesystem::directory_iterator end_itr;
			for ( boost::filesystem::directory_iterator itr( LOGS_PATH ); itr != end_itr; ++itr )
			{
				std::vector<std::string> fileNameStrs;
				std::string filename = itr->path().filename().generic_string();
				//boost::split(fileNameStrs, itr->leaf(), boost::is_any_of("."));
				boost::split(fileNameStrs, filename, boost::is_any_of("."));
				boost::gregorian::date logDate(boost::gregorian::from_string(fileNameStrs[0]));
				
				if ( logDate <  boost::gregorian::day_clock::local_day() - boost::gregorian::days(KEEP_LOGS_FOR_N_DAYS) ) {
					ci::deleteFile( LOGS_PATH + filename );
					log( "delete log file: " + filename );
					//	ci::deleteFile( LOGS_PATH + itr->leaf() );
					//	log( "delete log file: " + itr->leaf() );
				}
			}
	 */
		};
		
		
		static void log(const std::string msg) 
		{
			std::string fileName = ci::app::getAppPath().generic_string() + LOGS_FOLDER_NAME + getTodayFileName();
			std::ofstream logFile( fileName.c_str(), std::ios::app );		
			logFile << getTodayDateTime() << " >> " << msg << std::endl;
			logFile.close();
			ci::app::console() << msg << std::endl;		
		};
		
	private:
		
		static std::string getTodayFileName() 
		{
			std::string str;
			str += to_iso_extended_string( boost::gregorian::day_clock::local_day()) + ".txt";
			return  str;
		}
		
		
		static std::string getTodayDateTime() 
		{ 
			char buffer [80];
			time_t rawtime;
			time ( &rawtime );
			struct tm * timeinfo = localtime( &rawtime );
			strftime (buffer,80,"%c",timeinfo);
			return (std::string)buffer;
		};
		
	};
	
}

