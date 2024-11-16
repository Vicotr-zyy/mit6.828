#include <kern/e1000.h>

// LAB 6: Your driver code here
#include <inc/x86.h>
#include <inc/assert.h>
#include <inc/string.h>
#include <inc/error.h>
#include <kern/pmap.h>

volatile uint32_t *e1000;
// The driver is responsible for 
// allocating memory for the transmit and receive queues, 
// setting up DMA descriptors, 
// and configuring the E1000 with the location of these queues, 
// but everything after that is asynchronous.
// Transmit Descriptor Array
// len of descriptor is 16 bytes 128 = 8 * 16
#define tx_desc_len 16
// Transmit Descriptor Queue
__attribute__((__aligned__(16)))
static struct tx_desc tx_desc_buffer[tx_desc_len];
// Transmit packets buffers
static char packet_buffer[tx_desc_len][1024];

static void transmit_init();

int 
pci_e1000_attach(struct pci_func *pcif)
{
	int r;
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
	// Step 4. transmit a message for test from kernel
	transmit_pack("zhangyanyuan", 10);
	transmit_pack("hello", 5);
	return 1;
}

static void 
transmit_init()
{
	// Step 1. allocate a region of memory for transmit descriptor list (16-byte aligned)
	// Step 2. set TDBAL 32-bit address
	e1000[E1000_TDBAL] = (uint32_t)PADDR(tx_desc_buffer);
	e1000[E1000_TDBAH] = 0;
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
	// Step 7. initialzie the tx_desc_buffer to indicate empty and ready to recycle that descriptor and use it to transmit another packet
	struct tx_desc tx;
	int i = 0;
	for(i = 0; i < tx_desc_len ; i++){
		memset(&tx, 0, sizeof(struct tx_desc));
		tx.addr = (uint32_t)PADDR(packet_buffer[i]);
		tx.upper.data |= E1000_TXD_STAT_DD;
		//cprintf("tx.addr : 0x%08x\n", tx.addr);
		tx_desc_buffer[i] = tx;
	}

}

int
transmit_pack(const char *data, int len)
{
	// Step 1. read the transmit descriptor tail
	int index = e1000[E1000_TDT];	
	// check the transmit queue being full
	struct tx_desc c_tx = tx_desc_buffer[index];	
	if((c_tx.upper.data & E1000_TXD_STAT_DD) == 0){
		// the queue is full
		// send message to user to resend the message
		return -E_NO_MEM;
	}
	// Step 2. prepare a packet that's about to send
	// struct PageInfo * pginfo = page_alloc(ALLOC_ZERO);
	// page_insert(kern_pgdir, pginfo, 

	strncpy(packet_buffer[index], data, len);
	//tlb_invalidate(kern_pgdir, packet_buffer[index]);
	c_tx.lower.data |= E1000_TXD_CMD_RS;
	c_tx.lower.data |= E1000_TXD_CMD_EOP;
	//c_tx.lower.data |= E1000_TXD_CMD_IDE;
	//c_tx.lower.data |= E1000_TXD_CMD_RPS;
	c_tx.upper.fields.status = 0;
	c_tx.lower.flags.length = len;
	//cprintf("c_tx.lower : 0x%08x\n", c_tx.lower.data);
	// Step 3. copy to the descriptor buffer
	tx_desc_buffer[(index++ % tx_desc_len)] = c_tx;
	// Step 4. update the TDT
	e1000[E1000_TDT] = index;

	return 0;
}

