open_vcd
log_vcd [get_object /asd_pre_1_tb/*]
log_vcd [get_object /asd_pre_1_tb/dut/*]
log_vcd [get_object /asd_pre_1_tb/dut/u_spi/*]
log_vcd [get_object /asd_pre_1_tb/dut/u_stp/*]
log_vcd [get_object /asd_pre_1_tb/dut/u_gain/*]
log_vcd [get_object /asd_pre_1_tb/dut/u_pts/*]
run all
flush_vcd
quit