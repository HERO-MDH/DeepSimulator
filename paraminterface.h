/*
Copyright 2022 Mohammad Riazati

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef _PARAMINTERFACE_H
#define _PARAMINTERFACE_H

#ifndef __linux__
#pragma warning(disable:4996) // to suppress insecurity warning for strtok
#endif

#include <fstream>
#include <sstream>
#include <string>
#include <algorithm> 
#include <cctype>
#include <locale>
#include <iostream>

#include "utils.h"

using namespace std;

void* GetParam(string name);

//void ExportWeights4D(DataType **** weights, int x, int y, int z, int outz);
//void ExportWeights2D(DataType ** weights, int x, int outz);
//void ExportBiases(DataType * biases, int x);

void InitializeParam(string layer_types = "") {
	string param_file;
	ifstream datafile;

	param_file = FindInputFile("param.h");
	datafile.open(param_file);

	if (datafile.is_open()) cout << "Param file opened: " << param_file << endl; 
	else {
		cout << "Param file open failed" << endl;
		_getche(); exit(0);
	}

	int weights_x = 0, weights_y = 0, weights_z = 0, weights_outz = 0;
	int biases_x;

	int current_x = 0, current_y = 0, current_z = 0, current_out = 0;

	//DataType ****weights_4d = NULL;
	//DataType **weights_2d = NULL;
	//DataType *biases = NULL;
	
	int current_var_type = 0; //0: 4D weights, 1: 1D biases, 2: 2D weights
	//int current_layer;
	void *current_var = NULL;

	string line;
	char * current_token_char;
	string current_token;
	char * line_char_star;

	string temp_string;

	while (getline(datafile, line))	{
		if (line.find("weights_") != string::npos) {
			cout << "ReadParam: " << line.substr(0, line.find("=")) << endl;
			size_t a, b;
			a = line.find("weights_");// + ((string)"weights_").length();
			b = line.find("[");
			temp_string = line.substr(a, b - a);
			//current_layer = atoi(temp_string.c_str());
			current_var = GetParam(temp_string);

			line = line.substr(b);
			if (countSubstring(line, "[") == 4) { //4D weights
				current_var_type = 0; //4D weights
				temp_string = line;
				line_char_star = new char[temp_string.length() + 1];
				current_token_char = strtok((char*)temp_string.c_str()," [];");
				current_token = current_token_char;
				weights_x = atoi(current_token.c_str());

				current_token_char = strtok(NULL," [];");
				current_token = current_token_char;
				weights_y = atoi(current_token.c_str());

				current_token_char = strtok(NULL," [];");
				current_token = current_token_char;
				weights_z = atoi(current_token.c_str());

				current_token_char = strtok(NULL," [];");
				current_token = current_token_char;
				weights_outz = atoi(current_token.c_str());
				
				delete [] line_char_star;

				current_x = current_y = current_z = current_out = 0;

				a = line.find("=");
				line = line.substr(a + 1);
			}
			else if (countSubstring(line, "[") == 2) { //2D weights
				current_var_type = 2; //2D weights
				temp_string = line;
				line_char_star = new char[temp_string.length() + 1];
				current_token_char = strtok((char*)temp_string.c_str()," [];");
				current_token = current_token_char;
				weights_x = atoi(current_token.c_str());

				current_token_char = strtok(NULL," [];");
				current_token = current_token_char;
				weights_outz = atoi(current_token.c_str());
				
				delete [] line_char_star;

				current_x = current_out = 0;

				a = line.find("=");
				line = line.substr(a + 1);
			}
			else current_var_type = -1;
		}
		else if (line.find("biases_") != string::npos) {
			cout << "ReadParam: " << line.substr(0, line.find("=")) << endl;
			size_t a, b;
			a = line.find("biases_");// + ((string)"weights_").length();
			b = line.find("[");
			temp_string = line.substr(a, b - a);
			//current_layer = atoi(temp_string.c_str());
			current_var = GetParam(temp_string);

			line = line.substr(b);
			if (countSubstring(line, "[") == 1) {
				current_var_type = 1; //1D biases
				temp_string = line;
				line_char_star = new char[temp_string.length() + 1];
				current_token_char = strtok((char*)temp_string.c_str()," [];");
				current_token = current_token_char;
				biases_x = atoi(current_token.c_str());

				delete [] line_char_star;

				current_x = 0;

				a = line.find("=");
				line = line.substr(a + 1);
			}
			else current_var_type = -1;
		}

		line_char_star = new char[line.length() + 1];
		current_token_char = strtok((char*)line.c_str()," {},\t;");
		while(current_token_char)	{
			current_token = current_token_char;

			if(current_var_type == 0) { //0: 4D weights, 1: 1D biases, 2: 2D weights
				//weights_4d[current_x][current_y][current_z][current_out] = stof(current_token);
				*((DataType_weights*)current_var+(current_x * weights_y * weights_z * weights_outz + current_y * weights_z * weights_outz + current_z * weights_outz + current_out)) = (DataType_weights)atof(current_token.c_str());
				current_out++;
				if (current_out == weights_outz) {current_out = 0; current_z++;}
				if (current_z == weights_z) {current_z = 0; current_y++;}
				if (current_y == weights_y) {current_y = 0; current_x++;}
				if (current_x == weights_x) break;
			}
			else if(current_var_type == 1) { //0: 4D weights, 1: 1D biases, 2: 2D weights

				//biases[current_x] = stof(current_token);
				*((DataType_biases*)current_var+(current_x)) = (DataType_biases)atof(current_token.c_str());
				current_x++;
			}
			else if(current_var_type == 2) { //0: 4D weights, 1: 1D biases, 2: 2D weights
				//weights_2d[current_x][current_out] = stof(current_token);
				*((DataType_weights*)current_var+(current_x * weights_outz + current_out)) = (DataType_weights)atof(current_token.c_str());
				current_out++;
				if (current_out == weights_outz) {current_out = 0; current_x++;}
				if (current_x == weights_x) break;
			}

			current_token_char = strtok (NULL, " {},\t;");
		}

		delete [] line_char_star;
	}

#ifdef QUANTIZATION_FACTORS
	if (arguments["quantized"] == "ACTIVE") {
		ASSERT(layer_types != ""); //e.g., for LeNet: "CPCPFDDD"

		string q_parameters_file = FindInputFile("parameters.csv");
		ifstream q_parameters("parameters.csv");
		if (q_parameters.is_open()) cout << "Quantization parameters file opened: " << q_parameters_file << endl; 
		else {
			cout << "Quantization parameters file open failed" << endl;
			_getche(); exit(0);
		}

		string layer_type_name;
		string parameter_name;

		//Assumption: 
		//1- Ordered by layer, so the layer type name is skipped

		//li: last index
		int input_zero_points_li = 0, output_zero_points_li = 0, input_scale_factor_li = 0, output_scale_factor_li = 0, weight_scale_li = 0;

		while (getline(q_parameters, line))	{
			vector<string> line_parts = ExplodeString(line, ",");
			for (auto &part : line_parts) part = Trim(part);
			parameter_name = line_parts[0];
			layer_type_name = line_parts[1];

			if (parameter_name == "input_zero_points") {
				current_var = GetParam(parameter_name);
				*((DataType_IZP*)current_var+(input_zero_points_li++)) = (DataType_IZP)atof(line_parts[2].c_str());
			}
			else if (parameter_name == "output_zero_points" || parameter_name == "output_zero_points_classifier") {
				parameter_name = "output_zero_points"; //Note
				current_var = GetParam(parameter_name);
				*((DataType_OZP*)current_var+(output_zero_points_li++)) = (DataType_OZP)atof(line_parts[2].c_str());
			}
			else if (parameter_name == "input_scale_factor") {
				current_var = GetParam(parameter_name);
				*((DataType_ISF*)current_var+(input_scale_factor_li++)) = (DataType_ISF)atof(line_parts[2].c_str());
			}
			else if (parameter_name == "output_scale_factor") {
				current_var = GetParam(parameter_name);
				*((DataType_OSF*)current_var+(output_scale_factor_li++)) = (DataType_OSF)atof(line_parts[2].c_str());
			}
			else if (parameter_name == "weight_scale") {
				int current_layer_number = 0; //including all layer types
				int passed_c_and_d = 0;
				int layers_count = (int)layer_types.size();

				for (int i = 0; i < layers_count, passed_c_and_d <= weight_scale_li; i++) {
					current_layer_number = i + 1;
					if (layer_types[i] == 'C' || layer_types[i] == 'D') passed_c_and_d++;
				}
				current_var = GetParam(parameter_name + "_" + to_string(current_layer_number));
				weight_scale_li++;
				vector<string> values = ExplodeString(line_parts[2], " ");
				int values_size = (int)values.size();
				for (int i = 0; i < values_size; i++) {
					*((DataType_WSF*)current_var+(i)) = (DataType_WSF)atof(values[i].c_str());
				}
			}
		}
	}
#endif //QUANTIZATION_FACTORS
	return;
}

void* GetParam(string name) {
	//If null is returned, probably this variable is not defined as an array. Check if param-list.h matches param.h
	#ifdef __linux__
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wmisleading-indentation"
	#endif	

	void *result = NULL;

	if (name == "weights_1") result =(void*)weights_1_src; if (name == "biases_1") result =(void*)biases_1_src;
	if (name == "weights_2") result =(void*)weights_2_src; if (name == "biases_2") result =(void*)biases_2_src;
	if (name == "weights_3") result =(void*)weights_3_src; if (name == "biases_3") result =(void*)biases_3_src;
	if (name == "weights_4") result =(void*)weights_4_src; if (name == "biases_4") result =(void*)biases_4_src;
	if (name == "weights_5") result =(void*)weights_5_src; if (name == "biases_5") result =(void*)biases_5_src;
	if (name == "weights_6") result =(void*)weights_6_src; if (name == "biases_6") result =(void*)biases_6_src;
	if (name == "weights_7") result =(void*)weights_7_src; if (name == "biases_7") result =(void*)biases_7_src;
	if (name == "weights_8") result =(void*)weights_8_src; if (name == "biases_8") result =(void*)biases_8_src;
	if (name == "weights_9") result =(void*)weights_9_src; if (name == "biases_9") result =(void*)biases_9_src;
	if (name == "weights_10") result =(void*)weights_10_src; if (name == "biases_10") result =(void*)biases_10_src;
	if (name == "weights_11") result =(void*)weights_11_src; if (name == "biases_11") result =(void*)biases_11_src;
	if (name == "weights_12") result =(void*)weights_12_src; if (name == "biases_12") result =(void*)biases_12_src;
	if (name == "weights_13") result =(void*)weights_13_src; if (name == "biases_13") result =(void*)biases_13_src;
	if (name == "weights_14") result =(void*)weights_14_src; if (name == "biases_14") result =(void*)biases_14_src;
	if (name == "weights_15") result =(void*)weights_15_src; if (name == "biases_15") result =(void*)biases_15_src;
	if (name == "weights_16") result =(void*)weights_16_src; if (name == "biases_16") result =(void*)biases_16_src;
	if (name == "weights_17") result =(void*)weights_17_src; if (name == "biases_17") result =(void*)biases_17_src;
	if (name == "weights_18") result =(void*)weights_18_src; if (name == "biases_18") result =(void*)biases_18_src;
	if (name == "weights_19") result =(void*)weights_19_src; if (name == "biases_19") result =(void*)biases_19_src;
	if (name == "weights_20") result =(void*)weights_20_src; if (name == "biases_20") result =(void*)biases_20_src;
	if (name == "weights_21") result =(void*)weights_21_src; if (name == "biases_21") result =(void*)biases_21_src;
	if (name == "weights_22") result =(void*)weights_22_src; if (name == "biases_22") result =(void*)biases_22_src;
	if (name == "weights_23") result =(void*)weights_23_src; if (name == "biases_23") result =(void*)biases_23_src;
	if (name == "weights_24") result =(void*)weights_24_src; if (name == "biases_24") result =(void*)biases_24_src;
	if (name == "weights_25") result =(void*)weights_25_src; if (name == "biases_25") result =(void*)biases_25_src;
	if (name == "weights_26") result =(void*)weights_26_src; if (name == "biases_26") result =(void*)biases_26_src;
	if (name == "weights_27") result =(void*)weights_27_src; if (name == "biases_27") result =(void*)biases_27_src;
	if (name == "weights_28") result =(void*)weights_28_src; if (name == "biases_28") result =(void*)biases_28_src;
	if (name == "weights_29") result =(void*)weights_29_src; if (name == "biases_29") result =(void*)biases_29_src;
	if (name == "weights_30") result =(void*)weights_30_src; if (name == "biases_30") result =(void*)biases_30_src;
	if (name == "weights_31") result =(void*)weights_31_src; if (name == "biases_31") result =(void*)biases_31_src;
	if (name == "weights_32") result =(void*)weights_32_src; if (name == "biases_32") result =(void*)biases_32_src;
	if (name == "weights_33") result =(void*)weights_33_src; if (name == "biases_33") result =(void*)biases_33_src;
	if (name == "weights_34") result =(void*)weights_34_src; if (name == "biases_34") result =(void*)biases_34_src;
	if (name == "weights_35") result =(void*)weights_35_src; if (name == "biases_35") result =(void*)biases_35_src;
	if (name == "weights_36") result =(void*)weights_36_src; if (name == "biases_36") result =(void*)biases_36_src;
	if (name == "weights_37") result =(void*)weights_37_src; if (name == "biases_37") result =(void*)biases_37_src;
	if (name == "weights_38") result =(void*)weights_38_src; if (name == "biases_38") result =(void*)biases_38_src;
	if (name == "weights_39") result =(void*)weights_39_src; if (name == "biases_39") result =(void*)biases_39_src;
	if (name == "weights_40") result =(void*)weights_40_src; if (name == "biases_40") result =(void*)biases_40_src;
	if (name == "weights_41") result =(void*)weights_41_src; if (name == "biases_41") result =(void*)biases_41_src;
	if (name == "weights_42") result =(void*)weights_42_src; if (name == "biases_42") result =(void*)biases_42_src;
	if (name == "weights_43") result =(void*)weights_43_src; if (name == "biases_43") result =(void*)biases_43_src;
	if (name == "weights_44") result =(void*)weights_44_src; if (name == "biases_44") result =(void*)biases_44_src;
	if (name == "weights_45") result =(void*)weights_45_src; if (name == "biases_45") result =(void*)biases_45_src;
	if (name == "weights_46") result =(void*)weights_46_src; if (name == "biases_46") result =(void*)biases_46_src;
	if (name == "weights_47") result =(void*)weights_47_src; if (name == "biases_47") result =(void*)biases_47_src;
	if (name == "weights_48") result =(void*)weights_48_src; if (name == "biases_48") result =(void*)biases_48_src;
	if (name == "weights_49") result =(void*)weights_49_src; if (name == "biases_49") result =(void*)biases_49_src;
	if (name == "weights_50") result =(void*)weights_50_src; if (name == "biases_50") result =(void*)biases_50_src;

#ifdef QUANTIZATION_FACTORS
	if (name == "input_zero_points") result =(void*)input_zero_points_src;
	if (name == "output_zero_points") result =(void*)output_zero_points_src;
	if (name == "input_scale_factor") result =(void*)input_scale_factors_src; //note: int the parameters.csv, the "s" in the end was missing
	if (name == "output_scale_factor") result =(void*)output_scale_factors_src; //note: int the parameters.csv, the "s" in the end was missing

	if (name == "weight_scale_1") result =(void*)weight_scales_1_src;
	if (name == "weight_scale_2") result =(void*)weight_scales_2_src;
	if (name == "weight_scale_3") result =(void*)weight_scales_3_src;
	if (name == "weight_scale_4") result =(void*)weight_scales_4_src;
	if (name == "weight_scale_5") result =(void*)weight_scales_5_src;
	if (name == "weight_scale_6") result =(void*)weight_scales_6_src;
	if (name == "weight_scale_7") result =(void*)weight_scales_7_src;
	if (name == "weight_scale_8") result =(void*)weight_scales_8_src;
	if (name == "weight_scale_9") result =(void*)weight_scales_9_src;
	if (name == "weight_scale_10") result =(void*)weight_scales_10_src;
	if (name == "weight_scale_11") result =(void*)weight_scales_11_src;
	if (name == "weight_scale_12") result =(void*)weight_scales_12_src;
	if (name == "weight_scale_13") result =(void*)weight_scales_13_src;
	if (name == "weight_scale_14") result =(void*)weight_scales_14_src;
	if (name == "weight_scale_15") result =(void*)weight_scales_15_src;
	if (name == "weight_scale_16") result =(void*)weight_scales_16_src;
	if (name == "weight_scale_17") result =(void*)weight_scales_17_src;
	if (name == "weight_scale_18") result =(void*)weight_scales_18_src;
	if (name == "weight_scale_19") result =(void*)weight_scales_19_src;
	if (name == "weight_scale_20") result =(void*)weight_scales_20_src;
	if (name == "weight_scale_21") result =(void*)weight_scales_21_src;
	if (name == "weight_scale_22") result =(void*)weight_scales_22_src;
	if (name == "weight_scale_23") result =(void*)weight_scales_23_src;
	if (name == "weight_scale_24") result =(void*)weight_scales_24_src;
	if (name == "weight_scale_25") result =(void*)weight_scales_25_src;
	if (name == "weight_scale_26") result =(void*)weight_scales_26_src;
	if (name == "weight_scale_27") result =(void*)weight_scales_27_src;
	if (name == "weight_scale_28") result =(void*)weight_scales_28_src;
	if (name == "weight_scale_29") result =(void*)weight_scales_29_src;
	if (name == "weight_scale_30") result =(void*)weight_scales_30_src;
	if (name == "weight_scale_31") result =(void*)weight_scales_31_src;
	if (name == "weight_scale_32") result =(void*)weight_scales_32_src;
	if (name == "weight_scale_33") result =(void*)weight_scales_33_src;
	if (name == "weight_scale_34") result =(void*)weight_scales_34_src;
	if (name == "weight_scale_35") result =(void*)weight_scales_35_src;
	if (name == "weight_scale_36") result =(void*)weight_scales_36_src;
	if (name == "weight_scale_37") result =(void*)weight_scales_37_src;
	if (name == "weight_scale_38") result =(void*)weight_scales_38_src;
	if (name == "weight_scale_39") result =(void*)weight_scales_39_src;
	if (name == "weight_scale_40") result =(void*)weight_scales_40_src;
	if (name == "weight_scale_41") result =(void*)weight_scales_41_src;
	if (name == "weight_scale_42") result =(void*)weight_scales_42_src;
	if (name == "weight_scale_43") result =(void*)weight_scales_43_src;
	if (name == "weight_scale_44") result =(void*)weight_scales_44_src;
	if (name == "weight_scale_45") result =(void*)weight_scales_45_src;
	if (name == "weight_scale_46") result =(void*)weight_scales_46_src;
	if (name == "weight_scale_47") result =(void*)weight_scales_47_src;
	if (name == "weight_scale_48") result =(void*)weight_scales_48_src;
	if (name == "weight_scale_49") result =(void*)weight_scales_49_src;
	if (name == "weight_scale_50") result =(void*)weight_scales_50_src;
#endif //QUANTIZATION_FACTORS

	#ifdef __linux
	#pragma GCC diagnostic pop
	#endif
	if (!result) AddToLog(GetLogFileName(), "ERROR: GetParam returns NULL for (%s) ", name);
	return result;
}

/*
void ExportWeights4D(DataType **** weights, int x, int y, int z, int outz) {
	ofstream f_stream;
	string f_location = "weights-test.txt";
	f_stream.open(f_location, (ios::out));

	for(size_t i = 0; i < x; i++) {
		if (i > 0) f_stream << ",";
		f_stream << "\n{";
		for(size_t j = 0; j < y; j++) {
			if (j > 0) f_stream << ",";
			f_stream << "\n{";
			for(size_t k = 0; k < z; k++) {
				if (k > 0) f_stream << ",";
				f_stream << "\n{";
				for(size_t w = 0; w < outz; w++) {
					if (w > 0) f_stream << ",";
					f_stream << weights[i][j][k][w];
				}
				f_stream << "}";
			}
			f_stream << "}";
		}
		f_stream << "}";
	}

	f_stream.close();
}

void ExportWeights2D(DataType ** weights, int x, int outz) {
	ofstream f_stream;
	string f_location = "weights-test.txt";
	f_stream.open(f_location, (ios::out));

	for(size_t i = 0; i < x; i++)
	{
		if (i > 0) f_stream << ",";
		f_stream << "\n{";
		for(size_t j = 0; j < outz; j++)
		{
			if (j > 0) f_stream << ",";
			f_stream << weights[i][j];
		}
		f_stream << "}";
	}

	f_stream.close();
}

void ExportBiases(DataType * biases, int x) {
	ofstream f_stream;
	string f_location = "biases-test.txt";
	f_stream.open(f_location, (ios::out));

	for(size_t w = 0; w < x; w++)
	{
		if (w > 0) f_stream << ",";
		f_stream << biases[w];
	}

	f_stream.close();
}
*/

#endif //_PARAMINTERFACE_H