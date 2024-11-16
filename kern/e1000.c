#include <kern/e1000.h>

// LAB 6: Your driver code here
#include <inc/x86.h>
#include <inc/assert.h>
#include <inc/string.h>
#include <kern/pmap.h>

volatile uint32_t *e1000_bar0;

int 
pci_e1000_attach(struct pci_func *pcif)
{
	// Step 1. after attach and then enable pci device
	pci_func_enable(pcif);

	// Step 2. MMIO communicate PCI device through memory
	cprintf("pcif->reg_base[0] : 0x%08x pcif->reg_size[0] : 0x%08x\n",
									pcif->reg_base[0], pcif->reg_size[0]);
	// BAR0 memory mapped
	e1000_bar0 = mmio_map_region(pcif->reg_base[0], pcif->reg_size[0]);
	cprintf("e1000_bar0   : 0x%08x\n", e1000_bar0);
	cprintf("e1000_status : 0x%08x\n", e1000_bar0[E1000_STATUS]); 
	return 1;

}



