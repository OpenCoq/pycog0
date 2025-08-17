/*
 * opencog/agentzero/CognitiveLoop.h
 *
 * Copyright (C) 2024 OpenCog Foundation
 * SPDX-License-Identifier: AGPL-3.0-or-later
 *
 * Cognitive Loop Implementation
 * Implements perception-action-reflection cycle with AtomSpace integration
 * Part of the AGENT-ZERO-GENESIS project
 */

#ifndef _OPENCOG_AGENTZERO_COGNITIVE_LOOP_H
#define _OPENCOG_AGENTZERO_COGNITIVE_LOOP_H

#include <memory>
#include <thread>
#include <atomic>
#include <chrono>

#include <opencog/atomspace/AtomSpace.h>
#include <opencog/atoms/base/Handle.h>
#include <opencog/util/Logger.h>

namespace opencog {
namespace agentzero {

class AgentZeroCore;

/**
 * CognitiveLoop - Implements the basic perception-action-reflection cycle
 *
 * This class manages the cognitive processing cycle for Agent-Zero,
 * integrating with AtomSpace for state representation and providing
 * a foundation for more complex cognitive behaviors.
 *
 * Cycle phases:
 * 1. Perception - Process sensory input and update world model
 * 2. Planning - Analyze goals and generate action plans
 * 3. Action - Execute selected actions in the environment
 * 4. Reflection - Learn from outcomes and update knowledge
 */
class CognitiveLoop
{
private:
    // Core references
    AgentZeroCore* _agent_core;
    AtomSpacePtr _atomspace;
    
    // Loop control
    std::atomic<bool> _running;
    std::atomic<bool> _paused;
    std::unique_ptr<std::thread> _loop_thread;
    
    // Timing
    std::chrono::milliseconds _cycle_interval;
    std::atomic<long> _cycle_count;
    std::atomic<long> _last_cycle_duration_ms;
    
    // AtomSpace handles for cognitive state
    Handle _perception_context;
    Handle _planning_context;
    Handle _action_context;
    Handle _reflection_context;
    
    // Configuration
    bool _enable_perception;
    bool _enable_planning;
    bool _enable_action;
    bool _enable_reflection;
    
    // Internal methods
    void runMainLoop();
    bool executePerceptionPhase();
    bool executePlanningPhase();
    bool executeActionPhase();
    bool executeReflectionPhase();
    void updateCycleMetrics(const std::chrono::steady_clock::time_point& start_time);
    void handleLoopException(const std::exception& e);

public:
    /**
     * Constructor
     * @param agent_core Pointer to the parent AgentZeroCore instance
     * @param atomspace Shared pointer to the AtomSpace
     */
    CognitiveLoop(AgentZeroCore* agent_core, AtomSpacePtr atomspace);
    
    /**
     * Destructor - ensures loop is stopped and cleaned up
     */
    ~CognitiveLoop();
    
    // Loop control
    /**
     * Start the cognitive loop in a separate thread
     * @return true if successfully started
     */
    bool start();
    
    /**
     * Stop the cognitive loop and wait for thread completion
     * @return true if successfully stopped
     */
    bool stop();
    
    /**
     * Pause the cognitive loop (can be resumed)
     * @return true if successfully paused
     */
    bool pause();
    
    /**
     * Resume a paused cognitive loop
     * @return true if successfully resumed
     */
    bool resume();
    
    /**
     * Execute a single cognitive cycle manually
     * @return true if cycle completed successfully
     */
    bool executeSingleCycle();
    
    // State queries
    /**
     * Check if the loop is currently running
     * @return true if running
     */
    bool isRunning() const { return _running.load(); }
    
    /**
     * Check if the loop is currently paused
     * @return true if paused
     */
    bool isPaused() const { return _paused.load(); }
    
    /**
     * Get the current cycle count
     * @return number of completed cycles
     */
    long getCycleCount() const { return _cycle_count.load(); }
    
    /**
     * Get the duration of the last cycle in milliseconds
     * @return last cycle duration
     */
    long getLastCycleDuration() const { return _last_cycle_duration_ms.load(); }
    
    // Configuration
    /**
     * Set the cycle interval
     * @param interval_ms Interval between cycles in milliseconds
     */
    void setCycleInterval(int interval_ms) { 
        _cycle_interval = std::chrono::milliseconds(interval_ms); 
    }
    
    /**
     * Get the current cycle interval
     * @return cycle interval in milliseconds
     */
    int getCycleInterval() const { 
        return static_cast<int>(_cycle_interval.count()); 
    }
    
    /**
     * Enable or disable specific cognitive phases
     * @param perception Enable perception phase
     * @param planning Enable planning phase
     * @param action Enable action phase
     * @param reflection Enable reflection phase
     */
    void configurePhases(bool perception, bool planning, bool action, bool reflection);
    
    // AtomSpace integration
    /**
     * Get the perception context atom
     * @return Handle to perception context
     */
    Handle getPerceptionContext() const { return _perception_context; }
    
    /**
     * Get the planning context atom
     * @return Handle to planning context
     */
    Handle getPlanningContext() const { return _planning_context; }
    
    /**
     * Get the action context atom
     * @return Handle to action context
     */
    Handle getActionContext() const { return _action_context; }
    
    /**
     * Get the reflection context atom
     * @return Handle to reflection context
     */
    Handle getReflectionContext() const { return _reflection_context; }
    
    /**
     * Get status information for debugging
     * @return JSON string with status details
     */
    std::string getStatusInfo() const;
};

} // namespace agentzero
} // namespace opencog

#endif // _OPENCOG_AGENTZERO_COGNITIVE_LOOP_H