#include <iostream>
#include <limits> // para sa numeric limits
#include <mysql.h>
#include <string>
#include <iomanip> // this is for setw(each column has a fixed width), left, right para ma align ng maayos at hindi dikit dikit
#include <sstream> // ginagamit ito para sa: formatting reports, parsing input (kung may string na may multiple values)
#include <cctype> // access character handling functions that classify or transform characters,  such as checking if a character is a digit, letter, space, or converting between uppercase
#include <vector>
#include <ctime>
#include <conio.h>   //para sa _getch()
#include <algorithm> //para sa std::find
using namespace std;
#undef max //para to sa main program at borrow books yung code na may "max"


MYSQL* conn;
MYSQL_RES* res;
MYSQL_ROW row;

// Prototypes
bool loginAdmin();
bool loginMember(string& studentID, string& studentName);
bool validateStudentID(const string& id);
void createStudentAccount();
void adminMenu();
void memberMenu(string studentID, string studentName);
void displayBooks();
void borrowBook(string studentID, string studentName);
void returnBook(string studentID, string studentName);
void viewReports();
void viewOverdue();
void addBook();
void viewStudentHistory();  
void viewBorrowHistory(string studentID);
void logStudent(string student);


string inputPassword() {
    string password;
    char ch;
    while ((ch = _getch()) != '\r') {
        if (ch == '\b') {
            if (!password.empty()) {
                password.pop_back();
                cout << "\b \b";
            }
        } else {
            password.push_back(ch);
            cout << '*';
        }
    }
    cout << endl;
    return password;
}

bool loginMember(string& studentID, string& studentName) {
    cout << "Enter Student ID: ";
    getline(cin, studentID);

    cout << "Enter Password: ";
    string password = inputPassword();

    string query = "SELECT student_name FROM student_accounts "
                   "WHERE student_id='" + studentID + "' AND password='" + password + "'";
    if (mysql_query(conn, query.c_str()) == 0) {
        res = mysql_store_result(conn);
        row = mysql_fetch_row(res);
        if (row) {
            studentName = row[0];
            cout << "[OK] Member login successful! Welcome, " << studentName << "\n";
            logStudent(studentName);
            mysql_free_result(res);
            return true;
        }
        mysql_free_result(res);
    }
    cout << "[ERROR] Invalid student credentials.\n";
    return false;
}

bool loginAdmin() {
    string username, password; // masked input or yung parang nagiging asterisk

    cout << "Enter Admin Username: ";
    getline(cin, username);

    cout << "Enter Admin Password: ";
    password = inputPassword();

    string query =
        "SELECT * FROM admin_accounts "
        "WHERE username='" + username +
        "' AND password='" + password + "'";

    if (mysql_query(conn, query.c_str()) == 0) {
        res = mysql_store_result(conn);
        row = mysql_fetch_row(res);

        if (row) {
            cout << "[OK] Admin login successful!\n";
            mysql_free_result(res);
            return true;
        }

        mysql_free_result(res);
    }

    cout << "[ERROR] Invalid admin credentials.\n";
    return false;
}

// Connect sa database
bool connectDB() {
    conn = mysql_init(0);
    conn = mysql_real_connect(conn, "localhost", "root", "", "library_management_system", 3307, NULL, 0);
    if (conn) {
        cout << "[OK] Connected to database!\n";
        return true;
    } else {
        cout << "[ERROR] Connection failed: " << mysql_error(conn) << endl;
        return false;
    }
}

