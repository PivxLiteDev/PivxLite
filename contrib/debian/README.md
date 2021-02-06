
Debian
====================
This directory contains files used to package pivxlld/pivxl-qt
for Debian-based Linux systems. If you compile pivxlld/pivxl-qt yourself, there are some useful files here.

## pivxl: URI support ##


pivxl-qt.desktop  (Gnome / Open Desktop)
To install:

	sudo desktop-file-install pivxl-qt.desktop
	sudo update-desktop-database

If you build yourself, you will either need to modify the paths in
the .desktop file or copy or symlink your pivxl-qt binary to `/usr/bin`
and the `../../share/pixmaps/pivxl128.png` to `/usr/share/pixmaps`

pivxl-qt.protocol (KDE)

