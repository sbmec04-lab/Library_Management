#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <fstream>
#include <sstream>
#include <limits> // Zaroori hai buffer clear karne ke liye

using namespace std;

// --- DATE HELPER FUNCTIONS ---

string getCurrentDate() {
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d", &tstruct);
    return buf;
}

time_t parseDate(const string& dateStr) {
    if (dateStr == "N/A" || dateStr.empty()) return 0;
    struct tm tm = {0};
    int year = stoi(dateStr.substr(0, 4));
    int month = stoi(dateStr.substr(5, 2));
    int day = stoi(dateStr.substr(8, 2));
    tm.tm_year = year - 1900;
    tm.tm_mon = month - 1;
    tm.tm_mday = day;
    return mktime(&tm);
}

int calculateOverdueDays(const string& currentDate, const string& dueDate) {
    if (dueDate == "N/A" || dueDate.empty()) return 0;
    if (dueDate >= currentDate) return 0;
    time_t now = parseDate(currentDate);
    time_t due = parseDate(dueDate);
    double seconds = difftime(now, due);
    return static_cast<int>(seconds / (60 * 60 * 24));
}

// --- CLASS: BOOK ---
class Book {
private:
    string title;
    string author;
    int id;
    bool isIssued;
    string dueDate;

public:
    Book(string t, string a, int i, bool issued = false, string date = "N/A")
        : title(t), author(a), id(i), isIssued(issued), dueDate(date) {}

    string getTitle() const { return title; }
    int getID() const { return id; }
    bool getIssuedStatus() const { return isIssued; }
    string getDueDate() const { return dueDate; }

    void setIsIssued(bool status) { isIssued = status; }
    void setDueDate(string date) { dueDate = date; }

    // File format: ID|Title|Author|Issued(1/0)|DueDate
    string toFileString() const {
        return to_string(id) + "|" + title + "|" + author + "|" + (isIssued ? "1" : "0") + "|" + dueDate;
    }

    double calculateFine() const {
        if (!isIssued) return 0.0;
        string currentDate = getCurrentDate();
        int overdueDays = calculateOverdueDays(currentDate, dueDate);
        if (overdueDays <= 0) return 0.0;
        return overdueDays * 10.0; // 10 Rs per day fine
    }
};

// --- CLASS: LIBRARY ---
class Library {
private:
    vector<Book> books;
    const string filename = "library_data.txt";

    vector<string> split(const string &s, char delimiter) {
        vector<string> tokens;
        string token;
        istringstream tokenStream(s);
        while (getline(tokenStream, token, delimiter)) {
            tokens.push_back(token);
        }
        return tokens;
    }

public:
    Library() { loadData(); }

