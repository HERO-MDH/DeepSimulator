/*
Copyright 2022 Mohammad Riazati

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef _UTILS_H
#define _UTILS_H

#ifndef __linux__
#include "conio.h"
#endif
#include "stdlib.h"
#include "stdio.h"
#include "time.h"
#include <typeinfo>
#include <string>
#include <filesystem> //since c++ 17 //Project Properties - C / C++ - Language - C++ Language Standard - ISO C++17 Standard(/ std:c++17)
#include <ctime>
#include <fstream>
#include <sstream>
#include <cmath>

#ifdef __linux__
#include <sys/time.h> //for gettimeofday
#include <termios.h> //for _getche
#include <unistd.h> //for _getche
#include <stdio.h> //for _getche
int _getche(void) ;
#include <boost/core/demangle.hpp>
#define TypeName(i) (boost::core::demangle(typeid(i).name()))
#else
#define TypeName(i) (typeid(i).name())
#endif


#include <chrono>
using namespace std;
using namespace std::filesystem; //See the note above: <filesystem>

#define Q(x) #x
#define TOSTRING(x) Q(x)

#define LOGIT(x) AddToLogSimple(x)
#define ASSERT(a) {if (!(a)) LOGIT("Assert @ " + string(__FILE__) + ":" + to_string(__LINE__));}
#define ASSERTA ASSERT(false)
#define ASSERTT(a, t) {if (!(a)) LOGIT(t);}

map<string, string> arguments;

//https://www.geeksforgeeks.org/measure-execution-time-with-high-precision-in-c-c/ 
class Timer {
  struct point_type {
    time_t value_time_t;
    clock_t value_time_c;
    timespec value_time_ts;
    std::chrono::time_point<std::chrono::high_resolution_clock> value_time_ch; //decltype(std::chrono::high_resolution_clock::now()) //Note: Union does not support this type
    #ifdef __linux__
    timeval value_time_tv;
    #endif
  };

  point_type start_time, stop_time;
public:

  //CLOCK: Not working in multithreaded programs. 
  //Alternative for CLOCK if CPU clocks are needed: https://linux.die.net/man/3/clock_gettime
  enum TimerMethod {TIME, CLOCK, TOD, CGT, CHR};

  enum TimerUnit {S, MS, US};
  TimerMethod method = TIME;

  Timer() = default;
  Timer(TimerMethod p_method):method(p_method){Start();};
  void Start(){ //Resets the timer
    if (method == TIME) start_time.value_time_t = time(nullptr);
    else if (method == CLOCK) start_time.value_time_c = clock();
    else if (method == CHR) start_time.value_time_ch = std::chrono::high_resolution_clock::now();
#ifdef __linux__
    else if (method == TOD) gettimeofday(&(start_time.value_time_tv), NULL);
    else if (method == CGT) clock_gettime(CLOCK_REALTIME, &(start_time.value_time_ts)); //CLOCK_REALTIME, CLOCK_PROCESS_CPUTIME_ID, CLOCK_MONOTONIC ? etc.
#endif
  }

  double Stop(TimerUnit unit = MS){
    if (method == TIME) stop_time.value_time_t = time(nullptr);
    else if (method == CLOCK) stop_time.value_time_c = clock();
    else if (method == CHR) stop_time.value_time_ch = std::chrono::high_resolution_clock::now();
#ifdef __linux__
    else if (method == TOD) gettimeofday(&(stop_time.value_time_tv), NULL);
    else if (method == CGT) clock_gettime(CLOCK_REALTIME, &(stop_time.value_time_ts)); //CLOCK_REALTIME, CLOCK_PROCESS_CPUTIME_ID, CLOCK_MONOTONIC ? etc.
#endif

    if (unit == MS) return duration_ms();
    if (unit == US) return duration_us();
    else if (unit == S) return duration_s();

    return 0;
  }

  double duration_us(){
    return duration_s() * 1e6;
  }

  double duration_ms(){
    return duration_s() * 1e3;
  }

  double duration_s(){
    double result = 0;
    if (method == TIME) result = double(stop_time.value_time_t - start_time.value_time_t);
    else if (method == CLOCK) result = double(stop_time.value_time_c - start_time.value_time_c) / double(CLOCKS_PER_SEC);
    else if (method == CHR) result = std::chrono::duration_cast<std::chrono::nanoseconds>(stop_time.value_time_ch - start_time.value_time_ch).count() / 1.0e9;
#ifdef __linux__
    else if (method == TOD) result = (stop_time.value_time_tv.tv_sec - start_time.value_time_tv.tv_sec) + (stop_time.value_time_tv.tv_usec - start_time.value_time_tv.tv_usec) / 1.0e6;
    else if (method == CGT) result = (stop_time.value_time_ts.tv_sec - start_time.value_time_ts.tv_sec) + (stop_time.value_time_ts.tv_nsec - start_time.value_time_ts.tv_nsec) / 1.0e9;
#endif

    return result;
  }
};

string Trim(string param) {
  std::string s = param;
  s.erase(s.find_last_not_of(" \n\r\t")+1);
  return s;
}

int countSubstring(const std::string& str, const std::string& sub) {
    if (sub.length() == 0) return 0;
    int count = 0;
    for (size_t offset = str.find(sub); offset != std::string::npos;
     offset = str.find(sub, offset + sub.length()))
    {
        ++count;
    }
    return count;
}

string StringSubstituteAll(string source, string find, string replace) {
	size_t index = 0;
	string result = source;
	while (true) {
		/* Locate the substring to replace. */
		index = result.find(find, index); //second parameter is offset
		if (index == string::npos) break;

		/* Make the replacement. */
		result.replace(index, find.length(), replace);

		/* Advance index forward so the next iteration doesn't pick it up as well. */
		index += replace.length();
	}		
	return result;
}

