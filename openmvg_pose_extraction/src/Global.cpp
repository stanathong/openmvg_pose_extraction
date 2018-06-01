/**********************************************************
 * Project:			openmvg_pose_extraction
 * Description:		Read sfm_data.json file to get R,t
 * Last updated:	15 May 2018
 * Author:			stanathong
 **********************************************************/

#include "../include/Global.h"

const int IMAGE_WIDTH		= 1920;
const int IMAGE_HEIGHT		= 1080;

// Get the size of the file in order to allocate a sufficient buffer
int getFileSize(char * file_name)
{
	int file_size = -1;

	FILE * pfile = fopen(file_name, "r");
	if (NULL != pfile)
	{
		// Go to the end of the file
		if (fseek(pfile, 0L, SEEK_END) == 0)
		{
			// Get the size of the file
			file_size = ftell(pfile);
		}

		fclose(pfile);
	}

	return file_size;
};

// Compare two string case insensitively
bool caseInsensitiveStringCompare(const std::string& str1, const std::string& str2) 
{
    if (str1.size() != str2.size()) 
        return false;
    
    for (std::string::const_iterator c1 = str1.begin(), c2 = str2.begin(); c1 != str1.end(); ++c1, ++c2) 
	{
        if (tolower(*c1) != tolower(*c2))
            return false;
    }
    
	return true;
};

// Convert to double string with precision
std::string to_string_with_precision(double a_value, int n)
{
    std::ostringstream out;
    out << std::setprecision(n) << a_value;
    return out.str();
};

// Determine if the directory separator is either BACK_SLASH "\\" or FORWARD_SLASH "/"
// by checking the passed-in path
bool isDSBackSlash(std::string _path)
{
	std::size_t pos = _path.find("\\");
	if (pos != std::string::npos)
		return true; 
	
	return false;
};

