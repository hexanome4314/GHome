GHOME
=====
Green Home is a 4IF students project proposed for the first time by our INSA Lyon teacher-researcher.
W'll try to make it real and fonctionnal.

Who is it for?
--------------
Anyone with a server at home and who can afford buying SunSpot and EnOcean sensors. 
So few dandy now, but hopefully this will be the kind of stuff we'll see in every house in the 10-20 coming years.

Folders
-------

config/  : xml files for configuration
drivers/ : .so files of compiled drivers
include/ : global c includes files
lib/     : compiled .a lib files
log/     : execution log files
src/     : sources

Build
-----

In order to build, it is required to have libxml2, dl lib and phtread.
The project can be compiled with make. The executable will be created
in the root folder.

Web server
----------

You must have a configured web server linking to src/www
