cd C:\Users\Neko\OneDrive\UePlugins\WallpaperEnginePlugin\Content
IF EXIST "Cache" rd /s /q Cache
D:
cd D:\UE_4.26\Engine\Build\BatchFiles
call RunUAT.bat BuildPlugin -Plugin="C:\Users\Neko\OneDrive\UePlugins\WallpaperEnginePlugin\Wallpaper.uplugin" -Package="C:\Users\Neko\OneDrive\UePlugins\WallpaperEnginePlugin\Package\UE4.26\Wallpaper" -Rocket
c:
cd C:\Users\Neko\OneDrive\UePlugins\WallpaperEnginePlugin\Package\UE4.26\Wallpaper
rd /s /q Binaries
rd /s /q Intermediate
cd ..\
IF EXIST "Ue4.26Plugins.zip" del "Ue4.26Plugins.zip"
7z.exe a -r ..\Ue4.26Plugins.zip
cd ..\ 
rd /s /q "UE4.26"