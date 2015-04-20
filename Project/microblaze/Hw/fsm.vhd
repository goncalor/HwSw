library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.std_logic_arith.all;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

entity fsm is
	port(
			-- Input
			clk	  : in std_logic;
			rst	  : in std_logic;
			start : in std_logic;

			-- Output
			sel_word : out std_logic_vector(1 downto 0);
			we       : out std_logic;
			comp_en  : out std_logic
			);
end fsm;

architecture Behavioral of fsm is
---------------- Auxilliary Variables ----------------
	type fsm_states is (s_initial, s_write_char_1, s_write_char_2, s_write_char_3, s_write_char_4, s_end);
	signal currstate, nextstate: fsm_states;

begin
--------------- Initialize Machine ------------------
	state_reg : process(clk, rst) -- Machine starts with reset signal
	begin
		if clk'event and clk = '1' then
			if rst = '1' then
				currstate <= s_initial;
			else
				currstate <= nextstate;
			end if;
		end if ;
	end process ; -- state_reg

----------------- Machine States --------------------
	state_comb : process(currstate, clk, start)
	begin
		nextstate <= currstate;
		we <= '0';
		comp_en <= '0';
		sel_word <= "00";

		case(currstate) is
			------- Initial State -------------
			when s_initial => -- Initial State
				if start = '1' then
					nextstate <= s_write_char_1;
					we <= '0';
					comp_en <= '0';
					sel_word <= "00";
				end if;

			------- Write Char to Memory ------
			when s_write_char_1 =>
			sel_word <= "00";
			we <= '1';
			comp_en <= '0';
			nextstate <= s_write_char_2;

			when s_write_char_2 =>
			sel_word <= "01";
			we <= '1';
			comp_en <= '1';
			nextstate <= s_write_char_3;

			when s_write_char_3 =>
			sel_word <= "10";
			we <= '1';
			comp_en <= '1';
			nextstate <= s_write_char_4;

			when s_write_char_4 =>
			sel_word <= "11";
			we <= '1';
			comp_en <= '1';
			nextstate <= s_end;

			------- Final State -------------
			when s_end =>
				nextstate <= s_initial;

		end case ;
	end process ; -- state_comb

end Behavioral;
