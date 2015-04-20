library IEEE;
use IEEE.std_logic_1164.ALL;

--------------------------------------------------------------------------------
--
--
-- Definition of Ports
-- FSL_Clk         : Synchronous clock
-- FSL_Rst         : System reset, should always come from FSL bus
-- FSL_S_Clk       : Slave asynchronous clock
-- FSL_S_Read      : Read signal, requiring next available input to be read
-- FSL_S_Data      : Input data
-- FSL_S_CONTROL   : Control Bit, indicating the input data are control word
-- FSL_S_Exists    : Data Exist Bit, indicating data exist in the input FSL bus
-- FSL_M_Clk       : Master asynchronous clock
-- FSL_M_Write     : Write signal, enabling writing to output FSL bus
-- FSL_M_Data      : Output data
-- FSL_M_Control   : Control Bit, indicating the output data are contol word
-- FSL_M_Full      : Full Bit, indicating output FSL bus is full
--
-------------------------------------------------------------------------------

------------------------------------------------------------------------------
-- Entity Section
------------------------------------------------------------------------------
entity circuito is
  port (
  	-- DO NOT EDIT BELOW THIS LINE ---------------------
    -- Bus protocol ports, do not add or delete.
    FSL_Clk       : in	std_logic;
    FSL_Rst       : in	std_logic;
    FSL_S_Clk     : in	std_logic;
    FSL_S_Read    : out	std_logic;
    FSL_S_Data    : in	std_logic_vector(0 to 31);
    FSL_S_Control : in	std_logic;
    FSL_S_Exists  : in	std_logic;
    FSL_M_Clk     : in	std_logic;
    FSL_M_Write   : out	std_logic;
    FSL_M_Data    : out	std_logic_vector(0 to 31);
    FSL_M_Control : out	std_logic;
    FSL_M_Full    : in	std_logic
    -- DO NOT EDIT ABOVE THIS LINE ---------------------
  ) ;
end entity ; -- circuito

architecture Behavioral of circuito is

	component datapath is
		port(
			-- Input
			clk      : in std_logic;
			word     : in std_logic_vector(31 downto 0);
			sel_char : in std_logic_vector(1 downto 0);
			we       : in std_logic;
			comp_en  : in std_logic;

			-- Output
			count_out : out std_logic_vector(15 downto 0);
			count_out2 : out std_logic_vector(15 downto 0)
		);
	end component;

	component fsm is
	port(
			-- Input
			clk	  : in std_logic;
			rst	  : in std_logic;
			start : in std_logic;
			RW    : in std_logic;

			-- Output
			sel_word : out std_logic_vector(1 downto 0);
			we       : out std_logic;
			comp_en  : out std_logic;
			addr     : out std_logic_vector(7 downto 0)
			);
	end component;

	---------------- Auxilliary Signals ----------------
	signal sel_word_aux : std_logic_vector(1 downto 0) := (others => '0');
	signal we_aux       : std_logic := '0';
	signal comp_en_aux  : std_logic := '0';
begin

Inst_datapath : datapath
	port map(
		-- Input
		clk      => FSL_Clk,
		word     => FSL_S_Data,
		sel_char => sel_word_aux, -- vem da máquinda de estados
		we       => we_aux, -- vem da máquina de estados
		comp_en  => comp_en_aux,

		-- Output
		count_out => FSL_M_Data(0 to 15),
		count_out2 => FSL_M_Data(16 to 31)
	);

Inst_fsm : fsm
	port map(
		-- Input
		clk   => FSL_Clk, -- vem da fsl
		rst   => FSL_Rst, -- vem da fsl
		start => FSL_S_Exists, -- vem da fsl
		RW    => FSL_S_Control,

		-- Output
		sel_word => sel_word_aux, -- liga a sel_char da datapath
		we       => we_aux, -- liga a we da datapath
		comp_en  => comp_en_aux, -- liga a comp_en da datapath
		addr     => open -- liga isto à BRAM para retirar o endereço
	);

end Behavioral; -- Behavioral
