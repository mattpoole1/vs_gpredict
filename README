Gpredict for Venture Southland UHF Ground Station
====================================================================

This is a modified version of Gpredict tailored to suit the needs of Venture Southland UHF Ground Station

Author: Matthew Poole
Email: matt.poole1@gmail.com
Date: 10th April 2018

## Making Changes:
1. Make a new branch in which you can make and test your changes
`sudo git branch <your-branch>`
2. Change to your new branch
`sudo git checkout <your-branch>`
3. Now you can freely make changes and test them
4. To backup any local changes to your current branch
```
sudo git add .
sudo git commit -m "<Your message describing changes>"
sudo git push
```
5. Once you are happy your changes work as planned, merge your branch with master
```
sudo git checkout master
sudo git merge <your-branch>
```
6. If there are conflicts to resolve, use `git diff` to highlight these conflicts, then resolve them in a text editor. Once all conflicts have been resolved, push the edited files to the master branch.
```
sudo git add .
sudo git commit -m "Resolved conflicts by review"
sudo git push
```
7. Delete your branch as all changes are now in the master branch
`sudo git branch -d <your-branch>

Using the M2 RC2800 Antenna Positioner
======================================

## Initial Setup:
1. Launch gpredict (/usr/local/bin/gpredict)
2. Navigate to Edit>Preferences>Interfaces>Rotators>Add New
3. Set up the rotator as follows:
* ConfigName: M2AzEl
* Host: localhost
* Port: 4537
* Min Az: 0
* Max Az: 360
* Min El: 0
* Max El: 90
* Az Type: 0->180->360

## Before Using
1. Launch a terminal anywhere
2. Run the command `. rot.sh` to run the utility background programs.

## To use
1. Navigate into your desired Gpredict module
2. In the upper right hand corner of the window, click the drop down arrow and click Antenna Control
3. In the Settings window, select the M2AzEl Device
4. Gpredict should now be able to be used to operate the controller


Gpredict
=========

Gpredict is a real time satellite tracking and orbit prediction program
for the Linux desktop. It uses the SGP4/SDP4 propagation algorithms together
with NORAD two-line element sets (TLE).

Some core features of Gpredict include:

- Tracking of a large number of satellites only limited by the physical
  memory and processing power of the computer
- Display the tracking data in lists, maps, polar plots and any combination
  of these
- Have many modules open at the same either in a notebook or in their own
  windows. The modules can also run in full-screen mode
- You can use many ground stations
- Predict upcoming passes
- Gpredict can run in real-time, simulated real-time (fast forward and
  backward), and manual time control
- Detailed information both the real time and non-real time modes
- Doppler tuning of radios via Hamlib rigctld
- Antenna rotator control via Hamlib rotctld

Visit the gpredict homepage at http://gpredict.oz9aec.net/ for more info.


Requirements
------------

Gpredict is written using the Gtk+ 3 widget set, which is available for most
Unix like operating systems, Mac and Windows. Following libraries are required
for successful compilation of Gpredict:

- Gtk+ 3 or later
- GLib 2.32 or later
- GooCanvas 2
- Libcurl 7.16 or later
- Hamlib (runtime only, not required for build)

If you compile Gpredict from source you will also need the development packages
often with -dev or -devel in the package name, e.g. libgtk-3-dev. On Debian and
Ubuntu systems you can install the build dependencies using:

  sudo apt install libtool intltool autoconf automake libcurl4-openssl-dev
  sudo apt install pkg-config libglib2.0-dev libgtk-3-dev libgoocanvas-2.0-dev


To build and install gpredict from source, first unpack the source package:

  tar -xvf gpredict-x.y.z.tar.gz

Then change to the gpredict-x.y.z directory and build gpredict:

  ./configure
  make
  make install

The last step usually requires you to become root, otherwise you may not have
the required permissions to install gpredict. If you can not or do not want to
install gpredict as root, you can install gpredict into a custom directory by
adding --prefix=somedir to the ./configure step. For example

  ./configure --prefix=/home/user/predict
  
will configure the build to install the files into /home/user/gpredict folder.

If you are building directly from the git repository, you have to run
./autogen.sh instead of of configure. You can pass the same options to the
autogen.sh script as you would to the configure script.

If you want to know more about installation options refer to the INSTALL file
(not for beginners).


Using Gpredict
--------------

Gpredict comes bundled with some example data and will run "out of the box".
Once you got the idea of the UI, you can either modify the settings of the
default module (click on the small down-arrow in the top right corner), or
create a new module through File -> New Module.

You are highly encouraged to have a look at the user manual available at
http://gpredict.oz9aec.net/documents.php


User support
------------

User support is provided through the Gpredict forum hosted by the Libre Space
Foundation: https://community.libre.space/c/gpredict


Terms and conditions
--------------------

Gpredict is released under the GNU General Public License and comes with
NO WARRANTY whatsoever. If you have problems installing or using Gpredict,
feel free to ask for support for example on the Gpredict forum hosted by
the Libre Space Foundation https://community.libre.space/c/gpredict


Note that the development environment is being migrated from Sourceforge to
Github: https://github.com/csete/gpredict

Mailing lists, old bug trackers, etc, are still available on Sourceforge:
http://sourceforge.net/projects/gpredict


Happy Tracking!

Alexandru Csete
OZ9AEC
