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
    Port ( 
           CLK      : in  STD_LOGIC;  -- Señal de reloj
           SSEG_CA  : out STD_LOGIC_VECTOR (7 downto 0);
           SSEG_AN  : out STD_LOGIC_VECTOR (3 downto 0)
           );
end S1;

architecture Behavioral of S1 is

    -- Letras
    constant letra_S : STD_LOGIC_VECTOR(7 downto 0) := "10010010"; -- S
    constant letra_O : STD_LOGIC_VECTOR(7 downto 0) := "11000000"; -- O
    constant letra_L : STD_LOGIC_VECTOR(7 downto 0) := "11000111"; -- L
    constant letra_R : STD_LOGIC_VECTOR(7 downto 0) := "11001110"; -- R
    constant letra_A : STD_LOGIC_VECTOR(7 downto 0) := "10001000"; -- A
    constant letra_C : STD_LOGIC_VECTOR(7 downto 0) := "11000110"; -- C
    constant letra_espacio : STD_LOGIC_VECTOR(7 downto 0) := "11111111"; -- ' '
    
    -- Numero de letras que se mostrarán en el display
    constant n_letras : INTEGER := 7;

    -- Tipos buffers
    type buffer_type is array (0 to 3) of STD_LOGIC_VECTOR(7 downto 0);
    type full_buffer_type is array (0 to n_letras-1) of STD_LOGIC_VECTOR(7 downto 0);

    -- Buffers
    signal buffer_array : buffer_type := (letra_L, letra_R, letra_A, letra_C);
    signal full_buffer  : full_buffer_type := (letra_espacio, letra_S, letra_O, letra_L, letra_R, letra_A, letra_C);

    signal contador : INTEGER := 0;
    signal refresco : INTEGER range 0 to 3 := 0;  -- Controla qué dígito está activo
    signal contador_move : INTEGER := 0; -- Contador para mover el buffer 
    signal index : INTEGER range 0 to n_letras := 0; -- Índice de desplazamiento

begin

    -- Refresco del display
    process (CLK)
    begin
        if rising_edge(CLK) then
            contador <= contador + 1;
            if contador = 50000 then  
                contador <= 0;
                refresco <= refresco + 1;
                if refresco = 4 then
                    refresco <= 0;
                end if;
            end if;
        end if;
    end process;

    -- Desplazar el buffer 
    process (CLK)
    begin
        if rising_edge(CLK) then
            contador_move <= contador_move + 1;
            if contador_move = 35000000 then  -- 0,35 segundos
                contador_move <= 0;
                index <= (index - 1 + n_letras) mod n_letras;
                buffer_array <= (full_buffer(index), full_buffer((index + 1) mod n_letras), full_buffer((index + 2) mod n_letras), full_buffer((index + 3) mod n_letras));
            end if;
        end if;
    end process;

    -- Multiplexación de los 4 dígitos
    process (refresco)
    begin
        case refresco is
            when 0 => 
                SSEG_AN <= "1110";  
                SSEG_CA <= buffer_array(0);
            when 1 => 
                SSEG_AN <= "1101"; 
                SSEG_CA <= buffer_array(1);
            when 2 => 
                SSEG_AN <= "1011";  
                SSEG_CA <= buffer_array(2);
            when 3 => 
                SSEG_AN <= "0111"; 
                SSEG_CA <= buffer_array(3);
        end case;
    end process;

end Behavioral;
