library IEEE;
use IEEE.std_logic_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

entity datapath is
	port(
		-- Input
		clk : in std_logic;
		word : in std_logic_vector(31 downto 0);
		sel_char : in std_logic_vector(1 downto 0);
		we : in std_logic;
		-- Output
		count_out : out std_logic_vector(15 downto 0)
	);
end datapath;

architecture Behavioral of datapath is

	component BRAM
		generic(
		ADDR_WIDTH : integer;
		DATA_WIDTH : integer
	);
	port(
		clk : in std_logic;
		we : in std_logic;
		read_addr : in std_logic_vector(7 downto 0);
		write_addr : in std_logic_vector(7 downto 0);
		data_in : in std_logic_vector(15 downto 0);          
		data_out : out std_logic_vector(15 downto 0)
	);
	end component;

	---------------- Auxilliary Signals ----------------
	signal char : std_logic_vector(7 downto 0);
	signal bram_out : std_logic_vector(15 downto 0);
	signal adder_out : std_logic_vector(15 downto 0);
	signal reg_out : std_logic_vector(7 downto 0); 

begin

	Inst_BRAM : BRAM 
	generic map (
		ADDR_WIDTH => 8,
		DATA_WIDTH => 16
	)
	port map(
		clk => clk,
		we => we,
		read_addr => char,
		write_addr => reg_out,
		data_in => adder_out,
		data_out => bram_out
	);

	------------------- Select Word --------------------
	char <= word(7 downto 0) when sel_char = "00" else
		word(15 downto 8) when sel_char = "01" else
		word(23 downto 16) when sel_char = "10" else
		word(31 downto 24);

	------------------------ Add -----------------------
	-- Increments word count
	adder_out <= bram_out + 1;

	--------------------- Register ---------------------
	process(clk)
	begin
		if clk'event and clk='0' then
			reg_out <= char;
		end if;
	end process;

	----------------------- Exit -----------------------
	count_out <= bram_out;

end Behavioral;
