#include "utils.h"

#include <cctype>
#include <iostream>
#include <string>

using namespace std;

namespace u {

// trim
string tr(const string &s) {
  size_t a = 0, b = s.size();
  while (a < b && isspace((unsigned char)s[a]))
    a++;
  while (b > a && isspace((unsigned char)s[b - 1]))
    b--;
  return s.substr(a, b - a);
}

// lowercase
string lo(string s) {
  for (size_t i = 0; i < s.size(); i++)
    s[i] = (char)tolower((unsigned char)s[i]);
  return s;
}

// digits only
bool dig(const string &s) {
  if (s.empty())
    return false;
  for (char c : s)
    if (!isdigit((unsigned char)c))
      return false;
  return true;
}

// letters only
bool let(const string &s) {
  if (s.empty())
    return false;
  for (char c : s)
    if (!isalpha((unsigned char)c))
      return false;
  return true;
}

// phone: 9 digits + starts 73/77/78/71
bool phOK(const string &ph) {
  if (ph.size() != 9)
    return false;
  if (!dig(ph))
    return false;
  string p = ph.substr(0, 2);
  return (p == "73" || p == "77" || p == "78" || p == "71");
}

// price numeric (allow one dot)
bool prOK(const string &s, double &out) {
  string t = tr(s);
  if (t.empty())
    return false;

  int dots = 0;
  for (char c : t) {
    if (c == '.')
      dots++;
    else if (!isdigit((unsigned char)c))
      return false;
  }
  if (dots > 1)
    return false;

  try {
    out = stod(t);
  } catch (...) {
    return false;
  }

  return out >= 0.0;
}

// read line with prompt
string rl(const string &p) {
  cout << p;
  string s;
  getline(cin, s);
  return tr(s);
}

// read int in range [a,b]
int ri(const string &p, int a, int b) {
  while (true) {
    string s = rl(p);
    if (!dig(s)) {
      cout << "Invalid number.\n";
      continue;
    }

    long long v = 0;
    try {
      v = stoll(s);
    } catch (...) {
      cout << "Invalid number.\n";
      continue;
    }

    if (v < a || v > b) {
      cout << "Out of range.\n";
      continue;
    }
    return (int)v;
  }
}

// read price (double)
double rp(const string &p) {
  while (true) {
    string s = rl(p);
    double v = 0.0;
    if (!prOK(s, v)) {
      cout << "Invalid price.\n";
      continue;
    }
    return v;
  }
}

// pause
void ps() {
  cout << "Press ENTER to continue...";
  string x;
  getline(cin, x);
}

int sp(const string &line, char d, string *out, int mx) {
  int c = 0;
  string cur;

  for (char ch : line) {
    if (ch == d) {
      if (c < mx)
        out[c++] = cur;
      cur.clear();
    } else {
      cur.push_back(ch);
    }
  }
  if (c < mx)
    out[c++] = cur;
  return c;
}

} // namespace u
