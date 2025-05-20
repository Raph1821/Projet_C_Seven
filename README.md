# **SEVEN DEADLY SINS**

#### Participants : Minh Nhut NGUYEN & Manel MOULAHCENE

## **Project Overview**

This document outlines the implementation of a competitive framework for the Sevens (7,Nana Narabe) card game. We implement here a strategy that competes against those of our classmates to see whose algorithm most effectively wins at the Sevens card game.

## **Implemented strategy and justification**
### **1. Prioritizing the 7s:**

The game of Sevens always begins with the 7 cards. Each 7 played unlocks a new suit (Spades, Hearts, Clubs, Diamonds).
The strategy gives top priority to playing 7s, because without them, no progression can happen within that suit. Playing a 7 opens up the possibility to play higher and lower cards in that suit.

### **2. Future Optimization:**

When there are multiple playable cards, the strategy selects the one that is likely to unlock the most other cards in the player’s hand.
For example, playing a card like 9♣ might allow the player to later play 8♣. This minimizes future blocks and increases the chances of continuing turns without having to pass.

### **3. Strategic Preservation of Central Cards (6 and 8):**

Cards ranked 6 and 8 are central to each suit’s sequence. Playing them too early might enable opponents who are waiting on 5s or 9s.
So the strategy intentionally delays playing 6s and 8s whenever possible, to control the game’s pace and avoid giving easy plays to opponents.

### **4. Opponent Behavior Monitoring:**

The strategy begins collecting data on opponents’ behaviors:

-  Each played card is recorded (observeMove) to eventually reconstruct opponent hands or identify suit progress.
-  Each pass is tracked (observePass) to detect weaknesses by suit in other players. This may later be used for blocking tactics.

Although the data is not yet fully exploited, storing it lays the foundation for advanced adaptive gameplay in future improvements.

### **5. Strategic Retention / Bluffing:**

Sometimes, not playing a card you technically could is a better move.
For example, if the player holds 6♠ and also 5♠, playing 6♠ immediately enables the 5♠.
However, deliberately holding onto the 6♠ can force opponents to pass, gaining a psychological and tactical edge.

This introduces a form of controlled bluffing, making the strategy less predictable and more punishing to opponents.

### **6. Partial Randomization:**

To avoid making the strategy too predictable, especially against other AIs or human players, some decisions are made randomly among equally viable options.
For example, if multiple 7s are playable, the strategy picks one at random. This adds behavioral variation and makes the strategy harder to counter through pattern recognition.

## **Sample performance** 
![test](test.png)

## **Limitations and conclusions**
### ***Limitations:***
1. Passive opponent reading:
The strategy stores information about moves and passes but does not yet actively use it to deduce opponents’ hands or to intentionally block sequences.

2. No calculated risk-taking:
The strategy does not play aggressively, even in situations where doing so might force an opponent to pass or disrupt their plan.

3. Basic strategic retention:
Bluffing is only implemented for 6s and 8s with their immediate neighbors (5 or 9).
Other more advanced forms of blocking or strategic delay are not yet considered.

### ***conclusions:***
This project allowed us to design a card game in C++ with a modular architecture. We developed a smart strategy and tested it against random and greedy strategies. This work strengthened our understanding of object-oriented programming and dynamic loading.


## **Credit/acknowledgement/references** 

