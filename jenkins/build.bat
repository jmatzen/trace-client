rem cmd /c "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x64
set PATH="C:\Program Files (x86)\MSBuild\14.0\Bin";%PATH%;M:\cygwin64\bin;"C:\program files\git\bin";"C:\Windows\Microsoft.NET\Framework64\v4.0.30319"
cd ..
git submodule init
git submodule update
rmdir /s/q build
mkdir build
cd build
cmake -G "Visual Studio 14 2015 Win64" ..
msbuild trace-client.sln /p:Configuration=Release;OutputPath=out
xcopy /y/e bin\release\ayxiatrace.dll ..\dist\bin\x86_64\
xcopy /y/e bin\release\sample_*.exe ..\dist\bin\x86_64\
xcopy /y/e bin\release\ayxiatrace.lib ..\dist\lib\x86_64\
xcopy /y/e bin\release\libuv.lib ..\dist\lib\x86_64\
cd ..
rmdir /s/q build
mkdir build
cd build
cmake -G "Visual Studio 14 2015" ..
msbuild trace-client.sln /p:Configuration=Release;OutputPath=out
xcopy /y/e bin\release\ayxiatrace.dll ..\dist\bin\x86\
xcopy /y/e bin\release\sample_*.exe ..\dist\bin\x86\
xcopy /y/e bin\release\ayxiatrace.lib ..\dist\lib\x86\
xcopy /y/e bin\release\libuv.lib ..\dist\lib\x86\
xcopy /y/e ..\include ..\dist\include\
del ayxiatrace.zip
cd ..\dist
zip -r ..\ayxiatrace.zip *
sh ..\jenkins\ver.sh
