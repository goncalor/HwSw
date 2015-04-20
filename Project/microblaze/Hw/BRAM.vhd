library IEEE;
use IEEE.std_logic_1164.ALL;
use IEEE.std_logic_unsigned.ALL;

entity BRAM is
	Generic(
		ADDR_WIDTH : integer := 8;
		DATA_WIDTH : integer := 16
	);

	port(
		-- Input
		clk        : in std_logic;
		we         : in std_logic;
		read_addr  : in std_logic_vector(ADDR_WIDTH - 1 downto 0);
		read_addr2 : in std_logic_vector(ADDR_WIDTH - 1 downto 0);
		write_addr : in std_logic_vector(ADDR_WIDTH - 1 downto 0);
		data_in	   : in std_logic_vector(DATA_WIDTH - 1 downto 0);

		-- Output
		data_out  : out std_logic_vector(DATA_WIDTH - 1 downto 0);
		data_out2 : out std_logic_vector(DATA_WIDTH - 1 downto 0)
	);
end BRAM;

architecture Behavioral of BRAM is
	type MEM_TYPE is array (0 to (2**ADDR_WIDTH)-1) of STD_LOGIC_VECTOR(DATA_WIDTH - 1 downto 0);

	signal mem : MEM_TYPE := (others => X"0000");

begin

	---------------------- Writing ----------------------
	-- Writing is synchronous
	Write_Mem : process(clk, we)
	begin
		if clk'event and clk = '1' then
			if we = '1' then
				mem(conv_integer(write_addr)) <= data_in;
			end if ;
		end if ;
	end process ; -- Write_Mem

	-------------------- Reading -----------------------
	-- Reading is asynchronous
	data_out <= mem(conv_integer(read_addr));
	data_out2 <= mem(conv_integer(read_addr2));

end Behavioral;
