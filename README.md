# DasherServer :keyboard: :page_facing_up:
This project features an experimental backend and a simple development frontend for the [DasherCore](https://github.com/VRGroupRWTH/DasherCore/).
The backend can be used separately and features a websocket interface to communicate with the DasherCore-library. The protocol that is used is defined in the [Specification](Specification.md).

## Building on Linux :penguin:
```shell
git clone https://github.com/VRGroupRWTH/DasherServer.git
cd DasherServer
mkdir build
cd build
cmake ..
make
```

## Building on Windows :window:
1. `git clone https://github.com/VRGroupRWTH/DasherServer.git`
2. Execute CMake Configure and Generate
3. Build with Visual Studio

## Execution :love_you_gesture:
1. Start resulting binary from the build process
2. Open the `index.html` file from the `Frontend`-folder