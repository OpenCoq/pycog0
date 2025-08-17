/*
 * src/TaskManager.cpp
 *
 * Copyright (C) 2024 OpenCog Foundation
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * Task Manager Implementation
 * Manages goal decomposition and execution with AtomSpace integration
 * Part of the AGENT-ZERO-GENESIS project
 */

#include <sstream>
#include <algorithm>

#include <opencog/atoms/atom_types/types.h>
#include <opencog/atoms/base/Node.h>
#include <opencog/atoms/base/Link.h>
#include <opencog/atoms/truthvalue/SimpleTruthValue.h>

#include "opencog/agentzero/TaskManager.h"
#include "opencog/agentzero/AgentZeroCore.h"

using namespace opencog;
using namespace opencog::agentzero;

TaskManager::TaskManager(AgentZeroCore* agent_core, AtomSpacePtr atomspace)
    : _agent_core(agent_core)
    , _atomspace(atomspace)
    , _current_task(Handle::UNDEFINED)
    , _current_goal(Handle::UNDEFINED)
    , _goal_hierarchy_root(Handle::UNDEFINED)
    , _task_context(Handle::UNDEFINED)
    , _goal_context(Handle::UNDEFINED)
    , _execution_context(Handle::UNDEFINED)
    , _max_concurrent_tasks(1)
    , _enable_goal_decomposition(true)
    , _enable_priority_scheduling(true)
{
    logger().info() << "[TaskManager] Constructor: Initializing task management";
    initializeTaskManagement();
}

TaskManager::~TaskManager()
{
    logger().info() << "[TaskManager] Destructor: Cleaning up task management";
}

Handle TaskManager::setGoal(const std::string& goal_description, bool auto_decompose)
{
    logger().info() << "[TaskManager] Setting goal: " << goal_description;
    
    Handle goal_atom = createGoalAtom(goal_description);
    _current_goal = goal_atom;
    
    if (auto_decompose && _enable_goal_decomposition) {
        decomposeGoal(goal_atom);
    }
    
    return goal_atom;
}

Handle TaskManager::addSubgoal(const Handle& parent_goal, const std::string& subgoal_description)
{
    logger().debug() << "[TaskManager] Adding subgoal: " << subgoal_description;
    
    Handle subgoal = createGoalAtom(subgoal_description);
    
    // Create hierarchical link
    HandleSeq subgoal_link;
    subgoal_link.push_back(parent_goal);
    subgoal_link.push_back(subgoal);
    _atomspace->add_link(INHERITANCE_LINK, std::move(subgoal_link));
    
    return subgoal;
}

TruthValuePtr TaskManager::isGoalAchieved(const Handle& goal_atom)
{
    return calculateGoalAchievement(goal_atom);
}

Handle TaskManager::createTask(const std::string& task_description, Priority priority, const Handle& goal_atom)
{
    logger().debug() << "[TaskManager] Creating task: " << task_description;
    
    Handle task_atom = createTaskAtom(task_description, priority);
    
    // Associate with goal if provided
    if (goal_atom != Handle::UNDEFINED) {
        HandleSeq task_goal_link;
        task_goal_link.push_back(task_atom);
        task_goal_link.push_back(goal_atom);
        _atomspace->add_link(EVALUATION_LINK, std::move(task_goal_link));
    }
    
    // Add to task queue and set initial status
    _task_queue.push(task_atom);
    _task_status[task_atom] = TaskStatus::PENDING;
    _task_priorities[task_atom] = priority;
    
    return task_atom;
}

bool TaskManager::addTaskDependency(const Handle& task_atom, const Handle& dependency_atom)
{
    logger().debug() << "[TaskManager] Adding task dependency";
    
    _task_dependencies[task_atom].push_back(dependency_atom);
    
    // Create dependency link in AtomSpace
    HandleSeq dependency_link;
    dependency_link.push_back(task_atom);
    dependency_link.push_back(dependency_atom);
    _atomspace->add_link(SEQUENTIAL_AND_LINK, std::move(dependency_link));
    
    return true;
}

