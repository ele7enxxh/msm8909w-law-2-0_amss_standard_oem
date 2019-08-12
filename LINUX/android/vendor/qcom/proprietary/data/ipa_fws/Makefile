# IPAv3 (9x55) base addresses
DPS_ADDR := 0x07B5e000
HPS_ADDR := 0x07B5e080
GSI_ADDR := 0x07B08000

GSI_MEM_SIZE = 0x4000

all:
	python elf_creator.py \
	--gsi_fw fw_mhi_ipa.bin \
	--gsi_fw_address $(GSI_ADDR) \
	--gsi_fw_mem_size $(GSI_MEM_SIZE) \
	--hps hps.bin \
	--hps_addr $(HPS_ADDR) \
	--dps dps.bin \
	--dps_addr $(DPS_ADDR) \
	--outfile ipa_fws.elf
clean:
	rm ipa_fws.*