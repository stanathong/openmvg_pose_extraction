# Camera Poses and Intrinsics Extraction from OpenMVG's Results

[OpenMVG](https://github.com/openMVG/openMVG) offers a full set of functionalities towards Multiple View Geometry Reconstruction. Part of its library includes a complete Structure from Motion pipeline.<br>

To estimate the camera poses (without structures i.e. no sparse or dense pointcloud), we can shorten the pipeline provided [tutorial_demo.py](https://github.com/openMVG/openMVG/blob/master/src/software/SfM/tutorial_demo.py.in) to only run the following processes: <br>

```
openMVG_main_SfMInit_ImageListing
openMVG_main_ComputeFeatures
openMVG_main_ComputeMatches
openMVG_main_IncrementalSfM --OR-- openMVG_main_GlobalSfM

```
<br>
Completing this pipeline, we will obtain `sfm_data.bin` which could be named differently according to your running command. To produce the readable results, we will need to excecute `openMVG_main_ConvertSfM_DataFormat`. The command can be executed like below:<br><br>

```
cd $OPEN_MVG_BUILD/Linux-x86_64-RELEASE
openMVG_main_ConvertSfM_DataFormat -i sfm_data.bin -o sfm_data.json -V -I -E

```

where _sfm_data.bin_ is the SfM binary result produced by OpenMVG which will be converted to a json file named as _sfm_data.json_, hence `-o`, and `-V -I -E` are entered such that views, intrinsics and camera poses (extrinsics), in respective order, will be exported. 
<br><br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;The __views__ section contains the image file names and camera's centre prior (i.e. when GPS data are read from exif).<br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;The __intrinsics__ section contains the camera intrinsics i.e. focal length, priciple points, and distortion coefficients.<br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;The __extrinsics__ section contains the camera rotation matrix `R` and the camera center `C`, in which we can derive the perspective projection matrix as `P = K[R|t]` in which `t=-RC` and `K` is the intrinsics matrix.
<br>

# openmvg_pose_extraction

In this repository, a utility to extract camera pose `[R|t]` for each image and the camera intrinsics are extracted and stored as csv and txt files for further processing.<br>

### Development
The code is developed in C++ using MS Visual Studio 2012 BUT there is no reference to any MFC specific functions. It uses [RapidJson](http://rapidjson.org/) to read the content from the json file. However, downloading RapidJson is not required as all the necessary files are already included in this project under the folder "external".<br>

### Running Command
After building, it can be run on __Windows__ by using command
`openmvg_pose_extraction FULL_PATH_TO_SFM_DATA.json`. For example, <br>
```
openmvg_pose_extraction D:\\OpenMVG_Results\\sfm_data.json

```
And running on __Linux__ by using command <br>
```
openmvg_pose_extraction /home/usr/source/OpenMVG_Results/sfm_data.json

```
Notice the slash (that is `\\` for Windows and `/` for Linux) as the difference between the two operating systems. This is because the program will have to execute the `mkdir PATH\\RT` or `mkdir PATH/RT` command to create a folder named `RT` under the path where the file `sfm_data.json` is located.

### Output
An `RT` folder is created under `PATH` where sfm_data.json is located. The folder contains text files named as "<Image_file_name>.txt" each of which stores a 3x4 matrix corresponding to `[R|t]`. It also produces 2 CSV files: `intrinsics.csv` and `poses.csv`. `intrinsics.csv` have the intrinsics parameters for each camera specified by intrinsics_id. `poses.csv` contains camera poses.
