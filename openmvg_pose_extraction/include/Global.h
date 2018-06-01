/**********************************************************
 * Project:			openmvg_pose_extraction
 * Description:		Read sfm_data.json file to get R,t
 * Last updated:	15 May 2018
 * Author:			stanathong
 **********************************************************/

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#pragma once

#include <string>
#include <iostream>
#include <iomanip>
#include <cstdlib> 
#include "Definition.h"

extern const int IMAGE_WIDTH;
extern const int IMAGE_HEIGHT;

// Get the size of the file in order to allocate a sufficient buffer
int getFileSize(char * file_name);

// Compare two string case insensitively
bool caseInsensitiveStringCompare(const std::string& str1, const std::string& str2);

// Convert double to string with precision
// Ref: http://stackoverflow.com/questions/16605967/set-precision-of-stdto-string-when-converting-floating-point-values
std::string to_string_with_precision(double a_value, int n = 12);

// Determine if the directory separator is either BACK_SLASH '\' or FORWARD_SLASH '/'
// by checking the passed-in path
bool isDSBackSlash(std::string _path);

#endif
