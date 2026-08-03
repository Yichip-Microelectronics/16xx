#ifndef __HW_H__
#define __HW_H__

#include "yc_types.h"
#include "yc_log.h"
#include <string.h>
#define core_revision                             *(volatile word*)0x8000
#define core_mfv_d                                *(volatile byte*)0x8002
#define core_modem_test_mux                       *(volatile byte*)0x8004
#define core_testmode                             *(volatile byte*)0x8005
#define core_cache_init                           *(volatile byte*)0x8006
#define core_cache_ctrl                           *(volatile byte*)0x8007
#define core_dpll48_div                           *(volatile byte*)0x8008
#define core_qspi_clk_sel                         *(volatile byte*)0x8009
#define core_sys_clksel                           *(volatile byte*)0x800a
#define core_mcu_clk_sel                          *(volatile byte*)0x800b
#define core_sys_event_lock                       *(volatile byte*)0x8011
#define core_sys_event_clr                        *(volatile byte*)0x8012
#define core_sys_status                           *(volatile byte*)0x8014
#define core_anaclk_en                            *(volatile byte*)0x8017
#define core_clken                                *(volatile uint*)0x8018
#define core_audio_clkctrl                        *(volatile byte*)0x801c
#define core_systick_ovwr_dat                     *(volatile uint*)0x8030
#define core_systick_ovwr_rtc_val                 *(volatile uint*)0x8034
#define core_systick_div1m                        *(volatile word*)0x8038
#define core_timer                                *(volatile uint*)0x803c
#define core_systick                              *(volatile uint*)0x8040
#define core_rtc_systick_sync_cmd                 *(volatile byte*)0x8044
#define core_sft_irq                              *(volatile byte*)0x8045
#define core_rtc_cnt_sync                         *(volatile uint*)0x8048
#define core_systick_sync                         *(volatile uint*)0x804c
#define core_ice_ctrl                             *(volatile byte*)0x8050
#define core_ice_reg                              *(volatile byte*)0x8051
#define core_ice_cmd                              *(volatile byte*)0x8052
#define core_ice_break0                           *(volatile uint*)0x8054
#define core_ice_break1                           *(volatile uint*)0x8058
#define core_ice_reg_wdata                        *(volatile uint*)0x805c
#define core_ice_reg_rdata                        *(volatile uint*)0x8060
#define core_trace_addr                           *(volatile uint*)0x8064
#define core_trace_fifo_rdata                     *(volatile uint*)0x8068
#define core_exe_addr                             *(volatile uint*)0x806c
#define core_ice_status                           *(volatile byte*)0x8070
#define core_debug_baud                           *(volatile byte*)0x8074
#define core_wdt_counter                          *(volatile byte*)0x8078
#define core_wdt_int_en                           *(volatile byte*)0x8079
#define core_io_rst_wdat                          *(volatile byte*)0x807a
#define core_io_rst_src_en                        *(volatile byte*)0x807b
#define core_sys_timer                            *(volatile uint*)0x8080
#define core_sys_timer_en                         *(volatile byte*)0x8084
#define core_sys_timer_period                     *(volatile uint*)0x8088
#define core_sysclk_reg                           *(volatile uint*)0x808c
#define core_sum_ctrl                             *(volatile byte*)0x8090
#define core_sardata                              *(volatile word*)0x8094
#define core_sumdata                              *(volatile word*)0x8098
#define core_otp_addr                             *(volatile word*)0x80a0
#define core_otp_ctrl                             *(volatile word*)0x80a2
#define core_otp_data                             *(volatile byte*)0x80a4
#define core_otp_status                           *(volatile byte*)0x80a5
#define core_otp_lock                             *(volatile byte*)0x80a8
#define core_bist_ctrl                            *(volatile byte*)0x80b0
#define core_bist_done                            *(volatile word*)0x80b4
#define core_bist_fail                            *(volatile word*)0x80b8
#define core_lpo_counter_cali                     *(volatile byte*)0x80c0
#define core_lpo_cali_cnt                         *(volatile uint*)0x80c4
#define core_lpocnt                               *(volatile byte*)0x80c8
#define core_gpi_counter_cali                     *(volatile byte*)0x80d0
#define core_gpi_cali_cnt                         *(volatile uint*)0x80d4
#define core_gpi_cali_sel                         *(volatile byte*)0x80d8
#define core_chgr_intr_en                         *(volatile byte*)0x80e0
#define core_chgr_intr_mod                        *(volatile byte*)0x80e4
#define core_sdma_desc                            *(volatile uint*)0x8100
#define core_sdma_clken                           *(volatile byte*)0x8104
#define core_sdma_start                           *(volatile byte*)0x811c
#define core_sdma_done                            *(volatile byte*)0x811d
#define core_sdma_desc_addr                       *(volatile uint*)0x8120
#define core_sdma_remain                          *(volatile uint*)0x8124
#define core_sdma_wr_addr                         *(volatile uint*)0x8128
#define core_sdma_rd_addr                         *(volatile uint*)0x812c
#define core_qspi_ctrl                            *(volatile uint*)0x8140
#define core_qspi_start                           *(volatile byte*)0x8144
#define core_qspi_done                            *(volatile byte*)0x8145
#define core_qspi_txlen                           *(volatile uint*)0x8148
#define core_qspi_txaddr                          *(volatile uint*)0x814c
#define core_qspi_rxlen                           *(volatile uint*)0x8150
#define core_qspi_rxaddr                          *(volatile uint*)0x8154
#define core_flash_offset                         *(volatile uint*)0x8158
#define core_para_offset                          *(volatile uint*)0x815c
#define core_iic_start                            *(volatile byte*)0x8180
#define core_iic_ctrl                             *(volatile byte*)0x8184
#define core_iic_delay(x)                         *(volatile byte*)(0x8188 + x)
#define core_iic_txlen                            *(volatile uint*)0x8190
#define core_iic_txaddr                           *(volatile uint*)0x8194
#define core_iic_rxaddr                           *(volatile uint*)0x8198
#define core_iic_rxlen                            *(volatile uint*)0x819c
#define core_iic_remain                           *(volatile uint*)0x81a0
#define core_iic_status                           *(volatile byte*)0x81a4
#define core_iic1_start                           *(volatile byte*)0x81c0
#define core_iic1_ctrl                            *(volatile byte*)0x81c4
#define core_iic1_delay(x)                        *(volatile byte*)(0x81c8 + x)
#define core_iic1_txlen                           *(volatile uint*)0x81d0
#define core_iic1_txaddr                          *(volatile uint*)0x81d4
#define core_iic1_rxaddr                          *(volatile uint*)0x81d8
#define core_iic1_rxlen                           *(volatile uint*)0x81dc
#define core_iic1_remain                          *(volatile uint*)0x81e0
#define core_iic1_status                          *(volatile byte*)0x81e4
#define core_uart_ctrl                            *(volatile uint*)0x8240
#define core_uart_rx_start_addr                   *(volatile uint*)0x8244
#define core_uart_rx_end_addr                     *(volatile uint*)0x8248
#define core_uart_tx_start_addr                   *(volatile uint*)0x824c
#define core_uart_tx_end_addr                     *(volatile uint*)0x8250
#define core_uart_tx_wptr                         *(volatile uint*)0x8254
#define core_uart_rx_rptr                         *(volatile uint*)0x8258
#define core_uart_baud_rate                       *(volatile word*)0x825c
#define core_uart_intr_clr                        *(volatile byte*)0x825e
#define core_uart_status                          *(volatile byte*)0x8260
#define core_uart_tx_rptr                         *(volatile uint*)0x8264
#define core_uart_txitems                         *(volatile uint*)0x8268
#define core_uart_rx_wptr                         *(volatile uint*)0x826c
#define core_uart_rxitems                         *(volatile uint*)0x8270
#define core_pwm_clken                            *(volatile byte*)0x8280
#define core_pwm_int_en                           *(volatile byte*)0x8282
#define core_pwm_int_status                       *(volatile byte*)0x8284
#define core_pwm0_ctrl                            *(volatile byte*)0x8288
#define core_pwm2_ctrl                            *(volatile byte*)0x8289
#define core_pwm4_ctrl                            *(volatile byte*)0x828a
#define core_pwm6_ctrl                            *(volatile byte*)0x828b
#define core_pwm1_ctrl                            *(volatile byte*)0x828c
#define core_pwm3_ctrl                            *(volatile byte*)0x828d
#define core_pwm5_ctrl                            *(volatile byte*)0x828e
#define core_pwm7_ctrl                            *(volatile byte*)0x828f
#define core_pwm0_pcnt                            *(volatile word*)0x8290
#define core_pwm0_ncnt                            *(volatile word*)0x8292
#define core_pwm1_pcnt                            *(volatile word*)0x8298
#define core_pwm1_ncnt                            *(volatile word*)0x829a
#define core_pwm2_pcnt                            *(volatile word*)0x82a0
#define core_pwm2_ncnt                            *(volatile word*)0x82a2
#define core_pwm3_pcnt                            *(volatile word*)0x82a8
#define core_pwm3_ncnt                            *(volatile word*)0x82aa
#define core_pwm4_pcnt                            *(volatile word*)0x82b0
#define core_pwm4_ncnt                            *(volatile word*)0x82b2
#define core_pwm5_pcnt                            *(volatile word*)0x82b8
#define core_pwm5_ncnt                            *(volatile word*)0x82ba
#define core_pwm6_pcnt                            *(volatile word*)0x82c0
#define core_pwm6_ncnt                            *(volatile word*)0x82c2
#define core_pwm7_pcnt                            *(volatile word*)0x82c8
#define core_pwm7_ncnt                            *(volatile word*)0x82ca
#define core_pwm0_cnt                             *(volatile word*)0x82cc
#define core_pwm1_cnt                             *(volatile word*)0x82ce
#define core_pwm2_cnt                             *(volatile word*)0x82d0
#define core_pwm3_cnt                             *(volatile word*)0x82d2
#define core_pwm4_cnt                             *(volatile word*)0x82d4
#define core_pwm5_cnt                             *(volatile word*)0x82d6
#define core_pwm6_cnt                             *(volatile word*)0x82d8
#define core_pwm7_cnt                             *(volatile word*)0x82da
#define core_gpio_cfg(x)                          *(volatile byte*)(0x8300 + x)
#define core_gpio_ae(x)                           *(volatile byte*)(0x8338 + x)
#define core_gpio_in(x)                           *(volatile byte*)(0x8340 + x)
#define core_gpio_ds(x)                           *(volatile byte*)(0x8348 + x)
#define core_gpio_ien(x)                          *(volatile byte*)(0x8358 + x)
#define core_gpio_ts(x)                           *(volatile byte*)(0x8360 + x)
#define core_gpio_isr(x)                          *(volatile byte*)(0x8370 + x)
#define core_qspi1_ctrl                           *(volatile word*)0x8380
#define core_qspi1_start                          *(volatile byte*)0x8384
#define core_qspi1_status                         *(volatile byte*)0x8385
#define core_qspi1_txlen                          *(volatile uint*)0x8388
#define core_qspi1_txaddr                         *(volatile uint*)0x838c
#define core_qspi1_rxlen                          *(volatile uint*)0x8390
#define core_qspi1_rxaddr                         *(volatile uint*)0x8394
#define core_qspi1_cmdlen                         *(volatile byte*)0x839b
#define core_qspi1_cmdaddr                        *(volatile uint*)0x8398
#define core_qspi2_ctrl                           *(volatile word*)0x83c0
#define core_qspi2_start                          *(volatile byte*)0x83c4
#define core_qspi2_status                         *(volatile byte*)0x83c5
#define core_qspi2_txlen                          *(volatile uint*)0x83c8
#define core_qspi2_txaddr                         *(volatile uint*)0x83cc
#define core_qspi2_rxlen                          *(volatile uint*)0x83d0
#define core_qspi2_rxaddr                         *(volatile uint*)0x83d4
#define core_qspi2_cmdlen                         *(volatile byte*)0x83db
#define core_qspi2_cmdaddr                        *(volatile uint*)0x83d8
#define core_bt_cmd                               *(volatile byte*)0x8400
#define core_bt_ctrl                              *(volatile byte*)0x8401
#define core_bt_status                            *(volatile word*)0x8402
#define core_bt_desc_ptr                          *(volatile uint*)0x8404
#define core_bt_pptr                              *(volatile uint*)0x8408
#define core_bt_stack_base                        *(volatile uint*)0x840c
#define core_bt_white                             *(volatile byte*)0x8410
#define core_bt_seq                               *(volatile byte*)0x8411
#define core_bt_desc                              *(volatile word*)0x8412
#define core_bt_crc24                             *(volatile uint*)0x8414
#define core_access_code                          *(volatile uint*)0x8418
#define core_bt_bits                              *(volatile uint*)0x841c
#define core_pkt_len                              *(volatile byte*)0x8420
#define core_rand_num                             *(volatile word*)0x8424
#define core_bt_ch                                *(volatile byte*)0x8426
#define core_bt_mapch                             *(volatile byte*)0x8427
#define core_bt_map(x)                            *(volatile byte*)(0x8428 + x)
#define core_bt_hop                               *(volatile byte*)0x8430
#define core_bt_used_ch                           *(volatile byte*)0x8431
#define core_bt_hopinc                            *(volatile byte*)0x8432
#define core_rssi_cap                             *(volatile byte*)0x8434
#define core_sync_ts                              *(volatile uint*)0x8438
#define core_pend_ts                              *(volatile uint*)0x843c
#define core_nec_startaddr                        *(volatile uint*)0x8440
#define core_nec_endaddr                          *(volatile uint*)0x8444
#define core_nec_rptr                             *(volatile uint*)0x8448
#define core_nec_ctrl                             *(volatile word*)0x844c
#define core_nec_mode                             *(volatile byte*)0x844e
#define core_nec_intr_en                          *(volatile byte*)0x844f
#define core_nec_wptr                             *(volatile uint*)0x8450
#define core_nec_items                            *(volatile uint*)0x8454
#define core_nec_status                           *(volatile byte*)0x8458
#define core_aes_ctrl                             *(volatile byte*)0x8480
#define core_aes_data_len                         *(volatile byte*)0x8481
#define core_aes_cmd                              *(volatile byte*)0x8482
#define core_aes_status                           *(volatile byte*)0x8483
#define core_aes_key_addr                         *(volatile uint*)0x8484
#define core_aes_data_addr                        *(volatile uint*)0x8488
#define core_aes_xor_addr                         *(volatile uint*)0x848c
#define core_aes_ccm_addr                         *(volatile uint*)0x8490
#define core_aes_sdata_len                        *(volatile byte*)0x8494
#define core_aes_mic_result(x)                    *(volatile uint*)(0x84a0 + (x)*4)
#define core_aes_result(x)                        *(volatile uint*)(0x84b0 + (x)*4)
#define core_bistdis                              *(volatile byte*)0x84c0
#define core_bistfix_en                           *(volatile byte*)0x84c4
#define core_set_bistfix                          *(volatile byte*)0x84c8
#define core_bistfix_val                          *(volatile uint*)0x84cc
#define core_bist_result_mram0                    *(volatile uint*)0x84d0
#define core_bist_result_mram1                    *(volatile uint*)0x84d4
#define core_bist_result_mram2                    *(volatile uint*)0x84d8
#define core_bist_result_mram3                    *(volatile uint*)0x84dc
#define core_bist_result_mram4                    *(volatile uint*)0x84e0
#define core_uartc_ctrl                           *(volatile uint*)0x8500
#define core_uartc_rx_start_addr                  *(volatile uint*)0x8504
#define core_uartc_rx_end_addr                    *(volatile uint*)0x8508
#define core_uartc_tx_start_addr                  *(volatile uint*)0x850c
#define core_uartc_tx_end_addr                    *(volatile uint*)0x8510
#define core_uartc_tx_wptr                        *(volatile uint*)0x8514
#define core_uartc_rx_rptr                        *(volatile uint*)0x8518
#define core_uartc_baud_rate                      *(volatile word*)0x851c
#define core_uartc_intr_clr                       *(volatile byte*)0x851e
#define core_uartc_status                         *(volatile byte*)0x8520
#define core_uartc_tx_rptr                        *(volatile uint*)0x8524
#define core_uartc_txitems                        *(volatile uint*)0x8528
#define core_uartc_rx_wptr                        *(volatile uint*)0x852c
#define core_uartc_rxitems                        *(volatile uint*)0x8530
#define core_uartd_ctrl                           *(volatile uint*)0x8540
#define core_uartd_rx_start_addr                  *(volatile uint*)0x8544
#define core_uartd_rx_end_addr                    *(volatile uint*)0x8548
#define core_uartd_tx_start_addr                  *(volatile uint*)0x854c
#define core_uartd_tx_end_addr                    *(volatile uint*)0x8550
#define core_uartd_tx_wptr                        *(volatile uint*)0x8554
#define core_uartd_rx_rptr                        *(volatile uint*)0x8558
#define core_uartd_baud_rate                      *(volatile word*)0x855c
#define core_uartd_intr_clr                       *(volatile byte*)0x855e
#define core_uartd_status                         *(volatile byte*)0x8560
#define core_uartd_tx_rptr                        *(volatile uint*)0x8564
#define core_uartd_txitems                        *(volatile uint*)0x8568
#define core_uartd_rx_wptr                        *(volatile uint*)0x856c
#define core_uartd_rxitems                        *(volatile uint*)0x8570
#define core_usb_config(x)                        *(volatile byte*)(0x8600 + x)
#define core_usb_irq_mask                         *(volatile word*)0x8608
#define core_usb_epmap                            *(volatile word*)0x861c
#define core_usb_trig                             *(volatile uint*)0x8620
#define core_usb_ep_busy                          *(volatile byte*)0x8624
#define core_usb_in_miss                          *(volatile byte*)0x8626
#define core_usb_rx_wptr                          *(volatile uint*)0x8628
#define core_usb_irq_status                       *(volatile word*)0x862c
#define core_usb_status                           *(volatile word*)0x8630
#define core_usb_clkcnt                           *(volatile word*)0x8634
#define core_usb_sof_cnt                          *(volatile word*)0x8638
#define core_usb_rxcnt                            *(volatile byte*)0x863c
#define core_usb_rx_saddr                         *(volatile uint*)0x8640
#define core_usb_rx_buflen                        *(volatile uint*)0x8644
#define core_usb_rxptr                            *(volatile uint*)0x8648
#define core_usb_tx_saddr0                        *(volatile uint*)0x864c
#define core_usb_tx_saddr1                        *(volatile uint*)0x8650
#define core_usb_tx_saddr2                        *(volatile uint*)0x8654
#define core_usb_tx_saddr3                        *(volatile uint*)0x8658
#define core_uartb_ctrl                           *(volatile uint*)0x8680
#define core_uartb_rx_start_addr                  *(volatile uint*)0x8684
#define core_uartb_rx_end_addr                    *(volatile uint*)0x8688
#define core_uartb_tx_start_addr                  *(volatile uint*)0x868c
#define core_uartb_tx_end_addr                    *(volatile uint*)0x8690
#define core_uartb_tx_wptr                        *(volatile uint*)0x8694
#define core_uartb_rx_rptr                        *(volatile uint*)0x8698
#define core_uartb_baud_rate                      *(volatile word*)0x869c
#define core_uartb_intr_clr                       *(volatile byte*)0x869e
#define core_uartb_status                         *(volatile byte*)0x86a0
#define core_uartb_tx_rptr                        *(volatile uint*)0x86a4
#define core_uartb_txitems                        *(volatile uint*)0x86a8
#define core_uartb_rx_wptr                        *(volatile uint*)0x86ac
#define core_uartb_rxitems                        *(volatile uint*)0x86b0
#define core_adcd_dcoff                           *(volatile word*)0x8700
#define core_adcd_delay                           *(volatile byte*)0x8702
#define core_adcd_ctrl                            *(volatile byte*)0x8703
#define core_adcd_saddr                           *(volatile uint*)0x8704
#define core_adcd_buflen                          *(volatile word*)0x8708
#define core_adcd_wptr                            *(volatile uint*)0x870c
#define core_adcd_mic_hpf_alpha                   *(volatile word*)0x8710
#define core_adcd_fir_ctrl                        *(volatile byte*)0x8712
#define core_adcd_mic_ctrl                        *(volatile byte*)0x8713
#define core_adcd_dc_ctrl                         *(volatile byte*)0x8714
#define core_adcd_vol_ctrl                        *(volatile uint*)0x8718
#define core_adcd_avg_power                       *(volatile word*)0x871c
#define core_adcd_avg_dc                          *(volatile word*)0x871e
#define core_adcd_vof_wr_strobe                   *(volatile byte*)0x8722
#define core_adcd_vof_coef_wdata                  *(volatile uint*)0x8724
#define core_adcd_dsmdiv_ctrl                     *(volatile byte*)0x8730
#define core_adcd_dsmdiv_int                      *(volatile byte*)0x8737
#define core_adcd_dsmdiv_frac                     *(volatile uint*)0x8734
#define modem_tx_clken                             *(volatile byte*)0x8900
#define modem_ble_en                               *(volatile byte*)0x8901
#define modem_tx_clk_inv                           *(volatile byte*)0x8902
#define modem_dc_offset_low                        *(volatile byte*)0x8903
#define modem_dc_offset_high                       *(volatile byte*)0x8904
#define modem_hp_gain_low                          *(volatile byte*)0x8905
#define modem_hp_gain_high                         *(volatile byte*)0x8906
#define modem_lp_gain_low                          *(volatile byte*)0x8907
#define modem_lp_gain_high                         *(volatile byte*)0x8908
#define modem_fbdiv_intn                           *(volatile word*)0x8909
#define modem_mod_delay_cnt                        *(volatile byte*)0x890b
#define modem_fbdiv_frac                           *(volatile uint*)0x890c
#define modem_lp_phsel                             *(volatile byte*)0x890f
#define modem_if_sel                               *(volatile byte*)0x8920
#define modem_rx_i_inv                             *(volatile byte*)0x8921
#define modem_demod_iqcali_c21                     *(volatile byte*)0x8922
#define modem_demod_iqcali_c22_low                 *(volatile byte*)0x8923
#define modem_demod_iqcali_c22_high                *(volatile byte*)0x8924
#define modem_sync_win                             *(volatile byte*)0x8925
#define modem_sync_err_thld                        *(volatile byte*)0x8930
#define modem_sync_corr_thld_low                   *(volatile byte*)0x8931
#define modem_sync_corr_thld_high                  *(volatile byte*)0x8932
#define modem_freq_comp                            *(volatile byte*)0x8937
#define modem_h0_reg_low                           *(volatile byte*)0x8938
#define modem_h0_reg_high                          *(volatile byte*)0x8939
#define modem_h1_reg_low                           *(volatile byte*)0x893a
#define modem_h1_reg_high                          *(volatile byte*)0x893b
#define modem_h2_reg_low                           *(volatile byte*)0x893c
#define modem_h2_reg_high                          *(volatile byte*)0x893d
#define modem_dcc_threshold_low                    *(volatile byte*)0x8940
#define modem_dcc_threshold_high                   *(volatile byte*)0x8941
#define modem_dcclen1_threshold_low                *(volatile byte*)0x8942
#define modem_dcclen1_threshold_high               *(volatile byte*)0x8943
#define modem_dcclen2_threshold_low                *(volatile byte*)0x8944
#define modem_dcclen2_threshold_high               *(volatile byte*)0x8945
#define modem_loop_filt_coef                       *(volatile byte*)0x8946
#define modem_agc_disable                          *(volatile byte*)0x8960
#define modem_lna_idx_fix                          *(volatile byte*)0x8961
#define modem_agc_settletime                       *(volatile byte*)0x8962
#define modem_agc_timeout_low                      *(volatile byte*)0x8963
#define modem_agc_timeout_high                     *(volatile byte*)0x8964
#define modem_powdet_timeout_low                   *(volatile byte*)0x8965
#define modem_powdet_timeout_high                  *(volatile byte*)0x8966
#define modem_powdet_th                            *(volatile byte*)0x8967
#define modem_wb_sat_th                            *(volatile byte*)0x8968
#define modem_nb_sat_th                            *(volatile byte*)0x8969
#define modem_wb_gain1                             *(volatile byte*)0x896a
#define modem_wb_gain_dec                          *(volatile byte*)0x896b
#define modem_nb_gain_dec                          *(volatile byte*)0x896c
#define modem_wb_lowth                             *(volatile byte*)0x896d
#define modem_nb_lowth                             *(volatile byte*)0x896e
#define modem_agc_lock_timeout_low                 *(volatile byte*)0x896f
#define modem_agc_lock_timeout_high                *(volatile byte*)0x8970
#define modem_wb_delta_th                          *(volatile byte*)0x8971
#define modem_nb_delta_th                          *(volatile byte*)0x8972
#define modem_aci_detth                            *(volatile byte*)0x8973
#define modem_aci_wb_detth                         *(volatile byte*)0x8974
#define modem_delta_wb_bo                          *(volatile byte*)0x8975
#define modem_nb_dccalc_en                         *(volatile byte*)0x8976
#define modem_wb_dc_iin                            *(volatile byte*)0x8977
#define modem_wb_dc_qin                            *(volatile byte*)0x8978
#define modem_nb_dc_iin_low                        *(volatile byte*)0x8979
#define modem_nb_dc_iin_high                       *(volatile byte*)0x897a
#define modem_nb_dc_qin_low                        *(volatile byte*)0x897b
#define modem_nb_dc_qin_high                       *(volatile byte*)0x897c
#define modem_sec_wb_sat_th                        *(volatile byte*)0x897d
#define modem_sec_nb_sat_th                        *(volatile byte*)0x897e
#define modem_gain_abb_max                         *(volatile byte*)0x897f
#define modem_dagc_cap_enable                      *(volatile byte*)0x8980
#define modem_btdma_clken                          *(volatile byte*)0x8981
#define modem_btdma_saddr_low                      *(volatile byte*)0x8982
#define modem_btdma_saddr_high                     *(volatile byte*)0x8983
#define modem_btdma_eaddr_low                      *(volatile byte*)0x8984
#define modem_btdma_eaddr_high                     *(volatile byte*)0x8985
#define modem_rssi                                 *(volatile byte*)0x89a0
#define modem_wb_pow                               *(volatile byte*)0x89a1
#define modem_nb_pow                               *(volatile byte*)0x89a2
#define modem_lna_gain                             *(volatile byte*)0x89a3
#define modem_pkt_det                              *(volatile byte*)0x89a4
#define modem_wb_i_dc                              *(volatile byte*)0x89a5
#define modem_wb_q_dc                              *(volatile byte*)0x89a6
#define modem_nb_i_dc                              *(volatile word*)0x89a8
#define modem_nb_q_dc                              *(volatile word*)0x89aa
#define modem_gain_switch                          *(volatile byte*)0x89ac
#define rf_da_bbpll_ring_en                     *(volatile byte*)0x8a00
#define rf_da_bbpll_ck2sar_48m_en               *(volatile byte*)0x8a01
#define rf_da_bbpll_fbdiv_intn                  *(volatile byte*)0x8a02
#define rf_da_bbpll_fbdiv_frac                  *(volatile uint*)0x8a04
#define rf_rg_bbpll_sdm_mash_order_sel          *(volatile byte*)0x8a08
#define rf_rg_bbpll_ring_ib_sel                 *(volatile byte*)0x8a09
#define rf_rg_bbpll_lpf_c2_sel                  *(volatile byte*)0x8a0a
#define rf_rg_bbpll_lpf_r3_sel                  *(volatile byte*)0x8a0b
#define rf_rg_bbpll_test_dc_sel                 *(volatile byte*)0x8a0c
#define rf_rg_bbpll_pfd_pres_sel                *(volatile byte*)0x8a0d
#define rf_rg_bbpll_fbdiv_pw_sel                *(volatile byte*)0x8a0e
#define rf_da_gpadc_sar_ldo_trim                *(volatile byte*)0x8a30
#define rf_rg_gpadc_sar_cmp_isel                *(volatile byte*)0x8a31
#define rf_rg_gpadc_sar_datin                   *(volatile byte*)0x8a32
#define rf_rg_gpadc_sar_vcm_isel                *(volatile byte*)0x8a33
#define rf_rg_gpadc_sar_vref_sel                *(volatile byte*)0x8a34
#define rf_ad_gpadc_cal_dout                    *(volatile byte*)0x8a35
#define rf_rg_gpadc_dcpath_div2                 *(volatile byte*)0x8a36
#define rf_rg_test_vdc_en                       *(volatile byte*)0x8a38
#define rf_rg_test_vrfldo_sel                   *(volatile byte*)0x8a39
#define rf_rg_test_vdc_xtalvdc_en               *(volatile byte*)0x8a3a
#define rf_da_ldo_pabias_en                     *(volatile byte*)0x8a40
#define rf_da_ldo_logen_ibleed                  *(volatile byte*)0x8a41
#define rf_da_ldo_pabias_ibleed                 *(volatile byte*)0x8a42
#define rf_da_ldo_vco_ibleed                    *(volatile byte*)0x8a43
#define rf_rg_ldo_logen_vctrl                   *(volatile byte*)0x8a44
#define rf_rg_ldo_pa_vctrl                      *(volatile byte*)0x8a45
#define rf_rg_ldo_pabias_vctrl                  *(volatile byte*)0x8a46
#define rf_rg_rffe_matchcap_csel                *(volatile byte*)0x8a47
#define rf_da_rfpll_cal_finish_clr              *(volatile byte*)0x8a48
#define rf_da_rfpll_afc_ncount_target           *(volatile uint*)0x8a4c
#define rf_da_rfpll_vco_en                      *(volatile byte*)0x8a50
#define rf_da_rfpll_dac_lpf_c1sel               *(volatile byte*)0x8a51
#define rf_da_rx_iqadc_en                       *(volatile byte*)0x8a52
#define rf_da_tx_pada_en                        *(volatile byte*)0x8a53
#define rf_rx_abb_dcoc_force_en                 *(volatile byte*)0x8a54
#define rf_rx_abb_gc_force                      *(volatile byte*)0x8a55
#define rf_rx_lna_lg_isel_force                 *(volatile byte*)0x8a56
#define rf_rx_lna_hg_isel_gc7                   *(volatile byte*)0x8a58
#define rf_rx_lna_hg_isel_gc6                   *(volatile byte*)0x8a59
#define rf_rx_lna_hg_isel_gc5                   *(volatile byte*)0x8a5a
#define rf_rx_lna_lg_isel_gc4                   *(volatile byte*)0x8a5b
#define rf_rx_lna_lg_isel_gc3                   *(volatile byte*)0x8a5c
#define rf_rx_lna_lg_isel_gc2                   *(volatile byte*)0x8a5d
#define rf_rx_lna_lg_isel_gc1                   *(volatile byte*)0x8a5e
#define rf_rx_lna_lg_isel_gc0                   *(volatile byte*)0x8a5f
#define rf_rx_abb_gc_gc8                        *(volatile byte*)0x8a60
#define rf_rx_abb_gc_gc7                        *(volatile byte*)0x8a61
#define rf_rx_abb_gc_gc6                        *(volatile byte*)0x8a62
#define rf_rx_abb_gc_gc5                        *(volatile byte*)0x8a63
#define rf_rx_abb_gc_gc4                        *(volatile byte*)0x8a64
#define rf_rx_abb_gc_gc3                        *(volatile byte*)0x8a65
#define rf_rx_abb_gc_gc2                        *(volatile byte*)0x8a66
#define rf_rx_abb_gc_gc1                        *(volatile byte*)0x8a67
#define rf_rx_abb_gc_gc0                        *(volatile byte*)0x8a68
#define rf_da_tx_pa_gc                          *(volatile byte*)0x8a6c
#define rf_da_tx_pa_ibcore_sel                  *(volatile byte*)0x8a6d
#define rf_rg_tx_pada_rldsel                    *(volatile byte*)0x8a6e
#define rf_da_tx_pada_ibsel                     *(volatile byte*)0x8a6f
#define rf_rg_rfpll_vco_pkd_vth_sel             *(volatile byte*)0x8a70
#define rf_rg_rfpll_vco_res_sel_init            *(volatile byte*)0x8a71
#define rf_rg_rfpll_vco_cap_force_val           *(volatile byte*)0x8a72
#define rf_rg_rfpll_afc_vco_stable_time         *(volatile byte*)0x8a73
#define rf_rg_rfpll_aac_vco_stable_time         *(volatile byte*)0x8a74
#define rf_rg_rfpll_ghpc_dac_stable_time        *(volatile byte*)0x8a75
#define rf_rg_rfpll_dac_code_force_en           *(volatile byte*)0x8a76
#define rf_rg_rfpll_cntctrl_start_force_val     *(volatile byte*)0x8a77
#define rf_rg_rfpll_ghpc_dac_code_fhi           *(volatile word*)0x8a78
#define rf_rg_rfpll_ghpc_dac_code_flo           *(volatile word*)0x8a7a
#define rf_rg_rfpll_dac_code_force_val          *(volatile word*)0x8a7c
#define rf_rg_rfpll_afc_data_sync_force_val     *(volatile byte*)0x8a7e
#define rf_ad_rfpll_cal_result_data             *(volatile uint*)0x8a80
#define rf_rg_rfpll_cp_ioffsel                  *(volatile byte*)0x8a84
#define rf_rg_rfpll_dac_lpf_r1sel               *(volatile byte*)0x8a85
#define rf_rg_rfpll_dac_rload_sel               *(volatile byte*)0x8a86
#define rf_rg_rfpll_lpf_c3_sel                  *(volatile byte*)0x8a87
#define rf_rg_rfpll_vco_kv_sel                  *(volatile byte*)0x8a88
#define rf_rg_rfpll_fbdiv_pres_sel              *(volatile byte*)0x8a89
#define rf_rg_rfpll_test_dc_sel                 *(volatile byte*)0x8a8a
#define rf_rg_rfpll_sdm_cksel                   *(volatile byte*)0x8a8b
#define rf_rg_rx_lna_ib_tc_sel                  *(volatile byte*)0x8a8c
#define rf_rg_rx_lna_hg_vbpcas_sel              *(volatile byte*)0x8a8d
#define rf_rg_rx_tia_vcm_sel                    *(volatile byte*)0x8a8e
#define rf_rg_rx_tia_csel                       *(volatile byte*)0x8a8f
#define rf_rg_rx_tia_ib2nd_sel                  *(volatile byte*)0x8a90
#define rf_rg_rx_abb_cap_sel                    *(volatile byte*)0x8a91
#define rf_rg_rx_abb_rcross_sel                 *(volatile byte*)0x8a92
#define rf_rg_rx_abb_ib2nd_sel                  *(volatile byte*)0x8a93
#define rf_rg_rx_iqadc_ck_div2_en               *(volatile byte*)0x8a94
#define rf_rx_tiai_dcoc_gc0                     *(volatile byte*)0x8a98
#define rf_rx_tiaq_dcoc_gc0                     *(volatile byte*)0x8a99
#define rf_rx_tiai_dcoc_gc1                     *(volatile byte*)0x8a9a
#define rf_rx_tiaq_dcoc_gc1                     *(volatile byte*)0x8a9b
#define rf_rx_tiai_dcoc_gc2                     *(volatile byte*)0x8a9c
#define rf_rx_tiaq_dcoc_gc2                     *(volatile byte*)0x8a9d
#define rf_rx_tiai_dcoc_gc3                     *(volatile byte*)0x8a9e
#define rf_rx_tiaq_dcoc_gc3                     *(volatile byte*)0x8a9f
#define rf_rx_tiai_dcoc_gc4                     *(volatile byte*)0x8aa0
#define rf_rx_tiaq_dcoc_gc4                     *(volatile byte*)0x8aa1
#define rf_rx_tiai_dcoc_gc5                     *(volatile byte*)0x8aa2
#define rf_rx_tiaq_dcoc_gc5                     *(volatile byte*)0x8aa3
#define rf_rx_tiai_dcoc_gc6                     *(volatile byte*)0x8aa4
#define rf_rx_tiaq_dcoc_gc6                     *(volatile byte*)0x8aa5
#define rf_rx_tiai_dcoc_gc7                     *(volatile byte*)0x8aa6
#define rf_rx_tiaq_dcoc_gc7                     *(volatile byte*)0x8aa7
#define rf_rx_abbi_dcoc_gc0                     *(volatile byte*)0x8aa8
#define rf_rx_abbq_dcoc_gc0                     *(volatile byte*)0x8aa9
#define rf_rx_abbi_dcoc_gc1                     *(volatile byte*)0x8aaa
#define rf_rx_abbq_dcoc_gc1                     *(volatile byte*)0x8aab
#define rf_rx_abbi_dcoc_gc2                     *(volatile byte*)0x8aac
#define rf_rx_abbq_dcoc_gc2                     *(volatile byte*)0x8aad
#define rf_rx_abbi_dcoc_gc3                     *(volatile byte*)0x8aae
#define rf_rx_abbq_dcoc_gc3                     *(volatile byte*)0x8aaf
#define rf_rx_abbi_dcoc_gc4                     *(volatile byte*)0x8ab0
#define rf_rx_abbq_dcoc_gc4                     *(volatile byte*)0x8ab1
#define rf_rx_abbi_dcoc_gc5                     *(volatile byte*)0x8ab2
#define rf_rx_abbq_dcoc_gc5                     *(volatile byte*)0x8ab3
#define rf_rx_abbi_dcoc_gc6                     *(volatile byte*)0x8ab4
#define rf_rx_abbq_dcoc_gc6                     *(volatile byte*)0x8ab5
#define rf_rx_abbi_dcoc_gc7                     *(volatile byte*)0x8ab6
#define rf_rx_abbq_dcoc_gc7                     *(volatile byte*)0x8ab7
#define rf_rx_abbi_dcoc_gc8                     *(volatile byte*)0x8ab8
#define rf_rx_abbq_dcoc_gc8                     *(volatile byte*)0x8ab9
#define rf_rx_tiai_dcoc_force                   *(volatile byte*)0x8abc
#define rf_rx_tiaq_dcoc_force                   *(volatile byte*)0x8abd
#define rf_rx_abbi_dcoc_force                   *(volatile byte*)0x8abe
#define rf_rx_abbq_dcoc_force                   *(volatile byte*)0x8abf
#define rf_da_chgpump_ldo18_en                  *(volatile byte*)0x8ac0
#define rf_rg_chgpump_ldo25                     *(volatile byte*)0x8ac1
#define rf_da_chgpump_rco_csel                  *(volatile byte*)0x8ac2
#define rf_rg_chgpump_ldo18_vsel                *(volatile byte*)0x8ac3
#define rf_da_rco384m_en                        *(volatile byte*)0x8acc
#define rf_rg_rco384m_icorse_sel                *(volatile byte*)0x8acd
#define rf_rg_rco384m_ifine_sel                 *(volatile byte*)0x8ace
#define aadc_da_micbias_en                        *(volatile byte*)0x8b10
#define aadc_rg_mic_hvreg_vc                      *(volatile byte*)0x8b14
#define aadc_rg_pga_opa2_ibias_sel                *(volatile byte*)0x8b18
#define aadc_da_vmid_bg_en                        *(volatile byte*)0x8b1c
#define dadc_sadc0_en                             *(volatile byte*)0x8b80
#define dadc_sadc0_clk_div                        *(volatile byte*)0x8b84
#define dadc_sadc0_rlist                          *(volatile word*)0x8b88
#define dadc_sadc0_trig_ch                        *(volatile word*)0x8b8a
#define dadc_sadc0_rcr_start                      *(volatile byte*)0x8b8c
#define dadc_sadc0_icr_start                      *(volatile byte*)0x8b8d
#define dadc_sadc0_ctrl                           *(volatile word*)0x8b8e
#define dadc_sadc0_wd                             *(volatile byte*)0x8b90
#define dadc_sadc0_wdht                           *(volatile word*)0x8b92
#define dadc_sadc0_wdlt                           *(volatile word*)0x8b94
#define dadc_sadc0_rsaddr                         *(volatile uint*)0x8b98
#define dadc_sadc0_readdr                         *(volatile uint*)0x8b9c
#define dadc_sadc0_ieaddr                         *(volatile uint*)0x8ba0
#define dadc_sadc0_rwaddr                         *(volatile uint*)0x8ba4
#define dadc_sadc0_iwaddr                         *(volatile uint*)0x8ba8
#define dadc_sadc0_sampt0                         *(volatile byte*)0x8bac
#define dadc_sadc0_status                         *(volatile byte*)0x8bb0
#define dadc_sadc0_int_status                     *(volatile byte*)0x8bb1
#define dadc_sadc0_int_en                         *(volatile byte*)0x8bb2
#define dadc_sadc0_int_clr                        *(volatile byte*)0x8bb3
#define dadc_sadc0_active_list_lo                 *(volatile word*)0x8bb8
#define dadc_sadc0_active_idx_hi                  *(volatile byte*)0x8bba
#define dadc_sadc0_active_idx                     *(volatile byte*)0x8bbb
#define dadc_sadc0_rdat                           *(volatile word*)0x8bc0
#define dadc_sadc0_idat                           *(volatile word*)0x8bc2
#define rv_break_rv_brkn                              *(volatile byte*)0x8e80
#define rv_break_rv_break0                            *(volatile uint*)0x8e84
#define rv_break_rv_break1                            *(volatile uint*)0x8e88
#define rv_break_rv_break2                            *(volatile uint*)0x8e8c
#define rv_break_rv_break3                            *(volatile uint*)0x8e90
#define rv_break_rv_break4                            *(volatile uint*)0x8e94
#define rv_break_rv_break5                            *(volatile uint*)0x8e98
#define rv_break_rv_break6                            *(volatile uint*)0x8e9c
#define rv_break_rv_break7                            *(volatile uint*)0x8ea0