void createStudentAccount() {
    string name, password, studentID, yearLevel;

    cout << "Enter Student Name: ";
    getline(cin, name);

    while (true) {
        cout << "Enter Student ID (Format: CA202512345 ): ";
        getline(cin, studentID);

        if (!validateStudentID(studentID)) {
            cout << "[ERROR] Invalid Student ID format.\n";
            cout << "Format: CA + Year + 5 digits\n";
            cout << "Example: CA202512345\n";
            continue;
        }
        break;
    }

    while (true) {
        cout << "\n=== SELECT YEAR LEVEL ===\n";
        cout << "1. 1st Year IT\n";
        cout << "2. 2nd Year IT\n";
        cout << "3. 3rd Year IT\n";
        cout << "4. 4th Year IT\n";
        cout << "Choice: ";

        int choice;
        cin >> choice;
        cin.ignore();

        switch(choice) {
            case 1:
                yearLevel = "1st Year IT";
                break;

            case 2:
                yearLevel = "2nd Year IT";
                break;

            case 3:
                yearLevel = "3rd Year IT";
                break;

            case 4:
                yearLevel = "4th Year IT";
                break;

            default:
                cout << "[ERROR] Invalid Choice.\n";
                continue;
        }

        break;
    }

    cout << "Enter Password: ";
    password = inputPassword();

    string query =
        "INSERT INTO student_accounts "
        "(student_id, student_name, year_level, password) "
        "VALUES('"
        + studentID + "','"
        + name + "','"
        + yearLevel + "','"
        + password + "')";

    if (mysql_query(conn, query.c_str()) == 0) {
        cout << "[OK] Student account created successfully!\n";
    }
    else {
        cout << "[ERROR] Failed to create account: "
             << mysql_error(conn) << endl;
    }
}


string getCurrentDateTime() {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    char buf[20];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M", ltm);
    return string(buf);
}
string getDueDate(int days) {
    time_t now = time(0) + days*24*60*60;
    tm *ltm = localtime(&now);
    char buf[20];
    strftime(buf, sizeof(buf), "%Y-%m-%d", ltm);
    return string(buf);
}

void displayBooks() {
    if (mysql_query(conn, "SELECT book_id, title, quantity FROM book_inventories") == 0) {
        res = mysql_store_result(conn);
        cout << "\n--- Available Books ---\n";
        cout << left << setw(5) << "ID" << setw(45) << "Title" << setw(10) << "Quantity" << endl;
        cout << string(65, '-') << endl;
        while ((row = mysql_fetch_row(res))) {
            cout << left << setw(5) << row[0] << setw(45) << row[1] << setw(10) << row[2] << endl;
        }
        mysql_free_result(res);
    }
}



bool validateStudentID(const string& id) {
    if (id.size() != 11) return false;
    if (id.substr(0,2) != "CA") return false;

    string yearStr = id.substr(2,4);
    for (char c : yearStr) if (!isdigit(c)) return false;
    int year = stoi(yearStr);
    if (year < 2022) return false;

    for (int i = 6; i < 11; i++) {
        if (!isdigit(id[i])) return false;
    }
    return true;
}



// log student entry
void logStudent(string student) {
    string query = "INSERT INTO student_logs (student_name, entry_date, entry_time) VALUES('" 
                   + student + "', CURDATE(), CURTIME())";
    mysql_query(conn, query.c_str());
}

// Admin Menu
void adminMenu() {
    int choice;
    do {
        cout << "\n=== Admin Menu ===\n";
        cout << "1. Display Books\n";
        cout << "2. Add Books\n";
        cout << "3. View Reports\n";
        cout << "4. View Student History\n";
        cout << "5. View Overdue Books\n";
        cout << "6. Back to Main Menu\n";
        cout << "Choice: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1: 
                displayBooks(); 
                break;
            case 2: 
                addBook(); 
                break;
            case 3: 
                viewReports(); 
                break;
            case 4: 
                viewStudentHistory(); 
                break;
            case 5: 
                viewOverdue(); 
                break;
            case 6: 
                cout << "Returning to Main Menu...\n"; 
                break;
            default: 
                cout << "[ERROR] Invalid choice.\n"; 
                break;
        }
    } while (choice != 6);
}


// Member Menu
void memberMenu(string studentID, string studentName) {
    int choice;
    do {
        cout << "\n=== Member Menu ===\n";
        cout << "1. Display Books\n";
        cout << "2. Borrow Book\n";
        cout << "3. Return Book\n";
        cout << "4. View Borrow History\n";
        cout << "5. Back to Main Menu\n";
        cout << "Choice: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1: 
                displayBooks(); 
                break;
            case 2: 
                borrowBook(studentID, studentName); 
                break;
            case 3: 
                returnBook(studentID, studentName); 
                break;
            case 4: 
                viewBorrowHistory(studentID); 
                break;
            case 5: 
                cout << "Returning to Main Menu...\n"; 
                break;
            default: 
                cout << "[ERROR] Invalid choice.\n"; 
                break;
        }
    } while (choice != 5);
}

