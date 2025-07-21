SET PluginPath="%~dp0WallPaper.uplugin"
SET PackagePath="%~dp0WallPaper5_4"
D:
cd "D:\Epic Games\UE_5.4\Engine\Build\BatchFiles"
call RunUAT.bat BuildPlugin -Plugin=%PluginPath% -Package=%PackagePath% -Rocket
cd %PackagePath%
del /Q Source
cd ..
7z.exe a -r WallPaper5_4.zip .\WallPaper5_4
rd /S /Q CustomRenderFeature5_4
pause