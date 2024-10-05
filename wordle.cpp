#include <iostream>
#include <limits>
#include <fstream>
#include <random>
#include <chrono>
#include <vector>
#include <string>
#include <thread>
#include <chrono>

const std::string wordListFile = "words.txt";
enum class Difficulty {Easy, Medium, Hard};

void wordle_game();
void check_wordle_guess(std::string userGuess, std::string targetWord);
Difficulty get_difficulty();
std::pair<int, int> get_word_length(Difficulty difficulty);
std::string get_random_word(std::pair<int, int> wordLength);
int validate_choice(int minNum, int maxNum);
int validate_choice(int minNum, int maxNum, std::string prompt);
void to_upper(std::string& string);
void loading_bar();
void wordle_header();


int main() {   
    while (true) {
        std::cout << "Welcome to Wordle!\n"
            << "[1] Play Wordle\n"
            << "[2] How to play\n"
            << "[3] Exit\n";
        int choice = validate_choice(1, 2, "Option: ");
        switch (choice) {
        case 1:
            std::cout << "Starting Wordle...\n";
            loading_bar();
            wordle_game();
            break;
        case 2: std::cout << "1. Start: Run the program and select 'Play Wordle'.\n"
            << "2. Difficulty: Choose Easy (3-4 letters), Medium (5-6 letters), or Hard (7+ letters).\n"
            << "3. Guess: Guess the word. Blocks represent letters.\n"
            << "4. Input: Type your guess and press Enter. It should match the target word's length.\n"
            << "5. Check: Your guess is checked. \n"
            << "  - **Green**: Correct letter and position.\n"
            << "  - **Yellow**: Correct letter, wrong position.\n"
            << "  - **White**: Incorrect letter.\n"
            << "6. Options: During guessing, enter ‘D’ to change difficulty, or ‘N’ for a new word  \n"
            << "7. Win: Game continues until you guess correctly.\n";

            system("pause");
            break;
        case 3: std::cout << "Goodbye user!\n";
            break;
        }
    }
	return 0;
}

void wordle_game() {
    Difficulty difficulty = get_difficulty();
    std::pair<int, int> wordLength = get_word_length(difficulty);
    std::string targetWord = get_random_word(wordLength);

    wordle_header();
    std::cout << "The target word has been selected, Goodluck! \n[D] Change difficulty, [N] New word\n\n";

    const char emptyBlock = 177;  // Light shade block
    int guessCounter = 1;

    std::string userGuess;
    while (true) {


        std::cout << "Guess " << guessCounter << ": ";
        for (int i = 0; i < targetWord.length(); i++) {
            std::cout << emptyBlock;
        }
        std::cout << "\r";
        std::cout.flush();

        std::cout << "Guess " << guessCounter << ": ";
        std::cin >> userGuess;
        to_upper(userGuess);

        if (userGuess == "D") {
            wordle_game();
        }
        else if (userGuess == "N") {
            wordle_header();
            std::cout << "New target word has been selected, Goodluck! \n[D] Change difficulty, [N] New word\n\n";
            targetWord = get_random_word(wordLength);
            guessCounter = 1;
            continue;
        }
        else if (userGuess.length() != targetWord.length()) {
            std::cout << "Invalid input! The length of the guess should be " << targetWord.length() << ".\n";
            continue;
        }

        std::cout << "\r";
        std::cout.flush();

        std::string leftSide = "Guess " + std::to_string(guessCounter) + ": ";
        for (int i = 0; i < leftSide.length(); i++) {
            std::cout << " ";
        }

        check_wordle_guess(userGuess, targetWord);

        if (userGuess == targetWord) {
            std::cout << "Congratulations! You've guessed the word correctly.\n";
            std::cout << "[1] Play again\n"
                << "[2] Change Difficulty\n"
                << "[3] Exit\n";
            int choice = validate_choice(1, 3);
            switch (choice) {
            case 1:
                targetWord = get_random_word(wordLength);
                guessCounter = 1;
                break;
            case 2:
                wordle_game();
                break;
            case 3:
                return;
            }
            continue;
        }

        guessCounter++;
    }
}




