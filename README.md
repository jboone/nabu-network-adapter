# NABU Network Adapter

In 1983, the [NABU Network](https://en.wikipedia.org/wiki/NABU_Network) was among the first computer systems to distribute software and data over a high-speed cable television network, predating modern cable modem technology by about a decade. Unfortunately, NABU did not succeed as a business, and the NABU Network went dark in 1985.

The NABU system consisted of an 8-bit, Z80-based PC, a Network Adapter that interfaced the PC to the cable TV network, and the cable TV network infrastructure that put data onto the cable TV network. The network infrastructure seems to have largely disappeared when NABU went out of business. But there are many PCs and Network Adapters available today, and [retro-computing hobbyists](https://nabu.ca) have made great strides in making these computers useful again, using modern computer hardware to replace the Network Adapter. But so far, nobody has brought the Network Adapters back to life. They're just boat anchors. But if we knew enough about how they worked, could we simulate the cable TV signal and make a Network Adapter work again in 2024?

## Acknowledgements

The information in this repository is not mine alone, but a summary of collaboration with several people. Hats off especially to the folks in the NABU Discord server.

Here's some links to collaborators and their NABU Adapter learnings:

* [Phil Pemberton's NABU Adapter model NA-2 internals](https://philpem.me.uk/oldcomp/nabu/adaptor_internals)

## Topics

What do we need to learn about the NABU Network Adapter in order to interface to it and convince it the NABU Network is alive again?

* CATV frequency, bandwidth, power at the receiver
* Modulation: type, symbol rate, encoding, scrambling
* Data framing: synchronization patterns, headers, data encoding and compression, checksums

## Available Documentation

__TODO__: Things learned elsewhere, including likely modulation (OQPSK) and bit rate (6.3 Mbits/s) from the document "Cable Data Communications Systems 1983".

## Disassembly and Inspection

Most of the secrets we need to learn can be unearthed by analyzing and dissecting the Network Adapter hardware, which consists of:

* A radio-frequency (RF) shielded metal can that contains a cable TV tuner and the NABU Network data demodulator.
* A logic board that processes the demodulated data, synchronizes with and decodes the NABU Network data, extracts the desired software or data, and passes it on to the NABU PC.

A lot can be learned from looking up the circuit components, especially the integrated circuits, and noticing how they're laid out on the circuit board.

### RF Can

We know this RF can contains some sort of cable TV receiver. It likely implements a multiple-conversion receiver, which uses multiple stages of filtering, frequency shifting, and amplification to isolate and boost the signal for the demodulation stage. Looking inside, we find six compartments:

Two of the compartments are soldered shut, but when looking through the large adjustment holes, we can see there are large inductors, and there's no power terminals into these compoartments. A reasonable first guess is that these are passive filter stages.

The compartment closest to the cable TV connection has a few ICs, an 8 MHz crystal, and an [monolithic microwave integrated circuit](https://en.wikipedia.org/wiki/Monolithic_microwave_integrated_circuit) or "MMIC". The MC145155P is a frequency synthesizer, the CA3140E is an operational amplifier, and the SP4632 is a frequency prescaler. There's +5V and +18V power going into the compartment, along with three signals (EN, D, CLK) which likely tie to the three control signals of the MC145155P. Labels on the circuit board include "RF IN" and "IF FILTER" (with a wire that goes into the adjacent compartment). From my experience, this all suggests this is the first frequency-shifting stage. Using an [EMC probe](https://www.beehive-electronics.com/probes.html), I can detect a strong 325 MHz signal.

Skipping the two soldered-shut compartments, the fourth compartment contains two SL3127C high-frequency NPN transistor arrays and one MC1741C high-performance operational amplifier. The compartment only takes +18V power, and has one signal to/from the compartment. The only interesting labels is an "OUT" hole, which may be a test point.

The fifth compartment contains another SL3127C transistor array and another MC1741C op-amp, as well as a 14.270 MHz crystal. There's also a suspicious cluster of eight diodes, and four TO-92s labeled with "CR" and a number, suggesting they're actually diodes, not transistors. These diode clusters suggest mixers, which are used to perform frequency shifting. Other interesting labels on the circuit board are "IF INPUT", and two "DATA OUT" test points. The compartment takes +18V and has one other signal leaving the compartment, but not connected to anything on the outside.

The sixth and last compartment has far less analog circuitry. It contains two MC1414P dual differential comparators, two 74LS86 quad XOR logic gates, two 74LS74 dual D flip-flops, and one 74LS00 quad NAND gates. There's also a 3.1555 MHz crystal. It takes +18V and +5V, and has two signals leaving the compartment, labeled "DATA" and "CLK". There's a few test points, a couple of pads marked "X". On the back side of the circuit board, there are two capacitors wired from the "X" pads to the adjacent compartment. Given the data and clock signals leaving the compartment (which go to the logic board), it seems quite safe to assume this is some portion (maybe all of?) the demodulator.

__TODO__: Testing.

### Logic Board

__TODO__: Descrambler
__TODO__: Frame synchronizer(?)
__TODO__: PAL, PAL no. matching
__TODO__: FIFO + RAM
__TODO__: UART

Educated guesses about the 28-pin Motorola SC87253P on the logic board led many of us to conclude independently that it's likely a mask ROM MC6800, most likely the MC6805P2. The pinout matches the circuit board layout very well. Power, clock, and reset signals match, and collections of data signals match the ports on the MC6805P2.

## Firmware

@philpem extracted the firmware from the SC87253P, using a [well-documented attack](https://seanriddle.com/mc6805p2.html).

Loading the ROM file into [Ghidra](https://ghidra-sre.org/) was very satisfying. There are a few vectors located at the end of the ROM address range. I asked Ghidra to disassemble at each of the addresses in the table, and quickly had the entire ROM disassembled.

| Address | Name   | Function                  |
| ------- | ------ | ------------------------- |
|  0x07f6 | TWIRQ  | Timer wait vector         |
|  0x07f8 | TIRQ   | Timer internal vector     |
|  0x07fa | KEYSCN | External interrupt vector |
|  0x07fc | SWI    | Software interrupt vector |
|  0x07fe | RESET  | Reset vector              |

My attention immediately turned to code which manipulated PORTA. Ghidra helpfully lists out all the functions that read or write from PORTA, and I quickly identified a function that waits for a byte to be available at the UART, and then reads and returns it. Hey, this is fun!

__TODO__: What does the subscriber entitlement work? Is it the microprocessor? Seems unlikely to be the PC (security) or the PAL (complexity).

## Bitstream

__TODO__: How to build a cycle

## Modulation

__TODO__: GNU Radio flowgraph to transmit a cycle to a Network Adapter
