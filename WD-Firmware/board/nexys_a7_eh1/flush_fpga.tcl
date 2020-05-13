
##???? Ofer do I need this : open_project /home/oshinaar/riscv/eh1_fpga/hardware/project/script/nexys4ddr/nexys4ddr.xpr

## set_property source_mgmt_mode None [current_projec]
## update_ip_catalog

# connect to HW...
open_hw
connect_hw_server
open_hw_target
set_property PROGRAM.FILE {./eh1_reference_design.bit} [get_hw_devices xc7a100t_0]
current_hw_device [get_hw_devices xc7a100t_0]
refresh_hw_device -update_hw_probes false [lindex [get_hw_devices xc7a100t_0] 0]
create_hw_cfgmem -hw_device [lindex [get_hw_devices] 0] -mem_dev [lindex [get_cfgmem_parts {s25fl128sxxxxxx0-spi-x1_x2_x4}] 0]

#program the device 
set_property PROBES.FILE {} [get_hw_devices xc7a100t_0]
set_property FULL_PROBES.FILE {} [get_hw_devices xc7a100t_0]
set_property PROGRAM.FILE {./eh1_reference_design.bit} [get_hw_devices xc7a100t_0]
program_hw_devices [get_hw_devices xc7a100t_0]
