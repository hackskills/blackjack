/** blackjack.cpp
 * Shaquille Que 09-29-2018
 */

#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include <unistd.h>
using namespace std;

int MIN_PLAYERS = 1;
int MAX_PLAYERS = 7;

// CONSTANTS
const string suit_names[4] = { "clover", "diamond", "heart", "spade" };
const string value_names[13] = {"Ace","2","3","4","5","6","7","8","9","10","Jack","Queen","King"};
// safe => player can draw again
// blackjack => player got exactly 21
// bust => player got more than 21
enum currentStates {safe, blackjack, bust};
// distinguish between dealer and player
enum currentPlayer {dealer, player};

//Global Variables
// Initialize each player's money to be 100
vector<int> PLAYER_MONEY(MAX_PLAYERS+1, 100);
// Initialize each player's bet to be 0
vector<int> PLAYER_BET(MAX_PLAYERS+1, 0);
// Initialize vector of players' scores to be modified
// every time player draws cards below
// set player 0 to be the dealer
vector<int> PLAYER_SCORES(MAX_PLAYERS+1, 0);

//Create vector of player states (safe, blackjack, bust)
vector<int> PLAYER_STATES(MAX_PLAYERS+1);


class Card {
    private:
    int _card;

    public:
    Card() {
        _card = 0;
    }
    Card(int x) {
        _card = x;
    }
    //return pip value for non-face cards
    // return 1 for ace, 10 for jack, queen, king
    int value() {
        int val = (_card%13)+1;
        return val > 10 ? 10 : val; 
    }
    string suit() {
        return suit_names[(_card-(_card%13))/13];
    }
    string printCard() {
        return value_names[_card%13] + " " + suit();
    }
};

class Deck {
    private:
    Card _cards[52];
    int _nextCardIndex;

    public:
    Deck() {
        for (int i=0; i<52; i++) {
            _cards[i] = Card(i);
        }
        shuffle();
        _nextCardIndex = 0;
    }
    void shuffle() {
        std::srand(std::time(NULL));
        for (int i=0; i<(52-1); i++) {
            int swapIndex = (rand()%(52-1));
            swap(_cards[i], _cards[swapIndex]);
        }    
    }
    Card drawCard() {
        return _cards[_nextCardIndex++];
    }
};

void shortDelay() {
    usleep(300000);
}
void longDelay() {
    usleep(600000);
}
void veryLongDelay() {
    usleep(1200000);
}
void skipLine() {
    cout << "\n";
}

bool promptUserYesNo(string prompt) {
    // Returns true if user says yes, false if no
    // if user does not answer with y,yes,n,no, then
    // print error message and prompt user again
    while (true) {
        cout << prompt << "(y/n): ";
        string str;
        cin >> str;
        if (str == "n" || str == "no") {
            return false;
        }
        else if (str == "y" || str == "yes") {
            return true;
        }
        else {
            cout << "Please answer with only 'y' or 'n'." << "\n";
        }
    }
}

int promptUserInt(string prompt, int low, int high) {
    // Prompt user for integer.
    // If first character is not integer
    // or the integer is out of bounds,
    // prompt user for integer again.
    while(true) {
        cout << prompt << " (integer between " << low << " and " << high << "):\n";
        string s;
        cin >> s;
        int num = -1;
        if (isdigit(s[0])) {
            num = stoi(s);
            if (num >= low && num <= high) {
                return num;
            }
        }
        cout << "Please only enter an integer between " << low << " and " << high << ".\n";
    }
}

//returns maximum value of hand if second argument is true
// and minimum value of hand otherwise
int max_val(vector<Card> cards, bool wantMax) {
    int total = 0;
    bool aceInCards = false;
    vector<Card>::iterator it = cards.begin(); //iterator for vector
    for ( ; it!=cards.end(); it++) {
        total += (*it).value();
        if ((*it).value() == 1) { //if ace is in the pile
            aceInCards = true;
        }
    }
    if (aceInCards && wantMax) {
        //if ace is in pile, account for both cases
        // when the value of ace is 1 and when it is 11
        // note that having 2 aces will go over 21, which is a loss
        return (total+10) > 21 ? total : total+10; 
    }
    return total;
}

