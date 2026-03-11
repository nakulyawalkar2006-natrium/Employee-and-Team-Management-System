#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>
// --- CONSTANTS ---
#define MAX_EMPLOYEES 1000
#define MAX_TEAMS 100
#define DAYS_IN_WEEK 7
#define MAX_MEMBERS_PER_TEAM 5
#define MAX_NAME_LENGTH 64
#define MAX_DEAPRTMENT_LENGTH 64
#define EMPTY_EMP_ID 1001
#define EMPTY_TEAM_ID 101
#define MAX_WEEKLY_TARGET 50
#define MAX_EMPLOYEE_AGE 100
#define MIN_EMPLOYEE_AGE 15
#define MAX_SIZE_EMPLOYEE_STRUCT 1376 // 32 + 64*8 + 32 + 64*8 + 7*32 + 32 + 32
#define MAX_SIZE_TEAM_STRUCT 768 // 32 + 64*8 + 5*32 + 32 + 32
// --- DIRECT ACCESSSED VARIABLES ---
char days[DAYS_IN_WEEK][10] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
bool employee_exist[MAX_EMPLOYEES] = {false};
bool team_exist[MAX_TEAMS] = {false};
int num_of_employees = 0;
int num_of_teams = 0;
bool changes_made = false;
// --- EMPLOYEE STRUCTURE ---
typedef struct employee{
    int id;
    char name[MAX_NAME_LENGTH];
    int age;
    char department[MAX_DEAPRTMENT_LENGTH];
    int working_hours[DAYS_IN_WEEK];
    int weekly_target;
    int team_id;
} Employee;
Employee employees[MAX_EMPLOYEES];
// --- TEAM STRUCTURE ---
typedef struct team{
    int team_id;
    char name[MAX_NAME_LENGTH];
    int members_ids[MAX_MEMBERS_PER_TEAM];
    int members_count;
    int weekly_team_target;
} Team;
Team teams[MAX_TEAMS];
// ----- HELPER FUNCTIONS -----
void get_name(char name[]);
void get_department(char department[]);
void get_team_name(char team_name[]);
int calculate_total_hours(Employee emp);
int calculate_total_hours_team(int team_ID);
int generate_emp_id_valid();
int generate_team_id_valid();
int vacant_index_employee();
int vacant_index_team();
int find_employee_index(int emp_id);
int find_team_index(int teams_id);
void update_working_hours(int emp_id);
void update_weekly_target_employee(int emp_id, int new_target);
void update_weekly_target_teams(int team_ID, int new_target);
// ----- SORTING FUNCTIONS ----- 
void sort_employees(int low, int high);
int partition_employee_id(int low, int high);
void sort_teams(int low, int high);
int partition_teams_id(int low, int high);
void sort_employee_indices(int indices[], int low, int high);
int partition_employee_indices(int indices[], int low, int high);
void sort_team_indices(int indices[], int low, int high);
int partition_team_indices(int indices[], int low, int high);
// ----- FUNCTONS TO SAVE DATA TO FILE -----
void save_employee_details_in_file();
void save_team_details_in_file();
// ----- FUNCTIONS TO READ FROM FILE -----
int load_employee_details_from_file();
int load_team_details_from_file();
// ----- CORE FUNCTIONS -----
void initialize_employees();
void initialize_teams();
void add_employees();
void create_team();
void top_five_employees();
void team_achievement(int team_ID);
void team_ranking_all();
int team_ranking_specific(int team_id);
void employee_reward_specific(int emp_id);
void update_delete_employee(int emp_id);
void delete_team(int team_id);
void merge_team(int team_id1, int team_id2);
void display_team_info();
void menu();
// ----- Sorting Function (Employee ID) -----
void sort_employees(int low, int high){
    if (low < high) {
        int partition_index = partition_employee_id(low,high);
        sort_employees(low, partition_index - 1);
        sort_employees(partition_index + 1, high);
    }
}
int partition_employee_id(int low, int high){
    int pivot_index = employees[high].id;
    int i = low - 1;
    for (int j = low; j < high; j++){
        if (employees[j].id <= pivot_index){
            i++;
            Employee temp = employees[i];
            employees[i] = employees[j];
            employees[j] = temp;
        }
    }
    Employee temp = employees[i + 1];
    employees[i + 1] = employees[high];
    employees[high] = temp;
    return i + 1;
}
// ----- Sorting Function (Team ID) -----
void sort_teams(int low, int high){
    if(low < high){
        int partition_index = partition_teams_id(low,high);
        sort_teams(low,partition_index - 1);
        sort_teams(partition_index + 1, high);
    }
}
int partition_teams_id(int low, int high){
    int pivot_index = teams[high].team_id;
    int i = low - 1;
    for (int j = low; j < high; j++){
        if (teams[j].team_id <= pivot_index ){
            i++;
            Team temp = teams[i];
            teams[i] = teams[j];
            teams[j] = temp;
        }
    }
    Team temp = teams[i + 1];
    teams[i + 1] = teams[high];
    teams[high] = temp;
    return i + 1;
}
// ----- Sorting Function (Employee Indices Total working hours) -----
void sort_employee_indices(int indices[], int low, int high) {
    if (low < high) {
        int partition_index = partition_employee_indices(indices, low, high);
        sort_employee_indices(indices, low, partition_index - 1);
        sort_employee_indices(indices, partition_index + 1, high);
    }
}
int partition_employee_indices(int indices[], int low, int high) {
    int pivot_index = indices[high];
    int pivot_hours = calculate_total_hours(employees[pivot_index]);
    int i = low - 1;
    for (int j = low; j < high; j++) {
        int current_index = indices[j];
        if (employees[current_index].id != EMPTY_EMP_ID && calculate_total_hours(employees[current_index]) >= pivot_hours) {
            i++;
            int temp = indices[i];
            indices[i] = indices[j];
            indices[j] = temp;
        }
    }
    int temp = indices[i + 1];
    indices[i + 1] = indices[high];
    indices[high] = temp;
    return i + 1;
}
// ----- Sorting Function (Team Indices Total working hours) -----
void sort_team_indices(int indices[], int low, int high) {
    if (low < high) {
        int partition_index = partition_team_indices(indices, low, high);
        sort_team_indices(indices, low, partition_index - 1);
        sort_team_indices(indices, partition_index + 1, high);
    }
}
int partition_team_indices(int indices[], int low, int high) {
    int pivot_team_id = teams[indices[high]].team_id;
    int pivot_hours = calculate_total_hours_team(pivot_team_id);
    int i = low - 1;
    for (int j = low; j < high; j++) {
        int current_index = indices[j];
        if (teams[current_index].team_id != EMPTY_TEAM_ID && calculate_total_hours_team(teams[current_index].team_id) >= pivot_hours) {
            i++;
            int temp = indices[i];
            indices[i] = indices[j];
            indices[j] = temp;
        }
    }
    int temp = indices[i + 1];
    indices[i + 1] = indices[high];
    indices[high] = temp; 
    return i + 1;
}
// ----- Utility Functions -----
void get_name(char name[]){ // Get Employee Name
    bool valid = false;
    while (!valid){ // Checks Entered input are correct
        printf("\nEnter name of employee [MUST contain less than 63 characters]: ");
        fgets(name, MAX_NAME_LENGTH, stdin);
        name[strcspn(name, "\n")] = '\0';
        valid = true;
        for (int i = 0; name[i] != '\0' && valid; i++){
            if (!(isalpha(name[i]) || isspace(name[i]))){
                valid = false;
            }
        }
        if (!valid){
            printf("Error: Name contains invalid characters. Please try Again.\n");
        }
    }
    for (int i = 0; name[i] != '\0'; i++){
        name[i] = toupper(name[i]);
    }
}
void get_department(char department[]){ // Get Department Name
    bool valid = false;
    while (!valid){ // Checks Entered input are correct 
        printf("Enter Department of employee [MUST contain less than 63 characters]: ");
        fgets(department, MAX_DEAPRTMENT_LENGTH, stdin);
        department[strcspn(department, "\n")] = '\0';
        valid = true;
        for (int i = 0; department[i] != '\0' && valid; i++){
            if (!(isalpha(department[i]) || isspace(department[i]) || ispunct(department[i]))){
                valid = false;
            }
        }
        if (!valid){
            printf("Error: Department contains invalid characters. Please try Again.\n");
        }
    }
    for (int i = 0; department[i] != '\0'; i++){
        department[i] = toupper(department[i]);
    }
}
void get_team_name(char team_name[]){ // Get Team Name
    bool valid = false;
    while (!valid){ // Checks Entered input are correct
        valid = true;
        printf("Enter Team name [MUST contain less than 63 characters] : ");
        fgets(team_name, MAX_NAME_LENGTH, stdin);
        team_name[strcspn(team_name, "\n")] = '\0';
        for (int i = 0; team_name[i] != '\0' && valid; i++){
            if (!(isalnum(team_name[i]) || ispunct(team_name[i]) || isspace(team_name[i]))){
                valid = false;
            }
        }
        if (!valid){
            printf("Error: Team name contains invalid characters. Please try Again.\n");
        }
    }
    for (int i = 0; team_name[i] != '\0'; i++){
        team_name[i] = toupper(team_name[i]);
    }
}
// ----- Calculate Working Hours For Team and Employee -----
int calculate_total_hours(Employee emp){ // Calculates Total Working Hours
    int total_hours = 0;
    for (int i = 0; i < DAYS_IN_WEEK; i++){
        total_hours += emp.working_hours[i];
    }
    return total_hours;
}
int calculate_total_hours_team(int team_ID){ // Calculates Total Working Hours
    int total_hours = 0;
    int team_index = find_team_index(team_ID); // Get the index from the ID
    if(team_index != -1){
        for (int i = 0; i < teams[team_index].members_count; i++){
            int member_id = teams[team_index].members_ids[i];
            int member_index = find_employee_index(member_id); // Get the index from the member ID
            if (member_index != -1){
                total_hours += calculate_total_hours(employees[member_index]);
            }
        }
    }
    return total_hours;
}
// ----- Generates a Valid Employee ID and Team ID -----
int generate_emp_id_valid(){ // Generate a valid employee id
    int emp_id = -1;
    bool found = false;
    srand(time(NULL)); // takes current system time as seed
    if(num_of_employees != 1000){
        int random_number;
        while(!found){
            random_number = rand()%1000; // random number generation between 0 to 999  
            if(!employee_exist[random_number]){
                emp_id = random_number;
                found = true;
            }
        }
    }
    return emp_id;
}
int generate_team_id_valid(){ // Generate a valid team id
    int team_id = -1;
    bool found = false;
    srand(time(NULL));
    if(num_of_teams != 100){
        int random_number;
        while(!found){
            random_number = rand()%100;
            if(!team_exist[random_number]){
                team_id = random_number;
                found = true;
            }
        }
    }
    return team_id;
}
// ----- Finds Vacant index from Team and Employee array -----
int vacant_index_employee(){ // Finds vacant index in employee 
    for (int i = 0; i < MAX_EMPLOYEES; i++){
        if (employees[i].id == EMPTY_EMP_ID){
            return i;
        }
    }
    return -1;
}
int vacant_index_team(){ // Finds vacant index in team
    for (int i = 0; i < MAX_TEAMS; i++){
        if (teams[i].team_id == EMPTY_TEAM_ID){
            return i;
        }
    }
    return -1;
}
// ----- Finds Index of Employee ID and Team ID -----
int find_employee_index(int emp_id){  // Sorted arrays // binary search
    int emp_index = -1;
    bool found = false;
    int low = 0;
    int high = MAX_EMPLOYEES - 1;
    while(low <= high && !found){
        int mid = low + (high - low)/2;
        if(employees[mid].id == emp_id){
            found = true;
            emp_index = mid;
        }else if(employees[mid].id > emp_id){
            high = mid - 1;
        }else{
            low = mid + 1;
        }
    }
    return emp_index;
}
int find_team_index(int teams_id){
    int team_index = -1;
    bool found = false;
    int low = 0;
    int high = MAX_TEAMS - 1;
    while(low <= high && !found){
        int mid = low + (high - low)/2;
        if(teams[mid].team_id == teams_id){
            found = true;
            team_index = mid;
        }else if(teams[mid].team_id > teams_id){
            high = mid - 1;
        }else{
            low = mid + 1;
        }
    }
    return team_index;
}
// ----- Prints Employee Details -----
void print_employee_details(Employee emp){
    printf("   Employee ID           : %d\n", emp.id);
    printf("   Employee Name         : %s\n", emp.name);
    printf("   Employee Age          : %d\n", emp.age);
    printf("   Department Name       : %s\n", emp.department);
    printf("   Weekly Target (HOURS) : %d\n", emp.weekly_target);
    printf("   Team ID               : %d\n", emp.team_id);
    printf("   Working Hours         : ");
    for (int i = 0; i < DAYS_IN_WEEK; i++){
        printf("%d ", emp.working_hours[i]);
    } printf("\n");
    printf("   Total Working Hours   : %d\n", calculate_total_hours(emp));
}
void update_working_hours(int emp_id) {
    int emp_index = find_employee_index(emp_id);
    if(emp_index == -1){
        printf("Entered Employee doesn't exist\n");
    }else{
        for (int d = 0; d < DAYS_IN_WEEK; d++) {
            char choice;
            printf("Do you want to update hours for %s? [Y/N | y/n]: ", days[d]); // Fix: Use days[d]
            scanf(" %c", &choice);
            
            if (choice == 'Y' || choice == 'y') {
                int hours;
                bool valid = false;
                while (!valid) {
                    printf("Enter working hours for %s [0 -> 24]: ", days[d]); // Fix: Use days[d]
                        if (scanf("%d", &hours) == 1 && hours >= 0 && hours <= 24) {
                            employees[emp_index].working_hours[d] = hours;
                            valid = true;
                            getchar();
                        } else {
                            printf("Invalid input. Please enter a number between 0 and 24.\n");
                        }
                }
            }
        }
        printf("\nWorking hours set successfully for Employee ID %d\n", emp_id);
    }
}
// ----- Core Functions -----
// ----- Initialize Employee and Team Arrays -----
void initialize_employees(){
    for (int i = 0; i < MAX_EMPLOYEES; i++){
        employees[i].id = EMPTY_EMP_ID;
        strcpy(employees[i].name, "");
        employees[i].age = 0;
        strcpy(employees[i].department, "");
        for (int d = 0; d < DAYS_IN_WEEK; d++){
            employees[i].working_hours[d] = 0;
        }
        employees[i].weekly_target = 0;
        employees[i].team_id = EMPTY_TEAM_ID;
    }
    // Reset existence array
    for(int i=1; i<=MAX_EMPLOYEES; i++) employee_exist[i] = false;
    printf("\nAll employee records initialized.\n");
}
void initialize_teams(){
    for (int i = 0; i < MAX_TEAMS; i++){
        teams[i].team_id = EMPTY_TEAM_ID;
        strcpy(teams[i].name, "");
        for (int m = 0; m < MAX_MEMBERS_PER_TEAM; m++){
            teams[i].members_ids[m] = EMPTY_EMP_ID;
        }
        teams[i].members_count = 0;
        teams[i].weekly_team_target = 0;
    }
    // Reset existence array 
    for(int i=1; i<=MAX_TEAMS; i++) team_exist[i] = false;
    printf("\nAll team records initialized.\n");
}
// ----- ADDS EMPLOYEES -----
void add_employees(){
    bool add_emp = true;
    while (add_emp){
        int emp_ID = generate_emp_id_valid();
        if(emp_ID > 0 && emp_ID <= MAX_EMPLOYEES){
            bool valid = false;
            num_of_employees++;
            int emp_index = vacant_index_employee();
            employees[emp_index].id = emp_ID;
            employee_exist[emp_ID-1] = true; // Zero indexed // employee_exist 0->999 == 1->1000
            getchar();
            get_name(employees[emp_index].name);
            while(!valid){
                printf("Enter age of EMPLOYEE [18-100]: ");
                int emp_age;
                scanf("%d",&emp_age);
                if(emp_age>=MIN_EMPLOYEE_AGE && emp_age <=MAX_EMPLOYEE_AGE){
                    employees[emp_index].age = emp_age;
                    valid = true;
                }else {
                    printf("\n Enteredd age is INVALID.\n");
                }
            } valid = false;
            getchar();
            get_department(employees[emp_index].department);
            while(!valid){
                printf("Enter weekly target of EMPLOYEE [0-50]: ");
                int weekly_target_emp;
                scanf("%d",&weekly_target_emp);
                if(weekly_target_emp >= 0 && weekly_target_emp <= MAX_WEEKLY_TARGET){
                    employees[emp_index].weekly_target = weekly_target_emp;
                    valid = true;
                }else {
                    printf("Entered Target is out of range.\n");
                }
            }
            printf("\n EMPLOYEE with ID: %d added SUCCESSFULLY\n",emp_ID);
            changes_made = true;
        }else{
            printf("\n EMPLOYEE limit reached, Cannot add more employee id.\n");
        }
        if(emp_ID != -1){
            printf("\n ADD another EMPLOYEE? [Y/N | y/n] : ");
            char choice;
            scanf(" %c",&choice);
            if(choice != 'Y' && choice != 'y')
                add_emp = false;
        }else{
            add_emp = false;
        }
    }
    sort_employees(0,num_of_employees);
}
// ----- Creates Teams -----
void create_team(){
    int current_team_id = generate_team_id_valid();
    int vacant_index_in_team = vacant_index_team();
    if (vacant_index_in_team != -1 && current_team_id != -1){
        teams[vacant_index_in_team].team_id = current_team_id;
        team_exist[current_team_id - 1] = true;
        getchar();
        get_team_name(teams[vacant_index_in_team].name);
        int count;
        printf("Enter number of Employees to add [1-5]: ");
        scanf("%d", &count);
        if(count < 1 || count > MAX_MEMBERS_PER_TEAM) {
             printf("Invalid member count. Team creation cancelled.\n");
             teams[vacant_index_in_team].team_id = EMPTY_TEAM_ID; 
             team_exist[current_team_id - 1] = false;
        }else {
            teams[vacant_index_in_team].members_count = count;
            for (int team_member = 0; team_member < count; team_member++){
                int member_id;
                printf("Enter Employee ID of %d member: ", team_member + 1);
                scanf("%d", &member_id);
                int emp_index = find_employee_index(member_id);
                if(!employee_exist[member_id - 1]) {
                    printf("Invalid or non-existent Employee ID. Member not added.\n");
                    teams[vacant_index_in_team].members_count--;
                    team_member--;
                }else {
                    if( employees[emp_index].team_id == EMPTY_TEAM_ID ){
                        teams[vacant_index_in_team].members_ids[team_member] = member_id;
                        employees[emp_index].team_id = current_team_id;
                    }else{
                        team_member--;
                        printf(" Employee already part of another team.\n");
                    }
                }
                printf("\n");
            }
            printf("Enter Teams weekly target (hours): ");
            scanf("%d", &teams[vacant_index_in_team].weekly_team_target);
            num_of_teams++;
            printf("\n TEAM with ID %d and TEAM NAME : %s is created and all details are updated SUCCESSFULLY \n",current_team_id,teams[vacant_index_in_team].name);
            changes_made = true;
            sort_teams(0,num_of_teams);
        }
    }
    else{
        printf("Teams limit reached. Cannot create more teams.\n");
    }
}
// ----- Prints Top 5 Employees with Max working HOURS -----
void top_five_employees(){
    int num_employees = 0;
    int employee_indices[MAX_EMPLOYEES];
    for(int i = 0; i < MAX_EMPLOYEES; i++) {
        if (employees[i].id != EMPTY_EMP_ID) {
            employee_indices[num_employees++] = i;
        }
    }
    if (num_employees == 0) {
        printf("No employees to display.\n");
        return;
    }
    sort_employee_indices(employee_indices, 0, num_employees - 1);
    printf("\n----- Top 5 Employees based on total working hours -----\n");
    int count = 0;
    for (int i = 0; i < num_employees && count < 5; i++){
        int emp_index = employee_indices[i]; // Use the sorted index
        Employee emp = employees[emp_index];
        int total_hours = calculate_total_hours(emp);
        
        if (emp.id != EMPTY_EMP_ID) {
            printf("\nRank %d: ID: %-5d | Name: %-64s | Total Hours: %-2d | Target: %-2d\n", 
                   count + 1, emp.id, emp.name, total_hours, emp.weekly_target);
            
            if (total_hours > (3 * emp.weekly_target) / 2){
                printf("  -> Employee ID %d has exceeded %.2f%% of their weekly target, may take a time off. \n", 
                       emp.id , ((double)total_hours / emp.weekly_target - 1) * 100.00);
            }
            count++;
        }
    }
}
// ----- Prints Team Achievement -----
void team_achievement(int team_ID){
    int team_index = find_team_index(team_ID);
    if (team_index == -1) {
        printf("Team ID %d does not exist.\n", team_ID);
        return;
    }
    int total_hours = calculate_total_hours_team(team_ID); // Fixed to use the ID
    printf("\n----- Team Achievement for ID : %d -----\n", team_ID);
    printf("Team Name                : %s\n", teams[team_index].name);
    printf("Weekly Target            : %d\n", teams[team_index].weekly_team_target);
    printf("Total Team Working Hours : %d\n", total_hours);
    if (total_hours >= teams[team_index].weekly_team_target) {
        printf("  STATUS: Target ACHIEVED!\n");
    } else {
        printf("  STATUS: Target NOT ACHIEVED. Still needs %d hours.\n", teams[team_index].weekly_team_target - total_hours);
    }
}
// ----- Ranks All Team -----
void team_ranking_all(){
    int num_teams = 0;
    int team_indices[MAX_TEAMS];

    // 1. Populate the index array with only existing teams
    for(int i = 0; i < MAX_TEAMS; i++) {
        if (teams[i].team_id != EMPTY_TEAM_ID) {
            team_indices[num_teams++] = i;
        }
    }
    if (num_teams == 0) {
        printf("No teams to display.\n");
        return;
    }

    // 2. Sort the index array by total team working hours
    sort_team_indices(team_indices, 0, num_teams - 1);

    printf("\n----- Teams ranked by total working hours -----\n");
    for (int i = 0; i < num_teams; i++){
        int team_index = team_indices[i]; // Use the sorted index
        int team_id = teams[team_index].team_id;
        int total_hours = calculate_total_hours_team(team_id); // Pass the ID
        
        printf("\nRank : %-5d | Team ID: %-5d | Name: %-64s | Total Hours: %-3d\n", i + 1, team_id, teams[team_index].name, total_hours);
    }
}
// ----- Employee Reward of Specified----- 
void employee_reward_specific(int emp_id){ // Display rank of top 3 employees and points of all who achieved the target ehich is based on total hours
    int num_employees = 0;
    int employee_indices[MAX_EMPLOYEES];

    // 1. Populate the index array with only existing employees
    for(int i = 0; i < MAX_EMPLOYEES; i++) {
        if (employees[i].id != EMPTY_EMP_ID) {
            employee_indices[num_employees++] = i;
        }
    }
    if (num_employees == 0) {
        printf("No employees to display.\n"); // flag dalna he lavde
    }
    sort_employee_indices(employee_indices, 0, num_employees - 1); // Sort Employees by total working hours in descending order(locally)
    int empidx=find_employee_index(emp_id);
    for(int i=0;i<num_employees;i++){
        int total_hours = calculate_total_hours(employees[employee_indices[i]]);
        if(empidx==employee_indices[i]){
            if (total_hours >= employees[employee_indices[i]].weekly_target ){
                if(i==0){
                    printf("Employee ID: %d | Rank: %d | Rewards: 100 ",emp_id,i+1);
                }
                else if(i==1){
                    printf("Employee ID: %d | Rank: %d | Rewards: 75 ",emp_id,i+1);
                }
                else if(i==2){
                    printf("Employee ID: %d | Rank: %d | Rewards: 50 ",emp_id,i+1);
                }
                else if(total_hours > employees[employee_indices[i]].weekly_target ){
                    printf("Employee ID: %d | Rewards: 20 ",emp_id);   
                }
                else{
                    printf("Employee ID: %d | Rewards: 10 ",emp_id);
                }
             }
             else{
                printf("Employee ID: %d | Rewards: 0 ",emp_id,i+1);
             }


        }
    }
}
// ----- Update or Delete Employee -----
void update_delete_employee(int emp_id){
    char choice;
    int emp_indices=find_employee_index(emp_id);
    printf("Do you want to Update or Delete the employee details for employee id-%d? [U/D | u/d] : ", emp_id);
    scanf(" %c", &choice);
    if (choice == 'U' || choice == 'u'){ // for update
       if (emp_indices != -1 && emp_id != -1){
            //employees[emp_indices].id = emp_id;
            //employee_exist[emp_id] = true;
            getchar();
            get_name(employees[emp_indices].name);
            printf("Enter age of employee: ");
            scanf("%d", &employees[emp_indices].age);
            getchar();
            get_department(employees[emp_indices].department);
            printf("Enter weekly target of employee: ");
            scanf("%d", &employees[emp_indices].weekly_target);
            getchar();
            printf("\nEmployee with ID %d updated succesfully\n", emp_id);
            changes_made = true;
        }
        else{
            printf("\nError!!! Employee with ID %d updation unsuccesfully\n", emp_id);
        }
    }    
    else if(choice == 'D' || choice == 'd'){ // for delete
         if (emp_indices != -1 && emp_id != -1){
            int employeeTeamID=employees[emp_indices].team_id;
            employees[emp_indices].id = EMPTY_EMP_ID;
            strcpy(employees[emp_indices].name, "");
            employees[emp_indices].age = 0;
            strcpy(employees[emp_indices].department, "");
            for (int d = 0; d < DAYS_IN_WEEK; d++){
                employees[emp_indices].working_hours[d] = 0;
            }
            employees[emp_indices].weekly_target = 0;
            employees[emp_indices].team_id = 101;
            employee_exist[emp_indices] = false;
            if(employeeTeamID!=EMPTY_TEAM_ID){ // for removing the employee form the team
                int teamIndex=find_team_index(employeeTeamID); 
                // teams[teamIndex].members_count--; // decreasing count
                int employee_postion_in_team_array; // for checking the position of the employee in the members-id array
                int m;
                for(m=0;m<MAX_MEMBERS_PER_TEAM;m++){  // to get the postion of the employee in the team member list
                    if(teams[teamIndex].members_ids[m]==emp_id){
                        employee_postion_in_team_array=m;
                    }
                }
                int j = MAX_MEMBERS_PER_TEAM - 1;
                while(teams[teamIndex].members_ids[j] == EMPTY_EMP_ID && j>=0) 
                    j--;
                teams[teamIndex].members_ids[employee_postion_in_team_array] = teams[teamIndex].members_ids[j];
                teams[teamIndex].members_ids[j] = EMPTY_EMP_ID;

            }
            printf("\n  Employee with ID %d deleted succesfully\n", emp_id);
            changes_made = true;
            sort_employees(0,num_of_employees);
            num_of_employees--;
        }
        else{
            printf("\nError!!! Employee with ID %d deletion unsuccesfully\n", emp_id);
        } 
    } else 
        printf("INVALID choice.\n");
}
// ----- Delete Team -----
void delete_team(int team_id){
    int teamIndex=find_team_index(team_id);
    if(teamIndex == -1){
        printf("ERROR: TEAM ID is INVALID.\n");
    }else {
        for (int m = 0; m < teams[teamIndex].members_count; m++){
            int emp_index=find_employee_index(teams[teamIndex].members_ids[m]);
            employees[emp_index].team_id = EMPTY_TEAM_ID; // ie=101
            teams[teamIndex].members_ids[m] = EMPTY_EMP_ID;
        }
        teams[teamIndex].team_id = EMPTY_TEAM_ID;
        strcpy(teams[teamIndex].name, ""); 
        teams[teamIndex].members_count = 0;
        teams[teamIndex].weekly_team_target = 0;
        team_exist[team_id - 1] = false;// team exist false
        changes_made = true;
        sort_teams(0,num_of_teams);
        num_of_teams--;
    }
}
// ----- Merge Team -----
void merge_team(int team_id1, int team_id2){
    int teamIndex1=find_team_index(team_id1);
    int teamIndex2=find_team_index(team_id2);
    int combined[10]; // store all the 10(at most) emp id in this array sorted as per id
    int i,j,k;
    i=j=k=0;
    int c1=teams[teamIndex1].members_count; // count of members in team 1
    int c2=teams[teamIndex2].members_count; // count of members in team 2
    while((i<c1)&&(j<c2)){
        if(teams[teamIndex1].members_ids[i]<teams[teamIndex2].members_ids[j]){
            combined[k++]=teams[teamIndex1].members_ids[i++];
        }
        else{
            combined[k++]=teams[teamIndex2].members_ids[j++];
        }
    }
    if(i<c1){
        while(i<c1){
            combined[k++]=teams[teamIndex1].members_ids[i++];
        }
    }
    else{
        while(j<c2){
            combined[k++]=teams[teamIndex2].members_ids[j++];
        }
    }
    while(k<10){
        combined[k]=-1; // NO MORE EMPLOYEES
        k++;
    }
    int count_merged=c1+c2; // total employees in the combined array 
    delete_team(team_id1);
    delete_team(team_id2);
    //team_id1-->teamIndex1
    teamIndex1 = vacant_index_team();
    teams[teamIndex1].team_id = team_id1;
    team_exist[team_id1-1] = true;
    getchar();
    get_team_name(teams[teamIndex1].name);
    if(count_merged<=MAX_MEMBERS_PER_TEAM){ // for proper employye count in the team
        teams[teamIndex1].members_count = count_merged;
    }
    else{
        teams[teamIndex1].members_count = MAX_MEMBERS_PER_TEAM;
    }    
    k=0; // to use as combined counter again
    for (int team_member = 0; team_member < teams[teamIndex1].members_count; team_member++){
        teams[teamIndex1].members_ids[team_member] = combined[k];
        int emp_index = find_employee_index(combined[k]);
        if (emp_index != -1 && emp_index != EMPTY_EMP_ID ) { // -1 or  ie EMPTY_EMP_ID
            employees[emp_index].team_id =team_id1;
        }           
        k++;
    }
    // if(count_merged>MAX_MEMBERS_PER_TEAM){
    //     for(int m=MAX_MEMBERS_PER_TEAM; m<count_merged;m++){
    //         int emp_index = find_employee_index(combined[m]);
    //         employees[emp_index].team_id =EMPTY_TEAM_ID;        
    //     }
    // }
    printf("Enter Merged Teams weekly target (hours): ");
    scanf("%d", &teams[teamIndex1].weekly_team_target);
        
    printf("\n TEAM %d and TEAM %d are merged and all details are updated SUCCESSFULLY \n",team_id1,team_id2);
    sort_teams(0,num_of_teams);
    num_of_teams--;
}
// ----- Display team info -----
void display_team_info(){
    int num_teams = 0;
    int team_indices[MAX_TEAMS];

    // 1. Populate the index array with only existing teams
    for(int i = 0; i < MAX_TEAMS; i++) {
        if (teams[i].team_id != EMPTY_TEAM_ID) {
            team_indices[num_teams++] = i;
        }
    }
    
    if (num_teams == 0) {
        printf("No teams to display.\n");
        return;
    }

    // 2. Sort the index array by total team working hours
    sort_team_indices(team_indices, 0, num_teams - 1);
    for (int i = 0; i < num_teams; i++){ 
        
        if(teams[team_indices[i]].team_id != EMPTY_TEAM_ID){        
            printf("   Team ID    : %d\n", teams[team_indices[i]].team_id); 
            printf("   Team Name  : %s\n", teams[team_indices[i]].name);
            printf("   Team Rank  : %d\n", i+1); 
            for (int m = 0; m < teams[team_indices[i]].members_count; m++){          
                int emp_indice=find_employee_index(teams[team_indices[i]].members_ids[m]);           
                printf(" %d.  Member Employee ID  : %d\n |   Member Employee Name  : %s\n", m+1,teams[team_indices[i]].members_ids[m], employees[emp_indice].name);           
            }
            printf("   Weekly Team Target (HOURS)  : %d\n", teams[team_indices[i]].weekly_team_target);
            printf("   Current Total (HOURS)  : %d\n", calculate_total_hours_team(teams[team_indices[i]].team_id));
            printf("////////////////////////\n");
            // if(team_ranking_specific(teams[i].team_id)==-1){
            //     printf("No Valid Rank\n");
            // }
            // else{
            //     printf("   Current Rank  : %d\n",team_ranking_specific(teams[i].team_id)); // team rank func need to be changed for specifed team rank
            // }
        }   
    }
}
// ----- Menu -----
void menu(){
    printf("\n--- EMPLOYEE AND TEAM MANAGEMENT SYSTEM MENU ---\n");
    printf("1.  ADD NEW EMPLOYEE\n");
    printf("2.  CREATE TEAM\n");
    printf("3.  DISPLAY TOP 5 EMPLOYEES BASED ON WORKING HOURS\n");
    printf("4.  DISPLAY TEAM ACHIEVEMENT\n");
    printf("5.  DISPLAY TEAM RANKING BASED ON WORKING HOURS\n");
    printf("6.  DISPLAY EMPLOYEE REWARD\n");
    printf("7.  UPDATE OR DELETE EMPLOYEE\n");
    printf("8.  DELETE TEAM\n");
    printf("9.  MERGE TEAMS (TWO)\n");
    printf("10. DISPLAY ALL TEAM INFO\n");
    printf("11. UPDATE EMPLOYEE WORKING HOURS\n");
    printf("12. START WITH NEW EMPLOYEE RECORDS (RESETS ALL DATA)\n");
    printf("13. EXIT\n");
    printf("-----------------------------------------------\n");
    printf("Enter your option[1-13]: ");
}
// ----- Main -----
void main(){
    num_of_employees = load_employee_details_from_file();
    num_of_teams = load_team_details_from_file();
    bool running = true;
    while (running){
        menu();
        int choice;
        // Check for valid integer input
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n'); // flush invalid input
            continue;
        }
        
        switch (choice){
            case 1:{
                add_employees();
                break;
            }
            case 2:{
                create_team();
                break;
            }
            case 3:{
                top_five_employees();
                break;
            }
            case 4:{
                int team_id;
                printf("Enter Team ID to display achievement: ");
                scanf("%d", &team_id);
                team_achievement(team_id);
                break;
            }
            case 5:{
                team_ranking_all(); // change this to team_ranking_all
                break;
            }
            case 6:{
                printf("Enter Employee ID : \n");
                int emp_id;
                scanf("%d",&emp_id);
                int emp_index = find_employee_index(emp_id);
                if(emp_index == -1)
                    printf("Entered ID is INVALID.\n");
                else 
                    employee_reward_specific(emp_id);
                break;
            }
            case 7:{
                printf("Enter Employee ID : \n");
                int emp_id;
                scanf("%d",&emp_id);
                int emp_index = find_employee_index(emp_id);
                if(emp_index == -1)
                    printf("Entered ID is INVALID.\n");
                else 
                    update_delete_employee(emp_id);
                break;
            }
            case 8:{
                printf("Enter Team ID : \n");
                int team_id;
                scanf("%d",&team_id);
                int team_index = find_team_index(team_id);
                if(team_index == -1)
                    printf("Entered ID is INVALID.\n");
                else
                    delete_team(team_id);
                break;
            }
            case 9:{
                printf("Enter first Team ID : \n");
                int team_id1;
                scanf("%d",&team_id1);
                int team_index1 = find_team_index(team_id1);
                if(team_index1 == -1)
                    printf("Entered ID is INVALID.\n");
                printf("Enter second Team ID : \n");
                int team_id2;
                scanf("%d",&team_id2);
                int team_index2 = find_team_index(team_id2);
                if(team_index2 == -1)
                    printf("Entered ID is INVALID.\n");
                if(team_index1 != -1 && team_index2 !=-1){
                    merge_team(team_id1,team_id2);
                }
                break;
            }
            case 10:{
                display_team_info();
                break;
            }
            case 11:{
                printf("Enter Employee ID.\n");
                int emp_id;
                scanf("%d",&emp_id);
                getchar();
                update_working_hours(emp_id);
                break;
            }
            case 12:{
                initialize_employees();
                initialize_teams();
                break;
            }
            case 13:{
                if(changes_made){
                    printf("  ALERT: DO YOU WANT SAVE CHANGES [Y/N | y/n] : ");
                    char ch;
                    getchar();
                    scanf("%c",&ch);
                    if(ch == 'Y' || ch == 'y'){
                        save_employee_details_in_file();
                        save_team_details_in_file();
                    }
                    printf("\n    PROGRAM EXITED SUCCESSFULLY.\n");
                    exit(0);
                }
            }
            default:{
                printf("Invalid choice. Please try again.\n");
                break;
            }
        }
    }
}
void save_employee_details_in_file(){
    FILE *emp_file = fopen("employee.txt", "w");

    if (emp_file == NULL)
    {
        printf("ERROR: Unable to open the file.\n");
    }
    else
    {
        int num_of_employee = vacant_index_employee();
        fprintf(emp_file, "SR.No.| EMP ID| EMPLOYEE NAME |AGE | DEPARTMENT | WORKING HOURS | TARGET | TEAM ID \n");

        for (int i = 0; i < num_of_employee; i++)
        {
            fprintf(emp_file, "%d|%d|%s|%d|%s|", i + 1, employees[i].id, employees[i].name, employees[i].age, employees[i].department);
            int d;
            for (d = 0; d < DAYS_IN_WEEK - 1; d++)
            {
                fprintf(emp_file, "%d,", employees[i].working_hours[d]);
            }
            fprintf(emp_file, "%d|%d|%d\n", employees[i].working_hours[d], employees[i].weekly_target, employees[i].team_id);
        }
    }
    fclose(emp_file);
    if(emp_file!=NULL)
        printf("Employee data saved to employee.txt SUCCESSFULLY.\n");
}
void save_team_details_in_file(){
    FILE *team_file = fopen("team.txt","w");
    if(team_file == NULL)
        printf("ERROR: Unablen to open txt file.\n");
    else{
        int num_of_teams = vacant_index_team();
        fprintf(team_file,"SR.No | TEAM ID | TEAM NAME | TOTAL MEMBERS | TARGET | TEAM MEMBERS ID \n");
        for(int i=0; i<num_of_teams; i++){
            fprintf(team_file,"%d|%d|%s|%d|%d|",i+1,teams[i].team_id,teams[i].name,teams[i].members_count,teams[i].weekly_team_target);
            int m;
            for( m=0; m<MAX_MEMBERS_PER_TEAM-1;m++){
                fprintf(team_file,"%d,",teams[i].members_ids[m]);
            }
            fprintf(team_file,"%d\n",teams[i].members_ids[m]);
        }
    }
    fclose(team_file);
    if(team_file!=NULL)
        printf("Team data saved to team.txt SUCCESSFULLY.\n");
}

