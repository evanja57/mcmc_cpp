#include "deciphering_utils.hpp"

#include <algorithm>
#include <cmath>
#include <random>

double compute_log_probability(
    const std::string &text, const std::map<char, char> &permutation_map,
    const std::map<char, int> &char_to_ix,
    const std::vector<double> &frequency_statistics,
    const std::vector<std::vector<double> > &transition_matrix) {
  double log_prob = 0.0;
  if (text.empty()) {
    return log_prob;
  }

  log_prob += std::log(
      frequency_statistics[char_to_ix.at(permutation_map.at(text[0]))]);

  for (size_t i = 0; i < text.length() - 1; ++i) {
    char current_char = text[i];
    char next_char = text[i + 1];
    if (permutation_map.count(current_char) &&
        permutation_map.count(next_char)) {
      char permuted_current = permutation_map.at(current_char);
      char permuted_next = permutation_map.at(next_char);
      if (char_to_ix.count(permuted_current) &&
          char_to_ix.count(permuted_next)) {
        log_prob += std::log(transition_matrix[char_to_ix.at(permuted_current)]
                                              [char_to_ix.at(permuted_next)]);
      }
    }
  }
  return log_prob;
}

std::map<char, char> propose_move(const std::map<char, char> &p_map,
                                  const std::vector<char> &chars) {
  std::map<char, char> new_p_map = p_map;
  std::random_device rd;
  std::mt19937 g(rd());
  std::uniform_int_distribution<> dis(0, chars.size() - 1);
  int i = dis(g);
  int j = dis(g);
  while (i == j) {
    j = dis(g);
  }
  char char1 = chars[i];
  char char2 = chars[j];
  new_p_map[char1] = p_map.at(char2);
  new_p_map[char2] = p_map.at(char1);
  return new_p_map;
}

/**
 * @brief Computes the bigram (character pair) counts for a given text.
 */
TransitionCounts compute_transition_counts(
    const std::string &text, const std::map<char, int> &char_to_ix) {
  size_t n_chars = char_to_ix.size();
  TransitionCounts counts(n_chars, std::vector<int>(n_chars, 0));

  if (text.length() < 2) {
    return counts;
  }

  for (size_t i = 0; i < text.length() - 1; ++i) {
    char c1 = text[i];
    char c2 = text[i + 1];
    if (char_to_ix.count(c1) && char_to_ix.count(c2)) {
      counts[char_to_ix.at(c1)][char_to_ix.at(c2)]++;
    }
  }
  return counts;
}

/**
 * @brief Computes the log probability of a text using pre-calculated
 * transition counts.
 */
double compute_log_probability_by_counts(
    const TransitionCounts &text_transition_counts, char first_char_in_text,
    const std::map<char, char> &permutation_map,
    const std::map<char, int> &char_to_ix,
    const std::vector<double> &log_frequency_statistics,
    const std::vector<std::vector<double> > &log_transition_matrix) {
  // Start with the log probability of the first character.
  double log_prob = log_frequency_statistics.at(
      char_to_ix.at(permutation_map.at(first_char_in_text)));

  // Create a vector representing the permutation map for faster access.
  // p_map_indices[i] gives the permuted index of character i.
  std::vector<int> p_map_indices(char_to_ix.size());
  for (const auto &pair : permutation_map) {
    if (char_to_ix.count(pair.first) && char_to_ix.count(pair.second)) {
      p_map_indices[char_to_ix.at(pair.first)] = char_to_ix.at(pair.second);
    }
  }

  // Add the log probabilities from the transition matrix, weighted by the
  // counts.
  for (size_t i = 0; i < text_transition_counts.size(); ++i) {
    for (size_t j = 0; j < text_transition_counts[i].size(); ++j) {
      if (text_transition_counts[i][j] > 0) {
        log_prob += text_transition_counts[i][j] *
                    log_transition_matrix[p_map_indices[i]][p_map_indices[j]];
      }
    }
  }

  return log_prob;
}