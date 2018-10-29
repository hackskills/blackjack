# A Simple Blackjack Game

This is a simple blackjack game that I implemented to run on the UNIX terminal.

### Rules
The goal of the game is for a player to get a blackjack, or sum of 21, by drawing 2 cards. The value of each non-face card is the [pip value](https://en.wikipedia.org/wiki/Pip_(counting)), and the value of the Jack, Queen, and King are each 10. The Ace is a special card that can take on the value 1 or 11 as the player so chooses.  If the player does not reach blackjack, they may opt to draw more cards until 21 is reached. However, if the sum of their cards exceed 21, then they "bust" and automatically lose the game. The dealer then draws 2 cards, or more if the sum of the dealer's cards is less than 17. If at the end of the game, the dealer does not bust and gets a sum greater than the player's sum or if the player busts, then the player loses. If the all the players get a blackjack, then the dealer loses. However, if not all players got a blackjack, then any player who gets a blackjack can only win if the dealer does not draw a blackjack.
The full rules can be found on [Wikipedia](https://en.wikipedia.org/wiki/Blackjack).

### How the Game Works
The game can handle up to 7 players, although more players can be added by modifying the code. Each player starts with $100 and the game will continue for as long as there is a player who still has money. Players lose money by betting and then losing the round, and they win by betting and then winning the round. The player does not win or lose money if the sum is the same as the dealer's sum, as long as it's not a blackjack.
