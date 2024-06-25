#include	<iostream>
#include 	<fstream>
#include	<string>
#include	<cstdlib>
#include	<windows.h>	//for text color purpose
#include	"List.h"
#include	"Student.h"

bool CreateStuList(const char*, List*);
bool DeleteStudent(List*, char*);
bool PrintList(List, int);
bool InsertExamResult(const char*, List*);
bool PrintStatistic(List);
bool FilterStudent(List, List*, char*, int, int);
bool UpdateIDandPhone(List*);
bool FindPotentialFirstClass(List, List*, char*);
int menu();
bool Redundant(List*, Student);
void clearList(List*);
void removeDash(char*);


using namespace std;
int countdup = 0;


int main() {
	menu();
	cout << "\n\n";
	system("pause");
	return 0;
}

//1)Read student from file
bool CreateStuList(const char* filename, List* list) {
	Student studentinfo;
	ifstream input;
	char checkword[30];
	int i = 1;
	int countj = 0;
	char name1[15];
	input.open(filename);
	if (!input) {
		cout << "\nFailed to open the file" << endl;
		return false;
	}
	else {
		while (!input.eof()) {
			input >> checkword;
			if (strcmp(checkword, "=") == 0 && i == 1) {
				input >> studentinfo.id;
				i++;
			}
			else if (strcmp(checkword, "=") == 0 && i == 2) {
				input >> studentinfo.name;
				input.getline(name1, 15);
				strcat_s(studentinfo.name, name1);
				i++;
			}
			else if (strcmp(checkword, "=") == 0 && i == 3) {
				input >> studentinfo.course;
				i++;
			}
			else if (strcmp(checkword, "=") == 0 && i == 4) {
				input >> studentinfo.phone_no;
				i = 1;
				if (countj == 1) {
					list->insert(studentinfo);
					countj++;
				}
				else {
					if (!Redundant(list, studentinfo)) {   //ensure no duplicate record stored in linked list
						list->insert(studentinfo);
						countj++;
					}
				}
			}
		}
		input.close();
		return true;
	}
}

//return true if redundant data found, false otherwise
bool Redundant(List* list, Student tempStud) {
	Student temp;

	for (int i = 1; i <= list->size(); i++) {
		if (list->get(i, temp)) {
			if (temp.compareName1(tempStud))  //if name of student same with name of student in linked list
				return true;
		}
	}
	return false;
}

//2) Delete
bool DeleteStudent(List* list, char* id)
{
	Student deletestudent;
	for (int i = 1; i <= list->count; i++) {
		list->get(i, deletestudent);
		if (strcmp(deletestudent.id, id) == 0) {
			list->remove(i);
			return true;
		}
	}
	return false;
}

//3) Print student
bool PrintList(List list, int source)
{
	Exam exam;
	Student studList;
	if (list.empty()) {
		cout << "\nCannot print from an emtpy list. Please CREATE student list first." << endl << endl;
		return false;
	}
	else {
		if (source == 1)//display on screen
		{
			for (int i = 1; i <= list.size(); i++) {
				list.get(i, studList);
				cout << "******************************** STUDENT " << i << " ********************************";
				studList.print(cout);
				if (studList.exam_cnt <= 0) {
					cout << "THIS STUDENT HAVEN'T TAKEN ANY EXAM YET.\n" << endl;
				}
				else {
					cout << "\n------------------------------PAST YEAR RESULT : -----------------------------";
					for (int i = 0; i < studList.exam_cnt; i++)
						studList.exam[i].print(cout);
				}
				cout << "******************************** STUDENT " << i << " ********************************" << endl;
				cout << "\n\n";
			}
			return true;
		}
		else if (source == 2)//display on file
		{
			ofstream outfile;
			outfile.open("student_result.txt");
			for (int i = 1; i <= list.size(); i++) {
				list.get(i, studList);
				outfile << "******************************** STUDENT " << i << " ********************************";
				studList.print(outfile);
				if (studList.exam_cnt <= 0) {
					outfile << "THIS STUDENT HAVEN'T TAKEN ANY EXAM YET.\n" << endl;
				}
				else {
					outfile << "\n------------------------------PAST YEAR RESULT : -----------------------------";
					for (int i = 0; i < studList.exam_cnt; i++)
						studList.exam[i].print(outfile);
				}
				outfile << "******************************** STUDENT " << i << " ********************************" << endl;
				outfile << "\n\n";
			}
			cout << "\nNew student list has SUCCESSFULLY UPDATED inside student.txt file!" << endl;
			outfile.close();
			return true;
		}
	}
}