#define lpm_enon_ctrl                       *(volatile byte*)0x9f00
#define lpm_buck_oe                         *(volatile byte*)0x9f04
#define lpm_digldo_cnt                      *(volatile byte*)0x9f05
#define lpm_xtal_cnt                        *(volatile byte*)0x9f06
#define lpm_exen_cnt                        *(volatile byte*)0x9f07
#define lpm_ice_mode                        *(volatile byte*)0x9f08
#define lpm_go2sleep_wr                     *(volatile byte*)0x9f09
#define lpm_lpm_wreg                        *(volatile uint*)0x9f0c
#define lpm_icache_retry_num                *(volatile byte*)0x9f11
#define lpm_host_wakeup                     *(volatile byte*)0x9f12
#define lpm_ucode_flag                      *(volatile byte*)0x9f14
#define lpm_peripheral_lock                 *(volatile byte*)0x9f15
#define lpm_scan_mode_en                    *(volatile byte*)0x9f16
#define lpm_sw_used                         *(volatile byte*)0x9f17
#define lpm_lpm_pc                          *(volatile uint*)0x9f18
#define lpm_lpm_sp                          *(volatile word*)0x9f1c
#define lpm_puchk                           *(volatile byte*)0x9f20
#define lpm_key_det                         *(volatile word*)0x9f22
#define lpm_gpio_wake_cfg(x)                *(volatile uint*)(0x9f24 + (x)*5)
#define lpm_rtc_ov_en                       *(volatile byte*)0x9f50
#define lpm_rtc_ovwr_cmd                    *(volatile byte*)0x9f51
#define lpm_rtc_comp0                       *(volatile uint*)0x9f54
#define lpm_rtc_comp1                       *(volatile uint*)0x9f58
#define lpm_rtc_cnt                         *(volatile uint*)0x9f5c
#define lpm_code_key(x)                     *(volatile uint*)(0x9f60 + (x)*2)
#define lpm_ramr_isolation                  *(volatile byte*)0x9f70
#define lpm_lcd_ctrl                        *(volatile word*)0x9f74
#define lpm_lcd_ram0                        *(volatile uint*)0x9f78
#define lpm_lcd_ram1                        *(volatile uint*)0x9f7c
#define lpm_lcd_ram2                        *(volatile uint*)0x9f80
#define lpm_lcd_ram3                        *(volatile uint*)0x9f84
#define lpm_lcd_ram4                        *(volatile uint*)0x9f88
#define lpm_gpio_lcd(x)                     *(volatile byte*)(0x9f8c + x)
#define lpm_da_lpm_xtal_cgm_en_on           *(volatile byte*)0x9fc0
#define lpm_da_lpm_xtal_cgm_en              *(volatile byte*)0x9fc1
#define lpm_da_lpm_dvdd_retmem_en           *(volatile byte*)0x9fc2
#define lpm_rg_lpm_ldo_dig_lpm_vosel        *(volatile byte*)0x9fc3
#define lpm_rg_lpm_ldo_dig_lpm_vrefsel      *(volatile byte*)0x9fc4
#define lpm_da_lpm_ldo_0p33vdd_aon_bypb     *(volatile byte*)0x9fc5
#define lpm_da_lpm_ldo_dig_core_vsel        *(volatile byte*)0x9fc6
#define lpm_da_lpm_ldo_dig_core_czsel       *(volatile byte*)0x9fc7
#define lpm_rg_lpm_ldo_d2aiso_vsel          *(volatile byte*)0x9fc8
#define lpm_rg_lpm_rco32m_csel              *(volatile byte*)0x9fc9
#define lpm_da_lpm_xtal_cmom_sel            *(volatile byte*)0x9fca
#define lpm_da_lpm_xtal_ck2bbpll_en         *(volatile byte*)0x9fcb
#define lpm_rg_lpm_lpo32k_freq              *(volatile byte*)0x9fcc
#define lpm_rg_lpm_xtal_ldo_vref_sel        *(volatile byte*)0x9fcd
#define lpm_rg_lpm_buck_bypass_th_sel       *(volatile byte*)0x9fd0
#define lpm_rg_lpm_buck_fb_sel              *(volatile byte*)0x9fd1
#define lpm_rg_lpm_buck_icut_comp_en        *(volatile byte*)0x9fd2
#define lpm_rg_lpm_buck_pfm_disen           *(volatile byte*)0x9fd3
#define lpm_rg_lpm_ir_rx_en                 *(volatile byte*)0x9fd4



