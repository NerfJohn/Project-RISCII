library verilog;
use verilog.vl_types.all;
entity sim_top is
    generic(
        IS_TEST         : integer := 0
    );
    attribute mti_svvh_generic_type : integer;
    attribute mti_svvh_generic_type of IS_TEST : constant is 1;
end sim_top;
