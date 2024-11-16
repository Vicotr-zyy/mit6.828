#ifndef JOS_KERN_E1000_H
#define JOS_KERN_E1000_H

#define PCI_82540EM_VID	0x8086
#define PCI_82540EM_DID	0x100E

#include <kern/pci.h>
// BAR0 Reg
#define E1000_STATUS	(0x0008/4) // Device Status Register
int pci_e1000_attach(struct pci_func *pcif);

#endif	// JOS_KERN_E1000_H