//4) Exam Result
bool InsertExamResult(const char* filename, List* list) {
	if (list->empty()) {
		cout << "No student found in this record! " << endl;
		return false;
	}

	Exam exam;
	int pos = 1;
	bool found;

	ifstream read("exam.txt");
	if (!read) {
		cout << "Unable to open " << filename << " file!" << endl;
		return false;
	}

	Student stu, stu2;
	string text;

	getline(read, text); //ignore the first line which is the blank line

	while (read >> stu2.id) {
		found = false;
		for (int i = 1; i <= list->size(); i++) {
			list->get(i, stu);
			if (stu.compareID(stu2)) {
				pos = i;
				found = true;
				break;
			}
		}
		if (!found)
			continue;

		read >> exam.trimester >> exam.year >> exam.numOfSubjects;

		for (int i = 0; i < exam.numOfSubjects; i++) {
			read >> exam.sub[i].subject_code;
			read >> exam.sub[i].subject_name;
			read >> exam.sub[i].credit_hours;
			read >> exam.sub[i].marks;
		}

		exam.calculateGPA();
		stu.exam[stu.exam_cnt] = exam;
		stu.exam_cnt++; //increment on the no of trimester exam student has taken
		stu.calculateCurrentCGPA();
		list->set(pos, stu);

		cout << "\nStudent ID: " << stu2.id << endl;
		cout << "Numbers of Subject Taken: " << exam.numOfSubjects << endl;
		cout << "GPA for Current Trimester: " << fixed << setprecision(5) << exam.gpa << endl;
		cout << "Total Numbers of Trimester Taken: " << stu.exam_cnt << endl;
	}
	read.close();
	cout << "\n" << string(60, '-') << endl;
	cout << "\nEach exam result has been SUCCESSFULLY inserted in student list!" << endl << endl;
	return true;
}

//5) Print Statistic
bool PrintStatistic(List list) {
	int totalStudent = list.size();
	int cs = 0, ia = 0, ib = 0, cn = 0, ct = 0, pos = 1, totaltrimester = 0;
	double totalSubject = 0, totalCredit = 0, totalCGPA = 0;
	double avgCGPA, avgSubject, avgCredits;
	Student stu;
	Exam exam;
	Node* p;
	p = list.head;

	if (list.empty()) {
		cout << "Cannot print from an emtpy list. Please CREATE student list first. " << endl;
		return false;
	}
	if (p->item.current_cgpa == 0){
		cout << "Cannot print from an emtpy list. Please INSERT student exam result into the list first. " << endl;
		return false;
	}

	Node* cur;
	cur = list.head;

	while (cur != nullptr) {
		stu = cur->item;

		for (int i = 0; i < stu.exam_cnt; i++)
			totalSubject += stu.exam[i].numOfSubjects;

		totaltrimester += stu.exam_cnt;
		totalCGPA += stu.current_cgpa;
		totalCredit += stu.totalCreditsEarned;

		if (stu.exam_cnt >= 1) {
			if (strcmp(stu.course, "CS") == 0)
				cs++;
			else if (strcmp(stu.course, "IA") == 0)
				ia++;
			else if (strcmp(stu.course, "IB") == 0)
				ib++;
			else if (strcmp(stu.course, "CN") == 0)
				cn++;
			else if (strcmp(stu.course, "CT") == 0)
				ct++;
		}
		cur = cur->next; //move to next node
	}

	avgCGPA = totalCGPA / totalStudent;
	avgSubject = totalSubject / totaltrimester;
	avgCredits = totalCredit / totaltrimester;

	cout << "\t\tTotal Students: " << totalStudent << endl;
	cout << string(60, '-') << endl;
	cout << "\n\t\t CS Students - " << cs << endl;
	cout << "\n\t\t IA Students - " << ia << endl;
	cout << "\n\t\t IB Students - " << ib << endl;
	cout << "\n\t\t CN Students - " << cn << endl;
	cout << "\n\t\t CT Students - " << ct << endl << endl;
	cout << string(60, '-') << endl;
	cout << "Average CGPA: " << fixed << setprecision(5) << avgCGPA << endl;
	cout << "\nAverage Subjects taken Per Semester: " << fixed << setprecision(2) << avgSubject << endl;
	cout << "\nAverage Credits Earned Per Semester: " << fixed << setprecision(2) << avgCredits << endl << endl;
	return true;
}