string CleanTypeName(string source) {
	string result = source;
	result = StringSubstituteAll(result, "<", "-");
	result = StringSubstituteAll(result, ">", "");
	result = StringSubstituteAll(result, ",", "-");
	result = StringSubstituteAll(result, "struct ", "");

	return result;
}

string GetCurrentTimeAsString() {
  //_CRT_SECURE_NO_WARNINGS
  //https://stackoverflow.com/questions/22450423/how-to-use-crt-secure-no-warnings
  //Project settings (All configurations, x64): Configuration Properties>>C/C++>>Preporocessor>>Preprocessor Definitions>> _CRT_SECURE_NO_WARNINGS

  time_t rawtime;
  struct tm * timeinfo;
  char buffer[80];

  time (&rawtime);
  timeinfo = localtime(&rawtime);

  strftime(buffer,sizeof(buffer),"%Y-%m-%d %H:%M:%S",timeinfo);
  std::string str(buffer);

  return str;
}

string CurrentDirectory() {
	string result = std::filesystem::current_path().string();

	#ifdef __linux__
	result += "/";
	#else
	result +="\\";
	#endif

  return result;
}

string GetLogFileName() {
  static string log_file_name;

  if (log_file_name != "") return log_file_name;

	char filename[100];
  time_t t = time(0);	
  struct tm * now = localtime( & t ); 
	//sprintf(filename, "zLog-%d%02d%02d-%02d%02d%02d-%s.txt", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec, CleanTypeName(typeid(DataType).name()).c_str());
	sprintf(filename, "zLog-%d%02d%02d-%02d%02d%02d.txt", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);

  string result(filename);
  log_file_name = result;

  return result;
}

string GetLogAllElementsFileName() {
	string result = GetLogFileName();
	result = StringSubstituteAll(result, ".txt", "-AllDataElements.txt");

  return result;
}

void AddToLogSimple(string text) {
  string filename = GetLogFileName();
	ofstream pFile; pFile.open(filename, ios::app);

  cout << text << endl;
  pFile << text << endl; 
  pFile.close();
}

void AddToLog(string filename, string text1, string text2 = "", bool newline = true) {
  string text = StringSubstituteAll(text1, "%s", text2);
	//FILE * pFile = fopen (filename.c_str(), "a+");
	ofstream pFile; pFile.open(filename, ios::app);

  cout << text; if (newline) cout << endl; else cout << flush;
  //fprintf (pFile, (string(text) + (newline?"\n":"")).c_str()); 
  pFile << text + (newline?"\n":""); 
  //fclose (pFile);
  pFile.close();
}

