#include "utils.h"
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

using namespace std;
using std::cout;
using std::string;

const int MU = 200;  // اقصى عدد مستخدمين
const int MP = 1000; // اقصى عدد منتجات
const int MC = 30;   // اقصى عدد منتجات في السلة

const char *UF = "users.txt"; // اسماء ملفات التخزين
const char *PF = "products.txt";
const char *SF = "sales.txt";

struct User {
  string un, pw, tp; // Admin/Employee
  char g;            // M/F
  string ph;
  bool ac; // active?
};

struct Rec { // للمنتجات
  int id;
  string nm;
  double pr;
  int st;   // المخزون
  int sold; // for top5
};

string dt() {               // داله الوقت
  time_t t = time(nullptr); // ترجع رقم يمثل عدد الثواني من تاريخ 1/1/1970 داله

  tm *m = localtime(&t); // يحوّل t من رقم ثواني إلى تفاصيل مفصلة
  char b[64];            // نحجز مساحه لتخزين النص
  strftime(b, sizeof(b), "%Y-%m-%d %H:%M:%S", m);
  return string(b); // نحول مصفوفه char الى string
}

// داله لايجاد المستخدم find user
int fu(User *a, int n, const string &un) {
  for (int i = 0; i < n; i++)
    if (a[i].un == un)
      return i;
  return -1;
}

int fp(Rec *a, int n, int id) {
  for (int i = 0; i < n; ++i)
    if (a[i].id == id)
      return i;
  return -1;
}

int nx(Rec *a, int n) {
  int mx = 0;
  for (int i = 0; i < n; ++i)
    if (a[i].id > mx)
      mx = a[i].id;
  return mx + 1;
}

void su(User *a, int n) {
  ofstream o(UF);
  for (int i = 0; i < n; i++)
    o << a[i].un << '|' << a[i].pw << '|' << a[i].tp << '|' << a[i].g << '|'
      << a[i].ph << '|' << (a[i].ac ? 1 : 0) << '\n';
}

void lu(User *a, int &n) {
  n = 0;
  ifstream in(UF);
  if (!in)
    return;

  string line;
  while (getline(in, line)) {
    string p[6];
    if (u::sp(u::tr(line), '|', p, 6) < 6)
      continue;

    a[n].un = p[0];
    a[n].pw = p[1];
    a[n].tp = p[2];
    a[n].g = p[3].empty() ? 'M' : p[3][0];
    a[n].ph = p[4];
    a[n].ac = (p[5] == "1");
    n++;

    if (n >= MU)
      break; // وقف إذا وصلنا الحد
  }
}

void spf(Rec *a, int n) {
  ofstream o(PF);
  o << fixed << setprecision(2);
  for (int i = 0; i < n; i++)
    o << a[i].id << '|' << a[i].nm << '|' << a[i].pr << '|' << a[i].st << '|'
      << a[i].sold << '\n';
}
void lpf(Rec *a, int &n) {
  n = 0;
  ifstream in(PF);
  if (!in)
    return;

  string line;
  while (getline(in, line)) {
    string p[5];
    if (u::sp(u::tr(line), '|', p, 5) < 5)
      continue;

    a[n].id = stoi(p[0]);
    a[n].nm = p[1];
    a[n].pr = stod(p[2]);
    a[n].st = stoi(p[3]);
    a[n].sold = stoi(p[4]);

    n++;
    if (n >= MP)
      break;
  }
}

void asale(const string &s) {
  ofstream o(SF, ios::app);
  o << s << '\n';
}
int csale() {
  ifstream in(SF);
  if (!in)
    return 0;

  int c = 0;
  string line;
  while (getline(in, line))
    if (!u::tr(line).empty())
      c++;

  return c;
}

void defA(User *a, int &n) {
  if (n > 0)
    return;
  a[0].un = "admin";
  a[0].pw = "admin";
  a[0].tp = "Admin";
  a[0].g = 'M';
  a[0].ph = "771234567";
  a[0].ac = true;
  n = 1;
  su(a, n);
}

// ===== Input helpers =====
string iun(User *a, int n) {
  while (true) {
    string x = u::rl("Username (letters only): ");
    if (!u::let(x)) {
      cout << "Invalid username. Letters only.\n";
      continue;
    }
    if (fu(a, n, x) != -1) {
      cout << "The username is found\n";
      continue;
    }
    return x;
  }
}

string ipw() {
  while (true) {
    string x = u::rl("Password: ");
    if (x.empty()) {
      cout << "Password cannot be empty.\n";
      continue;
    }
    return x;
  }
}

string iph() {
  while (true) {
    string x = u::rl("Phone (9 digits, starts 73/77/78/71): ");
    if (!u::phOK(x)) {
      cout << "Invalid phone number.\n";
      continue;
    }
    return x;
  }
}

