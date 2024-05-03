# Wkernel
Wkernel is a Windows kernel mode driver designed for system level access and manipulation.
This driver provides functionalities to interact with the kernel memory of the operating system, primarily aimed at assisting in game modification or cheat development scenarios
Also for reading and writing for developing cheats.

## Features
- **Kernel memory access:** wkernel allows reading from and writing to kernel memory, enabling developers to create cheats or game modifications that require low level access to system resources.

## Requirements
In order to utilize wkernel, it is necessary to have the Windows Developer Kit installed, which provides essential tools and libraries for driver development.

## Usage
- **Initialization:** The driver exposes IOCTLs for initialization and control. Developers can initialize the driver and perform necessary setup through IOCTL calls.
- **Kernel memory operations:** wkernel facilitates reading and writing kernel memory, allowing developers to inspect or modify system data structures as needed.

## Note
This driver is a work in progress and does not have a user-friendly interface. It is primarily intended for developers with knowledge of kernel programming and system internals.

## Caution
Due to the nature of kernel mode drivers and their potential impact on system stability and security, it is advisable to execute wkernel in a controlled environment, such as a virtual machine, to mitigate any adverse effects on the host system.

## Disclaimer
Usage of wkernel or any similar tool for unauthorized purposes, such as cheating in online games or circumventing system security measures, may violate terms of service agreements and lead to legal consequences. Developers are advised to use this tool responsibly and ethically.

## Debug with dbgview64
![image](https://github.com/WzrterFX/wkernel/assets/122642787/c2a79900-4a27-4bb4-bce6-2f5b67e8eeee)

Unsigned kernel driver with kdmapper or gdrv loader

https://github.com/fengjixuchui/gdrv-loader

https://github.com/TheCruZ/kdmapper