//6) Filter 
bool FilterStudent(List list1, List* list2, char* course, int year, int totalcredit)
{
	clearList(list2);
	if (list1.empty()) {
		cout << "\nCannot print from an emtpy list. Please CREATE the list first." << endl;
		return false;
	}
	Node* cur;
	cur = list1.head;
	if (cur->item.totalCreditsEarned == 0){
		cout << "Cannot print from an emtpy list. Please INSERT student exam result into the list first. " << endl;
		return false;
	}

	Node* pointer;
	pointer = list1.head;
	Node* pointer2;
	pointer2 = list1.head;
	Node* p;
	p = list1.head;
	Node* p1;
	p1 = list1.head;

	Student stu;
	Exam exam;
	int found = 0, f2 = 0, f3 = 0;
	cout << "\n" << string(60, '-') << endl;

	//Search course in list
	for (int i = 1; i <= list1.size(); i++) {  //loop through every element in linked list to find student
		if (strcmp(pointer->item.course, course) == 0) {
			found++;
			list1.get(i, stu);  //print student information after found
		}
		pointer = pointer->next;
	}
	if (found != 0)
		cout << "SUCCESSFULLY found " << found << " student in " << course << " course!" << endl;
	else
		cout << "Cannot found student records with " << course << " course" << endl;


	//Search year in list
	for (int i = 1; i <= list1.size(); i++) {
		//use atoi to convert c-string to int
		//use substr to extract student id from position 0 and length of 2 which is taking out the first 2 character in student id
		//use c_str() to return c-string so can use atoi to convert c-string to int type
		//add 2000 so can compare with the year entered

		if (atoi(string(p->item.id).substr(0, 2).c_str()) + 2000 == year) {
			f2++;
			list1.get(i, stu);
		}
		p = p->next;
	}
	if (f2 != 0)
		cout << "SUCCESSFULLY found " << f2 << " student that enrolled in year " << year << "!" << endl;
	else
		cout << "Cannot found student records that enrolled in year " << year << endl;


	//Search total credits in list
	for (int i = 1; i <= list1.size(); i++) {
		if (pointer2->item.totalCreditsEarned >= totalcredit) {
			f3++;
			list1.get(i, stu);
		}
		pointer2 = pointer2->next;
	}
	if (f3 != 0)
		cout << "SUCCESSFULLY found " << f3 << " student that earned more than or equal to the total credits of " << totalcredit << "!" << endl;
	else
		cout << "Cannot found student that earned more than or equal to the total credits of " << totalcredit << endl;
	cout << "\n";

	//Insert student that fulfills all the three conditions into list2
	for (int j = 1; j <= list1.size(); j++) {
		if (strcmp(p1->item.course, course) == 0 && atoi(string(p1->item.id).substr(0, 2).c_str()) + 2000 == year && totalcredit <= p1->item.totalCreditsEarned) {
			list1.get(j,stu);
			list2->insert(stu);
			break;
		}
		p1 = p1->next;
	}

	if (list2->empty()) {
		cout << "However NO STUDENT fulfills all the three conditions that you have entered." << endl;
		return false;
	}
	return true;
}

