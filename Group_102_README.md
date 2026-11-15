<!-- group-102 -->

# Group ID: 102
# AcadERP-Lite
## Academic Records & Graduation Audit System

OOPD Project – 2 Credits

Group Members : Maurice Zafar (MT26171), Farman Malik (MT26160)

1. About the Project
AcadERP-Lite is a small command-line academic record management system developed in C++11. It keeps track of students, courses, registrations and grades, and uses this information to answer academic queries such as GPA calculation, average grade, course eligibility and graduation status.

The program works by reading one command per line from standard input. Commands can be used to add students and courses, register students for courses, submit grades and check academic progress.

While implementing the system, we focused on keeping the data organised, validating commands before modifying records, and handling errors without terminating the program. The implementation uses classes, fixed-size arrays, pointers and manual memory management, as required by the project specification.

2. Features Implemented
The following functionality is supported:

Course Management: Add regular, satisfactory (SU) and thesis courses with their credits, category and prerequisite.

Student Management: Add students using their roll numbers and names.

Course Registration: Register students for courses after checking eligibility conditions.

Grade Submission: Accept grades according to the course type.

Thesis Management: Track thesis progress and convert a successful thesis progress grade into a final letter grade.

Course Repetition: Allow a student to register again for a course after a failed attempt.

GPA Calculation: Calculate credit-weighted GPA using integer arithmetic and round it half-up to two decimal places.

Average Grade: Determine the average letter grade from the rounded GPA.

Graduation Audit: Check all five graduation requirements and report the rules that are not satisfied.

Eligibility Checking: Check the six eligibility conditions before allowing registration.

Transcript: Display a student's registrations in term order.

Save and Load: Save academic records to a file and load them back.

3. Project Structure
The project is organised into separate source files so that different parts of the system can be maintained independently.

AcadERP-Lite-Group-102/
│
├── src/
│   ├── grp_102_erp.h
│   ├── grp_102_erp.cpp
│   ├── grp_102_course.cpp
│   ├── grp_102_student.cpp
│   ├── grp_102_registration.cpp
│   ├── grp_102_commands.cpp
│   ├── grp_102_gpa.cpp
│   ├── grp_102_persistence.cpp
│   └── grp_102_main.cpp
│
├── tests/
│   ├── B01.in / B01.out
│   ├── B02.in / B02.out
│   ├── B03.in / B03.out
│   ├── B04.in / B04.out
│   ├── T01.in / T01.out
│   ├── ...
│   ├── T23.in / T23.out
│   ├── repeat.in / repeat.out
│   ├── sample.in / sample.out
│   └── thesis_repeat.in / thesis_repeat.out

###The project currently includes 30 input-output test pairs: 23 assignment test cases, 4 bonus test cases and 3 additional test cases covering repeated registrations and sample workflows.

├── Makefile
├── run_tests.sh
└── README.md

#Source file responsibilities
##File	Responsibility
grp_102_erp.h	Contains constants, enums, class declarations and function prototypes.
grp_102_erp.cpp	Implements the main ERP operations, searching, registration, grade handling and eligibility checks.
grp_102_course.cpp	Implements the Course class and its data access functions.
grp_102_student.cpp	Implements student records and registration-array management.
grp_102_registration.cpp	Implements registration details, grades and thesis states.
grp_102_commands.cpp	Reads and processes commands, validates their arguments and handles query output.
grp_102_gpa.cpp	Implements GPA, average-grade and graduation calculations.
grp_102_persistence.cpp	Implements the SAVE and LOAD operations.
grp_102_main.cpp	Contains the main input loop and reports command errors.
4. Design and Implementation
4.1 Classes and Data Organisation
The system is divided into four main classes:

Course

Stores the course code, course type, credit value, category and prerequisite. The prerequisite is stored as an index into the course array. A value of -1 represents a course without a prerequisite.

Student

Stores the student's roll number and name. Each student also maintains an array of registrations, which is allocated dynamically when required.

Registration

Stores the course index, semester number, grade and thesis state for a particular registration. It also helps distinguish between an ungraded registration and one that has already received a grade.