#define _rv_irq_prio(x)                      *(volatile word*)(0x8e00 + x*2)
#define _rv_irq_enab                         *(volatile unsigned int*)0x8e08
#define _rv_irq_thrd                         *(volatile byte*)0x8e0c
#define _rv_irq_pend                         *(volatile unsigned int*)0x8e40
#define _rv_irq_clam                         *(volatile byte*)0x8e64


#define imem_start				(byte *)0xc000


#define RV_BASEADDR 			(0x8040)
#define RV_ICE_CFG 			    RV_BASEADDR + (0x0)
#define RV_ICE_CTRL 			RV_BASEADDR + (0x10)
#define RV_ICE_REGSEL 			RV_BASEADDR + (0x11)
#define RV_ICE_CMD 			    RV_BASEADDR + (0x12)
#define RV_ICE_BREAK0			RV_BASEADDR + (0x14)
#define RV_ICE_BREAK1			RV_BASEADDR + (0x18)
#define RV_ICE_REG_WDATA		RV_BASEADDR + (0x1c)
#define RV_ICE_REG_RDATA		RV_BASEADDR + (0x20)
#define RV_TRACE_ADDR   		RV_BASEADDR + (0x24)
#define RV_TRACE_RDATA			RV_BASEADDR + (0x28)
#define RV_CURR_PC				RV_BASEADDR + (0x2c)
#define RV_ICE_STATUS			RV_BASEADDR + (0x30)