Handle TaskManager::getNextTask()
{
    std::vector<Handle> ready_tasks = getReadyTasks();
    
    if (ready_tasks.empty()) {
        return Handle::UNDEFINED;
    }
    
    if (!_enable_priority_scheduling) {
        return ready_tasks.front();
    }
    
    // Find highest priority task
    Handle best_task = ready_tasks.front();
    Priority best_priority = _task_priorities[best_task];
    
    for (const Handle& task : ready_tasks) {
        Priority task_priority = _task_priorities[task];
        if (static_cast<int>(task_priority) > static_cast<int>(best_priority)) {
            best_task = task;
            best_priority = task_priority;
        }
    }
    
    return best_task;
}

bool TaskManager::completeTask(const Handle& task_atom, bool success)
{
    logger().debug() << "[TaskManager] Completing task: " << task_atom;
    
    TaskStatus new_status = success ? TaskStatus::COMPLETED : TaskStatus::FAILED;
    updateTaskStatus(task_atom, new_status);
    
    if (task_atom == _current_task) {
        _current_task = Handle::UNDEFINED;
    }
    
    return true;
}

bool TaskManager::cancelTask(const Handle& task_atom)
{
    logger().debug() << "[TaskManager] Cancelling task: " << task_atom;
    
    updateTaskStatus(task_atom, TaskStatus::CANCELLED);
    
    if (task_atom == _current_task) {
        _current_task = Handle::UNDEFINED;
    }
    
    return true;
}

TaskManager::TaskStatus TaskManager::getTaskStatus(const Handle& task_atom) const
{
    auto it = _task_status.find(task_atom);
    return (it != _task_status.end()) ? it->second : TaskStatus::PENDING;
}

std::vector<Handle> TaskManager::getTasksByStatus(TaskStatus status) const
{
    std::vector<Handle> result;
    
    for (const auto& pair : _task_status) {
        if (pair.second == status) {
            result.push_back(pair.first);
        }
    }
    
    return result;
}

size_t TaskManager::clearPendingTasks()
{
    logger().info() << "[TaskManager] Clearing pending tasks";
    
    size_t cleared_count = 0;
    while (!_task_queue.empty()) {
        _task_queue.pop();
        cleared_count++;
    }
    
    return cleared_count;
}

std::string TaskManager::getStatusInfo() const
{
    std::ostringstream status;
    status << "{";
    status << "\"pending_tasks\":" << _task_queue.size() << ",";
    status << "\"current_task\":\"" << _current_task << "\",";
    status << "\"current_goal\":\"" << _current_goal << "\",";
    status << "\"max_concurrent_tasks\":" << _max_concurrent_tasks << ",";
    status << "\"goal_decomposition_enabled\":" << (_enable_goal_decomposition ? "true" : "false") << ",";
    status << "\"priority_scheduling_enabled\":" << (_enable_priority_scheduling ? "true" : "false");
    status << "}";
    return status.str();
}

