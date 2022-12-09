library verilog;
use verilog.vl_types.all;
entity mem_64k is
    generic(
        NON_VOLATILE    : integer := 0
    );
    port(
        inData          : in     vl_logic_vector(63 downto 0);
        inAddr          : in     vl_logic_vector(15 downto 0);
        isRd            : in     vl_logic;
        startReq        : in     vl_logic;
        outData         : out    vl_logic_vector(63 downto 0);
        reqFinish       : out    vl_logic;
        clk             : in     vl_logic;
        rstn            : in     vl_logic
    );
    attribute mti_svvh_generic_type : integer;
    attribute mti_svvh_generic_type of NON_VOLATILE : constant is 1;
end mem_64k;
