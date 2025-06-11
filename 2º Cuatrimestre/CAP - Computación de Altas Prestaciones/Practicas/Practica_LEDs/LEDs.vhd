----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 11.03.2025 18:38:30
-- Design Name: 
-- Module Name: S1 - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;
-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity S1 is
    Port ( RST: in STD_LOGIC;
           CLK : in STD_LOGIC;
           LED : out STD_LOGIC_VECTOR (15 downto 0));
end S1;

architecture Behavioral of S1 is
    constant max_count : INTEGER := 3500000;
    signal CONT : integer := 0;
    signal LED_value : STD_LOGIC_VECTOR (15 downto 0) := "0000000000000011";  -- 2 bits activos
    signal direction : STD_LOGIC := '1'; -- 1 derecha, 0 izquierda

begin
    LED <= LED_value;
    
    process (CLK)
    begin 
        if rising_edge(CLK) then
            if CONT < max_count then 
                CONT <= CONT + 1;
            else 
                CONT <= 0;
            end if;
        end if;
        if RST = '1' then
            CONT <= 0;
        end if;
    end process;
    
    process (CLK)
    begin 
        if rising_edge(CLK) then
            if CONT = 0 then
                if direction = '1' then
                    LED_value <= LED_value(13 downto 0) & "00";  -- Mover 2 bits a la derecha
                    if LED_value(13) = '1' then
                        direction <= '0';  -- Cambiar dirección a izq
                    end if;
                else
                    LED_value <= "00" & LED_value(15 downto 2);  -- Mover 2 bits a la izquierda
                    if LED_value(2) = '1' then
                        direction <= '1';  -- Cambiar dirección a drch
                    end if;
                end if;
            end if;
            if RST = '1' then
                direction <= '1';
                LED_value <= "0000000000000011";
            end if;
        end if;
    end process;
    
end Behavioral;