#define RV_ICE_CMD_STOP		1
#define RV_ICE_CMD_STEP		2
#define RV_ICE_CMD_CONTINUE	3
#define RV_ICE_CMD_WRREG		4
#define RV_ICE_CMD_START_TRACE	8
#define RV_ICE_CMD_STOP_TRACE		9

#define RV_ICE_BREAK_IDLE   0
#define RV_ICE_BREAK_EXE    1
#define RV_ICE_BREAK_RW     2
#define RV_ICE_BREAK_W      3


#define SYSCTRL_ENABLE_TIMER	0x02
#define SYSCTRL_ICACHE_BMODE	4
#define SYSCTRL_ICACHE_MMODE	8

#define QSPICFG_CONT_MODE		  (1 << 5)
#define QSPI_START			1 << 0
#define QSPI_CLEAR_INT		1 << 1

#define SDMA_START			1 << 0
#define SDMA_CLEAR_INT 		1 << 1

#define DMA_START_SDMA	1 << 0
#define DMA_STATUS_SDMA	1 << 0
#define DMA_STATUS_QSPI	1 << 1

#define STATUS_XTAL_STABLE	1
#define STATUS_LPO_CLOCK	2

#define LPM_RTC_COMP_EN	2
#define LPM_RTC_EN			8
#define LPM_RTC_WAKE_EN	16

