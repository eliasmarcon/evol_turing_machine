#include <ga/GA2DArrayGenome.h>
#include <ga/GA1DBinStrGenome.h>
#include <ga/GASimpleGA.h>
#include <ga/GASelector.h>
#include <ga/std_stream.h>
#include <ga/GAStatistics.h>
#include <ctime>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <random>
#include <chrono>
#include <unordered_set>
#include <fstream>
#include <sstream>
#include <list>

#include <string>
#include <cmath>
#include <bitset>
#include <time.h>

using namespace std;

int NUM_STATES = 2;
int POPULATION_SIZE = 10; //5
int MAX_GENERATIONS = 10; //200000

const int max_1s[] = {1, 4, 6, 13, 4098};
const int max_steps_possible[] = {1, 6, 21, 107, 47176870};
const std::string busy_beaver_filename = "./busy_beaver_solutions/busy_beaver_";
const std::string loops[] = {"00000", "00100", "00010", "00110"};

std::string busy_beaver_tape = "";
int max_steps_beaver = 0;

// vector to save found busy beaver
std::vector<std::vector<std::string>> busy_beaver_array_found;
std::vector<int> busy_beaver_array_step_size;
std::vector<std::string> busy_beaver_array_tape;

/*===========================================================================================*/
/*===================================== Helper Functions ====================================*/
/*===========================================================================================*/

void printElapsedTime(const std::chrono::time_point<std::chrono::system_clock>& start_time,
                      const std::chrono::time_point<std::chrono::system_clock>& end_time) {
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    // Calculate hours, minutes, seconds, and milliseconds
    auto hours = std::chrono::duration_cast<std::chrono::hours>(duration);
    duration -= hours;
    auto minutes = std::chrono::duration_cast<std::chrono::minutes>(duration);
    duration -= minutes;
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
    duration -= seconds;
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration);

    // Print elapsed time in hours, minutes, seconds, and milliseconds
    std::cout << "Time taken for evolution: " << hours.count() << " hours, "
              << minutes.count() << " minutes, "
              << seconds.count() << " seconds, and "
              << milliseconds.count() << " milliseconds\n"
              << std::endl;
}

// Function to convert integer to character (a, b, c, ...)
char intToChar(int value) {
    return static_cast<char>('a' + value);
}

// check for at least one halt state
bool checkForHaltState(GA2DArrayGenome<int> &genome){

    // check for at least one halt state
    for (int i = 0; i < genome.width(); ++i) {
        if (genome.gene(i, 4) == NUM_STATES) {
            return true;
        }
    }

    return false;
}

// replace random pair
GA2DArrayGenome<int> replaceRandomPair(GA2DArrayGenome<int> &genome){

    std::random_device rd;
    std::mt19937 gen(rd());

    // Generate a random index
    std::uniform_int_distribution<> dist(0, genome.width() - 1);

    // Replace the last value with a halt state
    genome.gene(dist(gen), 4, NUM_STATES);

    return genome;
}

std::string separateWithPipe(const std::string& input) {
    std::string result;

    for (size_t i = 0; i < input.length(); ++i) {
        result += input[i];
        
        // Add " | " after each character, except for the last one
        if (i < input.length() - 1) {
            result += " | ";
        }
    }

    return result;
}

std::vector<std::string> convertGenomeToVector(GA2DArrayGenome<int> &genome){

    std::vector<std::string> stateTableTuringMachine;

    for (int i = 0; i < genome.width(); ++i) {
        std::string tempVector;
        for (int j = 0; j < genome.height(); ++j) {
            tempVector += std::to_string(genome.gene(i, j));
        }
        stateTableTuringMachine.push_back(tempVector);
    }

    // convert first character to state
    for (int i = 0; i < stateTableTuringMachine.size(); ++i) {
        stateTableTuringMachine[i][0] = intToChar(stateTableTuringMachine[i][0] - '0');
    }

    // convert fourth character to direction
    for (int i = 0; i < stateTableTuringMachine.size(); ++i) {
        if (stateTableTuringMachine[i][3] == '0') {
            stateTableTuringMachine[i][3] = 'l';
        } else {
            stateTableTuringMachine[i][3] = 'r';
        }
    }

    // convert fifth character to state
    for (int i = 0; i < stateTableTuringMachine.size(); ++i) {

        // if state is halt state
        if ( stateTableTuringMachine[i][4] == static_cast<char>(NUM_STATES + '0')){
            stateTableTuringMachine[i][4] = 'h';
        } else {
            stateTableTuringMachine[i][4] = intToChar(stateTableTuringMachine[i][4] - '0');
        }
    }

    return stateTableTuringMachine;
    
}

