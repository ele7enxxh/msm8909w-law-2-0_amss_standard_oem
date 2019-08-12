<driver name="NULL">
typedef struct {  
   uint32 nGpioNumber;
   uint32 nFunctionSelect;
}TLMMGpioIdType;
  <device id="/tlmm/configs">
  <props name="blsp_spi_mosi[3]" type="TLMMGpioIdType">{0, 1}</props>
  <props name="blsp_spi_miso[3]" type="TLMMGpioIdType">{1, 1}</props>
  <props name="blsp_spi_cs_n[3]" type="TLMMGpioIdType">{2, 1}</props>
  <props name="blsp_spi_clk[3]" type="TLMMGpioIdType">{3, 1}</props>
  <props name="blsp_spi_mosi[1]" type="TLMMGpioIdType">{4, 1}</props>
  <props name="blsp3_spi_cs3_n" type="TLMMGpioIdType">{4, 4}</props>
  <props name="blsp_spi_miso[1]" type="TLMMGpioIdType">{5, 1}</props>
  <props name="blsp2_spi_cs3_n" type="TLMMGpioIdType">{5, 4}</props>
  <props name="blsp_spi_cs_n[1]" type="TLMMGpioIdType">{6, 1}</props>
  <props name="blsp_spi_clk[1]" type="TLMMGpioIdType">{7, 1}</props>
  <props name="blsp_spi_mosi[6]" type="TLMMGpioIdType">{8, 1}</props>
  <props name="blsp_spi_miso[6]" type="TLMMGpioIdType">{9, 1}</props>
  <props name="blsp_spi_cs_n[6]" type="TLMMGpioIdType">{10, 1}</props>
  <props name="blsp_spi_clk[6]" type="TLMMGpioIdType">{11, 1}</props>
  <props name="blsp_spi_mosi[4]" type="TLMMGpioIdType">{12, 1}</props>
  <props name="blsp_spi_miso[4]" type="TLMMGpioIdType">{13, 1}</props>
  <props name="blsp_spi_cs_n[4]" type="TLMMGpioIdType">{14, 1}</props>
  <props name="blsp_spi_clk[4]" type="TLMMGpioIdType">{15, 1}</props>
  <props name="blsp_spi_mosi[5]" type="TLMMGpioIdType">{16, 1}</props>
  <props name="blsp_spi_miso[5]" type="TLMMGpioIdType">{17, 1}</props>
  <props name="blsp2_spi_cs2_n" type="TLMMGpioIdType">{17, 2}</props>
  <props name="blsp_spi_cs_n[5]" type="TLMMGpioIdType">{18, 1}</props>
  <props name="blsp_spi_clk[5]" type="TLMMGpioIdType">{19, 1}</props>
  <props name="blsp_spi_mosi[2]" type="TLMMGpioIdType">{20, 2}</props>
  <props name="blsp_spi_miso[2]" type="TLMMGpioIdType">{21, 2}</props>
  <props name="blsp1_spi_cs2_n" type="TLMMGpioIdType">{37, 1}</props>
  <props name="blsp3_spi_cs2_n" type="TLMMGpioIdType">{65, 1}</props>
  <props name="blsp1_spi_cs3_n" type="TLMMGpioIdType">{65, 2}</props>
  <props name="blsp3_spi_cs1_n" type="TLMMGpioIdType">{95, 1}</props>
  <props name="blsp1_spi_cs1_n" type="TLMMGpioIdType">{97, 1}</props>
  <props name="blsp2_spi_cs1_n" type="TLMMGpioIdType">{98, 3}</props>
  <props name="blsp_spi_cs_n[2]" type="TLMMGpioIdType">{111, 1}</props>
  <props name="blsp_spi_clk[2]" type="TLMMGpioIdType">{112, 1}</props>
    <props name="tlmm_offset" type=DALPROP_ATTR_TYPE_UINT32>
      0x00000000 
    </props>
    <props name="tlmm_total_gpio" type=DALPROP_ATTR_TYPE_UINT32>
      113
    </props>
  </device>
</driver>
