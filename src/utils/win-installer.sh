#! /usr/bin/sh

# Script to create a Windows Installer using the QtInstallerFramework
# @author Marcos Cardinot

ROOTDIR="/c/evoplex"
CONFIGXML="$ROOTDIR/evoplex/src/utils/win-config.xml"
PACKAGEXML="$ROOTDIR/evoplex/src/utils/win-package.xml"
INSTALLERSCRIPT="$ROOTDIR/evoplex/src/utils/win-installerscript.qs"
LICENSE="$ROOTDIR/evoplex/LICENSE.txt"
LOGO="$ROOTDIR/evoplex/src/evoplex.png"
TODAY="$(date +'%Y-%m-%d')"

EVOPLEX_RELEASE="$(build/bin/evoplex -version)"
EVOPLEX_VERSION=$(echo $EVOPLEX_RELEASE | sed 's/-.*//')

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
cp $LOGO .
cp $CONFIGXML config.xml
setVersion config.xml
setXML Installer Version $EVOPLEX_VERSION config.xml
setXML Installer Title "Evoplex $EVOPLEX_RELEASE" config.xml
cd ..

mkdir packages/com.evoplex.evoplex
cd packages/com.evoplex.evoplex
mkdir meta data

cd meta
cp $INSTALLERSCRIPT $LICENSE .
cp $PACKAGEXML package.xml
setXML Package Version $EVOPLEX_VERSION package.xml
setXML Package ReleaseDate $TODAY package.xml
setXML Package DisplayName "Evoplex $EVOPLEX_RELEASE" package.xml
cd ..

cd data
cp -r $ROOTDIR/build/bin/. .
cp -r $ROOTDIR/build/lib/ .
cd ..

cd $ROOTDIR/win-installer
binarycreator.exe --offline-only -c config/config.xml -p packages evoplex_$EVOPLEX_RELEASE.exe