char ig() {
  while (true) {
    string x = u::rl("Gender (M/F): ");
    if (x.size() != 1) {
      cout << "Invalid.\n";
      continue;
    }
    char c = (char)std::toupper((unsigned char)x[0]);
    if (c != 'M' && c != 'F') {
      cout << "Invalid.\n";
      continue;
    }
    return c;
  }
}

string itp() {
  cout << "Type:\n1) Admin\n2) Employee\n";
  int t = u::ri("Choose: ", 1, 2);
  return (t == 1) ? "Admin" : "Employee";
}

// ===== Admin (FULL required parts) =====
void aAdd(User *a, int &n) {
  if (n >= MU) {
    cout << "Capacity reached.\n";
    return;
  }
  User x;
  x.un = iun(a, n);
  x.pw = ipw();
  x.tp = itp();
  x.g = ig();
  x.ph = iph();
  x.ac = true;
  a[n++] = x;
  su(a, n);
  cout << "User added.\n";
}

void aDel(User *a, int &n) {
  string un = u::rl("Username to remove: ");
  int i = fu(a, n, un);
  if (i == -1) {
    cout << "Not found.\n";
    return;
  }

  // prevent removing last Admin
  int adm = 0;
  for (int k = 0; k < n; ++k)
    if (a[k].tp == "Admin")
      adm++;

  if (a[i].tp == "Admin" && adm <= 1) {
    cout << "Cannot remove the last Admin.\n";
    return;
  }

  for (int k = i; k < n - 1; ++k)
    a[k] = a[k + 1];
  n--;
  su(a, n);
  cout << "User removed.\n";
}

void aAct(User *a, int n) {
  string un = u::rl("Username: ");
  int i = fu(a, n, un);
  if (i == -1) {
    cout << "Not found.\n";
    return;
  }
  cout << "1) Activate\n2) Deactivate\n";
  int ch = u::ri("Choose: ", 1, 2);
  a[i].ac = (ch == 1);
  su(a, n);
  cout << "Updated.\n";
}

void aViewAll(User *a, int n) {
  cout << "---- Users ----\n";
  for (int i = 0; i < n; ++i)
    cout << a[i].un << " | " << a[i].tp << " | " << a[i].g << " | " << a[i].ph
         << " | " << (a[i].ac ? "Active" : "Inactive") << "\n";
}

void aView(User *a, int n) {
  cout << "View Users:\n1) By Type\n2) By Gender\n3) All\n";
  int ch = u::ri("Choose: ", 1, 3);

  if (ch == 3) {
    aViewAll(a, n);
    return;
  }

  if (ch == 1) {
    cout << "1) Admin\n2) Employee\n";
    int t = u::ri("Choose: ", 1, 2);
    string tp = (t == 1) ? "Admin" : "Employee";
    cout << "---- Users (" << tp << ") ----\n";
    for (int i = 0; i < n; ++i)
      if (a[i].tp == tp)
        cout << a[i].un << " | " << a[i].tp << " | " << a[i].g << " | "
             << a[i].ph << " | " << (a[i].ac ? "Active" : "Inactive") << "\n";
  } else {
    cout << "1) M\n2) F\n";
    int g = u::ri("Choose: ", 1, 2);
    char gg = (g == 1) ? 'M' : 'F';
    cout << "---- Users (Gender " << gg << ") ----\n";
    for (int i = 0; i < n; ++i)
      if (a[i].g == gg)
        cout << a[i].un << " | " << a[i].tp << " | " << a[i].g << " | "
             << a[i].ph << " | " << (a[i].ac ? "Active" : "Inactive") << "\n";
  }
}

void aStat(User *a, int nu, Rec *p, int np) {
  int ac = 0;
  for (int i = 0; i < nu; ++i)
    if (a[i].ac)
      ac++;
  cout << "Users: " << nu << "\n";
  cout << "Active: " << ac << "\n";
  cout << "Products (records): " << np << "\n";
  cout << "Transactions: " << csale() << "\n";
}

void pAdd(Rec *a, int &n) {
  if (n >= MP) {
    cout << "Capacity reached.\n";
    return;
  }
  Rec r;
  r.id = nx(a, n);
  r.nm = u::rl("Product name: ");
  if (r.nm.empty()) {
    cout << "Name cannot be empty.\n";
    return;
  }
  r.pr = u::rp("Price: ");
  r.st = u::ri("Stock: ", 0, 1000000);
  r.sold = 0;

  a[n++] = r;
  spf(a, n);
  cout << "Added with ID: " << r.id << "\n";
}

