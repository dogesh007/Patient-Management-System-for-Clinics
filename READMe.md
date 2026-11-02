# Clinic Management & Patient Triage System

A C++ CLI-based Patient Management System designed to handle clinic registrations, triage queues, doctor scheduling, and life-saving emergency preemption in real-time.

---

## Key Features

- Patient Registration & Triage: Captures patient details, main medical problems, and nurse-assigned triage severity levels (1 to 5).
- Dual Queue Management:
  - Normal FIFO Queue: Holds patients with minor issues (Severities 1–3) on a First-In, First-Out basis.
  - Emergency Priority Queue: Prioritizes urgent cases (Severities 4–5) based on severity level and arrival order.
- Automated Doctor Scheduling: Automatically assigns available doctors to the highest-priority waiting patient.
- Emergency Preemption: Allows Level-5 critical patients to interrupt a doctor's active consultation, pausing the current patient to prioritize life-threatening cases.
- Severity Updates & Queue Transfers: Re-evaluates patient conditions dynamically and transfers them between queues if their severity changes while waiting.
- Real-time Waiting Time Tracking: Tracks exact patient wait times in minutes using standard C++ time libraries.

---

## Data Structures & Algorithms Used

- Structures (struct Patient): Custom data model to encapsulate patient attributes (ID, name, age, severity, arrival time, doctor assignment, and status).
- Queues (std::queue): Manages the first-come, first-served normal waiting line.
- Dynamic Arrays (std::vector): Used for patient record storage and flexible emergency queue sorting.
- Linear Search: Used to look up patients by ID (findPatient) and identify the highest priority emergency patient (getTopEmergencyIndex).
- State Machine Pattern: Tracks patient progress through distinct status states (NORMAL, EMERGENCY, TREATING, PAUSED, COMPLETED, REFERRED, DEFERRED).

---

## Clinic Status Codes

Code | Status | Description
1 | NORMAL | Waiting in standard FIFO queue
2 | EMERGENCY | Waiting in priority emergency queue
3 | TREATING | Currently in consultation with a doctor
4 | PAUSED | Consultation interrupted by a Level-5 emergency
5 | COMPLETED | Consultation finished successfully
6 | REFERRED | Patient referred to external specialist
7 | DEFERRED | On hold (queue capacity reached)

---

## How to Build and Run

### Prerequisites
Any standard C++ compiler supporting standard C++11 or higher (e.g., g++, clang++, or MSVC).

### Compilation
Open your terminal or command prompt and run:

g++ -std=c++11 main.cpp -o clinic_system

### Execution
Run the compiled executable:

On Linux/macOS:
./clinic_system

On Windows:
clinic_system.exe

---

## System Workflow & Navigation

1. Register Patient: Register a new patient and automatically place them into the appropriate queue based on initial triage.
2. Search Patient: View a patient's complete file, status, assigned doctor, and elapsed waiting time.
3. Display Queues: View real-time capacity and list of patients in both Normal and Emergency queues.
4. Display Doctors: Check the availability status of Doctor 1 and Doctor 2.
5. Update Severity: Adjust a waiting patient's severity level (transfers them to the correct queue automatically).
6. Finish Consultation: Close an active checkup as COMPLETED or REFERRED, and resume any paused patients.
7. Emergency Preemption: Interrupt a doctor to immediately treat an incoming Level-5 critical case.
8. Resume Paused Patient: Manually resume consultation with a previously interrupted patient.
9. Exit: Terminate the program.