ERP

Acts as the main management class. It maintains the student and course records and provides the operations required for registration, grade submission, eligibility checking and academic calculations.

The implementation uses fixed-size arrays for the main student and course records. This keeps storage predictable and follows the restrictions of the assignment.

4.2 Course Types and Grade Handling
Three course types are supported:

REGULAR: Accepts the letter grades A, A-, B, B-, C, C-, D and F.

SU: Accepts S and X. An S grade earns the course credits, while an X does not.

THESIS: Initially accepts S or X as a progress grade. A thesis with S can later be converted into a final letter grade.

A thesis moves through its states according to the grading process:

REGISTERED → IN_PROGRESS → FINAL

If the progress grade is X, the thesis moves to FAILED. A thesis that is already FINAL or FAILED cannot receive another grade.

4.3 Repeated Course Attempts
A student can register for a course again only after the latest graded attempt has failed.

For academic calculations, the system considers the latest attempt that has a grade. Earlier attempts do not contribute to the GPA or earned-credit calculations when a later graded attempt exists.

This is important because an ungraded registration should not replace the result of an earlier graded attempt.

4.4 GPA and Average Grade
The GPA is calculated as a credit-weighted average of grade points for regular courses and finalised theses.

The calculation is performed using integer arithmetic rather than floating-point values. The numerator and denominator are maintained as integers, and the result is rounded half-up to two decimal places as specified in the assignment.

SU courses are excluded from GPA calculations. A thesis is included only after it has been converted to a final letter grade.

If a student has no course that contributes to the GPA, the program reports E_NO_GPA.

The average grade is determined using the rounded GPA and the grade bands given in the project specification.

4.5 Graduation Audit
The GRAD command checks the following five requirements:

R1: The student has earned the required total credits.

R2: Every CORE course in the catalogue has been passed.

R3: The required number of ELECTIVE credits has been earned.

R4: The thesis has reached FINAL status with a grade of C or better.

R5: The rounded GPA meets the program's minimum GPA requirement.

If one or more requirements are not satisfied, the program reports their rule numbers in the specified order.

4.6 Course Eligibility
Before registration, the system checks the eligibility conditions in the required order:

E1 — Student and course exist.

E2 — The course has not already been passed.

E3 — There is no ungraded registration for the same course, including an active thesis registration.

E4 — The prerequisite has been passed.

E5 — The semester's registered credit load does not exceed the program limit.

E6 — The student has earned enough credits to register for a thesis.

The first failed condition determines the eligibility result.

5. Memory Management
The project uses manual memory management with new and delete[], instead of STL containers or smart pointers.

The main student and course records are stored in fixed-size arrays. Each Student object manages its own dynamically allocated registration array.

Allocation
The registration array is allocated in:

src/grp_102_student.cpp

The initializeRegistrations() function allocates space for MAX_REGS registration objects using:

regs = new Registration[MAX_REGS];
Allocation is performed when a student needs registration storage.

Deallocation
The allocated registration array is released in the Student destructor:

delete[] regs;
The Student objects are members of the ERP object, so their destructors are called automatically when the ERP object is destroyed.

The implementation also prevents accidental copying of Student objects, since copying an object that owns a dynamically allocated array could result in double deletion or invalid memory ownership.

The swap() operation is used when transferring student data during loading, so ownership of the allocated registration arrays is transferred rather than duplicated.

6. Error Handling
The program uses an ErrorCode enum to represent errors instead of exceptions.

Functions that can fail return an error code, and the calling function checks the result before continuing.

Some of the errors handled include:

E_PARSE — Invalid command or incorrect input format.

E_FULL — A storage limit has been reached.

E_DUPLICATE — A student or course already exists.

E_UNKNOWN_STUDENT — Student roll number was not found.

E_UNKNOWN_COURSE — Course code was not found.

E_BAD_COURSE — Invalid course details.

E_BAD_GRADE — Invalid grade symbol.

E_GRADE_TYPE — Grade does not match the course type.

E_NOT_REGISTERED — No matching registration exists.