string FloatToString(double param, int precicion = 2) {
  int temp = static_cast<int>(param);
  int digits = static_cast<int>(log10(param + 1));
  stringstream s;
  s << fixed << setprecision(precicion) << param; //1 for decimal point
  s.unsetf(ios::fixed | ios::scientific);
  return s.str();
}

void PrintProgress_del(int i, int truePredict, int run_range, int thread_count, string filename) {
  static int last_i = 0;
  int report_interval = max(1, run_range / 20);
  int next_i = (i + thread_count) >= run_range ? run_range : i + thread_count;

  if (thread_count == 1) {
	  if((next_i)%report_interval == 0)
		  AddToLog(filename, to_string(truePredict) + " / " + to_string(next_i) + "(" + FloatToString(truePredict * 1.0 / (next_i) * 100) + ") (%s) ", GetCurrentTimeAsString());
	  else if ((next_i)%(max(report_interval/20, 1)) == 0)
		  AddToLog(filename, ".", "", false);
  }
  else {
    if ((last_i) / report_interval < (next_i) / report_interval)
		  AddToLog(filename, to_string(truePredict) + " / " + to_string(next_i) + "(" + FloatToString(truePredict * 1.0 / (next_i) * 100) + ") (%s) ", GetCurrentTimeAsString());
  }

  last_i = next_i;
}

void PrintProgress(int i, int truePredict, int run_range, int thread_count, string filename) {
  static int last_i = 0;
  int report_interval = max(1, run_range / 20);
  int report_interval_small = max(report_interval/20, 1);
  static int last_report_interval_i = 0;
 	
  static Timer t1(Timer::CHR);
  
  if (last_i == 0) {// The first time this function is called
    t1.Start();
  }

  //thread_count parameter can also be 1 when fault-simulation is running, eventhough the real thread_count is not 1
  if (thread_count == 1) {
    if (last_i / report_interval < i / report_interval) {
      double exec_time_per_image = t1.Stop(Timer::S) / (i - last_report_interval_i);
      string exec_time_per_image_str = " (" + FloatToString(exec_time_per_image, 4) + " sec per image)";
      if (last_i == 0) exec_time_per_image_str = ""; //t1 has just started. So, it's invalid
		  AddToLog(filename, to_string(truePredict) + " / " + to_string(i) + " (" + FloatToString(truePredict * 1.0 / (i) * 100) + ") (" + GetCurrentTimeAsString() + ")" + exec_time_per_image_str);
      t1.Start();
      last_report_interval_i = i;
    }
    else if (last_i / report_interval_small < i / report_interval_small)
		  AddToLog(filename, ".", "", false);
  }
  else {
    if (last_i / report_interval < i / report_interval) {
      double exec_time_per_image = t1.Stop(Timer::S) / (i - last_report_interval_i);
      string exec_time_per_image_str = " (" + FloatToString(exec_time_per_image, 4) + " sec per image)";
      if (last_i == 0) exec_time_per_image_str = ""; //t1 has just started. So, it's invalid
		  AddToLog(filename, to_string(truePredict) + " / " + to_string(i) + " (" + FloatToString(truePredict * 1.0 / (i) * 100) + ") (" + GetCurrentTimeAsString() + ")" + exec_time_per_image_str);
      t1.Start();
      last_report_interval_i = i;
    }
  }

  last_i = i;

}

int GetNumberOfCpuCores() {
  int result = std::thread::hardware_concurrency();
  AddToLog(GetLogFileName(), "Number of cpu cores: " + to_string(result));
  return result;
}

int GetThreadCount() {
  if (arguments["threads"] != "") {
    GetNumberOfCpuCores(); //Just for information
    return stoi(arguments["threads"]);
  }

	int core_count = GetNumberOfCpuCores();
	int thread_count =  core_count - (core_count / 12 + 1); //5; //12;
	#ifdef _DEBUG
	thread_count = 1;
	#endif

  return thread_count;
}

