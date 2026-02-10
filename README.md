# README

This is my solution to a problem on [Codewars](https://www.codewars.com/kata/59f9cad032b8b91e12000035) which I started working on at some point in 2024 when I was a high school senior.

I abandoned this project at some point for reasons I can't remember.
Now that I'm doing a CS BS degree at RIT, I've decided to come back to this project.
I've always looked back fondly at my time working on this so much so that it inspired me to take RIT's CSCI-742 Compiler Construction course this semester.
Looking back, this was a good introduction to compiling at the time, and taking the class has inspired me to come back and finish this project.

## The BrainFuck Interpreter

To help me test my generated transpiled [BrainFuck](https://en.wikipedia.org/wiki/Brainfuck) (BF) code I made my own BF interpreter when I originally picked up this project.
It was not very accurate to the description of the interpreter that the [Codewars](https://www.codewars.com/kata/59f9cad032b8b91e12000035) problem stated.
The one cool feature I had built into it was that I could set a breakpoint in the BF code after which it would visually display the code and the BF tape and let me step through the execution of the code.

### Features

- Infinitely long tape in both directions.
- Tape cell values are [mod](https://en.wikipedia.org/wiki/Modular_arithmetic) 256.
- Interprets any valid BF code character by character.
- Has a visual step-by-step mode with breakpoints.
    - Both the place in execution of the BF code and the position of the tape can be seen at once.

### Usage

- The BF code must be in a file named *code.txt*.
- The input list of values to pass to the interpreter whenever a *,* instruction is encountered must be in a file named *input.txt*
- The interpreter is a single c file that can be compiled.
- Running the interpreter with no command line arguments will just execute the BF code and display the results of interpreting the code.
- Passing in a single number as the command line argument will set it as the breakpoint and execute up until this breakpoint then enter visual mode.

### Visual Mode Commands

```
Results: 48 7 55 
Pos: 93
[->+>>>+<<<<]<<<[-<+>]>>>>[-<+>]<<<<<.>>>>.>>>>.<<<<[-<+<<<->>>>]<[->+<]<<<.>>>>.>>>>.[-]<<<<[->+<]<<<<[->+
                                                ^
                                     v
... 048 000 000 000 007 000 000 000 055 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 000 ...
CMD: 
```

- A valid BF instruction will cause the code to execute up until the first instruction seen of that kind (without executing it).
- A singular lowercase **f** will cause the interpreter to exit visual mode and finish interpreting the rest of the code.
- Inputing a number greater than the currently displayed position (Pos: #) visible in the top left will set that number as the new breakpoint and execute up until that breakpoint.
