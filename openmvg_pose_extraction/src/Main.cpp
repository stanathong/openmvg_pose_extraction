/**********************************************************
 * Project:			openmvg_pose_extraction
 * Description:		Read sfm_data.json file to get R,t
 * Last updated:	15 May 2018
 * Author:			stanathong
 **********************************************************/

#include <cstdlib>
#include <chrono>			// To measure time

#include "../include/ReconstructionStructure.h"

void ShowUsage();

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		ShowUsage();
		return -1;
	}

	CReconstructionStructure obj(argv[1]);
	obj.run();

	return 0;
}

/*
 * Print out the help message.
 */
void ShowUsage()
{
	std::cout << "OpenMVG_Pose_Extraction\n";
	std::cout << "     Desc:      This program extracts R, C for each image from the resulting sfm data in json file.\n";
	std::cout << "				  The result is stored in a csv file.\n";
	std::cout << "     Usage:     openmvg_pose_extraction <full-path to sfm_data.json>\n";
	std::cout << "     Example:\n";
	std::cout << "     (Linux)    openmvg_pose_extraction /home/user/source/OpenMVG_Build/software/SfM/DATASET/reconstruction/sfm_data.json\n";
	std::cout << "     (Windows)  openmvg_pose_extraction G:\\OpenMVG_Build\\software\\SfM\\DATASET\\reconstruction\\sfm_data.json\n";
	std::cout << std::endl;
}
