#include <iostream>
#include <fstream> 
#include <string>
#include <vector>
#include <ctime>
#include <sstream>
// #include<bits/stdc++.h>

const std::string LIBRARY_FILE = "library_data.txt";

std::string getCurrentDate() {
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    // Format: YYYY-MM-DD
    strftime(buf, sizeof(buf), "%Y-%m-%d", &tstruct);
    return buf;
}

int calculateOverdueDays(const std::string& currentDate, const std::string& dueDate) {
   
    return (dueDate > currentDate) ? 0 : (std::stoi(currentDate.substr(8, 2)) - std::stoi(dueDate.substr(8, 2)));
}

class Book {
private:
    std::string title;
    std::string author;
    int id;
    bool isIssued;
    std::string dueDate;
public:
    // Default constructor added for file loading
    Book() : id(0), isIssued(false) {} 

    Book(const std::string& title, const std::string& author, int id)
        : title(title), author(author), id(id), isIssued(false), dueDate("") {}

    // Getters and Setters
    const std::string& getTitle() const { return title; }
    const std::string& getAuthor() const { return author; }
    int getID() const { return id; }
    bool getIssuedStatus() const { return isIssued; }
    void setIsIssued(bool status) { isIssued = status; }

    void setDueDate(const std::string& dueDate) { this->dueDate = dueDate; }
    const std::string& getDueDate() const { return dueDate; }

    // Setters for file loading
    void setTitle(const std::string& t) { title = t; }
    void setAuthor(const std::string& a) { author = a; }
    void setID(int i) { id = i; }
    void setIssuedStatus(bool status) { isIssued = status; }

    double calculateFine() const {
        const double fineRatePerDay = 0.50; 
        const double maxFine = 10.00; 
        if (!getIssuedStatus()) {
            return 0.0;
        }
        std::string dueDateString = getDueDate();
        std::string currentDate = getCurrentDate();
        int overdueDays = calculateOverdueDays(currentDate, dueDateString);
        if (overdueDays <= 0) {
            return 0.0; 
        }
        double calculatedFine = overdueDays * fineRatePerDay;
        return (calculatedFine > maxFine) ? maxFine : calculatedFine;
    }

    // New: Method to serialize Book data for saving
    std::string toString() const {
        // Format: ID|Title|Author|isIssued|DueDate
        return std::to_string(id) + "|" + title + "|" + author + "|" + 
               (isIssued ? "1" : "0") + "|" + dueDate;
    }
};

class Library {
private:
    std::vector<Book> books; 
public:
    // Load books from file
    void loadBooksFromFile() {
        std::ifstream inputFile(LIBRARY_FILE);
        std::string line;
        books.clear(); // Start with a clean slate

        if (inputFile.is_open()) {
            while (std::getline(inputFile, line)) {
                // Use a stringstream to parse the line
                std::stringstream ss(line);
                std::string segment;
                std::vector<std::string> parts;

                // Split the line by the delimiter '|'
                while (std::getline(ss, segment, '|')) {
                    parts.push_back(segment);
                }

                // Check if we have all 5 parts
                if (parts.size() == 5) {
                    Book book;
                    try {
                        book.setID(std::stoi(parts[0]));
                        book.setTitle(parts[1]);
                        book.setAuthor(parts[2]);
                        book.setIssuedStatus(parts[3] == "1");
                        book.setDueDate(parts[4]);
                        books.push_back(book);
                    } catch (const std::exception& e) {
                        std::cerr << "Error parsing book data: " << std::endl;
                    }
                }
            }
            inputFile.close();
            std::cout << "\n Library data loaded successfully from " << LIBRARY_FILE << std::endl;
        } else {
            std::cout << "\n No existing library data found. Starting with an empty library." << std::endl;
        }
    }

    // New: Save books to file
    void saveBooksToFile() const {
        std::ofstream outputFile(LIBRARY_FILE);

        if (outputFile.is_open()) {
            for (const auto& book : books) {
                outputFile << book.toString() << "\n";
            }
            outputFile.close();
            std::cout << "\n Library data saved successfully to " << LIBRARY_FILE << std::endl;
        } else {
            std::cerr << "\n ERROR: Could not open file " << LIBRARY_FILE << " for saving." << std::endl;
        }
    }

    void addBook(const Book& book) {
        books.push_back(book);
    }

    const Book* findBookById(int id) const { 
        for (const auto& book : books) {
            if (book.getID() == id) {
                return &book;
            }
        }
        return nullptr;
    }

    Book* findBookById(int id) { 
        for (auto& book : books) {
            if (book.getID() == id) {
                return &book;
            }
        }
        return nullptr;
    }

    void issueBook(int id, const std::string& dueDate) {
        Book* book = findBookById(id); 
        if (book && !book->getIssuedStatus()) {
            book->setIsIssued(true);
            book->setDueDate(dueDate);  
            std::cout << "Book issued: " << book->getTitle() << std::endl;
        } else {
            std::cout << "Book not found or already issued." << std::endl;
        }
    }

    void returnBook(int id) {
        Book* book = findBookById(id); 
        if (book && book->getIssuedStatus()) {
            book->setIsIssued(false);
            std::cout << "Book returned: " << book->getTitle() << std::endl;
        } else {
            std::cout << "Book not found or not issued." << std::endl;
        }
    }

    int getAvailableBooksCount() const {
        int count = 0;
        for (const auto& book : books) {
            if (!book.getIssuedStatus()) {
                count++;
            }
        }
        return count;
    }

    void displayBooks() const {
        std::cout << "Library Books:" << std::endl;
        for (const auto& book : books) {
            std::cout << "ID: " << book.getID() << ", Title: " << book.getTitle() << ", Author: " << book.getAuthor();
            if (book.getIssuedStatus()) {
                std::cout << " (Issued - Due: " << book.getDueDate() << ")";
            }
            std::cout << std::endl;
        }
    }
};

