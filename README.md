# Genetic Turing Machines - Busy Beaver Problem

This project demonstrates the use of a Genetic Algorithm (GA) to evolve the state table of a Turing Machine for solving the Busy Beaver problem. The genetic algorithm optimizes the state table's configuration with the goal of achieving the maximum number of ones on the tape and a specified maximum number of steps. More details about the busy beaver problem can be found [here](https://en.wikipedia.org/wiki/Busy_beaver)

## Disclaimer

The program uses a genetic algorithm to evolve a solution to the Busy Beaver problem for a given number of states. It represents the Turing machine's state transition table as a 2D array genome and evolves it to maximize the number of ones on the tape. The evolution is done through genetic operations like mutation and crossover.

Key Components:

- It defines the fitness function (objective) based on the number of ones on the tape and checks for a valid solution within the specified maximum steps.
- The genetic algorithm is configured with a population size, mutation and crossover probabilities, and a maximum number of generations.
- The program evolves a population of state transition tables to find the best solution for the Busy Beaver problem.
- The best solution (state transition table) is then converted to human-readable format and printed, along with the fitness score.

Note: The program checks if the found solution matches the known maximum ones and steps for the given number of states in the Busy Beaver problem. If so, it saves the solution details to a file. Also, the program takes three command-line arguments: 
- **the number of states**
- **the population size**
- **the maximum number of generations**

**Also, the Turing Machine does not run forever. Each number of states has a unique stopping number. So if the Turing machine has made the same number of steps as the stopping number it automatically stops, in order to not take forever.** 


## Folder Structure

- `./busy_beaver_solutions`: this folder includes txt files for a different number of states for the Turing machine, which resulted in a busy beaver either for Σ and/or for S

- `./out`: includes the compiled `main.cpp` script

- `./src`:

    - `main.cpp`: 
    
      - **Genetic Algorithm Setup:** Uses a Genetic Algorithm with a 2D array genome to evolve the state table of a Turing Machine. Configures parameters such as population size, mutation probability, and crossover probability.
      
      - **Objective Function:** Evaluates the fitness of each genome by simulating a Turing Machine with the corresponding state table. Measures the number of ones on the tape and checks for Busy Beaver conditions.
      
      - **Results and Saving:** Prints the best-evolved individual and its state table. Saves results, including state table and tape configuration, to a file if it achieves the maximum number of ones and steps for the Busy Beaver problem. 

- `busy_beaver_output.txt`: shows the current run for a busy beaver and saves the found busy beaver (if it does not already exist) to the corresponding file of the `./busy_beaver_solutions` folder.

- `run_busy_beaver.sh`: the bash script compiles and runs a C++ program for the Busy Beaver Genetic Algorithm, taking three command-line arguments: number of states, population size, and number of generations, with error checking and result output to a file.

## Compilation

Compile the program using the following command (if you want to compile it by yourself and not automatically with the `run_busy_beaver.sh` script):

```sh
g++ -o ./out/busy_beaver ./src/main.cpp -lga -fpermissive -Ofast
```

## Usage

Either you use your own compiled version with the following command
```sh
./out/busy_beaver <num_states> <population_size> <num_generations>
```

or you run the included bash script which compiles and runs the program automatically with the provided arguments

```sh
./run_busy_beaver.sh <num_states> <population_size> <num_generations>
```



## Requirements
- C++ compiler with support for C++11 features
- GaLib 


## Usage
The easiest way to run the busy beaver problem is to run the `run_busy_beaver.sh` script with the corresponding arguments. The results of the run will be saved in the `busy_beaver_output.txt` file and if a new busy beaver is found (a busy beaver which is not already in the corresponding file) the run will also be saved in the corresponding file in the `./busy_beaver_solutions` folder.

## Authors
- Elias Marcon MSc. | @eliasmarcon
- Ing. Fabian Steiner BSc. | @fasteiner
- Jan Langela Regincos BSc. | @janpilu