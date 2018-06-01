/**********************************************************
 * Project:			openmvg_pose_extraction
 * Description:		Read sfm_data.json file to get R,t
 * Last updated:	15 May 2018
 * Author:			stanathong
 **********************************************************/

#include <stdio.h>				// To manage FILE
#include <stdlib.h>				// To manage FILE
#include <iostream>
#include "../external/rapidjson/document.h"
#include "../external/rapidjson/filereadstream.h"
#include "../include/ReconstructionStructure.h"

CReconstructionStructure::CReconstructionStructure()
{
}

CReconstructionStructure::CReconstructionStructure(const std::string &_json_file_name)
{
	doInitialize(_json_file_name);
}

bool CReconstructionStructure::doInitialize(const std::string &_json_file_name)
{
	m_json_file_name = _json_file_name;

	// Get the path to store the resulting RT files
	getRTPath(_json_file_name);

	return true;
}

bool CReconstructionStructure::readReconstruction()
{
	// Declaration of buffer
	int BUFFER_SIZE = 65535;
	char * buffer;
	buffer = reinterpret_cast<char *> (malloc(sizeof(char) * (BUFFER_SIZE+1))); 
	if (!buffer)
	{
		m_bJSON_VALID = false;
		return false;
	}

	// Prepare the output
	if (!prepareOutput())
		return false;

	// Open the json file to read
	FILE * pfile = fopen(m_json_file_name.c_str(), "r");
	rapidjson::FileReadStream frstream(pfile, buffer, BUFFER_SIZE);
	rapidjson::Document document;
	document.ParseStream(frstream);
	fclose(pfile);
	free(buffer);

	if (document.HasParseError())
	{
		std::cout << "Failed to parse the reconstruction file: " << m_json_file_name << std::endl;		
		m_bJSON_VALID = false;
		return false;
	}

	// "views"
	if (document["views"].IsArray())
	{
		rapidjson::Value::ConstValueIterator it;
		for (it = document["views"].Begin(); it != document["views"].End(); ++it)
		{
			int key = 0;
			if (it->HasMember("key"))
				key = (*it)["key"].GetInt();
			
			if (it->HasMember("value") && (*it)["value"].IsObject())
			{
				const rapidjson::Value& camera_data = (*it)["value"]["ptr_wrapper"]["data"];

				// Construct shot with "views" content
				SHOT shot(	camera_data["filename"].GetString(), 
							camera_data["center"][0].GetDouble(),
							camera_data["center"][1].GetDouble(),
							camera_data["center"][2].GetDouble(),
							camera_data["id_intrinsic"].GetInt() );
				
				int id_pose = camera_data["id_pose"].GetInt();
				
				// Add the short to the map with id_pose as key
				m_shots.insert(std::make_pair(id_pose, shot));
			}
		}
	}

	// "intrinsics"
	if (document["intrinsics"].IsArray())
	{
		rapidjson::Value::ConstValueIterator it;
		for (it = document["intrinsics"].Begin(); it != document["intrinsics"].End(); ++it)
		{
			int key;
			if (it->HasMember("key"))
				key = (*it)["key"].GetInt();
			else
			{
				std::cout << "Error! No intrinsics ID!" << std::endl;
				continue;
			}

			// Only support camera model: "pinhole_brown_t2"
			if (it->HasMember("value") && 
				caseInsensitiveStringCompare((*it)["value"]["polymorphic_name"].GetString(), "pinhole_brown_t2"))
			{
				const rapidjson::Value& intrinsics_data = (*it)["value"]["ptr_wrapper"]["data"];

				// Construct the camera intrinsics
				INTRINSICS intrinsics(key,	intrinsics_data["value0"]["width"].GetInt(),
											intrinsics_data["value0"]["height"].GetInt(),
											intrinsics_data["value0"]["focal_length"].GetDouble(),
											intrinsics_data["value0"]["principal_point"][0].GetDouble(),
											intrinsics_data["value0"]["principal_point"][1].GetDouble(),
											intrinsics_data["disto_t2"][0].GetDouble(),
											intrinsics_data["disto_t2"][1].GetDouble(),
											intrinsics_data["disto_t2"][2].GetDouble(),
											intrinsics_data["disto_t2"][3].GetDouble(),
											intrinsics_data["disto_t2"][4].GetDouble() );

				// Output the camera intrinsics to file
				saveIntrinsics(intrinsics);

				// Add the camera intrinsics to the vector of camera intrinsics
				//m_cameras.push_back(intrinsics);
			}
		}
	}

	// "extrinsics"
	if (document["extrinsics"].IsArray())
	{
		rapidjson::Value::ConstValueIterator it;
		for (it = document["extrinsics"].Begin(); it != document["extrinsics"].End(); ++it)
		{
			int key;
			if (it->HasMember("key"))
				key = (*it)["key"].GetInt();
			else
			{
				std::cout << "Error! No extrinsics ID!" << std::endl;
				continue;
			}

			// Obtain the shot for the corresponding key (id_extrinsics)
			SHOT shot;
			MAP_SHOT_ITER shot_iter = m_shots.find(key);
			if (shot_iter != m_shots.end())
			{
				//shot = shot_iter->second;
			}
			else
			{
				std::cout << "Error! No extrinsics ID!" << std::endl;
				continue;			
			}

			if (it->HasMember("value") && (*it)["value"].HasMember("rotation") && (*it)["value"]["rotation"].IsArray())
			{
				// Obtain "rotation"
				const rapidjson::Value& rotation_object = (*it)["value"]["rotation"];

				// Continue to the next extrinsics if the number of rows in R != 3
				if (rotation_object.MemberCount() != 3) 
				{
					std::cout << "Error! Invalid rotation matrix for index: " << key << std::endl;
					continue;
				}

				cv::Mat R(3, 3, CV_64FC1);
				for (int r = 0; r < 3; r++)
					for (int c = 0; c < 3; c++)
						R.at<double>(r,c) = rotation_object[r][c].GetDouble();

				// Test with transpose matrix
				//R = R.t();

				m_shots[key].rotation = R.clone();

				// Obtain "center"
				const rapidjson::Value& center_object = (*it)["value"]["center"];

				// Continue to the next extrinsics if the number of rows in C != 3
				if (center_object.MemberCount() != 3) 
				{
					std::cout << "Error! Invalid C matrix for index: " << key << std::endl;
					continue;
				}

				cv::Mat C(3, 1, CV_64FC1);
				for (int r = 0; r < 3; r++)
					C.at<double>(r,0) = center_object[r].GetDouble();	

				m_shots[key].center = C.clone();

				// Compute the translation vector
				cv::Mat t = -R*C;

				m_shots[key].translation = t.clone();

				// Produce the output
				if ( !produceRT(m_shots[key]) )
					return false;

				// Note since we are not processing further, do not need to store the shot
				m_shots.erase(shot_iter);
			}
		}
	}

	return true;
}