    void loadData() {
        books.clear();
        ifstream file(filename);
        if (!file.is_open()) {
            // File nahi mili, koi baat nahi, nayi banegi
            return; 
        }

        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;
            vector<string> data = split(line, '|');
            if (data.size() >= 5) {
                int id = stoi(data[0]);
                string title = data[1];
                string author = data[2];
                bool isIssued = (data[3] == "1");
                string dueDate = data[4];
                books.push_back(Book(title, author, id, isIssued, dueDate));
            }
        }
        file.close();
        cout << "[System] Loaded " << books.size() << " books from database." << endl;
    }

    void saveData() {
        ofstream file(filename);
        for (const auto& book : books) {
            file << book.toFileString() << endl;
        }
        file.close();
    }

    void addBook(string title, string author, int id) {
        books.push_back(Book(title, author, id));
        saveData();
        cout << ">> Book Added Successfully!" << endl;
    }

    void displayBooks() const {
        if (books.empty()) {
            cout << "\n>> No books available in the library yet." << endl;
            return;
        }
        cout << "\n--- ALL BOOKS ---" << endl;
        for (const auto& book : books) {
            cout << "ID: " << book.getID() 
                 << " | Title: " << book.getTitle() 
                 << " | Status: " << (book.getIssuedStatus() ? "[ISSUED]" : "[AVAILABLE]");
            if(book.getIssuedStatus()) cout << " | Due: " << book.getDueDate();
            cout << endl;
        }
        cout << "-----------------" << endl;
    }

    void checkFine(int id) const {
        bool found = false;
        for (const auto& book : books) {
            if (book.getID() == id) {
                found = true;
                if (book.getIssuedStatus()) {
                    double fine = book.calculateFine();
                    cout << "\nBook: " << book.getTitle() << endl;
                    cout << "Due Date: " << book.getDueDate() << endl;
                    if (fine > 0) cout << ">> FINE APPLICABLE: Rs. " << fine << endl;
                    else cout << ">> No Fine. (Within due date)" << endl;
                } else {
                    cout << ">> This book is currently AVAILABLE (Not Issued)." << endl;
                }
                break;
            }
        }
        if (!found) cout << ">> Book ID not found!" << endl;
    }

    void issueBook(int id, string date) {
        bool found = false;
        for (auto& book : books) {
            if (book.getID() == id) {
                found = true;
                if (!book.getIssuedStatus()) {
                    book.setIsIssued(true);
                    book.setDueDate(date);
                    saveData();
                    cout << ">> Book Issued Successfully!" << endl;
                } else {
                    cout << ">> Sorry, this book is ALREADY ISSUED." << endl;
                }
                break;
            }
        }
        if (!found) cout << ">> Book ID not found!" << endl;
    }

    void returnBook(int id) {
        bool found = false;
        for (auto& book : books) {
            if (book.getID() == id) {
                found = true;
                if (book.getIssuedStatus()) {
                    double fine = book.calculateFine();
                    cout << "Returning: " << book.getTitle() << endl;
                    if (fine > 0) cout << ">> PAY FINE: Rs. " << fine << endl;
                    
                    book.setIsIssued(false);
                    book.setDueDate("N/A");
                    saveData();
                    cout << ">> Book Returned Successfully!" << endl;
                } else {
                    cout << ">> Error: This book was not issued." << endl;
                }
                break;
            }
        }
        if (!found) cout << ">> Book ID not found!" << endl;
    }
    
    // Helper to check if ID exists (to prevent duplicate IDs)
    bool idExists(int id) {
        for (const auto& book : books) {
            if (book.getID() == id) return true;
        }
        return false;
    }
};

// --- MENUS ---
void adminMenu(Library& library) {
    int choice;
    do {
        cout << "\n[ADMIN PANEL]\n1. Add Book\n2. Show All Books\n3. Check Fine\n4. Back\nChoice: ";
        if (!(cin >> choice)) { // Input validation logic
            cout << "Invalid input! Enter a number." << endl;
            cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        if (choice == 1) {
            string title, author;
            int id;
            
            cout << "Enter Book ID (Number): ";
            cin >> id;
            if (library.idExists(id)) {
                cout << "Error: Book ID already exists!" << endl;
                continue;
            }

            // Buffer Clear karna zaroori hai ID aur String ke beech mein
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); 

            cout << "Enter Title: ";
            getline(cin, title);
            
            cout << "Enter Author: ";
            getline(cin, author);

            library.addBook(title, author, id);

        } else if (choice == 2) {
            library.displayBooks();
        } else if (choice == 3) {
            int id;
            cout << "Enter Book ID: "; cin >> id;
            library.checkFine(id);
        }
    } while (choice != 4);
}

void studentMenu(Library& library) {
    int choice;
    do {
        cout << "\n[STUDENT PANEL]\n1. Issue Book\n2. Return Book\n3. Back\nChoice: ";
        if (!(cin >> choice)) {
            cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        if (choice == 1) {
            int id; string date;
            cout << "Enter Book ID: "; cin >> id;
            cout << "Enter Return Due Date (YYYY-MM-DD): "; cin >> date;
            library.issueBook(id, date);
        } else if (choice == 2) {
            int id;
            cout << "Enter Book ID: "; cin >> id;
            library.returnBook(id);
        }
    } while (choice != 3);
}

// --- MAIN ---
int main() {
    Library library; // Data automatically load hoga
    int choice;

    while (true) {
        cout << "\n=== LIBRARY MANAGEMENT SYSTEM ===\n1. Admin Login\n2. Student Login\n3. Exit\nChoice: ";
        if (!(cin >> choice)) {
            cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        if (choice == 1) {
            string pass;
            cout << "Enter Password (admin123): ";
            cin >> pass;
            if (pass == "admin123") {
                adminMenu(library);
            } else {
                cout << ">> Wrong Password!" << endl;
            }
        } else if (choice == 2) {
            studentMenu(library);
        } else if (choice == 3) {
            cout << "Exiting..." << endl;
            break;
        } else {
            cout << "Invalid Choice." << endl;
        }
    }
    return 0;
}
