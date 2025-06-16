#ifndef METROPOLIS_HASTINGS_HPP
#define METROPOLIS_HASTINGS_HPP

#include <functional>
#include <map>
#include <string>
#include <vector>

// Function declarations
std::map<char, char> metropolis_hastings_annealing(
    const std::map<char, char> &initial_state,
    std::function<std::map<char, char>(const std::map<char, char> &,
                                       const std::vector<char> &)>
        proposal_function,
    std::function<double(const std::string &, const std::map<char, char> &)>
        log_density,
    const std::vector<char> &chars, const std::string &text, int iters = 500000,
    int print_every = 99000, double initial_temp = 1.0,
    double final_temp = 0.001);

#endif  // METROPOLIS_HASTINGS_HPP