void pOne(Rec *a, int n) {
  int id = u::ri("Product ID: ", 1, 1000000000);
  int i = fp(a, n, id);
  if (i == -1) {
    cout << "Not found.\n";
    return;
  }
  cout << "ID: " << a[i].id << "\n";
  cout << "Name: " << a[i].nm << "\n";
  cout << "Price: " << std::fixed << std::setprecision(2) << a[i].pr << "\n";
  cout << "Stock: " << a[i].st << "\n";
  cout << "Sold: " << a[i].sold << "\n";
}

void pAll(Rec *a, int n) {
  if (n == 0) {
    cout << "(No products)\n";
    return;
  }
  cout << "ID | Name | Price | Stock | Sold\n";
  for (int i = 0; i < n; ++i)
    cout << a[i].id << " | " << a[i].nm << " | " << std::fixed
         << std::setprecision(2) << a[i].pr << " | " << a[i].st << " | "
         << a[i].sold << "\n";
}

void pDel(Rec *a, int &n) {
  int id = u::ri("ID to delete: ", 1, 1000000000);
  int i = fp(a, n, id);
  if (i == -1) {
    cout << "Not found.\n";
    return;
  }
  for (int k = i; k < n - 1; ++k)
    a[k] = a[k + 1];
  n--;
  spf(a, n);
  cout << "Deleted.\n";
}

void pUpd(Rec *a, int n) {
  int id = u::ri("ID to update: ", 1, 1000000000);
  int i = fp(a, n, id);
  if (i == -1) {
    cout << "Not found.\n";
    return;
  }

  cout << "Leave empty to keep current.\n";
  string nm = u::rl("New name (" + a[i].nm + "): ");
  if (!nm.empty())
    a[i].nm = nm;

  string prS = u::rl("New price (" + std::to_string(a[i].pr) + "): ");
  if (!prS.empty()) {
    double v = 0;
    if (!u::prOK(prS, v))
      cout << "Invalid price. Not changed.\n";
    else
      a[i].pr = v;
  }

  string stS = u::rl("New stock (" + std::to_string(a[i].st) + "): ");
  if (!stS.empty()) {
    if (!u::dig(stS))
      cout << "Invalid stock. Not changed.\n";
    else
      a[i].st = std::stoi(stS);
  }

  spf(a, n);
  cout << "Updated.\n";
}

void pSrc(Rec *a, int n) {
  cout << "Search:\n1) By ID\n2) By Name/Product Name\n";
  int ch = u::ri("Choose: ", 1, 2);

  if (ch == 1) {
    int id = u::ri("ID: ", 1, 1000000000);
    int i = fp(a, n, id);
    if (i == -1)
      cout << "Not found.\n";
    else
      cout << "Found: " << a[i].id << " | " << a[i].nm << " | " << std::fixed
           << std::setprecision(2) << a[i].pr << "\n";
  } else {
    string key = u::lo(u::rl("Keyword: "));
    if (key.empty()) {
      cout << "Empty keyword.\n";
      return;
    }

    bool any = false;
    for (int i = 0; i < n; ++i) {
      if (u::lo(a[i].nm).find(key) != string::npos) {
        cout << a[i].id << " | " << a[i].nm << " | " << std::fixed
             << std::setprecision(2) << a[i].pr << " | Stock=" << a[i].st
             << "\n";
        any = true;
      }
    }
    if (!any)
      cout << "Not found.\n";
  }
}

void invH(const string &emp) {
  cout << "====================================\n";
  cout << "            STORE INVOICE           \n";
  cout << "Date: " << dt() << "\n";
  cout << "Employee: " << emp << "\n";
  cout << "------------------------------------\n";
  cout << "Item                      Qty  Total\n";
  cout << "------------------------------------\n";
}

void invF(double gt) {
  cout << "------------------------------------\n";
  cout << "GRAND TOTAL: " << std::fixed << std::setprecision(2) << gt << "\n";
  cout << "====================================\n";
}

