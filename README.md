# Telemetry transmitter

Telemetry transmitter module is responsible to deliver CAN trafic from inside competition vehicle to a remote server. Data transmission is done over 802.15.4 protocol.

During BWSC 2023, a RF xBee was installed on both chase and competition vehicles. In direct line of sight, communication could be sustained for upto 150 meters.

During iLumen 2024, a single LTE/NB-IoT xBee was installed on competition vehicles. It transmitted data over 4G network to a stationary strategy unit.

## Requirements

This project is configured for STM32F103RB target (arch=arm32). To install ARM32 toolchain follow instructions for your specific OS:

_NOTE 1:_ Commands that start with `$` should be executed from the CLI.

_NOTE 2:_ WSL2 distors are also supported.

### Linux

Install [ARM GNU toolchain](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads). At the moment of writing this guide 14.2.Rel1 is latest version of the toolchain. Below there is an example on how to download and install x86_64 Linux hosted cross toolchain:

```
$ cd /usr/local/src
$ sudo wget https://developer.arm.com/-/media/Files/downloads/gnu/14.2.rel1/binrel/arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi.tar.xz https://developer.arm.com/-/media/Files/downloads/gnu/14.2.rel1/binrel/arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi.tar.xz.asc https://developer.arm.com/-/media/Files/downloads/gnu/14.2.rel1/binrel/arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi.tar.xz.sha256asc
```

_NOTE 1:_ Link in `wget` might be dead, so check the ARM GNU toolchain download page.

It is a good idea to verify checksums:

```
$ md5sum -c arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi.tar.xz.asc
$ sha256sum -c arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi.tar.xz.sha256asc
```

If checksums match you can safely continue:

```
$ sudo tar -xvf arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi.tar.xz
$ sudo rm *.tar.xz*
$ cd arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi/bin
$ find . -type f -executable | while read bin; do
  > sudo ln -s $(pwd)/$bin /usr/local/bin/
  > done
$ sudo ln -s /usr/local/src/arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi/arm-none-eabi/include/ /usr/local/include/arm-none-eabi
```

Install additional requirements:

```
$ sudo apt update
$ sudo apt install -y cmake git libusb-1.0 ninja-build openocd stlink-tools
```

### Windows

