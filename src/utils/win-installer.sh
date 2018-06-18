#! /usr/bin/sh

# Script to create a Windows Installer using the QtInstallerFramework
# @author Marcos Cardinot

ROOTDIR="/c/evoplex"
CONFIGXML="$ROOTDIR/evoplex/src/utils/win-config.xml"
PACKAGEXML="$ROOTDIR/evoplex/src/utils/win-package.xml"
LICENSE=$ROOTDIR/evoplex/LICENSE.txt
LOGO=$ROOTDIR/evoplex/src/evoplex.png

EVOPLEX_VERSION="$(build/bin/evoplex -version)"
TODAY="$(date +'%Y-%m-%d')"

function setXML() {
sed -i.bac "/<$1>/,/<\/$1>/ s/<$2><\/$2>/<$2>$3<\/$2>/g;" $4
}

set -v

cd $ROOTDIR/build/bin
windeployqt.exe .
cd $ROOTDIR


mkdir win-installer win-installer/config win-installer/packages
cd win-installer

cd config
cp $CONFIGXML config.xml
setVersion config.xml
setXML Installer Version $EVOPLEX_VERSION config.xml
cd ..

mkdir packages/com.evoplex.evoplex
cd packages/com.evoplex.evoplex
mkdir meta data

cd meta
cp $LICENSE $LOGO .
cp $PACKAGEXML package.xml
setXML Package Version $EVOPLEX_VERSION package.xml
setXML Package ReleaseDate $TODAY package.xml
cd ..

cd data
cp -r $ROOTDIR/build/bin/. .
cp -r $ROOTDIR/build/lib/ .
mkdir bin
mv evoplex.exe bin/
cd ..


cd $ROOTDIR/win-installer
binarycreator.exe --offline-only -c config/config.xml -p packages evoplex_$EVOPLEX_VERSION.exe
