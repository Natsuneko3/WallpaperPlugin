c:
cd C:\Users\Neko\OneDrive\UePlugins\WallpaperEnginePlugin\UE5\Plugins\Wallpaper\Content
IF EXIST "Cache" rd /s /q Cache
f:
cd F:\UE_5.0\Engine\Build\BatchFiles
call RunUAT.bat BuildPlugin -Plugin="C:\Users\Neko\OneDrive\UePlugins\WallpaperEnginePlugin\UE5\Plugins\Wallpaper\Wallpaper.uplugin" -Package="C:\Users\Neko\OneDrive\UePlugins\WallpaperEnginePlugin\Package\UE5.0\Wallpaper" -Rocket
c:
cd C:\Users\Neko\OneDrive\UePlugins\WallpaperEnginePlugin\Package\UE5.0\Wallpaper
rd /s /q Binaries
rd /s /q Intermediate
cd ..\
IF EXIST "Ue5Plugins.zip" del "Ue5Plugins.zip"
7z.exe a -r ..\Ue5Plugins.zip
cd ..\ 
rd /s /q "UE5.0"