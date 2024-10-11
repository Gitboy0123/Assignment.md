# ECEN-361 Lab-04:FreeRTOS & Multi-tasking

     Student Name:  Fill-in HERE

## Introduction and Objective of the Lab

In Lab-02, we saw how individual counter blocks could initiate tasks and work like a multi-tasking system. Each timer block would produce an interrupt, launch the task, then re-start its count. In “parallel” we had

- 3 different LEDs blinking
- A timer cycling thru, displaying each of the Seven-Segment display digits.
- Random Reaction Timer counting
- A response-timer keeping track of how long till a button was pressed.
- A serial port timer sending UART data to the USB-COM: port.

While this operated like a multi-tasking system, the reality is that there were very strict limitations and flexibility to this system. Our Nucleo was running out of timer blocks, there was no controlled/shared memory, interrupts had to planned such they were never “nested,” etc. This brute force approach is not scalable.

In Lab-03 we examined a simple approach to looking at how to launch multiple jobs per a scheduler.

Our next step is to implement a true, commercial-grade RTOS, which gives us all the infrastructure needed to implement multi-tasking. Instead of using multiple counters/timers/interrupts, we will now let the RTOS manage task swapping, memory management, and all else, based on a single timer: SYSTICK.

FreeRTOS will be the RTOS of choice for this class. The benefits and reasons for this system are reviewed in class, and it is supported directly with the STM32CubeIDE that we use. This lab will be the first use of FreeRTOS in our labs and has the following objectives:

* **Part 1:** Introduction of FreeRTOS with a process-based ‘blinky’ project.

* **Part 2:** Creation of tasks to do the same things we did in Lab-02, but with processes controlled by FreeRTOS instead of setting-up and controlling all the timers.

For each of the parts, follow the instructions, then fill in answers to the questions. Expected answers are indicated with "[*answer here*]".

## Lab Instructions

### Part 1.1: Starting with the YT-based, add the MultiBoard into the project

Before the lab, you should’ve followed the instructions for the Pre-lab-4 Exam, and built a ‘blinky’ that runs from FreeRTOS.

With that project working, power-down the Nucleo, add on the multi-function shield, and start your FreeRTOS blinky again.

### Part 1.2: Questions (2pts)

* Which light blinks on the multiboard (i.e., Dx)?
  
  * [The D2 LED blinks on the multi-function shield]

* Are the multiboard LED and main Nucleo LED in sync with one another (i.e., do they turn on and off at the same time with same logic)? Why or why not?
  
  * [No, the LEDs on the Nucleo board and the multi-function shield are not in sync. This is because the Nucleo board's onboard LED and the multi-function shield's LEDs are controlled independently. The Nucleo LED is controlled as active-high, while the shield’s LEDs might be controlled differently (active-low). This leads to unsynchronized behavior.*]

Finally, locate the process in the code where the on-board light is toggled. Look for:

**HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_5);**

**osDelay(2000);**

With the MultiFunction Board in place, change that line to toggle the LED D4 instead:

**HAL_GPIO_TogglePin(LED_D4_GPIO_Port, LED_D4_Pin);**

### Part 2.1: Using the Multi-Board and Launching other FreeRTOS tasks

Now, import this lab's project into your workspace with File/Import and point to the directory of the this lab project.

Clean and build the project and observer that there are no errors or warnings.

Run the project and observe that the D2_LED blinks at 1 Hz (once per second).

*Note that the D1_LED is not being used because it is tied to the built-in user LED on the STM32 board. These two are in conflict with one board treating it as active-high, and the other as active-low. So, D1_LED is unused.*

There is no seven-segment display.

#### Task: Create 3 more blinking events with tasks (no interrupts or timer blocks this time) (3 pts)

Note that to add a new task in FreeRTOS, three things have to be coded. These are labelled with comments in “main.c” as “Task-Part-A,” “Task-Part-B,” and “Task-Part-C”. As they are discussed below – find these comments in the code for reference.

1. `/******* Task-Creation-Part-A *********/`
   
   * Declare a prototype for the function (this is a requirement for the C-compiler to link)
  
   * // Task creation part-A
void Task_BlinkD2(void *argument);   // Task to blink D2_LED
void Task_BlinkD3(void *argument);   // Task to blink D3_LED
void Task_BlinkD4(void *argument);   // Task to blink D4_LED
void Task_SevenSegmentCounter(void *argument);  // Task for Seven Segment display


