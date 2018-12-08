# Microsoft Developer Studio Project File - Name="fisupeli" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=fisupeli - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "fisupeli.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "fisupeli.mak" CFG="fisupeli - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "fisupeli - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "fisupeli - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "fisupeli - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "F:\Code and Docs\Libraries\ijl11\include" /I "F:\Code and Docs\Libraries\fmodapi340\api\inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40b /d "NDEBUG"
# ADD RSC /l 0x40b /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 opengl32.lib glu32.lib glu32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "fisupeli - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "F:\Code and Docs\Libraries\ijl11\include" /I "F:\Code and Docs\Libraries\fmodapi340\api\inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40b /d "_DEBUG"
# ADD RSC /l 0x40b /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 opengl32.lib dxguid.lib dxerr8.lib dinput8.lib glu32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "fisupeli - Win32 Release"
# Name "fisupeli - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ASELoaderC.cpp
# End Source File
# Begin Source File

SOURCE=.\CameraC.cpp
# End Source File
# Begin Source File

SOURCE=.\ColorC.cpp
# End Source File
# Begin Source File

SOURCE=.\DecomposeAffineC.cpp
# End Source File
# Begin Source File

SOURCE=.\FishC.cpp
# End Source File
# Begin Source File

SOURCE=.\FishingContextC.cpp
# End Source File
# Begin Source File

SOURCE=.\FishingLineC.cpp
# End Source File
# Begin Source File

SOURCE=.\FishingPondC.cpp
# End Source File
# Begin Source File

SOURCE=.\FishingRodC.cpp
# End Source File
# Begin Source File

SOURCE=.\FishMeshC.cpp
# End Source File
# Begin Source File

SOURCE=.\fisupeli.cpp
# End Source File
# Begin Source File

SOURCE=.\FontC.cpp
# End Source File
# Begin Source File

SOURCE=.\glenxtensions.cpp
# End Source File
# Begin Source File

SOURCE=.\GLWindowC.cpp
# End Source File
# Begin Source File

SOURCE=.\LightC.cpp
# End Source File
# Begin Source File

SOURCE=.\Matrix3C.cpp
# End Source File
# Begin Source File

SOURCE=.\MeshC.cpp
# End Source File
# Begin Source File

SOURCE=.\QuatC.cpp
# End Source File
# Begin Source File

SOURCE=.\SceneC.cpp
# End Source File
# Begin Source File

SOURCE=.\ScenegraphItemI.cpp
# End Source File
# Begin Source File

SOURCE=.\SplineCurveC.cpp
# End Source File
# Begin Source File

SOURCE=.\TextureC.cpp
# End Source File
# Begin Source File

SOURCE=.\Vector2C.cpp
# End Source File
# Begin Source File

SOURCE=.\Vector3C.cpp
# End Source File
# Begin Source File

SOURCE=.\WaterC.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ASELoaderC.h
# End Source File
# Begin Source File

SOURCE=.\CameraC.h
# End Source File
# Begin Source File

SOURCE=.\ColorC.h
# End Source File
# Begin Source File

SOURCE=.\DecomposeAffineC.h
# End Source File
# Begin Source File

SOURCE=.\FishC.h
# End Source File
# Begin Source File

SOURCE=.\FishingContextC.h
# End Source File
# Begin Source File

SOURCE=.\FishingLineC.h
# End Source File
# Begin Source File

SOURCE=.\FishingPondC.h
# End Source File
# Begin Source File

SOURCE=.\FishingRodC.h
# End Source File
# Begin Source File

SOURCE=.\FishMeshC.h
# End Source File
# Begin Source File

SOURCE=.\FontC.h
# End Source File
# Begin Source File

SOURCE=.\glextensions.h
# End Source File
# Begin Source File

SOURCE=.\GLWindowC.h
# End Source File
# Begin Source File

SOURCE=.\LightC.h
# End Source File
# Begin Source File

SOURCE=.\Matrix3C.h
# End Source File
# Begin Source File

SOURCE=.\MeshC.h
# End Source File
# Begin Source File

SOURCE=.\PajaTypes.h
# End Source File
# Begin Source File

SOURCE=.\QuatC.h
# End Source File
# Begin Source File

SOURCE=.\SceneC.h
# End Source File
# Begin Source File

SOURCE=.\ScenegraphItemI.h
# End Source File
# Begin Source File

SOURCE=.\SplineCurveC.h
# End Source File
# Begin Source File

SOURCE=.\TextureC.h
# End Source File
# Begin Source File

SOURCE=.\Vector2C.h
# End Source File
# Begin Source File

SOURCE=.\Vector3C.h
# End Source File
# Begin Source File

SOURCE=.\WaterC.h
# End Source File
# Begin Source File

SOURCE=.\wglext.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bottom.bmp
# End Source File
# Begin Source File

SOURCE=.\res\fisupeli.rc
# End Source File
# Begin Source File

SOURCE=.\res\icon.ico
# End Source File
# Begin Source File

SOURCE=.\res\top.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\kasa\readme.html
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE="..\..\Code and Docs\Libraries\ijl11\lib\msvc\ijl11.lib"
# End Source File
# Begin Source File

SOURCE="..\..\Code and Docs\Libraries\fmodapi340\api\lib\fmodvc.lib"
# End Source File
# End Target
# End Project
