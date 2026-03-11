# Employee and Team Management System (C)

A robust, file-based management system written in C to track employee productivity, manage team assignments, and calculate performance metrics. This system uses structured data to handle complex relationships between individual employees and their respective teams.

## 🚀 Key Features

* **Performance Tracking:** Log daily working hours for each employee across a 7-day week.
* **Team Dynamics:** Create teams, assign up to 5 members per team, and calculate collective progress toward weekly targets.
* **Ranking & Rewards:** * Automatically identifies top 5 employees based on total hours.
    * Ranks teams based on aggregate productivity.
    * Specific reward logic for high-performers.
* **Data Persistence:** Save and load all records from `employee.txt` and `team.txt` using custom-built parsing logic.
* **Advanced Operations:** Supports merging two existing teams and deleting records with automated cleanup of member IDs.

---

## 🏗️ Technical Architecture

### Data Structures
The system is built on two primary `typedef struct` components:



1.  **Employee:** Stores personal details, weekly working hours (array), target hours, and a foreign key `team_id`.
2.  **Team:** Manages a collection of employee IDs, member counts, and team-specific targets.

### Algorithm Highlights
* **Sorting:** Implements **QuickSort** to handle employee and team rankings efficiently.
* **Searching:** Uses **Binary Search** for retrieving employee/team indices, ensuring $O(\log n)$ performance on sorted IDs.
* **File I/O:** Custom pipe-delimited (`|`) file format to ensure human-readable data storage.

---

## 🛠️ Functionality Overview

| Option | Description |
| :--- | :--- |
| **Add Employee** | Generates a unique ID and validates age/department constraints. |
| **Create Team** | Groups existing employees and sets collective goals. |
| **Team Achievement** | Compares current aggregate hours against the set target. |
| **Merge Teams** | Combines two teams while maintaining sorted member lists. |
| **Auto-Save** | Prompts to save changes to text files upon exiting. |

---

## 💻 Installation & Execution

### Compilation
Compile the project using any standard C compiler (GCC recommended):

```bash
gcc -o ManagementSystem main.c