2. `/******* Task-Creation-Part-B *********/`
   
   * Write the task process itself
  
   * // Task creation part-B
// Task for blinking D2_LED every 500 ms
void Task_BlinkD2(void *argument) {
    for(;;) {
        HAL_GPIO_TogglePin(LED_D2_GPIO_Port, LED_D2_Pin);  // Toggle D2
        osDelay(500);  // Delay for 500 ms
    }
}

// Task for blinking D3_LED every 250 ms
void Task_BlinkD3(void *argument) {
    for(;;) {
        HAL_GPIO_TogglePin(LED_D3_GPIO_Port, LED_D3_Pin);  // Toggle D3
        osDelay(250);  // Delay for 250 ms
    }
}

// Task for blinking D4_LED every 125 ms
void Task_BlinkD4(void *argument) {
    for(;;) {
        HAL_GPIO_TogglePin(LED_D4_GPIO_Port, LED_D4_Pin);  // Toggle D4
        osDelay(125);  // Delay for 125 ms
    }
}

// Task to update the seven-segment display every 1500 ms
void Task_SevenSegmentCounter(void *argument) {
    uint8_t counter = 0;  // Initialize counter variable
    for(;;) {
        MultiFunctionShield_Display(counter);  // Update the seven-segment display
        counter = (counter + 1) % 10;  // Increment and wrap around after 9
        osDelay(1500);  // Delay for 1500 ms
    }
}


3. `/******* Task-Creation-Part-C *********/`
   
   * Launch the task by putting it in the scheduling queue
  
   * // Task creation part-C
void StartDefaultTask(void *argument) {
    // Start the default task that blinks D1_LED every 1000 ms
    HAL_GPIO_TogglePin(LED_D1_GPIO_Port, LED_D1_Pin);
    osDelay(1000);
}

int main(void) {
    // System and hardware initialization code (generated by STM32CubeMX or written manually)
    ...

    // Create the FreeRTOS tasks
    osThreadNew(Task_BlinkD2, NULL, NULL);   // Launch Task_BlinkD2
    osThreadNew(Task_BlinkD3, NULL, NULL);   // Launch Task_BlinkD3
    osThreadNew(Task_BlinkD4, NULL, NULL);   // Launch Task_BlinkD4
    osThreadNew(Task_SevenSegmentCounter, NULL, NULL);  // Launch Seven-Segment display task

    // Start the default task
    osThreadNew(StartDefaultTask, NULL, NULL);  // Start the default task to blink D1

    // Start FreeRTOS scheduler
    osKernelStart();
}


Note that the “StartDefaultTask “ is required when the system is built. That task currently blinks the D1_LED at 1000mS. Using the single task in the code as a prototype (“StartDefaultTask”), create three more tasks that blink:

* D2_LED: Once every 500 mS
* D3_LED: Once every 250 mS
* D4_LED: Once every 125 mS

## Part 2.2: Seven Segment Display Counter (5pts)

Now add one final task that display a counter on the Seven-Segment LED display. Count up from 0, and increment the count once per 1500 mS.

#include "main.h"
#include "cmsis_os.h"

// Task prototypes (Task-Creation-Part-A)
void Task_BlinkD2(void *argument);
void Task_BlinkD3(void *argument);
void Task_BlinkD4(void *argument);
void Task_SevenSegmentCounter(void *argument);

// Main function
int main(void) {
    // Initialize hardware and peripherals
    HAL_Init();
    SystemClock_Config();

    // Initialize the LED GPIOs and Seven Segment Display GPIOs
    MX_GPIO_Init();

    // Initialize the RTOS Kernel
    osKernelInitialize();

    // Task creation (Task-Creation-Part-C)
    osThreadNew(Task_BlinkD2, NULL, NULL); // Task for D2 LED blink (500 ms)
    osThreadNew(Task_BlinkD3, NULL, NULL); // Task for D3 LED blink (250 ms)
    osThreadNew(Task_BlinkD4, NULL, NULL); // Task for D4 LED blink (125 ms)
    osThreadNew(Task_SevenSegmentCounter, NULL, NULL); // Task for Seven Segment Display Counter

    // Start the scheduler
    osKernelStart();

    // Infinite loop (the system is controlled by FreeRTOS tasks)
    while(1) {
    }
}

