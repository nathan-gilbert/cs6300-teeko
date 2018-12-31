# Nathan Gilbert
# CS 6300 
# Assignment 2 - TeekoMaster 5000
#

make:
#	g++ teeko.cpp input_check.cpp util.cpp node.cpp pcu_player.cpp -Wall -Wextra -pedantic -g -lpopt -o teeko
	g++ teeko.cpp input_check.cpp util.cpp node.cpp pcu_player.cpp -Wall -lpopt -o teeko

clean:
	rm ./teeko
