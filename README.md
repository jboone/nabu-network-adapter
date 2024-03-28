# NABU Network Adapter

In 1983, the [NABU Network](https://en.wikipedia.org/wiki/NABU_Network) was among the first computer systems to distribute software and data over a high-speed cable television network, predating modern cable modem technology by about a decade. Unfortunately, NABU did not succeed as a business, and the NABU Network went dark in 1985.

The NABU system consisted of an 8-bit, Z80-based PC, a Network Adapter that interfaced the PC to the cable TV network, and the cable TV network infrastructure that put data onto the cable TV network. The network infrastructure seems to have largely disappeared when NABU went out of business.

There are many NABU PCs still in use today, thanks to great strides made by [retro-computing hobbyists](https://nabu.ca) in applying modern computer hardware to replace the Network Adapter and the defunct cable infrastructure it relies on. But so far, nobody has brought the Network Adapters back to life. They're just boat anchors. But if we knew enough about how they worked, could we simulate the cable TV signal and make a Network Adapter work again in 2024?

## Acknowledgements

The information in this repository is not mine alone, but a summary of collaboration with several people. Hats off especially to the folks in the NABU Discord server.

Here are links to collaborators and their NABU Network Adapter learnings:

* [Phil Pemberton's NABU Adapter model NA-2 internals](https://philpem.me.uk/oldcomp/nabu/adaptor_internals)

## Topics

What do we need to learn about the NABU Network Adapter in order to interface to it and convince it the NABU Network is alive again?

* CATV frequency, bandwidth, power at the receiver
* Modulation: type, symbol rate, encoding, scrambling
* Data framing: synchronization patterns, headers, data encoding and compression, checksums

## Available Documentation

__TODO__: Things learned elsewhere, including likely modulation ([OQPSK](https://en.wikipedia.org/wiki/Phase-shift_keying#Offset_QPSK_(OQPSK))) and bit rate (6.3 Mbits/s) from the document "[Cable Data Communications Systems, 1983](https://publications.gc.ca/site/eng/9.879204/publication.html)".

## Disassembly and Inspection

Most of the secrets we need to learn can be unearthed by analyzing and dissecting the Network Adapter hardware, which consists of:

* A radio-frequency (RF) shielded metal can that contains a cable TV tuner and the NABU Network data demodulator.
* A logic board that processes the demodulated data, synchronizes with and decodes the NABU Network data, extracts the desired software or data, and passes it on to the NABU PC.

A lot can be learned from looking up the circuit components, especially the integrated circuits, and noticing how they're laid out on the circuit board.

##@ Power Supply

This is a run of the mill linear power supply. There are two THOMSON-CSF regulators, an SFC2805RC for the +5V rail, and an SFC2818RC for the +18V rail. Both regulators are in TO-3 packages mounted to the rear of the Network Adapter chassis, with external heat sinks.

### RF Can

We know this RF can contains some sort of cable TV receiver. It likely implements a multiple-conversion receiver, which uses one or more stages of filtering, frequency shifting, and amplification to isolate and boost the desired signal for the demodulation stage.

It has an F connector that goes to the back of the Network Adapter, and is where the cable TV network is attached. There are three wires running directly to the power supply, and a 9-pin connector that goes to the logic board.

Looking inside, we find six compartments:

#### Compartment 1

The compartment closest to the cable TV connection has a few integrated circuits, an 8 MHz crystal, and a [monolithic microwave integrated circuit](https://en.wikipedia.org/wiki/Monolithic_microwave_integrated_circuit) or "MMIC". The MC145155P is a frequency synthesizer, the CA3140E is an operational amplifier, and the SP4632 is a frequency prescaler. There's +5V and +18V power going into the compartment, along with three signals (labeled "EN", "D", and "CLK") which certainly tie to the three control signals of the MC145155P frequency synthesizer. Other labels on the circuit board include "RF IN" and "IF FILTER" (with a wire that goes into the adjacent compartment). From my experience, this all suggests this is the first frequency-shifting stage. Using an [EMC probe](https://www.beehive-electronics.com/probes.html), I can detect a strong 324 MHz signal. Logic analysis of the MC145155 configuration pins  show a single transaction right at power-on, setting SW1=0, SW2=0, and N=324. Voltmeter testing of the MC145155 RA pins shows RA\[2:0\]=001, which sets the R divider to 512. Given the 8 MHz crystal, the R counter output must be 8 MHz / 512 = 15.625 kHz. Multiplying back through the N counter (configured to divide by 324), we get a frequency input of 15.625 kHz * 324 = 5.0625 MHz. It's a safe assumption that the mixing local oscillator in this compartment is being divided by the SP4632 divide-by-64 prescaler before it goes into the PLL, which gives a mixing local oscillator frequency of 5.0625 MHz * 64 = 324 MHz. And that matches the dominant frequency spike I picked with my EMI probe! I love it when a plan comes together.

#### Compartments 2 and 3

These two compartments are soldered shut. When looking through the large adjustment holes, we can see there are coils of wire (inductors), and there's no power terminals into these compoartments. A reasonable first guess is that these are passive filter stages.

#### Compartment 4

This compartment contains two SL3127C high-frequency NPN transistor arrays and one MC1741C high-performance operational amplifier. The compartment only takes +18V power, and has one signal to/from the compartment. The only interesting labels is an "OUT" hole, which may be a test point. This compartment is a bit of mystery. My speculation is that it's an amplification stage, as there's few components (capacitors and inductors) I would associate with filtering, and no crystal-based clock source or clock signals coming from other compartments.

__TODO__: Determine if there's some form of AGC at work here.

__TODO__: Determine behavior of unused signal available on the terminal outside the compartment.

#### Compartment 5

This compartment contains another SL3127C transistor array and another MC1741C op-amp, as well as a 14.2470 MHz crystal. There's also a suspicious cluster of eight diodes, and four TO-92s labeled with "CR" and a number, suggesting they're actually diodes, not transistors. These diode clusters suggest mixers, which are used to perform frequency shifting. Other interesting labels on the circuit board are "IF INPUT", and two "DATA OUT" test points. The compartment takes +18V and has one other signal leaving the compartment, but not connected to anything on the outside.

Probing around the diode cluster reveals that there's a constant frequency from a local oscillator being applied to the "bottom" leads of the diodes, while the "top" leads seem to have the cable input signal. Adjusting the cable input signal up and down in frequency changes the signal present on the "top" leads, suggesting that they're performing a mixing (frequency-shifting) role.

__TODO__: Conclude that this is a quadrature demodulator?

__TODO__: Demonstrate that the diodes are mixing in quadrature. Look at the LO side and the input signal side and note quadrature...?

__TODO__: Observe "DATA OUT" signals at the same time, note quadrature...?

__TODO__: Determine the 14.2470 MHz crystal's function. Is the LO in this compartment actually 4 x 14.2470 = 56.9880 MHz? Is there a filter picking out a harmonic, or some other form of double clock-doubling?

__TODO__: Adjust your GNU Radio project to target 267.012 MHz, or 266.998 MHz?

#### Compartment 6

This last compartment has far less analog circuitry. It contains two MC1414P dual differential comparators, two 74LS86 quad XOR logic gates, two 74LS74 dual D flip-flops, and one 74LS00 quad NAND gates. There's also a 3.1555 MHz crystal. It takes +18V and +5V, and has two signals leaving the compartment, labeled "DATA" and "CLK". There's a few test points, a couple of pads marked "X". On the back side of the circuit board, there are two capacitors wired from the "X" pads to the adjacent compartment. Given the data and clock signals leaving the compartment (which go to the logic board), it seems quite safe to assume this is some portion (maybe all of?) the demodulator.

Looking at the schematic, we see the comparators slicing the quadrature signals from quadrature demodulator in compartment 5. This turns the smooth analog voltages into squared-up logic voltage signals for the logic ICs in this compartment.

The D flip flops are used to capture and delay the two quadrature signals. Exclusive OR logic gates compare the signals with their delayed version, outputting a "1" when the signal changes, and a "0" when it doesn't. This differential encoding simplifies the demodulator significantly, since it's the changes that represent the data, not the absolute polarity of the quadrature signals. In fact, when watching demodulator signals with the logic analyzer, you can see that the data in each of the quadrature channels swaps, or that the channels invert. So the encoding scheme for the modulation must take this in to account.

The quadrature signals are captured at different times, using two clocks that are 180 degrees out of phase with each other (in other words, "inverted"). This validates the claim that the NABU Network using *Offset* QPSK, where one of the two quadrature channels' modulations is delayed by a half a symbol time, to avoid complications that arise in QPSK.

__TODO__: Trace out the oscillator a bit more. There might be some sort of PLL in here, involving the U4 XORs, maybe pushing and pulling the oscillator crystal a bit to track the recovered symbol rate?

__TODO__: Testing.

### Logic Board

__TODO__: How did we identify the functional blocks? How did we hypothesize what they did? What do each of the fumctional blocks accomplish -- why are they there?

Connector J3 comes from the RF can.

| Pin | Function |
| --- | -------- |
|   1 | Ground   |
|   2 | PLL ENB input, active high latch enable |
|   3 | Received signal strength indication (RSSI)? |
|   4 | PLL CLK input |
|   5 | PLL DATA input, captured on clock rising edge  |
|   6 | Ground    |
|   7 | Demodulator clock output |
|   8 | Ground    |
|   9 | Demodulator data output, captured on clock rising edge |

The RSSI signal is at a high voltage when no signal is present, or signal is very low signal strength. RSSI voltage drops to near 0V when signal is strong.

The descrambler appears to be a [multiplicative descrambler](https://en.wikipedia.org/wiki/Scrambler#Multiplicative_(self-synchronizing)_scramblers), which is self-synchronizing. We can rule out an additive descrambler because there's no visible means in the schematic to initialize the scrambler's state. The polynomial is defined by which taps (flip-flop outputs) are being XORed together. The schematic shows taps at U20.Q0, U20.Q3, and U13.Q4. Rewritten as a polynomial equatiom, that'd be x^20 + x^3 + x^0. @philpem identified this polynomial as being used in [IESS-308](https://www.intelsat.com/wp-content/uploads/2020/08/IESS-308E11.pdf) and [ITU V.35](https://www.itu.int/rec/T-REC-V.35-198410-W/en).

__TODO__: Frame synchronizer(?)

__TODO__: PAL, PAL no. matching

__TODO__: FIFO + RAM

__TODO__: UART

Educated guesses about the 28-pin Motorola SC87253P on the logic board led many of us to conclude independently that it's likely a mask ROM [MC6805](http://bitsavers.trailing-edge.com/components/motorola/6805/6805_Users_Manual_2ed_1983.pdf), most likely the MC6805P2. The pinout matches the circuit board layout very well. Power, clock, and reset signals match, and collections of data signals match the ports on the MC6805P2.

## Firmware

@philpem extracted the firmware from the SC87253P mask ROM, using a [well-documented attack](https://seanriddle.com/mc6805p2.html). __TODO__: Read up on how it works, and describe and/or demonstrate

Loading the ROM file into [Ghidra](https://ghidra-sre.org/) was very satisfying. There are a few vectors located at the end of the ROM address range. I asked Ghidra to disassemble at each of the addresses in the table, and quickly had the entire ROM disassembled.

| Address | Name   | Function                  |
| ------- | ------ | ------------------------- |
|  0x07f6 | TWIRQ  | Timer wait vector         |
|  0x07f8 | TIRQ   | Timer internal vector     |
|  0x07fa | KEYSCN | External interrupt vector |
|  0x07fc | SWI    | Software interrupt vector |
|  0x07fe | RESET  | Reset vector              |

My attention immediately turned to code which manipulated data through PORTA using control signals on PORTB and PORTC. Ghidra helpfully lists out all the functions that read or write from PORTA, and I quickly identified a function that waits for a byte to be available at the UART, and then reads and returns it. Hey, this is fun!

__TODO__: What does the subscriber entitlement work? Is it the microprocessor? Seems unlikely to be the PC (security) or the PAL (complexity).

## Bitstream

__TODO__: How to build a cycle

## Modulation

__TODO__: GNU Radio flowgraph to transmit a cycle to a Network Adapter

## License

The contents of this repository, when not covered by other copyrights, are licensed as follows:

* Software, firmware, data: [CC0 1.0](https://creativecommons.org/public-domain/cc0/) (Creative Commons Zero, version 1.0)
* Hardware schematics, board designs: [CERN-OHL-P-2.0](https://ohwr.org/project/cernohl/wikis/Documents/CERN-OHL-version-2) (CERN Open Hardware License (OHL), version 2, permissive)

