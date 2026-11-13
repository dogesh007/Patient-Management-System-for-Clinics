# Clinic Management & Patient Triage System 

A high-performance C++ CLI-based Patient Management System designed to handle dynamic clinic capacities, real-time triage, intelligent doctor scheduling, and life-saving "Code Blue" emergency preemptions. 

This upgraded version replaces basic linear arrays with advanced data structures (Hash Maps, BSTs, and Stacks) to achieve O(1) and O(log N) time complexities for critical operations.

##  Key Features

*   **Dynamic Clinic Scaling:** Configure the total number of doctors, normal queue capacity, and emergency queue capacity on the fly without recompiling.
*   **"Code Blue" Protocol (Severity 6):** A new emergency tier for life-threatening cases (e.g., heart attacks) that bypasses all capacity limits and forcefully interrupts the doctor treating the least severe patient.
*   **Nested Preemption (LIFO Pausing):** If a doctor is treating a severe patient and gets interrupted by a Code Blue, the previous patient is pushed to a pause stack. Once the Code Blue is resolved, the doctor automatically resumes the paused patient.
*   **Dual Priority Queues:**
    *   *Normal Queue:* Standard First-In, First-Out (FIFO) for Severities 1–3.
    *   *Emergency Queue:* Sorted dynamically by Severity (highest first) and Arrival Time using a Binary Search Tree structure.
*   **O(1) Data Lookups:** Real-time patient tracking and severity updates happen instantly, regardless of the number of registered patients.

##  Data Structures & Algorithms Used

| Structure / Algorithm | Implementation | Time Complexity | Purpose |
| :--- | :--- | :--- | :--- |
| **Hash Map** | `std::unordered_map` | O(1) | Replaces linear arrays for central patient storage. Enables instant lookups, status checks, and updates by Patient ID. |
| **Binary Search Tree** | `std::set` | O(log N) | Acts as an advanced Priority Queue for emergencies. Automatically sorts by severity/arrival time and allows efficient deletion when a waiting patient's severity changes. |
| **Stacks** | `std::vector<std::stack<int>>` | O(1) | Manages nested preemptions via Last-In, First-Out (LIFO) logic for each doctor, ensuring paused patients are resumed in the correct priority order. |
| **Queue** | `std::queue` | O(1) | Manages standard patients on a strict First-Come, First-Served basis. |

##  Triage Severity Scale

| Level | Classification | Queue Assignment | Behavior |
| :--- | :--- | :--- | :--- |
| **1 - 3** | Normal / Minor | Normal Queue | FIFO processing. Subject to normal capacity limits. |
| **4 - 5** | Emergency | Emergency Queue | Prioritized sorting. Severity 5 can preempt normal consultations. |
| **6** | **CODE BLUE** | Emergency Queue | Ignores capacity limits. Instantly preempts the lowest-severity active consultation. |

##  How to Build and Run

### Prerequisites
Any standard C++ compiler supporting standard C++11 or higher (e.g., g++, clang++, or MSVC).

### Compilation
Open your terminal or command prompt and run:
`g++ -std=c++11 main.cpp -o clinic_system`

### Execution
Run the compiled executable:
*   **Linux/macOS:** `./clinic_system`
*   **Windows:** `clinic_system.exe`

##  System Workflow & Navigation

1.  **Register Patient:** Add a new patient, declare their symptoms, and assign a severity (1-6). The system automatically routes them to the correct queue or triggers a Code Blue preemption.
2.  **View Dashboard:** A unified, real-time look at all active doctors, pause stacks, queue capacities, and waiting emergency patients.
3.  **Update Severity:** Re-evaluate a waiting patient. If their condition worsens, they are immediately re-sorted into the correct queue in O(log N) time.
4.  **Finish Consultation:** Complete a checkup or refer the patient. Automatically pops the next paused patient from the doctor's stack, or pulls the highest-priority patient from the waiting queues.
5.  **Search Patient:** Instantly look up a patient's full record, assigned doctor, and exact wait time.
6.  **System Settings:** Dynamically reconfigure the number of active doctors or queue limits during runtime.
7.  **Exit:** Terminate the program session.
