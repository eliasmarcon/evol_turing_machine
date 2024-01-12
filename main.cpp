#include <ga/GA2DArrayGenome.h>
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

using namespace std;

const int POPULATION_SIZE = 5; //5
const int MAX_GENERATIONS = 175000; //200000

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



/*===========================================================================================*/
/*======================================= GA Functions ======================================*/
/*===========================================================================================*/

// Objective function for Sudoku
float objective(GAGenome& g) {

    GA2DArrayGenome<int>& genome = (GA2DArrayGenome<int>&)g;
    int fitness = 0;

    
    return (float)fitness;
}


// Initializer
void initializer(GAGenome& g) {
    GA2DArrayGenome<int>& genome = (GA2DArrayGenome<int>&)g;

    
}

// Mutator
int mutator(GAGenome& g, float p) {
    GA2DArrayGenome<int>& genome = (GA2DArrayGenome<int>&)g;

    int nMutations = 0;

    std::default_random_engine rng(std::random_device{}());

    return nMutations;
}

// Crossover
int crossover(const GAGenome& p1, const GAGenome& p2, GAGenome* c1, GAGenome* c2) {
    GA2DArrayGenome<int>& parent1 = (GA2DArrayGenome<int>&)p1;
    GA2DArrayGenome<int>& parent2 = (GA2DArrayGenome<int>&)p2;
    
    if (c1 && c2) {
        GA2DArrayGenome<int>& child1 = (GA2DArrayGenome<int>&)*c1;
        GA2DArrayGenome<int>& child2 = (GA2DArrayGenome<int>&)*c2;


        return 2;
    } else if (c1) {
        GA2DArrayGenome<int>& child = (GA2DArrayGenome<int>&)*c1;
        
        
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


    // Start measuring time
    auto start_time = std::chrono::high_resolution_clock::now();

    GA2DArrayGenome<float> genome(9, max, objective);
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
    GA2DArrayGenome<float> bestGenome = (GA2DArrayGenome<float> &)ga.statistics().bestIndividual();


    // Stop measuring time
    auto end_time = chrono::high_resolution_clock::now();

    // Print elapsed time
    printElapsedTime(start_time, end_time);


    return 0;
}