void printVector(std::vector<std::string> stateTableTuringMachine){

    for (const auto& str : stateTableTuringMachine) {
        std::cout << str << std::endl;
    }
    std::cout << std::endl;
}


// create a Class Turing Machine
class TuringMachine {

    private:

        std::string tape = "00000";

        int head = tape.size() / 2;
        int counter_ones = 0;
        int steps = 0;
        char initial_state = 'a';
        char current_state = initial_state;

        std::vector<std::string> stateTableTuringMachine;

    public: 

        // Constructor
        TuringMachine(std::vector<std::string> stateTableTuringMachine) {
            // Initialize your Turing Machine here
            this->stateTableTuringMachine = stateTableTuringMachine;
        }

        // check for looping for the first state
        bool checkFirstState(){

            // check for looping
            std::string firstState = stateTableTuringMachine[0];

            for (const std::string& str : loops) {
                if (str == firstState) {
                    return true;
                }
            }

            return false;
        }

        std::tuple<int, std::string, int> run(){

            // std::cout << "original head: " << head << std::endl;
            // std::cout << "original tape: " << tape << std::endl;

            while (current_state != 'h' && steps < max_steps_possible[NUM_STATES - 1] && !checkFirstState()){

                // read value from tape
                char tape_value = tape[head];

                // find the current state in the state table
                for (int i = 0; i < stateTableTuringMachine.size(); ++i) {
                    if (stateTableTuringMachine[i][0] == current_state && stateTableTuringMachine[i][1] == tape_value) {

                        // write new value to tape
                        tape[head] = stateTableTuringMachine[i][2];

                        // move head and check for out of bounds
                        if (stateTableTuringMachine[i][3] == 'l') {
                            // check for out of bounds
                            if (head == 0) {
                                tape.insert(0, "0");
                                head = 0;
                            } else {
                                head--;
                            }
                        } else {
                            // check for out of bounds
                            if (head == tape.size() - 1) {
                                tape.push_back('0');
                            }
                            head++;
                        }

                        // update current state
                        current_state = stateTableTuringMachine[i][4];

                        // update steps
                        steps++;

                        break;
                    }
                }
            }

            // count number of ones
            for (int i = 0; i < tape.size(); ++i) {
                if (tape[i] == '1') {
                    ++counter_ones;
                }
            }

            return std::make_tuple(counter_ones, tape, steps);
        }
};

bool checkIfBeaverExists(std::vector<std::string> bestVector, int& existingPopulationSize, int& existingMaxGenerations) {
    std::ifstream file(busy_beaver_filename + std::to_string(NUM_STATES) + "_states.txt");
    std::string line;

    while (std::getline(file, line)) {
        
        if (line.find("Busy Beaver:") != std::string::npos) {
            // Read the best vector from the file
            std::vector<std::string> existingVector;
            for (int i = 0; i < bestVector.size(); ++i) {
                std::getline(file, line);
                existingVector.push_back(line);
            }
            if (existingVector == bestVector) {
                return true; // The vector already exists
            }
        }
    }

    return false; // The vector does not exist
}

