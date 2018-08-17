Introduction
=================
SOCEngine is a simple 3D rendering framework written in C++

Previous project : github.com/TeamSOC/SOCEngine-Prototype

Requirements
=================
- Windows 7 later
- DirectX 11
- Visual Studio 2015
- Visual Studio 2012
- Python 3
- FBX SDK 2014.2.1

Features
=================
- Tile Based Rendering!
  - Deferred
  - Forward+
- MSAA (Only 4x)
- Physically Based Rendering
- Voxel Cone Tracing
- Frustum Culling
- Post Processing
  - Bloom / Eye Adaptation
  - Depth Of Field
  - Screen Space Ambient Occlusion
  - Sun Shaft
  - Motion blur
- Fbx-Conv model/material loading
- Shadow
  - PCF (Point, Spot light)
  - PCSS (Directional light)
- Skybox
- Atmospheric Scattering
  - Preetham Model
- Normal Mapping
- hlsl parser and generate shader initialization code.

Some Screens
=================
<img src="https://i.imgur.com/i6yyxXF.png" width="300"><img src="https://i.imgur.com/0NF1ijK.png" width="300"><img src="https://i.imgur.com/WR1KGw3.png" width="300"><img src="https://i.imgur.com/0NF1ijK.png" width="300"><img src="https://i.imgur.com/NJX7Yoj.png" width="300"><img src="https://i.imgur.com/3EyhDQT.png" width="300"><img src="https://i.imgur.com/PlsxsJ4.png" width="300"><img src="https://i.imgur.com/04KffSr.png" width="300"><img src="https://i.imgur.com/i3FklGO.png" width="300"><img src="https://i.imgur.com/6pDAXww.png" width="300"><img src="https://i.imgur.com/HEGuINm.png" width="300"><img src="https://i.imgur.com/gKdlH4B.png" width="300"><img src="https://i.imgur.com/rSshNB8.png" width="300"><img src="https://i.imgur.com/84IRZt7.png" width="300"><img src="https://i.imgur.com/A2BqAvY.png" width="300"><img src="https://i.imgur.com/k2N2g76.png" width="300"><img src="https://i.imgur.com/uC8DAuu.png" width="300"><img src="https://i.imgur.com/GpHI9On.png" width="300"><img src="https://i.imgur.com/BuZ3C3X.png" width="300"><img src="https://i.imgur.com/CjfGc37.png" width="300"><img src="https://i.imgur.com/RwZti2y.png" width="300"><img src="https://i.imgur.com/XDZB2Rw.png" width="300"><img src="https://i.imgur.com/xsyXMDS.png" width="300"><img src="https://i.imgur.com/n8e6ryi.png" width="300"><img src="https://i.imgur.com/0RbffuX.png" width="300">


To-do
=================
- Irradiance Volume
  - SH probe
- Hierarchical Z Buffer Occlusion Culling
- Volumetric light scattering
- Screen Space Decal
- Screen Space Local Reflection
- Animation
  - Forward kinematics
  - Inverse kinematics
- FXAA
- Improve voxel cone tracing
  - Casecade
  - Other issues
  
License
=================
MIT License
