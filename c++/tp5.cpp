#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <vector>
#include <map>
#include <set>
#include <limits>
#include <string>
#include <unordered_map>
#include <stdlib.h>
#include <ctime>

using namespace std;

typedef vector<int> vertices;


string vert_to_string(const vertices& vert){
  int i=1, si = vert.size();
  string s = "";
  for(int v : vert){
    ostringstream temp;
    temp<<v;
    string str=temp.str(); 
    s = s +str;
    if(i<si)
      s+=" "; 
    i++;
  }
  return s;
}

string to_string(int x){
  ostringstream temp;
  temp<<x;
  return temp.str();   
}
string to_string(long double x){
  ostringstream temp;
  temp<<x;
  return temp.str();   
}

struct simplex{
  int dim;
  long double val;
  vertices vert;
  string toString(){
    return vert_to_string(vert);
  }
};
bool compareSimplex(simplex& s1, simplex& s2){
  return s1.val < s2.val;
}

vector<simplex> read_filtration(string& name){

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
      set<int>vs;
      while(i <= s.dim){
        int f; stream >> f;
        vs.insert(f); i++;
      }
      s.vert = vector<int>(vs.begin(), vs.end());
      if(s.dim != -1)
        F.push_back(s);
    }
  }
  else{cout << "Failed to read file " << name << endl;}
  return F;

};

vertices xor_operation(const vertices& col1, const vertices& col2){
  int n1 = col1.size(), n2 = col2.size();
  int i=0, j=0;
  vector<int>result;
  while(i<n1 && j<n2){
    if(col1[i] < col2[j]){
      result.push_back(col1[i]);
      i++;
    } else if(col1[i] > col2[j]){
      result.push_back(col2[j]);
      j++;
    } else{
      i++;
      j++;
    }
  }
  
  while (i<n1){
    result.push_back(col1[i]);
    i++;
  }
  while(j<n2){
    result.push_back(col2[j]);
    j++;
  }
  return result;
}

void output_codebar(vector<string>&barcode_list, string filename){
  ofstream myfile;
  myfile.open(filename);
  myfile.precision(3);
  for(auto s : barcode_list){
    myfile << s << endl;
  }
  myfile.close();
}

int main(int argc, char** argv) {
  ios_base::sync_with_stdio(false);
  if (argc < 2) {
    cout << "Syntax: " << argv[0] << " <filtration_file>" << endl;
    return 0;
  }
  string name = argv[1];
  string outname = "barcode.out";
  if(argc>2)
    outname = argv[2];
  cout << "Reading filtration..." << endl;
  vector<simplex> F = read_filtration(name);
  cout << "Done." << endl;

  // we want to know the execution time of our algorithm
  int start_s=clock();
  
  // we sort our simplexes by the time when they were created
  sort(F.begin(), F.end(), compareSimplex);
  
  // we sort the vertices in each simplex => so we have unicity of string representations
  for(auto simp : F){
    sort(simp.vert.begin(), simp.vert.end());
  }
  
  // F_str contains the string representations of the simplexes in F
  vector<string>F_str;
  for(auto simp : F){
    F_str.push_back(simp.toString());
  }
  // we map each string representation to its index in F
  unordered_map<string, int>d;
  for(int i=0; i<F_str.size(); i++)
    d[F_str[i]] = i;
  
  // we create a sparse matrix "sparse"
  int m = F.size();
  vector<vector<int>>sparse(m, vector<int>());
  for(int i=0; i<m; i++){
    if(F[i].dim>0){
      for (int j=0; j<F[i].vert.size(); j++){
        vector<int>temp;
        for(int k=0; k<F[i].vert.size(); k++){
          if(k!=j) temp.push_back(F[i].vert[k]);
        }
        sparse[i].push_back(d[vert_to_string(temp)]);
      }
    }
  }
  for(int i=0; i<m; i++)
    sort(sparse[i].begin(), sparse[i].end());
  cout << "we created a sparse matrix" << endl;
  
  // Reducing matrix
  vector<int>lowsparse(m, -1);
  vector<int>index_with_i_as_low(m, -1);
  for(int j=0; j<m; j++){
    while(sparse[j].size()>0){
      int lowcol = sparse[j][sparse[j].size()-1];
      lowsparse[j] = lowcol;
      // while the lowest element of the column i has already appeared as the last element of a previous column
      // so the index is different from -1
      int ind = index_with_i_as_low[lowcol];
      if(ind>=0){
        // if this row is the lowest row in some other column,
        // then we must subtract this other column from the current column
        sparse[j] = xor_operation(sparse[j], sparse[ind]);
        lowsparse[j] = -1;
      } else{
        // else, we have just found the lowest element from the column
        // and can end the reduction of this column
        index_with_i_as_low[lowcol] = j;
        break;
      }
    }
  }
  cout << "we reduced it " << endl;
  vector<string>finalanswer;
  for (int j=0; j<m; j++){
    if(lowsparse[j]==-1){
      if(index_with_i_as_low[j]==-1){
        simplex temp = F[j];
        finalanswer.push_back(to_string(temp.dim)+" "+to_string(temp.val)+" inf");
      }
    }
    else{
      simplex temp1 = F[lowsparse[j]];
      simplex temp2 = F[j];
      finalanswer.push_back(to_string(temp1.dim)+" "+to_string(temp1.val)+" "+to_string(temp2.val));
    }
  }
  int stop_s=clock();
  cout << "Time of execution of the algorithm: " << (stop_s-start_s)/double(CLOCKS_PER_SEC) << endl;
  output_codebar(finalanswer, "barcodes/"+outname);
  
  cout << "end of execution" << endl;
  return 0;
}