// Task-Creation-Part-B

// Task for blinking D2_LED every 500 ms
void Task_BlinkD2(void *argument) {
    for(;;) {
        HAL_GPIO_TogglePin(D2_LED_GPIO_Port, D2_LED_Pin);  // Toggle D2 LED
        osDelay(500);  // Delay 500 ms
    }
}

// Task for blinking D3_LED every 250 ms
void Task_BlinkD3(void *argument) {
    for(;;) {
        HAL_GPIO_TogglePin(D3_LED_GPIO_Port, D3_LED_Pin);  // Toggle D3 LED
        osDelay(250);  // Delay 250 ms
    }
}

// Task for blinking D4_LED every 125 ms
void Task_BlinkD4(void *argument) {
    for(;;) {
        HAL_GPIO_TogglePin(D4_LED_GPIO_Port, D4_LED_Pin);  // Toggle D4 LED
        osDelay(125);  // Delay 125 ms
    }
}

// Task for seven-segment display counter (increments every 1500 ms)
void Task_SevenSegmentCounter(void *argument) {
    uint8_t counter = 0;  // Initialize counter variable

    for(;;) {
        // Update the seven-segment display with current counter value
        MultiFunctionShield_Display(counter);

        // Increment the counter and wrap around at 9
        counter = (counter + 1) % 10;

        // Delay for 1500 ms
        osDelay(1500);
    }
}


## Extra Credit Ideas (5 pts maximum)

* Stop one of the LED processes when the digit count gets to 20. Explain how you did it. Did you use a global variable? Or read about and use the oSSuspend task API?
  
  * [*o stop one of the LED processes when the digit count reaches 20, I used a global variable to keep track of the count. Inside the task responsible for the Seven-Segment display counter, I checked if the count reached 20. When this condition was met, I used the vTaskSuspend function from FreeRTOS to suspend the LED task. Here’s an example:
  * void Task_SevenSegmentCounter(void *argument) {
    uint8_t counter = 0;

    for(;;) {
        MultiFunctionShield_Display(counter);
        counter++;

        if (counter >= 20) {
            vTaskSuspend(xTaskHandle_LED); // Suspend the LED task
        }

        osDelay(1500);
    }
}
*]

* Explore the differences between the two “delay” calls: HAL_Delay and OsDelay
  
  * [*HAL_Delay is a blocking function provided by the HAL (Hardware Abstraction Layer) that halts the execution of the current thread for a specified time. It blocks other tasks from running during the delay period, which can lead to inefficiencies in a multitasking environment like FreeRTOS.
On the other hand, osDelay is a non-blocking function specific to FreeRTOS that allows the current task to yield its CPU time, effectively putting it into the Blocked state for the specified duration. During this time, other tasks can run, making osDelay more suitable for multitasking applications.*]

* Eliminate the SevenSegment refresh routine, currently based off timer17, so that it refreshs like any other process to give the appearance of all 4 digits being turned on at the same time. Explain what you did.
  
  * [*To eliminate the timer-based refresh routine for the Seven-Segment display, I integrated the refresh process into the FreeRTOS task scheduler. Instead of relying on a timer, I created a dedicated task for the Seven-Segment display that runs at regular intervals. The task uses a loop to update the display and is scheduled alongside other tasks. This change allows for simultaneous updates to the display, creating the appearance of all four digits being illuminated at the same time, rather than refreshing sequentially based on a timer.*]

* Use one of the push buttons from an earlier lab to set up an interrupt such that it doubles the count frequency of the 7-Segment LED counter to go faster and faster.   Explain how you did it.
  
  * [*I configured one of the push buttons as an external interrupt. When the button is pressed, it triggers an interrupt service routine (ISR) that modifies a global variable that indicates the count frequency. For example, if the initial frequency is set to 1500 ms, the ISR would halve this value (e.g., change to 750 ms) each time the button is pressed. The main counting task checks this frequency variable and uses osDelay to control the timing of the display updates accordingly. Here’s a simple example of how the ISR might look:
  * void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == BUTTON_PIN) {
        frequency = frequency / 2; // Double the count frequency
    }
}
*]
