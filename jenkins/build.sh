#!/bin/sh
rm -rf build dist *.zip

echo "updating git modules"
git submodule init
git submodule update
echo "building..."
rm -rf build
mkdir build
cd build

build () {
	echo building $1
	if [ $1 == 'x86_64' ]; then
		cmake -G "Visual Studio 14 2015 Win64" ..
	elif [ $1 == 'x86' ]; then
		cmake -G "Visual Studio 14 2015" ..
	fi
	msbuild.exe trace-client.sln /p:Configuration=Release;OutputPath=foo
	mkdir -p ../dist/bin/$1
	cp bin/Release/ayxiatrace.dll ../dist/bin/$1
	cp bin/Release/sample_*.exe ../dist/bin/$1
	mkdir -p ../dist/lib/$1
	cp bin/Release/ayxiatrace.lib ../dist/lib/$1
	cp bin/Release/libuv.lib ../dist/lib/$1
}

build x86
build x86_64

mkdir -p ../dist/include
cp -r ../include/ ../dist/include

cd ../dist
zip -r ../ayxia-client-sdk-`git describe --abbrev=0 --tags`.zip *
