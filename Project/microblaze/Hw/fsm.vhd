library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

entity fsm is
	port(
			-- Input
			clk	: in std_logic;
			rst	: in std_logic;

			-- Output
			sel_word : out std_logic_vector(1 downto 0);
			);
end fsm;

architecture Behavioral of fsm is
---------------- Auxilliary Variables ----------------
	type fsm_states is (s_initial, s_write_char, s_end);
	signal currstate, nextstate: fsm_states;
	integer count;
	integer max_words := 4;

begin
--------------- Initialize Machine ------------------
	state_reg : process(clk, rst) -- Machine starts with reset signal
	begin
		if rst = '1' then
			currstate <= s_initial;
		elsif clk'event and clk = '1' then
			currstate <= nextstate;
		end if;
	end process ; -- state_reg

----------------- Machine States --------------------
	state_comb : process(currstate)
	begin
		nextstate <= currstate;
		count := 0;

		case(currstate) is
			------- Initial State -------------
			when s_initial => -- Initial State
				if count != max_words then
					nextstate <= s_write_char;
					count := count + 1;
				else
					nextstate <= s_end;
					count := 0;
				end if;

			------- Write Char to Memory ------
			when s_write_char =>
				case(count) is
					when 0 =>
						nextstate <= s_initial;
						sel_word <= "00";
					when 1 =>
						nextstate <= s_initial;
						sel_word <= "01";
					when 2 =>
						nextstate <= s_initial;
						sel_word <= "10";
					when others =>
						nextstate <= s_initial;
						sel_word <= "11";
				end case ;

			------- Final State -------------
			when s_end =>
				nextstate <= s_initial;

		end case ;
	end process ; -- state_comb

end Behavioral;
