library IEEE;
use IEEE.std_logic_1164.ALL;

entity datapath is
	port(
				-- Input
				word					: in std_logic_vector(31 downto 0);
				sel_word			: in std_logic_vector(1 downto 0);
				char_count_in	: in std_logic_vector(15 downto 0);

				-- Output
				addr_write			: out std_logic_vector(7 downto 0);
				addr_read				: out std_logic_vector(7 downto 0);
				char_count_out	: out std_logic_vector(15 downto 0);
			);
end datapath;

architecture Behavioral of datapath is
---------------- Auxilliary Variables --------------
	signal char : std_logic_vector(7 downto 0) := (others => '0');

begin

------------------- Select Word --------------------
	char <= word(7 downto 0) when sel_word = "00" else
					word(15 downto 8) when sel_word = "01" else
					word(23 downto 16) when sel_word = "10" else
					word(31 downto 24);

------------------------ Add -----------------------
-- Increments word count
	char_count_out <= char_count_in + 1;

----------------------- Exit -----------------------
	addr_read <= char;
	addr_write <= char;

end Behavioral;