// Create a folder to store RT files and initialise the output CSV files
// RT file is in the format:
//	
//		r11 r12 r13 t1
//		r21 r22 r23 t2
//		r31 r32 r33 t3
//
bool CReconstructionStructure::prepareOutput()
{
	// Create the RT folder to store RT files
	std::string command = "mkdir " + m_str_RT_path;
	system(command.c_str());

	// Initialise the pose CSV file
	std::string output_filename = m_str_RT_path + "poses.csv" ;
	m_f_pose.open(output_filename.c_str(), std::ios::out);
	if (!m_f_pose.is_open())
	{
		std::cout << "Error! Failed to produce output: " << output_filename << std::endl;
		std::cout << "Please check if this path valid: " << m_str_RT_path << std::endl;
		return false;
	}

	// Store the file header
	m_f_pose << "image,center_prior_x,center_prior_y,center_prior_z,center_x,center_y,center_z,";
	m_f_pose << "r11,r12,r13,r21,r22,r23,r31,r32,r33,t1,t2,t3,intrinsics_id\n";

	// Initialise the intrinsics CSV file
	output_filename = m_str_RT_path + "intrinsics.csv" ;
	m_f_intrinsics.open(output_filename.c_str(), std::ios::out);
	if (!m_f_intrinsics.is_open())
	{
		std::cout << "Error! Failed to produce output: " << output_filename << std::endl;
		std::cout << "Please check if this path valid: " << m_str_RT_path << std::endl;
		return false;
	}

	// Store the file header
	m_f_intrinsics << "intrinsics_id,width,height,f,ppx,ppy,k1,k2,k3,p1,p2\n";
	
	return true;
}

