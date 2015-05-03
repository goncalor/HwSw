library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.std_logic_arith.all;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

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
entity stats_acc is
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
	);
end entity; -- stats_acc

architecture Behavioral of stats_acc is

	component datapath is
		port(
			-- Input
			clk      : in std_logic;
			word     : in std_logic_vector(31 downto 0);
			sel_char : in std_logic_vector(1 downto 0);
			we       : in std_logic;
			comp_en  : in std_logic;

			-- Output
			count_out  : out std_logic_vector(15 downto 0);
			count_out2 : out std_logic_vector(15 downto 0);
			curr_char  : out std_logic_vector(7 downto 0)
		);
	end component;

	---------------- Auxilliary Variables ----------------
	type fsm_states is (s_initial, s_save_END, s_count_1_special, s_count_1, s_count_2, s_count_3, s_count_4, s_report, s_report_1, s_end);
	signal currstate, nextstate: fsm_states;

	signal word              : std_logic_vector(31 downto 0);
	signal sel_word          : std_logic_vector(1 downto 0);
	signal we                : std_logic;
	signal comp_en           : std_logic;
	signal curr_char         : std_logic_vector(7 downto 0);

	signal char_END          : std_logic_vector(7 downto 0);
	signal report_counter    : std_logic_vector(7 downto 0) := (others => '0');
	signal end_count         : std_logic;
	signal all_sent          : std_logic;
	signal prev_exists       : std_logic;

	signal FSL_S_Read_aux    : std_logic;
	signal FSL_S_Data_aux    : std_logic_vector(0 to 31);
	signal FSL_S_Control_aux : std_logic;
	signal FSL_S_Exists_aux  : std_logic;
	signal FSL_M_Write_aux   : std_logic;
	signal FSL_M_Data_aux    : std_logic_vector(0 to 31);
	signal FSL_M_Control_aux : std_logic;
	signal FSL_M_Full_aux    : std_logic;
	signal en_counter        : std_logic;

