#include <iostream>
#include <string>
#include <vector>
#include <ctime>

std::string getCurrentDate() {
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d", &tstruct);
    return buf;
}
int calculateOverdueDays(const std::string& currentDate, const std::string& dueDate) {
    return (dueDate > currentDate) ? 0 : (std::stoi(currentDate.substr(8, 2)) - std::stoi(dueDate.substr(8, 2)));
}

class Book {
public:
    Book(const std::string& title, const std::string& author, int id)
        : title(title), author(author), id(id), isIssued(false) {}

    const std::string& getTitle() const { return title; }
    const std::string& getAuthor() const { return author; }
    int getID() const { return id; }
    bool getIssuedStatus() const { return isIssued; }
    void setIsIssued(bool status) { isIssued = status; }

    void setDueDate(const std::string& dueDate) { this->dueDate = dueDate; }
    const std::string& getDueDate() const { return dueDate; }

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

private:
    std::string title;
    std::string author;
    int id;
    bool isIssued;
    std::string dueDate;
};


class Library {
public:
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

    void issueBook(int id, const std::string& dueDate) {
        Book* book = const_cast<Book*>(findBookById(id)); 
        if (book && !book->getIssuedStatus()) {
            book->setIsIssued(true);
            book->setDueDate(dueDate);  
            std::cout << "Book issued: " << book->getTitle() << std::endl;
        } else {
            std::cout << "Book not found or already issued." << std::endl;
        }
    }

    void returnBook(int id) {
        Book* book = const_cast<Book*>(findBookById(id)); 
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
                std::cout << " (Issued)";
            }
            std::cout << std::endl;
        }
    }

private:
    std::vector<Book> books;
};

class LibrarySystem {
public:
    void adminMenu(Library& library) {
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
            std::cout << "Book found: " << book->getTitle() << ", Author: " << book->getAuthor() << std::endl;
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

int main() {
    Library library;
    LibrarySystem librarySystem;

    library.addBook(Book("The Great Gatsby", "F. Scott Fitzgerald", 101));
    library.addBook(Book("To Kill a Mockingbird", "Harper Lee", 102));
    library.addBook(Book("1984", "George Orwell", 103));

    int mainChoice;

    do {
        std::cout << "\nLibrary Management System\n";
        std::cout << "1. Admin\n";
        std::cout << "2. Student\n";
        std::cout << "3. Exit\n";
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
                std::cout << "Exiting..." << std::endl;
                break;
            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
        }
    } while (mainChoice != 3);

    return 0;
}
