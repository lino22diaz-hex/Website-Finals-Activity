-- Create database
DROP DATABASE IF EXISTS library_management_system;
CREATE DATABASE library_management_system;
USE library_management_system;

-- Student Accounts
CREATE TABLE student_accounts (
    student_id VARCHAR(20) PRIMARY KEY,
    student_name VARCHAR(100) NOT NULL,
    year_level ENUM(
        '1st Year IT',
        '2nd Year IT',
        '3rd Year IT',
        '4th Year IT'
    ) NOT NULL,
    password VARCHAR(100) NOT NULL
);

-- Student Logs
CREATE TABLE student_logs (
    log_id INT AUTO_INCREMENT PRIMARY KEY,
    student_name VARCHAR(100),
    entry_date DATE,
    entry_time TIME
);

-- Book Inventories
CREATE TABLE book_inventories (
    book_id INT AUTO_INCREMENT PRIMARY KEY,
    title VARCHAR(200) NOT NULL,
    year_level ENUM(
        '1st Year IT',
        '2nd Year IT',
        '3rd Year IT',
        '4th Year IT'
    ) NOT NULL,
    quantity INT NOT NULL DEFAULT 0
);

-- Borrowing Records
CREATE TABLE borrowing_records (
    record_id INT AUTO_INCREMENT PRIMARY KEY,
    book_id INT NOT NULL,
    student_id VARCHAR(20) NOT NULL,
    student_name VARCHAR(100) NOT NULL,
    borrow_date DATE NOT NULL,
    due_date DATE NOT NULL,

    FOREIGN KEY (book_id)
        REFERENCES book_inventories(book_id),

    FOREIGN KEY (student_id)
        REFERENCES student_accounts(student_id)
);

-- Return Records
CREATE TABLE return_records (
    return_id INT AUTO_INCREMENT PRIMARY KEY,
    book_id INT NOT NULL,
    student_id VARCHAR(20) NOT NULL,
    student_name VARCHAR(100) NOT NULL,
    return_date DATE NOT NULL,

    FOREIGN KEY (book_id)
        REFERENCES book_inventories(book_id),

    FOREIGN KEY (student_id)
        REFERENCES student_accounts(student_id)
);

-- Penalty Records
CREATE TABLE penalty_records (
    penalty_id INT AUTO_INCREMENT PRIMARY KEY,
    student_id VARCHAR(20) NOT NULL,
    student_name VARCHAR(100) NOT NULL,
    book_id INT NOT NULL,

    penalty_type ENUM(
        'Overdue',
        'Lost',
        'Damaged'
    ) NOT NULL,

    penalty_amount DECIMAL(10,2) NOT NULL,

    penalty_date DATE DEFAULT (CURRENT_DATE),

    FOREIGN KEY (book_id)
        REFERENCES book_inventories(book_id),

    FOREIGN KEY (student_id)
        REFERENCES student_accounts(student_id)
);

-- Admin Accounts
CREATE TABLE admin_accounts (
    admin_id INT AUTO_INCREMENT PRIMARY KEY,
    username VARCHAR(50) UNIQUE NOT NULL,
    password VARCHAR(100) NOT NULL
);

-- Default Admin
INSERT INTO admin_accounts(username,password)
VALUES
('admin','12345678');

--------------------------------------------------
-- 1st Year IT Books
--------------------------------------------------

INSERT INTO book_inventories(title,year_level,quantity) VALUES
('Communication & Soft Skills','1st Year IT',250),
('Computer Networking Fundamentals','1st Year IT',250),
('Fundamentals of Information Technology','1st Year IT',250),
('Computer Organization and Architecture','1st Year IT',250),
('Data Structures through C','1st Year IT',250),
('Programming in C','1st Year IT',250),
('Visual Programming and Web Technology','1st Year IT',250);

--------------------------------------------------
-- 2nd Year IT Books
--------------------------------------------------

INSERT INTO book_inventories(title,year_level,quantity) VALUES
('Programming in C++','2nd Year IT',250),
('Relational Database Management System','2nd Year IT',250),
('Software Testing','2nd Year IT',250),
('Core Java','2nd Year IT',250),
('Operating System','2nd Year IT',250),
('Algorithm Design and Optimization','2nd Year IT',250),
('Data Mining','2nd Year IT',250);

--------------------------------------------------
-- 3rd Year IT Books
--------------------------------------------------

INSERT INTO book_inventories(title,year_level,quantity) VALUES
('Advanced Java','3rd Year IT',250),
('Project Management','3rd Year IT',250),
('Information Theory and Digital Electronics','3rd Year IT',250),
('Data Communication','3rd Year IT',250),
('Management Information Systems','3rd Year IT',250),
('.NET Technologies','3rd Year IT',250),
('SQL-Server Database','3rd Year IT',250);

--------------------------------------------------
-- 4th Year IT Books
--------------------------------------------------

INSERT INTO book_inventories(title,year_level,quantity) VALUES
('Artificial Intelligence','4th Year IT',250),
('Computer Ethics and Cyber Law','4th Year IT',250),
('Computer Graphics','4th Year IT',250),
('Distributed Systems','4th Year IT',250),
('Microprocessor Systems','4th Year IT',250),
('Multimedia','4th Year IT',250),
('Software Engineering','4th Year IT',250),
('Web Development','4th Year IT',250),
('E-Commerce','4th Year IT',250);