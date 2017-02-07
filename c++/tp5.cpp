#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <vector>
#include <map>
#include <limits>
#include <string>

using namespace std;

typedef vector<int> vertices;


struct simplex{
  int dim;
  float val;
  vertices vert;
  string toString(){
    string s = "";
    for (int v : vert){
      ostringstream temp;
      temp<<v;
      str=temp.str(); 
      s = s +str;
      s+=" ";
    }
    s = s.substr(0, s.length()-1);
    return s;
  }
};
bool compareSimplex(simplex s1, simplex s2){
  return s1.val < s2.val;
}



vector<simplex> read_filtration(string name){

  vector<simplex> F; F.clear();
  char input_file[100];
  sprintf(input_file, "%s", (char*) name.c_str());
  ifstream input(input_file);

  if (input){
    string line;
    while(getline(input,line)){
      stringstream stream(line);
      simplex s; s.vert.clear();
      s.dim = -1; stream >> s.val; stream >> s.dim; int i = 0;
      while(i <= s.dim){
        int f; stream >> f;
	s.vert.push_back(f); i++;
      }
      if(s.dim != -1)
        F.push_back(s);
    }
  }
  else{cout << "Failed to read file " << name << endl;}
  return F;

};


int main(int argc, char** argv) {

  if (argc != 2) {
    cout << "Syntax: " << argv[0] << " <filtration_file>" << endl;
    return 0;
  }
  string name = argv[1];
  cout << "Reading filtration..." << endl;
  vector<simplex> F = read_filtration(name);
  cout << "Done." << endl;

  sort(F.begin(), F.end(), compareSimplex);
  
  for(auto simp : F){
    sort(simp.vert.begin(), simp.vert.end());
  }
  vector<string>F_str;
  for(auto simp : F){
    F_str.push_back(simp.toString());
  }
  cout << "all is good" << endl;

  return 0;
}

