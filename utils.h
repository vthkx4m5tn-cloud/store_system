#ifndef UTILS_H
#define UTILS_H

#include <string>

using std::string;

namespace u {
string tr(const string &s);
bool dig(const string &s);
bool let(const string &s);

bool phOK(const string &ph);             // 9 digits + starts 73/77/78/71
bool prOK(const string &s, double &out); // numeric

string rl(const string &p);
int ri(const string &p, int a, int b);
double rp(const string &p);

void ps();
int sp(const string &line, char d, string *out, int mx);
string lo(string s);
} // namespace u

#endif
