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
#include "Client.hpp"
#include <iostream>
#include <fstream>
#include <cereal/archives/json.hpp>
#include <cereal/types/vector.hpp>


Client::~Client() {
}


void Client::initialize(unsigned int player, unsigned int board_size){

    this->player = player;
    this->board_size = board_size;
    string playerf = "player_" + to_string(this->player) + ".action_board.json";

    //create 2d array for the action board and fill with 0's

    vector<vector<int>> board;
    vector<int> row;

    for(int i = 0; i < board_size; i++){
        row.push_back(0);
    }
    for(int j = 0; j < board_size; j++){
        board.push_back(row);
    }

    remove(playerf.c_str());
    //create and serialize action board
    ofstream ab_ofp(playerf);
    cereal::JSONOutputArchive write_archive(ab_ofp);
    write_archive(cereal::make_nvp("board", board));
}

/**
 * Fires a shot on the coordinate target and creates a player_#.shot.json file
 * @param x - coordinate
 * @param y - coordinate
 */
void Client::fire(unsigned int x, unsigned int y) {

    string shotf = "player_" + to_string(this->player) + ".shot.json";

    remove(shotf.c_str());
    //create and serialize action board
    ofstream shotofp(shotf);
    cereal::JSONOutputArchive write_archive(shotofp);
    write_archive(cereal::make_nvp("x", x), cereal::make_nvp("y", y));
}

/**
 * Checks if a result file is available for
 * @return true if result is available, false otherwise
 */
bool Client::result_available() {

    string filename = "player_" + to_string(this->player) + ".result.json";
    ifstream resultif(filename);
    return resultif.good();

}

/**
 * Gets the result from the player_#.result.json
 * @return the result as either HIT, MISS, or OUT_OF_BOUNDS
 */
int Client::get_result() {

    string filename = "player_" + to_string(this->player) + ".result.json";
    int result;

    ifstream resultif(filename);
    cereal::JSONInputArchive read_archive(resultif);
    read_archive(result);
    resultif.close();

    remove(filename.c_str());

    if(result > 1 || result < -1){
        throw invalid_argument("Wrong result");
    }
    return result;

}

/**
 * Updates the internal representation of player_#.action_board.json on the result of a shot.
 * @param result - the result returned from the server
 * @param x - coordinate
 * @param y - coordinate
 */
void Client::update_action_board(int result, unsigned int x, unsigned int y) {

    vector<vector<int>> board;
    string playerf = "player_" + to_string(this->player) + ".action_board.json";

    ifstream ipf(playerf);
    cereal::JSONInputArchive read_archive(ipf);
    read_archive(board);
    ipf.close();
    remove(playerf.c_str());

    board[y][x] = result;

    ofstream ofp(playerf);
    cereal::JSONOutputArchive write_archive(ofp);
    write_archive(cereal::make_nvp("board", board));


}

/**
 * Formats a string representing player_#.action_board.json as ASCII
 * @return ASCII representation of the action board
 */
string Client::render_action_board(){

    string boardstr;
    vector<vector<int>> board;
    string playerf = "player_" + to_string(this->player) + ".action_board.json";

    ifstream ipf(playerf);
    cereal::JSONInputArchive read_archive(ipf);
    read_archive(board);
    ipf.close();
    remove(playerf.c_str());

    for(int i = 0; i < 21; i++){
        for(int j = 0; j < 21; j++){
            boardstr += to_string(board[i][j]);
        }
    }
    return boardstr;
}