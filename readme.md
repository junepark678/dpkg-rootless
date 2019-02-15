modified dpkg for iOS - rootlessJB
----------------------------------

Allows universal iOS debs to be installed and configured at once on rootlessJB,
making them meet all the requirements.

No need to patch, move files, alter, sign anything in the .deb beforehand.

Just use the same .deb files as you would in any other jailbreak (provided the code is compatible)


This version of dpkg for iOS parses the .deb file that is to be installed (dpkg -i) and:
 

1) collects information about mach-o files identifying them as MH_DYLIB or MH_EXECUTE
2) if its an MH_DYLIB, 	it signs it with ldid2 with empty entitlements and calls FIXUP_DYLIB on it
3) if its an MH_EXECUTE, it signs it with ldid2 with either the minimal entitlements required or by replacing its own to make sure they are correct.
4) collects .app executables found that is to be moved after uicache app installation, finds them in /var/containers/Bundle/Application after uicache, and injects them.
5) patches all files to replace wrong directory references with rootlessJB ones.
6) collects which daemons/processes are to be injected by the containing tweaks (by their .plist) and if its a restartable daemon, it restarts it so that FIXUP_DYLIB can have immediate effect.
7) calls uicache if an .app is found
8) places a ".jb" flag file inside jailbreak .app folder, so that it knows that it should be uninstalled later if its package is removed (by dpkg --remove)
8) when all files are in their correct place, it restarts previously found daemons again so that FIXUP_DYLIB /and/ TweakInject.dylib can have immediate effect.
9) Well, that's it. the .deb should work out of the box.


On dpkg --remove, 


1) it reads the package's file contents and finds *.app directories that were in the package list.
2) it finds any .app matching the package's containing apps in /var/containerss/Bundle/Application/* 
3) it checks for .jb file inside that directory
4) it reads the CFBundleIdentifier from the *.app/Info.plist and calls LaunchServices to uninstall the app as well.
5) it removes all package contents as usual

In other words, it's a complete install/uninstall tweaks method on rootlessJB.


Complile
--------

	./autogen
	export CPPFLAGS="-I/usr/local/opt/ncurses/include"; export CFLAGS="-arch arm64 -isysroot $(xcrun -sdk iphoneos --show-sdk-path)"; export AR=ar;  ./configure --disable-dselect --disable-start-stop-daemon --host=x86_64-apple-darwin --target=arm64-apple-darwin --prefix= --disable-devel-docs
	make clean; make;
	
	dpkg must be signed with lsdents.plist entitlements

by @limneos