#define INTR_BT				1
#define INTR_WP				2
#define INTR_TIMER	    	3
#define INTR_UART			4
#define INTR_USB			5

#define VINTR_BT            31
#define VINTR_TIMER         30

#define _CLK_USB				10
#define _CLK_UART			11
#define _CLK_UARTB			12
#define _CLK_AES             13
#define _CLK_SDMA			14
#define _CLK_BT				15
#define _CLK_SPI				16

#define USB_DMA_EN					1 << 4
#define UCFG_ENABLE_ISO_OUT		1
#define UCFG_ENABLE_ISO_IN			2

#define UADDR_DISABLE_BCST			1 << 7

#define USB_IRQ_SETUP			0x1
#define USB_IRQ_SUSPEND		0x2
#define USB_IRQ_NAK				0x4
#define USB_IRQ_RESET			0x8
#define USB_IRQ_ACK				0x10
#define USB_IRQ_RXREADY		0x20
#define USB_IRQ_RXFULL			0x40
#define USB_IRQ_CLKCAL			0x80
#define USB_IRQ_TXDONE(x)		(1 << x + 8)

#define USB_STATUS_EP0_TXBUSY		1
#define USB_STATUS_EP1_TXBUSY		2
#define USB_STATUS_EP2_TXBUSY		4
#define USB_STATUS_EP3_TXBUSY		8
#define USB_STATUS_RXEMPTY		0x40


