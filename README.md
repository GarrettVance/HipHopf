# HipHopf
DirectX D3D11 rendering of the Hopf Fibration. A brief video is online at https://youtu.be/rXuUOs55AN0. 

This is a Windows 10 UWP project built with Visual Studio 2017. For a more responsive graphics experience, please build for x64 Release rather than debug. 

Rendering the Hopf Fibration demands some deep mathematical results, most of which I attribute to Niles Johnson (also on github) and the excellent paper "An Elementary Introduction to the Hopf Fibration" by David W. Lyons. 

Dependencies: The HipHopf project depends on two external libraries, namely Microsoft DirectXTK and DirectXMesh. As of October 22, 2019, these dependencies are satisfied through the nuget functionality in VisualStudio: namely Tools -> NuGet Package Manager -> Package Manager Console. See the "packages.config" file in this repository. If nuget is unavailable, download these libraries from Microsoft's github page (both DirectXTK and DirectXMesh distributions provide Visual Studio solution files targeting Windows 10 UWP). Build these libraries for x64, after which you can build the HipHopf project. 
