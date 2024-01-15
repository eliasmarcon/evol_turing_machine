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

const int POPULATION_SIZE = 1; //5
const int MAX_GENERATIONS = 0; //200000

int NUM_STATES = 2;
int const max_1s[] = {1, 4, 6, 13, 4098};
int const max_steps[] = {500, 1500, 5000, 15000, 1000000};
std::string const loops[] = {"00000", "00100", "00010", "00110"};


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
bool checkForHaltState(std::vector<std::string> stateTableTuringMachine){

    // check for at least one halt state
    for (int i = 0; i < stateTableTuringMachine.size(); ++i) {
        if (stateTableTuringMachine[i][4] == static_cast<char>(NUM_STATES + '0')) {
            return true;
        }
    }

    return false;
}

// replace random pair
std::vector<std::string> replaceRandomPair(std::vector<std::string> stateTableTuringMachine){

    std::random_device rd;
    std::mt19937 gen(rd());

    // Generate a random index
    std::uniform_int_distribution<> dist(0, stateTableTuringMachine.size() - 1);

    // Replace the last value of the random pair with 'h'
    stateTableTuringMachine[dist(gen)][4] = NUM_STATES + '0';

    return stateTableTuringMachine;
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

        std::string tape = "0000000000000000000000000000000";

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

        int run(){

            // std::cout << "original head: " << head << std::endl;
            // std::cout << "original tape: " << tape << std::endl;

            while (current_state != 'h' && steps < max_steps[NUM_STATES - 1] && !checkFirstState()){

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

                        if (steps % 1000 == 0){
                            std::cout << "steps: " << steps << std::endl;
                            // std::cout << "head: " << head << std::endl;
                            // std::cout << "tape: " << tape << std::endl;
                            // std::cout << std::endl;
                        }

                        break;
                    }
                }
            }

            // count number of ones
            for (int i = 0; i < tape.size(); ++i) {
                if (tape[i] == '1') {
                    counter_ones++;
                }
            }

            // // print head and tape
            // std::cout << "final head: " << head << std::endl;
            // std::cout << "final tape: " << tape << std::endl;

            return counter_ones;
        }
};