void check_wordle_guess(std::string userGuess, std::string targetWord) {
    const char block = 219;  // Full block
    //const char emptyBlock = 177;  // Light shade block
    std::string green = "\033[32m";
    std::string yellow = "\033[33m";
    std::string white = "\033[37m";
    std::string reset = "\033[0m";

    std::vector<bool> matched(targetWord.size(), false);  // Keep track of matched letters

    for (int i = 0; i < userGuess.length(); i++) {
        if (i < targetWord.length() && userGuess[i] == targetWord[i]) {
            std::cout << green << userGuess[i] << "" << reset;
            matched[i] = true;  // flag to indicate match
        }
        else {
            size_t foundPos = targetWord.find(userGuess[i]);
            if (foundPos != std::string::npos && !matched[foundPos]) {
                std::cout << yellow << userGuess[i] << "" << reset;
                matched[foundPos] = true;  
            }
            else {
                std::cout << white << userGuess[i] << "" << reset;
            }
        }
    }
    std::cout << "\n";
}



Difficulty get_difficulty() {
    wordle_header();
	std::cout << "Choose a difficulty:\n"
		<< "[1] Easy (3-4)\n"
		<< "[2] Medium (5-6)\n"
		<< "[3] Hard (7+)\n";
	int choice = validate_choice(1,3, "Option: ");

    Difficulty difficulty[3] = {Difficulty::Easy, Difficulty::Medium, Difficulty::Hard};
	return difficulty[choice-1];
}


std::pair<int, int> get_word_length(Difficulty difficulty) {
    switch (difficulty) {
        case Difficulty::Easy:
            return std::make_pair(3, 4);
        case Difficulty::Medium:
			return std::make_pair(5, 6);
        case Difficulty::Hard:
            return std::make_pair(7, std::numeric_limits<int>::max());
    }
}


std::string get_random_word(std::pair<int, int> wordLength) {
    std::ifstream file(wordListFile);
    std::string line;
    std::vector<std::string> words;
    
    while (std::getline(file, line)) {
        if (line.length() >= wordLength.first && line.length() <= wordLength.second) {
            words.push_back(line);
        }
    }

    std::random_device rd;
    std::default_random_engine engine(rd());
    std::uniform_int_distribution<int> range(0, words.size()-1);

    int randomIndex = range(engine);

    to_upper(words[randomIndex]);
    return words[randomIndex];
}


int validate_choice(int minNum, int maxNum) {
    long long number;  // Can hold higher values than int 
    while (true) {
        std::cin >> number;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // Ignore the newline character
        if (std::cin.fail() || number < std::numeric_limits<int>::min() || number > std::numeric_limits<int>::max()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input! Please enter a number within the range of int.\n";
        }
        else if (number < minNum || number > maxNum) {
            std::cout << "Invalid input! Please enter a number between " << minNum << " and " << maxNum << ".\n";
        }
        else {
            return static_cast<int>(number);  // If the input is valid, return the number
        }
    }
}


int validate_choice(int minNum, int maxNum, std::string prompt) {

    long long number;  // Can hold higher values than int 
    while (true) {
        std::cout << prompt;
        std::cin >> number;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (std::cin.fail() || number < std::numeric_limits<int>::min() || number > std::numeric_limits<int>::max()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input! Please enter a number within the range of int.\n";
        }
        else if (number < minNum || number > maxNum) {
            std::cout << "Invalid input! Please enter a number between " << minNum << " and " << maxNum << ".\n";
        }
        else {
            return static_cast<int>(number);  // If the input is valid, return the number
        }
    }
}

void to_upper(std::string& string) {
    for (char& c : string) {
		c = std::toupper(c);
	}
}

void loading_bar() {
    const char block = 219;  // Full block
    const char emptyBlock = 177;  // Light shade block

    std::random_device rd;
    std::default_random_engine engine(rd());
    std::uniform_int_distribution<int> dist_duration(100000, 300000);
    std::uniform_int_distribution<int> dist_counts(10, 20);
    std::uniform_int_distribution<int> dist_addition(1, 7); 
    int duration = dist_duration(engine);
    int counts = dist_counts(engine);

    const int barLength = 30; 
    int currentLength = 0;

    std::cout << "\nLoading:\n";
    for (int i = 0; i < barLength; ++i) {
        std::cout << emptyBlock;
    }
    std::cout << "\r"; // Moves the cursor back to the start of the line so that the next output will overwrite the previous one
    std::cout.flush(); 

    for (int i = 0; i < counts && currentLength < barLength; i++) {
        std::this_thread::sleep_for(std::chrono::microseconds(duration));
        int addition = dist_addition(engine);
        for (int j = 0; j < addition && currentLength < barLength; j++) {
            std::cout << block;
            ++currentLength;
        }
        std::cout.flush();
    }
}

void wordle_header() {
    system("cls");
    std::cout << "WORDLE\n\n";
}