int load_employee_details_from_file() {
    FILE *emp_file = fopen("employee.txt", "r");
    if (emp_file == NULL) {
        printf("ERROR: Unable to open employee.txt for reading\n");
        return 0;
    }else{
        initialize_employees();
    }
    char line[512];
    int count = 0;
    int lineptr = 1;
    if (fgets(line, sizeof(line), emp_file) == NULL) {
        fclose(emp_file);
    }

    while (fgets(line, sizeof(line), emp_file) != NULL && count < MAX_EMPLOYEES) {
        int sr_no;
        int matched = sscanf(line, "%d|%d|%[^|]|%d|%[^|]|%d,%d,%d,%d,%d,%d,%d|%d|%d",
            &sr_no,
            &employees[count].id,
            employees[count].name, 
            &employees[count].age,
            employees[count].department,
            &employees[count].working_hours[0],
            &employees[count].working_hours[1],
            &employees[count].working_hours[2],
            &employees[count].working_hours[3],
            &employees[count].working_hours[4],
            &employees[count].working_hours[5],
            &employees[count].working_hours[6],
            &employees[count].weekly_target,
            &employees[count].team_id
        );
        lineptr++;
        int expected_matches =(5 + DAYS_IN_WEEK + 2);
        if (matched == expected_matches) {
            employee_exist[employees[count].id-1] = true;
            count++;
        } else {
            printf("   ERROR AT LINE %d , EMPLOYEE ID :%d\n",lineptr,employees[count].id);
        }
    }
    if(emp_file!=NULL)
        printf("SUCCESSFULLY loaded %d employee records.\n", count);
    fclose(emp_file);
    return count;
}
int load_team_details_from_file() {
    FILE *team_file = fopen("team.txt", "r");
    if (team_file == NULL) {
        printf("   ERROR: Unable to open team.txt for reading\n");
        return 0;
    }else{
        initialize_teams();
    }

    char line[1024]; 
    int count = 0;
    int lineptr = 1;
    if (fgets(line, sizeof(line), team_file) == NULL) {
    }

    while (fgets(line, sizeof(line), team_file) != NULL && count < MAX_TEAMS) {
        
        int sr_no;
        int fixed_member_count = 5;

        int matched = sscanf(line, "%d|%d|%[^|]|%d|%d|%d,%d,%d,%d,%d",
            &sr_no,
            &teams[count].team_id,
            teams[count].name, 
            &teams[count].members_count,
            &teams[count].weekly_team_target,
            &teams[count].members_ids[0],
            &teams[count].members_ids[1],
            &teams[count].members_ids[2],
            &teams[count].members_ids[3],
            &teams[count].members_ids[4]
        );
        lineptr++;
        int expected_matches = 5 + fixed_member_count;
        if (matched == expected_matches) {
            team_exist[teams[count].team_id-1] = true;
            count++;
        } else {
            printf("   ERROR AT LINE %d , TEAM ID :%d\n",lineptr,teams[count].team_id);
        }
    }
    if(team_file!=NULL)
        printf("SUCCESSFULLY loaded %d team records.\n", count);
    fclose(team_file);
    return count;
}