1. Install [ARM GNU toolchain](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads). At the moment of writing this guide 14.2.Rel1 is latest version of the toolchain.
2. Install [Git](https://git-scm.com/downloads/win).
3. Install [CMake](https://cmake.org/download/).
4. Install [Ninja](https://github.com/ninja-build/ninja/releases).
5. Install [OpenOCD](https://github.com/xpack-dev-tools/openocd-xpack/releases).
6. Install [STM32 ST-LINK utility](https://www.st.com/en/development-tools/stsw-link004.html)

_Note 1:_ Make sure all programs' executables are added to `Environment Variables > System variables > Path`.

### Mac (WIP)

Install system requirements:

```
#TODO
```

## Project configuration

First, create an SSH key:

```
$ ssh-keygen -t rsa -b 4096
```

Follow all default steps for creating the key and do not generate passcode for your key. Afterwards, extract public key:

```
$ cat ~/.ssh/id_rsa.pub
```

Copy public key and paste it into your GitHub account > Settings > SSH and GPG keys. Afterwards, configure your git config with your GitHub name and email.

```
$ git config --global user.name "Joe Doe" 
$ git config --global user.email "joedoe@email.com" 
```

Finally, clone the repo:

```
$ cd ~
$ git clone --recurse-submodules --shallow-submodules git@github.com:SolisEV-UTCN/telemetry_tx.git telemetry_tx
$ cd telemetry_tx
```

## Building

Substitute options from commands in the later steps with one the values below:

_NOTE 1:_ Default values are valid only for Docker and script builds.

| `<BUILD_TYPE>`   | Purpose                             |
| ---------------- | ----------------------------------- |
| Release          | Final, optimized version.           |
| Debug  (default) | Development and debugging.          |
| MinSizeRel       | Optimized for minimal binary size.  |
| RelWithDebInfo   | Optimized build with debug symbols. |

| `<TARGET>`            |
| --------------------- |
| stm32f103rb (default) |

### Using Docker

Make sure you have a docker installed and running. Docker will mount a local volume to output generated files.

```
$ docker build . -t stm32 -f dev/dockerfile
$ docker run -v $(pwd)/build:/build -e BUILD_TYPE=<BUILD_TYPE> -e TARGET=<TARGET> stm32
```

### Using script

```
$ scripts/build.sh -b <BUILD_TYPE> -t <TARGET>
```

### Using CMake

After modifying `CMakeLists.txt` file, regenerate CMake configuration:

```
$ rm -Rf build/<BUILD_TYPE>
$ mkdir -p build/<BUILD_TYPE>
$ cmake -GNinja -DCMAKE_TOOLCHAIN_FILE=dev/cmake/<TARGET>.cmake -DCMAKE_BUILD_TYPE=<BUILD_TYPE> -S . -B build/<BUILD_TYPE>
```

To build project run following command:

```
$ cmake --build build/<BUILD_TYPE> --clean-first
```

## Flashing

__DO NOT DISCONNECT YOUR DEVICE FROM POWER WHILE FLASHING__

Connect target with a ST-Link/JTAG connector. Check if device is recognized by your system. `$ sudo st-link --probe`, this will generate information about your device. For STM32F103 Nucleo board I get following output:
```
Found 1 stlink programmers
  version:    V2J33S25
  serial:     066CFF545589564867125526
  flash:      131072 (pagesize: 1024)
  sram:       20480
  chipid:     0x410
  dev-type:   STM32F1xx_MD
```

Finally, flash target `.bin` file onto FLASH memory.

`$ sudo st-flash --reset write build/Debug/telemetry_tx.bin 0x08000000`

_Note 1:_ If your device-type is unknown or you can't connect to SWD check ST-Link jumpers on the board. Also, make sure JP5 jumper is set on U5V.

_NOTE 2:_ If using WSL2, you need to pass USB device from host computer. For Windows you can use [usbipd](https://github.com/dorssel/usbipd-win). Run as administrator following PowerShell commands:

`> usbipd list` this will output all connected devices. I got following output:

```
Connected:
BUSID  VID:PID    DEVICE                                                        STATE
1-3    0483:374b  ST-Link Debug, USB Mass Storage Device, STMicroelectronic...  Not shared
1-14   8087:0026  Intel(R) Wireless Bluetooth(R)                                Not shared
2-1    17ef:609b  USB Input Device                                              Not shared
2-3    0d8c:013c  USB PnP Sound Device, USB Input Device                        Not shared
```

ST-Link on my computer was mounted with bus ID of 1-3. Next you need to make this bus shared, to do so run following command:

`> usbipd bind -b 1-3`

To verify if binding was succesfull you can rerun list command. Device should be now shared, see example below:

```
Connected:
BUSID  VID:PID    DEVICE                                                        STATE
1-3    0483:374b  ST-Link Debug, USB Mass Storage Device, STMicroelectronic...  Shared
1-14   8087:0026  Intel(R) Wireless Bluetooth(R)                                Not shared
2-1    17ef:609b  USB Input Device                                              Not shared
2-3    0d8c:013c  USB PnP Sound Device, USB Input Device                        Not shared
```

Afterwards, you can attach this device to your WSL2 instance:

`> usbipd attach -w -b 1-3`

## Debugging

Before debugging, you need to add your USB device to OpenOCD permission group. On WSL, you have to enable `systemd` in boot config. Add following lines to `/etc/wsl.conf`.

```
[boot]
systemd = true
```

Afterwards, restart your WSL from PowerShell:

```
> wsl --shutdown
> wsl
```

### Using OpenOCD & GDB

Debugging is done via OpenOCD and arm-none-eabi-gdb. For more information on how to debug with GDB you can read this [summary](https://jacobmossberg.se/posts/2017/01/17/use-gdb-on-arm-assembly-program.html).

1. Open OpenOCD: `$ openocd -f /usr/share/openocd/scripts/interface/stlink.cfg -f /usr/share/openocd/scripts/target/stm32f1x.cfg`
2. Start GDB with target ELF file: `$ arm-none-eabi-gdb -x scripts/default_dirs.gdb build/<BUILD_TYPE>/<TARGET>/telemetry_tx.elf`
3. Connect to target: `(gdb) target extended-remote localhost:3333`

_Note 1:_ Commands that start with `(gdb)` should be executed from the GDB terminal.

### Using Visual Studio Code

1. Run either `STM32F103: Debug` or `STM32F103: RelWithDebInfo` launch options.
