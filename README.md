# Chess Game using the SDL2 library
Chess AI is in the works.

The AI uses a Monte Carlo Search Tree in order to find the best moves. During the expansion phase of the MCTS Algorithm, moves are selected using a reasonably sized Neural Net (10 Hidden * 100 Nodes Each).

Improvements in the design will come form improved tokenization of the board. Currently the input size is 672. The output is of size 128. 

I like the current design because it can allow for trading scenarios to be searched first. This is because of the 128 size output, there are two sets of 64. These sets represent the chosen source and destination squares of the moves. Every legal move in this output is included as a possible tree in the expansion phase.

## The Board:
<div>
<img src="chess.jpg" alt="chess game image"></img>
</div>

### To Compile:
g++ --std=c++11 -DSIZE=6 chess_main.cc -o m -I include -L lib -l SDL2-2.0.0


### Improvements:
En Passant and Castling have yet to be implemented because, while such a feature would be easy to add, it needs to be seamless because the Chess Logic Code is used for the AI Move Generation.

Create training sets use a python script that connects to a python chess api. Also train some nets.

A lot more.