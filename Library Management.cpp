#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <ctime>
using namespace std;

struct Book {
    string title;
    string author;
    bool available;
    string issuedTo;
    time_t dueDate;
    double price;
};

map<string, string> users;
map<string, string> roles;
vector<Book> books;
map<string, vector<Book>> issuedBooks;
map<string, double> fines;

const double FINE_PER_DAY = 5.0;

void loadData() {
    ifstream userFile("users.txt");
    if (userFile.is_open()) {
        string username, password, role;
        while (userFile >> username >> password >> role) {
            users[username] = password;
            roles[username] = role;
        }
        userFile.close();
    }

    ifstream bookFile("books.txt");
    if (bookFile.is_open()) {
        Book book;
        while (bookFile >> book.title >> book.author >> book.available >> book.issuedTo >> book.dueDate >> book.price) {
            books.push_back(book);
        }
        bookFile.close();
    }
}

void saveData() {
    ofstream bookFile("books.txt");
    for (auto& book : books) {
        bookFile << book.title << " " << book.author << " " << book.available << " "
                 << book.issuedTo << " " << book.dueDate << " " << book.price << endl;
    }
    bookFile.close();
}

void searchBooks() {
    string keyword;
    cout << "Enter book title or author to search: ";
    cin.ignore();
    getline(cin, keyword);
    
    cout << "\n--- Search Results ---" << endl;
    for (auto& book : books) {
        if (book.title.find(keyword) != string::npos || book.author.find(keyword) != string::npos) {
            cout << "Title: " << book.title << ", Author: " << book.author
                 << ", Status: " << (book.available ? "Available" : "Borrowed by " + book.issuedTo) << endl;
        }
    }
}

void viewFines(string username) {
    cout << "Your current fine: $" << fines[username] << endl;
}

void payFine(string username) {
    double amount;
    cout << "Enter amount to pay: ";
    cin >> amount;
    if (amount >= fines[username]) {
        fines[username] = 0;
        cout << "Fine cleared!" << endl;
    } else {
        fines[username] -= amount;
        cout << "Remaining fine: $" << fines[username] << endl;
    }
}

void calculateFine(string username) {
    time_t now = time(0);
    for (auto& book : issuedBooks[username]) {
        if (book.dueDate < now) {
            int daysOverdue = (now - book.dueDate) / (24 * 60 * 60);
            fines[username] += daysOverdue * FINE_PER_DAY;
        }
    }
}

void borrowOrBuyBook(string username) {
    string title;
    int option;
    cout << "Enter the title of the book: ";
    cin.ignore();
    getline(cin, title);
    
    for (auto& book : books) {
        if (book.title == title) {
            cout << "Do you want to: 1. Borrow  2. Buy? Enter choice: ";
            cin >> option;
            if (option == 1 && book.available) {
                book.available = false;
                book.issuedTo = username;
                book.dueDate = time(0) + (7 * 24 * 60 * 60);
                issuedBooks[username].push_back(book);
                cout << "You have borrowed: " << book.title << " (Return by: " << ctime(&book.dueDate) << ")" << endl;
                saveData();
            } else if (option == 2) {
                cout << "Book purchased successfully! Price: $" << book.price << endl;
            } else {
                cout << "Invalid choice or book not available!" << endl;
            }
            return;
        }
    }
    cout << "Book not found!" << endl;
}

void adminDashboard() {
    int choice;
    while (true) {
        cout << "\n--- Admin Dashboard ---" << endl;
        cout << "1. View Books" << endl;
        cout << "2. Add Book" << endl;
        cout << "3. Logout" << endl;
        cout << "Enter choice: ";
        cin >> choice;
        if (choice == 1) searchBooks();
        else if (choice == 2) {
            Book newBook;
            cout << "Enter Book Title: ";
            cin.ignore();
            getline(cin, newBook.title);
            cout << "Enter Author Name: ";
            getline(cin, newBook.author);
            cout << "Enter Price: ";
            cin >> newBook.price;
            newBook.available = true;
            newBook.issuedTo = "";
            newBook.dueDate = 0;
            books.push_back(newBook);
            saveData();
            cout << "Book added successfully!" << endl;
        }
        else if (choice == 3) break;
        else cout << "Invalid choice!" << endl;
    }
}

void memberMenu(string username) {
    int choice;
    while (true) {
        cout << "\n1. Search Books" << endl;
        cout << "2. View Fines" << endl;
        cout << "3. Pay Fine" << endl;
        cout << "4. Borrow or Buy Book" << endl;
        cout << "5. Logout" << endl;
        cout << "Enter choice: ";
        cin >> choice;
        if (choice == 1) searchBooks();
        else if (choice == 2) viewFines(username);
        else if (choice == 3) payFine(username);
        else if (choice == 4) borrowOrBuyBook(username);
        else if (choice == 5) break;
        else cout << "Invalid choice!" << endl;
    }
}

int main() {
    int choice;
    string currentUser;
    bool loggedIn = false;
    loadData();

    while (true) {
        cout << "\n--- Library System ---" << endl;
        cout << "1. Login" << endl;
        cout << "2. Exit" << endl;
        cout << "Enter choice: ";
        cin >> choice;
        
        if (choice == 1) {
            cout << "Enter username: ";
            cin >> currentUser;
            if (roles.find(currentUser) != roles.end() && roles[currentUser] == "admin") {
                adminDashboard();
            } else {
                cout << "Welcome, " << currentUser << "!" << endl;
                calculateFine(currentUser);
                memberMenu(currentUser);
            }
        } else if (choice == 2) {
            cout << "Exiting system. Goodbye!" << endl;
            return 0;
        } else {
            cout << "Invalid choice!" << endl;
        }
    }
    return 0;
}