void saveBusyBeaver(int POPULATION_SIZE, int MAX_GENERATIONS, std::vector<std::string> bestVector, int bestFitness, int NUM_STATES, 
                    const std::chrono::time_point<std::chrono::system_clock>& start_time,
                    const std::chrono::time_point<std::chrono::system_clock>& end_time,
                    bool busy_beaver_both) {

    int existingPopulationSize = 0;
    int existingMaxGenerations = 0;

    if (checkIfBeaverExists(bestVector, existingPopulationSize, existingMaxGenerations)) {
        
        std::cout << "But Busy Beaver already exists!" << std::endl;

    } else {

        std::cout << "Values are saved into" << busy_beaver_filename + std::to_string(NUM_STATES) + "_states.txt" << std::endl;

        // Vector does not exist, save the new values
        std::ofstream myfile;
        std::string filename = busy_beaver_filename + std::to_string(NUM_STATES) + "_states.txt";
        myfile.open(filename, std::ios_base::app);

        myfile << "Population size: " << POPULATION_SIZE << std::endl;
        myfile << "Max generations: " << MAX_GENERATIONS << std::endl;
        myfile << "Number of states: " << NUM_STATES << std::endl << std::endl;

        myfile << "Busy Beaver: " << std::endl;
        for (const auto& str : bestVector) {
            myfile << str << std::endl;
        }

        myfile << "\nBest fitness: " << bestFitness << std::endl;

        myfile << "\nBusy Beaver tape: " << std::endl;
        myfile << separateWithPipe(busy_beaver_tape) << std::endl;

        myfile << "\nMax steps: " << max_steps_beaver << std::endl;

        myfile << "\nBusy Beaver for Σ and S: " << (busy_beaver_both ? "Yes!" : "No just for Σ!") << std::endl;

        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

        // Calculate hours, minutes, seconds, and milliseconds
        auto hours = std::chrono::duration_cast<std::chrono::hours>(duration);
        duration -= hours;
        auto minutes = std::chrono::duration_cast<std::chrono::minutes>(duration);
        duration -= minutes;
        auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
        duration -= seconds;
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration);

        // Print elapsed time in hours, minutes, seconds, and milliseconds
        myfile << "\nTime taken for evolution: " << hours.count() << " hours, "
            << minutes.count() << " minutes, "
            << seconds.count() << " seconds, and "
            << milliseconds.count() << " milliseconds\n"
            << std::endl;

        myfile << "--------------------------------------------------------------------------------" << std::endl << std::endl;
        myfile.close();
    }
}

/*===========================================================================================*/
/*======================================= GA Functions ======================================*/
/*===========================================================================================*/

// Objective function for the Busy Beaver problem
float objective(GAGenome& g) {

    GA2DArrayGenome<int> &genome = (GA2DArrayGenome<int> &)g;
    
	int fitness = 0;

    // convert genome to vector
    std::vector<std::string> stateTableTuringMachine = convertGenomeToVector(genome);

    // create a Turing Machine
    TuringMachine turingMachine(stateTableTuringMachine);

    // run the Turing Machine
    std::tuple<int, std::string, int> result = turingMachine.run();

    // get the number of ones
    fitness = std::get<0>(result);

    if (fitness == max_1s[NUM_STATES - 1] && std::get<2>(result) <= max_steps_possible[NUM_STATES - 1]) { 
        // get the tape
        busy_beaver_tape = std::get<1>(result);

        // get the steps
        max_steps_beaver = std::get<2>(result);

        // save busy beaver into vector
        busy_beaver_array_found.push_back(stateTableTuringMachine);
        // save step size
        busy_beaver_array_step_size.push_back(max_steps_beaver);
        // save tape
        busy_beaver_array_tape.push_back(busy_beaver_tape);

    } else {  
        fitness = 0;
    }

    return (float)fitness;
}


// Initializer
void initializer(GAGenome& g) {
    
    GA2DArrayGenome<int> &genome = (GA2DArrayGenome<int> &)g;

    // Create a vector of vectors of strings
    std::vector<std::string> stateTableTuringMachine;

    // Initialize the vector with keys 'a0', 'a1', 'b0', 'b1', ..., and empty strings as values
    for (int state = 0; state < NUM_STATES; ++state) {
        
        for (int zero_one = 0; zero_one < 2; ++zero_one) {
            
            std::random_device rd;
            std::mt19937 gen(rd());

            // random 0 or 1 for overwriting the current value
            int randomNumber = std::uniform_int_distribution<>(0, 1)(gen);

            // random l or r for moving left or right
            int randomDirection = std::uniform_int_distribution<>(0, 1)(gen);

            // random state to go to with a halt state
            int randomState = std::uniform_int_distribution<>(0, NUM_STATES)(gen);

            stateTableTuringMachine.push_back(std::to_string(state) + std::to_string(zero_one) + std::to_string(randomNumber) + std::to_string(randomDirection) + std::to_string(randomState));
        }
    }

	// Set the genome with the generated genes
    for (int i = 0; i < genome.width(); ++i) {
        for (int j = 0; j < genome.height(); ++j) {
            genome.gene(i, j, stateTableTuringMachine[i][j] - '0');
        }
    }

    // check halt state
    bool hasH = checkForHaltState(genome);

    if (!hasH){
        genome = replaceRandomPair(genome);
    }
}