void viewBorrowHistory(string studentID) {
    string query = "SELECT b.book_id, b.title, r.due_date "
                   "FROM borrowing_records r "
                   "JOIN book_inventories b ON r.book_id=b.book_id "
                   "WHERE r.student_id='" + studentID + "'";

    if (mysql_query(conn, query.c_str()) == 0) {
        res = mysql_store_result(conn);
        cout << "\n--- Borrow History ---\n";
        cout << left << setw(10) << "Book ID"
             << setw(40) << "Title"
             << setw(15) << "Due Date"
             << setw(40) << "Reminder" << endl;
        cout << string(105, '-') << endl;

        while ((row = mysql_fetch_row(res))) {
            cout << left << setw(10) << row[0]
                 << setw(40) << row[1]
                 << setw(15) << row[2]
                 << setw(40) << "Penalty P75/day overdue, P500 if lost/damaged" << endl;
        }

        mysql_free_result(res);
    } else {
        cout << "[ERROR] Failed to fetch borrow history: " << mysql_error(conn) << endl;
    }
}

void addPenalty(
    string studentID,
    string studentName,
    int bookID)
{
    int choice;

    cout << "\nPenalty Type\n";
    cout << "1. Lost Book\n";
    cout << "2. Damaged Book\n";
    cout << "Choice: ";
    cin >> choice;
    cin.ignore();

    string type;

    if(choice == 1)
        type = "Lost";
    else if(choice == 2)
        type = "Damaged";
    else
        return;

    string query =
    "INSERT INTO penalty_records "
    "(student_id, student_name, book_id, penalty_type, penalty_amount)"
    " VALUES('"
    + studentID + "','"
    + studentName + "',"
    + to_string(bookID) +
    ",'" + type + "',500)";

    mysql_query(conn, query.c_str());

    cout << "[OK] Penalty Recorded.\n";
}


