#include <kern/e1000.h>

// LAB 6: Your driver code here
#include <inc/x86.h>
#include <inc/assert.h>
#include <inc/string.h>
#include <kern/pmap.h>

volatile uint32_t *e1000;

// Transmit Descriptor Array
#define tx_desc_len 16
__attribute__((__aligned__(16)))
static struct tx_desc tx_desc_buffer[tx_desc_len];

static void transmit_init();

int 
pci_e1000_attach(struct pci_func *pcif)
{
	// Step 1. after attach and then enable pci device
	pci_func_enable(pcif);

	// Step 2. MMIO communicate PCI device through memory
	cprintf("pcif->reg_base[0] : 0x%08x pcif->reg_size[0] : 0x%08x\n",
									pcif->reg_base[0], pcif->reg_size[0]);
	// BAR0 memory mapped
	e1000 = mmio_map_region(pcif->reg_base[0], pcif->reg_size[0]);
	cprintf("e1000_status : 0x%08x\n", e1000[E1000_STATUS]); 
	// Step 3. Transmit Initialization
	cprintf("Transmit Initialization Starting *******\n");
	transmit_init();
	return 1;
}

static void 
transmit_init()
{
	// Step 1. allocate a region of memory for transmit descriptor list (16-byte aligned)
	// Step 2. set TDBAL 32-bit address
	e1000[E1000_TDBAL] = (uint32_t)tx_desc_buffer;
	cprintf("tx_desc_buffer addr : 0x%08x\n", tx_desc_buffer);
	// Step 3. set TDLEN the size of the descriptor ring in bytes ( 128-byte aligned)
	e1000[E1000_TDLEN] = sizeof(struct tx_desc) * tx_desc_len;
	// Step 4. write 0b to both the TDH/TDT to ensure the registers are initialized
	e1000[E1000_TDH] = 0x0;
	e1000[E1000_TDT] = 0x0;
	// Step 5. initialize the transmit control reg TCTL
	e1000[E1000_TCTL] |= E1000_TCTL_EN; // set enable bit 
	e1000[E1000_TCTL] |= E1000_TCTL_PSP; // set Pad Short Packet bit 
	e1000[E1000_TCTL] &= ~E1000_TCTL_CT;
	e1000[E1000_TCTL] |= (0x10 << 4);  // set threshold to 0x10h
	e1000[E1000_TCTL] &= ~E1000_TCTL_COLD;
	e1000[E1000_TCTL] |= (0x40 << 12);  // set collision distance to 0x40h
	// Step 6. Transmit IPG reg
	//e1000[E1000_TIPG] = 6 <<10 | 8 << 10 | 10;
	//we assume duplex-communicate and 802.3 IEEE
	e1000[E1000_TIPG] = 10;
}
