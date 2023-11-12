/*

Prerequisites:
 - Vivado 2023.1 XDMA AXI Demo Project:
   github.com/mwrnd/innova2_xdma_demo
 - XDMA Drivers from github.com/xilinx/dma_ip_drivers
   Install Instructions at github.com/mwrnd/innova2_flex_xcku15p_notes


Compile with:

  gcc -Wall innova2_xdma_test.c -o innova2_xdma_test -lm

Run with:

  sudo ./innova2_xdma_test

*/

#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>




// The XDMA AXI aclk depends on PCIe Lane Width, AXI Data Width, and
// Maximum Link Speed settings in the XDMA IP Block
#define XDMA_CLK_MHz	250

// The PCIe to AXI Translation Offset for the PCIe to AXI Lite Interface
#define XDMA_PCIe_to_AXI_Translation_Offset 0x00000000

// Using 2 mebibyte == 2^21 = 2097152 byte array. Size was defined in the
// Vivado FPGA Project Block Diagram Address Editor as the Data Range for BRAM
// On Linux, read/write can transfer at most 0x7FFFF000 (2,147,479,552) bytes
#define DATA_SIZE 2097152



// Global struct for XDMA Device files
struct _XDMA {
	char *userfilename;
	char *h2cfilename;
	char *c2hfilename;
	int userfd;
	int h2cfd;
	int c2hfd;
} xdma;





ssize_t read_axilite_word(uint64_t address, uint32_t *read_word)
{
	ssize_t rc = 0;

	rc = pread(xdma.userfd, read_word, 4, address);
	if (rc < 0) {
		fprintf(stderr, "%s, read data @ 0x%lX failed, %ld.\n",
			xdma.userfilename, address, rc);
		perror("File Read");
		return -EIO;
	}
	if (rc != 4) {
		fprintf(stderr, "%s, read underflow @ 0x%lX, read %ld/4.\n",
			xdma.userfilename, address, rc);
		perror("Read Underflow");
	}
	
	return rc;
}




ssize_t write_axilite_word(uint64_t address, uint32_t write_word)
{
	ssize_t rc;

	rc = pwrite(xdma.userfd, &write_word, 4, address);
	if (rc < 0) {
		fprintf(stderr, "%s, write data @ 0x%lX failed, %ld.\n",
			xdma.userfilename, address, rc);
		perror("File Write");
		return -EIO;
	}
	if (rc != 4) {
		fprintf(stderr, "%s, write underflow @ 0x%lX, %ld/4.\n",
			xdma.userfilename, address, rc);
		perror("Write Underflow");
	}

	return rc;
}




ssize_t read_from_axi(uint64_t address, size_t bytes, void *buffer)
{
	ssize_t rc;

	rc = pread(xdma.c2hfd, buffer, bytes, address);
	//fsync(xdma.c2hfd);

	if (rc < 0) {
		fprintf(stderr, "%s, read data @ 0x%lX failed, %ld.\n",
			xdma.c2hfilename, address, rc);
		perror("File Read");
		return -EIO;
	}
	if (rc != bytes) {
		fprintf(stderr, "%s, read underflow @ 0x%lX, %ld/%ld.\n",
			xdma.c2hfilename, address, rc, bytes);
		perror("Read Underflow");
	}

	return rc;
}




ssize_t write_to_axi(uint64_t address, size_t bytes, void *buffer)
{
	ssize_t rc;

	rc = pwrite(xdma.h2cfd, buffer, bytes, address);
	//fsync(xdma.h2cfd);

	if (rc < 0) {
		fprintf(stderr, "%s, write data @ 0x%lX failed, %ld.\n",
			xdma.h2cfilename, address, rc);
		perror("File Write");
		return -EIO;
	}
	if (rc != bytes) {
		fprintf(stderr, "%s, write underflow @ 0x%lX, %ld/%ld.\n",
			xdma.h2cfilename, address, rc, bytes);
		perror("Write Underflow");
	}

	return rc;
}




int estimate_clock_MHz(uint8_t delay, char *clkname, uint64_t axi_gpio_addr)
{
	uint8_t read_data[16];
	uint32_t val1 = 0;
	uint32_t val2 = 0;
	uint32_t val3 = 0;
	uint32_t val4 = 0;
	uint32_t diff_clkA;
	uint32_t diff_clkB;
	ssize_t rc;
	double clk_ratio = 0;


	if (axi_gpio_addr == 0) { return -1; }


	rc = read_from_axi(axi_gpio_addr, 16, &read_data);
	if (rc != 16) {return -1; }

	memcpy(&val1, &read_data[0], 4);
	memcpy(&val2, &read_data[8], 4);

	printf("\nt=0 clock counter values at 0x%lX : 0x%08X 0x%08X\n",
		axi_gpio_addr, val1, val2);


	sleep(delay);


	rc = read_from_axi(axi_gpio_addr, 16, &read_data);
	if (rc != 16) {return -1; }

	memcpy(&val3, &read_data[0], 4);
	memcpy(&val4, &read_data[8], 4);

	printf("t=%d clock counter values at 0x%lX : 0x%08X 0x%08X\n",
		delay, axi_gpio_addr, val3, val4);


	diff_clkA = abs(val3-val1);
	diff_clkB = abs(val4-val2);
	clk_ratio =(((double)diff_clkB)/((double)diff_clkA));

	printf("    Clock %s has ratio = %lf, estimate %lf MHz",
		clkname, clk_ratio, (clk_ratio * XDMA_CLK_MHz));

	if ((clk_ratio * XDMA_CLK_MHz) < 1) {
		printf(" = % 9.0lf Hz\n",
			(clk_ratio * (XDMA_CLK_MHz * 1000000)));
	} else {
		printf("\n");
	}

	return 0;
}




