# CPU Scheduler Core Logic: Team Workflow Protocol

**Project Objective:** Our immediate goal is to achieve 100% completion and mathematical verification of the backend scheduling algorithms. **Do not write any GUI code.** I will handle the Qt GUI integration personally once the core logic is finished and tested.

---

## IMPORTANT!!!!!!

**If you will use LLM or Ai Agent to help you while coding your part, please use this PROMPT!!**
```md
**System Role:** You are an expert C++ Systems Architecture Assistant and Git Workflow Enforcer. Your task is to help me write a specific CPU Scheduling algorithm for a university team project and guide me through the exact version control steps to submit it safely.

**Project Context:**
Our team is building a live CPU Scheduler simulator. The project uses C++17, standard containers, and smart pointers. The system uses the Strategy Design Pattern. A team lead is currently handling the CMake build system and Qt6 GUI separately. 

**Your Constraints (STRICT):**
1. Do NOT write any Qt code, GUI code, or front-end logic. 
2. Do NOT suggest modifications to the base interfaces (`IScheduler.h`, `Process.h`, `ExecutionRecord.h`). These files are locked by the team lead. My code must conform to the existing signatures.
3. Only output C++ code meant for my specific assigned files: `src/core/[MyAlgorithm].cpp` and `include/core/[MyAlgorithm].h`.
4. Use `std::unique_ptr`, `std::vector`, and standard C++ practices. No raw pointers.

**The Interface to Implement:**
I am responsible for inheriting from `IScheduler` and implementing these pure virtual functions:
- `void addProcess(const Process& p)`: Ingest a process into my internal queues.
- `void tick()`: Advance the simulation by 1 unit of time. Update remaining burst times, handle context switching, and log `ExecutionRecord` instances to track the live Gantt chart.
- `void removeProcess(int pid)`: Safely find and `std::erase` a process. If the deleted process is currently executing in the CPU, handle the preemption cleanly without causing a segmentation fault.
- `std::vector<Process> getProcesses()`: Return the current state of all processes.
- `std::vector<ExecutionRecord> getGanttChart()`: Return the timeline records.
- `bool isFinished()`: Return true only when all processes in my queue have completed their bursts.

**The Git Workflow Rules (CRITICAL):**
I am strictly forbidden from committing directly to the `main` branch or pushing modified test files (like `main.cpp`). When we are ready to save work, you must provide the exact terminal commands to:
1. Sync the repo (`git checkout main`, `git pull`).
2. Create a feature branch (`git checkout -b feature/[algorithm-name]`).
3. Stage ONLY my two assigned files (`git add src/core/... include/core/...`).
4. Commit with a clear message.
5. Push upstream so I can open a Pull Request.

**How We Will Work:**
I will tell you which specific algorithm I am assigned to build (e.g., Round Robin, Non-Preemptive SJF). 
- First, you will explain how we will manage the internal state safely during the `tick()` loop.
- Second, you will help me write the `.h` and `.cpp` implementation.
- Third, once the code is complete, you will generate the exact Git terminal commands I need to push my branch safely.

Acknowledge these instructions and ask me which algorithm I am assigned to build.
```
--- 

## 1. The Golden Rules of the Architecture

* **Strict Interface Adherence:** The files `IScheduler.h`, `Process.h`, and `ExecutionRecord.h` are locked. You are strictly forbidden from altering method signatures, adding parameters, or changing data types in these files without a team consensus.
* **Backend Isolation:** You are only responsible for your assigned `src/core/YourAlgorithm.cpp` and `include/core/YourAlgorithm.h` files. Do not modify `MainWindow` or any UI files.
* **Memory Management:** No raw pointers. Use `std::unique_ptr` and standard containers (`std::vector`, `std::queue`) to prevent memory leaks and segmentation faults.

---

## 2. Git Branching Strategy (No Commits to Main)

To prevent merge conflicts and broken builds, we use a strict feature-branch workflow. Follow this exact sequence every time you sit down to code:

### Step 1: Sync Your Local Repository
Always start by making sure you have the latest architecture.
```bash
git checkout main
git pull origin main
```

### Step 2: Create Your Isolated Workspace
Create a branch named after the algorithm you are building.
```bash
git checkout -b feature/your-algorithm-name
```
*(Example: `git checkout -b feature/round-robin`)*

### Step 3: Write and Test Your Code
Write your logic strictly inside your assigned `.cpp` and `.h` files.

### Step 4: Stage and Commit
Only stage the specific files you worked on. **Never commit a modified `main.cpp`.**
```bash
git add src/core/YourAlgorithm.cpp include/core/YourAlgorithm.h
git commit -m "feat: implement tick logic for [Algorithm]"
```

### Step 5: Push Your Branch to GitHub
Push your local code up to the remote repository so GitHub knows it exists.
```bash
git push -u origin feature/your-algorithm-name
```

### Step 6: Open a Pull Request (PR) on GitHub
This is how you officially submit your code to be merged into the project.
1. Go to our repository page on GitHub in your web browser.
2. At the top of the page, GitHub usually detects your recent push and shows a yellow/green banner with a button that says **Compare & pull request**. Click it.
   * *(If you don't see the banner, click the **Pull requests** tab at the top, then click the green **New pull request** button. Set the "base" branch to `main` and the "compare" branch to your `feature/` branch).*
3. **Title:** Give your PR a clear title (e.g., "Add Round Robin implementation").
4. **Description:** Leave a brief comment confirming you tested it locally and listing any edge cases you handled.
5. Click the green **Create pull request** button. 
6. Notify the team lead (Mosaad) that your PR is ready for review. Do not merge it yourself!

---

## 3. Algorithm Implementation Checklist

When implementing your specific scheduling class, you must fulfill all pure virtual functions defined in `IScheduler`:

- [ ] **`addProcess(const Process& p)`:** Safely insert the incoming process into your internal data structure (e.g., a master list or ready queue).
- [ ] **`tick()`:** The core simulation loop. Every time this is called, simulate 1 unit of time passing:
  - Update the remaining burst time of the currently running process.
  - Handle context switching if a process finishes or is preempted.
  - Update the `ExecutionRecord` to draw the Gantt chart live (tick-by-tick).
- [ ] **`removeProcess(int pid)`:** Safely find and `std::erase` the process from your queues. *Critical: If the process is currently executing in the CPU, handle the preemption cleanly so the program does not crash.*
- [ ] **`getProcesses()`:** Return a `std::vector` of all processes in their current, updated state.
- [ ] **`getGanttChart()`:** Return the `std::vector<ExecutionRecord>` tracking the timeline.
- [ ] **`isFinished()`:** Return `true` only when all processes in your queue have completely finished their burst times.

---

## 4. Local Testing Protocol (Pre-GUI)

Since the graphical interface is not complete, you must test your algorithm using the command line inside `src/main.cpp`. 

1. Instantiate your specific algorithm class.
2. Hardcode 3-4 processes covering edge cases (e.g., simultaneous arrivals, long bursts, high priority).
3. Run a simulation loop:
   ```cpp
   while (!scheduler->isFinished()) {
       scheduler->tick();
   }
   ```
4. Print the final Waiting Times, Turnaround Times, and Gantt Chart sequence to the console.
5. **Manually verify the math.** If it is mathematically sound, it is ready to be pushed.

**⚠️ WARNING:** Before you commit your code, run `git status`. Ensure you revert any changes made to `src/main.cpp` so you do not accidentally push your local test harness to the main repository.


