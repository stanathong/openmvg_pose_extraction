/**********************************************************
 * Project:			openmvg_pose_extraction
 * Description:		Read sfm_data.json file to get R,t
 * Last updated:	15 May 2018
 * Author:			stanathong
 **********************************************************/

#ifndef _RECONSTRUCTION_STRUCTURE_H_
#define	_RECONSTRUCTION_STRUCTURE_H_

#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <iostream>		// for standard I/O
#include <fstream>		// for std::ofstream
#include <cstddef>
#include <cstdlib>		// To use system
#include "Global.h"

class CReconstructionStructure
{
public:
	CReconstructionStructure();

	CReconstructionStructure(const std::string &_json_file_name);

	bool doInitialize(const std::string &_json_file_name);

	bool run();

private:
	bool readReconstruction();

	// Get the path to the folder to store RT files
	void getRTPath(const std::string &_json_file_name);

	// Create a folder to store RT files and initialise the output CSV file
	bool prepareOutput();

	// Produce the output results for each image frame
	bool produceRT(SHOT &shot);

	// Save camera intrinsics to the output file
	void saveIntrinsics(INTRINSICS &intrinsics);

public:
	VEC_INTRINSICS	m_cameras;
	MAP_SHOT		m_shots;

private:
	bool			m_bJSON_VALID;			// A flag indicating whether the json file is valid		
	std::string		m_json_file_name;		// Full path to the passed-in json file
	std::string		m_str_main_path;		// Path to the json file and storing the output csv file
	std::string		m_str_RT_path;			// Path to store RT files
	std::ofstream	m_f_pose;				// Output CSV file containing poses
	std::ofstream	m_f_intrinsics;			// Output CSV file containing intrinsics
};


#endif
