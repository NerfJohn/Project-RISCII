library verilog;
use verilog.vl_types.all;
entity dut_soc is
    port(
        pwrOn           : in     vl_logic;
        clk             : out    vl_logic;
        firstWord       : out    vl_logic_vector(15 downto 0);
        halt            : out    vl_logic
    );
end dut_soc;
