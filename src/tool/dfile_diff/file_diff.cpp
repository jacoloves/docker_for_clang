#include <iostream>
#include <string>
#include <fstream>
using namespace std;

/************/
/* prottype */
/************/
bool CheckArg(int); // variable check
void ErrorEnd(); // error end process
int FileLineCounter(char*); // file line count

// constant
const int kFileNum = 3;
const int kFirstFile = 1;
const int kSecondFile = 2;
const int kFirstChar = 0;
const int kSecondChar = 1;


//main
int main(int argc, char* argv[]) {
  // variable
  bool check_sum;
  int file1_line_num;
  int file2_line_num;
  int diff_file_line;

  // variable init
  check_sum = true;
  file1_line_num = 0;
  file2_line_num = 0;
  diff_file_line = 0;

  check_sum = CheckArg(argc);

  // check success
  if (!check_sum) {
    cerr << "Incorrect number of input files" << endl;
    ErrorEnd();
  }

  // first file count
  file1_line_num = FileLineCounter(argv[kFirstFile]);
  cout << argv[kFirstFile] << " -> " << file1_line_num << " steps " << endl;
  
  // second file count
  file2_line_num = FileLineCounter(argv[kSecondFile]);
  cout << argv[kSecondFile] << " -> " << file2_line_num << " steps " << endl;

  // diff 2 file lines
  diff_file_line = file1_line_num - file2_line_num;
  cout << "diff -> " << diff_file_line << " steps" << endl;

  return EXIT_SUCCESS; 
}

// variable number check
bool CheckArg(int argc) {
  if (argc != kFileNum)
    return false;
  return true;
}

// error end process
void ErrorEnd() {
  cerr << "process failed" << endl;
  exit(EXIT_FAILURE);
}

// file line count
int FileLineCounter(char* argv_file_name) {
  // variable
  string file_name; // filename
  fstream file; // input/output-Stream
  int file_count; // file line counter
  string file_line; // file line
  
  // variable init
  file_name = "";
  file_count = 0;

  file_name = argv_file_name;

  // file open
  file.open(file_name, ios::in);
  if(!file.is_open()) {
    cerr << "file dosen't open" << endl;
    ErrorEnd();
  }

  // file read
  while (getline(file, file_line)) {
    // comment & blank removal
    if ((file_line[kFirstChar] != '/' && file_line[kSecondChar] != '/') || (file_line[kFirstChar] != '/' && file_line[kSecondChar] != '*'))
      file_count++;
  }
  
  // file close
  file.close();

  return file_count;
}