#define UART_STATUS_TXBUSY	1
#define UART_STATUS_TXFULL		2
#define UART_STATUS_RXEMPTY	4
#define UART_STATUS_RXFULL		8


#define UART_CTRL_RXEN			1
#define UART_CTRL_TXEN			2
#define UART_CTRL_HALFDUPLEX	0x40
#define UART_CTRL_PBAUD		0x80

#define FLASH_CMD_WRITE_STATUS	0x01
#define FLASH_CMD_PROGRAM		0x02
#define FLASH_CMD_READ				0x03
#define FLASH_CMD_READ_STATUS	0x05
#define FLASH_CMD_WRITE_ENABLE	0x06
#define FLASH_CMD_CHIP_ERASE		0xC7
#define FLASH_CMD_ERASE_BLOCK	0x20
#define FLASH_CMD_READ_STATUS2	0x35
#define FLASH_CMD_DUAL_READ		0x3b
#define FLASH_CMD_QUAD_READ		0x6b
#define FLASH_CMD_QUAD_READ2		0xeb
#define FLASH_CMD_EXIT_PD			0xab
#define FLASH_STATUS_BUSY			1

#define FLASH_ADDR_KEY				0xf000

#define DAC_MONO				1 << 8
#define FILTER_ENABLE			1 << 4
#define SDM_ENABLE				1 << 5
#define CONT_MODE				1 << 2
#define DAC_ENABLE				1 << 0

#define CLKL_ENABLE				1 << 2

#define DAC_HALF_EMPTY			1 << 6

#define I2SCFG_ENABLE			1 << 0
#define I2SCFG_RXEN				1 << 1
#define I2SCFG_48FS				1 << 4
#define I2SCFG_64FS				2 << 4
#define I2SCFG_128FS				3 << 4
#define I2S_MODE_PCM			1 << 2
#define I2S_MODE_LEFT			2 << 2
#define I2S_MODE_RIGHT			3 << 2
#define I2S_DATA24BIT			1 << 6
#define I2SCFG_MASTER			1 << 7

#define I2SCFG_PCM_32FS16		I2S_MODE_PCM | I2SCFG_64FS
#define I2SCFG_PCM_64FS16		I2S_MODE_PCM | I2SCFG_128FS
#define I2SCFG_PCM_64FS24		I2SCFG_PCM_64FS16 | I2S_DATA24BIT
#define I2SCFG_I2S_32FS16		0
#define I2SCFG_I2S_48FS16		I2SCFG_48FS
#define I2SCFG_I2S_48FS24		I2SCFG_48FS | I2S_DATA24BIT
#define I2SCFG_I2S_64FS16		I2SCFG_64FS
#define I2SCFG_I2S_64FS24		I2SCFG_64FS | I2S_DATA24BIT
#define I2SCFG_LEFT_32FS16		I2S_MODE_LEFT
#define I2SCFG_LEFT_48FS16		I2S_MODE_LEFT | I2SCFG_48FS
#define I2SCFG_LEFT_64FS16		I2S_MODE_LEFT | I2SCFG_64FS
#define I2SCFG_LEFT_48FS24		I2SCFG_LEFT_48FS16 | I2S_DATA24BIT
#define I2SCFG_LEFT_64FS24		I2SCFG_LEFT_64FS16 | I2S_DATA24BIT
#define I2SCFG_RIGHT_48FS16		I2S_MODE_RIGHT | I2SCFG_48FS
#define I2SCFG_RIGHT_64FS16		I2S_MODE_RIGHT | I2SCFG_64FS
#define I2SCFG_RIGHT_64FS24		I2SCFG_RIGHT_64FS16 | I2S_DATA24BIT

#define IIC_STATUS_DONE			1
#define IIC_STATUS_ACK			2
#define IIC_STATUS_CRCOK		4

#define IIC_CTRL_RESTART		1
#define IIC_CTRL_WR_IMEM		2

#define MHZ				24
#define MS				1000
#define SECOND			1000*MS
#define SLOT			625
#define DSLOT			(SLOT*2)

#define ST_TIMER_MISSED			(1 << 0)
#define ST_STOP_SAVELIST		(1 << 1)
#define ST_USB_ADDRESSED		(1 << 2)
#define ST_USB_CONFIGURED		(1 << 3)
#define ST_USB_SETUP_OUT		(1 << 4)
#define ST_AUDIO_ENABLE		(1 << 5)
#define ST_MINMAX_ENABLE		(1 << 6)

#define SDMA_SRC_WRAP		1
#define SDMA_DEST_WRAP	2
#define SDMA_SET			4
#define SDMA_LAST			8
#define SDMA_SHIFT			17
#define SDMA_BSIZE_SHIFT	16


#ifdef BTDMA_OLD
#define BTDESC_S8			2
#define BTDESC_S2			3
#define BTDESC_LAST			(1 << 2)
#define BTDESC_RXWIN		(1 << 3)
#define BTDESC_TXCRC		(1 << 3)
#define BTDESC_CRC			(1 << 4)
#define BTDESC_WHITE		(1 << 5)
#define BTDESC_WMASK		(1 << 5)
#define BTDESC_SYNC		    (1 << 6)
#define BTDESC_WORD		    (1 << 6)
#define BTDESC_AUTOLEN		(1 << 7)
#define BTDESC_PARAM		1 << 7

#define BTDESC_RX			0 << 12
#define BTDESC_TX			1 << 12
#define BTDESC_WRITE		2 << 12
#define BTDESC_RW_PATT  	3 << 12
#define BTDESC_INTR  		4 << 12
#define BTDESC_DELAY  		5 << 12
#define BTDESC_CALL  		6 << 12
#define BTDESC_TIMER  		7 << 12


#define BTCMD_START		    1
#define BTCMD_STOP			2
#define BTCMD_STEP			3
#define BTCMD_PACKETEND	    4
#define BTCMD_RECALC		5
#define BTCMD_RESET_MDM	    6

#define BTSTATUS_TXEMPTY	1 << 0
#define BTSTATUS_IDLE		1 << 1
#define BTSTATUS_CRCERR	1 << 2
#define BTSTATUS_RXSYNC	1 << 3

#define BTCTRL_BLE_MODE		(1 << 0)
#define BTCTRL_ENCRYPTION		(1 << 1)
#define BTCTRL_ENABLE_TIMER	(1 << 4)
#else
#define BTDESC_S8			2
#define BTDESC_S2			3
#define BTDESC_WHCRC		(1 << 2)
#define BTDESC_ENC		    (1 << 3)
#define BTDESC_SEQ		    (1 << 4)
#define BTDESC_LEN			(1 << 5)
#define BTDESC_LAST			(1 << 8)
#define INSTR_LAST		    (BTDESC_LAST << 18)

#define BTDESC_VAL(ins,addr,desc)	ins << 27 | (desc) << 18 | (((int)(addr))&0x3ffffff)

#define BDSET(val)					*btdesc_ptr++ = val
#define BDSET_START(d)				if(btdesc_ptr) {BDSET_END}; btdesc_ptr = btdesc_start = d, btdesc_len = sizeof(d)
#define BDSET_START_O(d)            btdesc_ptr = btdesc_start = d, btdesc_len = sizeof(d)
#define BDSET_END					if(((int)btdesc_ptr - (int)btdesc_start) > btdesc_len){\
                                        yc_bt_bb_log("_ERROR:%x-%x-%x\r\n",btdesc_start,btdesc_ptr,btdesc_len);\
                                        while(1);\
                                    }; \
                                    *(btdesc_ptr - 1) |= BDSET_LAST, btdesc_ptr = 0;\



#define BDSET_LAST					(1 << 26)

