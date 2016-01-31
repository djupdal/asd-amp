xelab --debug typical \
-incremental \
-L work \
-L simprims_ver \
-L unifast_ver \
-L unisims_ver \
-L secureip \
-prj toplevel.prj \
-s toplevel.snap \
work.asd_pre_1_tb glbl
xsim toplevel.snap -t toplevel.cmd