// The main running function
bool CReconstructionStructure::run()
{
	if (!readReconstruction())
		return false;

	// Close the output files
	m_f_pose.close();
	m_f_intrinsics.close();

	return true;
}

// Save camera intrinsics to the output file
void CReconstructionStructure::saveIntrinsics(INTRINSICS &intrinsics)
{
	m_f_intrinsics << intrinsics.intrinsics_id << ",";
	m_f_intrinsics << intrinsics.width << ",";
	m_f_intrinsics << intrinsics.height << ",";
	m_f_intrinsics << to_string_with_precision(intrinsics.f) << ",";
	m_f_intrinsics << to_string_with_precision(intrinsics.ppx) << ",";
	m_f_intrinsics << to_string_with_precision(intrinsics.ppy) << ",";
	m_f_intrinsics << to_string_with_precision(intrinsics.k1) << ",";
	m_f_intrinsics << to_string_with_precision(intrinsics.k2) << ",";
	m_f_intrinsics << to_string_with_precision(intrinsics.k3) << ",";
	m_f_intrinsics << to_string_with_precision(intrinsics.p1) << ",";
	m_f_intrinsics << to_string_with_precision(intrinsics.p2) << "\n";
}

// Produce RT matrix files for each image frame
bool CReconstructionStructure::produceRT(SHOT &shot)
{
	// Construct the output file as image_name.txt e.g. Carriageway_0.jpg.txt
	std::string filename = m_str_RT_path + shot.image_name + ".txt"; 
	std::ofstream fileRT(filename.c_str(), std::ofstream::out);
	if (!fileRT.is_open())
		return false;

	// Save RT matrix into the output RT file
	for (int r = 0; r < 3; r++)
	{
		std::string line =	to_string_with_precision(shot.rotation.at<double>(r,0)) + " " + 
							to_string_with_precision(shot.rotation.at<double>(r,1)) + " " +
							to_string_with_precision(shot.rotation.at<double>(r,2)) + " " +
							to_string_with_precision(shot.translation.at<double>(r,0));
		fileRT << line << "\n";
	}

	fileRT.close();

	// Store the pose into the output CSV file
	// image name
	m_f_pose << shot.image_name << ",";
	// center_prior_x, center_prior_y, center_prior_z
	for (int r = 0; r < 3; r++)
		m_f_pose << to_string_with_precision( shot.center_prior.at<double>(r,0) ) << ",";
	// center_x, center_y, center_z
	for (int r = 0; r < 3; r++)
		m_f_pose << to_string_with_precision( shot.center.at<double>(r,0) ) << ",";
	// r11,r12,r13,r21,r22,r23,r31,r32,r33
	for (int r = 0; r < 3; r++)
		for (int c = 0; c < 3; c++)
			m_f_pose << to_string_with_precision( shot.rotation.at<double>(r,c) ) << ",";
	// t1,t2,t3
	for (int r = 0; r < 3; r++)
		m_f_pose << to_string_with_precision( shot.translation.at<double>(r,0) ) << ",";
	// intrinsics_id
	m_f_pose << shot.intrinsics_id << "\n";

	return true;
}

// Get the path to the folder to store RT files
// Example: _json_file_name = DATA_PATH/xxx.json
// RTPath will be constructed as DATA_PATH/RT/
void CReconstructionStructure::getRTPath(const std::string &_json_file_name)
{
	// Extract the data path and construct the RT path
	std::size_t pos = _json_file_name.find_last_of("\\/");
	if (pos == std::string::npos) // Not found - the data path can be empty
	{
		m_str_main_path = "";
#ifdef _WIN32
		m_str_RT_path = "RT\\";
#else
		m_str_RT_path = "RT/";
#endif
	}
	else
	{
		m_str_main_path = _json_file_name.substr(0, pos);
#ifdef _WIN32
		m_str_RT_path = m_str_main_path + "\\RT\\";
#else
		m_str_RT_path = m_str_main_path + "/RT/";
#endif
	}
}