#define INSTR_WBYTE(addr, val)		BTDESC_VAL(0, addr, val)
#define INSTR_INTR 	 				BTDESC_VAL(1, 0, 0)
#define INSTR_DELAY(val)			BTDESC_VAL(2, val, 0)
#define INSTR_TIMER(val)			BTDESC_VAL(3, val, 0)
#define INSTR_BITS(val) 	 		BTDESC_VAL(4, val, 0)
#define INSTR_RWORD(addr) 			BTDESC_VAL(5, addr, 0)
#define INSTR_WWORD(addr)			BTDESC_VAL(6, addr, 0)
#define INSTR_WPTR(ptr,desc)		BTDESC_VAL(7, &ptr, desc)
#define INSTR_TX(addr, desc) 		BTDESC_VAL(8, addr, desc)
#define INSTR_TXP(ptr, desc) 		BTDESC_VAL(9, &ptr, desc)
#define INSTR_RXSYNC 				BTDESC_VAL(10, 0, 0)
#define INSTR_RXP(ptr, desc) 		BTDESC_VAL(11, &ptr, desc)
#define INSTR_RETBIT0(addr,bit) 	BTDESC_VAL(12, addr, bit)
#define INSTR_RETBIT1(addr,bit) 	BTDESC_VAL(13, addr, bit)
#define INSTR_RETEQ(addr, val)		BTDESC_VAL(14, addr, val)
#define INSTR_RETNE(addr, val)		BTDESC_VAL(15, addr, val)
#define INSTR_RETNW(addr)			BTDESC_VAL(16, addr, 0)
#define INSTR_CALL(ptr,desc) 		BTDESC_VAL(17, &(ptr), desc)
#define INSTR_WBITS(addr, mask)		BTDESC_VAL(18, addr, (mask)&0xff)
#define INSTR_RPBYTE(ptr,pos)       BTDESC_VAL(19, &ptr, pos)
#define INSTR_INCBITS(val)          BTDESC_VAL(20, (short)val, 0)
#define INSTR_INCADDR(addr)         BTDESC_VAL(21, addr, 0)
#define INSTR_RETNEB(addr)          BTDESC_VAL(22, addr, 0)
#define INSTR_RETEQB(addr)          BTDESC_VAL(23, addr, 0)

#define BDSET_WBYTE(addr, val)		BDSET(INSTR_WBYTE(addr, val))
#define BDSET_INTR 	 				BDSET(INSTR_INTR)
#define BDSET_DELAY(val)			BDSET(INSTR_DELAY(val))
#define BDSET_TIMER(val)			BDSET(INSTR_TIMER(val))
#define BDSET_BITS(val) 	 		BDSET(INSTR_BITS(val))
#define BDSET_RWORD(addr) 			BDSET(INSTR_RWORD(addr))
#define BDSET_WWORD(addr)			BDSET(INSTR_WWORD(addr))
#define BDSET_WPTR(addr)			BDSET(INSTR_WPTR(addr,0))
#define BDSET_TX(addr, desc) 		BDSET(INSTR_TX(addr, desc))
#define BDSET_TXP(addr, desc) 		BDSET(INSTR_TXP(addr, desc))
#define BDSET_RXSYNC 				BDSET(INSTR_RXSYNC)
#define BDSET_RXP(addr, desc) 		BDSET(INSTR_RXP(addr, desc))
#define BDSET_RETBIT0(addr,bit) 	BDSET(INSTR_RETBIT0(addr,bit))
#define BDSET_RETBIT1(addr,bit) 	BDSET(INSTR_RETBIT1(addr,bit))
#define BDSET_RETEQ(addr, val)		BDSET(INSTR_RETEQ(addr, val))
#define BDSET_RETNE(addr, val)		BDSET(INSTR_RETNE(addr, val))
#define BDSET_RETNW(addr)			BDSET(INSTR_RETNW(addr))
#define BDSET_CALL(addr) 	 		BDSET(INSTR_CALL(addr,0))
#define BDSET_WBITS(addr, mask)		BDSET(INSTR_WBITS(addr, (mask)&0xff))
#define BDSET_RPBYTE(addr,pos)      BDSET(INSTR_RPBYTE(addr, pos))
#define BDSET_INCBITS(val)          BDSET(INSTR_INCBITS(val))
#define BDSET_INCADDR(addr)         BDSET(INSTR_INCADDR(addr))
#define BDSET_RETNEB(addr)          BDSET(INSTR_RETNEB(addr))
#define BDSET_RETEQB(addr)          BDSET(INSTR_RETEQB(addr))



#define BTCMD_START			1
#define BTCMD_STOP			2
#define BTCMD_STEP			3
#define BTCMD_CLEAR			4
#define BTCMD_RECALC		5
#define BTCMD_RESET_MDM		6
#define BTCMD_TOGGLE_NESN	7
#define BTCMD_TOGGLE_SN		8

#define BTSTATUS_TXEMPTY	1 << 0
#define BTSTATUS_IDLE		1 << 1
#define BTSTATUS_CRCERR		1 << 2
#define BTSTATUS_SYNC		1 << 3
#define BTSTATUS_SNEQ		1 << 4
#define BTSTATUS_RXOK		9


#define STBIT_CRCERR		2
#define STBIT_RXSYNC		3
#define STBIT_SNEQ			4

#define SREQ_ADV_ADDR			8


#endif

#define FREQ_MUL_24M		2
#define FREQ_DIV_24M		36
#define FREQ_RX			0x80
#define FREQ_IF_RX		4
#define FREQ_IF_TX		0

#define AES_KEY_DWORD		1
#define AES_DATA_DWORD	    2
#define AES_CBC				4
#define AES_CTR_MIC			8
#define AES_CTR_DAT			12
#define AES_BIG_ENDIAN		16

#define AES_CMD_START		1
#define AES_CMD_INITIV		2
#define AES_CMD_CLRINT		3
#define AES_CMD_CLRIV		4

#define AES_STBIT_DATAVALID		0
#define AES_STBIT_MICOK		1
#define AES_STBIT_MICDONE			2

#define	usb_intid		0
#define	iic_intid		1
#define	qspi_intid		2
#define	spid_intid		3
#define	key_intid		4
#define	uart_intid		5
#define	uartb_intid	6
#define	adcd_intid		7
#define	i2s_intid		8
#define	bt_intid		9
#define	rsa_intid		10

#define  gpcfg_input						0
#define  gpcfg_qspi_ncs        2
#define  gpcfg_qspi_sck        3
#define  gpcfg_qspi_io0        4
#define  gpcfg_qspi_io1        5
#define  gpcfg_qspi_io2        6
#define  gpcfg_qspi_io3        7
#define  gpcfg_uart_txd        8
#define  gpcfg_uart_rxd        9
#define  gpcfg_uart_rts        10
#define  gpcfg_uart_cts        11
#define  gpcfg_uartb_txd       12
#define  gpcfg_uartb_rxd       13
#define  gpcfg_uartb_rts       14
#define  gpcfg_uartb_cts       15
#define  gpcfg_pwm_out0        16
#define  gpcfg_nec_gpioi       17
#define  gpcfg_qspi1_ncs       18
#define  gpcfg_qspi2_sck       19
#define  gpcfg_qspi1_io0       20
#define  gpcfg_qspi1_io1       21
#define  gpcfg_qspi1_io2       22
#define  gpcfg_qspi1_io3       23
#define  gpcfg_i2s_dout        24
#define  gpcfg_i2s_lrckout     25
#define  gpcfg_i2s_sclkout      26
#define  gpcfg_i2s_mclkout     27
#define  gpcfg_i2s_din         28
#define  gpcfg_i2s_lrckin      29
#define  gpcfg_i2s_sclkin       30
#define  gpcfg_acmp_out			31

#define  gpcfg_spid_mosi       34
#define  gpcfg_spid_miso       35
#define  gpcfg_spid_ncsi       36
#define  gpcfg_spid_scki       37


#define  gpcfg_qdec_x0         38
#define  gpcfg_qdec_x1         39
#define  gpcfg_iic_scl_drv         43
#define  gpcfg_iic_scl         44
#define  gpcfg_iic_sda         45
#define  gpcfg_dac_out         46
#define  gpcfg_dac_outn         47
#define  gpcfg_jtag_swclk      60
#define  gpcfg_jtag_swdat      61
#define  gpcfg_output_low      62
#define  gpcfg_output_high     63

#define  gpcfg_pullup					0x40
#define  gpcfg_pulldown        0x80
#define  gpcfg_no_ie           0xc0

#define misc_spid_start 	2

#define csr_mstatus		0x300
#define csr_mie			0x304
#define csr_mscratch		0x340
#define csr_mepc			0x341
#define csr_mcause		0x342
#define csr_mcycle		0xb00
#define csr_mcycleh		0xb80

#define MIE_SOFT        1<<3
#define MIE_TIMER       1<<7
#define MIE_EXT         1<<11
#define MIE_BTTMR       1<<30
#define MIE_BT          1<<31

#define PREFETCH_LINE(addr)	*(volatile int*)addr = 0
#define P2W(x)	((int)(x) & 0xffff)
#define W2P(x)	(byte*)((int)(x) & 0xffff | 0x10000)

#define HREAD(x)		*(volatile byte*)(x)
#define HWRITE(x,y)		HREAD(x) = (y)
#define HREADW(reg)			(HREAD(reg) | HREAD(reg + 1) << 8)

#define ADDRB(x)        (*(volatile unsigned char*)(x))
#define ADDRW(x)        (*(volatile unsigned short*)(x))
#define ADDR(x)         (*(volatile unsigned long*)(x))
#define WAKEUP(x,y)      ADDRB(0x9f24+x/2) = (y) << ((x%2)*4)
#define PWMCNT(x,p,n)   do{ADDRW(0x8290+x*8) = p; ADDRW(0x8292+x*8) = n;}while(0)
#define GPIO_AE(x)      do{if(x>32) ADDRB(0x833c) |= 1<<(x-32);else ADDR(0x8338) |=1<<x;}while(0)
#define GPIO_CONFIG(x)  (*(volatile char*)(0x8300+(x)))

