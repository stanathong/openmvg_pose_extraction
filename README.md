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
Completing this pipeline, we will obtain `sfm_data.bin`, which could be named differently according to your running command. To produce the readable results, we will need to excecute `openMVG_main_ConvertSfM_DataFormat`. The command can be executed like below:<br>

```
$OPEN_MVG_BUILD/Linux-x86_64-RELEASE/openMVG_main_ConvertSfM_DataFormat -i sfm_data.bin -o sfm_data.json -V -I -E
```
<br>
where `sfm_data.bin` is the SfM binary result produced by OpenMVG which will be converted to a json file named as `sfm_data.json` and -V -I -E are entered such that views, intrinsics and camera poses (extrinsics) will be exported. <br>

In this repository, a utility to extract camera pose `[R|t]` for each image and the camera intrinsics are extracted and stored as csv and txt files for further processing.<br>