//to clear list2
void clearList(List* list)
{
	while (!list->empty())
		list->remove(1);
}


//7) Update Student ID and Phone No
bool UpdateIDandPhone(List* list) {
	Student stu;
	Node* cur;
	char id[20];
	char phone[40];

	if (list->empty()) {
		cout << "Cannot print from an emtpy list. " << endl;
		return false;
	}

	for (int i = 1; i <= list->size(); i++) {
		list->get(i, stu);

		//Update Student ID
		if (strcmp(stu.course, "CS") == 0) {
			strcpy_s(id, "BCS");
		}
		else if (strcmp(stu.course, "IA") == 0) {
			strcpy_s(id, "BIA");
		}
		else if (strcmp(stu.course, "IB") == 0) {
			strcpy_s(id, "BIB");
		}
		else if (strcmp(stu.course, "CN") == 0) {
			strcpy_s(id, "BCN");
		}
		else if (strcmp(stu.course, "CT") == 0) {
			strcpy_s(id, "BCT");
		}
		strcat_s(id, stu.id);
		strcpy_s(stu.id, id);

		//Update Student Phone Number
		if (stu.phone_no[0] % 2 == 1) {
			strcpy_s(phone, "01");
		}
		else {
			strcpy_s(phone, "02");
		}
		strcat_s(phone, stu.phone_no);
		removeDash(phone);
		strcpy_s(stu.phone_no, phone);

		list->set(i, stu);
		list->get(i, stu);
		cout << "\nNew Student ID: " << stu.id << endl;
		cout << "New Student Phone No.: " << stu.phone_no << endl;
	}
	return true;
}

//remove dash in phone no
void removeDash(char* str) {
	char* read = str;
	char* write = str;
	while (*read) {
		if (*read != '-') {
			*write++ = *read;
		}
		read++;
	}
	*write = '\0'; //Null-terminate
}

//8) Find the Potential First Class Student
bool FindPotentialFirstClass(List list1, List* list2, char* course) {
	bool PotentialToGetFirstClass;
	Student stu;
	Exam exam;

	clearList(list2);
	if (list1.empty() || !list2->empty()) {
		cout << "\nCannot print from an emtpy list." << endl;
		return false;
	}

	Node* cur = list1.head;
	while (cur != NULL) {
		stu = cur->item;
		if (strcmp(stu.course, course) != 0) {
			cur = cur->next;
			continue;
		}
		PotentialToGetFirstClass = true; //initialize for each student

		if (stu.exam_cnt >= 3) { //the number of trimester taken by a student

			//Check every trimesters' gpa
			for (int i = 0; i < stu.exam_cnt; i++) {
				if (stu.exam[i].gpa >= 3.75000 && stu.totalCreditsEarned >= 12) { //check gpa and credit hours
					continue; //continue check other trimester's gpa
				}
				else if (stu.exam[i].gpa < 3.50000) {
					PotentialToGetFirstClass = false;
					break; //exit loop
				}
			}
		}
		else {
			PotentialToGetFirstClass = false;
		}

		if (PotentialToGetFirstClass) {
			list2->insert(stu);
			cout << "\nThe student " << stu.name << " in the course " << stu.course << " has the potential to get first class" << endl;
			break;
		}
		cur = cur->next;
	}
	return true;
}