E_ALREADY_GRADED — The registration has already been graded.

E_THESIS_STATE — Invalid thesis state for conversion.

E_NO_GPA — No course contributes to the GPA.

When a command fails, the main loop prints the error with its input line number in the format:

ERROR <line_number> <ERROR_CODE>
The program then continues processing the remaining input lines. Failed commands are validated before changes are made so that an unsuccessful operation does not partially modify the stored records.

7. Input Commands
The program accepts one command per line.

Command	Syntax
PROGRAM	PROGRAM <total> <elective> <mingpa> <maxload> <thesiscredits>
COURSE	COURSE <code> <REGULAR|SU|THESIS> <credits> <CORE|ELECTIVE> <prereq>
STUDENT	STUDENT <roll> <name>
REGISTER	REGISTER <roll> <course> <term>
GRADE	GRADE <roll> <course> <term> <symbol>
CONVERT	CONVERT <roll> <thesis> <letter>
GPA	GPA <roll>
AVG	AVG <roll>
GRAD	GRAD <roll>
ELIGIBLE	ELIGIBLE <roll> <course> <term>
TRANSCRIPT	TRANSCRIPT <roll>
SAVE	SAVE <file>
LOAD	LOAD <file>
Use - for a course without a prerequisite.

Blank lines and lines beginning with # are ignored. They still count towards input line numbers.

Successful data-changing commands do not print anything. Query commands print their results in the format specified by the assignment.

8. Building the Project
Requirements
A Linux environment with g++ and make installed is required.

The project is written in C++11 and uses the following compiler flags:

-std=c++11 -Wall -Wextra -fno-exceptions
Build
From the project root directory, run:

make
This compiles the source files and generates the executable:

acaderp
Run the Program
To execute the program:

./acaderp
Commands can be entered interactively or supplied through an input file:

./acaderp < tests/T01.in
9. Running the Test Suite
The project includes input and expected-output files inside the tests/ directory.

Each test uses a pair of files:

Tnn.in — Input commands.

Tnn.out — Expected output.

The test script builds the project, executes the test inputs and compares the program output against the corresponding expected output.

Run the complete test suite using:

./run_tests.sh
The script reports the number of passed and failed tests and displays differences when an output does not match.

The test suite covers regular and SU grading, thesis progression and conversion, repeated attempts, GPA rounding, graduation rules, eligibility conditions, invalid commands, storage limits and other boundary cases. Bonus functionality is also tested.

10. Assumptions
The implementation follows the rules explicitly stated in the project specification. The following input and design choices are used:

Course names, student names and identifiers are supplied as single words, as required by the command format.

Course prerequisites must refer to a course that has already been added to the catalogue.

The first non-blank, non-comment command may be PROGRAM. If it is omitted, the default program values specified in the assignment are used.

Semester numbers are restricted to the range 1–12.

A registration without a grade does not replace the latest graded attempt when calculating academic results.

The transcript displays registrations in ascending semester order.

SAVE and LOAD use the file-based persistence format implemented by the project. They are intended to preserve the academic records and program configuration needed to continue working with the system.

No additional grading scale or graduation rule has been introduced beyond the assignment requirements.

11. Bonus Functionality
The following bonus commands have also been implemented:

TRANSCRIPT

TRANSCRIPT <roll>
Displays the student's registrations with their semester, course details, credits and grade, arranged in term order.

SAVE

SAVE <file>
Writes the current academic records and program configuration to the specified file.

LOAD

LOAD <file>
Loads previously saved records. The loading operation uses a temporary ERP object so that invalid file contents do not partially replace the existing records.

The <fstream> header is used for these file operations, as permitted for the persistence bonus in the assignment.

12. Final Note
AcadERP-Lite was developed as an exercise in applying object-oriented programming concepts to a practical record-management problem. The implementation brings together class design, arrays, pointers, dynamic memory management, command parsing, error handling and integer-based calculations in one command-driven application.

The main objective was to keep the implementation understandable and modular while ensuring that the academic rules and output formats defined in the project specification are followed.