// return Book
void returnBook(string studentID, string studentName) {
    string input;
    cout << "Enter Book IDs to return (e.g. 1,2,3): ";
    getline(cin, input);

    stringstream ss(input);
    string bookIDStr;
    vector<int> bookIDs;
    vector<string> bookTitles;
    vector<string> dueDates;
    vector<bool> isBorrowed;

    while (getline(ss, bookIDStr, ',')) {
        bookIDStr.erase(0, bookIDStr.find_first_not_of(" \t"));
        bookIDStr.erase(bookIDStr.find_last_not_of(" \t") + 1);
        if (bookIDStr.empty()) continue;
        int bookID = stoi(bookIDStr);

        string title = "No record of borrowing this book";
        string due = "-";
        bool borrowed = false;

        string check = "SELECT b.title, r.due_date FROM borrowing_records r "
                       "JOIN book_inventories b ON r.book_id=b.book_id "
                       "WHERE r.book_id=" + to_string(bookID) + " AND r.student_id='" + studentID + "'";
        if (mysql_query(conn, check.c_str()) == 0) {
            MYSQL_RES* res = mysql_store_result(conn);
            MYSQL_ROW row = mysql_fetch_row(res);
            if (row) {
                title = row[0];
                due = row[1];
                borrowed = true;
            }
            mysql_free_result(res);
        }

        bookIDs.push_back(bookID);
        bookTitles.push_back(title);
        dueDates.push_back(due);
        isBorrowed.push_back(borrowed);
    }

    if (bookIDs.empty()) {
        cout << "[INFO] No Book IDs entered.\n";
        return;
    }

    cout << "\n--- Return Details ---\n";
    cout << left << setw(10) << "Book ID"
         << setw(40) << "Title/Status"
         << setw(20) << "Return Date"
         << setw(15) << "Due Date"
         << setw(40) << "Penalty Fee" << endl;
    cout << string(130, '-') << endl;

    bool anyBorrowed = false;
    for (size_t i = 0; i < bookIDs.size(); i++) {
        if (isBorrowed[i]) {
            anyBorrowed = true;
            cout << left << setw(10) << bookIDs[i]
                 << setw(40) << bookTitles[i]
                 << setw(20) << getCurrentDateTime()
                 << setw(15) << dueDates[i]
                 << "Penalty P75/day overdue, P500 if lost/damaged" << endl;
        } else {
            cout << left << setw(10) << bookIDs[i]
                 << setw(40) << "No record of borrowing this book"
                 << setw(20) << "-"
                 << setw(15) << "-"
                 << "-" << endl;
        }
    }

    if (!anyBorrowed) {
        cout << "\n[INFO] No borrowed books found. Returning to Member Menu...\n";
        return;
    }

    cout << "\nReminder: If the book is lost or damaged, you will pay P500.\n";
    cout << "Press ENTER to proceed with returning, or any other key to cancel.\n";

    char confirm = cin.get();
    if (confirm == '\n') {
        for (size_t i = 0; i < bookIDs.size(); i++) {
            if (!isBorrowed[i]) continue;

            string del = "DELETE FROM borrowing_records WHERE book_id=" + to_string(bookIDs[i]) +
                         " AND student_id='" + studentID + "'";
            mysql_query(conn, del.c_str());

            string update = "UPDATE book_inventories SET quantity=quantity+1 WHERE book_id=" + to_string(bookIDs[i]);
            mysql_query(conn, update.c_str());

            string ret = "INSERT INTO return_records (book_id, student_id, student_name, return_date) VALUES(" 
                         + to_string(bookIDs[i]) + ",'" + studentID + "','" + studentName + "', CURDATE())";
            mysql_query(conn, ret.c_str());

            // penalty computation per day overdue
            string penaltyCheck = "SELECT DATEDIFF(CURDATE(), '" + dueDates[i] + "')";
            if (mysql_query(conn, penaltyCheck.c_str()) == 0) {
                MYSQL_RES* res2 = mysql_store_result(conn);
                MYSQL_ROW row2 = mysql_fetch_row(res2);
                if (row2) {
                    int daysLate = stoi(row2[0]);
                    if (daysLate > 0) {
                        int penaltyAmount = daysLate * 75;
                        string penalty = "INSERT INTO penalty_records (student_id, student_name, book_id, penalty_type, penalty_amount) "
                                         "VALUES('" + studentID + "', '" + studentName + "', " + to_string(bookIDs[i]) + ", 'Overdue', " + to_string(penaltyAmount) + ")";
                        mysql_query(conn, penalty.c_str());
                    }
                }
                mysql_free_result(res2);
            }
        }
        cout << "[OK] Books returned successfully!\n";
    } else {
        cout << "[INFO] Return cancelled. Returning to Member Menu...\n";
    }
}

