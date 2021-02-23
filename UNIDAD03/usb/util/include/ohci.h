//////////////////////////////////////////////////////////////////////////
//  ohci.h  v1.00
//////////////////////////////////////////////////////////////////////////

#ifndef FYSOS__OHCI
#define FYSOS__OHCI

#pragma pack(1)

#define OHCRevision          0x00
#define OHCControl           0x04
#define OHCCommandStatus     0x08
#define OHCInterruptStatus   0x0C
#define OHCInterruptEnable   0x10
#define OHCInterruptDisable  0x14
#define OHCHCCA              0x18
#define OHCPeriodCurrentED   0x1C
#define OHCControlHeadED     0x20
#define OHCControlCurrentED  0x24
#define OHCBulkHeadED        0x28
#define OHCBulkCurrentED     0x2C
#define OHCDoneHead          0x30
#define OHCFmInterval        0x34
#define OHCFmRemaining       0x38
#define OHCFmNumber          0x3C
#define OHCPeriodicStart     0x40
#define OHCLSThreshold       0x44
#define OHCRhDescriptorA     0x48
#define OHCRhDescriptorB     0x4C
#define OHCRhStatus          0x50
#define OHCRhPortStatus      0x54

#define OHCRhDescriptorA_MASK 0xFFFFFBF0

struct OHCI_ED {
  bit32u flags;
  bit32u tailp;
  bit32u headp;
  bit32u nexted;
};

struct OHCI_TD {
  bit32u flags;
  bit32u cbp;
  bit32u nexttd;
  bit32u be;
};

struct OHCI_HCCA {
  bit32u HccaInterruptTable[32];
  bit16u HccaFrameNumber;
  bit16u HccaPad1;
  bit32u HccaDoneHead;
  bit8u  reserved[116];
  bit32u unknown;
};


#define TD_DP_SETUP  0
#define TD_DP_OUT    1
#define TD_DP_IN     2
#define TD_DP_RESV   3

// we have to place all data in a single struct so that we can pass it back
//  from local access to physcal memory.
struct OHCI_FRAME {
  struct OHCI_HCCA hcca;            // 256 bytes
  bit8u  reserved0[16];             //  16
  struct OHCI_ED   ed_table[31];    // 496
  struct OHCI_ED   control_ed[16];  // 256
  struct OHCI_ED   bulk_ed[16];     // 256
  struct OHCI_TD   our_tds[32];     // 32 * 4 * 4 
  bit8u  setup[8];                  // 8
  bit8u  packet[32];                // return packet data space
};



#endif // FYSOS__OHCI
