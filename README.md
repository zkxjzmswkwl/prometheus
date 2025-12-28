# Prometheus

Hey and welcome, thanks for stopping by!

This will get appended to, soon-ish.

# Usage

Download the 0.8 beta from https://archive.org/details/overwatch-beta-0-8-0-24919 and put it somewhere. Compile the patcher and patch the executable. Compile prometheus, rename it to inject.dll and move it into the directory with GameClientApp.exe. Also move all the dll dependencies of prometheus.dll into the directory. Enjoy.

## NOTE

Since this was hastily refactored and some types were pasted into this project which i havent checked for errors yet, some stuff may be broken which I havent noticed yet. This will get fixed in the following days.

Also dont look into the window manager. Its an abomination. You have been warned.

# Open Source libraries used (TODO)

keystone
capstone
imgui
imnodes
pe
lazy_importer
nlohmann_json
ixwebsocket
freetype