c:
cd C:\Users\Neko\OneDrive\UePlugins\WallpaperEnginePlugin\Content
IF EXIST "Cache" rd /s /q Cache

d:
cd D:\UE_5.1\Engine\Build\BatchFiles
call RunUAT.bat BuildPlugin -Plugin="C:\Users\Neko\OneDrive\UePlugins\WallpaperEnginePlugin\Wallpaper.uplugin" -Package="C:\Users\Neko\OneDrive\UePlugins\WallpaperEnginePlugin\Package\UE5.1\Wallpaper" -Rocket

c:
cd C:\Users\Neko\OneDrive\UePlugins\WallpaperEnginePlugin\Package\UE5.1\Wallpaper
IF EXIST "Binaries" rd /s /q Binaries
IF EXIST "Intermediate" rd /s /q Intermediate
cd ..\
IF EXIST "UE5_1Plugins.zip" del "UE5_1Plugins.zip"
7z.exe a -r ..\UE5.1Plugins.zip
cd ..\ 
rd /s /q "UE5.1"
