//////////////////////////////////////////////////////////////////////////
//  pci.h
//////////////////////////////////////////////////////////////////////////

#ifndef FYSOS__PCI
#define FYSOS__PCI

#pragma pack(push, 1)

#define  PCI_ADDR     0x0CF8
#define  PCI_DATA     0x0CFC

#define  PCI_MAX_BUS      64  // PCI specs say 256 (we go to 64)
#define  PCI_MAX_DEV      32  // limit
#define  PCI_MAX_FUNC      8  // limit

struct PCI_POS {
  bit8u  bus;
  bit8u  dev;
  bit8u  func;
};

struct PCI_DEV {
  bit16u vendorid;         // Vendor ID = FFFFh if not 'a valid entry'
  bit16u deviceid;         // Device ID
  bit16u command;          // Command
  bit16u status;           // Status
  bit8u  revid;            // Revision ID
  bit8u  p_interface;      //
  bit8u  sub_class;        // 
  bit8u  _class;           //
  bit8u  cachelinesz;      // Cache Line Size
  bit8u  latencytmr;       // Latency Timer
  bit8u  hdrtype;          // Header Type (0,1,2, if bit 7 set, multifunction device)
  bit8u  selftest;
  bit32u base0;            // This one for OHCI, EHCI, and xHCI
  bit32u base1;            //
  bit32u base2;            //
  bit32u base3;            //
  bit32u base4;            // This one for UHCI
  bit32u base5;            //
  bit32u cardbus;          //
  bit16u subsysvid;        //
  bit16u subsysid;         //
  bit32u rombase;          //
  bit8u  capsoff;          //
  bit8u  resv0[3];         //
  bit32u resv1;            //
  bit8u  irq;              //
  bit8u  intpin;           //
  bit8u  min_time;         //
  bit8u  max_time;         //
  bit32u varies[48];       // varies by device.
};

#pragma pack(pop)

// in port byte (8-bit)
__inline__ bit8u inpb(const bit16u port) {
  bit8u rv;
  __asm__ __volatile__ ("inb %1, %0"
	  : "=a" (rv)
    : "dN" (port));
  return rv;
}

// out port byte (8-bit)
__inline__ void outpb(const bit16u port, const bit8u data) {
  __asm__ __volatile__ ("outb %1, %0"
	  :
    : "dN" (port),
      "a" (data));
}

// in port word (16-bit)
__inline__ bit16u inpw(const bit16u port) {
  bit16u rv;
  __asm__ __volatile__ ("inw %1, %0"
	  : "=a" (rv)
    : "dN" (port));
  return rv;
}

// out port word (16-bit)
__inline__ void outpw(const bit16u port, const bit16u data) {
  __asm__ __volatile__ ("outw %1, %0"
	  :
    : "dN" (port),
      "a" (data));
}

// in port double (32-bit)
__inline__ bit32u inpd(const bit16u port) {
  bit32u rv;
  __asm__ __volatile__ ("inl %1, %0"
	  : "=a" (rv)
    : "dN" (port));
  return rv;
}

// out port double (32-bit)
__inline__ void outpd(const bit16u port, const bit32u data) {
  __asm__ __volatile__ ("outl %1, %0"
	  :
    : "dN" (port),
      "a" (data));
}

// port is the byte offset from 0x00
bit8u pci_read_byte(const int bus, const int dev, const int func, const int port) {
  const bit32u val = 0x80000000 |
    (bus << 16) |
    (dev << 11) |
    (func << 8) |
    (port & 0xFC);
  outpd(PCI_ADDR, val);
  return inpb(PCI_DATA + (port & 3));
}

// port is the byte offset from 0x00
bit16u pci_read_word(const int bus, const int dev, const int func, const int port) {
  if ((port & 3) < 3) {
    const bit32u val = 0x80000000 |
      (bus << 16) |
      (dev << 11) |
      (func << 8) |
      (port & 0xFC);
    outpd(PCI_ADDR, val);
    return inpw(PCI_DATA + (port & 0x3));
  } else
    return (pci_read_byte(bus, dev, func, port + 1) << 8) | pci_read_byte(bus, dev, func, port);
}

// port is the byte offset from 0x00
bit32u pci_read_dword(const int bus, const int dev, const int func, const int port) {
  if ((port & 3) == 0) {
    const bit32u val = 0x80000000 |
      (bus << 16) |
      (dev << 11) |
      (func << 8) |
      (port & 0xFC);
    outpd(PCI_ADDR, val);
    return inpd(PCI_DATA);
  } else
    return (pci_read_word(bus, dev, func, port + 2) << 16) | pci_read_word(bus, dev, func, port);
}

