/*
Code for Personal To-Do List Manager Programming Exercise
Name : Maddapati Aditya Vardhan Reddy
Roll Number : CB.EN.U4CSE20335
*/

#include <bits/stdc++.h>

class Task {
public:
    class Builder;

    int getTaskId() const {
        return taskId;
    }

    std::string getDescription() const {
        return description;
    }

    bool isCompleted() const {
        return completed;
    }

    std::string getDueDate() const {
        return dueDate;
    }

private:
    Task(int taskId, std::string description, bool completed, std::string dueDate)
        : taskId(taskId), description(std::move(description)), completed(completed), dueDate(std::move(dueDate)) {}

    int taskId;
    std::string description;
    bool completed;
    std::string dueDate;

    friend class Builder;
};

class Task::Builder {
public:
    Builder(std::string description)
        : taskId(++nextTaskId), description(std::move(description)), completed(false) {}

    Builder& setDueDate(std::string dueDate) {
        this->dueDate = std::move(dueDate);
        return *this;
    }

    Builder& markCompleted() {
        completed = true;
        return *this;
    }

    Task build() {
        return Task(taskId, description, completed, dueDate);
    }

private:
    static int nextTaskId;
    int taskId;
    std::string description;
    bool completed;
    std::string dueDate;
};

int Task::Builder::nextTaskId = 0;

class Memento {
public:
    explicit Memento(const std::vector<Task>& state)
        : state(state) {}

    const std::vector<Task>& getState() const {
        return state;
    }

private:
    std::vector<Task> state;
};

class ToDoList {
public:
    void addTask(const Task& task) {
        tasks.push_back(task);
        saveState();
    }

    void markTaskCompleted(int taskId) {
        auto it = findTask(taskId);
        if (it != tasks.end()) {
            Task::Builder builder = Task::Builder(it->getDescription());
            builder.setDueDate(it->getDueDate()).markCompleted();
            *it = builder.build();
            saveState();
        } else {
            throw std::invalid_argument("Task not found");
        }
    }

    void deleteTask(int taskId) {
        auto it = findTask(taskId);
        if (it != tasks.end()) {
            tasks.erase(it);
            saveState();
        } else {
            throw std::invalid_argument("Task not found");
        }
    }

    void viewTasks(const std::string& filter) const {
        std::cout << "Task List:\n";
        for (const auto& task : getFilteredTasks(filter)) {
            std::cout << "ID: " << task.getTaskId() << ", "
                      << task.getDescription() << " - "
                      << (task.isCompleted() ? "Completed" : "Pending")
                      << (task.getDueDate().empty() ? "" : ", Due: " + task.getDueDate())
                      << std::endl;
        }
    }

    void undo() {
        if (!undoStack.empty()) {
            redoStack.push(saveState());
            tasks = undoStack.top().getState();
            undoStack.pop();
        } else {
            std::cout << "Nothing to undo.\n";
        }
    }

    void redo() {
        if (!redoStack.empty()) {
            undoStack.push(saveState());
            tasks = redoStack.top().getState();
            redoStack.pop();
        } else {
            std::cout << "Nothing to redo.\n";
        }
    }

private:
    std::vector<Task> tasks;
    std::stack<Memento> undoStack;
    std::stack<Memento> redoStack;

    std::vector<Task>::iterator findTask(int taskId) {
        return std::find_if(tasks.begin(), tasks.end(),
                            [taskId](const Task& task) { return task.getTaskId() == taskId; });
    }

    std::vector<Task> getFilteredTasks(const std::string& filter) const {
        if (filter == "Show completed") {
            return getCompletedTasks();
        } else if (filter == "Show pending") {
            return getPendingTasks();
        } else {
            return tasks;
        }
    }

    std::vector<Task> getCompletedTasks() const {
        std::vector<Task> completedTasks;
        for (const auto& task : tasks) {
            if (task.isCompleted()) {
                completedTasks.push_back(task);
            }
        }
        return completedTasks;
    }

    std::vector<Task> getPendingTasks() const {
        std::vector<Task> pendingTasks;
        for (const auto& task : tasks) {
            if (!task.isCompleted()) {
                pendingTasks.push_back(task);
            }
        }
        return pendingTasks;
    }

    Memento saveState() {
        undoStack.push(Memento(tasks));
        redoStack = std::stack<Memento>(); // Clear redo stack
        return Memento(tasks);
    }
};

// Function to display the menu and get user input
char displayMenuAndGetChoice() {
    std::cout << "\n===== ToDo List Manager =====\n"
              << "1. Add Task\n"
              << "2. Mark Task as Completed\n"
              << "3. Delete Task\n"
              << "4. View Tasks\n"
              << "5. Undo\n"
              << "6. Redo\n"
              << "7. Quit\n"
              << "Enter your choice (1-7): ";

    char choice;
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear input buffer
    return choice;
}

int main() {
    ToDoList toDoList;

    char choice;
    do {
        choice = displayMenuAndGetChoice();

        switch (choice) {
            case '1': {
                std::cout << "Enter task description: ";
                std::string description;
                std::getline(std::cin, description);

                std::cout << "Enter due date (optional): ";
                std::string dueDate;
                std::getline(std::cin, dueDate);

                Task task = Task::Builder(description).setDueDate(dueDate).build();
                toDoList.addTask(task);

                std::cout << "Task added successfully! (ID: " << task.getTaskId() << ")\n";
                break;
            }
            case '2': {
                std::cout << "Enter task ID to mark as completed: ";
                int taskId;
                std::cin >> taskId;

                try {
                    toDoList.markTaskCompleted(taskId);
                    std::cout << "Task marked as completed!\n";
                } catch (const std::invalid_argument& e) {
                    std::cerr << "Error: " << e.what() << std::endl;
                }

                break;
            }
            case '3': {
                std::cout << "Enter task ID to delete: ";
                int taskId;
                std::cin >> taskId;

                try {
                    toDoList.deleteTask(taskId);
                    std::cout << "Task deleted successfully!\n";
                } catch (const std::invalid_argument& e) {
                    std::cerr << "Error: " << e.what() << std::endl;
                }

                break;
            }
            case '4': {
                std::cout << "Choose filter option ('Show all', 'Show completed', 'Show pending'): ";
                std::string filter;
                std::getline(std::cin, filter);

                toDoList.viewTasks(filter);
                break;
            }
            case '5':
                toDoList.undo();
                std::cout << "Undo performed.\n";
                break;
            case '6':
                toDoList.redo();
                std::cout << "Redo performed.\n";
                break;
            case '7':
                std::cout << "Exiting ToDo List Manager. Goodbye!\n";
                break;
            default:
                std::cout << "Invalid choice. Please enter a number between 1 and 7.\n";
        }

    } while (choice != '7');

    return 0;
}

