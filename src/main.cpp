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
const int MAX_GENERATIONS = 10; //200000

int NUM_STATES = 2;
int const max_1s[] = {1, 4, 6, 13, 4098};


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

/*
// create a Class Turing Machine
class TuringMachine {

    private:

        char tape [30] = "00000000000000000000000000000";
    bool hasH = false;


    public: 

        // Constructor
        TuringMachine() {
            // Initialize your Turing Machine here
        }

        // If 'h' is not present, replace a random pair's last value with 'h'
        if (!hasH) {
            std::random_device rd;
            std::mt19937 gen(rd());

            // Generate a random index
            std::uniform_int_distribution<> dist(0, stateTableTuringMachine.size() - 1);
            int randomIndex = dist(gen);

            // Access the random vector
            std::string& randomString = stateTableTuringMachine[randomIndex];

            // Replace the last value with 'h'
            if (!randomString.empty()) {
                randomString.back() = 'h';
            }

            // Print the updated stateTableTuringMachine
            std::cout << "Updated stateTableTuringMachine: " << std::endl;
            for (const auto& vector : stateTableTuringMachine) {
                for (const auto& str : vector) {
                    std::cout << str << ' ';
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }

};
*/
	



/*===========================================================================================*/
/*======================================= GA Functions ======================================*/
/*===========================================================================================*/

// Objective function for the Busy Beaver problem
float objective(GAGenome& g) {

    GA2DArrayGenome<int> &genome = (GA2DArrayGenome<int> &)g;
    
	int fitness = 0;

    
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

    // // Print the initialized vector of vectors of strings
	// std::cout << "Initialized string in vector: " << std::endl;
    // for (const auto& str : stateTableTuringMachine) {
    //     std::cout << str << std::endl;
    // }
    // std::cout << std::endl;

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
    GA2DArrayGenome<std::string>& genome = (GA2DArrayGenome<std::string>&)g;

    int nMutations = 0;

    std::default_random_engine rng(std::random_device{}());

    return nMutations;
}


// Crossover
int crossover(const GAGenome& p1, const GAGenome& p2, GAGenome* c1, GAGenome* c2) {
    GA2DArrayGenome<std::string>& parent1 = (GA2DArrayGenome<std::string>&)p1;
    GA2DArrayGenome<std::string>& parent2 = (GA2DArrayGenome<std::string>&)p2;
    
    if (c1 && c2) {
        GA2DArrayGenome<std::string>& child1 = (GA2DArrayGenome<std::string>&)*c1;
        GA2DArrayGenome<std::string>& child2 = (GA2DArrayGenome<std::string>&)*c2;


        return 2;
    } else if (c1) {
        GA2DArrayGenome<std::string>& child = (GA2DArrayGenome<std::string>&)*c1;
        
        
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
    std::cout << "\nStarting evolution...\n\n";
    ga.evolve();


    // Stop measuring time
    auto end_time = chrono::high_resolution_clock::now();

    // Print elapsed time
    printElapsedTime(start_time, end_time);


    return 0;
}