// vborrow multiple books
void borrowBook(string studentID, string studentName) {
    string yearLevel;

while(true)
{
    cout << "\n=== SELECT YEAR LEVEL ===\n";
    cout << "1. 1st Year IT\n";
    cout << "2. 2nd Year IT\n";
    cout << "3. 3rd Year IT\n";
    cout << "4. 4th Year IT\n";
    cout << "Choice: ";

    int choice;
    cin >> choice;
    cin.ignore();

    switch(choice)
    {
        case 1:
            yearLevel = "1st Year IT";
            break;

        case 2:
            yearLevel = "2nd Year IT";
            break;

        case 3:
            yearLevel = "3rd Year IT";
            break;

        case 4:
            yearLevel = "4th Year IT";
            break;

        default:
            cout << "[ERROR] Invalid Choice. Please select a valid Year Level\n";
            continue;
    }

    break;
}

    string query = "SELECT book_id, title, quantity FROM book_inventories WHERE year_level='" + yearLevel + "'";
    if (mysql_query(conn, query.c_str()) == 0) {
        res = mysql_store_result(conn);
        cout << "\n--- " << yearLevel << " Books ---\n";
        cout << left << setw(10) << "Book ID" << setw(40) << "Title" << setw(10) << "Quantity" << endl;
        cout << string(60, '-') << endl;
        while ((row = mysql_fetch_row(res))) {
            cout << left << setw(10) << row[0] << setw(40) << row[1] << setw(10) << row[2] << endl;
        }
        mysql_free_result(res);
    }

    string input;
    cout << "Enter Book IDs to borrow (e.g. 1,2,3): ";
    getline(cin, input);

    stringstream ss(input);
    string bookIDStr;
    vector<int> bookIDs;
    vector<string> bookTitles;

    while (getline(ss, bookIDStr, ',')) {
        bookIDStr.erase(0, bookIDStr.find_first_not_of(" \t"));
        bookIDStr.erase(bookIDStr.find_last_not_of(" \t") + 1);
        if (bookIDStr.empty()) continue;
        int bookID = stoi(bookIDStr);

        // para maprevent duplicate entry
        if (std::find(bookIDs.begin(), bookIDs.end(), bookID) != bookIDs.end()) {
            cout << "[INFO] Duplicate Book ID " << bookID << " skipped.\n";
            continue;
        }

        // Check kung already borrowed na ang books
        string checkBorrow = "SELECT record_id FROM borrowing_records WHERE book_id=" + to_string(bookID) +
                             " AND student_id='" + studentID + "'";
        if (mysql_query(conn, checkBorrow.c_str()) == 0) {
            MYSQL_RES* resCheck = mysql_store_result(conn);
            MYSQL_ROW rowCheck = mysql_fetch_row(resCheck);
            if (rowCheck) {
                cout << "[INFO] You already borrowed Book ID " << bookID << ". Skipped.\n";
                mysql_free_result(resCheck);
                continue;
            }
            mysql_free_result(resCheck);
        }

        // Check availability ng books
        string check = "SELECT title, quantity FROM book_inventories WHERE book_id=" + to_string(bookID);
        mysql_query(conn, check.c_str());
        res = mysql_store_result(conn);
        row = mysql_fetch_row(res);
        if (!row) { mysql_free_result(res); continue; }
        string title = row[0]; int qty = stoi(row[1]); mysql_free_result(res);
        if (qty <= 0) { cout << "[INFO] Book unavailable.\n"; continue; }

        bookIDs.push_back(bookID);
        bookTitles.push_back(title);
    }

    if (bookIDs.empty()) {
        cout << "[INFO] No valid books to borrow. Returning to Member Menu...\n";
        return;
    }

    cout << "\n--- Borrow Details ---\n";
    cout << left << setw(10) << "Book ID"
         << setw(40) << "Title"
         << setw(20) << "Borrow Date"
         << setw(15) << "Due Date"
         << setw(40) << "Penalty Fee" << endl;
    cout << string(130, '-') << endl;

    for (size_t i = 0; i < bookIDs.size(); i++) {
        cout << left << setw(10) << bookIDs[i]
             << setw(40) << bookTitles[i]
             << setw(20) << getCurrentDateTime()
             << setw(15) << getDueDate(7)
             << "Penalty P75/day overdue, P500 if lost/damaged" << endl;
    }

    cout << "\nReminder: If the book is lost or damaged, you will pay P500.\n";
    cout << "You can pay it in the portal: https://sms.icct.edu.ph/\n";
    cout << "Press ENTER to proceed with borrowing, or any other key to cancel and return to the Member Menu.\n";

        char confirm;
        cin >> noskipws >> confirm;  
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');


    if (confirm == '\n') {
        for (size_t i = 0; i < bookIDs.size(); i++) {
            string borrow = "INSERT INTO borrowing_records (book_id, student_id, student_name, borrow_date, due_date) VALUES(" 
                + to_string(bookIDs[i]) + ",'" + studentID + "','" + studentName + "', CURDATE(), DATE_ADD(CURDATE(), INTERVAL 7 DAY))";
            mysql_query(conn, borrow.c_str());
            string update = "UPDATE book_inventories SET quantity=quantity-1 WHERE book_id=" + to_string(bookIDs[i]);
            mysql_query(conn, update.c_str());
        }
        cout << "[OK] Books borrowed successfully!\n";
    } else {
        cout << "[INFO] Borrow cancelled. Returning to Member Menu...\n";
    }
}