void buy(Rec *a, int n, const string &emp) {
  if (n == 0) {
    cout << "No products.\n";
    return;
  }

  int cid[MC], cq[MC], cc = 0;

  while (cc < MC) {
    cout << "\n1) Add item\n2) Finish\n";
    if (u::ri("Choose: ", 1, 2) == 2)
      break;

    int id = u::ri("Product ID: ", 1, 1000000000);
    int i = fp(a, n, id);
    if (i == -1) {
      cout << "Not found.\n";
      continue;
    }

    int q = u::ri("Qty: ", 1, 1000000);
    if (q > a[i].st) {
      cout << "Not enough stock.\n";
      continue;
    }

    cid[cc] = id;
    cq[cc] = q;
    cc++;
    cout << "Added.\n";
  }

  if (cc == 0) {
    cout << "No purchase.\n";
    return;
  }

  invH(emp);
  double gt = 0.0;

  cout << fixed << setprecision(2);

  for (int k = 0; k < cc; k++) {
    int i = fp(a, n, cid[k]);
    if (i == -1)
      continue;

    int q = cq[k];
    double lt = a[i].pr * q;
    gt += lt;

    a[i].st -= q;
    a[i].sold += q;

    asale(dt() + "|" + emp + "|" + to_string(a[i].id) + "|" + a[i].nm + "|" +
          to_string(q) + "|" + to_string(a[i].pr) + "|" + to_string(lt));

    string nm = a[i].nm;
    if (nm.size() > 24)
      nm = nm.substr(0, 24);

    cout << left << setw(24) << nm << right << setw(4) << q << right << setw(8)
         << lt << '\n';
  }

  invF(gt);
  spf(a, n);
  cout << "Done.\n";
}

void top5(Rec *a, int n) {
  if (n == 0) {
    cout << "No products.\n";
    return;
  }

  bool used[MP]; // MP كبير عندك (1000) فمناسب
  for (int i = 0; i < n; i++)
    used[i] = false;

  cout << "===== TOP 5 SOLD =====\n";
  cout << "Rank | ID | Name | Sold\n";

  for (int rank = 1; rank <= 5; rank++) {
    int best = -1;

    for (int i = 0; i < n; i++) {
      if (used[i])
        continue;
      if (best == -1 || a[i].sold > a[best].sold)
        best = i;
    }

    if (best == -1 || a[best].sold <= 0)
      break;

    used[best] = true;
    cout << rank << " | " << a[best].id << " | " << a[best].nm << " | "
         << a[best].sold << "\n";
  }
}

bool li(User *a, int n, User &out) {
  cout << "===== Login =====\n";
  string un = u::rl("Username: ");
  string pw = u::rl("Password: ");

  int i = fu(a, n, un);
  if (i == -1) {
    cout << "Invalid credentials.\n";
    return false;
  }
  if (a[i].pw != pw) {
    cout << "Invalid credentials.\n";
    return false;
  }
  if (!a[i].ac) {
    cout << "Account is Inactive.\n";
    return false;
  }

  out = a[i];
  return true;
}

void am(User *a, int &nu, Rec *p, int np) {
  while (true) {
    cout << "\nADMIN MENU\n"
            "1) Add user\n"
            "2) Remove user\n"
            "3) Activate/Deactivate\n"
            "4) View users (type/gender)\n"
            "5) Stats\n"
            "6) Logout\n";
    int ch = u::ri("Choose: ", 1, 6);

    if (ch == 1)
      aAdd(a, nu);
    else if (ch == 2)
      aDel(a, nu);
    else if (ch == 3)
      aAct(a, nu);
    else if (ch == 4)
      aView(a, nu);
    else if (ch == 5)
      aStat(a, nu, p, np);
    else
      break;

    u::ps();
  }
}

void em(Rec *p, int &np, const string &emp) {
  while (true) {
    cout << "\nEMPLOYEE MENU\n"
            "1) Add product\n"
            "2) Delete product\n"
            "3) Update product\n"
            "4) View one\n"
            "5) View all\n"
            "6) Search\n"
            "7) Purchase + Invoice\n"
            "8) Top5\n"
            "9) Logout\n";
    int ch = u::ri("Choose: ", 1, 9);

    if (ch == 1)
      pAdd(p, np);
    else if (ch == 2)
      pDel(p, np);
    else if (ch == 3)
      pUpd(p, np);
    else if (ch == 4)
      pOne(p, np);
    else if (ch == 5)
      pAll(p, np);
    else if (ch == 6)
      pSrc(p, np);
    else if (ch == 7)
      buy(p, np, emp);
    else if (ch == 8)
      top5(p, np);
    else
      break;

    u::ps();
  }
}

int main() {
  User *us = new User[MU];
  Rec *pr = new Rec[MP];

  int nu = 0, np = 0;
  lu(us, nu);
  defA(us, nu);
  lpf(pr, np);

  while (true) {
    cout << "=============================\n";
    cout << "   STORE MANAGEMENT SYSTEM   \n";
    cout << "=============================\n";

    User cur;
    if (!li(us, nu, cur)) {
      cout << "\n1) Try again\n2) Exit\n";
      int ch = u::ri("Choose: ", 1, 2);
      if (ch == 2)
        break;
      continue;
    }

    if (cur.tp == "Admin")
      am(us, nu, pr, np);
    else
      em(pr, np, cur.un);
  }

  delete[] us;
  delete[] pr;

  cout << "Goodbye.\n";
  return 0;
}
