//    Battleship game assignment for MSU CSCI 366
//    Copyright (C) 2020    Mike P. Wittie
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <https://www.gnu.org/licenses/>.


#include "common.hpp"
#include "Server.hpp"

/**
 * Calculate the length of a file (helper function)
 *
 * @param file - the file whose length we want to query
 * @return length of the file in bytes
 */
int get_file_length(ifstream *file){


}

/** Performs Server initialization
        *
        * Need to do initialization outside of the constructor, so that the object may be initialized inside the
        * googletest SetUp() method.
* The method opens player setup boards containing ship positions and checks the size of boards compared to
* board_size parameter.
* @param board_size - the size of a square board
* @param p1_setup_board - file name of player 1's board
* @param p2_setup_board - file name of player 2's board*/

void Server::initialize(unsigned int board_size,
                        string p1_setup_board,
                        string p2_setup_board) {

    if (board_size != BOARD_SIZE){
        throw ServerException("Incorrect board size");
    }
    else if (p1_setup_board.length() < 1 || p2_setup_board.length() < 1){
        throw ServerException("Empty board files");
    }
    else{
        this->board_size = board_size;
    }
}

/**
 * Checks the coordinates of a shot against setup board of player
 *
 * Check that player number within bounds, checks that shot coordinates within bounds, determines if the shot
 * results in a HIT, or a MISS.
 * @param player - player number
 * @param x - coordinate
 * @param y - coordinate
 * @return returns shot result as either HIT, MISS, or OUT_OF_BOUNDS
 */

int Server::evaluate_shot(unsigned int player, unsigned int x, unsigned int y) {

    if(player < 1 || player > MAX_PLAYERS){
        throw ServerException("Bad player number");
    }

    if(x < 0 || x > board_size - 1 || y < 0 || y > board_size - 1){
        return OUT_OF_BOUNDS;
    }

    ifstream inpf;

    if(player == 1){
        inpf.open("player_2.setup_board.txt");
    }
    if(player == 2){
        inpf.open("player_1.setup_board.txt");
    }
    vector<vector<char>> board(board_size, vector<char>(board_size));

    while(!inpf.eof()){
        for(int i = 0; i < board_size; i++){
            for(int j = 0; j < board_size; j++){
                inpf >> board[i][j];
            }
        }
    }
    if(board[x][y] == '_'){
        return MISS;
    } else{
        return HIT;
    }
}

/**
 * Processes a shot issued by player
 *
 * Gets the shot from player, extracts coordinates, passes the information of evaluate_shot, and writes the result
 * into player_#.result.json.
 * @param player - player number
 * @return returns SHOT_PROCESSED, or NO_SHOT_FILE if nothing to process
 */
int Server::process_shot(unsigned int player) {

    if(player < 1 || player > MAX_PLAYERS){
        throw ServerException("Bad player number");
    }

    int x, y;
    string shotf = "player_" + to_string(player) + ".shot.json";
    string resultf = "player_" + to_string(player) + ".result.json";

    ifstream ipf(shotf);
    if(!ipf){
        return NO_SHOT_FILE;
    }
    else if(ipf.good()) {
        cereal::JSONInputArchive read_archive(ipf);
        read_archive(x, y);
        ipf.close();
        remove(shotf.c_str());

        int result = evaluate_shot(player, y, x);

        remove(resultf.c_str());

        ofstream opf(resultf);
        cereal::JSONOutputArchive write_archive(opf);
        write_archive(CEREAL_NVP(result));
        return SHOT_FILE_PROCESSED;
    }
}