// Prints either player's or dealer's cards with total value
void printPlayer(vector<Card> pCards, int currPlayer) {
    if (currPlayer == player) {
        cout << "Your cards: ";
    } else {
        cout << "Dealer's card(s): ";
    }
    vector<Card>::iterator it = pCards.begin();
    for ( ; it!=pCards.end(); it++) {
        cout << (*it).printCard();
        cout << (it+1 == pCards.end() ? ";     " : ", ");
    }
    cout << "Total: " << max_val(pCards, true) << ".\n";
    return;
}

// Get player's bet and ensures an int
int getBet(int playerNumber) {
    cout << "You currently have $" << PLAYER_MONEY[playerNumber] << ".\n";
    longDelay();
    string prompt;
    prompt = "Please enter the amount you want to bet";
    int val = promptUserInt(prompt, 0, PLAYER_MONEY[playerNumber]);
    return val;
}

// Returns state (safe, blackjack, bust) and prints out state if
// the state is blackjack or bust
int getState(vector<Card> vecCards, int playerNumber) {
    int state;
    if (max_val(vecCards, true)==21) { // player got blackjack
        PLAYER_SCORES[playerNumber] = 21;
        cout << "Blackjack!\n";
        veryLongDelay();
        if (playerNumber == 0) { // i.e. dealer got blackjack
            cout << "All players lose.\n";
            veryLongDelay();
        }
        return blackjack;
    } else if (max_val(vecCards, false) > 21) { // player got bust
        PLAYER_SCORES[playerNumber] = max_val(vecCards, false);
        cout << "Bust!\n";
        veryLongDelay();
        return bust;
    }
    PLAYER_SCORES[playerNumber] = max_val(vecCards,true);
    return safe;
}

// Decreases player's money by bet and prints this
void lostBet(int playerNumber, int state) {
    PLAYER_MONEY[playerNumber] -= PLAYER_BET[playerNumber];
    if (state == bust) {
        cout << "Player " << playerNumber << " bust with a total score of " << PLAYER_SCORES[playerNumber] << " and lost $" << PLAYER_BET[playerNumber] << "!\n";
    } else if (state == blackjack) {
        cout << "Player " << playerNumber << " got a blackjack and lost $" << PLAYER_BET[playerNumber] << "!\n";
    } else { // state == safe
        cout << "Player " << playerNumber << " got a total score of " << PLAYER_SCORES[playerNumber] << " and lost $" << PLAYER_BET[playerNumber] << "!\n";
    }
}

// Increases player's money by bet and prints this
void wonBet(int playerNumber, int state) {
    PLAYER_MONEY[playerNumber] += PLAYER_BET[playerNumber];
    // player never wins if bust
    if (state == blackjack) {
        cout << "Player " << playerNumber << " got a blackjack and won $" << PLAYER_BET[playerNumber] << "!\n";
    } else { // state == safe
        cout << "Player " << playerNumber << " got a total score of " << PLAYER_SCORES[playerNumber] << " and won $" << PLAYER_BET[playerNumber] << "!\n";
    }
}

// Performs the final outcome after all players and dealer has drawn
// Takes care of the general case when not all players got a blackjack
// or not all got bust and not all passed
void printFinalOutcome(int playerNumber, int dealerScore) {
    // Player loses if they bust, regardless of dealer's cards
    if (PLAYER_STATES[playerNumber]==bust) {
        lostBet(playerNumber, bust);
    // If player got blackjack, dealer wins
    // only if dealer gets blackjack
    } else if (PLAYER_STATES[playerNumber]==blackjack) {
        if (PLAYER_STATES[0]==blackjack) { // dealer also got blackjack
            lostBet(playerNumber, blackjack);
        } else { // dealer did not get blackjack
            wonBet(playerNumber, blackjack);
        }
        
    } else { // Otherwise, player is safe (i.e. did not bust or get blackjack)
        if (PLAYER_STATES[0] == bust) {
            wonBet(playerNumber, safe);
        } else { // compare dealer score with player score
            if (PLAYER_SCORES[playerNumber]>dealerScore) { //player has higher score
                wonBet(playerNumber, safe);
            } else if (PLAYER_SCORES[playerNumber]==dealerScore) { // player loses
                cout << "Player " << playerNumber << " got the same score as the dealer!\n";
            } else {
                lostBet(playerNumber, safe);
            }
        }
    }
}