int main(int argc, char **argv)
{
	uint64_t addr = 0;
	uint32_t val = 0;
	int errorcount = 0;
	uint8_t wrte_data[DATA_SIZE];
	uint8_t read_data[DATA_SIZE];
	ssize_t rc;




	// Open M_AXI_LITE Device as Read-Write
	xdma.userfilename = "/dev/xdma0_user";

	xdma.userfd = open(xdma.userfilename, O_RDWR);

	if (xdma.userfd < 0) {
		fprintf(stderr, "unable to open device %s, %d.\n",
			xdma.userfilename, xdma.userfd);
		perror("File Open");
		exit(EXIT_FAILURE);
	}


	// Open M_AXI H2C Host-to-Card Device as Write-Only
	xdma.h2cfilename = "/dev/xdma0_h2c_0";

	xdma.h2cfd = open(xdma.h2cfilename, O_WRONLY);

	if (xdma.h2cfd < 0) {
		fprintf(stderr, "unable to open device %s, %d.\n",
			xdma.h2cfilename, xdma.h2cfd);
		perror("File Open");
		exit(EXIT_FAILURE);
	}


	// Open M_AXI C2H Card-to-Host Device as Read-Only
	xdma.c2hfilename = "/dev/xdma0_c2h_0";

	xdma.c2hfd = open(xdma.c2hfilename, O_RDONLY);

	if (xdma.c2hfd < 0) {
		fprintf(stderr, "unable to open device %s, %d.\n",
			xdma.c2hfilename, xdma.c2hfd);
		perror("File Open");
		exit(EXIT_FAILURE);
	}

	printf("\n");




	// Generate Random Data

	// Seed the random number generator with an arbitrary value: an address
	srandom((unsigned int)((long int)xdma.h2cfilename));

	// Generate a random data array of size DATA_SIZE
	for (int indx = 0; indx < DATA_SIZE ; indx = indx + 4)
	{
		val = rand();
		memcpy(&wrte_data[indx], &val, 4);
	}




	// -------- AXI BRAM Write then Read Test ----------------------------

	// Write the random data to the FPGA's AXI BRAM at address 0x80000000
	addr = 0x80000000;
	rc = write_to_axi(addr, DATA_SIZE, &wrte_data);

	printf("\nWrote %ld bytes to   %s at address 0x%lX\n",
		rc, xdma.h2cfilename, addr);


	// Read data from the FPGA's AXI BRAM
	addr = 0x80000000;
	rc = read_from_axi(addr, DATA_SIZE, &read_data);

	printf("\nRead  %ld bytes from %s at address 0x%lX\n",
		rc, xdma.c2hfilename, addr);


	// Compare the Written and Read Data

	errorcount = 0;
	for (int indx = 0; indx < DATA_SIZE ; indx++)
	{
		if (read_data[indx] != wrte_data[indx])
		{
			errorcount++;
			printf("Data did not match at index %d, ", indx);
			printf("read_data = 0x%02X, wrte_data = 0x%02X\n",
				read_data[indx], wrte_data[indx]);
		}

		// too many errors, something is wrong, do not check any more
		if (errorcount > 7 ) { break; }
	}

	if (errorcount == 0)
	{
		printf("\nSuccess - Read Data matches Written Data!\n\n");
	} else {
		printf("Too many errors encountered, something is wrong.\n\n");
	}




	// -------- AXI Lite GPIO LED Test -----------------------------------

	// Toggle LED once a second for 4 seconds
	// GPIO for LED is at AXI Address 0x90000 which is PCIe address
	// 0x90000 as XDMA_PCIe_to_AXI_Translation_Offset=0x00000000
	addr = 0x90000;
	uint32_t write_word = 0;
	for (int i = 0; i < 5 ; i++)
	{
		write_word = (uint32_t)(i & 0x00000001);
		write_axilite_word(addr, write_word);

		if (write_word) {
			printf("LED D18 should be ON.\n");
		} else {
			printf("LED D18 should be OFF.\n");
		}

		sleep(1);
	}

	printf("\n");




	// -------- Estimate Frequency of non-XDMA Clocks --------------------
	printf("\nCompare clocks against %dMHz XDMA axi_aclk:", XDMA_CLK_MHz);

	uint8_t delay = 1;
	estimate_clock_MHz(delay, "sys_clk_100MHz", 0x70100000);
	estimate_clock_MHz(delay, "emc_clk_150MHz", 0x70110000);
	estimate_clock_MHz(delay, "uram_clk", 0x70120000);

	printf("\n");




	// -------- AXI Lite BRAM Test ---------------------------------------

	printf("\nWrite Values to AXI Lite BRAM:\n");
	addr = 0x80000;
	write_axilite_word((addr+0x00), 0xA0A0A700); printf("  0xA0A0A700\n");
	write_axilite_word((addr+0x04), 0xA1A1A604); printf("  0xA1A1A604\n");
	write_axilite_word((addr+0x08), 0xA2A2A508); printf("  0xA2A2A508\n");
	write_axilite_word((addr+0x0C), 0xA3A3A40C); printf("  0xA3A3A40C\n");


	printf("\nRead Values from AXI Lite BRAM:\n");
	uint32_t read_word = 0;
	addr = 0x80000;
	// print the data in the return data buffer
	for (int i = 0 ; i < 16; i = i + 4)
	{
		read_axilite_word((addr+i), &read_word);

		printf("PCIe AXILite Address 0x%08lX=%ld data = %u = 0x%08X\n",
			(addr+i), (addr+i), read_word, read_word);
	}

	printf("\n");




	close(xdma.userfd);
	close(xdma.h2cfd);
	close(xdma.c2hfd);
	exit(EXIT_SUCCESS);
}

