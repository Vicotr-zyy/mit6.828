#ifndef JOS_KERN_E1000_H
#define JOS_KERN_E1000_H

#define PCI_82540EM_VID	0x8086
#define PCI_82540EM_DID	0x100E

#include <kern/pci.h>
// BAR0 Reg
#define E1000_CTRL     (0x00000/4) /* Device Control - RW */
#define E1000_CTRL_DUP (0x00004/4)  /* Device Control Duplicate (Shadow) - RW */
#define E1000_STATUS   (0x00008/4)  /* Device Status - RO */
#define E1000_TCTL     (0x00400/4)  /* TX Control - RW */
#define E1000_TCTL_EXT (0x00404/4)  /* Extended TX Control - RW */
#define E1000_TDBAL    (0x03800/4)  /* TX Descriptor Base Address Low - RW */
#define E1000_TDBAH    (0x03804/4)  /* TX Descriptor Base Address High - RW */
#define E1000_TDLEN    (0x03808/4)  /* TX Descriptor Length - RW */
#define E1000_TDH      (0x03810/4)  /* TX Descriptor Head - RW */
#define E1000_TDT      (0x03818/4)  /* TX Descripotr Tail - RW */
#define E1000_TIPG     (0x00410/4)  /* TX Inter-packet gap -RW */

/* Transmit Control */
#define E1000_TCTL_RST    0x00000001    /* software reset */
#define E1000_TCTL_EN     0x00000002    /* enable tx */
#define E1000_TCTL_BCE    0x00000004    /* busy check enable */
#define E1000_TCTL_PSP    0x00000008    /* pad short packets */
#define E1000_TCTL_CT     0x00000ff0    /* collision threshold */
#define E1000_TCTL_COLD   0x003ff000    /* collision distance */
#define E1000_TCTL_SWXOFF 0x00400000    /* SW Xoff transmission */
#define E1000_TCTL_PBE    0x00800000    /* Packet Burst Enable */
#define E1000_TCTL_RTLC   0x01000000    /* Re-transmit on late collision */
#define E1000_TCTL_NRTU   0x02000000    /* No Re-transmit on underrun */
#define E1000_TCTL_MULR   0x10000000    /* Multiple request support */


int pci_e1000_attach(struct pci_func *pcif);

struct tx_desc{
	uint64_t addr;
	uint16_t length;
	uint8_t cso;
	uint8_t cmd;
	uint8_t status;
	uint8_t css;
	uint16_t special;
};

#endif	// JOS_KERN_E1000_H
