# Learn RISC-V and Bare Metal Embedded Coding 

Mission statement: To understand FemtoRV (RISC-V) code and try to add peripherals and write bare metal drivers. 

![](FemtoRV/TUTORIALS/Images/IceStick_hello.gif)

UART Driver and Testing :: [8/25/26 - 9/31/26]
------------------------------------------------------------
This includes:
- Understanding FemtoRV CodeBase , and understanding most configs andh helper functions 
- Understanding UART address space and how to access through the RISC-V core 
- Since I had no access to FPGA Board , testing had to be done using **GTK Wave** and **iverilog** 

FemtoRV: a minimalistic RISC-V CPU
----------------------------------- 
[FemtoRV](FemtoRV/README.md) is a minimalistic RISC-V design, with
easy-to-read Verilog sources directly written from the RISC-V specification. 
The most elementary version (quark), an RV32I core, weights 400 lines of VERILOG
(documented version), and 100 lines if you remove the comments. There
are also more elaborate versions, the biggest one (petitbateau) is an RV32IMFC
core. The repository also includes a companion SoC, with
drivers for an UART, a led matrix, a small OLED display, SPI RAM and
SDCard. Its most basic configuration fits on the Lattice IceStick (<
1280 LUTs). It can be used for teaching processor design and RISC-V
programming.