class LibrarySystem {
public:
    void adminMenu(Library& library) {
        // ... (menu logic) ...
        int adminChoice;

        do {
            std::cout << "\nAdmin Menu\n";
            std::cout << "1. Add Book\n";
            std::cout << "2. Number of Available Books\n";
            std::cout << "3. Check for Fine\n";
            std::cout << "4. Back to Main Menu\n";
            std::cout << "Enter your choice: ";
            std::cin >> adminChoice;

            switch (adminChoice) {
                case 1:
                    addBook(library);
                    break;
                case 2:
                    displayAvailableBooksCount(library);
                    break;
                case 3:
                    checkFine(library);
                    break;
                case 4:
                    // Save data before returning
                    library.saveBooksToFile(); 
                    std::cout << "Returning to Main Menu..." << std::endl;
                    break;
                default:
                    std::cout << "Invalid choice. Please try again." << std::endl;
            }
        } while (adminChoice != 4);
    }

    void studentMenu(Library& library) {
        int studentChoice;

        do {
            std::cout << "\nStudent Menu\n";
            std::cout << "1. Search for Book\n";
            std::cout << "2. Issue Book\n";
            std::cout << "3. Return Book\n";
            std::cout << "4. Back to Main Menu\n";
            std::cout << "Enter your choice: ";
            std::cout << "\nEnter your choice: ";
            std::cin >> studentChoice;

            switch (studentChoice) {
                case 1:
                    searchForBook(library);
                    break;
                case 2:
                    issueBook(library);
                    break;
                case 3:
                    returnBook(library);
                    break;
                case 4:
                    // Save data before returning
                    library.saveBooksToFile(); 
                    std::cout << "Returning to Main Menu..." << std::endl;
                    break;
                default:
                    std::cout << "Invalid choice. Please try again." << std::endl;
            }
        } while (studentChoice != 4);
    }

private:
    void addBook(Library& library) {
        std::string title, author;
        int id;

        std::cout << "Enter Book Title: ";
        std::cin.ignore(); 
        std::getline(std::cin, title);

        std::cout << "Enter Author: ";
        std::getline(std::cin, author);

        std::cout << "Enter Book ID: ";
        std::cin >> id;

        Book newBook(title, author, id);
        library.addBook(newBook);

        std::cout << "Book added successfully." << std::endl;
    }

    void displayAvailableBooksCount(const Library& library) const {
        int count = library.getAvailableBooksCount();
        std::cout << "Number of Available Books: " << count << std::endl;
    }

    void checkFine(const Library& library) const {
        int id;
        std::cout << "Enter Book ID to check for fine: ";
        std::cin >> id;

        const Book* book = library.findBookById(id);
        if (book) {
            if (book->getIssuedStatus()) {
                std::cout << "Book Title: " << book->getTitle() << std::endl;
                std::cout << "Due Date: " << book->getDueDate() << std::endl;
                double fineAmount = book->calculateFine();
                if (fineAmount > 0.0) {
                    std::cout << "Fine Amount: $" << fineAmount << std::endl;
                } else {
                    std::cout << "No fines for this book." << std::endl;
                }
            } else {
                std::cout << "Book is not currently issued." << std::endl;
            }
        } else {
            std::cout << "Book not found." << std::endl;
        }
    }

    void searchForBook(const Library& library) const {
        int id;
        std::cout << "Enter Book ID: ";
        std::cin >> id;

        const Book* book = library.findBookById(id);
        if (book) {
            std::cout << "Book found: " << book->getTitle() << ", Author: " << book->getAuthor();
            if (book->getIssuedStatus()) {
                std::cout << " (Issued - Due: " << book->getDueDate() << ")";
            } else {
                std::cout << " (Available)";
            }
            std::cout << std::endl;
        } else {
            std::cout << "Book not found." << std::endl;
        }
    }

    void issueBook(Library& library) {
        int id;
        std::string dueDate;
        std::cout << "Enter Book ID to issue: ";
        std::cin >> id;
        std::cout << "Enter Due Date (YYYY-MM-DD): ";
        std::cin >> dueDate;
        library.issueBook(id, dueDate);
    }

    void returnBook(Library& library) {
        int id;
        std::cout << "Enter Book ID to return: ";
        std::cin >> id;
        library.returnBook(id);
    }
};

// --- Main Function (Updated) ---
int main() {
    Library library;
    LibrarySystem librarySystem;

    // Load existing data at startup
    library.loadBooksFromFile();

    
    if (library.getAvailableBooksCount() == 0) {
        library.addBook(Book("The Great Gatsby", "F. Scott Fitzgerald", 101));
        library.addBook(Book("To Kill a Mockingbird", "Harper Lee", 102));
        library.addBook(Book("1984", "George Orwell", 103));
        std::cout << "\nDefault books added for first run." << std::endl;
    }
   

    int mainChoice;

    do {
        std::cout << "\n=== Library Management System ===\n";
        std::cout << "1. Admin\n";
        std::cout << "2. Student\n";
        std::cout << "3. Display All Books\n";
        std::cout << "4. Exit\n";
        std::cout << "Enter your choice: ";
        std::cin >> mainChoice;

        switch (mainChoice) {
            case 1:
                librarySystem.adminMenu(library);
                break;
            case 2:
                librarySystem.studentMenu(library);
                break;
            case 3:
                library.displayBooks();
                break;
            case 4:
                // Save data one last time before exiting the program
                library.saveBooksToFile(); 
                std::cout << "Exiting..." << std::endl;
                break;
            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
        }
    } while (mainChoice != 4);

    return 0;
}