#define MEMPTR(val)			(void *)((int)(val))
#define GETW(addr)			(*(byte *)(addr) | *(byte *)((int)(addr) + 1) << 8)
#define GETT(addr)			(*(byte *)(addr + 2) << 16 | GETW(addr))
#define GETL(addr)			(*(byte *)(addr + 3) << 24 | GETT(addr))
#define PUTW(addr, val)		*(byte *)(addr) = (val) & 0xff, *(byte *)((int)(addr) + 1) = (val) >> 8 & 0xff
#define PUTT(addr, val)		PUTW(addr, val), *(byte *)((int)(addr) + 2) = (val) >> 16 & 0xff
#define PUTL(addr, val)		PUTT(addr, val), *(byte *)((int)(addr) + 3) = (val) >> 24 & 0xff

#define MIN(a,b)            (a)>(b) ? (b) : (a)
#define MAX(a,b)            (a)>(b) ? (a) : (b)

#define rom_interface_func_table  0x1ff0004
#define  rom_delay                      0
#define  rom_delayus                   1
#define  rom_delay_ms                  2
#define  rom_init_otp                  3
#define  rom_init_otp_write            4
#define  rom_deinit_otp                5
#define  rom_read_otp                  6
#define  rom_write_otp                 7
#define  rom_program_otp_bit           8
#define  rom_qspi0_dmastart             9
#define  rom_qspi0_write                10
#define  rom_qspi0_readwrite            11
#define  rom_qspi0_flashcmd             12
#define  rom_qspi0_read_flashstatus1    13
#define  rom_qspi0_read_flashstatus2    14
#define  rom_qspi0_write_flashstatus    15
#define  rom_qspi0_flash_quadenable     16
#define  rom_qspi0_flash_write          17
#define  rom_qspi0_flash_read           18
#define  rom_qspi0_flash_chiperase      19
#define  rom_qspi0_flash_erase          20
#define  rom_qspi0_flash_jedecid        21
#define  rom_wfi                       22
#define  rom_CRC16                     23
#define  rom_param_check               24
#define  rom_flash_start                25
#define ROM_API(n) (rom_interface_func_table + n*4)

#define _delay(x)    ((void (*)(uint32_t))(ROM_API(rom_delay)))(x)
#define _delay_us(x)  ((void (*)(uint32_t))(ROM_API(rom_delayus)))(x)
#define _delay_ms(x) ((void (*)(uint32_t))(ROM_API(rom_delay_ms)))(x)
#define flash_power_down()  ((void (*)(uint8_t))(ROM_API(rom_qspi0_flashcmd)))(0xB9)

#define __ramcode__  __attribute__((section ("._SECTION_RAM_CODE_DEFINE")))

#define RTC_CALI_CNT    (0xa)//2^x*30us
void static inline rtc_cali_start(uint8_t time)
{
    core_lpo_counter_cali = (time&0xf);
    if((core_lpo_cali_cnt & (1<<31)) == 0)
        core_lpo_counter_cali |= 1<<4;
    core_lpo_counter_cali |= 1<<4;
}
byte static inline rtc_cali_done(void)
{
    return (core_lpo_cali_cnt & (1<<31)) != 0;
}

uint32_t static inline rtc_cali_end(void)
{
    while((core_lpo_cali_cnt & (1<<31)) == 0);
    return core_lpo_cali_cnt & (~(1<<31));
}

void static inline rtc_systick_sync(void)
{
    core_rtc_systick_sync_cmd = 1;
}

void static inline rtc_systick_sync_wait(void)
{
    while((core_rtc_systick_sync_cmd & (1<<4)) == 0);
}

void static inline rtc_systick_ovwr(uint32_t syscnt,uint32_t rtccnt)
{
    core_systick_ovwr_dat = syscnt;
    core_systick_ovwr_rtc_val = rtccnt;
    core_rtc_systick_sync_cmd = 1<<1;
}
void static inline rtc_systick_ovwr_wait(void)
{
    while((core_rtc_systick_sync_cmd & (1<<5)) == 0);
}

// cache way conflict step is 0x1000
void inline _prefetch(void *start_addr, void *end_addr, int permanent)
{
    int addr, val = 1;
    for(addr = (int)start_addr & ~0x1f; addr < (int)end_addr + 32; addr += 32)
        if(permanent)
            __ASM volatile("sw %1,0(%0)" ::"r"(addr), "r"(val));
        else
            __ASM volatile("lw x0,0(%0)" ::"r"(addr));
}

// invalidate will remove permanent cache line
void inline invalidate(void *start_addr, void *end_addr)
{
    int addr;
    for(addr = (int)start_addr & ~0x1f; addr < (int)end_addr + 32; addr += 32)
        __ASM volatile("sw x0,0(%0)" ::"r"(addr));
}

unsigned int inline read_csr(const int csr)
{
    unsigned int ret;
    __ASM volatile("csrr %0,%1" :"=r"(ret): "i"(csr));
    return ret;
}

void inline write_csr(const int csr, int val)
{
    __ASM("csrw %0,%1" :: "i"(csr), "r"(val));
}

void inline set_csr_bit(const int csr, int bit)
{
    __ASM("csrs %0,%1" :: "i"(csr), "r"(bit));
}

void inline clr_csr_bit(const int csr, int bit)
{
    __ASM("csrc %0,%1" :: "i"(csr), "r"(bit));
}

void inline enable_timer_intr()
{
    set_csr_bit(csr_mie, 1 << 7);
}

void inline disable_timer_intr()
{
    clr_csr_bit(csr_mie, 1 << 7);
}

void inline vector_intr(int intr,bool st)
{
    if(st)
        set_csr_bit(csr_mie, 1 << (intr&0x1f));
    else
        clr_csr_bit(csr_mie, 1 << (intr&0x1f));
}
static void inline enable_ext_intr(int intr)
{
    _rv_irq_prio(intr / 8) |= 1 << 2 * (intr % 8);
    _rv_irq_enab      |= 1 << intr;

    set_csr_bit(csr_mie, 1 << 11);
}

void inline critical_section(int enter)
{
    if(enter)
        clr_csr_bit(csr_mstatus, 1 << 3);
    else
        set_csr_bit(csr_mstatus, 1 << 3);
}

void static inline enable_clock(int clock)
{
    core_clken |= 1 << clock;
}

void static inline ana_clock_enable(int clock)
{
    core_anaclk_en |= 1 << clock;
}

int inline issim()
{
    return 0;//core_revision & 0x80;
}

void static inline disable_timer()
{
    core_sys_timer_en &= ~SYSCTRL_ENABLE_TIMER;
}

void static inline enable_timer()
{
    core_sys_timer_en |= SYSCTRL_ENABLE_TIMER;
}

uint8_t static inline timer_status()
{
    return core_sys_timer_en & SYSCTRL_ENABLE_TIMER;
}

void static inline ice_break(void)
{
    *(volatile byte *)(RV_ICE_CMD) = RV_ICE_CMD_STOP;
}

void static inline ice_watchpoint(uint32_t addr, uint8_t mode)
{
    uint8_t tmp = ADDRB(RV_ICE_CTRL);
    ADDR(RV_ICE_BREAK0) = addr;
    tmp &= ~3;
    ADDRB(RV_ICE_CTRL) = tmp | (mode & 3);
}

/* ============================ queue ============================ */

int static inline queue_empty(byte wptr, byte rptr)
{
	return wptr == rptr;
}

int static inline queue_size(int sizex4, byte wptr, byte rptr)
{
	return wptr >= rptr ? wptr - rptr : sizex4 / 4 - (rptr - wptr);
}
int static inline get_last_queue(int *queue, int sizex4, byte * wptr, byte * rptr)
{
    int val;

	if(queue_empty(*wptr, *rptr)) return 0;
	val = queue[*rptr];
    return val;
}
	// queue items cannot exceed 256
int static inline  get_queue(int *queue, int sizex4, byte * wptr, byte * rptr)
{
	int val;

	if(queue_empty(*wptr, *rptr)) return 0;
	val = queue[*rptr];
	*rptr = *rptr == sizex4 / 4 - 1 ? 0 : *rptr + 1;
	return val;
}

int static inline  put_queue(int *queue, int sizex4, byte * wptr, byte * rptr, int val)
{
	byte nwptr = *wptr == sizex4 / 4 - 1 ? 0 : *wptr + 1;

	if(nwptr == *rptr) return -1;											// full
	queue[*wptr] = val;
	*wptr = nwptr;
    return 0;
}

void static inline queue_init(int *queue, int sizex4, byte * wptr, byte * rptr)
{
    *wptr = 0;
    *rptr = 0;
}

void static inline rand1(byte *rand, int len)
{
	for(int  i = 0;i<len; i++)
		rand[i] = core_rand_num;
}

void static inline mem_cpy(byte *dst, byte *src, int len)
{
	for(int  i = 0;i<len; i++)
		dst[i] = src[i];
}

typedef struct _tmrtask {
    uint timer;
    void (*cb)(uint);
    uint arg;
    uint add_ts;
    uint cb_ts;
}tmrtask;

typedef struct  {
	byte advtx;         // 0
	byte scanrx;        // 1
	byte advrx;         // 2
	byte reqrx;         // 3
	byte rsprx;			// 4
	byte connrx;        // 5
    byte conntx;        // 6
	byte evtrx;         // 7


    byte total_dev;     // 8
    byte active_dev;    // 9

	byte status;
	byte flag;
	volatile byte state;
	volatile byte cmd;
	byte acked;

    byte *encbuf;
    uint calib_old_rtc;
    uint calib_loop_cnt;
    uint32_t calib_anchor;
    byte le_inited;
	bool le_master:1;
    byte unused:7;

	byte twptr, trptr;
    volatile byte tfull;
    tmrtask timers[16];
} fixed;
extern fixed s;

void assert(int);
void ice_break();
// void _delay(int m);

#endif
