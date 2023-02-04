#!/bin/bash

prefix=${PREFIX=/usr/local}
app_title="T3F_APP_TITLE"
app_name=T3F_APP_NAME
app_dir=T3F_APP_DIR
app_categories=T3F_APP_CATEGORIES

if [ $EUID != 0 ]
then
  echo "Installation of this application requires superuser priveleges. If you are not "
  echo "already running this script as a superuser you may be asked to authenticate."
  echo ""
  sudo $0
else
  if which bin/$app_name &>/dev/null
  then
    echo "Installing T3F_APP_TITLE for all users..."
    mkdir -p $prefix/share/$app_name
    mkdir -p $prefix/share/doc
    mkdir -p $prefix/share/doc/$app_name
    mkdir -p $prefix/share/icons
    mkdir -p $prefix/share/applications
    mkdir -p $prefix/share/menu
    cp -a bin/data $prefix/share/$app_name
    cp docs/changelog $prefix/share/doc/$app_name/changelog || true
    cp docs/README $prefix/share/doc/$app_name/README || true
    cp docs/copyright $prefix/share/doc/$app_name/copyright || true
    cp icons/icon.svg $prefix/share/icons/$app_name.svg || true
    cp icons/icon.png $prefix/share/icons/$app_name.png || true
    cp bin/$app_name $prefix/$app_dir/$app_name
    printf "[Desktop Entry]\nName="$app_title"\nExec=$prefix/$app_dir/$app_name %%F\nIcon=$app_name\nTerminal=false\nType=Application\nCategories=$app_categories;" > $prefix/share/applications/$app_name.desktop
    chmod 755 $prefix/$app_dir/$app_name
    find $prefix/share/$app_name -type f -exec chmod 644 {} \;
    chmod 644 $prefix/share/doc/$app_name/README || true
    chmod 644 $prefix/share/doc/$app_name/changelog || true
    chmod 644 $prefix/share/doc/$app_name/copyright || true
    chmod 644 $prefix/share/icons/$app_name.svg || true
    chmod 644 $prefix/share/icons/$app_name.png || true
    chmod 644 $prefix/share/applications/$app_name.desktop
    update-desktop-database
  else
    echo "Application executable binary missing. Installation cancelled."
    echo ""
  fi
fi