begin

	Inst_datapath : datapath
	port map(
		-- Input
		clk      => FSL_Clk,
		word     => word,
		sel_char => sel_word, -- vem da máquinda de estados
		we       => we, -- vem da máquina de estados
		comp_en  => comp_en,

		-- Output
		count_out  => FSL_M_Data_aux(0 to 15),
		count_out2 => FSL_M_Data_aux(16 to 31),
		curr_char  => curr_char
	);

	--------------- Initialize Machine ------------------
	state_reg : process(FSL_clk, FSL_Rst) -- Machine starts with reset signal
	begin
		if FSL_clk'event and FSL_clk = '1' then
			if FSL_Rst = '1' then
				currstate <= s_initial;
			else
				currstate <= nextstate;
			end if;
		end if ;
	end process ; -- state_reg

	----------------- Machine States --------------------
	state_comb : process(currstate, FSL_clk, FSL_S_Exists_aux, FSL_S_Control_aux, FSL_M_Full_aux, end_count, all_sent, prev_exists, en_counter)
	begin
		nextstate <= currstate;
		we <= '0';
		comp_en <= '0';
		sel_word <= "00";
		FSL_S_Read_aux <= '0';
		FSL_M_Write_aux <= '0';
		FSL_M_Control_aux <= '0';
		en_counter <= '0';

		case(currstate) is
			------- Initial State -------------
			when s_initial => -- Initial State
				if FSL_S_Exists_aux = '1' then
					FSL_S_Read_aux <= '1';
					if FSL_S_Control_aux = '1' then
						nextstate <= s_save_END;
					end if;
				end if;

			------ save end of file char ------
			when s_save_END =>
				nextstate <= s_count_1_special;
			-- char_END is saved in a process bellow

			------- Write Char to Memory ------
			when s_count_1_special =>	-- first char. comp_en should be 0
				if FSL_S_Exists_aux = '1' then
					sel_word <= "00";
					we <= '1';
					comp_en <= '0';
					FSL_S_Read_aux <= '1';
					if end_count = '1' then
						nextstate <= s_report;
						FSL_M_Control_aux <= '1';
					else
						nextstate <= s_count_2;
					end if;
				end if ;

			when s_count_1 =>
				if FSL_S_Exists_aux = '1' then
					sel_word <= "00";
					we <= '1';
					-- only enable compare if the prev char is valid.
					-- otherwise if should not be considered
					if prev_exists = '1' then
						comp_en <= '1';
					end if;
					FSL_S_Read_aux <= '1';
					if end_count = '1' then
						nextstate <= s_report;
						FSL_M_Control_aux <= '1';
					else
						nextstate <= s_count_2;
					end if;
				end if;

			when s_count_2 =>
				sel_word <= "01";
				we <= '1';
				comp_en <= '1';
				if end_count = '1' then
					nextstate <= s_report;
					FSL_M_Control_aux <= '1';
				else
					nextstate <= s_count_3;
				end if;

			when s_count_3 =>
				sel_word <= "10";
				we <= '1';
				comp_en <= '1';
				if end_count = '1' then
					nextstate <= s_report;
					FSL_M_Control_aux <= '1';
				else
					nextstate <= s_count_4;
				end if;

			when s_count_4 =>
				sel_word <= "11";
				we <= '1';
				comp_en <= '1';
				if end_count = '1' then
					nextstate <= s_report;
					FSL_M_Control_aux <= '1';
				else
					nextstate <= s_count_1;
				end if;

			---- Report results to Master ---
			when s_report =>
				nextstate <= s_report_1;
				if FSL_M_Full_aux = '0' then
					en_counter <= '1';
				end if ;

			when s_report_1 =>

				if FSL_M_Full_aux = '0' then
					FSL_M_Write_aux <= '1';
					en_counter <= '1';
					if all_sent = '1' then
						nextstate <= s_end;
					end if ;
				end if;

			------- Final State -------------
			when s_end =>
				nextstate <= s_initial;
				FSL_M_Write_aux <= '0';
		end case;
	end process; -- state_comb

	-- update Counter for report phase
	Counter : process(report_counter, FSL_Clk)
	begin
		if FSL_Clk'event and FSL_Clk = '1' then
			if en_counter = '1' then
				report_counter <= report_counter + 2;
			elsif (currstate /= s_report and currstate /= s_report_1) then
				report_counter <= X"00";
			end if ;
		end if;
	end process ; -- Counter

	-- save char_END
	process(FSL_clk)
	begin
		if FSL_clk'event and FSL_clk = '1' then
			if FSL_S_Control_aux = '1' then
				char_END <= FSL_S_Data_aux(0 to 7);
			end if;
		end if;
	end process;

	-- save prev Exists signal
	process(FSL_clk)
	begin
		if FSL_clk'event and FSL_clk = '1' then
			prev_exists <= FSL_S_Exists_aux;
		end if;
	end process;

	-- registers at the input and output of the accelerator
	process(FSL_clk)
	begin
		if FSL_clk'event and FSL_clk = '1' then
			-- output
			FSL_M_Data    <= FSL_M_Data_aux;
			FSL_M_Control <= FSL_M_Control_aux;
		end if;
	end process;

	-- input
	FSL_S_Data_aux    <= FSL_S_Data;
	FSL_S_Control_aux <= FSL_S_Control;
	FSL_S_Exists_aux  <= FSL_S_Exists;
	FSL_M_Full_aux    <= FSL_M_Full;

	-- output
	FSL_S_Read    <= FSL_S_Read_aux;
	FSL_M_Write   <= FSL_M_Write_aux;

	end_count <= '1' when curr_char = char_END else '0';

	-- when reporting the word is substituted for report_counter
	word <= x"000000" & report_counter when currstate = s_report or currstate = s_report_1 else FSL_S_Data_aux;
	all_sent <= '1' when report_counter = "11111110" else '0';	-- 254

end Behavioral;