// port is the byte offset from 0x00
void pci_write_byte(const int bus, const int dev, const int func, const int port, const bit8u value) {
  const bit32u val = 0x80000000 |
    (bus << 16) |
    (dev << 11) |
    (func << 8) |
    (port & 0xFC);
  outpd(PCI_ADDR, val);
  outpb(PCI_DATA + (port & 3), value);
}

// port is the byte offset from 0x00
void pci_write_word(const int bus, const int dev, const int func, const int port, const bit16u value) {
  if ((port & 3) < 3) {
    const bit32u val = 0x80000000 |
      (bus << 16) |
      (dev << 11) |
      (func << 8) |
      (port & 0xFC);
    outpd(PCI_ADDR, val);
    outpw(PCI_DATA + (port & 0x3), value);
  } else {
    pci_write_byte(bus, dev, func, port + 0, (bit8u) (value & 0xFF));
    pci_write_byte(bus, dev, func, port + 1, (bit8u) (value >> 8));
  }
}

// port is the byte offset from 0x00
void pci_write_dword(const int bus, const int dev, const int func, const int port, const bit32u value) {
  if ((port & 3) == 0) {
    const bit32u val = 0x80000000 |
      (bus << 16) |
      (dev << 11) |
      (func << 8) |
      (port & 0xFC);
    outpd(PCI_ADDR, val);
    outpd(PCI_DATA, value);
  } else {
    pci_write_word(bus, dev, func, port + 0, (bit16u) (value & 0xFFFF));
    pci_write_word(bus, dev, func, port + 2, (bit16u) (value >> 16));
  }
}

/* PCI specs v3.0, section 6.2.5.1 (page 227)
   Decode (I/O or memory) of a register is disabled via the command register before sizing a
   Base Address register. Software saves the original value of the Base Address register, writes
   0 FFFF FFFFh to the register, then reads it back. Size calculation can be done from the
   32-bit value read by first clearing encoding information bits (bit 0 for I/O, bits 0-3 for
   memory), inverting all 32 bits (logical NOT), then incrementing by 1. The resultant 32-bit
   value is the memory/I/O range size decoded by the register. Note that the upper 16 bits of
   the result is ignored if the Base Address register is for I/O and bits 16-31 returned zero
   upon read. The original value in the Base Address register is restored before re-enabling
   decode in the command register of the device.
   
   64-bit (memory) Base Address registers can be handled the same, except that the second
   32-bit register is considered an extension of the first; i.e., bits 32-63. Software writes
   0FFFFFFFFh to both registers, reads them back, and combines the result into a 64-bit value.
   Size calculation is done on the 64-bit value.
*/
bit32u pci_mem_range(const bit8u bus, const bit8u dev, const bit8u func, const bit8u port) {
  bit32u org0, org1, cmnd;
  bit64u range;
  
  // get the command register, save it, and disable I/O
  cmnd = pci_read_word(bus, dev, func, 0x04);
  pci_write_word(bus, dev, func, 0x04, cmnd & ~0x07);
  
  // read the original value(s)
  org0 = pci_read_dword(bus, dev, func, port);
  if ((org0 & 0x07) == 0x04)  // 64 bit and mem I/O
    org1 = pci_read_dword(bus, dev, func, port+4);
  
  // write 0xFFFFFFFF
  pci_write_dword(bus, dev, func, port, 0xFFFFFFFF);
  if ((org0 & 0x07) == 0x04)  // 64 bit and mem I/O
    pci_write_dword(bus, dev, func, port+4, 0xFFFFFFFF);
  
  // read it back.
  range = pci_read_dword(bus, dev, func, port);
  if ((org0 & 0x07) == 0x04)  // 64 bit and mem I/O
    range |= ((bit64u) pci_read_dword(bus, dev, func, port+4) << 32);
  
  // write back the original value
  pci_write_dword(bus, dev, func, port, org0);
  if ((org0 & 0x07) == 0x04)  // 64 bit and mem I/O
    pci_write_dword(bus, dev, func, port+4, org1);
  
  // restore the command register
  pci_write_word(bus, dev, func, 0x04, cmnd);
  
  if (org0 & 1) { // port I/O
    org0 = (bit32u) range;
    if ((org0 & 0xFFFF0000) == 0)    // upper 16-bits are reserved if read as zero
      org0 |= 0xFFFF0000;
    return (bit32u) (~(org0 & ~0x1) + 1);
  } else
    return (bit32u) (~(range & (bit64u) ~0xF) + 1);
}

#endif // FYSOS__PCI
