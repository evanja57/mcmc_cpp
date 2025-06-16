#ifndef DECIPHERING_UTILS_HPP
#define DECIPHERING_UTILS_HPP

#include <map>
#include <string>
#include <vector>

// A 2D vector to store the counts of character transitions.
using TransitionCounts = std::vector<std::vector<int> >;

// Function declarations
double compute_log_probability(
    const std::string &text, const std::map<char, char> &permutation_map,
    const std::map<char, int> &char_to_ix,
    const std::vector<double> &frequency_statistics,
    const std::vector<std::vector<double> > &transition_matrix);
std::map<char, char> propose_move(const std::map<char, char> &p_map,
                                  const std::vector<char> &chars);

/**
 * @brief Computes the bigram (character pair) counts for a given text.
 * This is done once before the MCMC simulation to avoid re-scanning the text.
 * @param text The input text string.
 * @param char_to_ix A map from characters to their corresponding integer
 * index.
 * @return A 2D vector where [i][j] is the number of times character j follows
 * character i.
 */
TransitionCounts compute_transition_counts(
    const std::string &text, const std::map<char, int> &char_to_ix);

/**
 * @brief Computes the log probability of a text using pre-calculated
 * transition counts. This is the fast version used inside the MCMC loop.
 * @param text_transition_counts The pre-calculated bigram counts of the text
 * to be decoded.
 * @param first_char_in_text The first character of the text to be decoded.
 * @param permutation_map The current permutation map being tested.
 * @param char_to_ix A map from characters to their integer index.
 * @param log_frequency_statistics The pre-calculated log of the frequency
 * statistics.
 * @param log_transition_matrix The pre-calculated log of the transition
 * matrix.
 * @return The log likelihood of the text under the given permutation.
 */
double compute_log_probability_by_counts(
    const TransitionCounts &text_transition_counts, char first_char_in_text,
    const std::map<char, char> &permutation_map,
    const std::map<char, int> &char_to_ix,
    const std::vector<double> &log_frequency_statistics,
    const std::vector<std::vector<double> > &log_transition_matrix);

#endif  // DECIPHERING_UTILS_HPP