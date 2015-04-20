library IEEE;
use IEEE.std_logic_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

entity datapath is
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
end datapath;

architecture Behavioral of datapath is

	component BRAM
		generic(
		ADDR_WIDTH : integer;
		DATA_WIDTH : integer
	);
	port(
		clk        : in std_logic;
		we         : in std_logic;
		read_addr  : in std_logic_vector(7 downto 0);
		read_addr2 : in std_logic_vector(7 downto 0);
		write_addr : in std_logic_vector(7 downto 0);
		data_in    : in std_logic_vector(15 downto 0);
		data_out   : out std_logic_vector(15 downto 0);
		data_out2  : out std_logic_vector(15 downto 0)
	);
	end component;

	---------------- Auxilliary Signals ----------------
	signal char       : std_logic_vector(7 downto 0);
	signal char2      : std_logic_vector(7 downto 0);
	signal prev_char  : std_logic_vector(7 downto 0);
	signal bram_out   : std_logic_vector(15 downto 0);
	signal adder_out  : std_logic_vector(15 downto 0);
	signal adder_in   : std_logic_vector(15 downto 0);
	signal prev_count : std_logic_vector(15 downto 0);
	signal prev_we    : std_logic;

begin

	Inst_BRAM : BRAM
	generic map (
		ADDR_WIDTH => 8,
		DATA_WIDTH => 16
	)
	port map(
		clk => clk,
		we => prev_we,
		read_addr => char,
		read_addr2 => char2,
		write_addr => prev_char,
		data_in => prev_count,
		data_out => bram_out,
		data_out2 => count_out2 
	);

	char2 <= char xor x"01";

	------------------- Select Word --------------------
	char <= word(7 downto 0) when sel_char = "00" else
		word(15 downto 8) when sel_char = "01" else
		word(23 downto 16) when sel_char = "10" else
		word(31 downto 24);

	---------------------- Adder -----------------------
	-- Mux and comparator
	adder_in <= bram_out when (char /= prev_char or comp_en = '0') else prev_count;
	-- Increments word count
	adder_out <= adder_in + 1;

	-------------------- Registers ---------------------
	process(clk)
	begin
		if clk'event and clk='1' then
			prev_char <= char;
			prev_count <= adder_out;
			prev_we <= we;
		end if;
	end process;

	--------------------- Output -----------------------
	count_out <= bram_out;
	-- count_out2 is also an output

end Behavioral;
