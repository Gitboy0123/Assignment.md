# ECEN-361 Lab-05:SPI & Logic Analyzer

     Student Name:  Fill-in HERE

## Introduction and Objectives of the Lab

This lab will require very little code development. The project as cloned from the GitHub-Classroom, runs without modification. You’ll be asked to use a logic analyzer to capture traces of the formats and compare their utility.

- Part 1: Physical observation of different digital serial communication protocols: I2C, SPI, and UART
- Part 2: Become familiar with a logic analyzer, capture & decoding capabilities.

For each of the parts, follow the instructions, then fill in answers to the questions. Expected answers are indicated with <mark>[*answer here*]</mark>.

## Part 1: Accept the Assignment, Download the repo, Run it

Same initial procedure as previous labs – get the laboratory into your STM32CubeIDE workspace, then clean/build/run it.

#### Step 1: Install a terminal emulator program

In order to interact with the program, you’ll need to bring up a serial terminal emulator, like PuTTY or Tera Term (windows) or screen (MAC).

Terminal emulator specs are (always the same for this class):

![A screenshot of a computer Description automatically generated](media/882ca964308ef4205da643920ba17e74.png)

Your COM: port will be found with DeviceManager or on a MAC as (/dev/tty…). If you install a terminal emulator like Tera Term, it will enumerate your COM ports for you. :)

With a terminal emulator running you’ll see the opening banner and a prompt to enter a line of text:

![A screenshot of a computer Description automatically generated](media/ed4011ac6d395eff2f0655ee1e7c0f97.png)

#### Step 2: Install the Saelae Logic Analyzer Software: [Logic 2](https://www.saleae.com/downloads/)

#### Step 3: Connect and label the probes for the SPI, UART and I2C:

To probe the SPI and I2C pins, you’ll use the following:

![](media/d5fa4df94685e0bb8c8541c91e4aabde.png)

Note that these pins are found from the STMCubeIDE configuration file. Opening will show (SPI1 for example):

![A screenshot of a computer Description automatically generated](media/5c9b944fc4426cec69226305fbca80bd.png)

Go to the GPIO Settings tab to see what pin names are assigned to each signal. You’ll manually change and configure I/O pins later and in other labs, so be sure you know how to find what signal is assigned to what I/O pin!

Plug in the logic analyzer and run the “Logic 2” program.

Connect probes from the Saelae Logic, using:

- GND (Black pins are on bottom)
- SPI1_SCK
- SPI1_MOSI
- SP1_NSS

![A circuit board with wires Description automatically generated](media/c881588d604d57405b3388adf3e42807.png)

Label them in the software. Make sure the inputs are Digital:

![A screenshot of a computer Description automatically generated](media/8497fae159b07b17ca4e4284654b25f0.png)

Program the Analyzer to decode SPI:

![A screenshot of a computer program Description automatically generated](media/f611c99d272dfe2e1a4e41603a4f9bb0.png)

Set the mode to trigger on the falling edge of SPI1_NSS:

![](media/3581b472df544c25626a2c5516a6995a.png)

Run a capture on the Logic2 (big green play button).

Enter some TXT into the TTY emulator:

![A screenshot of a computer Description automatically generated](media/e909f102dd64c11c5619232531556422.png)

Look at the results. Change the output formatting to be ASCII

![A screenshot of a computer Description automatically generated](media/fd7f6eb58b4646583564621bed24fdd3.png)

Experiment with this, send different codes, learn to use the tool, then answer the following questions:

## Part 1: Questions

* What is the default bitrate?  (time per bit, NOT per byte!)  -- Use the measurement tool (looks like a ruler): <mark>[*answer here*]</mark>

* How much time between each byte? <mark>[*answer here*]</mark>

* Which has more overhead:  USART or SPI? And why? <mark>[*answer here*]</mark>

The transfer rate is a function of a clock divider on the main clock (80Mhz). Default in your code is 128. Find the line that defines this: hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128; rate in **main.c.** Change this line to make the SPI transfer as fast as possible, compile, run, and capture the results and note what you see.

* What is fastest bit rate possible with this processor?  Equation? <mark>[*answer here*]</mark>

* Is there any problem capturing the fastest data of a USART channel with the logic analyzer? <mark>[*answer here*]</mark>

## Part 2: Doing the same with I2C

Attach the I2C Signals to look at the data coming out, then answer:

* What is the default bitrate? <mark>[*answer here*]</mark>

* How much time between each byte? <mark>[*answer here*]</mark>

* What is the value of the data coming out first?  It’s not like the others. <mark>[*answer here*]</mark>

## Part 3: Doing the same with a UART

Attach the USART3_TX and RX and sample again, looking at the data coming out, then answer:

* What is the default bitrate? Can this be estimated from the baud rate? What is the equation? <mark>[*answer here*]</mark>

* What is the max bitrate (or baud rate if you prefer) that is easily supported? When we say supported, this means that two devices can easily be set up to communicate. See [here](https://support.sbg-systems.com/sc/kb/latest/technology-insights/uart-baud-rate-and-output-rate) for more information. <mark>[*answer here*]</mark>

## Extra Credit Fun Ideas (5 pts max)

Document what you did as appropriate here with a small paragraph and show a video clip of the results.

* Add on the MultiFunction Board, and have it display the number of characters sent each time.

* Enable SPI2, as shown in the table above (thru the .IOC configuration file), and read code that gets sent from SPI1->SPI2

* Currently SPI is running in “blocking” (polled) mode.  Change it to run via an interrupt so the processor could be more efficient.

* Enable a ‘smart’ trigger on the Logic Analyzer so it doesn’t actually begin capturing data until specific data is decoded/found.

## Reference

The following is a capture of all three protocols.

Notes that you’ll need:

- The LogicAnalyzer can apply different protocols to different signals. In this case all three are shown
- I2C in master mode first transmits the address (here it’s 0x11), then waits for the ACK to send the data. Without a SLAVE no data is actually sent
- The UART is much slower and has overhead of start/stop bits

![A screen shot of a computer Description automatically generated](media/a23903139a8f27019f1dbef9024cb7b8.png)
