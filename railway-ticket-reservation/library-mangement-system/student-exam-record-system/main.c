#include <stdio.h>
#include <string.h>

#define MAX 50
#define SUBJECTS 5

typedef struct {
    int roll;
    char name[50];
    char branch[30];
    int marks[SUBJECTS];
    int total;
    float percentage;
    char grade;
    int pass; // 1 = pass, 0 = fail
} Student;

Student students[MAX];
int count = 0;

/* Function declarations */
void loadFromFile();
void saveToFile();
int findStudentByRoll(int roll);
void calculateResult(Student *s);
void addStudent();
void displayStudent();
void displayAllSorted();
void searchByName();
void updateStudent();
void deleteStudent();
void subjectStatistics();
void passFailStatistics();

int main() {
    int choice;
    loadFromFile();

    do {
        printf("\n===== STUDENT EXAMINATION SYSTEM =====\n");
        printf("1. Add Student\n");
        printf("2. Display Student by Roll\n");
        printf("3. Display All Students (Sorted by Percentage)\n");
        printf("4. Search Student by Name\n");
        printf("5. Update Student Record\n");
        printf("6. Delete Student Record\n");
        printf("7. Subject-wise Statistics\n");
        printf("8. Pass/Fail Statistics\n");
        printf("9. Save & Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: addStudent(); break;
            case 2: displayStudent(); break;
            case 3: displayAllSorted(); break;
            case 4: searchByName(); break;
            case 5: updateStudent(); break;
            case 6: deleteStudent(); break;
            case 7: subjectStatistics(); break;
            case 8: passFailStatistics(); break;
            case 9: saveToFile(); printf("Data saved. Exiting...\n"); break;
            default: printf("Invalid choice!\n");
        }
    } while (choice != 9);

    return 0;
}

/* Load data */
void loadFromFile() {
    FILE *fp = fopen("students.dat", "rb");
    if (fp != NULL) {
        fread(&count, sizeof(int), 1, fp);
        fread(students, sizeof(Student), count, fp);
        fclose(fp);
    }
}

/* Save data */
void saveToFile() {
    FILE *fp = fopen("students.dat", "wb");
    fwrite(&count, sizeof(int), 1, fp);
    fwrite(students, sizeof(Student), count, fp);
    fclose(fp);
}

/* Find student */
int findStudentByRoll(int roll) {
    for (int i = 0; i < count; i++)
        if (students[i].roll == roll)
            return i;
    return -1;
}

/* Calculate result */
void calculateResult(Student *s) {
    s->total = 0;
    s->pass = 1;

    for (int i = 0; i < SUBJECTS; i++) {
        s->total += s->marks[i];
        if (s->marks[i] < 40)
            s->pass = 0;
    }

    s->percentage = s->total / (float)SUBJECTS;

    if (s->percentage >= 90) s->grade = 'A';
    else if (s->percentage >= 75) s->grade = 'B';
    else if (s->percentage >= 60) s->grade = 'C';
    else if (s->percentage >= 40) s->grade = 'D';
    else s->grade = 'F';
}

/* Add student */
void addStudent() {
    if (count >= MAX) {
        printf("Maximum limit reached!\n");
        return;
    }

    Student s;
    printf("Enter Roll Number: ");
    scanf("%d", &s.roll);

    if (findStudentByRoll(s.roll) != -1) {
        printf("Duplicate roll number!\n");
        return;
    }

    printf("Enter Name: ");
    scanf(" %[^\n]", s.name);
    printf("Enter Branch: ");
    scanf(" %[^\n]", s.branch);

    for (int i = 0; i < SUBJECTS; i++) {
        do {
            printf("Enter marks for subject %d: ", i + 1);
            scanf("%d", &s.marks[i]);
        } while (s.marks[i] < 0 || s.marks[i] > 100);
    }

    calculateResult(&s);
    students[count++] = s;
    printf("Student added successfully.\n");
}

/* Display student */
void displayStudent() {
    int roll;
    printf("Enter roll number: ");
    scanf("%d", &roll);

    int idx = findStudentByRoll(roll);
    if (idx == -1) {
        printf("Student not found.\n");
        return;
    }

    Student s = students[idx];
    printf("\nRoll: %d\nName: %s\nBranch: %s\n", s.roll, s.name, s.branch);
    printf("Total: %d\nPercentage: %.2f\nGrade: %c\nStatus: %s\n",
           s.total, s.percentage, s.grade, s.pass ? "PASS" : "FAIL");
}

/* Display sorted list */
void displayAllSorted() {
    Student temp[MAX];
    memcpy(temp, students, sizeof(Student) * count);

    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (temp[i].percentage < temp[j].percentage) {
                Student t = temp[i];
                temp[i] = temp[j];
                temp[j] = t;
            }
        }
    }

    printf("\nRank Roll Name Percentage\n");
    for (int i = 0; i < count; i++) {
        printf("%d   %d   %s   %.2f\n", i + 1, temp[i].roll, temp[i].name, temp[i].percentage);
    }
}

/* Search by name */
void searchByName() {
    char key[50];
    printf("Enter name to search: ");
    scanf(" %[^\n]", key);

    for (int i = 0; i < count; i++) {
        if (strstr(students[i].name, key)) {
            printf("%d %s %.2f\n", students[i].roll, students[i].name, students[i].percentage);
        }
    }
}

/* Update student */
void updateStudent() {
    int roll;
    printf("Enter roll number to update: ");
    scanf("%d", &roll);

    int idx = findStudentByRoll(roll);
    if (idx == -1) {
        printf("Student not found.\n");
        return;
    }

    for (int i = 0; i < SUBJECTS; i++) {
        do {
            printf("Enter new marks for subject %d: ", i + 1);
            scanf("%d", &students[idx].marks[i]);
        } while (students[idx].marks[i] < 0 || students[idx].marks[i] > 100);
    }

    calculateResult(&students[idx]);
    printf("Record updated.\n");
}

/* Delete student */
void deleteStudent() {
    int roll;
    printf("Enter roll number to delete: ");
    scanf("%d", &roll);

    int idx = findStudentByRoll(roll);
    if (idx == -1) {
        printf("Student not found.\n");
        return;
    }

    for (int i = idx; i < count - 1; i++)
        students[i] = students[i + 1];

    count--;
    printf("Record deleted.\n");
}

/* Subject statistics */
void subjectStatistics() {
    for (int s = 0; s < SUBJECTS; s++) {
        int max = 0, min = 100, sum = 0;
        for (int i = 0; i < count; i++) {
            int m = students[i].marks[s];
            if (m > max) max = m;
            if (m < min) min = m;
            sum += m;
        }
        printf("Subject %d -> Max: %d Min: %d Avg: %.2f\n",
               s + 1, max, min, count ? sum / (float)count : 0);
    }
}

/* Pass/fail statistics */
void passFailStatistics() {
    int pass = 0, fail = 0;
    for (int i = 0; i < count; i++)
        students[i].pass ? pass++ : fail++;

    printf("Passed: %d\nFailed: %d\nPass Percentage: %.2f\n",
           pass, fail, count ? (pass * 100.0 / count) : 0);
}