// Mutator
int mutator(GAGenome& g, float p) {
    GA2DArrayGenome<int>& genome = (GA2DArrayGenome<int>&)g;

    int nMutations = 0;

    // mutate
    for (int i = 0; i < genome.width(); ++i) {
        for (int j = 2; j < genome.height(); ++j) {

            std::default_random_engine rng(std::random_device{}());

            if (GAFlipCoin(p)) {

                if (j == genome.height() - 1){
                    int newRandomState = std::uniform_int_distribution<>(0, NUM_STATES)(rng);
                    genome.gene(i, j, newRandomState);
                }else{
                    int newValue = std::uniform_int_distribution<>(0, 1)(rng);
                    genome.gene(i, j, newValue);
                }

                nMutations++;
            }
        }
    }

    // check halt state
    bool hasH = checkForHaltState(genome);

    if (!hasH){
        genome = replaceRandomPair(genome);
    }

    return nMutations;
}


// Crossover
int crossover(const GAGenome& p1, const GAGenome& p2, GAGenome* c1, GAGenome* c2) {

    GA2DArrayGenome<int>& parent1 = (GA2DArrayGenome<int>&)p1;
    GA2DArrayGenome<int>& parent2 = (GA2DArrayGenome<int>&)p2;

    if (c1 && c2) {
        GA2DArrayGenome<int>& child1 = (GA2DArrayGenome<int>&)*c1;
        GA2DArrayGenome<int>& child2 = (GA2DArrayGenome<int>&)*c2;

        std::default_random_engine rng(std::random_device{}());

        // create cutoff point
        int cutoff = std::uniform_int_distribution<>(0, parent1.width() - 1)(rng);

        // create children
        for (int i = 0; i < parent1.width(); ++i) {
            for (int j = 0; j < parent1.height(); ++j) {
                if (i < cutoff) {
                    child1.gene(i, j, parent1.gene(i, j));
                    child2.gene(i, j, parent2.gene(i, j));
                } else {
                    child1.gene(i, j, parent2.gene(i, j));
                    child2.gene(i, j, parent1.gene(i, j));
                }
            }
        }

        return 2;
    } else if (c1) {
        GA2DArrayGenome<int>& child = (GA2DArrayGenome<int>&)*c1;

        std::default_random_engine rng(std::random_device{}());

        // create cutoff point
        int cutoff = std::uniform_int_distribution<>(0, parent1.width() - 1)(rng);

        // create child
        for (int i = 0; i < parent1.width(); ++i) {
            for (int j = 0; j < parent1.height(); ++j) {
                if (i < cutoff) {
                    child.gene(i, j, parent1.gene(i, j));
                } else {
                    child.gene(i, j, parent2.gene(i, j));
                }
            }
        }

        return 1;
    } else {
        return 0;
    }
}


/*===========================================================================================*/
/*====================================== Main Function ======================================*/
/*===========================================================================================*/

