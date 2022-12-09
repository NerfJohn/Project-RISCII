library verilog;
use verilog.vl_types.all;
entity clk_rst is
    port(
        pwrOn           : in     vl_logic;
        clk             : out    vl_logic;
        rstn            : out    vl_logic
    );
end clk_rst;
