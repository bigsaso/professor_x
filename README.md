# Raspberry Pi Setup and C++ Project Configuration

This README provides step-by-step instructions for setting up a Raspberry Pi and configuring the C++ project for the eeg wheelchair.

## Table of Contents

- [Raspberry Pi Setup](#raspberry-pi-setup)
  - [Installing Raspberry Pi OS](#installing-raspberry-pi-os)
  - [Connecting to Your Pi via SSH](#connecting-to-your-pi-via-ssh)
  - [Enabling and Connecting over VNC](#enabling-and-connecting-over-vnc)
- [Installing C++ and Setting Up CMake](#installing-c-and-setting-up-cmake)
- [Installing FFTW Library and Configuring for C++ Project](#installing-fftw-library-and-configuring-for-c-project)
- [References](#references)

---

## Raspberry Pi Setup

### Installing Raspberry Pi OS

1. Insert an 8GB or larger microSD card into your computer.
2. Download, install, and run Raspberry Pi Imager.
3. Click "Choose OS" and select Raspberry Pi OS (32-bit or 64-bit based on your Pi model).
4. Click "Choose Storage" and select your microSD card.
5. Access the settings menu (CTRL + Shift + X) and enable SSH. Set a hostname, username, and password.
6. Enter your Wi-Fi network's SSID, password, and country code.
7. Click "Write" to write the OS to the card.

### Connecting to Your Pi via SSH

1. Download and install Putty, an SSH client for Windows.
2. Enter the hostname you selected in step 5(a) above in Putty and click "Open."
3. Accept any security warnings and enter your username and password to connect.

### Enabling and Connecting over VNC

1. After SSH connection, enter `sudo raspi-config` at the command prompt.
2. Select "Interfacing Options" and then choose "VNC."
3. Confirm by selecting "Yes" to enable the VNC server.
4. On your PC, download, install, and launch VNC Viewer.
5. Create a new connection and enter the hostname you selected in step 5(a).
6. Connect and enter the Pi's username and password when prompted.

---

## Installing C++ and Setting Up CMake

### C++

- C/C++ are included in Raspberry Pi OS, and some common libraries (like GPIO and camera) are pre-installed as well.

### CMake

1. Install essential build tools and libraries:
$ sudo apt-get install build-essential libssl-dev

2. Download and install CMake:<br />
$ cd /tmp<br />
$ wget https://github.com/Kitware/CMake/releases/download/v3.20.0/cmake-3.20.0.tar.gz<br />
$ tar -zxvf cmake-3.20.0.tar.gz<br />
$ cd cmake-3.20.0<br />
$ ./bootstrap<br />
$ make (or gmake on RPi)<br />
$ sudo make install

3. Verify the installation:<br />
$ cmake --version

---

## Installing FFTW Library and Configuring for C++ Project

### Download and install FFTW Library

1. Download FFTW Library:<br />
$ wget https://www.fftw.org/fftw-3.3.10.tar.gz<br />
$ tar xf fftw-3.3.10.tar.gz<br />
$ cd fftw-3.3.10

2. Configure and install:<br />
$ ./configure<br />
$ make<br />
$ sudo make install

### Set-up to use in our C++ project

1. Set the correct FFTW directory in the CMakeLists.txt file.<br />
2. Compile your C++ source file:<br />
$ cmake .<br />
$ make
3. Run the code<br />
$ ./post_process

---

## References

[1] [How to set up a headless Raspberry Pi, without ever attaching a monitor](https://www.tomshardware.com/reviews/raspberry-pi-headless-setup-how-to,6028.html) - A. Piltch, Tom’s Hardware, accessed Oct. 22, 2023.

