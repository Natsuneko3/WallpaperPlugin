cd C:\Users\Neko\OneDrive\UePlugins\WallpaperEnginePlugin\Content
IF EXIST "Cache" rd /s /q Cache
e:
cd E:\UE_4.27\Engine\Build\BatchFiles
call RunUAT.bat BuildPlugin -Plugin="C:\Users\Neko\OneDrive\UePlugins\WallpaperEnginePlugin\Wallpaper.uplugin" -Package="C:\Users\Neko\OneDrive\UePlugins\WallpaperEnginePlugin\Package\UE4.27\Wallpaper" -Rocket
c:
cd C:\Users\Neko\OneDrive\UePlugins\WallpaperEnginePlugin\Package\UE4.27\Wallpaper
IF EXIST "Binaries" rd /s /q Binaries
IF EXIST "Intermediate" rd /s /q Intermediate
cd ..\
IF EXIST "Ue4.27Plugins.zip" del "Ue4.27Plugins.zip"
7z.exe a -r ..\Ue4.27Plugins.zip
cd ..\ 
rd /s /q "UE4.27"