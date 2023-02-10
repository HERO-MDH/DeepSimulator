/*
Copyright 2022 Mohammad Riazati

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef _DATAINTERFACE_H
#define _DATAINTERFACE_H

#include <fstream>
#include <sstream>
#include <string.h>
#include <algorithm> 
#include <cctype>
#include <locale>
#include <iostream>

#include "utils.h"

using namespace std;

DataType_input ****testdata;
int testlabels[10000];

void InitilizeData(int data_count, int x_size, int y_size, int z_size) {
	string param_file;
	ifstream datafile;

	param_file = FindInputFile("data.h");
	datafile.open(param_file);

	if (datafile.is_open()) cout << "Data file opened: " << param_file << endl; 
	else {
		cout << "Data file open failed" << endl;
		_getche(); exit(0);
	}

	int current_line_number = 0;

	testdata = new DataType_input***[10000];
	for(int i = 0; i < data_count; i++) {
		testdata[i] = new DataType_input**[x_size];
		for(int j = 0; j < x_size; j++) {
			testdata[i][j] = new DataType_input*[y_size];
			for(int k = 0; k < y_size; k++) {
				testdata[i][j][k] = new DataType_input[z_size];
			}
		}
	}

	int current_data, current_x, current_y, current_z;
	current_data = current_x = current_y = current_z = 0;

	bool is_reading_data = true;

	string line, line_remaining;
	char * current_token_char;
	string current_token;
	char * line_char_star;

	while (getline(datafile, line)) {
		current_line_number++;
		if (line.find("data") != string::npos) {
			size_t a = line.find("=");
			line = line.substr(a+1);
		}

		if (line.find("labels") != string::npos) {
			size_t a = line.find("=");
			line = line.substr(a+1);
			is_reading_data = false;
			current_data = 0;
		}

		line_char_star = new char[line.length() + 1];
		current_token_char = strtok((char*)line.c_str()," {},\t;");
		while(current_token_char) {
			current_token = current_token_char;

			if(is_reading_data) {
				if (current_data < data_count) {
					testdata[current_data][current_x][current_y][current_z] = (DataType_input)atof(current_token.c_str());
					current_z++;
					if (current_z == z_size) {current_z = 0; current_y++;}
					if (current_y == y_size) {current_y = 0; current_x++;}
					if (current_x == x_size) {
						current_x = 0; current_data++; 
						if ((current_data + 1) % 1000 == 0) 
							cout << "ReadData: " << current_data + 1 << endl;
					}
				}
			}
			else {
				if (current_data < data_count) {
					testlabels[current_data] = atoi(current_token.c_str());
					current_data++;
				}
			}

			current_token_char = strtok (NULL, " {},\t;");
		}

		delete [] line_char_star;
	}
	return;
}

#endif //_DATAINTERFACE_H