//dito makakapag add ang admin ng new book
    void addBook() {
    char confirm;
    cout << "\nDo you want to add a new book? (Y/N): ";
    cin >> confirm;
    cin.ignore();
    confirm = toupper(confirm);

    if (confirm != 'Y') {
        cout << "[INFO] Add Book cancelled. Returning to Admin Menu...\n";
        return; // balik agad sa admin menu
    }

    string title, yearLevel;
    int quantity;

    cout << "Enter Book Title: ";
    getline(cin, title);

    cout << "Select Year Level (1st Year IT, 2nd Year IT, 3rd Year IT, 4th Year IT): ";
    getline(cin, yearLevel);

    cout << "Enter Quantity: ";
    cin >> quantity;
    cin.ignore();

    string query = "INSERT INTO book_inventories (title, year_level, quantity) VALUES('" 
                   + title + "','" + yearLevel + "'," + to_string(quantity) + ")";
    if (mysql_query(conn, query.c_str()) == 0) {
        cout << "[OK] Book added successfully!\n";
    } else {
        cout << "[ERROR] Failed to add book: " << mysql_error(conn) << endl;
    }
}


// Vbiew borrowing reports
void viewReports() {
    
string borrowQuery =
    "SELECT r.record_id, b.title, s.student_id, r.student_name, r.borrow_date, r.due_date "
    "FROM borrowing_records r "
    "JOIN book_inventories b ON r.book_id = b.book_id "
    "JOIN student_accounts s ON r.student_id = s.student_id";

if (mysql_query(conn, borrowQuery.c_str()) == 0) {
    res = mysql_store_result(conn);
    cout << "\n--- Borrowing Records ---\n";
cout << left << setw(5)  << "ID"
     << setw(40) << "Title"
     << setw(15) << "Student ID"
     << setw(20) << "Student"
     << setw(15) << "Borrow Date"
     << setw(15) << "Due Date" << endl;
cout << string(115, '-') << endl;

while ((row = mysql_fetch_row(res))) {
    cout << left << setw(5)  << row[0]
         << setw(40) << row[1]
         << setw(15) << row[2]
         << setw(20) << row[3]
         << setw(15) << row[4]
         << setw(15) << row[5] << endl;
}

    mysql_free_result(res);
} else {
    cout << "[ERROR] Failed to fetch borrowing records: " << mysql_error(conn) << endl;
}


   //return records
string returnQuery =
    "SELECT r.return_id, b.title, s.student_id, r.student_name, r.return_date "
    "FROM return_records r "
    "JOIN book_inventories b ON r.book_id = b.book_id "
    "JOIN student_accounts s ON r.student_id = s.student_id";

if (mysql_query(conn, returnQuery.c_str()) == 0) {
    res = mysql_store_result(conn);
    cout << "\n--- Return Records ---\n";
    cout << left << setw(5)  << "ID"
         << setw(40) << "Title"
         << setw(15) << "Student ID"
         << setw(20) << "Student"
         << setw(15) << "Return Date" << endl;
    cout << string(115, '-') << endl;

    while ((row = mysql_fetch_row(res))) {
        cout << left << setw(5)  << row[0]
             << setw(40) << row[1]
             << setw(15) << row[2]
             << setw(20) << row[3]
             << setw(15) << row[4] << endl;
    }
    mysql_free_result(res);
}

cout << "\nPress Enter to return to Admin Menu...";
cin.ignore();
cin.get();
}

