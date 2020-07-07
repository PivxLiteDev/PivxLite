
Debian
====================
This directory contains files used to package pivxld/pivxl-qt
for Debian-based Linux systems. If you compile pivxld/pivxl-qt yourself, there are some useful files here.

## pivxl: URI support ##


pivxl-qt.desktop  (Gnome / Open Desktop)
To install:

	sudo desktop-file-install pivxl-qt.desktop
	sudo update-desktop-database

If you build yourself, you will either need to modify the paths in
the .desktop file or copy or symlink your pivxl-qt binary to `/usr/bin`
and the `../../share/pixmaps/pivx128.png` to `/usr/share/pixmaps`

pivxl-qt.protocol (KDE)