//threads, fault-simulation, export-data, mul-name, mul-layers-config, image-count
void ReadOptions(int argc, char* argv[]) {
  //Supported options:
  //-quantized
  //-fault-simulation
  //-fault-count
  //-export-data
  //-mul-name
  //-mul-layers-config
  //-threads
  //-image-count

	string parameter, value;
	int i = 1; //the first argument is the executable file name
	while (i < argc) {
		parameter = Trim(argv[i]);
		++i;
		parameter = parameter.substr(1);
		if (i == argc || argv[i][0] == '-') {
      value = "ACTIVE";
			arguments.insert(make_pair(parameter, value));
      AddToLog(GetLogFileName(), "Program argument: " + parameter + "(" + value + ")");
			continue;
		}

		value = Trim(argv[i]);
		++i;
		arguments.insert(make_pair(parameter, StringSubstituteAll(value, "#", "\n")));

    AddToLog(GetLogFileName(), "Program argument: " + parameter + "(" + value + ")");
	}

  if (arguments["mul-layers-config"] == "") arguments["mul-layers-config"] = "00000000000000000000000000000000000000000000000000000000000";
}

string FindInputFile(string file_name) {
  string result;
  try {
    result = CurrentDirectory() + file_name; 
    if (exists(result)) return result;

	  result = "E:\\MDH\\Tools\\DeepHLS\\Project\\GeneratedCPredict\\Project1\\" + file_name;
    if (exists(result)) return result;

	  result = "D:\\Projects\\DeepHLS\\Project1\\" + file_name;
    if (exists(result)) return result;

    result = "..\\" + file_name;
    if (exists(result)) return result;
  }
  catch (const std::exception&) {
    return "";
  }

  return "";
}

int GetImageCount(int default_count = 10000) {
  if (arguments["image-count"] == "") arguments["image-count"] = to_string(default_count);
  return stoi(arguments["image-count"]);
}

string FormatTime(int value, string output_format = "H:M:S", string input_unit = "S") {
  if (input_unit == "S" && output_format == "H:M:S")
  {
    int val = value;
    int h = val / 3600;
    int m = (val - h * 3600) / 60;
    int s = (val - h * 3600 - m * 60);

    string h_string, m_string, s_string;
    h_string = to_string(h); if (h_string.length() == 1) h_string = "0" + h_string;
    m_string = to_string(m); if (m_string.length() == 1) m_string = "0" + m_string;
    s_string = to_string(s); if (s_string.length() == 1) s_string = "0" + s_string;

    return h_string + ":" + m_string + ":" + s_string;
  }

  return "";
}

template<typename T>
void ExportTensor(string file_name, void* input_tensor, T an_element, int dimentions_count, int dim1_size, int dim2_size = 0, int dim3_size = 0) {
  //cout << TypeName(an_element);
  //return;
  ofstream output(file_name);
  if (dimentions_count == 1) {
    for (int i = 0; i < dim1_size; ++i) {
      T temp = ((T*)input_tensor)[i];
      output << i << "," << (long long int)temp << endl;
    }
  }

  output.close();
}


vector<string> ExplodeString(string source, string delimiter) {
  vector<string> result;

  if (source.length() == 0) return result;
  if (source.find(delimiter) == string::npos) {result.push_back(source); return result;}
  //now, at least two parts (one delimiter)
  
  string part;
  string current_source = source;

  while (current_source.find(delimiter) != string::npos) {
    part = current_source.substr(0, current_source.find(delimiter));
    result.push_back(part);

    current_source = current_source.substr(part.size() + delimiter.size());
  }

  result.push_back(current_source);
  return result;
}

string StrToLower(string param) {
  string result = param;
  transform(result.begin(), result.end(), result.begin(), ::tolower);

  return result;
}


#ifdef __linux__
int _getche(void) 
{ 
    struct termios oldattr, newattr; 
    int ch; 
    tcgetattr(STDIN_FILENO, &oldattr); 
    newattr = oldattr; 
    newattr.c_lflag &= ~(ICANON | ECHO); 
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr); 
    ch = getchar(); 
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr); 
    return ch; 
} 
#endif //__linux__

#endif // _UTILS_H