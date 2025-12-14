# BloodHound C++ Mimic - Project Analysis & Team Task Division

## Project Overview
The current project is a C++ application designed to mimic the functionality of BloodHound, a security tool for analyzing Active Directory trust relationships. It currently supports:
- **Data Ingestion**: Parsing BloodHound JSON exports (Users, Computers, Groups, etc.).
- **Graph Storage**: In-memory representation of nodes and edges.
- **Basic Analysis**: Shortest path finding, reachability analysis, and identifying controllers.
- **CLI**: A simple menu-driven command-line interface.

## Goals for Expansion
To make this a complete and impressive project for a group of 5, the scope should be expanded to include:
1.  **Performance Optimization**: Handling large datasets efficiently.
2.  **Advanced Analytics**: More complex graph algorithms (Centrality, Community Detection).
3.  **Enhanced UI**: A better user experience (TUI) or interactive shell.
4.  **Reporting**: Exporting results in a readable format (HTML/Text).
5.  **Robustness**: Error handling, logging, and testing.

---

## Team Roles & Task Division

### Member 1: Team Lead & Core Architecture
**Focus**: System Architecture, Performance, and Integration.
*   **Refactor Graph Storage**: Convert the current `std::string` based graph to use Integer IDs internally. This will significantly reduce memory usage and improve algorithm speed.
    *   *Task*: Create a `NodeMapper` class to map String IDs (SIDs) to `int` and back.
*   **Persistence**: Implement a Save/Load feature.
    *   *Task*: Serialize the `Graph` object to a binary file so large datasets don't need to be re-parsed every run.
*   **Integration**: Manage the `main.cpp` loop and ensure all other members' modules connect correctly.
*   **Code Review**: Review PRs from other members.

### Member 2: Data Engineering (Ingestion & Export)
**Focus**: Parsing, File I/O, and Data Integrity.
*   **Parallel Parsing**: Speed up data loading.
    *   *Task*: Use `std::thread` or `std::async` to parse `users.json`, `computers.json`, etc., concurrently.
*   **ZIP Support**: Direct ingestion from BloodHound ZIP files.
    *   *Task*: Integrate a library (like `miniz` or `libzip`) or system command to extract/read ZIPs on the fly.
*   **Data Sanitization**: Handle edge cases.
    *   *Task*: filtering out incomplete nodes, handling duplicate edges, and logging data quality issues.
*   **Export Functionality**:
    *   *Task*: Allow users to export query results (e.g., "All Domain Admins") to CSV or JSON files.

### Member 3: Graph Algorithms Specialist
**Focus**: Mathematical logic and Security Analytics.
*   **Centrality Measures**:
    *   *Task*: Implement **Betweenness Centrality** to find "Bridge" nodes that connect different parts of the network.
    *   *Task*: Implement **PageRank** to identify the most "privileged" or important users/computers.
*   **Advanced Pathfinding**:
    *   *Task*: Implement "All Simple Paths" (with a hop limit) to find *all* ways to compromise a target, not just the shortest.
    *   *Task*: Implement Cycle Detection (finding circular group memberships).
*   **Pre-computation**:
    *   *Task*: Pre-calculate and cache "High Value" targets (Domain Admins) for instant lookup.

### Member 4: User Interface (CLI/TUI) Expert
**Focus**: User Experience and Interactivity.
*   **Interactive Shell / TUI**:
    *   *Task*: Move away from simple `cin >> choice`. Implement a proper shell with command history (e.g., `search user <name>`, `path <start> <end>`).
    *   *Task*: (Optional) Use a library like `FTXUI` or `ncurses` for a visual terminal interface (menus, progress bars).
*   **Autocomplete**:
    *   *Task*: Implement tab-completion for node names in the search prompt.
*   **Visual Feedback**:
    *   *Task*: Use ANSI color codes to distinguish between Users (Green), Computers (Blue), and Groups (Yellow) in the output.

### Member 5: Reporting & Visualization
**Focus**: Output generation and Testing.
*   **HTML Reporting**:
    *   *Task*: Generate a standalone HTML file containing the analysis results.
    *   *Task*: (Bonus) Embed a simple JS graph visualizer (like `vis.js`) in the HTML to show the attack path interactively.
*   **ASCII Visualization**:
    *   *Task*: Create a function to print attack paths as nice ASCII trees in the terminal (improving the current simple list).
*   **Unit Testing**:
    *   *Task*: Write unit tests (using `assert` or a framework like `Catch2`) for the Graph algorithms to ensure accuracy.
    *   *Task*: Create a small "dummy" dataset for testing purposes.

---

## Technical Roadmap

1.  **Phase 1 (Week 1)**:
    *   **M1**: Sets up the repo and basic integer-based Graph class.
    *   **M2**: Writes the parallel parser.
    *   **M3**: Researches and prototypes Centrality algorithms.
    *   **M4**: Designs the command structure for the new CLI.
    *   **M5**: Creates the dummy dataset and basic test suite.

2.  **Phase 2 (Week 2)**:
    *   **M1**: Merges Parser and Graph. Implements Persistence.
    *   **M3**: Integrates Algorithms into the main Graph class.
    *   **M4**: Builds the Interactive Shell/TUI.
    *   **M2**: Adds Export functionality.
    *   **M5**: Implements ASCII visualization.

3.  **Phase 3 (Week 3)**:
    *   **All**: Integration testing.
    *   **M5**: Finalizes HTML reporting.
    *   **M1**: Performance tuning.
    *   **M4**: Polish UI (colors, help menus).

## Recommended Libraries
*   **JSON**: `nlohmann/json` (Already included).
*   **TUI**: `FTXUI` (Modern, C++17) or `ncurses` (Classic).
*   **Testing**: `Catch2` or `Google Test`.
*   **Zip**: `miniz` (Single header) or `libzip`.