// Delay program to make it more human friendly


int main(){
    
    cout << "Welcome to Blackjack!\n";
    // Initialize the prompt to be used for each user input
    string prompt;
    prompt = "Enter the number of players";
    // Get number of players between 1 and 7
    int players = promptUserInt(prompt, MIN_PLAYERS, MAX_PLAYERS);
    shortDelay();
    cout << "Registered number of players: " << players << ".\n";
    bool continueGame = true;
    while(continueGame) {

        //Initialize the deck of cards
        Deck deck;
        
        
        // Set flag allPlayersPass to true
        // If any player plays this round, set flag to false
        bool allPlayersPass = true;


        // Initialize 2D vector of players' cards
        vector<vector<Card> > playersCards(players);


        Card dealerCard1 = deck.drawCard();
        Card dealerCard2 = deck.drawCard();
        // Add dealer's card to vector dealerCards
        vector<Card> dealerCards;
        dealerCards.push_back(dealerCard1);
        // Add dealer's 2nd card only after all players finish

        longDelay();
        // Loop through all players
        for (int i=1; i<=players; i++) {
            skipLine();
            skipLine();
            

            if (PLAYER_MONEY[i]==0) { // Player i has no money, skip turn
                cout << "Player " << i << " is broke!\n";
                PLAYER_BET[i] = 0;
                veryLongDelay();
            } else { // Player i has money, continues playing game
                cout << "Welcome Player " << i << "!\n";
                veryLongDelay();
                
                int bet = getBet(i);
                PLAYER_BET[i] = bet;
                skipLine();
                shortDelay();

                if (bet==0) { // Player i does not want to bet, skip round
                    cout << "Player " << i << " passes this round.\n";
                    veryLongDelay();
                } else { // Player i bets some money, plays round
                    allPlayersPass = false; // A player did not pass, set flag to false
                    int state = safe; // player has not gotten blackjack or bust
                    Card card1 = deck.drawCard();
                    Card card2 = deck.drawCard();
                    
                    // put player i's cards in vector pCards
                    vector<Card> pCards;
                    pCards.push_back(card1);
                    pCards.push_back(card2);

                    printPlayer(dealerCards, dealer);
                    longDelay();
                    printPlayer(pCards, player);
                    veryLongDelay();
                    
                    if (max_val(pCards, true)==21) { // player already got blackjack
                        state = blackjack;
                        cout << "Blackjack!\n";
                        veryLongDelay();
                    }
                    skipLine();

                    while (state==safe) { // while player has not gotten blackjack or bust
                        prompt = "Would you like to draw another card?";
                        bool userWantsToDraw = promptUserYesNo(prompt); // ask if player wants to draw
                        longDelay();
                        skipLine();
                        if (!userWantsToDraw) { // player does not want to draw anymore
                            PLAYER_SCORES[i] = max_val(pCards, true); // set score
                            break; // break out of while loop, does not prompt player to draw again
                        }

                        //otherwise, player still wants to draw
                        Card newCard = deck.drawCard(); 
                        pCards.push_back(newCard);
                        cout << "You drew " << newCard.printCard() << ".\n"; //print newly drawn card
                        veryLongDelay();
                        skipLine();
                        printPlayer(pCards, player);
                        state = getState(pCards, i);
                        veryLongDelay();
                        skipLine();
                    }
                    PLAYER_STATES[i] = state;
                }
            }
        }
        skipLine();
        skipLine();
        longDelay();
        
        //
        // dealer draws if at least one player did not bust 
        // or if at least one player did not get a blackjack
        //

        // Initialize flags to be true
        // will be changed to false once exceptions
        // are found. If no exception found, then
        // flags remain true
        bool allPlayersBust = true;
        bool allPlayersBlackjack = true;
        bool allPlayersBroke = true;

        for (int i=1; i<=players; i++) {
            // if any player did not bust, the allPlayersBust is false
            if (PLAYER_SCORES[i]<=21 && PLAYER_BET[i]!=0) { 
                allPlayersBust = false;
            }
            // if any player did not get Blackjack, then allPlayersBlackjack is false
                allPlayersBlackjack = false;
            if (PLAYER_SCORES[i]!=21 && PLAYER_BET[i]!=0) { 
            }
        }
        if (allPlayersPass) { // All players pass. Skip to next round
            cout << "All players passed.\n";
            skipLine();
            longDelay();
        } else if (allPlayersBust) { // All players bust. Dealer wins
            cout << "All players who betted bust. Dealer wins!\n";
            longDelay();
            skipLine(); 
            for (int playerNumber=1; playerNumber<=players; playerNumber++) {
                if (PLAYER_BET[playerNumber]!=0) {
                    lostBet(playerNumber, bust); // subtract players' bets from their money
                    longDelay();
                } else {
                    cout << "Player " << playerNumber << " skipped this round.\n";
                }
            }
        } else if (allPlayersBlackjack) { // All players get blackjack, so Dealer loses
            cout << "All players got a blackjack!\n";
            skipLine();
            for (int playerNumber=1; playerNumber<=players; playerNumber++) {
                wonBet(playerNumber, blackjack); // add players' bets to their money
                longDelay();
            }
        } else { // otherwise, Dealer has to show and draw cards to determine outcome
            dealerCards.push_back(dealerCard2);
            printPlayer(dealerCards, dealer);
            skipLine();
            // Dealer keeps drawing until score reaches 17 or more
            // by convention, dealer still draws if ace is drawn
            // along with cards with total 6 (i.e. max_val = 17 and min_val = 6)
            while (max_val(dealerCards, true)<17 || ((max_val(dealerCards,true)==17)&&(max_val(dealerCards,false)==6))) { 
                Card newCard = deck.drawCard();
                dealerCards.push_back(newCard);
                veryLongDelay();
                cout << "Dealer drew " << newCard.printCard() << ".\n";
                veryLongDelay();
                printPlayer(dealerCards, dealer);
            }
            int dealerState = getState(dealerCards, 0);
            skipLine();
            PLAYER_STATES[0] = dealerState;
            // Loop through all players
            for (int playerNumber=1; playerNumber<=players; playerNumber++) {
                veryLongDelay();
                // If player skipped this round
                if (PLAYER_MONEY[playerNumber]==0 || PLAYER_BET[playerNumber]==0) { 
                    cout << "Player " << playerNumber << " skipped this round.\n";
                    longDelay();
                } else {
                // If player did not skip this round, change
                // their money according to their bet
                    printFinalOutcome(playerNumber, max_val(dealerCards, true));
                }
            }
        }
        
        // Prints each player's money
        for (int i=1; i<=players; i++) {
            veryLongDelay();
            cout << "Player " << i << " has $" << PLAYER_MONEY[i] << ".\n";
        }

        // Sets flag allPlayersBroke to true if all players are broke
        for (int i=1; i<=players; i++) {
            if (PLAYER_MONEY[i]!=0) { // All players are broke!
                allPlayersBroke = false; 
            }
        }
        // Ends game if all players are broke
        if (allPlayersBroke) {
            skipLine();
            skipLine();
            veryLongDelay();
            cout << "Game over! All players are broke.\n";
            veryLongDelay();
            cout << "Thank you for playing. I hope you enjoyed the game!\n";
            skipLine();
            skipLine();
            break;
        }

        skipLine();
        skipLine();
        veryLongDelay();
        prompt = "Do you want to play another round?";
        
        continueGame = promptUserYesNo(prompt);
        // If players do not want to continue, greet farewell
        if (!continueGame) {
            skipLine();
            veryLongDelay();
            cout << "Farewell. Have a nice day!\n";
        }
        skipLine();
        skipLine();
    }

    return 0;
}