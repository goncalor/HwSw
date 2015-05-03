
LIBRARY ieee;
USE ieee.std_logic_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--USE ieee.numeric_std.ALL;

ENTITY datapath_tb IS
	END datapath_tb;

ARCHITECTURE behavior OF datapath_tb IS 

	-- Component Declaration for the Unit Under Test (UUT)

	COMPONENT datapath
		PORT(
			clk : IN  std_logic;
			word : IN  std_logic_vector(31 downto 0);
			sel_char : IN  std_logic_vector(1 downto 0);
			we : IN  std_logic;
			comp_en : IN  std_logic;
			count_out : OUT  std_logic_vector(15 downto 0)
		);
	END COMPONENT;


	--Inputs
	signal clk : std_logic := '0';
	signal word : std_logic_vector(31 downto 0) := (others => '0');
	signal sel_char : std_logic_vector(1 downto 0) := (others => '0');
	signal we : std_logic := '0';
	signal comp_en : std_logic := '0';

	--Outputs
	signal count_out : std_logic_vector(15 downto 0);

	-- Clock period definitions
	constant clk_period : time := 10 ns;

BEGIN

	-- Instantiate the Unit Under Test (UUT)
	uut: datapath PORT MAP (
		clk => clk,
		word => word,
		sel_char => sel_char,
		we => we,
		comp_en => comp_en,
		count_out => count_out
	);

	-- Clock process definitions
	clk_process :process
	begin
		clk <= '0';
		wait for clk_period/2;
		clk <= '1';
		wait for clk_period/2;
	end process;


	-- Stimulus process
	stim_proc: process
	begin		
		-- hold reset state for 100 ns.
		wait for 100 ns;	

		wait for clk_period*10;

		-- insert stimulus here 

		we <= '1',
			'0' after 13*clk_period; -- perform reads only, to check values

		comp_en <= '0',
			'1' after 1*clk_period;

		word <= x"03020100",
			x"07060504" after 4*clk_period,
			x"07070708" after 8*clk_period,
			x"09090909" after 12*clk_period,
			x"07070707" after 16*clk_period;

		sel_char <= "00",
			"01" after 1*clk_period,
			"10" after 2*clk_period,
			"11" after 3*clk_period,
			"00" after 4*clk_period,
			"01" after 5*clk_period,
			"10" after 6*clk_period,
			"11" after 7*clk_period,
			"00" after 8*clk_period,
			"01" after 9*clk_period,
			"10" after 10*clk_period,
			"11" after 11*clk_period;

		wait;
	end process;

END;