//9) Menu
int menu()
{
	List studList;
	List studList2;
	string choice;
	do {
		system("cls");
		system("Color E");
		cout << string(60, char(175)) << endl;
		cout << "           __  ___   ______   _   __   __  __" << endl;
		cout << "          /  |/  /  / ____/  / | / /  / / / /" << endl;
		cout << "         / /  / /  / /___   / /|  /  / / / / " << endl;
		cout << "        / /  / /  / /___   / /|  /  / /_/ / " << endl;
		cout << "       /_/  /_/  /_____/  /_/ |_/   |____/  " << endl;
		cout << string(60, char(175)) << endl;
		cout << "  \t\t1. Create Student List" << endl;
		cout << "  \t\t2. Delete Student" << endl;
		cout << "  \t\t3. Print Student List" << endl;
		cout << "  \t\t4. Insert Exam Result" << endl;
		cout << "  \t\t5. Print Exam Statistic" << endl;
		cout << "  \t\t6. Filter Student" << endl;
		cout << "  \t\t7. Update Student's ID and Phone" << endl;
		cout << "  \t\t8. Find Potential First Class Student" << endl;
		cout << "  \t\t9. Exit" << endl;
		cout << string(60, char(175)) << endl;
		cout << "Enter your choice: ";
		getline(cin, choice);
		//check validity of choice
		if (choice.length() != 1) {
			cout << "Invalid choice. Please try again" << endl;
		}
		else {
			if (choice == "1") {
				cout << "\n" << string(60, '-') << endl;
				cout << "\t\tCreate Student List" << endl;
				cout << string(60, '-') << endl;
				if (CreateStuList("student.txt", &studList) == true) {
					cout << "\n" << studList.count << " records have been SUCCESSFULLY read from student.txt and stored in a list." << endl << endl;
					if (countdup > 0)
						cout << "\nThere are " << countdup << " records is duplicated and ignored. " << endl;
				}
				else
					cout << "\nFAILED to create student list and read student.txt file" << endl;
				system("pause");
			}
			else if (choice == "2") {
				cout << "\n" << string(60, '-') << endl;
				cout << "\t\tDelete Student Record" << endl;
				cout << string(60, '-') << endl;
				char stuId[13];
				bool invalid;

				if (studList.count != 0) {
					do {
						cout << "\nEnter student ID: ";
						cin.getline(stuId, 13);

						//convert course entered into upper case
						for (int i = 0; i < strlen(stuId); i++)
							stuId[i] = toupper(stuId[i]);

						//length of student id = 10
						invalid = (strlen(stuId) > 11 || strlen(stuId) < 7);
						if (invalid) {
							cin.clear();
							cin.ignore(100, '\n');
							cout << "INVALID student id. Please try again\n" << endl;
						}
					} while (invalid);

					if (DeleteStudent(&studList, &stuId[0]))
						cout << "\nThe student id " << stuId << " has delected SUCCESSFULLY! " << endl;
					else
						cout << "\nStudent id with " << stuId << " CANNOT be found" << endl;
				}
				else {
					cout << "\nThe list is EMPTY. There are nothing to delete! Please CREATE student list first." << endl;
				}
				system("pause");
			}
			else if (choice == "3") {
				string input;
				bool ValidNo;
				cout << "\n" << string(60, '-') << endl;
				cout << "\t\tPrint Student List" << endl;
				cout << string(60, '-') << endl;
				do {
					ValidNo = true;
					cout << "Please select where do you want to display student list [1-Screen/2-File]: ";
					cin >> input;
					cout << "\n";
					if (input == "1") {
						PrintList(studList, 1);
						system("pause");
					}
					else if (input == "2") {
						PrintList(studList, 2);
						system("pause");
					}
					else {
						cout << "Invalid Input. Please try again.\n" << endl;
						ValidNo = false;
					}
				} while (!ValidNo);
			}
			else if (choice == "4") {
				cout << "\n" << string(60, '-') << endl;
				cout << "\t\tInsert Exam Result" << endl;
				cout << string(60, '-') << endl;
				if (!InsertExamResult("exam.txt", &studList))
					cout << "List is empty. Please CREATE the student list first." << endl;
				system("pause");
			}
			else if (choice == "5") { 
				cout << "\n" << string(60, '-') << endl;
				cout << "\t\tPrint Exam Statistic" << endl;
				cout << string(60, '-') << endl;
				PrintStatistic(studList);
				system("pause");
			}
			else if (choice == "6") {
				char course[5];
				int year, totalcredit;
				bool ValidNo = false;
				cout << "\n" << string(60, '-') << endl;
				cout << "\t\tFilter Student" << endl;
				cout << string(60, '-') << endl;

				while (!ValidNo) {
					cout << "\nPlease enter the course that you which to filter on [IA/IB/CS]: ";
					cin.getline(course, 5);
					//convert course entered into upper case
					for (int i = 0; i < strlen(course); i++)
						course[i] = toupper(course[i]);
					if (strlen(course) != 2) {
						cin.clear();
						cin.ignore(100, '\n');
						cout << "Invalid course entered.Please try again. \n";
					}
					else
						ValidNo = true;
				}

				ValidNo = false;
				while (!ValidNo) {
					cout << "\nPlease enter student enrolled year [2012/2013]: ";
					cin >> year;
					if ( cin.fail() || year < 2000 || year > 2025) {
						cin.clear();
						cin.ignore(100, '\n');
						cout << "Invalid year entered. Please try again.\n";
					}
					else
						ValidNo = true;
				}

				ValidNo = false;
				while (!ValidNo) {
					cout << "\nPlease enter student total credit hour: ";
					cin >> totalcredit;
					if (cin.fail() || totalcredit < 0 || totalcredit > 99) {
						cin.clear();
						cin.ignore(100, '\n');
						cout << "Invalid total credit hour entered. Please try again.\n";
					}
					else
						ValidNo = true;
				}
				if (FilterStudent(studList, &studList2, course, year, totalcredit)){
					cout << "\nFiltering student who has fulfilled the 3 conditions that entered: " << endl;
					PrintList(studList2, 1);
				}
				system("pause");
			}
			else if (choice == "7") {
				cout << "\n" << string(60, '-') << endl;
				cout << "\t\tUpdate Student's ID and Phone" << endl;
				cout << string(60, '-') << endl;
				if (!UpdateIDandPhone(&studList))
					cout << "Please CREATE student list first. " << endl;
				else{
					cout << string(60, '-') << endl;
					cout << "\nStudent ID and phone numbers have SUCCESSFULLY UPDATED!" << endl << endl;
				}
				system("pause");
			}
			else if (choice == "8") {
				int ans;
				bool invalid = false;
				char course[3];

				cout << "\n" << string(60, '-') << endl;
				cout << "\t\tFind Potential First Class Student" << endl;
				cout << string(60, '-') << endl;
				do{
					invalid = false;
					cout << "Please enter the course number that you want to find the potential first class student : " << endl;
					cout << "1. IA \t 2. IB \t 3. CS \t 4. CN \t 5. CT \nAnswer [1/2/3]:";
					cin >> ans;
					if ( ans == 1){
						invalid = true;
						strcpy_s(course, "IA");
					}
					else if (ans == 2){
						invalid = true;
						strcpy_s(course, "IB");
					}
					else if ( ans == 3){
						invalid = true;
						strcpy_s(course, "CS");
					}
					else if ( ans == 4){
						invalid = true;
						strcpy_s(course, "CN");
					}
					else if (ans == 5){
						invalid = true;
						strcpy_s(course, "CT");
					}
					else{
						cin.clear();
						cin.ignore(100, '\n');
						cout << "Invalid Input! Please try agian.\n" << endl;
						cout << "\n";
						invalid = false;
					}
				} while (!invalid);

				if (!FindPotentialFirstClass(studList, &studList2, course))
					cout << "Please CREATE student list first. " << endl;
				else {
					if (studList2.empty()){
						cout << "\nThere is no student in " << course << " that has potential to get first class." << endl;
					}
					else {
						PrintList(studList2, 1);
					}
				}
				system("pause");
			}
			else if (choice == "9") {
				cout << "\nEXIT SUCCESSFULLY! SEE YOU NEXT TIME ;)" << endl;
				break;
			}
		}
	} while (choice != "9");
	return 0;
}


