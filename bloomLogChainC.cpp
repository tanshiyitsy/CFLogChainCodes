#include "picosha2.h"
#include <fstream>
#include <chrono>
#include <iostream>
#include <vector>
#include <thread>
#include "bloom_filter.hpp"
#include <windows.h>
#include <openssl/sha.h>
using namespace std;
// 吞吐量

int have_finish = 0;
int acc=0;
vector<int> res;

class Log {
	public:
		int seq;
		string content;
		Log(int s, string c) {
			this->seq = s;
			this->content = c;
		}
};

class Pkg {
	public:
		vector<int> indexList;
		bloom_filter bf;
		string pkghash;
		Pkg(vector<int> l,bloom_filter bf,string s){
			this->indexList=l;
			this->bf=bf;
			this->pkghash=s;	
		}
};

void getTPS() {
	while (1) {
		cout << (have_finish-acc) << endl;
		res.push_back(have_finish-acc);
		acc=have_finish;
		cout<<"have_finish=="<<have_finish<<endl;
		this_thread::sleep_for(std::chrono::seconds(1));
	}
}

int main() {
//	DWORD startTime = GetTickCount();
//	DWORD endTime = GetTickCount();
//	cout << "run time =" << (endTime - startTime) << " ms" << endl;
	
	
//	string filename("c:\\users\\administrator\\Desktop\\100W.log");
	string filename("e:\\log\\HDFS_2\\hadoop-hdfs-datanode-mesos-22.log");
	//vector<Log> logs;
	vector<string> logs;
	string line;
	ifstream input_file(filename.c_str());
	if (!input_file.is_open()) {
		cerr << "Could not open the file - '"
		     << filename << "'" << endl;
		return 0;
	}
	int s;
	string c;
	int ctt = 0;
	while (getline(input_file, line)) {
		//int pos = line.find(" ");
		//s = stoi(line.substr(0, pos));
		//c = line.substr(pos + 11, line.length());
		//Log* lg = new Log(ctt, line);
		//logs.push_back(*lg);
		logs.push_back(line);
		ctt++;
//		cout << ctt << endl;
	}
	input_file.close();
	//	cout<<lines[0];
	//    cout<<logs[0].seq<<" "<<logs[0].content<<endl;
	cout<<logs.size();
	bloom_parameters parameters;

	// How many elements roughly do we expect to insert?
	parameters.projected_element_count = 120000;

	// Maximum tolerable false positive probability? (0,1)
	parameters.false_positive_probability = 0.0001; // 1 in 10000

	// Simple randomizer (optional)
	parameters.random_seed = 0xA5A5A5A5;

	if (!parameters) {
		std::cout << "Error - Invalid set of bloom filter parameters!" << std::endl;
		return 1;
	}

	parameters.compute_optimal_parameters();

	//Instantiate Bloom Filter
	bloom_filter filter(parameters);

	////DWORD startTime = GetTickCount();

	////for (int i = 0;i < 10000;i++) {
	////	filter.insert(logs[i].content);
	////}

	//////	for(auto l : logs){
	//////		filter.insert(l);
	//////	}

	////DWORD endTime = GetTickCount();
	////cout << "run time =" << (endTime - startTime) << " ms" << endl;

	//	cout<<filter.contains("ssssss")<<endl;
	//	cout<<filter.contains(logs[0].content)<<endl;
	thread t1(getTPS);
	
	DWORD startTime;
	DWORD endTime;
	
	this_thread::sleep_for(std::chrono::seconds(3));
	int ct = 0;
	vector<int> idl;
	string content="";
	vector<Pkg> pkgList;
	int i;
	for (i = 0; i < logs.size(); i++) {
		content+=logs[i];
		idl.push_back(i);
		filter.insert(logs[i]);
		ct++;
		have_finish++;
		if (ct == 10000) {
			
//			startTime = GetTickCount();
//			cout<<"contetn len=="<<content.length()<<endl;
			string hash_hex_str;
			picosha2::hash256_hex_string(content,hash_hex_str);
//			endTime = GetTickCount();
//			cout << "run time =" << (endTime - startTime) << " ms" << endl;
			
//			startTime = GetTickCount();
			bloom_filter filter2(filter);
//			endTime = GetTickCount();
//			cout << "run time =" << (endTime - startTime) << " ms" << endl;
			
//			startTime = GetTickCount();
			Pkg *p=new Pkg(idl,filter2,hash_hex_str);
			pkgList.push_back(*p);
//			endTime = GetTickCount();
//			cout << "run time =" << (endTime - startTime) << " ms" << endl;	
			this_thread::sleep_for(std::chrono::milliseconds(1));
			idl.clear();
			content="";
			ct = 0;
			filter.clear();
		}
	}
	cout<<pkgList.size()<<endl;
//	cout<<pkgList[1]<<endl;
//	cout<<pkgList[1]->pkghash<<endl;
//	cout<<pkgList[1]->bf.contains(logs[10005])<<endl;
//	t1.join();
	while(1) {
		if(have_finish>=logs.size()) {
			break;
		}
	}
	cout<<"[";
	for(int i=0;i<res.size();i++){
		cout<<res[i]<<",";
	}
	cout<<"]";
	return 0;
}
