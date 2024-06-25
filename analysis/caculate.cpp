#include <cstdio>
#include <stdint.h>
#include <map>
#include <vector>
#include <utility>
#include <algorithm>
#include <unistd.h>
#include <string>
#include <string.h>
#include "trace-format.h"
#include "utils.hpp"
#include "sim-setting.h"

using namespace std;

string trace_file="/content/myTest/simulation/mix/mix_test.tr";
string file_name="throughput.txt";
uint16_t node=2;

void parse_opt(int argc, char* argv[]){
	for (int opt=0; (opt = getopt(argc, argv, "tr:fn:n:")) != -1;) {
		switch (opt) {
			case 'tr':
				trace_file = optarg;
				break;
			case 'fn':
				file_name = optarg;
				break;
			case 'n':
				node = atoi(optarg);
				break;
			default: /* '?' */
				fprintf(stderr, 
						"usage: %s [-h] [-tr TRACE_FILE] [-fn FILENAME] [-n NODE]\n"
						"\n"
						"optional arguments:\n"
						"  -h, --help     show this help message and exit\n"
						"  -tr TRACE_FILE      Specify the trace file. Usually like\n"
                        "  -fn FILENAME      Specify the output file. Usually like\n"
                        "  -n NODE      Specify the target node. Usually like\n",
						argv[0]);
				exit(EXIT_FAILURE);
		}
	}
}


void print_throughput(uint16_t target){
	FILE *file=fopen(file_name.c_str(),"w");
    FILE *trace=fopen(trace_file.c_str(),"r");
    SimSetting sim_setting;
	sim_setting.Deserialize(trace);
	#if 0
	// print sim_setting
	for (auto i : sim_setting.port_speed)
		for (auto j : i.second)
			printf("%u,%u:%lu\n", i.first, j.first, j.second);
	#endif
    ns3::TraceFormat tr;
	map<uint64_t,float> maps;
	while (tr.Deserialize(trace) > 0){
    if(tr.l3Prot==0x11&&tr.node==node){
		uint64_t time=tr.time/1E4;
		auto itr=maps.find(time);
		if(itr!=maps.end())
		{	
			itr->second=itr->second+tr.size;
		}
		else{
			pair<uint64_t,float> pairToInsert(time,tr.size);
			maps.insert(pairToInsert);
		}
	}
	}
	for(auto itr=maps.begin();itr!=maps.end();++itr){
		float bandwidth=8*itr->second/1E4;
		float time=itr->first/1E2;
		fprintf(file,"%f %f\n",time,bandwidth);
	}
    fclose(file);
    fclose(trace);
}

int main(int argc, char* argv[]){
	parse_opt(argc, argv);
    print_throughput(node);
	return 0;
}
