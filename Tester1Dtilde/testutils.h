#ifndef TESTUTILS_H
#define TESTUTILS_H
// ==========
// This file provides some functions and classes
// that are commonly used in test cases.
//
// Class Config : helps to read parameters from file
// Class Stat   : helps to make statistics 
//				  (getcputime, ram usage, compute mi/ma/avg, etc)
// ==========
#include <exception>
#include <cassert>
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <ostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <direct.h>
#include <ctime>
#include <tuple>
#include <list>
#include <numeric>
#include "windows.h"
#include "process.h"
#include "psapi.h"	//for Windows' getProcess...
using namespace std;
// Some constants
#define EPSILON 0.0001

// Class for reading config parameters
struct Config
{	// Declare program level parameters here. Then add them to constructor
	// Only add parameters that will be changed frequently to readConfig() and printConfig()
	int VERSION;			   // 0 : depth first
							   // 1 : best first
							   // 2 : breadth first
							   // 3 : depth first + memo
							   // 4 : best first + memo
							   // 5 : breadth first + memo
							   // 6 : depth first + solution memo
							   // 10: sipsi
							   // 20: only generate data
							   // 30: compute stat by reading results from file.
	int CLEAN_STRA;			   // Memo Cleaning Strategy. Use
	int DOM_STRA;			   // Dominance Strategy. Set to 12 to enable predictive memo, default 0.
	int SIZE_START;
	int SIZE_STEP;
	int INS_START;			   // The number of the starting instance		 
	int NB_COL_INPUT_FILE;     // nb of col in INPUT_FILE
	bool SOLVE_ONE;			   // Only solve one instance ?
	string SOLVER_PATH;		   // The solver program to call for solving each instance
	string INS_FILE;		   // Default input instance path
	string SOLVER_SOL_FILE;	   // Result file created by solver
	string INPUT_FILE;	       // Tester detail output used as input for computing stat.txt
	string OUTPUT_FILE;	       // Tester detail output : collection of all results, for instance
	string STAT_FILE;	       // Tester statistic output
	string NOTIF;

	// Parameters that are not read from file
	int LB_STRA;
	int MAX_SIZE_PB;		   // default 1000
	int TIME_LIM;			   // Time limitation in seconds. 18000 by default (5h)
	int N_INS_PER_SETTING;		   // Number of instances generated for each size

	Config()
		: VERSION(0)
		, CLEAN_STRA(0)
		, DOM_STRA(0)
		, SIZE_START(70)
		, SIZE_STEP(10)
		, INS_START(1)
		, SOLVE_ONE(false)
		, SOLVER_PATH("")
		, INS_FILE("ins.txt")
		, SOLVER_SOL_FILE("sol.txt")
		, INPUT_FILE("res_in.txt")
		, NB_COL_INPUT_FILE(10)
		, OUTPUT_FILE("res.txt")
		, STAT_FILE("stat.txt")
		, NOTIF("")
		, LB_STRA(0)
		, MAX_SIZE_PB(500)
		, TIME_LIM(18000)
		, N_INS_PER_SETTING(10)
	{}

	void readConfig(string fname) {
		ifstream ifs(fname);
		if (!ifs) {
			cout << "Config file " << fname << " not found." << endl;
			return;
		}
		else {
			cout << "Using config file " << fname << endl;
		}
		// Parse file
		string line;
		while (!ifs.eof()) {
			getline(ifs, line);
			if (line.empty() || line[0] == '#')continue;
			int pequal = int(line.find("="));
			if (!isalpha(line[0]) || pequal == string::npos) {
				cout << "Warning : illegal line (" << line << ") skipped!\n";
				continue;
			}
			string name = line.substr(0, pequal);
			stringstream val = stringstream(line.substr(pequal + 1));

			// Add new properties here
			if (name == "VERSION")			val >> VERSION;
			else if (name == "CLEAN_STRA")	val >> CLEAN_STRA;
			else if (name == "DOM_STRA")	val >> DOM_STRA;
			else if (name == "SIZE_START")	val >> SIZE_START;
			else if (name == "SIZE_STEP")	val >> SIZE_STEP;
			else if (name == "INS_START")	val >> INS_START;
			else if (name == "SOLVE_ONE")	val >> SOLVE_ONE;
			else if (name == "SOLVER_PATH")	val >> SOLVER_PATH;
			else if (name == "INS_FILE")	val >> INS_FILE;
			else if (name == "SOLVER_SOL_FILE")	val >> SOLVER_SOL_FILE;
			else if (name == "OUTPUT_FILE")	val >> OUTPUT_FILE;
			else if (name == "INPUT_FILE")	val >> INPUT_FILE;
			else if (name == "NB_COL_INPUT_FILE")	val >> NB_COL_INPUT_FILE;
			else if (name == "STAT_FILE")	val >> STAT_FILE;
			else if (name == "NOTIF")		val >> NOTIF;
		}
	}

