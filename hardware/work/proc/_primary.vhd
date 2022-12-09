library verilog;
use verilog.vl_types.all;
entity proc is
    port(
        inDataD         : out    vl_logic_vector(63 downto 0);
        inAddrD         : out    vl_logic_vector(15 downto 0);
        isRdD           : out    vl_logic;
        startReqD       : out    vl_logic;
        outDataD        : in     vl_logic_vector(63 downto 0);
        reqFinishD      : in     vl_logic;
        inDataI         : out    vl_logic_vector(63 downto 0);
        inAddrI         : out    vl_logic_vector(15 downto 0);
        startReqI       : out    vl_logic;
        outDataI        : in     vl_logic_vector(63 downto 0);
        reqFinishI      : in     vl_logic;
        sigHalt         : out    vl_logic;
        sigFirstWord    : out    vl_logic_vector(15 downto 0);
        clk             : in     vl_logic;
        rstn            : in     vl_logic
    );
end proc;
