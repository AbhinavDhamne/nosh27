# STM32 Nucleo32 - Button Controlled LED Blinking in Low Power Mode

## Project Overview
This project demonstrates LED blinking control using a push button on the STM32L412KB Nucleo32 board. The system enters low power run mode to optimize power consumption while toggling an LED at different frequencies based on the number of button presses. The implemented logic follows a cyclic pattern:

1. **First button press**: LED blinks at **0.5 Hz** (1000ms ON/OFF cycle)
2. **Second button press**: LED blinks at **1 Hz** (500ms ON/OFF cycle)
3. **Third button press**: LED blinks at **2 Hz** (250ms ON/OFF cycle)
4. **Fourth button press**: LED **turns OFF**
5. **Fifth button press**: Cycle **repeats from the first state**

## Hardware Requirements
- **STM32L412KB Nucleo32** development board
- **Push button** (configured with an internal pull-up resistor)
- **10nF debouncing capacitor** (for noise filtering)
-  **10K Pull resistor** (for external pull up )
- 
- **LED** (connected to GPIOB pin 3 in nucleo32 board)

## Hardware Connections
| Component  | Connection  |
|------------|------------|
| Push Button | PB4 (D13)  |
| LED         | PB3        |
|Resistor | Across button and 3v3|
| Debounce Capacitor | Across Button |

## Software Implementation
### System Configuration
- The **low power run mode** is enabled using `HAL_PWREx_EnableLowPowerRunMode();` to minimize power consumption.
- The **system clock (HCLK)** is set to **2 MHz**.
- The **button** is configured as an **external interrupt (EXTI)** on PB4.
- The **LED toggling frequency** is controlled using `HAL_Delay()` in the main loop.
- **Debouncing** is handled using a **10nF capacitor** and a software-based timestamp check.

### Code Implementation
#### Interrupt Service Routine
The button press is handled using an **EXTI (External Interrupt) callback**:
```c
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    curr_time = HAL_GetTick();
    if (curr_time - prev_time > DEBOUNCE_PERIOD_MS) {
        state++;
        state = state % 4; // Ensures cyclic behavior
        switch (state) {
            case 0:
                delay_ms = 0; // LED OFF
                break;
            case 1:
                delay_ms = 1000; // 0.5 Hz
                break;
            case 2:
                delay_ms = 500; // 1 Hz
                break;
            case 3:
                delay_ms = 250; // 2 Hz
                break;
        }
        prev_time = HAL_GetTick();
    }
}
```

#### Main Loop
The LED toggles based on the current **state**:
```c
while (1) {
    if (state != 0) {
        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_3);
        HAL_Delay(delay_ms);
    } else {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
    }
}
```

## Power Optimization
- The **low power run mode** is enabled to operate at reduced clock frequency (2 MHz) and minimize power usage.
- The system only toggles the LED when necessary, reducing unnecessary computations.
- The push button interrupt avoids continuous polling, saving power.


## Note
The original problem statement specified the **NUCLEO-G070RB** board, but due to availability constraints, the **STM32L412KB Nucleo32** was used instead. The fundamental implementation remains the same, with slight modifications to accommodate the L4 series low-power features.

## Future Enhancements
- Implement deep sleep mode for further power optimization.
- Add **deeper sleep states** when the LED is OFF.
- Implement a RTOS based approach , to make the MCU go in sleep during Idle time.

----