void saveBusyBeaver(int POPULATION_SIZE, int MAX_GENERATIONS, std::vector<std::string> bestVector, int bestFitness, int NUM_STATES, 
                    const std::chrono::time_point<std::chrono::system_clock>& start_time,
                    const std::chrono::time_point<std::chrono::system_clock>& end_time){

    std::ofstream myfile;
    std::string filename = "./busy_beaver_solutions/busy_beaver_" + std::to_string(NUM_STATES) + ".txt";
    myfile.open (filename, std::ios_base::app);

    myfile << "Population size: " << POPULATION_SIZE << std::endl;
    myfile << "Max generations: " << MAX_GENERATIONS << std::endl;
    myfile << "Number of states: " << NUM_STATES << std::endl << std::endl;
    
    myfile << "Best vector: " << std::endl;
    for (const auto& str : bestVector) {
        myfile << str << std::endl;
    }
    
    myfile << "\nBest fitness: " << bestFitness << std::endl;
    
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


/*===========================================================================================*/
/*======================================= GA Functions ======================================*/
/*===========================================================================================*/

// Objective function for the Busy Beaver problem
float objective(GAGenome& g) {

    GA2DArrayGenome<int> &genome = (GA2DArrayGenome<int> &)g;
    
	int fitness = 0;

    // convert genome to vector
    std::vector<std::string> stateTableTuringMachine = convertGenomeToVector(genome);

    // // Print the initialized vector of vectors of strings
    // std::cout << "\nInitialized vector of vectors of strings: " << std::endl;
    // for (const auto& str : stateTableTuringMachine) {
    //     std::cout << str << std::endl;
    // }
    // std::cout << std::endl;

    // create a Turing Machine
    TuringMachine turingMachine(stateTableTuringMachine);

    // run the Turing Machine
    fitness = turingMachine.run();

    if (fitness > max_1s[NUM_STATES - 1]) {
        fitness = 0;
    }

    // Print the fitness
    std::cout << "Fitness: " << fitness << std::endl;

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

    // check halt state
    bool hasH = checkForHaltState(stateTableTuringMachine);

    if (!hasH){
        stateTableTuringMachine = replaceRandomPair(stateTableTuringMachine);
    }

	// Set the genome with the generated genes
    for (int i = 0; i < genome.width(); ++i) {
        for (int j = 0; j < genome.height(); ++j) {
            // cast char to int
            genome.gene(i, j, stateTableTuringMachine[i][j] - '0');
        }
    }

	// Print the genome
	std::cout << "Initialized genome: " << std::endl;
	for (int i = 0; i < genome.width(); ++i) {
        for (int j = 0; j < genome.height(); ++j) {
            std::cout << genome.gene(i, j);
        }
        std::cout << std::endl;
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
    std::vector<std::string> stateTableTuringMachine = convertGenomeToVector(genome);

    bool hasH = checkForHaltState(stateTableTuringMachine);

    if (!hasH){
        stateTableTuringMachine = replaceRandomPair(stateTableTuringMachine);
    }

    // Set the genome with the generated genes
    for (int i = 0; i < genome.width(); ++i) {
        for (int j = 0; j < genome.height(); ++j) {
            // cast char to int
            genome.gene(i, j, stateTableTuringMachine[i][j] - '0');
        }
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
/*===================================== Selector Class ======================================*/
/*===========================================================================================*/

class BestSelector : public GASelectionScheme {
    public:
        GADefineIdentity("BestSelector", 0);

        BestSelector() : GASelectionScheme() {}
        virtual ~BestSelector() {}

        virtual GASelectionScheme* clone() const {
            return new BestSelector;
        }

        virtual void assign(GAPopulation& p) {
            GASelectionScheme::assign(p);
        }

        virtual void update() {
            GASelectionScheme::update();
        }

        virtual GAGenome& select() const {
            const int numCandidates = POPULATION_SIZE * 0.5;  
            const int numSelected = POPULATION_SIZE * 0.3;    

            int idx[numCandidates];
            GAGenome* candidates[numCandidates];

            for (int i = 0; i < numCandidates; ++i) {
                idx[i] = GARandomInt(0, pop->size() - 1);
                candidates[i] = &(pop->individual(idx[i]));
            }

            // Sort candidates based on their scores in descending order
            std::sort(candidates, candidates + numCandidates,
                    [](const GAGenome* a, const GAGenome* b) {
                        return a->score() > b->score();
                    });

            // Return a reference to the 10th best individual
            return *candidates[numSelected - 1];
        }
};


/*===========================================================================================*/
/*====================================== Main Function ======================================*/
/*===========================================================================================*/

int main(int argc, char* argv[]) {
    
    srand(static_cast<unsigned int>(time(nullptr)));

    // // Read number of states from command line
    // if (argc > 1) {

    //     // Check if the number of states is valid
    //     if (atoi(argv[1]) < 1 || atoi(argv[1]) > 4) {
    //         std::cout << "Please enter a number of states between 1 and 4" << std::endl;
    //         return 0;
    //     }
        
    //     NUM_STATES = atoi(argv[1]);
    // } else {
    //     std::cout << "Please enter the number of states between 1 and 4: ";
    //     std::cin >> NUM_STATES;
    // }	

    // Start measuring time
    auto start_time = std::chrono::high_resolution_clock::now();

    GA2DArrayGenome<int> genome(std::pow(NUM_STATES, 2), 5, objective);

    genome.initializer(initializer);
    genome.mutator(mutator);
    genome.crossover(crossover);

    GASimpleGA ga(genome);
    ga.populationSize(POPULATION_SIZE);
    ga.pMutation(0.1);
    ga.pCrossover(0.9);
    ga.minimaxi(GAGeneticAlgorithm::MAXIMIZE);
    ga.set(gaNnGenerations, MAX_GENERATIONS);

    BestSelector selector;
    ga.selector(selector);

    // Evolve and output information for each generation
    std::cout << "Starting evolution...\n\n";
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

    if (bestFitness == max_1s[NUM_STATES - 1]) {
        std::cout << "Busy Beaver found! Values are saved into ./busy_beaver_solutions/.." << std::endl;
        saveBusyBeaver(POPULATION_SIZE, MAX_GENERATIONS, bestVector, bestFitness, NUM_STATES, start_time, end_time);
    } else {
        std::cout << "No Busy Beaver found!" << std::endl;
    }

    return 0;
}