void viewStudentHistory() {
    string studentID;
    cout << "Enter Student ID to view history: ";
    getline(cin, studentID);

    string query = "SELECT s.student_id, s.student_name, s.year_level, b.book_id, b.title, r.borrow_date, r.due_date "
                   "FROM borrowing_records r "
                   "JOIN student_accounts s ON r.student_id = s.student_id "
                   "JOIN book_inventories b ON r.book_id = b.book_id "
                   "WHERE s.student_id='" + studentID + "'";

    if (mysql_query(conn, query.c_str()) == 0) {
        res = mysql_store_result(conn);
        cout << "\n--- Student History ---\n";
        cout << left << setw(15) << "Student ID"
             << setw(20) << "Name"
             << setw(15) << "Year Level"
             << setw(10) << "Book ID"
             << setw(40) << "Title"
             << setw(15) << "Borrow Date"
             << setw(15) << "Due Date" << endl;
        cout << string(130, '-') << endl;

        while ((row = mysql_fetch_row(res))) {
            cout << left << setw(15) << row[0]
                 << setw(20) << row[1]
                 << setw(15) << row[2]
                 << setw(10) << row[3]
                 << setw(40) << row[4]
                 << setw(15) << row[5]
                 << setw(15) << row[6] << endl;
        }
        mysql_free_result(res);
    }
}




// dito para ma-view ang overdue books
        void viewOverdue() {
            string query =
            "SELECT r.record_id, "
            "r.book_id, "
            "b.title, "
            "s.student_id, "
            "r.student_name, "
            "r.borrow_date, "
            "r.due_date, "
            "DATEDIFF(CURDATE(), r.due_date) * 75 AS penalty "
            "FROM borrowing_records r "
            "JOIN student_accounts s ON r.student_id=s.student_id "
            "JOIN book_inventories b ON r.book_id=b.book_id "
            "WHERE r.due_date < CURDATE()";

    if (mysql_query(conn, query.c_str()) == 0) {
        res = mysql_store_result(conn);
        cout << "\n--- Overdue Books ---\n";
        cout << left << setw(5)  << "ID"
             << setw(10) << "BookID"
             << setw(40) << "Title"
             << setw(15) << "Student ID"
             << setw(20) << "Student"
             << setw(20) << "Borrow Date"
             << setw(15) << "Due Date"
             << setw(10) << "Penalty" << endl;
        cout << string(135, '-') << endl;

        int totalPenalty = 0;
        while ((row = mysql_fetch_row(res))) {
            cout << left << setw(5)  << row[0]
                 << setw(10) << row[1]
                 << setw(40) << row[2]
                 << setw(15) << row[3]
                 << setw(20) << row[4]
                 << setw(20) << row[5]
                 << setw(15) << row[6]
                 << "P" << row[7] << endl;
                totalPenalty += stoi(row[7]);
        }
        cout << "\nTotal Penalty Fee: P" << totalPenalty << endl;
        mysql_free_result(res);
    } else {
        cout << "[ERROR] Failed to fetch overdue books: " << mysql_error(conn) << endl;
    }
}


// main program or Menu
int main() {
    // Connect to DB
    conn = mysql_init(0);
    conn = mysql_real_connect(conn, "localhost", "root", "", "library_management_system", 3307, NULL, 0);
    if (!conn) {
        cout << "[ERROR] Connection failed: " << mysql_error(conn) << endl;
        return 0;
    }
    cout << "[OK] Connected to database!\n";

    int choice;
    string studentID, studentName;

    do {
        cout << "\n=== Main Menu ===\n";
        cout << "1. Create Student Account\n";
        cout << "2. Login as Admin\n";
        cout << "3. Login as Member\n";
        cout << "4. Exit\n";
        cout << "Choice: ";
        cin >> choice;

        // error handling kapag mali ang input
        if (cin.fail()) {
            cin.clear(); 
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  //kaya meron nito para bumalik sa loop (sa main menu program)
            cout << "[ERROR] Invalid input. Please enter a number.\n";
            continue;
        }

        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
            case 1: 
                createStudentAccount(); 
                break;
            case 2: 
                if (loginAdmin()) adminMenu(); 
                break;
            case 3: 
                if (loginMember(studentID, studentName)) memberMenu(studentID, studentName); 
                break;
            case 4: 
                cout << "Exiting program...\n"; 
                break;
            default: 
                cout << "[ERROR] Invalid choice.\n"; 
                break;
        }
    } while (choice != 4);

    mysql_close(conn);
    return 0;
}








