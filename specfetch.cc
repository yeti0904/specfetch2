#include <cstdio>
#include <cstdint>
#include <cmath>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <sys/sysinfo.h>
using std::stoi;
using std::string;
using std::vector;
using std::ifstream;
namespace FS = std::filesystem;

bool fexists(string fname) {
	ifstream fhnd(fname);
	return fhnd.good();
}

void strip(string& str) {
	while (str.size() > 0 && (str[0] == ' ' || str[0] == '\t')) {
		str.erase(0, 1);
	}
	while (str.size() > 0 && (str[str.size() - 1] == ' ' || str[str.size() - 1] == '\t')) {
		str.erase(str.size() - 1, 1);
	}
}

void gotoxy(uint16_t x, uint16_t y) {
	printf("\033[%d;%dH", y, x);
}

int main(void) {
	bool showCPU = false;
	bool showHDD = false;
	bool showGPU = false;
	bool showNET = false;
	bool showOS = false;

	// green
	string colour = "\033[1;32m";
	// reset colour escape
	string reset = "\033[0m";

	if (fexists("/proc/cpuinfo")) showCPU = true;

	if (showCPU) {
		// get CPU name
		string CPU_Name;
		ifstream fhnd("/proc/cpuinfo");
		string line;
		while (getline(fhnd, line)) {
			if (line.find("model name") != string::npos) {
				CPU_Name = line.substr(line.find(":") + 2);
				break;
			}
		}

		// get CPU cores
		fhnd.close();
		fhnd = ifstream("/proc/cpuinfo");
		uint8_t CPU_Cores = 0;
		while (getline(fhnd, line)) {
			if (line.find("cpu cores") != string::npos) {
				CPU_Cores = stoi(line.substr(line.find(":") + 2));
				break;
			}
		}

		// get CPU threads
		fhnd.close();
		fhnd = ifstream("/proc/cpuinfo");
		uint8_t CPU_Threads = 0;
		while (getline(fhnd, line)) {
			if (line.find("siblings") != string::npos) {
				CPU_Threads = stoi(line.substr(line.find(":") + 2));
				break;
			}
		}

		// get CPU vendor
		fhnd.close();
		fhnd = ifstream("/proc/cpuinfo");
		string CPU_Vendor;
		while (getline(fhnd, line)) {
			if (line.find("vendor_id") != string::npos) {
				CPU_Vendor = line.substr(line.find(":") + 2);
				break;
			}
		}

		// set colour based on CPU vendor
		if (CPU_Vendor == "GenuineIntel") colour = "\033[1;32m";
		else if (CPU_Vendor == "AuthenticAMD") colour = "\033[1;31m";

		// print CPU info
		printf("%sCPU: %s%s (%d cores, %d threads)\n", colour.c_str(), reset.c_str(), CPU_Name.c_str(), CPU_Cores, CPU_Threads);

	}

	// show RAM info
	struct sysinfo info;
	sysinfo(&info);

	// get RAM total
	size_t RAM_Total = info.totalram;
	//RAM_Total *= info.mem_unit;

	string unit;
	if (RAM_Total > 1024 * 1024 * 1024) {
		RAM_Total = ceil((float)RAM_Total / (1024 * 1024 * 1024));
		unit = "GB";
	} else if (RAM_Total > 1024 * 1024) {
		RAM_Total = ceil((float)RAM_Total / (1024 * 1024));
		unit = "MB";
	} else {
		unit = "KB";
	}

	printf("%sRAM: %s%lu %s\n", colour.c_str(), reset.c_str(), RAM_Total, unit.c_str());

	// get GPU model
	string GPU_Model;
	// if nvidia
	if (fexists("/proc/driver/nvidia/gpus")) {
		string GPU_Path = FS::directory_iterator("/proc/driver/nvidia/gpus")->path().string();
		// read the information file at GPU_Path and get the Model
		ifstream fhnd(GPU_Path + "/information");
		string line;
		while (getline(fhnd, line)) {
			if (line.find("Model") != string::npos) {
				GPU_Model = line.substr(line.find(":") + 2);
				strip(GPU_Model);
				break;
			}
		}
		showGPU = true;
	}
	// not sure how to get an AMD gpu model

	if (showGPU) {
		printf("%sGPU: %s%s\n", colour.c_str(), reset.c_str(), GPU_Model.c_str());
	}
}
