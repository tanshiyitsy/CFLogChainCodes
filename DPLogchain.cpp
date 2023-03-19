#include "picosha2.h"
#include <fstream>
#include <chrono>
#include <iostream>
#include <vector>
#include <thread>
#include <windows.h>
#include <openssl/sha.h>
using namespace std;

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
		int packtime;
		string pkghash;
		string content;
		Pkg(vector<int> l,int packtime,string s,string c){
			this->indexList=l;
			this->packtime=packtime;
			this->pkghash=s;	
			this->content=c;
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

string buildTree(vector<Pkg> p){
	//bottom 1024  height=11  2047pkg per tree    102400 TX per Tree
	int ms=0;
	for(int i=0;i<p.size();i++){
		ms+=p[i].packtime;
	}
	//cout<<"ms=="<<ms<<endl;
	this_thread::sleep_for(std::chrono::milliseconds(ms));
	while(p.size()>1){
//		cout<<"merge"<<p.size()<<endl;
		vector<Pkg> temp;
		for(int i=0;i<p.size();i+=2){
			string c=p[i].content+p[i+1].content;
			vector<int> idxL=p[i].indexList;
			vector<int> idxL2=p[i+1].indexList;
			for(int j=0;j<idxL2.size();j++){
				idxL.push_back(idxL2[j]);
			}
			string hash_hex_str;
			picosha2::hash256_hex_string(cc,hash_hex_str);
//			cout<<"[";
//			for(int i=0;i<idxL.size();i++){
//				cout<<idxL[i]<<" ";
//			}
//			cout<<"]"<<endl;
//			cout<<c<<endl;
			temp.push_back(*new Pkg(idxL,0,hash_hex_str,c));
		}
		p=temp;
	}
	return p[0].pkghash;
}

int main() {
//	93.3 info  6.3 WARN    0.3 ERROR  0.1 FATAL

	
	
//	string filename("c:\\users\\administrator\\Desktop\\100W.log");
	string filename("e:\\log\\HDFS_2\\hadoop-hdfs-datanode-mesos-19.log");
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
		logs.push_back(line);
		ctt++;
	}
	input_file.close();
	cout<<logs.size()<<endl;

	vector<Pkg> packs;
	
	DWORD startTime;
	DWORD endTime;
	string content="";
	vector<int> idxList;
	int ct=0;
	startTime = GetTickCount();
	for(int i=0;i<logs.size();i++){
		content+=logs[i];
		idxList.push_back(i);
		ct++;
		if(ct==100){
			string hash_hex_str;
			picosha2::hash256_hex_string(content,hash_hex_str);
			endTime=GetTickCount();
//			cout<<(endTime - startTime)<<endl;
			packs.push_back(*new Pkg(idxList,(endTime - startTime),hash_hex_str,content));
			startTime=endTime;
			ct=0;
			content="";
			idxList.clear();
		}
	}
	cout<<"the total count of package=="<<(packs.size())<<endl;
	
	vector<int> final;
	for(int i=0;i<packs.size();i+=1024){
		if(packs.size()-i<1024) break;
		vector<Pkg> temp; 
		for(int j=i;j<i+1024;j++){
			temp.push_back(packs[j]);
		}
//		cout<<temp.size()<<endl;
		startTime=GetTickCount();
		string f=buildTree(temp);
		endTime=GetTickCount();
		double dd=102400000/(endTime-startTime);
		cout<<dd<<endl;
		final.push_back(dd);
	}
	cout<<"[";
	for(int i=0;i<final.size();i++){
		cout<<final[i]<<",";	
	}
	cout<<"]";
	
	
//	thread t1(getTPS);
//	
//	this_thread::sleep_for(std::chrono::seconds(3));
//	int ct = 0;
//	vector<int> idl;
//	string content="";
//	vector<Pkg> pkgList;
//	int i;
//	for (i = 0; i < logs.size(); i++) {
//		content+=logs[i];
//		idl.push_back(i);
//		filter.insert(logs[i]);
//		ct++;
//		have_finish++;
//		if (ct == 10000) {
//			
////			startTime = GetTickCount();
////			cout<<"contetn len=="<<content.length()<<endl;
//			content=content.substr(0, content.length()/10);
//			string hash_hex_str;
//			picosha2::hash256_hex_string(content,hash_hex_str);
////			endTime = GetTickCount();
////			cout << "run time =" << (endTime - startTime) << " ms" << endl;
//			
////			startTime = GetTickCount();
//			bloom_filter filter2(filter);
////			endTime = GetTickCount();
////			cout << "run time =" << (endTime - startTime) << " ms" << endl;
//			
////			startTime = GetTickCount();
//			Pkg *p=new Pkg(idl,filter2,hash_hex_str);
//			pkgList.push_back(*p);
////			endTime = GetTickCount();
////			cout << "run time =" << (endTime - startTime) << " ms" << endl;	
//			this_thread::sleep_for(std::chrono::milliseconds(1));
//			idl.clear();
//			content="";
//			ct = 0;
//			filter.clear();
//		}
//	}
//	cout<<pkgList.size()<<endl;
////	cout<<pkgList[1]<<endl;
////	cout<<pkgList[1]->pkghash<<endl;
////	cout<<pkgList[1]->bf.contains(logs[10005])<<endl;
////	t1.join();
//	while(1) {
//		if(have_finish>=logs.size()) {
//			break;
//		}
//	}
//	cout<<"[";
//	for(int i=0;i<res.size();i++){
//		cout<<res[i]<<",";
//	}
//	cout<<"]";
	return 0;
}
