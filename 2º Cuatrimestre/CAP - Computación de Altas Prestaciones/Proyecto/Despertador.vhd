library IEEE; 
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

entity S1 is
    Port ( 
        CLK      : in  STD_LOGIC;
        SSEG_CA  : out STD_LOGIC_VECTOR (7 downto 0);
        SSEG_AN  : out STD_LOGIC_VECTOR (3 downto 0);
        LED      : out STD_LOGIC_VECTOR (15 downto 0);
        RST      : in  STD_LOGIC;
        SW       : in  STD_LOGIC;
        BTN      : in  STD_LOGIC_VECTOR (3 downto 0)
    );
end S1;

architecture Behavioral of S1 is

    type digit_array is array (0 to 9) of STD_LOGIC_VECTOR(7 downto 0);
    constant digits : digit_array := (
        "11000000", -- 0
        "11111001", -- 1
        "10100100", -- 2
        "10110000", -- 3
        "10011001", -- 4
        "10010010", -- 5
        "10000010", -- 6
        "11111000", -- 7
        "10000000", -- 8
        "10011000"  -- 9
    );

    signal refresco            : INTEGER range 0 to 3 := 0;
    signal contador_refresco   : INTEGER := 0;

    signal segundos            : INTEGER range 0 to 59 := 0;
    signal minutos             : INTEGER range 0 to 59 := 25;
    signal horas               : INTEGER range 0 to 23 := 20;
    signal contador_segundos   : INTEGER := 0;

    signal alarm_horas         : INTEGER range 0 to 23 := 0;
    signal alarm_minutos       : INTEGER range 0 to 59 := 0;

    signal backup_horas        : INTEGER range 0 to 23 := 0;
    signal backup_minutos      : INTEGER range 0 to 59 := 0;

    type display_array is array (0 to 3) of STD_LOGIC_VECTOR(7 downto 0);
    signal display             : display_array := (others => (others => '0'));

    signal btn_prev            : STD_LOGIC_VECTOR(3 downto 0) := (others => '0');
    signal LED_value           : STD_LOGIC_VECTOR (15 downto 0) := (others => '0');  
    signal alarm_mode          : STD_LOGIC := '0';
    signal rst_prev            : STD_LOGIC := '0'; -- Para detectar flanco de subida
    signal alarma_activa       : STD_LOGIC := '0'; -- Estado de la alarma encendida

begin
    LED <= LED_value;

    process (CLK)
    begin
        if rising_edge(CLK) then

            -- Cambio de modo con flanco de subida del botón RST
            if RST = '1' and rst_prev = '0' then
                alarm_mode <= not alarm_mode;

                if alarm_mode = '0' then
                    -- Entrando a alarm_mode: guardar la hora actual
                    backup_horas   <= horas;
                    backup_minutos <= minutos;

                    LED_value <= "0000000011111111";
                else
                    -- Saliendo de alarm_mode:
                    -- Guardar configuración como hora de alarma
                    alarm_horas   <= horas;
                    alarm_minutos <= minutos;

                    -- Restaurar hora normal
                    horas   <= backup_horas;
                    minutos <= backup_minutos;

                    LED_value <= (others => '0');
                end if;
            end if;
            rst_prev <= RST;

            -- Si NO estamos en alarm_mode, el reloj avanza
            if alarm_mode = '0' then
                contador_segundos <= contador_segundos + 1;
                if contador_segundos = 100000000 then
                    contador_segundos <= 0;

                    if segundos = 59 then
                        segundos <= 0;
                        if minutos = 59 then
                            minutos <= 0;
                            if horas = 23 then
                                horas <= 0;
                            else
                                horas <= horas + 1;
                            end if;
                        else
                            minutos <= minutos + 1;
                        end if;
                    else
                        segundos <= segundos + 1;
                    end if;
                end if;

                -- Lógica alarma y LEDs según SW y coincidencia hora
                if SW = '1' then
                    if (horas = alarm_horas) and (minutos = alarm_minutos) then
                        alarma_activa <= '1';    -- activar alarma
                    end if;
                else
                    alarma_activa <= '0';        -- apagar alarma si SW desactivado
                end if;

                -- LED_value según alarma_activa
                if alarma_activa = '1' then
                    LED_value <= (others => '1');
                else
                    LED_value <= (others => '0');
                end if;
            end if;

            -- Si estamos en alarm_mode, configurar hora con botones
            if alarm_mode = '1' then
                if BTN(0) = '1' and btn_prev(0) = '0' then -- Aumentar minutos unidades
                    if (minutos mod 10) = 9 then
                        minutos <= minutos - 9;
                    else
                        minutos <= minutos + 1;
                    end if;
                end if;

                if BTN(1) = '1' and btn_prev(1) = '0' then -- Aumentar minutos decenas
                    if minutos + 10 > 59 then
                        minutos <= minutos mod 10;
                    else
                        minutos <= minutos + 10;
                    end if;
                end if;

                if BTN(2) = '1' and btn_prev(2) = '0' then -- Aumentar horas unidades
                    if (horas mod 10) = 9 or horas = 23 then
                        horas <= horas - (horas mod 10);
                    else
                        horas <= horas + 1;
                    end if;
                end if;

                if BTN(3) = '1' and btn_prev(3) = '0' then -- Aumentar horas decenas
                    if horas + 10 > 23 then
                        horas <= horas mod 10;
                    else
                        horas <= horas + 10;
                    end if;
                end if;
            end if;

            btn_prev <= BTN;
        end if;
    end process;

    -- Actualización de display (HH:mm)
    process (CLK)
    begin
        if rising_edge(CLK) then
            display(0) <= digits(minutos mod 10);          
            display(1) <= digits((minutos / 10) mod 10);    
            display(2) <= digits(horas mod 10);            
            display(3) <= digits((horas / 10) mod 10);     
        end if;
    end process;

    -- Refresco de display
    process (CLK)
    begin
        if rising_edge(CLK) then
            contador_refresco <= contador_refresco + 1;
            if contador_refresco = 50000 then
                contador_refresco <= 0;
                refresco <= (refresco + 1) mod 4;
            end if;
        end if;
    end process;

    -- Multiplexado del display
    process (refresco)
    begin
        case refresco is
            when 0 =>
                SSEG_AN <= "1110";
                SSEG_CA <= display(0);
            when 1 =>
                SSEG_AN <= "1101";
                SSEG_CA <= display(1);
            when 2 =>
                SSEG_AN <= "1011";
                SSEG_CA <= display(2);
            when 3 =>
                SSEG_AN <= "0111";
                SSEG_CA <= display(3);
            when others =>
                SSEG_AN <= "1111";
                SSEG_CA <= "11111111";
        end case;
    end process;

end Behavioral;
