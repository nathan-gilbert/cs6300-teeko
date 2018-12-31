# CS 6300 - Artificial Intelligence Assignment 2
## March 15, 2007

## Preliminaries

## To compile

Assuming you are in the directory with the source code and corresponding Makefile:
```sh
% make
```

## To Run

```sh
% ./teeko [args...] game-statefile
```

## Problem Definition

This assignment was an exercise in using adversarial search techniques while
developing software to play the classic game of Teeko. This game involves a 5x5
grid, and 8 colored “pegs”, 4 for black and 4 for red. Initially, pieces are
placed on the board where ever the players choose, but after all pieces have
been placed, pegs can only be moved one space at a time. The game is won as
soon as a player has 4 pegs in a row, or a box.  This game is interesting from
a computer science perspective because it is a good example of fully-observable
environment, that involves two agents competing for one of many possible goals.
It’s a very good chance to employ some of the now standard adversarial search
algorithms like Minimax. These algorithms provide a good method for dealing
with adversaries which you can believe will “act rationally”, sometimes even
when they are not. In general, these algorithms must check an exponential
number of steps to determine the maximal move, but with some cleverness, the
number of nodes in the search can be reduced (although the exponential is not
eliminated.)

