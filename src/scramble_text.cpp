#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "utils.hpp"

int main(int argc, char *argv[]) {
  std::string input_file;
  std::string output_file;
  std::string key_file;

  // Command-line argument parsing
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg == "-i" && i + 1 < argc) {
      input_file = argv[++i];
    } else if (arg == "-o" && i + 1 < argc) {
      output_file = argv[++i];
    } else if (arg == "-k" && i + 1 < argc) {
      key_file = argv[++i];
    }
  }

  if (input_file.empty() || output_file.empty() || key_file.empty()) {
    std::cerr << "Usage: " << argv[0]
              << " -i <input_file> -o <output_file> -k <key_file>" << std::endl;
    std::cerr << "  -i: The text file to encode." << std::endl;
    std::cerr << "  -o: The file to write the secret message to." << std::endl;
    std::cerr << "  -k: The file to save the cipher key to." << std::endl;
    return 1;
  }

  // Read the source text
  std::string text;
  read_file(input_file, text);
  if (text.empty()) {
    std::cerr << "Input file is empty or could not be read." << std::endl;
    return 1;
  }

  // Generate a random permutation key
  std::vector<char> az_chars = az_list();
  std::map<char, char> p_map = generate_random_permutation_map(az_chars);

  std::cout << "Generated a new random key." << std::endl;
  print_permutation_map(p_map);

  // Apply the permutation to scramble the text
  std::string scrambled_text = apply_permutation(text, p_map);

  // Write the scrambled text to the output file
  std::ofstream out_file(output_file);
  if (!out_file.is_open()) {
    std::cerr << "Could not open output file: " << output_file << std::endl;
    return 1;
  }
  out_file << scrambled_text;
  out_file.close();
  std::cout << "Successfully saved scrambled text to " << output_file
            << std::endl;

  // Save the key to its own file
  save_permutation_map(p_map, key_file);

  return 0;
}
