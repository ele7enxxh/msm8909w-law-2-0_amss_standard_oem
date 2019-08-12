#line 1 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/buses/spi/config/spi_props_tz_8909.xml"























<driver name="Spi">
   <global_def>
      <var_seq name="spiqup1_appclk_name"   type=DALPROP_DATA_TYPE_STRING>   gcc_blsp1_qup1_spi_apps_clk   </var_seq>
      <var_seq name="spiqup2_appclk_name"   type=DALPROP_DATA_TYPE_STRING>   gcc_blsp1_qup2_spi_apps_clk   </var_seq>
      <var_seq name="spiqup3_appclk_name"   type=DALPROP_DATA_TYPE_STRING>   gcc_blsp1_qup3_spi_apps_clk   </var_seq>	  
      <var_seq name="spiqup4_appclk_name"   type=DALPROP_DATA_TYPE_STRING>   gcc_blsp1_qup4_spi_apps_clk   </var_seq>
      <var_seq name="spiqup5_appclk_name"   type=DALPROP_DATA_TYPE_STRING>   gcc_blsp1_qup5_spi_apps_clk   </var_seq>
      <var_seq name="spiqup6_appclk_name"   type=DALPROP_DATA_TYPE_STRING>   gcc_blsp1_qup6_spi_apps_clk   </var_seq>

      <var_seq name="spiqup1_hclk_name"     type=DALPROP_DATA_TYPE_STRING>   gcc_blsp1_ahb_clk             </var_seq>
      <var_seq name="spiqup2_hclk_name"     type=DALPROP_DATA_TYPE_STRING>   gcc_blsp1_ahb_clk             </var_seq>
      <var_seq name="spiqup3_hclk_name"     type=DALPROP_DATA_TYPE_STRING>   gcc_blsp1_ahb_clk             </var_seq>
      <var_seq name="spiqup4_hclk_name"     type=DALPROP_DATA_TYPE_STRING>   gcc_blsp1_ahb_clk             </var_seq>
      <var_seq name="spiqup5_hclk_name"     type=DALPROP_DATA_TYPE_STRING>   gcc_blsp1_ahb_clk             </var_seq>
      <var_seq name="spiqup6_hclk_name"     type=DALPROP_DATA_TYPE_STRING>   gcc_blsp1_ahb_clk             </var_seq>

      <var_seq name="gpio_spi1_clk_str"    type=DALPROP_DATA_TYPE_STRING>   blsp_spi_clk[1]    </var_seq>
      <var_seq name="gpio_spi1_cs_str"     type=DALPROP_DATA_TYPE_STRING>   blsp_spi_cs_n[1]   </var_seq>
      <var_seq name="gpio_spi1_mosi_str"   type=DALPROP_DATA_TYPE_STRING>   blsp_spi_mosi[1]   </var_seq>
      <var_seq name="gpio_spi1_miso_str"   type=DALPROP_DATA_TYPE_STRING>   blsp_spi_miso[1]   </var_seq>

      <var_seq name="gpio_spi2_clk_str"    type=DALPROP_DATA_TYPE_STRING>   blsp_spi_clk[2]    </var_seq>
      <var_seq name="gpio_spi2_cs_str"     type=DALPROP_DATA_TYPE_STRING>   blsp_spi_cs_n[2]   </var_seq>
      <var_seq name="gpio_spi2_mosi_str"   type=DALPROP_DATA_TYPE_STRING>   blsp_spi_mosi[2]   </var_seq>
      <var_seq name="gpio_spi2_miso_str"   type=DALPROP_DATA_TYPE_STRING>   blsp_spi_miso[2]   </var_seq>

      <var_seq name="gpio_spi3_clk_str"    type=DALPROP_DATA_TYPE_STRING>   blsp_spi_clk[3]    </var_seq>
      <var_seq name="gpio_spi3_cs_str"     type=DALPROP_DATA_TYPE_STRING>   blsp_spi_cs_n[3]   </var_seq>
      <var_seq name="gpio_spi3_mosi_str"   type=DALPROP_DATA_TYPE_STRING>   blsp_spi_mosi[3]   </var_seq>
      <var_seq name="gpio_spi3_miso_str"   type=DALPROP_DATA_TYPE_STRING>   blsp_spi_miso[3]   </var_seq>

      <var_seq name="gpio_spi4_clk_str"    type=DALPROP_DATA_TYPE_STRING>   blsp_spi_clk[4]    </var_seq>
      <var_seq name="gpio_spi4_cs_str"     type=DALPROP_DATA_TYPE_STRING>   blsp_spi_cs_n[4]   </var_seq>
      <var_seq name="gpio_spi4_mosi_str"   type=DALPROP_DATA_TYPE_STRING>   blsp_spi_mosi[4]   </var_seq>
      <var_seq name="gpio_spi4_miso_str"   type=DALPROP_DATA_TYPE_STRING>   blsp_spi_miso[4]   </var_seq>

      <var_seq name="gpio_spi5_clk_str"    type=DALPROP_DATA_TYPE_STRING>   blsp_spi_clk[5]    </var_seq>
      <var_seq name="gpio_spi5_cs_str"     type=DALPROP_DATA_TYPE_STRING>   blsp_spi_cs_n[5]   </var_seq>
      <var_seq name="gpio_spi5_mosi_str"   type=DALPROP_DATA_TYPE_STRING>   blsp_spi_mosi[5]   </var_seq>
      <var_seq name="gpio_spi5_miso_str"   type=DALPROP_DATA_TYPE_STRING>   blsp_spi_miso[5]   </var_seq>

      <var_seq name="gpio_spi6_clk_str"    type=DALPROP_DATA_TYPE_STRING>   blsp_spi_clk[6]    </var_seq>
      <var_seq name="gpio_spi6_cs_str"     type=DALPROP_DATA_TYPE_STRING>   blsp_spi_cs_n[6]   </var_seq>
      <var_seq name="gpio_spi6_mosi_str"   type=DALPROP_DATA_TYPE_STRING>   blsp_spi_mosi[6]   </var_seq>
      <var_seq name="gpio_spi6_miso_str"   type=DALPROP_DATA_TYPE_STRING>   blsp_spi_miso[6]   </var_seq>

   </global_def>

   <device id=DALDEVICEID_SPI_DEVICE_1>
      <props name="QUP_CORE_NUM"       type=DALPROP_ATTR_TYPE_UINT32>   1           </props>
      <props name="QUP_BASE_ADDR"      type=DALPROP_ATTR_TYPE_UINT32>   0x78B5000   </props>
      <props name="SW_ENABLE_BAM"      type=DALPROP_ATTR_TYPE_UINT32>   1           </props>
      <props name="BAM_BLSP_ID"        type=DALPROP_ATTR_TYPE_UINT32>   1           </props>
      <props name="BAM_BASE_ADDR"      type=DALPROP_ATTR_TYPE_UINT32>   0x7884000   </props>
      <props name="BAM_INTERRUPT_ID"   type=DALPROP_ATTR_TYPE_UINT32>   270         </props>
      <props name="BAM_TX_PIPE_NUM"    type=DALPROP_ATTR_TYPE_UINT32>   4           </props>
      <props name="BAM_RX_PIPE_NUM"    type=DALPROP_ATTR_TYPE_UINT32>   5           </props>
       
      <props name="CHIP_APP_CLK_NAME"   type=DALPROP_ATTR_TYPE_STRING_PTR>   spiqup1_appclk_name   </props>
      <props name="CHIP_HCLK_NAME"      type=DALPROP_ATTR_TYPE_STRING_PTR>   spiqup1_hclk_name     </props>
      <props name="gpio_spi_clk_str"    type=DALPROP_ATTR_TYPE_STRING_PTR>   gpio_spi1_clk_str     </props>
      <props name="gpio_spi_cs_str"     type=DALPROP_ATTR_TYPE_STRING_PTR>   gpio_spi1_cs_str      </props>
      <props name="gpio_spi_mosi_str"   type=DALPROP_ATTR_TYPE_STRING_PTR>   gpio_spi1_mosi_str    </props>
      <props name="gpio_spi_miso_str"   type=DALPROP_ATTR_TYPE_STRING_PTR>   gpio_spi1_miso_str    </props>
   </device>

    <device id=DALDEVICEID_SPI_DEVICE_2>
      <props name="QUP_CORE_NUM"       type=DALPROP_ATTR_TYPE_UINT32>   2           </props>
      <props name="QUP_BASE_ADDR"      type=DALPROP_ATTR_TYPE_UINT32>   0x78B6000   </props>
      <props name="SW_ENABLE_BAM"      type=DALPROP_ATTR_TYPE_UINT32>   1           </props>
      <props name="BAM_BLSP_ID"        type=DALPROP_ATTR_TYPE_UINT32>   1           </props>
      <props name="BAM_BASE_ADDR"      type=DALPROP_ATTR_TYPE_UINT32>   0x7884000   </props>
      <props name="BAM_INTERRUPT_ID"   type=DALPROP_ATTR_TYPE_UINT32>   270         </props>
      <props name="BAM_TX_PIPE_NUM"    type=DALPROP_ATTR_TYPE_UINT32>   6           </props>
      <props name="BAM_RX_PIPE_NUM"    type=DALPROP_ATTR_TYPE_UINT32>   7           </props>
       
      <props name="CHIP_APP_CLK_NAME"   type=DALPROP_ATTR_TYPE_STRING_PTR>   spiqup2_appclk_name   </props>
      <props name="CHIP_HCLK_NAME"      type=DALPROP_ATTR_TYPE_STRING_PTR>   spiqup2_hclk_name     </props>
      <props name="gpio_spi_clk_str"    type=DALPROP_ATTR_TYPE_STRING_PTR>   gpio_spi2_clk_str     </props>
      <props name="gpio_spi_cs_str"     type=DALPROP_ATTR_TYPE_STRING_PTR>   gpio_spi2_cs_str      </props>
      <props name="gpio_spi_mosi_str"   type=DALPROP_ATTR_TYPE_STRING_PTR>   gpio_spi2_mosi_str    </props>
      <props name="gpio_spi_miso_str"   type=DALPROP_ATTR_TYPE_STRING_PTR>   gpio_spi2_miso_str    </props>
    </device>

    <device id=DALDEVICEID_SPI_DEVICE_3>
      <props name="QUP_CORE_NUM"       type=DALPROP_ATTR_TYPE_UINT32>   3           </props>
      <props name="QUP_BASE_ADDR"      type=DALPROP_ATTR_TYPE_UINT32>   0x78B7000   </props>
      <props name="SW_ENABLE_BAM"      type=DALPROP_ATTR_TYPE_UINT32>   1           </props>
      <props name="BAM_BLSP_ID"        type=DALPROP_ATTR_TYPE_UINT32>   1           </props>
      <props name="BAM_BASE_ADDR"      type=DALPROP_ATTR_TYPE_UINT32>   0x7884000   </props>
      <props name="BAM_INTERRUPT_ID"   type=DALPROP_ATTR_TYPE_UINT32>   270         </props>
      <props name="BAM_TX_PIPE_NUM"    type=DALPROP_ATTR_TYPE_UINT32>   8           </props>
      <props name="BAM_RX_PIPE_NUM"    type=DALPROP_ATTR_TYPE_UINT32>   9           </props>
 
      <props name="CHIP_APP_CLK_NAME"   type=DALPROP_ATTR_TYPE_STRING_PTR>   spiqup3_appclk_name   </props>
      <props name="CHIP_HCLK_NAME"      type=DALPROP_ATTR_TYPE_STRING_PTR>   spiqup3_hclk_name     </props>
      <props name="gpio_spi_clk_str"    type=DALPROP_ATTR_TYPE_STRING_PTR>   gpio_spi3_clk_str     </props>
      <props name="gpio_spi_cs_str"     type=DALPROP_ATTR_TYPE_STRING_PTR>   gpio_spi3_cs_str      </props>
      <props name="gpio_spi_mosi_str"   type=DALPROP_ATTR_TYPE_STRING_PTR>   gpio_spi3_mosi_str    </props>
      <props name="gpio_spi_miso_str"   type=DALPROP_ATTR_TYPE_STRING_PTR>   gpio_spi3_miso_str    </props>
    </device>

    <device id=DALDEVICEID_SPI_DEVICE_4>
      <props name="QUP_CORE_NUM"       type=DALPROP_ATTR_TYPE_UINT32>   4           </props>
      <props name="QUP_BASE_ADDR"      type=DALPROP_ATTR_TYPE_UINT32>   0x78B8000   </props>
      <props name="SW_ENABLE_BAM"      type=DALPROP_ATTR_TYPE_UINT32>   1           </props>
      <props name="BAM_BLSP_ID"        type=DALPROP_ATTR_TYPE_UINT32>   1           </props>
      <props name="BAM_BASE_ADDR"      type=DALPROP_ATTR_TYPE_UINT32>   0x7884000   </props>
      <props name="BAM_INTERRUPT_ID"   type=DALPROP_ATTR_TYPE_UINT32>   270         </props>
      <props name="BAM_TX_PIPE_NUM"    type=DALPROP_ATTR_TYPE_UINT32>   10          </props>
      <props name="BAM_RX_PIPE_NUM"    type=DALPROP_ATTR_TYPE_UINT32>   11          </props>
 
      <props name="CHIP_APP_CLK_NAME"   type=DALPROP_ATTR_TYPE_STRING_PTR>   spiqup4_appclk_name   </props>
      <props name="CHIP_HCLK_NAME"      type=DALPROP_ATTR_TYPE_STRING_PTR>   spiqup4_hclk_name     </props>
      <props name="gpio_spi_clk_str"    type=DALPROP_ATTR_TYPE_STRING_PTR>   gpio_spi4_clk_str     </props>
      <props name="gpio_spi_cs_str"     type=DALPROP_ATTR_TYPE_STRING_PTR>   gpio_spi4_cs_str      </props>
      <props name="gpio_spi_mosi_str"   type=DALPROP_ATTR_TYPE_STRING_PTR>   gpio_spi4_mosi_str    </props>
      <props name="gpio_spi_miso_str"   type=DALPROP_ATTR_TYPE_STRING_PTR>   gpio_spi4_miso_str    </props>
    </device>

    <device id=DALDEVICEID_SPI_DEVICE_5>
      <props name="QUP_CORE_NUM"       type=DALPROP_ATTR_TYPE_UINT32>   5           </props>
      <props name="QUP_BASE_ADDR"      type=DALPROP_ATTR_TYPE_UINT32>   0x78B9000   </props>
      <props name="SW_ENABLE_BAM"      type=DALPROP_ATTR_TYPE_UINT32>   1           </props>
      <props name="BAM_BLSP_ID"        type=DALPROP_ATTR_TYPE_UINT32>   1           </props>
      <props name="BAM_BASE_ADDR"      type=DALPROP_ATTR_TYPE_UINT32>   0x7884000   </props>
      <props name="BAM_INTERRUPT_ID"   type=DALPROP_ATTR_TYPE_UINT32>   270         </props>
      <props name="BAM_TX_PIPE_NUM"    type=DALPROP_ATTR_TYPE_UINT32>   12          </props>
      <props name="BAM_RX_PIPE_NUM"    type=DALPROP_ATTR_TYPE_UINT32>   13          </props>
 
      <props name="CHIP_APP_CLK_NAME"   type=DALPROP_ATTR_TYPE_STRING_PTR>   spiqup5_appclk_name   </props>
      <props name="CHIP_HCLK_NAME"      type=DALPROP_ATTR_TYPE_STRING_PTR>   spiqup5_hclk_name     </props>
      <props name="gpio_spi_clk_str"    type=DALPROP_ATTR_TYPE_STRING_PTR>   gpio_spi5_clk_str     </props>
      <props name="gpio_spi_cs_str"     type=DALPROP_ATTR_TYPE_STRING_PTR>   gpio_spi5_cs_str      </props>
      <props name="gpio_spi_mosi_str"   type=DALPROP_ATTR_TYPE_STRING_PTR>   gpio_spi5_mosi_str    </props>
      <props name="gpio_spi_miso_str"   type=DALPROP_ATTR_TYPE_STRING_PTR>   gpio_spi5_miso_str    </props>
    </device>

    <device id=DALDEVICEID_SPI_DEVICE_6>
      <props name="QUP_CORE_NUM"       type=DALPROP_ATTR_TYPE_UINT32>   6           </props>
      <props name="QUP_BASE_ADDR"      type=DALPROP_ATTR_TYPE_UINT32>   0x78BA000   </props>
      <props name="SW_ENABLE_BAM"      type=DALPROP_ATTR_TYPE_UINT32>   1           </props>
      <props name="BAM_BLSP_ID"        type=DALPROP_ATTR_TYPE_UINT32>   1           </props>
      <props name="BAM_BASE_ADDR"      type=DALPROP_ATTR_TYPE_UINT32>   0x7884000   </props>
      <props name="BAM_INTERRUPT_ID"   type=DALPROP_ATTR_TYPE_UINT32>   270         </props>
      <props name="BAM_TX_PIPE_NUM"    type=DALPROP_ATTR_TYPE_UINT32>   14          </props>
      <props name="BAM_RX_PIPE_NUM"    type=DALPROP_ATTR_TYPE_UINT32>   15          </props>
 
      <props name="CHIP_APP_CLK_NAME"   type=DALPROP_ATTR_TYPE_STRING_PTR>   spiqup6_appclk_name   </props>
      <props name="CHIP_HCLK_NAME"      type=DALPROP_ATTR_TYPE_STRING_PTR>   spiqup6_hclk_name     </props>
      <props name="gpio_spi_clk_str"    type=DALPROP_ATTR_TYPE_STRING_PTR>   gpio_spi6_clk_str     </props>
      <props name="gpio_spi_cs_str"     type=DALPROP_ATTR_TYPE_STRING_PTR>   gpio_spi6_cs_str      </props>
      <props name="gpio_spi_mosi_str"   type=DALPROP_ATTR_TYPE_STRING_PTR>   gpio_spi6_mosi_str    </props>
      <props name="gpio_spi_miso_str"   type=DALPROP_ATTR_TYPE_STRING_PTR>   gpio_spi6_miso_str    </props>
    </device>
</driver>
 
