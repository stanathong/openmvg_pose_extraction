/**********************************************************
 * Project:			openmvg_pose_extraction
 * Description:		Read sfm_data.json file to get R,t
 * Last updated:	15 May 2018
 * Author:			stanathong
 **********************************************************/

#ifndef _DEFINITION_H_
#define	_DEFINITION_H_

#pragma once
#pragma warning (disable:4786)

#include <vector>
#include <string>
#include <map>
#include <utility>					// std::pair, std::make_pair
#include <opencv2/core/core.hpp> 

// Structure of extrinsics
struct SHOT
{
	std::string image_name;
	cv::Mat center_prior;
	cv::Mat center;
	cv::Mat rotation;
	cv::Mat translation;
	int intrinsics_id;

	// Default constructor
	SHOT() 	
	{	
		image_name = "";
		center_prior = cv::Mat::zeros(3, 1, CV_64FC1);
		center = cv::Mat::zeros(3, 1, CV_64FC1);
		rotation = cv::Mat::eye(3, 3, CV_64FC1);
		translation = cv::Mat::zeros(3, 1, CV_64FC1);
		intrinsics_id = 0;
	}

	// Constructor with passed-in view as cv::Mat
	SHOT(std::string _image_name, cv::Mat _center_prior, int _intrinsics_id) 	
	{	
		image_name = _image_name;
		center_prior = _center_prior.clone();
		center = cv::Mat::zeros(3, 1, CV_64FC1);
		rotation = cv::Mat::eye(3, 3, CV_64FC1);
		translation = cv::Mat::zeros(3, 1, CV_64FC1);
		intrinsics_id = _intrinsics_id;
	}

	// Constructor with passed-in view as doubles
	SHOT(std::string _image_name, double _center_prior_x, double _center_prior_y, 
		double _center_prior_z, int _intrinsics_id) 	
	{	
		image_name = _image_name;
		center_prior = (cv::Mat_<double>(3,1) << _center_prior_x, _center_prior_y, _center_prior_z);
		center = cv::Mat::zeros(3, 1, CV_64FC1);
		rotation = cv::Mat::eye(3, 3, CV_64FC1);
		translation = cv::Mat::zeros(3, 1, CV_64FC1);
		intrinsics_id = _intrinsics_id;
	}
};

typedef std::map< int, SHOT >	MAP_SHOT;
typedef MAP_SHOT::iterator		MAP_SHOT_ITER;

// Structure of intrinsics
struct INTRINSICS
{
	int intrinsics_id;
	int width;
	int height;
	double f, ppx, ppy;
	double k1, k2, k3, p1, p2;

	INTRINSICS() 	
	{	
		intrinsics_id = 0;
		width = 1920;
		height = 1080;
		f = 1800.0;
		ppx = 960.0;
		ppy = 540.0;
		k1 = k2 = k3 = p1 = p2 = 0.0;
	}

	INTRINSICS(int _intrinsics_id, int _width, int _height, double _f, double _ppx, double _ppy,
		double _k1, double _k2, double _k3, double _p1, double _p2) 	
	{	
		intrinsics_id = _intrinsics_id;
		width = _width;
		height = _height;
		f = _f;
		ppx = _ppx;
		ppy = _ppy;
		k1 = _k1;
		k2 = _k2;
		k3 = _k3;
		p1 = _p1;
		p2 = _p2;
	}
};

typedef std::vector< INTRINSICS >	VEC_INTRINSICS;

#endif