bool TaskManager::processTaskManagement()
{
    logger().debug() << "[TaskManager] Processing task management cycle";
    
    try {
        // Get next task if none currently active
        if (_current_task == Handle::UNDEFINED) {
            _current_task = getNextTask();
            
            if (_current_task != Handle::UNDEFINED) {
                updateTaskStatus(_current_task, TaskStatus::ACTIVE);
                logger().debug() << "[TaskManager] Started task: " << _current_task;
            }
        }
        
        // Process current task (simplified - just mark as completed)
        if (_current_task != Handle::UNDEFINED) {
            // In a real implementation, this would execute the actual task
            completeTask(_current_task, true);
            logger().debug() << "[TaskManager] Completed task: " << _current_task;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        logger().error() << "[TaskManager] Error in task processing: " << e.what();
        return false;
    }
}

// Private implementation methods

void TaskManager::initializeTaskManagement()
{
    logger().debug() << "[TaskManager] Initializing task management structures";
    
    std::string agent_name = _agent_core->getAgentName();
    _task_context = _atomspace->add_node(CONCEPT_NODE, agent_name + "_TaskContext");
    _goal_context = _atomspace->add_node(CONCEPT_NODE, agent_name + "_GoalContext");
    _execution_context = _atomspace->add_node(CONCEPT_NODE, agent_name + "_ExecutionContext");
    _goal_hierarchy_root = _atomspace->add_node(CONCEPT_NODE, agent_name + "_GoalHierarchy");
}

Handle TaskManager::createTaskAtom(const std::string& task_description, Priority priority)
{
    Handle task_atom = _atomspace->add_node(CONCEPT_NODE, "Task_" + task_description);
    
    // Set truth value based on priority
    double priority_strength = static_cast<double>(priority) / 20.0; // Normalize to 0-1
    TruthValuePtr task_tv = SimpleTruthValue::createTV(priority_strength, 0.9);
    task_atom->setTruthValue(task_tv);
    
    return task_atom;
}

Handle TaskManager::createGoalAtom(const std::string& goal_description)
{
    Handle goal_atom = _atomspace->add_node(CONCEPT_NODE, "Goal_" + goal_description);
    
    // Initial goal truth value
    TruthValuePtr goal_tv = SimpleTruthValue::createTV(0.0, 0.9); // Not achieved yet
    goal_atom->setTruthValue(goal_tv);
    
    return goal_atom;
}

bool TaskManager::decomposeGoal(const Handle& goal_atom)
{
    logger().debug() << "[TaskManager] Decomposing goal: " << goal_atom;
    
    // Simplified goal decomposition - create basic subtasks
    // In a real implementation, this would use reasoning to decompose goals
    
    addSubgoal(goal_atom, "Analyze_Goal");
    addSubgoal(goal_atom, "Plan_Actions");
    addSubgoal(goal_atom, "Execute_Plan");
    addSubgoal(goal_atom, "Verify_Results");
    
    return true;
}

std::vector<Handle> TaskManager::getReadyTasks()
{
    std::vector<Handle> ready_tasks;
    
    for (const auto& pair : _task_status) {
        if (pair.second == TaskStatus::PENDING && checkTaskDependencies(pair.first)) {
            ready_tasks.push_back(pair.first);
        }
    }
    
    return ready_tasks;
}

bool TaskManager::checkTaskDependencies(const Handle& task_atom)
{
    auto dep_it = _task_dependencies.find(task_atom);
    if (dep_it == _task_dependencies.end()) {
        return true; // No dependencies
    }
    
    for (const Handle& dependency : dep_it->second) {
        TaskStatus dep_status = getTaskStatus(dependency);
        if (dep_status != TaskStatus::COMPLETED) {
            return false; // Dependency not satisfied
        }
    }
    
    return true; // All dependencies satisfied
}

void TaskManager::updateTaskStatus(const Handle& task_atom, TaskStatus status)
{
    _task_status[task_atom] = status;
    
    // Update truth value based on status
    double strength = 0.0;
    switch (status) {
        case TaskStatus::PENDING: strength = 0.2; break;
        case TaskStatus::ACTIVE: strength = 0.5; break;
        case TaskStatus::COMPLETED: strength = 1.0; break;
        case TaskStatus::FAILED: strength = 0.0; break;
        case TaskStatus::CANCELLED: strength = 0.1; break;
        case TaskStatus::SUSPENDED: strength = 0.3; break;
    }
    
    TruthValuePtr status_tv = SimpleTruthValue::createTV(strength, 0.9);
    task_atom->setTruthValue(status_tv);
}

TruthValuePtr TaskManager::calculateGoalAchievement(const Handle& goal_atom)
{
    // Simplified goal achievement calculation
    // In a real implementation, this would analyze subtasks and conditions
    
    return SimpleTruthValue::createTV(0.5, 0.7); // Partially achieved
}