int main(int argc, char* argv[]) {
    
    srand(static_cast<unsigned int>(time(nullptr)));

    // Read number of states from command line
    if (argc == 4) {
        NUM_STATES = atoi(argv[1]);
        POPULATION_SIZE = atoi(argv[2]);
        MAX_GENERATIONS = atoi(argv[3]);
    } else {

        std::cout << "Usage: " << argv[0] << " <number of states> <population size> <max generations>" << std::endl;
        exit(0);
    }	

    // Start measuring time
    auto start_time = std::chrono::high_resolution_clock::now();

    GA2DArrayGenome<int> genome(NUM_STATES * 2, 5, objective);

    genome.initializer(initializer);
    genome.mutator(mutator);
    genome.crossover(crossover);

    GASimpleGA ga(genome);
    ga.populationSize(POPULATION_SIZE);
    ga.pMutation(0.1);
    ga.pCrossover(0.9);

    // Evolve and output information for each generation
    std::cout << "Starting evolution...\n";
    ga.evolve();
    GA2DArrayGenome<int> bestGenome = (GA2DArrayGenome<int> &)ga.statistics().bestIndividual();

    // Stop measuring time
    auto end_time = chrono::high_resolution_clock::now();
    

    // Print the best individual
    std::cout << "\nBest individual:" << std::endl;
    std::vector<std::string> bestVector = convertGenomeToVector(bestGenome);
    printVector(bestVector);

    // best fitness
    int bestFitness = ga.statistics().bestIndividual().score();
    std::cout << "Best fitness: " << bestFitness << std::endl;

    // Print elapsed time
    printElapsedTime(start_time, end_time);

    if (bestFitness == max_1s[NUM_STATES - 1] && max_steps_beaver == max_steps_possible[NUM_STATES - 1]) {
        std::cout << "Busy Beaver found for Σ and S! ";
        saveBusyBeaver(POPULATION_SIZE, MAX_GENERATIONS, bestVector, bestFitness, NUM_STATES, start_time, end_time, true);
    } else if (bestFitness == max_1s[NUM_STATES - 1]) {
        std::cout << "Busy Beaver found for Σ! ";
        saveBusyBeaver(POPULATION_SIZE, MAX_GENERATIONS, bestVector, bestFitness, NUM_STATES, start_time, end_time, false); 
    } else {
        std::cout << "No Busy Beaver found!" << std::endl;
    }



    // drop duplicates
    std::sort(busy_beaver_array_found.begin(), busy_beaver_array_found.end());
    busy_beaver_array_found.erase(std::unique(busy_beaver_array_found.begin(), busy_beaver_array_found.end()), busy_beaver_array_found.end());
    
    // save all busy beavers into separate file
    std::ofstream myfile;
    std::string filename = "./test_busy_beaver_" + std::to_string(NUM_STATES) + "_states_all.txt";

    myfile.open(filename, std::ios_base::app);

    int counter = 0;
    for (const auto& str : busy_beaver_array_found) {

        if (busy_beaver_array_step_size[counter] < max_steps_possible[NUM_STATES - 1]) {

            myfile << "Population size: " << POPULATION_SIZE << std::endl;
            myfile << "Max generations: " << MAX_GENERATIONS << std::endl;
            myfile << "Number of states: " << NUM_STATES << std::endl << std::endl;
            myfile << "Busy Beaver: " << std::endl;

            for (const auto& str2 : str) {
                myfile << str2 << std::endl;
            }

            myfile << "\nBest fitness: " << 13 << std::endl;

            // get the tape
            myfile << "\nBusy Beaver tape: " << std::endl;
            myfile << separateWithPipe(busy_beaver_array_tape[counter]) << std::endl << std::endl;

            //save step size
            myfile << "Max steps: " << busy_beaver_array_step_size[counter++] << std::endl << std::endl;       

            myfile << "Busy Beaver for Σ and S: No just for Σ!" << std::endl;

        }
    }

    // // print the found busy beaver
    // std::vector<std::string> otherVector2 = {"a01lb", "a11lc", "b01rc", "b11lh", "c01la", "c10rb"};
    // std::vector<std::string> otherVector3 = {"a01rb", "a11lc", "b01la", "b11rb", "c01lb", "c11rh"};
    // std::vector<std::string> otherVector4 = {"a01rb", "a11lh", "b00rc", "b11rb", "c01lc", "c11la"};
    // std::vector<std::string> otherVector5 = {"a00rb", "a10la", "b01rc", "b11rh", "c01la", "c11rb"};
    
    // std::cout << "\nBusy Beaver Arrray:" << std::endl;
    // for (const auto& initialVector : busy_beaver_array_found) {

    //     if (initialVector == otherVector2 || initialVector == otherVector3 || initialVector == otherVector4 || initialVector == otherVector5) {
    //         std::cout << "Initial genome is equal to other vector!" << std::endl;
    //         printVector(initialVector);
    //     }
    // }

    return 0;
}
