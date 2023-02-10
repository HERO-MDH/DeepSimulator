/*
Copyright 2022 Mohammad Riazati

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef _FIXED_POINT_ANALYSIS
#define _FIXED_POINT_ANALYSIS

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include "utils.h"
#include <algorithm>
#include <climits>

using namespace std;

class DataElement {
	public:
		int LayerNumber;
		string ElementGroup;
		int Id;
		float Data;
		int index1, index2, index3;

		string ToString() {
			stringstream result;
			result << LayerNumber << ",";
			result << ElementGroup << ",";
			result << Id << ",";
			result << Data << ",";
			result << index1 << ",";
			result << index2 << ",";
			result << index3;// << ",";

			return result.str();
		};
};

class DataMinMax {
	public:
		int LayerNumber;
		string ElementGroup;
		float MinData = (float)INT_MAX;
		float MaxData = (float)INT_MIN;
};

bool DataElementLessThan(DataElement* a, DataElement* b) {
	string a_group = StrToLower(a->ElementGroup);
	string b_group = StrToLower(b->ElementGroup);
	return 
				(a->LayerNumber < b->LayerNumber) || 
				(a->LayerNumber == b->LayerNumber && a_group < b_group) ||
				(a->LayerNumber == b->LayerNumber && a_group == b_group && a->index1 < b->index1) ||
				(a->LayerNumber == b->LayerNumber && a_group == b_group && a->index1 == b->index1 && a->index2 < b->index2) ||
				(a->LayerNumber == b->LayerNumber && a_group == b_group && a->index1 == b->index1 && a->index2 == b->index2 && a->index3 < b->index3) ||
				(a->LayerNumber == b->LayerNumber && a_group == b_group && a->index1 == b->index1 && a->index2 == b->index2 && a->index3 == b->index3 && a->Id < b->Id)
		; 
}

bool DataMinMaxLessThan(DataMinMax* a, DataMinMax* b) {
	string a_ElementGroup = a->ElementGroup;
	string b_ElementGroup = b->ElementGroup;
	transform(a_ElementGroup.begin(), a_ElementGroup.end(), a_ElementGroup.begin(), ::tolower);
	transform(b_ElementGroup.begin(), b_ElementGroup.end(), b_ElementGroup.begin(), ::tolower);

	return 
				(a_ElementGroup < b_ElementGroup) || 
				(a_ElementGroup == b_ElementGroup && a->LayerNumber < b->LayerNumber)
		; 
}

vector<DataElement*> AllDataElements;
vector<DataMinMax*> DataMinMaxElements;
map<string,int> LastId;

void StoreData(int pLayerNumber, string pElementGroup, float pData, bool pStoreAll, int index1 = -1, int index2 = -1, int index3 = -1) {
	//return;
	static bool logged = false;
	bool pStoreAllEnabled = true;
	#ifndef _DEBUG
	pStoreAllEnabled = false;
	if (!logged) AddToLog(GetLogFileName(), "StoreAll not supported in release mode. Will be ignored.");
	logged = true;
	#else	
	#endif

	if (pStoreAll && pStoreAllEnabled) {
		if (false
			|| pElementGroup == "biases"
			|| pElementGroup == "weights"
			|| pElementGroup == "temp_element"
			)
			return;

		string temp_string = (string)"l" + to_string(pLayerNumber) + "_" + pElementGroup; //l1_LayerOutput
		if(LastId.count(temp_string) == 0) LastId[temp_string] = 0;

		DataElement *temp_DataElement = new DataElement;
		temp_DataElement->Id = LastId[temp_string]++;
		temp_DataElement->ElementGroup = pElementGroup;
		temp_DataElement->LayerNumber = pLayerNumber;
		temp_DataElement->Data = pData;
		temp_DataElement->index1 = index1;
		temp_DataElement->index2 = index2;
		temp_DataElement->index3 = index3;

		try	{
			AllDataElements.push_back(temp_DataElement); //Not working well in Release Mode
		}
		catch (...) { //Project Properties -> C/C++ -> Code Generation -> Modify the Enable C++ Exceptions to "Yes With SEH Exceptions".
			cout << "ERR: " << temp_DataElement->ToString() << endl;
			return;
		}
	}

	bool exists = false;
	for(size_t i = 0; i < DataMinMaxElements.size(); i++) {
		if(DataMinMaxElements[i]->LayerNumber == pLayerNumber && DataMinMaxElements[i]->ElementGroup == pElementGroup) {
			exists = true;
			if (DataMinMaxElements[i]->MaxData < pData) DataMinMaxElements[i]->MaxData = pData;
			if (DataMinMaxElements[i]->MinData > pData) DataMinMaxElements[i]->MinData = pData;
			break;
		}
	}

	if(!exists)	{
		DataMinMax *temp_DataMinMax = new DataMinMax;
		DataMinMaxElements.push_back(temp_DataMinMax);
		temp_DataMinMax->ElementGroup = pElementGroup;
		temp_DataMinMax->LayerNumber = pLayerNumber;
		temp_DataMinMax->MaxData = pData;
		temp_DataMinMax->MinData = pData;
	}
	
	return;
}

void ExportData(string pFileName, bool pMinMax = false) {
	//if (arguments["export-data"] != "ACTIVE") return;

	//errno_t err;
	//FILE * pFile;

	bool pStoreAllEnabled = true;
	#ifndef _DEBUG
	pStoreAllEnabled = false;
	//In this case, AllDataElements is empty
	#else	
	#endif

	if(!pMinMax) {
		if (pStoreAllEnabled) {
			cout << "Exporting Data Elements ..." << endl;

			fstream pFile; pFile.open(pFileName, ios::app);

			std::sort(AllDataElements.begin(), AllDataElements.end(), DataElementLessThan);

			pFile << "LayerNumber,ElementGroup,Id,Data,index1,index2,index3" << endl;

			cout << endl << "Row number, layer, data element: " << endl;
			for (size_t i = 0; i < AllDataElements.size(); i++) {
				if (i == 0 || AllDataElements[i - 1]->LayerNumber != AllDataElements[i]->LayerNumber || AllDataElements[i - 1]->ElementGroup != AllDataElements[i]->ElementGroup)
					cout << i << "," << AllDataElements[i]->LayerNumber << "," << AllDataElements[i]->ElementGroup << endl;

				pFile << AllDataElements[i]->LayerNumber << "," << AllDataElements[i]->ElementGroup << "," << AllDataElements[i]->Id << "," << AllDataElements[i]->Data << "," << AllDataElements[i]->index1 << "," << AllDataElements[i]->index2 << "," << AllDataElements[i]->index3 << endl;
			}

			pFile.close();
			cout << "Exporting Data Done!" << endl << endl;
		}
	}
	else {
		cout << endl << "MinMax ..." << endl;

		fstream pFile; pFile.open(pFileName, ios::app);

		std::sort(DataMinMaxElements.begin(), DataMinMaxElements.end(), DataMinMaxLessThan);

		pFile << "LayerNumber, ElementGroup, MinData, MaxData" << endl;
		cout  << "LayerNumber, ElementGroup, MinData, MaxData" << endl;

		for (size_t i = 0; i < DataMinMaxElements.size(); i++) {
			pFile << DataMinMaxElements[i]->LayerNumber << "," << DataMinMaxElements[i]->ElementGroup << "," << DataMinMaxElements[i]->MinData << "," << DataMinMaxElements[i]->MaxData << endl;
			cout  << DataMinMaxElements[i]->LayerNumber << "," << DataMinMaxElements[i]->ElementGroup << "," << DataMinMaxElements[i]->MinData << "," << DataMinMaxElements[i]->MaxData << endl;
		}

		pFile << "Exporting MinMax Done!" << endl << endl;
		cout << "Exporting MinMax Done!" << endl << endl;

		pFile.close();
	}

	return;
}

#endif //_FIXED_POINT_ANALYSIS