	void printConfig()
	{
		cout << "\nParameters =========="
			<< "\nVERSION         =" << VERSION
			<< "\nCLEAN_STRA      =" << CLEAN_STRA
			<< "\nDOM_STRA        =" << DOM_STRA
			<< "\nSIZE_START      =" << SIZE_START
			<< "\nSIZE_STEP       =" << SIZE_STEP
			<< "\nINS_START       =" << INS_START
			<< "\nSOLVE_ONE       =" << SOLVE_ONE
			<< "\nSOLVER_PATH     =" << SOLVER_PATH
			<< "\nINS_FILE	      =" << INS_FILE
			<< "\nSOLVER_SOL_FILE =" << SOLVER_SOL_FILE
			<< "\nINPUT_FILE      =" << INPUT_FILE
			<< "\nOUTPUT_FILE     =" << OUTPUT_FILE
			<< "\nSTAT_FILE       =" << STAT_FILE
			<< "\nNOTIF		      =" << NOTIF
			<< endl;
	}

	// Modify this method to send a mail on the completion of tests.
	void notify(string msg = "") {
		if (msg.empty() && !NOTIF.empty())
			msg = NOTIF;
		if (!msg.empty())
		{
			//cerr << "Notification disabled by lei..." << endl;
			system(("echo Subject: " + msg + " > mail.txt").c_str());
			system("curl.exe smtp://smtp.gmail.com:587 -v --mail-from \"di.polytech@gmail.com\" --mail-rcpt \"shang.france@gmail.com\" --ssl -u di.polytech@gmail.com:Le0@polytech -T mail.txt -k --anyauth");
			system("del mail.txt");
		}
	}
};

// Class representing a result
struct Result {
	int sol, lb, ret;
	double tCpu, tWall;
	unsigned long long cutActive;
	unsigned long long cutDone;
	unsigned long long nbKpermAdded;
	unsigned long long nbPolluted;
	unsigned long long nbNodes;
	unsigned long long nbArcs;
	unsigned long long ram;
};

// Class for making statistics
struct Stat {
	map<int, vector<Result>> resMap;

	// Methodes ==========
	void addResult(Result& r, int insSize) {
		resMap[insSize].push_back(r);
	}

	template<typename TP, typename T = decay<TP>::type>
	tuple<T, double, T> getMinAvgMax(int insSize, TP Result::* fieldRef) {
#undef min
#undef max
		T mi = numeric_limits<T>::max();
		T ma = numeric_limits<T>::min();
		T sum = 0;
		for (auto & r : resMap[insSize])
		{
			T v = r.*fieldRef;
			if (v < mi)mi = v;
			if (v > ma)ma = v;
			sum += v;
		}
		return make_tuple(mi, sum / double(resMap[insSize].size()), ma);
	}

	// Measure REAL CPU Time 
	//  Windows
#ifdef _WIN32
#include <Windows.h>
	static long long getRam() {
		HANDLE currProc = GetCurrentProcess();
		PROCESS_MEMORY_COUNTERS pmc;
		if (K32GetProcessMemoryInfo(currProc, &pmc, sizeof(pmc)))
			return pmc.WorkingSetSize;
		return -1;
	}

	static double getCpuTime() {
		FILETIME a, b, c, d;
		if (GetProcessTimes(GetCurrentProcess(), &a, &b, &c, &d) != 0) {
			//  Returns total user time.
			//  Can be tweaked to include kernel times as well.
			double kernelT = (double)(c.dwLowDateTime |
				((unsigned long long)c.dwHighDateTime << 32)) * 0.0000001;
			double userT = (double)(d.dwLowDateTime |
				((unsigned long long)d.dwHighDateTime << 32)) * 0.0000001;
			//cout << "Kernel = " << kernelT << ";\t User = " << userT<<endl;
			return kernelT + userT;
		}
		else {
			//  Handle error
			cerr << "GetProcessTimes returns 0. Current WallTime = " << getWallTime() << endl;;
			return 0;
		}
	}

	static double getWallTime() {
		LARGE_INTEGER time, freq;
		if (!QueryPerformanceFrequency(&freq)) {
			cerr << "getWallTime->QueryPerformanceFrequency return false." << endl;
			return 0;
		}
		if (!QueryPerformanceCounter(&time)) {
			cerr << "getWallTime->QueryPerformanceCounter return false." << endl;
			return 0;
		}
		return (double)time.QuadPart / freq.QuadPart;
	}

	//  Posix/Linux
#else
#include <time.h>
#include <sys/time.h>
	static double getWallTime() {
		struct timeval time;
		if (gettimeofday(&time, NULL)) {
			return 0;
		}
		return (double)time.tv_sec + (double)time.tv_usec * .000001;
	}
	double getCpuTime() {
		return (double)clock() / CLOCKS_PER_SEC;
	}
#endif
};
#endif
