#include <stdio.h>
#include <getopt.h>
#include <vector>
#include <string.h>
#include <iostream>
#include <cstdlib>
#include <algorithm>

struct Ball{
    bool isLeft;
    bool isBlack;

    Ball(bool left, bool black)
        : isLeft(left), isBlack(black) {}
    
    Ball()
        : isLeft(true), isBlack(true) {}
};

class Simulator{
    public:
    void getOptions(int arc, char** argv);
    void run();
    void printResults();

    private:
    std::vector<Ball> balls;
    std::vector<size_t> results;
    char mode = 'r'; //r for random, e for even
    size_t trials = 1;
    size_t ballsPerColor = 5;

    void setup();
    void swap();
    bool done();
    bool quiet = false;
    double avg = 0;
};

void Simulator::getOptions(int argc, char** argv){
    int option_index = 0;
    int option = 0;

    opterr = false;

    struct option longOpts[] = {{"help", no_argument, nullptr, 'h'},
                                {"trials", required_argument, nullptr, 't'},
                                {"mode", required_argument, nullptr, 'm'},
                                {"balls", required_argument, nullptr, 'b'},
                                {"quiet", no_argument, nullptr, 'q'},
                                {nullptr, 0, nullptr, '\0'}};

    while((option = getopt_long(argc, argv, "ht:m:b:q", longOpts, &option_index)) != -1){
        switch(option){
            case 'h':
                std::cout << "Usage: ./sort\n\t[--help | -h]\n"
                          <<                  "\t[--trials | -t] <positive integer>\n"
                          <<                  "\t[--mode | -m] <random or even>\n"
                          <<                  "\t[--balls | -b] <positive integer>\n"
                          <<                  "\t[--quiet | -q]\n";
                exit(0);

            case 'm':
                if(strcmp(optarg, "even") == 0 || strcmp(optarg, "e") == 0){
                    mode = 'e';
                }
                else if(strcmp(optarg, "random") == 0 || strcmp(optarg, "r") == 0){
                    mode = 'r';
                }
                else{
                    std::cerr << "Unrecognized mode\n";
                    exit(1);
                }
                break;

            case 't':
                if(atoi(optarg) < 1){
                    std::cerr << "Invalid number of trials\n";
                    exit(1);
                }
                trials = atoi(optarg);
                break;
            
            case 'b':
                if(atoi(optarg) < 1){
                    std::cerr << "Invalid number of balls\n";
                    exit(1);
                }
                ballsPerColor = atoi(optarg);
                break;

            case 'q':
                quiet = true;
                break;

            default:
                std::cerr << "Unrecognized input\n";
                exit(1);
                break;
        }
    }
}

void Simulator::run(){
    for(size_t t = 0; t < trials; ++t){
        size_t counter = 0;
        setup();
        while(!done()){
            swap();
            ++counter;
        }
        if(!quiet)
            results.push_back(counter);
        avg += (double)(counter) / (double)(trials);
    }
}

void Simulator::setup(){
    balls.resize(0);
    for(size_t i = 0; i < ballsPerColor; ++i){
        if(i % 2 == 0)
            balls.emplace_back(true, true);
        else
            balls.emplace_back(false, true);
    }
    for(size_t i = 0; i < ballsPerColor; ++i){
        if(i % 2 == 0)
            balls.emplace_back(false, false);
        else
            balls.emplace_back(true, false);
    }
    if(mode == 'r'){
        std::random_shuffle(balls.begin(), balls.end());
    }
}

bool Simulator::done(){
    bool blackLeft = true;
    if((balls[0].isBlack && !balls[0].isLeft) || (!balls[0].isBlack && balls[0].isLeft)){
        blackLeft = false;
    }
    for(Ball b : balls){
        if(blackLeft){
            if((b.isBlack && !b.isLeft) || (!b.isBlack && b.isLeft))
                return false;
        }
        else{
            if((b.isBlack && b.isLeft) || (!b.isBlack && !b.isLeft))
                return false;
        }
    }
    return true;
}

void Simulator::swap(){
    size_t random = std::rand() % balls.size();
    balls[random].isLeft = !balls[random].isLeft;
}

void Simulator::printResults(){
    if(!quiet){
        for(size_t t = 0; t < trials; ++t){
            std::cout << "Trial " << (t + 1) << ": " << results[t] << "\n";
        }
    }
    std::cout << "Average: " << avg << "\n";
}

int main(int argc, char** argv){
    std::cout.precision(10);
    Simulator sim;
    sim.getOptions(argc, argv);
    sim.run();
    sim.printResults();
}