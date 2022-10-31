-- MySQL
-- Create a table to hold student information including name, dob, link to picture, and student id
CREATE TABLE student (
    student_id VARCHAR(10) NOT NULL,
    name VARCHAR(50),
    dob DATE,
    picture VARCHAR(50) NOT NULL,
    PRIMARY KEY (student_id)
);